// bael_loggermanager.cpp                                             -*-C++-*-
#include <bael_loggermanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_loggermanager_cpp,"$Id$ $CSID$")

#include <bael_attributecontext.h>
#include <bael_context.h>
#include <bael_defaultobserver.h>             // for testing only
#include <bael_fixedsizerecordbuffer.h>
#include <bael_loggermanagerdefaults.h>
#include <bael_recordattributes.h>            // for testing only
#include <bael_recordobserver.h>            // for testing only
#include <bael_severity.h>
#include <bael_testobserver.h>                // for testing only

#include <bcemt_mutex.h>
#include <bcemt_once.h>
#include <bcemt_readlockguard.h>
#include <bcemt_rwmutex.h>
#include <bcemt_writelockguard.h>

#include <bdema_managedptr.h>

#include <bdetu_datetime.h>
#include <bdetu_systemtime.h>

#include <bdet_datetime.h>
#include <bdet_timeinterval.h>

#include <bdef_function.h>
#include <bdef_bind.h>
#include <bdesu_processutil.h>

#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_new.h>            // placement 'new' syntax
#include <bsl_string.h>
#include <bsl_sstream.h>

//=============================================================================
//                           IMPLEMENTATION NOTES
//-----------------------------------------------------------------------------
//
///Single-Threaded vs. Multi-Threaded Logger Manager
///-------------------------------------------------
// Prior releases of 'bael' were appropriate only for single-threaded
// applications.  Thus, 'bael_LoggerManager::singleton().getLogger()' returns a
// reference to an instance of 'bael_Logger' useful for logging.  The same
// 'bael_Logger' is returned each time 'getLogger()' is called.  In this
// multi-threaded release, the overloaded 'allocateLogger' method returns a
// distinct instance of 'bael_Logger' each time that it is called.  This
// enables each thread to have its own thread-specific logger.  The behavior of
// these methods reflects the historical two-stage release of logger
// functionality.
//
///Category Registry Capacity
///--------------------------
// The category registry capacity is managed by the 'setMaxNumCategories' and
// 'maxNumCategories' methods, and the following data member:
//..
//    unsigned int bael_LoggerManager::d_maxNumCategoriesMinusOne;
//..
// From the client's perspective, valid capacity values are in the range
// '[0 .. INT_MAX]'.  A value of 0 implies that no limit is imposed.  Capacity
// values stored internally in 'd_maxNumCategoriesMinusOne' are in the range
// '[-1 .. INT_MAX - 1]' (i.e., stored in an 'unsigned int' variable).  Note
// that 'setMaxNumCategories' and 'maxNumCategories' compensate by subtracting
// 1 from 'd_maxNumCategoriesMinusOne' and adding 1 to
// 'd_maxNumCategoriesMinusOne', respectively.  This trick allows for more
// efficient capacity testing, e.g.:
//..
//    if (d_maxNumCategoriesMinusOne >= d_categoryManager.length()) {
//        // there is sufficient capacity to add a new category
//        // ...
//    }
//..
// as compared to something like the following if the registry capacity
// were stored in a hypothetical 'int' data member
// 'bael_LoggerManager::d_maxNumCategories' not biased by 1:
//..
//    if (d_maxNumCategories == 0
//     || d_maxNumCategories > d_categoryManager.length()) {
//        // there is sufficient capacity to add a new category
//        // ...
//    }
//..
//-----------------------------------------------------------------------------

namespace BloombergLP {

namespace {

template <typename NewFunc, typename OldFunc>
NewFunc makeFunc(const OldFunc& old) {
    return old ? NewFunc(old) : NewFunc();
}

const char *filterName(
           bsl::string                                     *filteredNameBuffer,
           const char                                      *originalName,
           const bdef_Function<void (*)(bsl::string *, const char *)>&
                                                            nameFilter)
    // If the specified category 'nameFilter' is a non-null functor, apply
    // 'nameFilter' to the specified 'originalName', store the translated
    // result in the specified 'filteredNameBuffer', and return the address
    // of the non-modifiable data of 'filteredNameBuffer'; return
    // 'originalName' otherwise (i.e., if 'nameFilter' is null).
{
    BSLS_ASSERT(filteredNameBuffer);
    BSLS_ASSERT(originalName);

    const char *name;
    if (nameFilter) {
        filteredNameBuffer->clear();
        nameFilter(filteredNameBuffer, originalName);
        filteredNameBuffer->push_back(0);  // append the null 'char'
        name = filteredNameBuffer->c_str();
    } else {
        name = originalName;
    }
    return name;
}

bool isCategoryEnabled(bael_ThresholdAggregate *levels,
                       const bael_Category&     category,
                       int                      severity)
    // Load, into the specified 'levels', the active threshold levels for
    // 'category'.  Return 'true' if the specified 'severity' is more
    // severe (i.e., is numerically less than) at least one of the threshold
    // levels of the specified 'category', and 'false' otherwise.
{
    if (category.relevantRuleMask()) {
        bael_AttributeContext *context = bael_AttributeContext::getContext();
        context->determineThresholdLevels(levels, &category);
        int threshold = bael_ThresholdAggregate::maxLevel(*levels);
        return threshold >= severity;
    }
    *levels = category.thresholdLevels();
    return category.maxLevel() >= severity;
}

const char *const DEFAULT_CATEGORY_NAME = "";
const char *const TRIGGER_BEGIN =
                                 "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_END =    "--- END RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_ALL_BEGIN =
                             "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ALL ---";
const char *const TRIGGER_ALL_END =
                               "--- END RECORD DUMP CAUSED BY TRIGGER ALL ---";

void copyAttributesWithoutMessage(bael_Record                  *record,
                                  const bael_RecordAttributes&  srcAttribute)
    // Load into the specified 'record' all fixed fields except 'message',
    // 'fileName' and 'lineNumber' from the specified 'srcAttribute'.
{

    record->fixedFields().setTimestamp(srcAttribute.timestamp());
    record->fixedFields().setProcessID(srcAttribute.processID());
    record->fixedFields().setThreadID(srcAttribute.threadID());
    record->fixedFields().setCategory(srcAttribute.category());
    record->fixedFields().setSeverity(srcAttribute.severity());

}

}  // close unnamed namespace

                           // -----------------
                           // class bael_Logger
                           // -----------------

// PRIVATE CREATORS
bael_Logger::bael_Logger(
           bael_RecordObserver                             *recordObserver,
           bael_RecordBuffer                               *recordBuffer,
           const bdem_Schema                               *schema,
           const bael_Logger::UserPopulatorCallback&        populator,
           const PublishAllTriggerCallback&                 publishAllCallback,
           int                                              scratchBufferSize,
           bael_LoggerManagerConfiguration::LogOrder        logOrder,
           bael_LoggerManagerConfiguration::TriggerMarkers  triggerMarkers,
           bslma_Allocator                                 *basicAllocator)
: d_recordPool(-1, basicAllocator)
, d_recordObserver_p(recordObserver)
, d_recordBuffer_p(recordBuffer)
, d_userSchema_p(schema)
, d_populator(populator)
, d_publishAll(publishAllCallback)
, d_scratchBufferSize(scratchBufferSize)
, d_logOrder(logOrder)
, d_triggerMarkers(triggerMarkers)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(d_recordObserver_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_userSchema_p);
    BSLS_ASSERT(d_allocator_p);

    // 'snprintf' message buffer
    d_scratchBuffer_p = (char *)d_allocator_p->allocate(d_scratchBufferSize);
}

bael_Logger::~bael_Logger()
{
    BSLS_ASSERT(d_recordObserver_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_userSchema_p);
    BSLS_ASSERT(d_publishAll);
    BSLS_ASSERT(d_scratchBuffer_p);
    BSLS_ASSERT(d_allocator_p);

    d_recordBuffer_p->removeAll();
    d_allocator_p->deallocate(d_scratchBuffer_p);
}

// PRIVATE MANIPULATORS
void bael_Logger::publish(bael_Transmission::Cause cause)
{
    d_recordBuffer_p->beginSequence();
    const int len = d_recordBuffer_p->length();
    bael_Context context(cause, 0, len);

    if (1 == len) {  // for len == 1, order does not matter, so optimize it
        context.setRecordIndexRaw(0);
        bcema_SharedPtr<const bael_Record> handle(d_recordBuffer_p->back());
        d_recordObserver_p->publish(handle, context);
        d_recordBuffer_p->popBack();
    }
    else {
        if (bael_LoggerManagerConfiguration::BAEL_LIFO == d_logOrder) {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                bcema_SharedPtr<const bael_Record> handle(
                                                     d_recordBuffer_p->back());
                d_recordObserver_p->publish(handle, context);
                d_recordBuffer_p->popBack();
            }
        }
        else {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                bcema_SharedPtr<const bael_Record> handle(
                                                     d_recordBuffer_p->back());
                d_recordObserver_p->publish(handle, context);
                d_recordBuffer_p->popFront();
            }
        }
    }
    d_recordBuffer_p->endSequence();
}

void bael_Logger::logMessage(const bael_Category&            category,
                             int                             severity,
                             bael_Record                    *record,
                             const bael_ThresholdAggregate&  levels)
{
    record->fixedFields().setTimestamp(bdetu_SystemTime::nowAsDatetimeGMT());

    record->fixedFields().setCategory(category.categoryName());
    record->fixedFields().setSeverity(severity);

    static int pid = bdesu_ProcessUtil::getProcessId();
    record->fixedFields().setProcessID(pid);

    record->fixedFields().setThreadID(bcemt_ThreadUtil::selfIdAsUint64());

    if (d_populator) {
        d_populator(&record->userFields(), *d_userSchema_p);
    }

    bcema_SharedPtr<bael_Record> handle(record, &d_recordPool, d_allocator_p);

    if (levels.recordLevel() >= severity) {
        d_recordBuffer_p->pushBack(handle);
    }

    if (levels.passLevel() >= severity) {

        // Publish this record.

        d_recordObserver_p->publish(handle,
                              bael_Context(bael_Transmission::BAEL_PASSTHROUGH,
                                           0,                 // recordIndex
                                           1));               // sequenceLength
    }

    typedef bael_LoggerManagerConfiguration Config;

    if (levels.triggerLevel() >= severity) {
        bael_Record *marker = 0;
        bael_Context triggerContext(bael_Transmission::BAEL_TRIGGER, 0, 1);

        // Print markers around the trigger logs if configured.

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            marker = getRecord(record->fixedFields().fileName(),
                               record->fixedFields().lineNumber());
            copyAttributesWithoutMessage(marker, record->fixedFields());
            marker->fixedFields().setMessage(TRIGGER_BEGIN);

            bcema_SharedPtr<const bael_Record> handle(marker,
                                                      &d_recordPool,
                                                      d_allocator_p);
            d_recordObserver_p->publish(handle, triggerContext);
        }

        // Publish all records archived by *this* logger.

        publish(bael_Transmission::BAEL_TRIGGER);

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            marker->fixedFields().setMessage(TRIGGER_END);

            bcema_SharedPtr<const bael_Record> handle(marker,
                                                      &d_recordPool,
                                                      d_allocator_p);
            d_recordObserver_p->publish(handle, triggerContext);
        }
    }

    if (levels.triggerAllLevel() >= severity) {
        bael_Record *marker = 0;
        bcema_SharedPtr<const bael_Record> record_p;
        bael_Context triggerContext(bael_Transmission::BAEL_TRIGGER, 0, 1);

        // Print markers around the trigger logs if configured.

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            marker = getRecord(record->fixedFields().fileName(),
                               record->fixedFields().lineNumber());
            copyAttributesWithoutMessage(marker, record->fixedFields());
            marker->fixedFields().setMessage(TRIGGER_ALL_BEGIN);

            bcema_SharedPtr<const bael_Record> handle(marker,
                                                      &d_recordPool,
                                                      d_allocator_p);
            d_recordObserver_p->publish(handle, triggerContext);
        }

        // Publish all records archived by *all* loggers.

        d_publishAll(bael_Transmission::BAEL_TRIGGER_ALL);

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            marker->fixedFields().setMessage(TRIGGER_ALL_END);

            bcema_SharedPtr<const bael_Record> handle(marker,
                                                      &d_recordPool,
                                                      d_allocator_p);
            d_recordObserver_p->publish(handle, triggerContext);
        }
    }
}

// MANIPULATORS
bael_Record *bael_Logger::getRecord(const char *file, int line)
{
    bael_Record *record = d_recordPool.getObject();
    record->userFields().removeAll();
    record->fixedFields().clearMessage();
    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

void bael_Logger::logMessage(const bael_Category&  category,
                             int                   severity,
                             const char           *fileName,
                             int                   lineNumber,
                             const char           *message)
{
    BSLS_ASSERT(fileName);
    BSLS_ASSERT(message);

    bael_ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        return;                                                       // RETURN
    }

    bael_Record *record = getRecord(fileName, lineNumber);
    record->fixedFields().setMessage(message);

    logMessage(category, severity, record, thresholds);
}

void bael_Logger::logMessage(const bael_Category&  category,
                             int                   severity,
                             bael_Record          *record)
{
    bael_ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        return;                                                       // RETURN
    }

    logMessage(category, severity, record, thresholds);
}

void bael_Logger::publish()
{
    publish(bael_Transmission::BAEL_MANUAL_PUBLISH);
}

void bael_Logger::removeAll()
{
    d_recordBuffer_p->removeAll();
}

char *bael_Logger::obtainMessageBuffer(bcemt_Mutex **mutex, int *bufferSize)
{
    d_scratchBufferMutex.lock();
    *mutex = &d_scratchBufferMutex;
    *bufferSize = d_scratchBufferSize;
    return d_scratchBuffer_p;
}

char *bael_Logger::messageBuffer()
{
    return d_scratchBuffer_p;
}

// ACCESSORS
int bael_Logger::messageBufferSize() const
{
    return d_scratchBufferSize;
}

                           // ------------------------
                           // class bael_LoggerManager
                           // ------------------------

// CLASS MEMBERS
bael_LoggerManager *bael_LoggerManager::s_singleton_p = 0;

// PRIVATE CLASS METHODS
void bael_LoggerManager::initSingletonImpl(
                 bael_RecordObserver                    *recordObserver,
                 const bael_LoggerManagerConfiguration&  configuration,
                 bslma_Allocator                        *basicAllocator)
{
    BSLS_ASSERT(recordObserver);

    static bcemt_Once doOnceObj = BCEMT_ONCE_INITIALIZER;
    bcemt_OnceGuard doOnceGuard(&doOnceObj);
    if (doOnceGuard.enter() && 0 == s_singleton_p) {
        bslma_Allocator *allocator =
                                bslma_Default::globalAllocator(basicAllocator);
        new(*allocator) bael_LoggerManager(recordObserver, configuration, allocator);
    }
    else {
        bael_LoggerManager::singleton().getLogger().
            logMessage(*s_singleton_p->d_defaultCategory_p,
                       bael_Severity::BAEL_WARN,
                       __FILE__,
                       __LINE__,
                       "BAEL logger manager has already been initialized!");
    }
}

// CLASS METHODS
void
bael_LoggerManager::createLoggerManager(
                        bdema_ManagedPtr<bael_LoggerManager>   *manager,
                        bael_RecordObserver                    *recordObserver,
                        const bael_LoggerManagerConfiguration&  configuration,
                        bslma_Allocator                        *basicAllocator)
{
    bslma_Allocator *allocator = bslma_Default::allocator(basicAllocator);

    manager->load(new(*allocator) bael_LoggerManager(configuration,
                                                     recordObserver,
                                                     allocator),
                  allocator);
}

bael_LoggerManager& bael_LoggerManager::initSingleton(
                        bael_RecordObserver                    *recordObserver,
                        const bael_LoggerManagerConfiguration&  configuration,
                        bslma_Allocator                        *basicAllocator)
{
    initSingletonImpl(recordObserver, configuration, basicAllocator);
    return *s_singleton_p;
}

void bael_LoggerManager::shutDownSingleton()
{
    if (s_singleton_p) {
        s_singleton_p->allocator()->deleteObjectRaw(s_singleton_p);
    }
}

void bael_LoggerManager::initSingleton(bael_RecordObserver *recordObserver,
                                       bslma_Allocator     *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                            bael_RecordObserver             *recordObserver,
                            const FactoryDefaultThresholds&  factoryThresholds,
                            bslma_Allocator                 *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                       bael_RecordObserver                  *recordObserver,
                       const DefaultThresholdLevelsCallback& defaultThresholds,
                       bslma_Allocator                      *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                       bael_RecordObserver                  *recordObserver,
                       const DefaultThresholdLevelsCallback& defaultThresholds,
                       const FactoryDefaultThresholds&       factoryThresholds,
                       bslma_Allocator                      *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                     bael_RecordObserver                       *recordObserver,
                     const bdem_Schema&                         userSchema,
                     const bael_Logger::UserPopulatorCallback&  populator,
                     bslma_Allocator                           *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                  bael_RecordObserver                       *recordObserver,
                  const FactoryDefaultThresholds&            factoryThresholds,
                  const bdem_Schema&                         userSchema,
                  const bael_Logger::UserPopulatorCallback&  populator,
                  bslma_Allocator                           *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                   bael_RecordObserver                      *recordObserver,
                   const DefaultThresholdLevelsCallback&     defaultThresholds,
                   const bdem_Schema&                        userSchema,
                   const bael_Logger::UserPopulatorCallback& populator,
                   bslma_Allocator                          *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

void bael_LoggerManager::initSingleton(
                   bael_RecordObserver                      *recordObserver,
                   const DefaultThresholdLevelsCallback&     defaultThresholds,
                   const FactoryDefaultThresholds&           factoryThresholds,
                   const bdem_Schema&                        userSchema,
                   const bael_Logger::UserPopulatorCallback& populator,
                   bslma_Allocator                          *basicAllocator)
{
    bael_LoggerManagerConfiguration configuration;
    bael_LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(recordObserver, configuration, basicAllocator);
}

bael_LoggerManager& bael_LoggerManager::singleton()
{
    return *s_singleton_p;
}

bael_Record *bael_LoggerManager::getRecord(const char *file, int line)
{
    // This static method is called to obtain a record when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record is therefore supplied by the
    // currently installed default allocator.

    bslma_Allocator *allocator = bslma_Default::defaultAllocator();
    bael_Record *record = new(*allocator) bael_Record(allocator);
    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

void bael_LoggerManager::logMessage(int severity, bael_Record *record)
{
    bsl::ostringstream datetimeStream;
    datetimeStream << bdetu_SystemTime::nowAsDatetimeGMT();

    static int pid = bdesu_ProcessUtil::getProcessId();

    bael_Severity::Level severityLevel = (bael_Severity::Level)severity;

    bsl::fprintf(stderr,
                 "\n%s %d %llu %s %s %d %s %s\n",
                 datetimeStream.str().c_str(),
                 pid,
                 bcemt_ThreadUtil::selfIdAsUint64(),
                 bael_Severity::toAscii(severityLevel),
                 record->fixedFields().fileName(),
                 record->fixedFields().lineNumber(),
                 "UNINITIALIZED_LOGGER_MANAGER",
                 record->fixedFields().message());

    // This static method is called to log a message when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record, supplied by the currently
    // installed default allocator, must therefore be reclaimed properly.

    bael_Record::deleteObject(record);
}

char *bael_LoggerManager::obtainMessageBuffer(bcemt_Mutex **mutex,
                                              int          *bufferSize)
{
    const int DEFAULT_LOGGER_BUFFER_SIZE = 8192;
    static char buffer[DEFAULT_LOGGER_BUFFER_SIZE];

    static bsls_ObjectBuffer<bcemt_Mutex> staticMutex;
    BCEMT_ONCE_DO {
        // This mutex must remain valid for the lifetime of the task, and is
        // intentionally never destroyed.  This function may be called on
        // program termination, e.g., if a statically initialized object
        // performs logging during its destruction.

        new (staticMutex.buffer()) bcemt_Mutex();
    }

    staticMutex.object().lock();
    *mutex      = &staticMutex.object();
    *bufferSize = DEFAULT_LOGGER_BUFFER_SIZE;

    return buffer;
}

// PRIVATE CREATORS
bael_LoggerManager::bael_LoggerManager(
                        const bael_LoggerManagerConfiguration&  configuration,
                        bael_RecordObserver                    *recordObserver,
                        bslma_Allocator                        *basicAllocator)
: d_recordObserver_p(recordObserver)
, d_nameFilter(configuration.categoryNameFilterCallback())
, d_defaultThresholds(configuration.defaultThresholdLevelsCallback())
, d_defaultThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_factoryThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_userSchema(configuration.userSchema(),
               bslma_Default::globalAllocator(basicAllocator))
, d_populator(configuration.userPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma_Default::globalAllocator(basicAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma_Default::globalAllocator(basicAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma_Default::globalAllocator(basicAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma_Default::globalAllocator(basicAllocator))
{
    BSLS_ASSERT(recordObserver);

    constructObject(configuration);
}

// PRIVATE MANIPULATORS
void bael_LoggerManager::publishAllImp(bael_Transmission::Cause cause)
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_loggersLock);
    bsl::set<bael_Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->publish(cause);
    }
}

void bael_LoggerManager::constructObject(
                          const bael_LoggerManagerConfiguration& configuration)
{
    BSLS_ASSERT(0 == d_logger_p);
    BSLS_ASSERT(0 == d_defaultCategory_p);
    BSLS_ASSERT(0 == d_recordBuffer_p);

    // TBD: This would make more sense in initSingletonImp and should be moved
    // there when the constructor becomes 'private' again.

    // The attribute context must be initialized with the global allocator,
    // since the lifetime of objects in thread local memory cannot be
    // externally controlled.

    bael_AttributeContext::initialize(&d_categoryManager,
                                      bslma_Default::globalAllocator(0));

    d_publishAllCallback
        = bdef_Function<void (*)(bael_Transmission::Cause)>(
                bdef_MemFnUtil::memFn(&bael_LoggerManager::publishAllImp, this)
              , d_allocator_p);

    int recordBufferSize = configuration.defaults().defaultRecordBufferSize();
    d_recordBuffer_p     = new(*d_allocator_p) bael_FixedSizeRecordBuffer(
                                                              recordBufferSize,
                                                              d_allocator_p);

    d_logger_p = new(*d_allocator_p) bael_Logger(d_recordObserver_p,
                                                 d_recordBuffer_p,
                                                 &d_userSchema,
                                                 d_populator,
                                                 d_publishAllCallback,
                                                 d_scratchBufferSize,
                                                 d_logOrder,
                                                 d_triggerMarkers,
                                                 d_allocator_p);
    d_loggers.insert(d_logger_p);
    d_defaultCategory_p = d_categoryManager.addCategory(
                                   DEFAULT_CATEGORY_NAME,
                                   d_defaultThresholdLevels.recordLevel(),
                                   d_defaultThresholdLevels.passLevel(),
                                   d_defaultThresholdLevels.triggerLevel(),
                                   d_defaultThresholdLevels.triggerAllLevel());
}

// CREATORS
bael_LoggerManager::bael_LoggerManager(
                        bael_RecordObserver                    *recordObserver,
                        const bael_LoggerManagerConfiguration&  configuration,
                        bslma_Allocator                        *basicAllocator)
: d_recordObserver_p(recordObserver)
, d_nameFilter(configuration.categoryNameFilterCallback())
, d_defaultThresholds(configuration.defaultThresholdLevelsCallback())
, d_defaultThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_factoryThresholdLevels(configuration.defaults().defaultRecordLevel(),
                           configuration.defaults().defaultPassLevel(),
                           configuration.defaults().defaultTriggerLevel(),
                           configuration.defaults().defaultTriggerAllLevel())
, d_userSchema(configuration.userSchema(),
               bslma_Default::globalAllocator(basicAllocator))
, d_populator(configuration.userPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma_Default::globalAllocator(basicAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma_Default::globalAllocator(basicAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma_Default::globalAllocator(basicAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma_Default::globalAllocator(basicAllocator))
{
    BSLS_ASSERT(recordObserver);

    constructObject(configuration);

    // TBD: the following check was added back per Andrei's suggestion when
    // the constructor was made temporarily 'public' for BDE release 1.4.0.
    // It should be removed when the constructor becomes 'private' again.
    // (However, it is unclear if we'll ever be able to make the constructor
    // 'private'.)

    if (0 == s_singleton_p) {
        s_singleton_p = this;
    }
}

bael_LoggerManager::~bael_LoggerManager()
{
    BSLS_ASSERT(d_logger_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_defaultCategory_p);
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT( 0 < d_loggers.size());
    BSLS_ASSERT(-1 <= (int)d_maxNumCategoriesMinusOne);

    // Clear the singleton pointer as early as possible to minimize the chance
    // that one thread is destroying the singleton while another is still
    // accessing the data members.  Then immediately reset all category holders
    // to their default value.  (Note that this might not *be* the singleton,
    // so check for that)

    if (this == s_singleton_p) {
        s_singleton_p = 0;
    }

    d_categoryManager.resetCategoryHolders();  // do this first!

    bsl::set<bael_Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->~bael_Logger();
        d_allocator_p->deallocate(*itr);
    }
    d_recordBuffer_p->~bael_RecordBuffer();
    d_allocator_p->deallocate(d_recordBuffer_p);
}

// MANIPULATORS
bael_Logger *bael_LoggerManager::allocateLogger(bael_RecordBuffer *buffer)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_loggersLock);

    bael_Logger *logger = new(*d_allocator_p) bael_Logger(d_recordObserver_p,
                                                          buffer,
                                                          &d_userSchema,
                                                          d_populator,
                                                          d_publishAllCallback,
                                                          d_scratchBufferSize,
                                                          d_logOrder,
                                                          d_triggerMarkers,
                                                          d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

bael_Logger *bael_LoggerManager::allocateLogger(
                                          bael_RecordBuffer *buffer,
                                          int                scratchBufferSize)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_loggersLock);

    bael_Logger *logger = new(*d_allocator_p) bael_Logger(d_recordObserver_p,
                                                          buffer,
                                                          &d_userSchema,
                                                          d_populator,
                                                          d_publishAllCallback,
                                                          scratchBufferSize,
                                                          d_logOrder,
                                                          d_triggerMarkers,
                                                          d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

bael_Logger *bael_LoggerManager::allocateLogger(bael_RecordBuffer   *buffer,
                                                bael_RecordObserver *recordObserver)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_loggersLock);

    bael_Logger *logger = new(*d_allocator_p) bael_Logger(recordObserver,
                                                          buffer,
                                                          &d_userSchema,
                                                          d_populator,
                                                          d_publishAllCallback,
                                                          d_scratchBufferSize,
                                                          d_logOrder,
                                                          d_triggerMarkers,
                                                          d_allocator_p);
    d_loggers.insert(logger);

    return logger;
}

bael_Logger *bael_LoggerManager::allocateLogger(
                                          bael_RecordBuffer   *buffer,
                                          int                  scratchBufferSize,
                                          bael_RecordObserver *recordObserver)
{
    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_loggersLock);

    bael_Logger *logger = new(*d_allocator_p) bael_Logger(recordObserver,
                                                          buffer,
                                                          &d_userSchema,
                                                          d_populator,
                                                          d_publishAllCallback,
                                                          scratchBufferSize,
                                                          d_logOrder,
                                                          d_triggerMarkers,
                                                          d_allocator_p);

    d_loggers.insert(logger);

    return logger;
}

void bael_LoggerManager::deallocateLogger(bael_Logger *logger)
{
    d_loggersLock.lockWrite();
    d_loggers.erase(logger);
    d_loggersLock.unlock();

    d_defaultLoggersLock.lockWrite();

    bsl::map<void *, bael_Logger *>::iterator itr = d_defaultLoggers.begin();
    while (itr != d_defaultLoggers.end()) {
        if (logger == itr->second) {
            d_defaultLoggers.erase(itr++);
        }
        else {
            ++itr;
        }
    }
    d_defaultLoggersLock.unlock();

    logger->~bael_Logger();
    d_allocator_p->deallocate(logger);
}

bael_Logger& bael_LoggerManager::getLogger()
{
    // TBD: optimize it using thread local storage
    d_defaultLoggersLock.lockRead();
    bsl::map<void *, bael_Logger *>::iterator itr =
             d_defaultLoggers.find((void *)bcemt_ThreadUtil::selfIdAsUint64());
    d_defaultLoggersLock.unlock();
    return itr != d_defaultLoggers.end() ? *(itr->second) : *d_logger_p;
}

void bael_LoggerManager::setLogger(bael_Logger *logger)
{
    void *id = (void *)bcemt_ThreadUtil::selfIdAsUint64();

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_defaultLoggersLock);
    if (0 == logger) {
        d_defaultLoggers.erase(id);
    }
    else {
        d_defaultLoggers[id] = logger;
    }
}

bael_Category *bael_LoggerManager::lookupCategory(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

bael_Category *bael_LoggerManager::addCategory(const char *categoryName,
                                               int         recordLevel,
                                               int         passLevel,
                                               int         triggerLevel,
                                               int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    bael_Category *category = d_categoryManager.lookupCategory(
                                                            localCategoryName);
    if (category) {
        return 0;                                                     // RETURN
    }

    if (d_maxNumCategoriesMinusOne <
                                   (unsigned int) d_categoryManager.length()) {
        return 0;                                                     // RETURN
    }

    return d_categoryManager.addCategory(localCategoryName,
                                         recordLevel,
                                         passLevel,
                                         triggerLevel,
                                         triggerAllLevel);
}

const bael_Category *bael_LoggerManager::setCategory(const char *categoryName)
{
    return setCategory(0, categoryName);
}

const bael_Category *bael_LoggerManager::setCategory(
                                           bael_CategoryHolder *categoryHolder,
                                           const char          *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    bael_Category *category = d_categoryManager.lookupCategory(
                                                            categoryHolder,
                                                            localCategoryName);

    if (!category
     && d_maxNumCategoriesMinusOne >=
                                   (unsigned int) d_categoryManager.length()) {
        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        if (d_defaultThresholds) {
            d_defaultThresholds(&recordLevel,
                                &passLevel,
                                &triggerLevel,
                                &triggerAllLevel,
                                localCategoryName);
        }
        else {
            recordLevel     = d_defaultThresholdLevels.recordLevel();
            passLevel       = d_defaultThresholdLevels.passLevel();
            triggerLevel    = d_defaultThresholdLevels.triggerLevel();
            triggerAllLevel = d_defaultThresholdLevels.triggerAllLevel();
        }

        category = d_categoryManager.addCategory(categoryHolder,
                                                 localCategoryName,
                                                 recordLevel,
                                                 passLevel,
                                                 triggerLevel,
                                                 triggerAllLevel);

        if (!category) {  // added by another thread?
            category = d_categoryManager.lookupCategory(categoryHolder,
                                                        localCategoryName);
        }
    }

    return category ? category
                    : d_categoryManager.lookupCategory(categoryHolder,
                                                       DEFAULT_CATEGORY_NAME);
}

bael_Category *bael_LoggerManager::setCategory(const char *categoryName,
                                               int         recordLevel,
                                               int         passLevel,
                                               int         triggerLevel,
                                               int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    bael_Category *category = d_categoryManager.lookupCategory(
                                                            localCategoryName);
    if (category) {
        category->setLevels(recordLevel,
                            passLevel,
                            triggerLevel,
                            triggerAllLevel);
        return category;                                              // RETURN
    }

    if (d_maxNumCategoriesMinusOne <
                                   (unsigned int) d_categoryManager.length()) {
        return 0;                                                     // RETURN
    }

    category = d_categoryManager.addCategory(localCategoryName,
                                             recordLevel,
                                             passLevel,
                                             triggerLevel,
                                             triggerAllLevel);

    if (category) {
        return category;                                              // RETURN
    }
    else {
        return d_categoryManager.lookupCategory(localCategoryName);   // RETURN
    }
}

bael_Category& bael_LoggerManager::defaultCategory()
{
    return *d_defaultCategory_p;
}

int bael_LoggerManager::setDefaultThresholdLevels(int recordLevel,
                                                  int passLevel,
                                                  int triggerLevel,
                                                  int triggerAllLevel)
{
    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    if (!bael_Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return BAEL_FAILURE;                                          // RETURN
    }

    d_defaultThresholdLevels.setLevels(recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel);

    return BAEL_SUCCESS;
}

void bael_LoggerManager::resetDefaultThresholdLevels()
{
    d_defaultThresholdLevels = d_factoryThresholdLevels;
}

void bael_LoggerManager::setCategoryThresholdsToCurrentDefaults(
                                                       bael_Category *category)
{
    category->setLevels(d_defaultThresholdLevels.recordLevel(),
                        d_defaultThresholdLevels.passLevel(),
                        d_defaultThresholdLevels.triggerLevel(),
                        d_defaultThresholdLevels.triggerAllLevel());
}

void bael_LoggerManager::setCategoryThresholdsToFactoryDefaults(
                                                       bael_Category *category)
{
    category->setLevels(d_factoryThresholdLevels.recordLevel(),
                        d_factoryThresholdLevels.passLevel(),
                        d_factoryThresholdLevels.triggerLevel(),
                        d_factoryThresholdLevels.triggerAllLevel());
}

void bael_LoggerManager::setDefaultThresholdLevelsCallback(
                                      DefaultThresholdLevelsCallback *callback)
{
    if (callback) {
        d_defaultThresholds = *callback;
    } else {
        DefaultThresholdLevelsCallback nullCallback;
        d_defaultThresholds = nullCallback;
    }
}

void bael_LoggerManager::setMaxNumCategories(int length)
{
    d_maxNumCategoriesMinusOne = length - 1;
}

bael_RecordObserver *bael_LoggerManager::observer()
{
    return d_recordObserver_p;
}

void bael_LoggerManager::publishAll()
{
    publishAllImp(bael_Transmission::BAEL_MANUAL_PUBLISH_ALL);
}

int bael_LoggerManager::addRule(const bael_Rule& value)
{
    return d_categoryManager.addRule(value);
}

int bael_LoggerManager::addRules(const bael_RuleSet& rules)
{
    return d_categoryManager.addRules(rules);
}

int bael_LoggerManager::removeRule(const bael_Rule& value)
{
    return d_categoryManager.removeRule(value);
}

int bael_LoggerManager::removeRules(const bael_RuleSet& rules)
{
    return d_categoryManager.removeRules(rules);
}

void bael_LoggerManager::removeAllRules()
{
    d_categoryManager.removeAllRules();
}

// ACCESSORS
bslma_Allocator *bael_LoggerManager::allocator() const
{
    return d_allocator_p;
}

const bael_Category *bael_LoggerManager::lookupCategory(
                                                const char *categoryName) const
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

const bael_Category& bael_LoggerManager::defaultCategory() const
{
    return *d_defaultCategory_p;
}

const bael_RecordObserver *bael_LoggerManager::observer() const
{
    return d_recordObserver_p;
}

const bael_Logger::UserPopulatorCallback *
bael_LoggerManager::userPopulatorCallback() const
{
    return d_populator ? &d_populator : 0;
}

int bael_LoggerManager::defaultRecordThresholdLevel() const
{
    return d_defaultThresholdLevels.recordLevel();
}

int bael_LoggerManager::defaultPassThresholdLevel() const
{
    return d_defaultThresholdLevels.passLevel();
}

int bael_LoggerManager::defaultTriggerThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerLevel();
}

int bael_LoggerManager::defaultTriggerAllThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerAllLevel();
}

int bael_LoggerManager::maxNumCategories() const
{
    return (int)d_maxNumCategoriesMinusOne + 1;
}

int bael_LoggerManager::numCategories() const
{
    return d_categoryManager.length();
}

const bael_RuleSet& bael_LoggerManager::ruleSet() const
{
    return d_categoryManager.ruleSet();
}

bool bael_LoggerManager::isCategoryEnabled(const bael_Category *category,
                                           int                  severity) const
{
    if (category->relevantRuleMask()) {
        bael_AttributeContext *context = bael_AttributeContext::getContext();
        bael_ThresholdAggregate levels(0, 0, 0, 0);
        context->determineThresholdLevels(&levels, category);
        int threshold = bael_ThresholdAggregate::maxLevel(levels);
        return threshold >= severity;
    }
    return category->maxLevel() >= severity;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
