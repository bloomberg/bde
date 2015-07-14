// ball_loggermanager.cpp                                             -*-C++-*-
#include <ball_loggermanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_loggermanager_cpp,"$Id$ $CSID$")

#include <ball_attributecontext.h>
#include <ball_context.h>
#include <ball_defaultobserver.h>             // for testing only
#include <ball_fixedsizerecordbuffer.h>
#include <ball_loggermanagerdefaults.h>
#include <ball_recordattributes.h>            // for testing only
#include <ball_severity.h>
#include <ball_testobserver.h>                // for testing only

#include <bdlmtt_mutex.h>
#include <bdlmtt_once.h>
#include <bdlmtt_readlockguard.h>
#include <bdlmtt_qlock.h>
#include <bdlmtt_rwmutex.h>
#include <bdlmtt_writelockguard.h>

#include <bdlf_bind.h>
#include <bdlf_function.h>
#include <bdlsu_processutil.h>
#include <bdlt_datetime.h>
#include <bsls_timeinterval.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_currenttime.h>

#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bsls_assert.h>
#include <bsls_log.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>       // for warning print only
#include <bsl_new.h>            // placement 'new' syntax
#include <bsl_sstream.h>
#include <bsl_string.h>

//=============================================================================
//                           IMPLEMENTATION NOTES
//-----------------------------------------------------------------------------
//
///Single-Threaded vs. Multi-Threaded Logger Manager
///-------------------------------------------------
// Prior releases of 'bael' were appropriate only for single-threaded
// applications.  Thus, 'ball::LoggerManager::singleton().getLogger()' returns a
// reference to an instance of 'ball::Logger' useful for logging.  The same
// 'ball::Logger' is returned each time 'getLogger()' is called.  In this
// multi-threaded release, the overloaded 'allocateLogger' method returns a
// distinct instance of 'ball::Logger' each time that it is called.  This
// enables each thread to have its own thread-specific logger.  The behavior of
// these methods reflects the historical two-stage release of logger
// functionality.
//
///Category Registry Capacity
///--------------------------
// The category registry capacity is managed by the 'setMaxNumCategories' and
// 'maxNumCategories' methods, and the following data member:
//..
//    unsigned int ball::LoggerManager::d_maxNumCategoriesMinusOne;
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
// 'ball::LoggerManager::d_maxNumCategories' not biased by 1:
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
           const bdlf::Function<void (*)(bsl::string *, const char *)>&
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

bool isCategoryEnabled(ball::ThresholdAggregate *levels,
                       const ball::Category&     category,
                       int                      severity)
    // Load, into the specified 'levels', the active threshold levels for
    // 'category'.  Return 'true' if the specified 'severity' is more
    // severe (i.e., is numerically less than) at least one of the threshold
    // levels of the specified 'category', and 'false' otherwise.
{
    if (category.relevantRuleMask()) {
        ball::AttributeContext *context = ball::AttributeContext::getContext();
        context->determineThresholdLevels(levels, &category);
        int threshold = ball::ThresholdAggregate::maxLevel(*levels);
        return threshold >= severity;
    }
    *levels = category.thresholdLevels();
    return category.maxLevel() >= severity;
}


static bdlmtt::QLock s_bslsLogLock = BDLMTT_QLOCK_INITIALIZER;
   // A lock used to protect the configuration of the 'bsl_Log' callback
   // handler.  This lock prevents the logger manager from being destroyed
   // concurrently to an attempt to log a record.

void bslsLogMessage(const char *fileName,
                    int         lineNumber,
                    const char *message)
   // Write a 'bael' record having the specified 'fileName', 'lineNumber', and
   // 'message'.  Note that this function signature matches
   // 'bsls_Log::LogMessageHandler' and is intended to be installed a
   // 'bsls_Log' message handler.
{
    static ball::Category *category               = 0;
    static const char    *BSLS_LOG_CATEGORY_NAME = "BSLS.LOG";

    bdlmtt::QLockGuard qLockGuard(&s_bslsLogLock);

    if (ball::LoggerManager::isInitialized()) {
        ball::Logger&  logger = ball::LoggerManager::singleton().getLogger();
        ball::Record  *record = logger.getRecord(fileName, lineNumber);

        ball::LoggerManager& singleton = ball::LoggerManager::singleton();
        if (0 == category) {
            singleton.addCategory(BSLS_LOG_CATEGORY_NAME,
                                  singleton.defaultRecordThresholdLevel(),
                                  singleton.defaultPassThresholdLevel(),
                                  singleton.defaultTriggerThresholdLevel(),
                                  singleton.defaultTriggerAllThresholdLevel());
            category = singleton.lookupCategory(BSLS_LOG_CATEGORY_NAME);
        }

        ball::RecordAttributes& attributes = record->fixedFields();
        attributes.setMessage(message);

        logger.logMessage(*category, ball::Severity::BAEL_ERROR, record);
    }
    else {
        (bsls::Log::platformDefaultMessageHandler)(fileName,
                                                   lineNumber,
                                                   message);
    }
}

const char *const DEFAULT_CATEGORY_NAME = "";
const char *const TRIGGER_BEGIN =
                                 "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_END =    "--- END RECORD DUMP CAUSED BY TRIGGER ---";
const char *const TRIGGER_ALL_BEGIN =
                             "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ALL ---";
const char *const TRIGGER_ALL_END =
                               "--- END RECORD DUMP CAUSED BY TRIGGER ALL ---";

void copyAttributesWithoutMessage(ball::Record                  *record,
                                  const ball::RecordAttributes&  srcAttribute)
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

namespace ball {
                           // -----------------
                           // class Logger
                           // -----------------

// PRIVATE CREATORS
Logger::Logger(
           Observer                                   *observer,
           RecordBuffer                               *recordBuffer,
           const bdlmxxx::Schema                               *schema,
           const Logger::UserPopulatorCallback&        populator,
           const PublishAllTriggerCallback&                 publishAllCallback,
           int                                              scratchBufferSize,
           LoggerManagerConfiguration::LogOrder        logOrder,
           LoggerManagerConfiguration::TriggerMarkers  triggerMarkers,
           bslma::Allocator                                *basicAllocator)
: d_recordPool(-1, basicAllocator)
, d_observer_p(observer)
, d_recordBuffer_p(recordBuffer)
, d_userSchema_p(schema)
, d_populator(populator)
, d_publishAll(publishAllCallback)
, d_scratchBufferSize(scratchBufferSize)
, d_logOrder(logOrder)
, d_triggerMarkers(triggerMarkers)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(d_observer_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_userSchema_p);
    BSLS_ASSERT(d_allocator_p);

    // 'snprintf' message buffer
    d_scratchBuffer_p = (char *)d_allocator_p->allocate(d_scratchBufferSize);
}

Logger::~Logger()
{
    BSLS_ASSERT(d_observer_p);
    BSLS_ASSERT(d_recordBuffer_p);
    BSLS_ASSERT(d_userSchema_p);
    BSLS_ASSERT(d_publishAll);
    BSLS_ASSERT(d_scratchBuffer_p);
    BSLS_ASSERT(d_allocator_p);

    d_recordBuffer_p->removeAll();
    d_allocator_p->deallocate(d_scratchBuffer_p);
}

// PRIVATE MANIPULATORS
void Logger::publish(Transmission::Cause cause)
{
    d_recordBuffer_p->beginSequence();
    const int len = d_recordBuffer_p->length();
    Context context(cause, 0, len);

    if (1 == len) {  // for len == 1, order does not matter, so optimize it
        context.setRecordIndexRaw(0);
        d_observer_p->publish(d_recordBuffer_p->back(), context);
        d_recordBuffer_p->popBack();
    }
    else {
        if (LoggerManagerConfiguration::BAEL_LIFO == d_logOrder) {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                d_observer_p->publish(d_recordBuffer_p->back(), context);
                d_recordBuffer_p->popBack();
            }
        }
        else {
            for (int i = 0; i < len; ++i) {
                context.setRecordIndexRaw(i);
                d_observer_p->publish(d_recordBuffer_p->front(), context);
                d_recordBuffer_p->popFront();
            }
        }
    }
    d_recordBuffer_p->endSequence();
}

void Logger::logMessage(const Category&            category,
                             int                             severity,
                             Record                    *record,
                             const ThresholdAggregate&  levels)
{
    record->fixedFields().setTimestamp(bdlt::CurrentTime::utc());

    record->fixedFields().setCategory(category.categoryName());
    record->fixedFields().setSeverity(severity);

    static int pid = bdlsu::ProcessUtil::getProcessId();
    record->fixedFields().setProcessID(pid);

    record->fixedFields().setThreadID(bdlmtt::ThreadUtil::selfIdAsUint64());

    if (d_populator) {
        d_populator(&record->userFields(), *d_userSchema_p);
    }

    bsl::shared_ptr<Record> handle(record, &d_recordPool, d_allocator_p);

    if (levels.recordLevel() >= severity) {
        d_recordBuffer_p->pushBack(handle);
    }

    if (levels.passLevel() >= severity) {

        // Publish this record.

        d_observer_p->publish(handle,
                              Context(Transmission::BAEL_PASSTHROUGH,
                                           0,                 // recordIndex
                                           1));               // sequenceLength

    }

    typedef LoggerManagerConfiguration Config;

    if (levels.triggerLevel() >= severity) {

        // Print markers around the trigger logs if configured.

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            Context triggerContext(Transmission::BAEL_TRIGGER, 0, 1);
            Record *marker = getRecord(
                                         record->fixedFields().fileName(),
                                         record->fixedFields().lineNumber());

            bsl::shared_ptr<Record> handle(marker,
                                                &d_recordPool,
                                                d_allocator_p);

            copyAttributesWithoutMessage(handle.get(), record->fixedFields());

            handle->fixedFields().setMessage(TRIGGER_BEGIN);

            d_observer_p->publish(handle, triggerContext);

            // Publish all records archived by *this* logger.

            publish(Transmission::BAEL_TRIGGER);

            handle->fixedFields().setMessage(TRIGGER_END);

            d_observer_p->publish(handle, triggerContext);
        }
        else {

            // Publish all records archived by *this* logger.

            publish(Transmission::BAEL_TRIGGER);

        }
    }

    if (levels.triggerAllLevel() >= severity) {

        // Print markers around the trigger logs if configured.

        if (Config::BAEL_BEGIN_END_MARKERS == d_triggerMarkers) {
            Context triggerContext(Transmission::BAEL_TRIGGER, 0, 1);
            Record *marker = getRecord(
                                         record->fixedFields().fileName(),
                                         record->fixedFields().lineNumber());

            bsl::shared_ptr<Record> handle(marker,
                                                &d_recordPool,
                                                d_allocator_p);

            copyAttributesWithoutMessage(handle.get(), record->fixedFields());

            handle->fixedFields().setMessage(TRIGGER_ALL_BEGIN);

            d_observer_p->publish(handle, triggerContext);

            // Publish all records archived by *all* loggers.

            d_publishAll(Transmission::BAEL_TRIGGER_ALL);

            handle->fixedFields().setMessage(TRIGGER_ALL_END);

            d_observer_p->publish(handle, triggerContext);
        }
        else {

            // Publish all records archived by *all* loggers.

            d_publishAll(Transmission::BAEL_TRIGGER_ALL);

        }
    }
}

// MANIPULATORS
Record *Logger::getRecord(const char *file, int line)
{
    Record *record = d_recordPool.getObject();
    record->userFields().removeAll();
    record->fixedFields().clearMessage();
    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

void Logger::logMessage(const Category&  category,
                             int                   severity,
                             const char           *fileName,
                             int                   lineNumber,
                             const char           *message)
{
    BSLS_ASSERT(fileName);
    BSLS_ASSERT(message);

    ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        return;                                                       // RETURN
    }

    Record *record = getRecord(fileName, lineNumber);
    record->fixedFields().setMessage(message);
    logMessage(category, severity, record, thresholds);
}

void Logger::logMessage(const Category&  category,
                             int                   severity,
                             Record          *record)
{
    ThresholdAggregate thresholds(0, 0, 0, 0);
    if (!isCategoryEnabled(&thresholds, category, severity)) {
        d_recordPool.deleteObject(record);
        return;                                                       // RETURN
    }
    logMessage(category, severity, record, thresholds);
}

void Logger::publish()
{
    publish(Transmission::BAEL_MANUAL_PUBLISH);
}

void Logger::removeAll()
{
    d_recordBuffer_p->removeAll();
}

char *Logger::obtainMessageBuffer(bdlmtt::Mutex **mutex, int *bufferSize)
{
    d_scratchBufferMutex.lock();
    *mutex = &d_scratchBufferMutex;
    *bufferSize = d_scratchBufferSize;
    return d_scratchBuffer_p;
}

char *Logger::messageBuffer()
{
    return d_scratchBuffer_p;
}

// ACCESSORS
int Logger::messageBufferSize() const
{
    return d_scratchBufferSize;
}

int Logger::numRecordsInUse() const
{
    return d_recordPool.numObjects() -
           d_recordPool.numAvailableObjects();
}
}  // close package namespace


                           // ------------------------
                           // class ball::LoggerManager
                           // ------------------------

// CLASS MEMBERS
ball::LoggerManager *ball::LoggerManager::s_singleton_p = 0;

namespace ball {
// PRIVATE CLASS METHODS
void LoggerManager::initSingletonImpl(
                 Observer                          *observer,
                 const LoggerManagerConfiguration&  configuration,
                 bslma::Allocator                       *basicAllocator)
{
    BSLS_ASSERT(observer);

    static bdlmtt::Once doOnceObj = BDLMTT_ONCE_INITIALIZER;
    bdlmtt::OnceGuard doOnceGuard(&doOnceObj);
    if (doOnceGuard.enter() && 0 == s_singleton_p) {
        bslma::Allocator *allocator =
                               bslma::Default::globalAllocator(basicAllocator);
        new(*allocator) LoggerManager(observer, configuration, allocator);

        // Configure 'bsls_log' to publish records using 'bael' via the
        // 'LoggerManager' singleton.

        // TBD: The bsls_log integration is being disabled for BDE 2.23 (see
        // 64382709).
        // bdlmtt::QLockGuard qLockGuard(&s_bslsLogLock);
        // bsls::Log::setLogMessageHandler(&bslsLogMessage);
                                            
    }
    else {
        LoggerManager::singleton().getLogger().
            logMessage(*s_singleton_p->d_defaultCategory_p,
                       Severity::BAEL_WARN,
                       __FILE__,
                       __LINE__,
                       "BAEL logger manager has already been initialized!");
    }
}

// CLASS METHODS
void
LoggerManager::createLoggerManager(
                        bslma::ManagedPtr<LoggerManager>  *manager,
                        Observer                          *observer,
                        const LoggerManagerConfiguration&  configuration,
                        bslma::Allocator                       *basicAllocator)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    manager->load(new(*allocator) LoggerManager(configuration,
                                                     observer,
                                                     allocator),
                  allocator);
}

LoggerManager& LoggerManager::initSingleton(
                        Observer                          *observer,
                        const LoggerManagerConfiguration&  configuration,
                        bslma::Allocator                       *basicAllocator)
{
    initSingletonImpl(observer, configuration, basicAllocator);
    return *s_singleton_p;
}

void LoggerManager::shutDownSingleton()
{
    if (s_singleton_p) {
        {
            // Restore the default 'bsls_log' message handler.  Note that this
            // lock is necessary to ensure that the singleton is not destroyed
            // while a 'bsls_log' record is being published.

            // TBD: The bsls_log integration is being disabled for BDE 2.23
            // (see 64382709).
            // bdlmtt::QLockGuard qLockGuard(&s_bslsLogLock);
            // bsls::Log::setLogMessageHandler(
            //                      &bsls::Log::platformDefaultMessageHandler);
        }
        s_singleton_p->allocator()->deleteObjectRaw(s_singleton_p);
    }
}

void LoggerManager::initSingleton(Observer    *observer,
                                       bslma::Allocator *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                            Observer                   *observer,
                            const FactoryDefaultThresholds&  factoryThresholds,
                            bslma::Allocator                *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                       Observer                        *observer,
                       const DefaultThresholdLevelsCallback& defaultThresholds,
                       bslma::Allocator                     *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                       Observer                        *observer,
                       const DefaultThresholdLevelsCallback& defaultThresholds,
                       const FactoryDefaultThresholds&       factoryThresholds,
                       bslma::Allocator                     *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                     Observer                             *observer,
                     const bdlmxxx::Schema&                         userSchema,
                     const Logger::UserPopulatorCallback&  populator,
                     bslma::Allocator                          *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                  Observer                             *observer,
                  const FactoryDefaultThresholds&            factoryThresholds,
                  const bdlmxxx::Schema&                         userSchema,
                  const Logger::UserPopulatorCallback&  populator,
                  bslma::Allocator                          *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                   Observer                            *observer,
                   const DefaultThresholdLevelsCallback&     defaultThresholds,
                   const bdlmxxx::Schema&                        userSchema,
                   const Logger::UserPopulatorCallback& populator,
                   bslma::Allocator                         *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(observer, configuration, basicAllocator);
}

void LoggerManager::initSingleton(
                   Observer                            *observer,
                   const DefaultThresholdLevelsCallback&     defaultThresholds,
                   const FactoryDefaultThresholds&           factoryThresholds,
                   const bdlmxxx::Schema&                        userSchema,
                   const Logger::UserPopulatorCallback& populator,
                   bslma::Allocator                         *basicAllocator)
{
    LoggerManagerConfiguration configuration;
    LoggerManagerDefaults      defaults;
    defaults.setDefaultThresholdLevelsIfValid(
                                          factoryThresholds.d_recordLevel,
                                          factoryThresholds.d_passLevel,
                                          factoryThresholds.d_triggerLevel,
                                          factoryThresholds.d_triggerAllLevel);
    configuration.setDefaultValues(defaults);
    configuration.setDefaultThresholdLevelsCallback(defaultThresholds);
    configuration.setUserFields(userSchema, populator);
    initSingletonImpl(observer, configuration, basicAllocator);
}

LoggerManager& LoggerManager::singleton()
{
    return *s_singleton_p;
}

Record *LoggerManager::getRecord(const char *file, int line)
{
    // This static method is called to obtain a record when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record is therefore supplied by the
    // currently installed default allocator.

    bslma::Allocator *allocator = bslma::Default::defaultAllocator();
    Record *record = new(*allocator) Record(allocator);
    record->fixedFields().setFileName(file);
    record->fixedFields().setLineNumber(line);
    return record;
}

void LoggerManager::logMessage(int severity, Record *record)
{
    bsl::ostringstream datetimeStream;
    datetimeStream << bdlt::CurrentTime::utc();

    static int pid = bdlsu::ProcessUtil::getProcessId();

    Severity::Level severityLevel = (Severity::Level)severity;

    bsl::fprintf(stderr,
                 "\n%s %d %llu %s %s %d %s ",
                 datetimeStream.str().c_str(),
                 pid,
                 bdlmtt::ThreadUtil::selfIdAsUint64(),
                 Severity::toAscii(severityLevel),
                 record->fixedFields().fileName(),
                 record->fixedFields().lineNumber(),
                 "UNINITIALIZED_LOGGER_MANAGER");

    bslstl::StringRef message = record->fixedFields().messageRef();
    bsl::fwrite(message.data(), 1, message.length(), stderr);

    bsl::fprintf(stderr, "\n");

    // This static method is called to log a message when the logger manager
    // singleton is not available (either has not been initialized or has been
    // destroyed).  The memory for the record, supplied by the currently
    // installed default allocator, must therefore be reclaimed properly.

    Record::deleteObject(record);
}

char *LoggerManager::obtainMessageBuffer(bdlmtt::Mutex **mutex,
                                              int          *bufferSize)
{
    const int DEFAULT_LOGGER_BUFFER_SIZE = 8192;
    static char buffer[DEFAULT_LOGGER_BUFFER_SIZE];

    static bsls::ObjectBuffer<bdlmtt::Mutex> staticMutex;
    BDLMTT_ONCE_DO {
        // This mutex must remain valid for the lifetime of the task, and is
        // intentionally never destroyed.  This function may be called on
        // program termination, e.g., if a statically initialized object
        // performs logging during its destruction.

        new (staticMutex.buffer()) bdlmtt::Mutex();
    }

    staticMutex.object().lock();
    *mutex      = &staticMutex.object();
    *bufferSize = DEFAULT_LOGGER_BUFFER_SIZE;

    return buffer;
}

// PRIVATE CREATORS
LoggerManager::LoggerManager(
                        const LoggerManagerConfiguration&  configuration,
                        Observer                          *observer,
                        bslma::Allocator                       *basicAllocator)
: d_observer_p(observer)
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
               bslma::Default::globalAllocator(basicAllocator))
, d_populator(configuration.userPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma::Default::globalAllocator(basicAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma::Default::globalAllocator(basicAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma::Default::globalAllocator(basicAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    BSLS_ASSERT(observer);

    constructObject(configuration);
}

// PRIVATE MANIPULATORS
void LoggerManager::publishAllImp(Transmission::Cause cause)
{
    bdlmtt::ReadLockGuard<bdlmtt::RWMutex> guard(&d_loggersLock);
    bsl::set<Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->publish(cause);
    }
}

void LoggerManager::constructObject(
                          const LoggerManagerConfiguration& configuration)
{
    BSLS_ASSERT(0 == d_logger_p);
    BSLS_ASSERT(0 == d_defaultCategory_p);
    BSLS_ASSERT(0 == d_recordBuffer_p);

    // TBD: This would make more sense in initSingletonImp and should be moved
    // there when the constructor becomes 'private' again.

    // The attribute context must be initialized with the global allocator,
    // since the lifetime of objects in thread local memory cannot be
    // externally controlled.

    AttributeContext::initialize(&d_categoryManager,
                                      bslma::Default::globalAllocator(0));

    d_publishAllCallback
        = bdlf::Function<void (*)(Transmission::Cause)>(
                bdlf::MemFnUtil::memFn(&LoggerManager::publishAllImp, this)
              , d_allocator_p);

    int recordBufferSize = configuration.defaults().defaultRecordBufferSize();
    d_recordBuffer_p     = new(*d_allocator_p) FixedSizeRecordBuffer(
                                                              recordBufferSize,
                                                              d_allocator_p);

    d_logger_p = new(*d_allocator_p) Logger(d_observer_p,
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
LoggerManager::LoggerManager(
                        Observer                          *observer,
                        const LoggerManagerConfiguration&  configuration,
                        bslma::Allocator                       *basicAllocator)
: d_observer_p(observer)
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
               bslma::Default::globalAllocator(basicAllocator))
, d_populator(configuration.userPopulatorCallback())
, d_logger_p(0)
, d_categoryManager(bslma::Default::globalAllocator(basicAllocator))
, d_maxNumCategoriesMinusOne((unsigned int)-1)
, d_loggers(bslma::Default::globalAllocator(basicAllocator))
, d_recordBuffer_p(0)
, d_defaultCategory_p(0)
, d_scratchBufferSize(configuration.defaults().defaultLoggerBufferSize())
, d_defaultLoggers(bslma::Default::globalAllocator(basicAllocator))
, d_logOrder(configuration.logOrder())
, d_triggerMarkers(configuration.triggerMarkers())
, d_allocator_p(bslma::Default::globalAllocator(basicAllocator))
{
    BSLS_ASSERT(observer);

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

LoggerManager::~LoggerManager()
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

    // TBD: Remove this test once the observer changes in BDE 2.12 have
    // stabilized.
    if (0xdeadbeef == *((unsigned int*)(d_observer_p))){
        bsl::cerr << "ERROR: LoggerManager: "
                  << "Observer is destroyed but still being used."
                  << bsl::endl;
    }
    else {
        d_observer_p->releaseRecords();
    }

    if (this == s_singleton_p) {
        s_singleton_p = 0;
    }

    d_categoryManager.resetCategoryHolders();  // do this first!

    bsl::set<Logger *>::iterator itr;
    for (itr = d_loggers.begin(); itr != d_loggers.end(); ++itr) {
        (*itr)->~Logger();
        d_allocator_p->deallocate(*itr);
    }
    d_recordBuffer_p->~RecordBuffer();
    d_allocator_p->deallocate(d_recordBuffer_p);
}

// MANIPULATORS
Logger *LoggerManager::allocateLogger(RecordBuffer *buffer)
{
    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(d_observer_p,
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

Logger *LoggerManager::allocateLogger(
                                          RecordBuffer *buffer,
                                          int                scratchBufferSize)
{
    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(d_observer_p,
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

Logger *LoggerManager::allocateLogger(RecordBuffer *buffer,
                                                Observer     *observer)
{
    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(observer,
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

Logger *LoggerManager::allocateLogger(
                                          RecordBuffer *buffer,
                                          int                scratchBufferSize,
                                          Observer     *observer)
{
    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_loggersLock);

    Logger *logger = new(*d_allocator_p) Logger(observer,
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

void LoggerManager::deallocateLogger(Logger *logger)
{
    d_loggersLock.lockWrite();
    d_loggers.erase(logger);
    d_loggersLock.unlock();

    d_defaultLoggersLock.lockWrite();

    bsl::map<void *, Logger *>::iterator itr = d_defaultLoggers.begin();
    while (itr != d_defaultLoggers.end()) {
        if (logger == itr->second) {
            d_defaultLoggers.erase(itr++);
        }
        else {
            ++itr;
        }
    }
    d_defaultLoggersLock.unlock();

    logger->~Logger();
    d_allocator_p->deallocate(logger);
}

Logger& LoggerManager::getLogger()
{
    // TBD: optimize it using thread local storage
    d_defaultLoggersLock.lockRead();
    bsl::map<void *, Logger *>::iterator itr =
             d_defaultLoggers.find((void *)bdlmtt::ThreadUtil::selfIdAsUint64());
    d_defaultLoggersLock.unlock();
    return itr != d_defaultLoggers.end() ? *(itr->second) : *d_logger_p;
}

void LoggerManager::setLogger(Logger *logger)
{
    void *id = (void *)bdlmtt::ThreadUtil::selfIdAsUint64();

    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_defaultLoggersLock);
    if (0 == logger) {
        d_defaultLoggers.erase(id);
    }
    else {
        d_defaultLoggers[id] = logger;
    }
}

Category *LoggerManager::lookupCategory(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

Category *LoggerManager::addCategory(const char *categoryName,
                                               int         recordLevel,
                                               int         passLevel,
                                               int         triggerLevel,
                                               int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(
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

const Category *LoggerManager::setCategory(const char *categoryName)
{
    return setCategory(0, categoryName);
}

const Category *LoggerManager::setCategory(
                                           CategoryHolder *categoryHolder,
                                           const char          *categoryName)
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(
                                                            categoryHolder,
                                                            localCategoryName);

    if (!category
     && d_maxNumCategoriesMinusOne >=
                                   (unsigned int) d_categoryManager.length()) {
        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        {
            bdlmtt::ReadLockGuard<bdlmtt::RWMutex> guard(&d_defaultThresholdsLock);
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

Category *LoggerManager::setCategory(const char *categoryName,
                                               int         recordLevel,
                                               int         passLevel,
                                               int         triggerLevel,
                                               int         triggerAllLevel)
{
    BSLS_ASSERT(categoryName);

    if (!Category::areValidThresholdLevels(recordLevel,
                                                passLevel,
                                                triggerLevel,
                                                triggerAllLevel)) {
        return 0;                                                     // RETURN
    }

    bsl::string filteredName;

    const char *localCategoryName = filterName(&filteredName,
                                               categoryName,
                                               d_nameFilter);

    Category *category = d_categoryManager.lookupCategory(
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

Category& LoggerManager::defaultCategory()
{
    return *d_defaultCategory_p;
}

int LoggerManager::setDefaultThresholdLevels(int recordLevel,
                                                  int passLevel,
                                                  int triggerLevel,
                                                  int triggerAllLevel)
{
    enum { BAEL_SUCCESS = 0, BAEL_FAILURE = -1 };

    if (!Category::areValidThresholdLevels(recordLevel,
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

void LoggerManager::resetDefaultThresholdLevels()
{
    d_defaultThresholdLevels = d_factoryThresholdLevels;
}

void LoggerManager::setCategoryThresholdsToCurrentDefaults(
                                                       Category *category)
{
    category->setLevels(d_defaultThresholdLevels.recordLevel(),
                        d_defaultThresholdLevels.passLevel(),
                        d_defaultThresholdLevels.triggerLevel(),
                        d_defaultThresholdLevels.triggerAllLevel());
}

void LoggerManager::setCategoryThresholdsToFactoryDefaults(
                                                       Category *category)
{
    category->setLevels(d_factoryThresholdLevels.recordLevel(),
                        d_factoryThresholdLevels.passLevel(),
                        d_factoryThresholdLevels.triggerLevel(),
                        d_factoryThresholdLevels.triggerAllLevel());
}

void LoggerManager::setDefaultThresholdLevelsCallback(
                                      DefaultThresholdLevelsCallback *callback)
{
    bdlmtt::WriteLockGuard<bdlmtt::RWMutex> guard(&d_defaultThresholdsLock);
    if (callback) {
        d_defaultThresholds = *callback;
    } else {
        DefaultThresholdLevelsCallback nullCallback;
        d_defaultThresholds = nullCallback;
    }
}

void LoggerManager::setMaxNumCategories(int length)
{
    d_maxNumCategoriesMinusOne = length - 1;
}

Observer *LoggerManager::observer()
{
    return d_observer_p;
}

void LoggerManager::publishAll()
{
    publishAllImp(Transmission::BAEL_MANUAL_PUBLISH_ALL);
}

int LoggerManager::addRule(const Rule& value)
{
    return d_categoryManager.addRule(value);
}

int LoggerManager::addRules(const RuleSet& rules)
{
    return d_categoryManager.addRules(rules);
}

int LoggerManager::removeRule(const Rule& value)
{
    return d_categoryManager.removeRule(value);
}

int LoggerManager::removeRules(const RuleSet& rules)
{
    return d_categoryManager.removeRules(rules);
}

void LoggerManager::removeAllRules()
{
    d_categoryManager.removeAllRules();
}

// ACCESSORS
bslma::Allocator *LoggerManager::allocator() const
{
    return d_allocator_p;
}

const Category *LoggerManager::lookupCategory(
                                                const char *categoryName) const
{
    BSLS_ASSERT(categoryName);

    bsl::string filteredName;

    return d_categoryManager.lookupCategory(filterName(&filteredName,
                                                       categoryName,
                                                       d_nameFilter));
}

const Category& LoggerManager::defaultCategory() const
{
    return *d_defaultCategory_p;
}

const Observer *LoggerManager::observer() const
{
    return d_observer_p;
}

const Logger::UserPopulatorCallback *
LoggerManager::userPopulatorCallback() const
{
    return d_populator ? &d_populator : 0;
}

int LoggerManager::defaultRecordThresholdLevel() const
{
    return d_defaultThresholdLevels.recordLevel();
}

int LoggerManager::defaultPassThresholdLevel() const
{
    return d_defaultThresholdLevels.passLevel();
}

int LoggerManager::defaultTriggerThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerLevel();
}

int LoggerManager::defaultTriggerAllThresholdLevel() const
{
    return d_defaultThresholdLevels.triggerAllLevel();
}

int LoggerManager::maxNumCategories() const
{
    return (int)d_maxNumCategoriesMinusOne + 1;
}

int LoggerManager::numCategories() const
{
    return d_categoryManager.length();
}

const RuleSet& LoggerManager::ruleSet() const
{
    return d_categoryManager.ruleSet();
}

bool LoggerManager::isCategoryEnabled(const Category *category,
                                           int                  severity) const
{
    if (category->relevantRuleMask()) {
        AttributeContext *context = AttributeContext::getContext();
        ThresholdAggregate levels(0, 0, 0, 0);
        context->determineThresholdLevels(&levels, category);
        int threshold = ThresholdAggregate::maxLevel(levels);
        return threshold >= severity;
    }
    return category->maxLevel() >= severity;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
