// ball_loggermanager.t.cpp                                           -*-C++-*-
#include <ball_loggermanager.h>

#include <ball_attributecontext.h>
#include <ball_fixedsizerecordbuffer.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_loggermanagerdefaults.h>
#include <ball_rule.h>
#include <ball_userfieldtype.h>
#include <ball_userfields.h>
#include <ball_scopedattribute.h>
#include <ball_severity.h>
#include <ball_streamobserver.h>
#include <ball_testobserver.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdls_pathutil.h>
#include <bdls_tempdirectoryguard.h>

#include <bdlt_currenttime.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_managedptr.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_condition.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_threadutil.h>

#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bslstl_stringref.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_iostream.h>
#include <bsl_functional.h>
#include <bsl_fstream.h>
#include <bsl_map.h>
#include <bsl_memory.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_string.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#include <bsl_c_ctype.h>     // tolower()
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>    // rand_r()

#ifdef BSLS_PLATFORM_OS_UNIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

// Note: on Windows -> WinGDI.h:#define ERROR 0
#ifdef BSLS_PLATFORM_OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#undef ERROR
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bdlf::PlaceHolders;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a logger manager that form the base of the BDE
// logging subsystem.  We must ensure that the component correctly handles
// various configuration parameters supplied at construction or via
// manipulators.  We also test that internal broadcast observer implemented in
// this version of the logger manager correctly forwards published log messages
// to all registered observers.  Finally, we test the logger manager scoped
// guard that is used so simplify the initialization and destruction of the
// logger manager singleton for client applications.
//
// ----------------------------------------------------------------------------
// 'ball::Logger' private interface (tested indirectly):
// [16] ball::Logger(Obs*, *buffer, Sch*, Pop&, Pac&, *ba);
// [16] ~ball::Logger();
// [16] bool logMessage(const Category& , int , ball::Record *, levels);
// [17] void publish(ball::Transmission::Cause cause);
//
// 'ball::Logger' public interface:
// [17] void logMessage(category, severity, file, line, msg);
// [16] char *messageBuffer();
// [16] void publish();
// [16] void removeAll();
// [16] int messageBufferSize() const;
// [35] int numRecordsInUse() const;
//
// 'ball::LoggerManager' private interface (tested indirectly):
// [16] void publishAllImp(ball::Transmission::Cause cause);
//
// 'ball::LoggerManager' public interface:
// [ 3] createLoggerManager(manager, configuration, basicAlloc = 0);
// [ *] static ball::Record *getRecord(const char *file, int line);
// [ 8] LoggerManager& initSingleton(bslma::Allocator *globalAlloc = 0);
// [ 3] static LoggerManager& initSingleton(const Configuration&, *alloc=0);
// [ 4] static LoggerManager& initSingleton(observer, *alloc = 0);
// [ 5] static LoggerManager& initSingleton(observer, config, *alloc = 0);
// [34] static int initSingleton(LoggerManager *, false);
// [33] static int initSingleton(LoggerManager *, true);
// [ *] static bool isInitialized();
// [ *] static void logMessage(int severity, ball::Record *record);
// [ *] static ball::LoggerManager& singleton();
// [ 2] LoggerManager(Observer*, configuration, *ba = 0);
// [ 2] LoggerManager(configuration, *globalAlloc = 0);
// [ 2] ~LoggerManager();
// [18] ball::Logger *allocateLogger(RecordBuffer *buffer)
// [18] ball::Logger *allocateLogger(RecordBuffer *buffer, int msgBufSize);
// [18] ball::Logger *allocateLogger(*buffer, *observer);
// [18] ball::Logger *allocateLogger(*buffer, int msgBufSize, *observer);
// [18] void deallocateLogger(ball::Logger *logger);
// [ *] ball::Logger& getLogger();
// [18] void setLogger(ball::Logger *logger);
// [13] Category *addCategory(const char *name, int, int, int, int);
// [13] Category& defaultCategory();
// [13] Category *lookupCategory(const char *name);
// [13] const Category *setCategory(const char *name);
// [13] Category *setCategory(const char *name, int, int, int, int);
// [13] void setMaxNumCategories(int);
// [10] deregisterAllObservers();
// [10] deregisterObserver(const bsl::string_view&);
// [10] findObserver(const bsl::string_view&);
// [10] findObserver(const shared_ptr<OBSERVER>*, const string_view&);
// [ *] const Obs *observer() const;
// [10] registerObserver(const shared_ptr<Observer>&, const string_view&);
// [10] void visitObservers(OBSERVER_VISITOR visitor);
// [10] void visitObservers(OBSERVER_VISITOR visitor) const;
// [ 9] const Aggregate& getDefaultThresholdLevels() const;
// [ 9] Aggregate getNewCategoryThresholdLevels(const char *) const;
// [13] int setDefaultThresholdLevels(int, int, int, int);
// [13] int resetDefaultThresholdLevels();
// [13] void setCategoryThresholdsToCurrentDefaults(Cat *cat);
// [13] void setCategoryThresholdsToFactoryDefaults(Cat *cat);
// [16] void publishAll();
// [ *] const Cat& defaultCategory() const;
// [13] int defaultPassThresholdLevel() const;
// [13] int defaultRecordThresholdLevel() const;
// [13] int defaultTriggerAllThresholdLevel() const;
// [13] int defaultTriggerThresholdLevel() const;
// [29] bool isCategoryEnabled(const ball::Category *, int) const;
// [ *] const Cat *lookupCategory(const char *name) const;
// [13] int maxNumCategories() const;
// [13] int numCategories() const;
// [NA] const Pop *userFieldsPopulatorCallback() const;
//
// 'ball::LoggerManagerScopedGuard' public interface:
// [27] ball::LoggerManagerScopedGuard(Obs*, const Configuration&, ba = 0);
// [27] ~ball::LoggerManagerScopedGuard();
//
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// 'ball::LoggerManagerCategoryIter' public interface:
// [42] LoggerManagerCategoryIter(const LoggerManager& lm);
// [42] ~LoggerManagerCategoryIter();
// [42] void operator++();
// [42] operator const void *() const;
// [42] const Cat& operator()() const;
//
// 'ball::LoggerManagerCategoryManip' public interface:
// [43] LoggerManagerCategoryManip(LoggerManager *lm);
// [43] ~LoggerManagerCategoryManip();
// [43] void advance();
// [43] Cat& operator()();
// [43] operator const void *() const;
#endif // BDE_OMIT_INTERNAL_DEPRECATED
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] TESTING LOGMESSAGE LOG ORDER and TRIGGER MARKERS
// [20] TESTING CONCURRENT ACCESS TO 'd_loggers'
// [21] TESTING CONCURRENT ACCESS TO 'd_defaultLoggers'
// [22] TESTING CONCURRENT ACCESS TO 'setCategory'
// [23] TESTING CONCURRENT ACCESS TO 'initSingleton'
// [24] TESTING CONCURRENT ACCESS TO 'lookupCategory'
// [25] TBD
// [26] TBD
// [28] LOW-LEVEL LOGGING
// [29] TESTING 'isCategoryEnabled' (RULE BASED LOGGING)
// [30] TESTING 'ball::Logger::logMessage' (RULE BASED LOGGING)
// [31] TESTING '~LoggerManager' calls 'Observer::releaseRecords'
// [36] SINGLETON REINITIALIZATION
// [38] USAGE EXAMPLE #1
// [39] USAGE EXAMPLE #2
// [40] USAGE EXAMPLE #3
// [41] USAGE EXAMPLE #4
// [37] CONCERN: RECORD POOL MEMORY CONSUMPTION
// [19] CONCERN: PERFORMANCE IMPLICATIONS
// [12] CONCERN: LOG RECORD POPULATOR CALLBACKS
// [11] CONCERN: INTERNAL BROADCAST OBSERVER
// [ 9] CONCERN: DEFAULT THRESHOLD LEVELS CALLBACK
// [ 6] CONCERN: CATEGORY NAME FILTER CALLBACK
// [ 5] CONCERN: DEFAULT THRESHOLD LEVELS
// [ 3] CONCERN: LOGGER MANAGER DEFAULTS
// [-1] CONCERN: LEGACY OBSERVERS LIFETIME

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::LoggerManager              Obj;

typedef ball::Logger                     Logger;

typedef ball::Category                   Cat;
typedef ball::Observer                   Obs;
typedef ball::Record                     Rec;
typedef ball::RecordAttributes           Attr;
typedef ball::RecordBuffer               RecBuf;

typedef ball::UserFieldType              FieldType;
typedef ball::UserFields                 FieldValues;

typedef ball::ThresholdAggregate         Thresholds;

typedef Logger::PublishAllTriggerCallback   Pac;
typedef Obj::UserFieldsPopulatorCallback    Pop;

typedef Obj::CategoryNameFilterCallback     Cnf;
typedef Obj::DefaultThresholdLevelsCallback Dtc;

#define F_ __FILE__                           // current source file name

const int NUM_LOGGERS = 10;
const int BUF_SIZE    = 32768;

const char *NAMES[] = {  // test category names
    "A",
    "AB",
    "ab",
    "ABC",
    "ABCDEDFGHIJKL",
    "A.B.C.E",
    "abcd_efgh_.abcd",
    "a.b.c.e.d.f",
    "1",
    "12345",
    "123abc456def",
    "A really really really looooooooooooooooooooooooooooooooong name",
};
enum { NUM_NAMES = sizeof NAMES / sizeof *NAMES };

struct my_Severity {
    enum Level {
        e_FATAL =  32,
        e_ERROR =  64,
        e_WARN  =  96,
        e_INFO  = 128,
        e_DEBUG = 160,
        e_TRACE = 192
    };
};

// *** Keep the following in sync with the .cpp file. ***

const int   MESSAGE_BUFFER_SIZE   = 8192;
const char *DEFAULT_CATEGORY_NAME = "";

const unsigned char FACTORY_RECORD     =  0;
const unsigned char FACTORY_PASS       = 64;
const unsigned char FACTORY_TRIGGER    =  0;
const unsigned char FACTORY_TRIGGERALL =  0;

void executeInParallel(int numThreads, bslmt::ThreadUtil::ThreadFunction func)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
    // to i'th thread.  Finally join all the threads.
{
    bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        const bsls::Types::IntPtr ii = i;
        bslmt::ThreadUtil::create(&threads[i], func, (void*) ii);
    }
    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

// ============================================================================
//                               USAGE EXAMPLE 1
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_1 {

///Example 1: Initialization #1
/// - - - - - - - - - - - - - -
// Clients that perform logging must first instantiate the singleton logger
// manager using the 'ball::LoggerManagerScopedGuard' class.  This example
// shows how to create a logger manager with the most basic "default behavior".
// Subsequent examples will show more customized behavior.
//
// The following snippets of code illustrate the initialization sequence
// (typically performed near the top of 'main').
//
// First, we create a 'ball::LoggerManagerConfiguration' object,
// 'configuration', and set the logging "pass-through" level -- the level at
// which log records are published to registered observers -- to 'WARN' (see
// {'Categories, Severities, and Threshold Levels'}):
//..
    // myApp.cpp

    int main()
    {
        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(ball::Severity::e_WARN);
//..
// Next, create a 'ball::LoggerManagerScopedGuard' object whose constructor
// takes the configuration object just created.  The guard will initialize the
// logger manager singleton on creation and destroy the singleton upon
// destruction.  This guarantees that any resources used by the logger manager
// will be properly released when they are not needed:
//..
        ball::LoggerManagerScopedGuard guard(configuration);
//..
// Note that the application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Finally, we create a 'ball::StreamObserver' object 'observer' that will
// publish records to 'stdout' and register it with the logger manager
// singleton.  Note that observers must be registered by name; this example
// simply uses "default" for a name:
//..
        bslma::Allocator *alloc = bslma::Default::globalAllocator(0);

        bsl::shared_ptr<ball::StreamObserver> observer(
                                  new(*alloc) ball::StreamObserver(&bsl::cout),
                                  alloc);
        ball::LoggerManager::singleton().registerObserver(observer, "default");
//..
// The application is now prepared to log messages using the 'ball' logging
// subsystem:
//..
          // ...

          return 0;
      }
//..

}  // close namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_1

// ============================================================================
//                               USAGE EXAMPLE 2
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_2 {

///Example 2: Initialization #2
/// - - - - - - - - - - - - - -
// In this example, we demonstrate a more elaborate initial configuration for
// the logger manager.  In particular, we create the singleton logger manager
// with a configuration that has a category name filter functor, a
// 'DefaultThresholdLevelsCallback' functor, and user-chosen values for the
// "factory default" threshold levels.
//
// First, we define three 'static' functions that are employed by the two
// functors.  The 'toLower' function implements our category name filter.  It
// is wrapped within a functor object and maps category names to lower-case:
//..
    static
    void toLower(bsl::string *buffer, const char *s)
    {
        ASSERT(buffer);
        ASSERT(s);

        buffer->clear();
        while (*s) {
            buffer->push_back(static_cast<char>(bsl::tolower(*s)));
            ++s;
        }
        buffer->push_back(0);
    }
//..
// The following two functions provide the implementation for our
// 'DefaultThresholdLevelsCallback' functor.  The 'inheritThresholdLevels'
// function is wrapped within a functor object; the 'getDefaultThresholdLevels'
// function is a helper that does the hard work.  We assume a hierarchical
// category naming scheme that uses '.' to delimit the constituents of names.
// For example, the three categories named "x", "x.y", and "x.y.z" are related
// in the sense that "x" is an ancestor of both "x.y" and "x.y.z", and "x.y" is
// an ancestor "x.y.z".  Suppose that "x" is added to the registry first.  If
// "x.y" is then added to the registry by calling 'setCategory(const char *)',
// it would "inherit" threshold level values from "x".  Similarly, when "x.y.z"
// is added to the registry by calling the 1-argument 'setCategory' method, it
// inherits threshold level values from "x.y" (i.e., a category inherits from
// its nearest ancestor that exists in the registry when it is added).  Note
// that a category named "xx.y" (for example) is not related to either of "x",
// "x.y", or "x.y.z":
//..
    static
    int getDefaultThresholdLevels(int                        *recordLevel,
                                  int                        *passLevel,
                                  int                        *triggerLevel,
                                  int                        *triggerAllLevel,
                                  char                        delimiter,
                                  const ball::LoggerManager&  loggerManager,
                                  const char                 *categoryName)
        // Obtain appropriate threshold levels for the category having the
        // specified 'categoryName' by searching the registry of the specified
        // 'loggerManager', and store the resulting values at the specified
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // addresses.  A hierarchical category naming scheme is assumed that
        // employs the specified 'delimiter' to separate the components of
        // category names.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless 'recordLevel',
        // 'passLevel', 'triggerLevel', and 'triggerAllLevel' are non-null, and
        // 'categoryName' is null-terminated.
    {
        ASSERT(recordLevel);
        ASSERT(passLevel);
        ASSERT(triggerLevel);
        ASSERT(triggerAllLevel);
        ASSERT(categoryName);

        enum { SUCCESS =  0,
               FAILURE = -1 };

        bsl::string buffer(categoryName);
        while (1) {
            const ball::Category *category =
                loggerManager.lookupCategory(buffer.c_str());
            if (0 != category) {
                *recordLevel     = category->recordLevel();
                *passLevel       = category->passLevel();
                *triggerLevel    = category->triggerLevel();
                *triggerAllLevel = category->triggerAllLevel();
                return SUCCESS;                                       // RETURN
            }

            const char *newEnd = bsl::strrchr(buffer.c_str(), delimiter);
            if (0 == newEnd) {
                return FAILURE;                                       // RETURN
            }
            buffer.resize(newEnd - buffer.data());
        }
    }

    static
    void inheritThresholdLevels(int        *recordLevel,
                                int        *passLevel,
                                int        *triggerLevel,
                                int        *triggerAllLevel,
                                const char *categoryName)
        // Obtain appropriate threshold levels for the category having the
        // specified 'categoryName', and store the resulting values at the
        // specified 'recordLevel', 'passLevel', 'triggerLevel', and
        // 'triggerAllLevel' addresses.  The behavior is undefined unless
        // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'
        // are non-null, and 'categoryName' is null-terminated.
    {
        ASSERT(recordLevel);
        ASSERT(passLevel);
        ASSERT(triggerLevel);
        ASSERT(triggerAllLevel);
        ASSERT(categoryName);

        const ball::LoggerManager& manager = ball::LoggerManager::singleton();
        if (0 != getDefaultThresholdLevels(recordLevel,
                                           passLevel,
                                           triggerLevel,
                                           triggerAllLevel,
                                           '.',
                                           manager,
                                           categoryName)) {
            *recordLevel     = manager.defaultRecordThresholdLevel();
            *passLevel       = manager.defaultPassThresholdLevel();
            *triggerLevel    = manager.defaultTriggerThresholdLevel();
            *triggerAllLevel = manager.defaultTriggerAllThresholdLevel();
        }
    }
//..
// Then, we create the callback functors that will be supplied to the logger
// manager singleton initialization (as in "Example 1" above, we assume that
// the initialization sequence occurs somewhere near the top of 'main'):
//..
    // myApp2.cpp

    int main()
    {
        // ...

        ball::LoggerManager::CategoryNameFilterCallback nameFilter(&toLower);

        ball::LoggerManager::DefaultThresholdLevelsCallback
                                   thresholdsCallback(&inheritThresholdLevels);
//..
// Next, we define four values for our custom "factory default" thresholds.
// These values will be stored within the logger manager and will be available
// to all users whenever the "factory defaults" are needed, for the life of the
// logger manager.  In this example, however, we will also be installing the
// 'thresholdsCallback' defined above, so unless that functor is un-installed
// (by a call to 'setDefaultThresholdLevelsCallback'), these four "factory
// defaults" will have no practical effect, since the callback mechanism "steps
// in front of" the default values:
//..
        int recordLevel     = 125;
        int passLevel       = 100;
        int triggerLevel    =  75;
        int triggerAllLevel =  50;
//..
// Then, we can configure a 'ball::LoggerManagerDefaults' object, 'defaults',
// with these four threshold values.  'defaults' can then be used to configure
// the 'ball::LoggerManagerConfiguration' object that will be passed to the
// 'ball::LoggerManagerScopedGuard' constructor (below):
//..
        ball::LoggerManagerDefaults defaults;
        defaults.setDefaultThresholdLevelsIfValid(recordLevel,
                                                  passLevel,
                                                  triggerLevel,
                                                  triggerAllLevel);
//..
// Next, we create and set the 'ball::LoggerManagerConfiguration' object,
// 'configuration', that will describe our desired configuration:
//..
        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultValues(defaults);
        configuration.setCategoryNameFilterCallback(nameFilter);
        configuration.setDefaultThresholdLevelsCallback(thresholdsCallback);
//..
// Then, we instantiate the singleton logger manager, passing in the
// 'configuration' that we have just created:
//..
        ball::LoggerManagerScopedGuard guard(configuration);
//..
// Note that application is now prepared to log messages using the 'ball'
// logging subsystem, but until the application registers an observer, all log
// messages will be discarded.
//
// Now, we will demonstrate the functors and client-supplied default threshold
// overrides.
//
// First, we obtain a reference to the singleton logger manager:
//..
        ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// Then, we obtain a reference to the *Default* *Category* and 'ASSERT' that
// its threshold levels match the client-supplied values that override the
// "factory-supplied" default values:
//..
        const ball::Category& defaultCategory = manager.defaultCategory();
        ASSERT(125 == defaultCategory.recordLevel());
        ASSERT(100 == defaultCategory.passLevel());
        ASSERT( 75 == defaultCategory.triggerLevel());
        ASSERT( 50 == defaultCategory.triggerAllLevel());
//..
// Next, we add a category named "BloombergLP" (by calling 'addCategory').
// Note that threshold levels supplied with the category override all defaults
// (including thresholds set by the supplied callback).  Also note that the
// logger manager invokes the supplied category name filter to map the category
// name to lower-case before the new category is added to the category
// registry.  The name filter is also invoked by 'lookupCategory' whenever a
// category is searched for (i.e., by name) in the registry:
//..
        const ball::Category *blpCategory =
                           manager.addCategory("BloombergLP", 128, 96, 64, 32);
        ASSERT(blpCategory == manager.lookupCategory("BLOOMBERGLP"));
        ASSERT(  0 == bsl::strcmp("bloomberglp", blpCategory->categoryName()));
        ASSERT(128 == blpCategory->recordLevel());
        ASSERT( 96 == blpCategory->passLevel());
        ASSERT( 64 == blpCategory->triggerLevel());
        ASSERT( 32 == blpCategory->triggerAllLevel());
//..
// Then, we add a second category named "BloombergLP.bal.ball" (by calling
// 'setCategory') and 'ASSERT' that the threshold levels are "inherited" from
// category "BloombergLP":
//..
        const ball::Category *ballCategory =
                                   manager.setCategory("BLOOMbergLP.bal.ball");

        ASSERT(ballCategory == manager.lookupCategory("bloomberglp.bal.ball"));
        ASSERT(  0 == bsl::strcmp("bloomberglp.bal.ball",
                                  ballCategory->categoryName()));
        ASSERT(128 == ballCategory->recordLevel());
        ASSERT( 96 == ballCategory->passLevel());
        ASSERT( 64 == ballCategory->triggerLevel());
        ASSERT( 32 == ballCategory->triggerAllLevel());
//..
// Now, we add a third category named "Other.equities", again by calling
// 'setCategory'.  This category has no ancestor currently in the registry, so
// its threshold levels match those of the *Default* *Category*:
//..
        const ball::Category *equitiesCategory =
                                         manager.setCategory("Other.equities");
        ASSERT(equitiesCategory == manager.lookupCategory("OTHER.EQUITIES"));
        ASSERT(  0 == bsl::strcmp("other.equities",
                                  equitiesCategory->categoryName()));
        ASSERT(125 == equitiesCategory->recordLevel());
        ASSERT(100 == equitiesCategory->passLevel());
        ASSERT( 75 == equitiesCategory->triggerLevel());
        ASSERT( 50 == equitiesCategory->triggerAllLevel());

// Finally, we create a 'ball::StreamObserver' object 'observer' that will
// publish records to 'stdout' and register it with the logger manager
// singleton.  Note that observers must be registered by name; this example
// simply uses "default" for a name:
//..
        bslma::Allocator *alloc = bslma::Default::globalAllocator(0);

        bsl::shared_ptr<ball::StreamObserver> observer(
                                  new(*alloc) ball::StreamObserver(&bsl::cout),
                                  alloc);

        manager.registerObserver(observer, "default");
        // ...

        return 0;
    }
//..
}  // close namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_2

// ============================================================================
//                               USAGE EXAMPLE 3
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_3 {

///Example 3: Efficient Logging of 'ostream'-able Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how instances of a class supporting
// streaming to 'bsl::ostream' (via overloaded 'operator<<') can be logged.  It
// also demonstrates how to use the 'logMessage' method to log messages to a
// logger.  Suppose we want to *efficiently* log instances of the following
// class:
//..
    class Information {
        // This (incomplete) class is a simple aggregate of a "heading" and
        // "contents" pertaining to that heading.  It serves to illustrate how
        // to log the string representation of an object.

        bsl::string d_heading;
        bsl::string d_contents;

      public:
        Information(const char *heading, const char *contents);
        ~Information();
        const bsl::string& heading() const;
        const bsl::string& contents() const;
    };
//..
// In addition, we define the following free operator for streaming instances
// of 'Information' to an 'bsl::ostream':
//..
    bsl::ostream& operator<<(bsl::ostream&      stream,
                             const Information& information)
    {
        stream << information.heading();
        stream << ": ";
        stream << information.contents() << bsl::endl;
        return stream;
    }
//..
// The following function logs an 'Information' object to the specified
// 'logger':
//..
    void logInformation(ball::Logger          *logger,
                        const Information&     information,
                        ball::Severity::Level  severity,
                        const ball::Category&  category,
                        const char            *fileName,
                        int                    lineNumber)
    {
//..
// First, obtain a record that has its 'fileName' and 'lineNumber' attributes
// set:
//..
        ball::Record *record = logger->getRecord(fileName, lineNumber);
//..
// Then, we get a modifiable reference to the fixed fields of 'record':
//..
        ball::RecordAttributes& attributes = record->fixedFields();
//..
// Next, we create a 'bsl::ostream' to which the string representation
// 'information' can be output.  Note that 'stream' is supplied with the stream
// buffer of 'attributes':
//..
        bsl::ostream stream(&attributes.messageStreamBuf());
//..
// Then, we stream 'information' into our output 'stream'.  This will set the
// message attribute of 'record' to the streamed data:
//..
        stream << information;
//..
// Finally, we log 'record' using 'logger':
//..
        logger->logMessage(category, severity, record);
    }
//..
// Notice that we did not need to allocate a scratch buffer to stream the
// object contents into.  That would have required an extra copy and the cost
// of allocation and deallocation, and thus would have been more inefficient.

        //  Elided function definitions

Information::Information(const char *heading, const char *contents)
: d_heading(heading)
, d_contents(contents)
{
}

Information::~Information()
{
}

const bsl::string& Information::heading() const
{
    return d_heading;
}

const bsl::string& Information::contents() const
{
    return d_contents;
}

}  // close namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_3

// ============================================================================
//                               USAGE EXAMPLE 4
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_4 {

///Example 4: Logging using a 'ball::Logger'
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates using the a 'ball::Logger' directly to log
// messages.  In practice, clients are encouraged to use the logging macros
// (see {'ball_log'}, which cannot be shown here for dependency reasons.  The
// following example assumes logging has been correctly initialized (see prior
// examples).
//
// The following simple 'factorial' function takes and returns values of type
// 'int'.  Note that this function has a very limited range of input, namely
// integers in the range '[0 .. 13]'.  This limited range serves to illustrate
// a usage pattern of the logger, namely to log "warnings" whenever a key
// function is given bad input.
//
// For this example, it is sufficient to use the severity levels defined in the
// 'ball::Severity::Level' enumeration:
//..
    enum Level {
        OFF   =   0,  // disable generation of corresponding message
        FATAL =  32,  // a condition that will (likely) cause a *crash*
        ERROR =  64,  // a condition that *will* cause incorrect behavior
        WARN  =  96,  // a *potentially* problematic condition
        INFO  = 128,  // data about the running process
        DEBUG = 160,  // information useful while debugging
        TRACE = 192   // execution trace data
    };
//..
// Note that the intervals left between enumerator values allow applications
// to define additional values in case there is a desire to log with more
// finely-graduated levels of severity.  We will not need that granularity
// here; 'ball::Severity::e_WARN' is appropriate to log a warning message if
// the input argument to our factorial function is not in this range of values.
//
// We will register a unique category for this function, so that logged
// messages from our function will be identified in the published output.
// Also, with a unique category name, the logging behavior of this function can
// be administered by resetting the various threshold levels for the category.
// In this example, we will accept the default thresholds.
//
// The 'setCategory' method accepts a name and returns the address of a
// 'ball::Category' with that name or, in some circumstances, the address of
// the *Default* *Category* (see the function-level documentation of
// 'setCategory' for details).  The address returned by 'setCategory' is stored
// in a function-static pointer variable (i.e., it is fetched only once upon
// first use).  In this example, we assume that we are writing a function for
// Equities Graphics that will live in that group's Math library.  The dot
// "delimiters" ('.') have no particular significance to the logger, but may be
// used by the administration methods to "induce" a hierarchical behavior on
// our category, should that be useful.  See, e.g., the callback functor
// 'ball::LoggerManager::DefaultThresholdLevelsCallback' and its documentation,
// and Usage Example 2 above for information on how to use category names to
// customize logger behavior:
//..
    int factorial(int n)
        // Return the factorial of the specified value 'n' if the factorial
        // can be represented as an 'int', and a negative value otherwise.
    {
        static const ball::Category *factorialCategory =
            ball::LoggerManager::singleton().setCategory(
                                            "equities.graphics.math.factorial",
                                            ball::Severity::e_INFO,
                                            ball::Severity::e_TRACE,
                                            ball::Severity::e_ERROR,
                                            ball::Severity::e_FATAL);
//..
// We must also obtain a reference to a logger by calling the logger manager
// 'getLogger' method.  Note that this logger may not safely be cached as a
// function 'static' variable since our function may be called in different
// threads having different loggers.  Even in a single-threaded program, the
// owner of 'main' is free to install new loggers at any point, so a
// statically-cached logger would be a problem:
//..
        ball::Logger& logger = ball::LoggerManager::singleton().getLogger();
//..
// Now we validate the input value 'n'.  If 'n' is either negative or too
// large, we will log a warning message (at severity level
// 'ball::Severity::e_WARN') and return a negative value.  Note that calls to
// 'logMessage' have no run-time overhead (beyond the execution of a simple
// 'if' test) unless 'ball::Severity::e_WARN' is at least as severe as one of
// the threshold levels of 'factorialCategory':
//..
        if (0 > n) {
            logger.logMessage(*factorialCategory,
                              ball::Severity::e_WARN,
                              __FILE__,
                              __LINE__,
                              "Attempt to take factorial of negative value.");
            return n;                                                 // RETURN
        }

        enum { MAX_ARGUMENT = 13 };  // maximum value accepted by 'factorial'

        if (MAX_ARGUMENT < n) {
            logger.logMessage(*factorialCategory,
                              ball::Severity::e_WARN,
                              __FILE__,
                              __LINE__,
                              "Result too large for 'int'.");
            return -n;                                                // RETURN
        }
//..
// The remaining code proceeds mostly as expected, but adds one last message
// that tracks control flow when 'ball::Severity::e_TRACE' is at least as
// severe as one of the threshold levels of 'factorialCategory' (e.g., as
// might be the case during debugging):
//..
        int product = 1;
        while (1 < n) {
            product *= n;
            --n;
        }

        logger.logMessage(*factorialCategory,
                          ball::Severity::e_TRACE,
                          __FILE__,
                          __LINE__,
                          "Exiting 'factorial' successfully.");

        return product;
    }
//..

}  // close namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_4

namespace {
    // Various helper functions used to test default threshold levels and
    // related callbacks.

static
int getDefaultThresholdLevels(int                        *recordLevel,
                              int                        *passLevel,
                              int                        *triggerLevel,
                              int                        *triggerAllLevel,
                              char                        delimiter,
                              const ball::LoggerManager&  loggerManager,
                              const char                 *categoryName)
{
    ASSERT(recordLevel);
    ASSERT(passLevel);
    ASSERT(triggerLevel);
    ASSERT(triggerAllLevel);
    ASSERT(categoryName);

    enum { SUCCESS =  0,
           FAILURE = -1 };

    bsl::string buffer(categoryName);

    while (1) {
        const ball::Category *category =
                                  loggerManager.lookupCategory(buffer.c_str());
        if (0 != category) {
                *recordLevel     = category->recordLevel();
                *passLevel       = category->passLevel();
                *triggerLevel    = category->triggerLevel();
                *triggerAllLevel = category->triggerAllLevel();
                return SUCCESS;                                       // RETURN
        }

        const char *newEnd = bsl::strrchr(buffer.c_str(), delimiter);
        if (0 == newEnd) {
            return FAILURE;                                           // RETURN
        }
        buffer.resize(newEnd - buffer.data());
    }
}

static Obj *g_overrideManager;

static
void inheritThresholdLevelsUsingManager(
                                   int                       *recordLevel,
                                   int                       *passLevel,
                                   int                       *triggerLevel,
                                   int                       *triggerAllLevel,
                                   const char                *categoryName,
                                   const ball::LoggerManager *lm)
{
    ASSERT(recordLevel);
    ASSERT(passLevel);
    ASSERT(triggerLevel);
    ASSERT(triggerAllLevel);
    ASSERT(categoryName);

    if (0 == lm) {
        lm = g_overrideManager;
    }

    if (0 != getDefaultThresholdLevels(recordLevel,
                                       passLevel,
                                       triggerLevel,
                                       triggerAllLevel,
                                       '.',
                                       *lm,
                                       categoryName)) {
        *recordLevel     = lm->defaultRecordThresholdLevel();
        *passLevel       = lm->defaultPassThresholdLevel();
        *triggerLevel    = lm->defaultTriggerThresholdLevel();
        *triggerAllLevel = lm->defaultTriggerAllThresholdLevel();
    }
}

static
void inheritThresholdLevels(int        *recordLevel,
                            int        *passLevel,
                            int        *triggerLevel,
                            int        *triggerAllLevel,
                            const char *categoryName)
{
    const ball::LoggerManager& lm = ball::LoggerManager::singleton();
    inheritThresholdLevelsUsingManager(recordLevel, passLevel,
                                       triggerLevel, triggerAllLevel,
                                       categoryName, &lm);
}

void verifyLoggerManagerDefaults(const ball::LoggerManager&  manager,
                                 bslma::Allocator           *expectedAllocator,
                                 ball::Observer             *expectedObserver)
    // This function verifies defaults for the specified 'manager'.  Note that
    // expected Observer is passed to verify legacy API (where raw pointer to
    // Observer is supplied during singleton initialization).
{
    bslma::Allocator *oa = bslma::Default::globalAllocator(expectedAllocator);

    const Obj& X = manager;

    ASSERT(oa                 == X.allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    ASSERT(expectedObserver   == X.observer());
#else
    (void)expectedObserver;
#endif

    ASSERT(FACTORY_RECORD     == X.defaultRecordThresholdLevel());
    ASSERT(FACTORY_PASS       == X.defaultPassThresholdLevel());
    ASSERT(FACTORY_TRIGGER    == X.defaultTriggerThresholdLevel());
    ASSERT(FACTORY_TRIGGERALL == X.defaultTriggerAllThresholdLevel());

    ASSERT(1                  == X.numCategories());

    const Cat& defaultCategory = X.defaultCategory();
    ASSERT(0 == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                              defaultCategory.categoryName()));
    ASSERT(FACTORY_RECORD     == defaultCategory.recordLevel());
    ASSERT(FACTORY_PASS       == defaultCategory.passLevel());
    ASSERT(FACTORY_TRIGGER    == defaultCategory.triggerLevel());
    ASSERT(FACTORY_TRIGGERALL == defaultCategory.triggerAllLevel());
}

}  // close unnamed namespace

static int globalFactorialArgument;  // TBD kludge

static
void myUserFieldsPopulator(ball::UserFields *list)
{
    list->appendInt64(globalFactorialArgument);
}

// ============================================================================
//                  LOGGER MANAGER SINGLETON REINITIALIZATION
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_SINGLETON_REINITIALIZATION {

static int bslsLogMsgCount = 0;

static
void countingBslsLogMessageHandler(bsls::LogSeverity::Enum,
                                   const char              *fileName,
                                   int                      lineNumber,
                                   const char              *message)
    // Handle a 'ball' record having the specified 'fileName', 'lineNumber',
    // and 'message', and increment the count of messages routed to this
    // handler.
{
    ASSERT(fileName);        (void)fileName;
    ASSERT(0 < lineNumber);  (void)lineNumber;
    ASSERT(message);         (void)message;

    ++bslsLogMsgCount;
}

static
void logMessageTest(
                bool                                   loggerManagerExistsFlag,
                const BloombergLP::ball::TestObserver& testObserver,
                int                                    numPublishedSoFar)
    // Log a message through the logger manager singleton if the specified
    // 'loggerManagerExistsFlag' is 'true' and unconditionally invoke
    // 'BSLS_LOG_WARN', then verify the expected logging behavior based on
    // 'loggerManagerExistsFlag' and the specified 'testObserver' and
    // 'numPublishedSoFar'.
{
    ASSERT(loggerManagerExistsFlag == Obj::isInitialized());

    // Log through the singleton if it exists.

    if (loggerManagerExistsFlag) {
        Obj& mX = Obj::singleton();  const Obj& X = mX;

        mX.setCategory("X.Y.Z");  // default threshold levels
        const Cat *cat = X.lookupCategory("X.Y.Z");

        ball::Logger& logger = mX.getLogger();

        logger.logMessage(*cat, 64, __FILE__, __LINE__, "'ball' message");
    }

    // Log using 'bsls::Log' regardless of whether the singleton exists.

    BSLS_LOG_WARN("'bsls::Log' message");

    if (loggerManagerExistsFlag) {
        numPublishedSoFar += 2;
    }
    ASSERT(numPublishedSoFar == testObserver.numPublishedRecords());

    if (loggerManagerExistsFlag) {
        ASSERT(0 == bslsLogMsgCount);
    }
    else {
        ASSERT(1 == bslsLogMsgCount);
    }
}

enum {
    k_NUM_ITERATIONS = 64,
    k_NUM_THREADS    =  3
};

struct ThreadArgs {
    ball::TestObserver                     *d_to_p;
    const ball::LoggerManagerConfiguration *d_lmc_p;
};

extern "C" void *initSingletonThread(void *args)
{
    ThreadArgs *threadArgs = reinterpret_cast<ThreadArgs *>(args);

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManager::initSingleton(threadArgs->d_to_p,
                                           *threadArgs->d_lmc_p);
#else
        ball::LoggerManager::initSingleton(*threadArgs->d_lmc_p);
#endif
    }

    return 0;
}

extern "C" void *shutDownSingletonThread(void *)
{
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        ball::LoggerManager::shutDownSingleton();
    }

    return 0;
}

extern "C" void *scopedGuardThread(void *args)
{
    ThreadArgs *threadArgs = reinterpret_cast<ThreadArgs *>(args);

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManagerScopedGuard guard(threadArgs->d_to_p,
                                             *threadArgs->d_lmc_p);
#else
        ball::LoggerManagerScopedGuard guard(*threadArgs->d_lmc_p);
#endif
    }

    return 0;
}

}  // close namespace BALL_LOGGERMANAGER_SINGLETON_REINITIALIZATION

// ============================================================================
//                    FUNKY WINDOWS-SPECIFIC INITSINGLETON
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_WINDOWS_SPECIFIC_INITSINGLETON {

struct TrackingAllocator : public bslma::Allocator {
    // This 'struct' provides a concrete implementation of the
    // 'bslma::Allocator' protocol that tracks the address of a
    // 'ball::LoggerManager' object and detects when it has been deleted.

    // PUBLIC DATA
    void *d_tracked_pointer_p;  // address of tracked 'LoggerManager' object
    bool  d_deleted;            // 'true' iff tracked pointer has been deleted

    // CREATORS
    TrackingAllocator()
        // Create a tracking allocator that is initialized to track no object.
    : d_tracked_pointer_p(0)
    , d_deleted(false)
    {
    }

    // MANIPULATORS
    void *allocate(size_type size)
        // Return a newly allocated block of memory of (at least) the specified
        // positive 'size' (in bytes).  If this allocator it not yet tracking
        // an address and 'sizeof(ball::LoggerManager) == size', then set this
        // object to track the address that is returned.
    {
        void *p = ::operator new(size);
        if (!d_tracked_pointer_p
            && sizeof(ball::LoggerManager) == size) {
            d_tracked_pointer_p = p;
        }
        return p;
    }

    virtual void deallocate(void *address)
        // Return the memory block at the specified 'address' back to this
        // allocator.  If 'address' matches the pointer being tracked, then set
        // a flag indicating that it has been deleted.
    {
        if (address && d_tracked_pointer_p == address) {
            d_deleted = true;
        }
        ::operator delete(address);
    }
};

}  // close namespace BALL_LOGGERMANAGER_WINDOWS_SPECIFIC_INITSINGLETON

// ============================================================================
//                         CASE 17 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOGGERMANAGER_TEST_CASE_17 {

class MyObserver : public ball::Observer {
    // This concrete implementation of 'ball::Observer' maintains a count of
    // the number of messages published to it and gives access to that count
    // through 'publishCount'.

    int           d_publishCount;
    bsl::ostream& d_stream;

  public:
    // CREATORS
    explicit
    MyObserver(bsl::ostream& stream) : d_publishCount(0), d_stream(stream)
    {
    }

    // MANIPULATORS
    void publish(const ball::Record& record, const ball::Context& context)
    {
        ++d_publishCount;
        d_stream << "Log " << context.recordIndex() + 1
                 << " of " << context.sequenceLength()
                 << " : "  << record.fixedFields().message()
                 << "\n"   << bsl::flush;
    }

    void resetPublishCount()
    {
        d_publishCount = 0;
    }

    // ACCESSORS
    int publishCount() const
    {
        return d_publishCount;
    }

};

}  // close namespace BALL_LOGGERMANAGER_TEST_CASE_17

namespace {
class TestDestroyObserver : public ball::Observer {
    // This class counts the number of 'releaseRecords' methods invocations.

    int d_releaseCnt;

  public:
    explicit TestDestroyObserver() : d_releaseCnt(0)
    {
    }

    virtual void releaseRecords()
    {
        d_releaseCnt++;
    }

    int getReleaseCnt()
    {
        return d_releaseCnt;
    }
};

class PerformanceObserver : public ball::Observer {
    // This class is used by performance tests.  No real logging is done by
    // this observer.

    int d_publishCount;

  public:
    PerformanceObserver()
    : d_publishCount(0)
    {
    }

    using Observer::publish;   // Avoid hiding base class method;

    virtual void publish(const bsl::shared_ptr<const ball::Record>& record,
                         const ball::Context&                       context)
    {
        (void)record;
        (void)context;
        ++d_publishCount;
    }

    virtual void releaseRecords()
    {
    }

    int publishCount() const
    {
        return d_publishCount;
    }
};

void doPerformanceTest(ball::LoggerManager& loggerManager,
                       int                  num_logs,
                       bool                 verbose)
{
    const char *currentFile = __FILE__;

    bsls::Stopwatch timer;

    ball::Logger& logger = loggerManager.getLogger();

    const ball::Category *cat = loggerManager.lookupCategory(
                                                            "PerformanceTest");

    if (verbose) cerr << "              'logMessage' < RECORD      : ";
    timer.start();
    for (int i = 0; i < num_logs; ++i) {
        logger.logMessage(*cat, 250, currentFile, __LINE__, "Message");
    }
    timer.stop();
    if (verbose) cerr << num_logs/timer.elapsedTime() << " (log/sec)" << endl;

    if (verbose) cerr << "RECORD      < 'logMessage' < PASS        : ";
    timer.start();
    for (int i = 0; i < num_logs; ++i) {
        logger.logMessage(*cat, 180, currentFile, __LINE__, "Message");
    }
    timer.stop();
    if (verbose) cerr << num_logs/timer.elapsedTime() << " (log/sec)" << endl;
    logger.logMessage(*cat, 0, currentFile, __LINE__, "Message");

    if (verbose) cerr << "PASS        < 'logMessage' < TRIGGER     : ";
    timer.start();
    for (int i = 0; i < num_logs; ++i) {
        logger.logMessage(*cat, 120, currentFile, __LINE__, "Message");
    }
    timer.stop();
    if (verbose) cerr << num_logs/timer.elapsedTime() << " (log/sec)" << endl;
    logger.logMessage(*cat, 0, currentFile, __LINE__, "Message");

    if (verbose) cerr << "TRIGGER     < 'logMessage' < TRIGGER ALL : ";
    timer.start();
    for (int i = 0; i < num_logs; ++i) {
        logger.logMessage(*cat, 70, currentFile, __LINE__, "Message");
    }
    timer.stop();
    if (verbose) cerr << num_logs/timer.elapsedTime() << " (log/sec)" << endl;
    logger.logMessage(*cat, 0, currentFile, __LINE__, "Message");

    if (verbose) cerr << "TRIGGER ALL < 'logMessage'               : ";
    timer.start();
    for (int i = 0; i < num_logs; ++i) {
        logger.logMessage(*cat, 20, currentFile, __LINE__, "Message");
    }
    timer.stop();
    if (verbose) cerr << num_logs/timer.elapsedTime() << " (log/sec)" << endl;
}

}  // close unnamed namespace

namespace BALL_LOGGERMANAGER_TEST_LOOKUP_CATEGORY {
enum {
    k_NUM_THREADS = 4  // number of threads
};

const char *LC[] = {
    "ball_loggermanagerconfiguration",
    "ball_broadcastobserver",
    "ball_recordstringformatter",
    "ball_thresholdaggregate"
};

const char *UC[] = {
    "BALL_LOGGERMANAGERCONFIGURATION",
    "BALL_BROADCASTOBSERVER",
    "BALL_RECORDSTRINGFORMATTER",
    "BALL_THRESHOLDAGGREGATE"
};

struct ThreadData {
    int d_index;  // index of current thread
};

bslmt::Barrier barrier(k_NUM_THREADS);

extern "C" {
    void *workerThreadLookupCategory(void *arg)
    {
        ThreadData *p = reinterpret_cast<ThreadData *>(arg);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        ASSERT(manager.lookupCategory(UC[p->d_index]) != 0);
        return 0;
    }
}  // extern "C"

}  // close namespace BALL_LOGGERMANAGER_TEST_LOOKUP_CATEGORY

namespace BALL_LOGGERMANAGER_TEST_INIT_SINGLETON {
enum {
    k_NUM_THREADS = 4  // number of threads
};

struct ThreadData {
    int                  d_index;      // index of current thread
    ball::LoggerManager *d_manager_p;  // instance observed by current thread
};

bslmt::Barrier barrier(k_NUM_THREADS);

extern "C" {
    void *workerThreadInitSingleton(void *arg)
    {
        ThreadData *p = reinterpret_cast<ThreadData *>(arg);

        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(p->d_index,
                                                       p->d_index,
                                                       p->d_index,
                                                       p->d_index);

        barrier.wait();
        ball::LoggerManager::initSingleton(configuration);
        p->d_manager_p = &ball::LoggerManager::singleton();

        return 0;
    }
}  // extern "C"

}  // close namespace BALL_LOGGERMANAGER_TEST_INIT_SINGLETON

namespace BALL_LOGGERMANAGER_CONCURRENT_TESTS {

enum {
    k_NUM_THREADS = 5,       // number of threads
    k_NUM_ITERATIONS = 1000  // number of iterations
};

enum { k_MAX_LIMIT = 32 * 1024 };
bslmt::Barrier barrier(k_NUM_THREADS);

extern "C" {

    void *workerThreadSetCategory(void *)
        // Thread for testing parallel calls to 'setCategory'.
    {
        ball::LoggerManager& manager = Obj::singleton();
        barrier.wait();
        for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
            ASSERT(manager.setCategory("TEST-CATEGORY") != 0);
        }
        return 0;
    }

    void *workerThreadDefaultLoggers(void *arg)
        // Thread for testing parallel calls to 'setLogger'.
    {
        ball::LoggerManager& manager = Obj::singleton();

        int remainder = (int)(bsls::Types::IntPtr)arg % 2;
        if (remainder == 1) {  // odd numbered threads
            ball::FixedSizeRecordBuffer buf(k_MAX_LIMIT);

            ball::Logger *logger = manager.allocateLogger(&buf);
            barrier.wait();
            for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
                manager.setLogger(logger);
                ASSERT(logger == &manager.getLogger());
            }
            manager.deallocateLogger(logger);
        }
        else {                 // odd numbered threads
            const ball::Logger *logger = &manager.getLogger();
            barrier.wait();
            for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
                ASSERT(logger == &manager.getLogger());
            }
        }

        return 0;
    }
}  // extern "C"

class PublishCountingObserver : public ball::Observer {
    // This concrete implementation of 'ball::Observer' maintains a count of
    // the number of messages published to it and gives access to that count
    // through 'publishCount'.

    // DATA
    bsls::AtomicInt d_publishCount;  // count of published records

  public:
    // CREATORS
    PublishCountingObserver()
    : d_publishCount(0)
        // Create a Publish Counting Observer having an initial count of 0.
    {
    }

    ~PublishCountingObserver()
        // Destroy this Publish Counting Observer.
    {
    }

    // MANIPULATORS
    void publish(const ball::Record&, const ball::Context&)
        // Increment the count maintained by this observer by 1.
    {
        ++d_publishCount;
    }

    // ACCESSORS
    int publishCount() const
        // Return the current count maintained by this observer.
    {
        return d_publishCount;
    }
};

extern "C" {
    void *workerThreadMultipleLoggers(void *)
    {
        ball::LoggerManager& mX = Obj::singleton();
        const ball::Category *category = mX.lookupCategory("test-loggers");
        ASSERT(category);

        barrier.wait();
        for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
            ball::FixedSizeRecordBuffer buf(k_MAX_LIMIT);

            ball::Logger *logger = mX.allocateLogger(&buf);

            logger->logMessage(*category,
                               ball::Severity::e_ERROR,
                               __FILE__,
                               __LINE__,
                               "test-message");
            mX.deallocateLogger(logger);
        }
        return 0;
    }
}  // extern "C"

}  // close namespace BALL_LOGGERMANAGER_CONCURRENT_TESTS

namespace BALL_LOGGERMANAGER_TEST_DEFAULTTHRESHOLDLEVELSCALLBACK {
enum {
    k_NUM_THREADS = 4  // number of threads
};
ball::LoggerManager *manager;
bsl::function<void(int *, int *, int *, int *, const char *)> functionDTLC;
bslmt::Barrier barrier(k_NUM_THREADS + 1);
bslmt::Condition condition;

void callback(int *r, int *p, int *t, int *a, const char *c)
    // Wait for all the setting threads to be ready to set the callback, invoke
    // the 'inheritThresholdLevels' method on the specified 'r', 'p', 't', 'a',
    // and 'c', and then time out waiting for the setting threads to finish.
    // (They can't, because a lock is held until this function returns.)
{
    barrier.wait();
    inheritThresholdLevels(r, p, t, a, c);
    bslmt::Mutex m;
    bslmt::LockGuard<bslmt::Mutex> guard(&m);
    ASSERT(-1 == condition.timedWait(&m, bdlt::CurrentTime::now() + 3));
}

extern "C" void *setCategory(void *)
    // Call 'setCategory' in order to invoke the 'callback' method above.
{
    manager->setCategory("joe");
    return 0;
}

extern "C" {
    void *workerThreadDTLC(void *)
        // Wait for all threads to be ready, then attempt to set the callback
        // and signal completion.  The thread waiting for the signal will time
        // out, because the setter mutex is locked by that thread.
    {
        barrier.wait();
        manager->setDefaultThresholdLevelsCallback(&functionDTLC);
        condition.signal();
        return 0;
    }
}  // extern "C"

}  // close namespace BALL_LOGGERMANAGER_TEST_DEFAULTTHRESHOLDLEVELSCALLBACK

namespace TEST_CASE_OBSERVER_VISITOR {

class TestObserverVisitor {
    // This class implements a test observer visitor.  This visitor will verify
    // that all observers in the registry of the logger manager supplied at
    // construction were visited.

  private:
    // DATA
    const ball::LoggerManager *d_loggerManager_p;

  public:
    // CREATORS
    explicit
    TestObserverVisitor(const ball::LoggerManager *loggerManager)
    : d_loggerManager_p(loggerManager)
    {
    }

    // ACCESSORS
    void operator()(const bsl::shared_ptr<ball::Observer>& observer,
                    const bsl::string_view&                observerName) const
    {
        ASSERT(observer == d_loggerManager_p->findObserver(observerName));
    }

    void operator()(bsl::shared_ptr<ball::Observer>&,
                    const bsl::string_view&) const
    {
        ASSERT(!"This overload must not be ever called.");
    }
};

}  // close namespace TEST_CASE_OBSERVER_VISITOR

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

void toLower(bsl::string *buffer, const char *s)
{
    ASSERT(buffer);
    ASSERT(s);

    buffer->clear();
    while (*s) {
        buffer->push_back(static_cast<char>(tolower(*s)));
        ++s;
    }
    buffer->push_back(0);
}

void testThresholdLevelsCb(int        *recordLevel,
                           int        *passLevel,
                           int        *triggerLevel,
                           int        *triggerAllLevel,
                           const char *)
{
    *recordLevel     = 44;
    *passLevel       = 33;
    *triggerLevel    = 22;
    *triggerAllLevel = 11;
}

void testUserFieldsPopulatorCb(ball::UserFields *list)
{
    list->appendString("UFString");
    list->appendInt64(1234);
}

void testAttributesCollector1(
                          const ball::LoggerManager::AttributeVisitor& visitor)
{
    visitor(ball::Attribute("spanId", 1234));
}

void testAttributesCollector2(
                          const ball::LoggerManager::AttributeVisitor& visitor)
{
    visitor(ball::Attribute("dti.tDiff", "25ms"));
}
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      case 43: {
        // --------------------------------------------------------------------
        // TESTING 'ball::LoggerManagerCategoryManip'
        //
        // Concerns:
        //: 1 The basic concern is that the constructor, the destructor, the
        //:   manipulators:
        //:    - void advance();
        //:    - Cat& operator()();
        //:   and the accessor:
        //:    - operator const void *() const;
        //:   operate as expected.
        //
        // Plan:
        //: 1 Create a logger manager X.  Add categories to X having various
        //:   names and threshold level values.  Create an iterator for X.
        //:   Change the threshold level values using the modifiable access
        //:   provided by the iterator.  Verify that the values are changed.
        //:   Change the threshold levels back to their original values, and
        //:   verify that they were reset.  (C-1)
        //
        // Testing:
        //   LoggerManagerCategoryManip(LoggerManager *lm);
        //   ~LoggerManagerCategoryManip();
        //   void advance();
        //   Cat& operator()();
        //   operator const void *() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ball::LoggerManagerCategoryManip'"
                          << endl
                          << "=========================================="
                          << endl;

        ball::TestObserver               testObserver(&cout);
        ball::LoggerManagerConfiguration mLMC;
        ball::LoggerManagerScopedGuard   lmg(&testObserver, mLMC);

        Obj& mLM = Obj::singleton();

        for (int i = 0; i < NUM_NAMES; ++i) {
            // mod 253 so plus 1 is in valid range.
            ASSERT(mLM.addCategory(NAMES[i],
                                   (i + 7)  * (i + 23) % 253,   // record
                                   (i + 3)  * (i + 11) % 253,   // pass
                                   (i + 5)  * (i + 17) % 253,   // trigger
                                   (i + 13) * (i + 29) % 253)); // trigger-all
        }

        int count = 0;
        for (ball::LoggerManagerCategoryManip it(&mLM); it; it.advance()) {
            const Cat *p  = mLM.lookupCategory(it().categoryName());
            const int  rl = p->recordLevel();
            const int  pl = p->passLevel();
            const int  tl = p->triggerLevel();
            const int  al = p->triggerAllLevel();

            ASSERT(rl == it().recordLevel());
            ASSERT(pl == it().passLevel());
            ASSERT(tl == it().triggerLevel());
            ASSERT(al == it().triggerAllLevel());

            it().setLevels(it().recordLevel() + 1,
                           it().passLevel() + 1,
                           it().triggerLevel() + 1,
                           it().triggerAllLevel() + 1);

            ASSERT(rl + 1 == p->recordLevel());
            ASSERT(pl + 1 == p->passLevel());
            ASSERT(tl + 1 == p->triggerLevel());
            ASSERT(al + 1 == p->triggerAllLevel());

            it().setLevels(it().recordLevel() - 1,
                           it().passLevel() - 1,
                           it().triggerLevel() - 1,
                           it().triggerAllLevel() - 1);

            ASSERT(rl == p->recordLevel());
            ASSERT(pl == p->passLevel());
            ASSERT(tl == p->triggerLevel());
            ASSERT(al == p->triggerAllLevel());

            ++count;
        }
        ASSERT(NUM_NAMES + 1 == count);  // + 1 for the *Default* *Category*

      } break;
      case 42: {
        // --------------------------------------------------------------------
        // TESTING 'ball::LoggerManagerCategoryIter'
        //
        // Concerns:
        //: 1 The basic concerns for the iterator are that the constructor,
        //:   the destructor, the manipulator 'operator++', and the accessors:
        //:    - operator const void *() const;
        //:    - const Cat& operator()() const;
        //:   operate as expected.
        //
        // Plan:
        //: 1 Create a map of 'bsl::string' and 'Category *'; add to the
        //:   logger manager various categories with different threshold
        //:   levels.  Add to our map the name of the category and the address
        //:   of the category obtained when it is added.  Create an iterator of
        //:   the logger manager, walk through the categories.  Obtain the name
        //:   of the category returned by the iterator, use this name to lookup
        //:   in our map, verify that a category is obtained, and verify all
        //:   threshold levels are equal.  Remove the map entry from our map.
        //:   Verify that when the iteration is complete, our map is empty.
        //:   Note that the Default Category is always present in the logger
        //:   manager, and needs to be added to our map separately.  (C-1)
        //
        // Testing:
        //   LoggerManagerCategoryIter(const LoggerManager& lm);
        //   ~LoggerManagerCategoryIter();
        //   void operator++();
        //   operator const void *() const;
        //   const Cat& operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'ball::LoggerManagerCategoryIter'"
                          << endl
                          << "========================================="
                          << endl;

        ball::TestObserver               testObserver(&cout);
        ball::LoggerManagerConfiguration mLMC;
        ball::LoggerManagerScopedGuard   lmg(&testObserver, mLMC);

        Obj& mLM = Obj::singleton();

        Cat *cat;
        bsl::map<bsl::string, const Cat *> nameCatMap;
        for (int i = 0; i < NUM_NAMES; ++i) {
            ASSERT(cat = mLM.addCategory(
                                    NAMES[i],
                                    (i + 7)  * (i + 23) % 255,   // record
                                    (i + 3)  * (i + 11) % 255,   // pass
                                    (i + 5)  * (i + 17) % 255,   // trigger
                                    (i + 13) * (i + 29) % 255)); // trigger-all
            nameCatMap.insert(bsl::make_pair(bsl::string(NAMES[i]), cat));
        }

        // add *Default* *Category*
        nameCatMap.insert(bsl::make_pair(mLM.defaultCategory().categoryName(),
                                         &mLM.defaultCategory()));

        for (ball::LoggerManagerCategoryIter it(mLM); it; ++it) {
            bsl::map<bsl::string, const Cat *>::iterator itr =
                                          nameCatMap.find(it().categoryName());
            ASSERT(itr != nameCatMap.end());

            const Cat *cat = itr->second;
            ASSERT(cat);

            ASSERT(cat->recordLevel()     == it().recordLevel());
            ASSERT(cat->passLevel()       == it().passLevel());
            ASSERT(cat->triggerLevel()    == it().triggerLevel());
            ASSERT(cat->triggerAllLevel() == it().triggerAllLevel());
            nameCatMap.erase(it().categoryName());
        }
        ASSERT(0 == nameCatMap.size())

      } break;
#endif // BDE_OMIT_INTERNAL_DEPRECATED
      case 41: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 4
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE #4
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE 4" << endl
                                  << "===============" << endl;

        using namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_4;

        ball::LoggerManagerConfiguration mXC;
        ball::LoggerManagerScopedGuard   lmGuard(mXC);

        factorial(10);
        factorial(-1);
        factorial(15);

      } break;
      case 40: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 3
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE #3
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE 3" << endl
                                  << "===============" << endl;

        using namespace BALL_LOGGERMANAGER_USAGE_EXAMPLE_3;

        ball::LoggerManagerConfiguration mXC;
        ball::LoggerManagerScopedGuard   lmGuard(mXC);

        ball::LoggerManager& mX = ball::LoggerManager::singleton();

        ball::Category *cat = mX.addCategory("test-category",
                                             ball::Severity::e_INFO,
                                             ball::Severity::e_WARN,
                                             ball::Severity::e_ERROR,
                                             ball::Severity::e_FATAL);

        ball::Logger& logger = mX.getLogger();

        Information information("MY-HEADING", "MY-CONTENTS");

        if (verbose) {
            logInformation(&logger,
                           information,
                           ball::Severity::e_WARN,
                           *cat,
                           __FILE__,
                           __LINE__);
        }

      } break;
      case 39: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE #2
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE 2" << endl
                                  << "===============" << endl;

        BALL_LOGGERMANAGER_USAGE_EXAMPLE_2::main();

      } break;
      case 38: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE #1
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE 1" << endl
                                  << "===============" << endl;

        BALL_LOGGERMANAGER_USAGE_EXAMPLE_1::main();

      } break;
      case 37: {
        // --------------------------------------------------------------------
        // TESTING RECORD POOL MEMORY CONSUMPTION
        //
        // Concerns:
        //: 1 When a log record containing a log message above certain size
        //:   is returned to the log record pool, the memory consumed by the
        //:   message is released..
        //
        // Plan:
        //: 1 Generate a sequence of log record of various sizes and verify
        //:   that the log record is being reset prior return to the record
        //:   object pool.  (C-1)
        //
        // Testing:
        //   CONCERN: RECORD POOL MEMORY CONSUMPTION
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING RECORD POOL MEMORY CONSUMPTION"
                          << "\n======================================"
                          << endl;

        bslma::TestAllocator da("default", veryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ball::LoggerManagerConfiguration mLMC;

        // We will be recording a number of [huge] TRACE messages, and then
        // triggering their publication by an INFO message.
        ASSERT(0 == mLMC.setDefaultThresholdLevelsIfValid(
                                                      ball::Severity::e_TRACE,
                                                      0,
                                                      0,
                                                      ball::Severity::e_INFO));

        ball::LoggerManagerScopedGuard lmGuard(mLMC);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();
        ball::Logger& logger = ball::LoggerManager::singleton().getLogger();

        ball::Category *category = manager.setCategory("Test",
                                                       0,
                                                       ball::Severity::e_INFO,
                                                       0,
                                                       0);

        // The log record is published and returned to the record pool.
        logger.logMessage(*category, ball::Severity::e_INFO, "F", 1, "M");

        bsls::Types::Int64 B = ga.numBytesInUse();

        if (veryVerbose) { P(ga.numBytesInUse()) }

        // Logging another log record with a small message.  Such a small
        // message will NOT result in additional memory allocation.
        char smallMessage[] = { "This is a small log message" };

        logger.logMessage(*category,
                          ball::Severity::e_INFO,
                          "F",
                          2,
                          smallMessage);

        if (veryVerbose) { P_(B) P(ga.numBytesInUse()) }

        ASSERTV(B, ga.numBytesInUse(), B == ga.numBytesInUse());

        // Same, but with a big message.  Such a big message will result in
        // additional memory allocation.
        char bigMessage[1024];
        bsl::memset(bigMessage, 'X', sizeof (bigMessage) - 1);
        bigMessage[1023] = 0;

        // When a log record with big message is returned to the pool, the
        // record is cleaned up deallocating memory consumed by the message.
        logger.logMessage(*category,
                          ball::Severity::e_INFO,
                          "F",
                          3,
                          bigMessage);

        // We must see that after logging a log record with big message memory
        // usage has dropped ( the message memory was de-allocated when the
        // log record is returned to the record pool).
        if (veryVerbose) { P_(B) P(ga.numBytesInUse()) }

        ASSERTV(B, ga.numBytesInUse(), B > ga.numBytesInUse());
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING SINGLETON REINITIALIZATION
        //
        // Concerns:
        //: 1 The logger manager singleton can be reinitialized by successively
        //:   creating and destroying 'ball::LoggerManagerScopedGuard' objects.
        //:
        //: 2 The logger manager singleton can be reinitialized by successive
        //:   calls to 'initSingleton' and 'shutDownSingleton'.
        //:
        //: 3 'ball::LoggerManagerScopedGuard', 'initSingleton', and
        //:   'shutDownSingleton' are thread safe.
        //
        // Plan:
        //: 1 In a loop, create a 'ball::LoggerManagerScopedGuard' object,
        //:   letting it go out of scope at the end of each iteration.  Log a
        //:   message to the singleton while the guard exists.  Also log a
        //:   'bsls::Log' message both before and after the guard has been
        //:   constructed.  Verify expected logging behavior.  (C-1)
        //:
        //: 2 In a loop, successively call 'initSingleton' and
        //:   'shutDownSingleton'.  Log a message to the singleton when it
        //:   exists.  Also log a 'bsls::Log' message both before and after
        //:   each call to 'initSingleton'.  Verify expected logging behavior.
        //:   (C-2)
        //:
        //: 3 Create three threads, one that calls 'initSingleton' in a loop,
        //:   one that calls 'shutDownSingleton' in a loop, and one that
        //:   creates a 'ball::LoggerManagerScopedGuard' object in a loop.
        //:   If the singleton exists after the three threads are joined, log a
        //:   message to the singleton and also log a 'bsls::Log' message.  If
        //:   the singleton does not exist after joining the threads, create it
        //:   before trying to log.  In either case, verify expected logging
        //:   behavior.  (C-3)
        //
        // Testing:
        //   SINGLETON REINITIALIZATION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SINGLETON REINITIALIZATION" << endl
                          << "==================================" << endl;

        using namespace BALL_LOGGERMANAGER_SINGLETON_REINITIALIZATION;

        bsls::Log::setLogMessageHandler(&countingBslsLogMessageHandler);

        bslma::TestAllocator da("default", veryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&da);
        bslma::Default::setGlobalAllocator(&ga);

        if (verbose)
            cout << "Create 'ball::LoggerManagerScopedGuard' in a loop."
                 << endl;
        {
            ball::LoggerManagerConfiguration lmc(&oa);
            ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(
                                                       0,
                                                       ball::Severity::e_TRACE,
                                                       0,
                                                       0));

            bsl::shared_ptr<ball::TestObserver> sTO =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

                  ball::TestObserver& mTO = *sTO;
            const ball::TestObserver& TO = mTO;

            ASSERT(0 == TO.numPublishedRecords());

            for (int i = 0; i < 5; ++i) {
                if (veryVerbose) { cout << "\titeration: "; P(i) }

                ASSERT(!Obj::isInitialized());
                bslsLogMsgCount = 0;
                logMessageTest(false, TO, TO.numPublishedRecords());

                {
                     ball::LoggerManagerScopedGuard guard(lmc);

                     ASSERT(Obj::isInitialized());
                     Obj::singleton().registerObserver(sTO, "TO");
                     bslsLogMsgCount = 0;
                     logMessageTest(true, TO, TO.numPublishedRecords());
                }
            }
        }
        ASSERT(!Obj::isInitialized());

        if (verbose)
            cout << "Call 'initSingleton' and 'shutDownSingleton' in a loop."
                 << endl;
        {
            ball::LoggerManagerConfiguration lmc(&oa);
            ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(
                                                       0,
                                                       ball::Severity::e_TRACE,
                                                       0,
                                                       0));

            bsl::shared_ptr<ball::TestObserver> sTO =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

                  ball::TestObserver& mTO = *sTO;
            const ball::TestObserver& TO = mTO;

            ASSERT(0 == TO.numPublishedRecords());

            for (int i = 0; i < 5; ++i) {
                if (veryVerbose) { cout << "\titeration: "; P(i) }

                ASSERT(!Obj::isInitialized());
                bslsLogMsgCount = 0;
                logMessageTest(false, TO, TO.numPublishedRecords());

                {
                     ball::LoggerManager::initSingleton(lmc);

                     ASSERT(Obj::isInitialized());
                     Obj::singleton().registerObserver(sTO, "TO");
                     bslsLogMsgCount = 0;
                     logMessageTest(true, TO, TO.numPublishedRecords());

                     ball::LoggerManager::shutDownSingleton();
                }
            }
        }
        ASSERT(!Obj::isInitialized());

        if (verbose)
            cout << "Test nested scoped guards (which is a bit funky)."
                 << endl;
        {
            ball::LoggerManagerConfiguration lmc(&oa);
            ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(
                                                       0,
                                                       ball::Severity::e_TRACE,
                                                       0,
                                                       0));

            bsl::shared_ptr<ball::TestObserver> sTO =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

                  ball::TestObserver& mTO = *sTO;
            const ball::TestObserver& TO = mTO;

            ASSERT(0 == TO.numPublishedRecords());

            {
                ASSERT(!Obj::isInitialized());
                bslsLogMsgCount = 0;
                logMessageTest(false, TO, TO.numPublishedRecords());

                ball::LoggerManagerScopedGuard guard(lmc);

                ASSERT( Obj::isInitialized());
                Obj::singleton().registerObserver(sTO, "TO");

                bslsLogMsgCount = 0;
                logMessageTest(true, TO, TO.numPublishedRecords());
                {
                     ball::LoggerManagerScopedGuard guard(lmc);

                     ASSERT(Obj::isInitialized());
                     bslsLogMsgCount = 0;
                     logMessageTest(true, TO, TO.numPublishedRecords());
                }
                ASSERT(!Obj::isInitialized());
                bslsLogMsgCount = 0;
                logMessageTest(false, TO, TO.numPublishedRecords());
            }
            ASSERT(!Obj::isInitialized());
            bslsLogMsgCount = 0;
            logMessageTest(false, TO, TO.numPublishedRecords());
        }
        ASSERT(!Obj::isInitialized());

        if (verbose) cout << "Test thread safety." << endl;
        {
            ball::LoggerManagerConfiguration lmc(&oa);
            ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(
                                                       0,
                                                       ball::Severity::e_TRACE,
                                                       0,
                                                       0));

            bsl::shared_ptr<ball::TestObserver> sTO =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

                  ball::TestObserver& mTO = *sTO;
            const ball::TestObserver& TO = mTO;

            ThreadArgs threadArgs = { &mTO, &lmc };

            for (int i = 0; i < 7; ++i) {
                if (veryVerbose) { cout << "\titeration: "; P(i) }

                bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

                bslmt::ThreadUtil::create(
                                        &threads[0],
                                        initSingletonThread,
                                        reinterpret_cast<void *>(&threadArgs));

                bslmt::ThreadUtil::create(&threads[1],
                                          shutDownSingletonThread,
                                          (void *)0);

                bslmt::ThreadUtil::create(
                                        &threads[2],
                                        scopedGuardThread,
                                        reinterpret_cast<void *>(&threadArgs));

                for (int j = 0; j < k_NUM_THREADS; ++j) {
                    bslmt::ThreadUtil::join(threads[j], 0);
                }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                if (!Obj::isInitialized()) {
                    Obj::initSingleton(&mTO, lmc);
                }
#else
                if (!Obj::isInitialized()) {
                    Obj::initSingleton(lmc);
                }
                Obj::singleton().registerObserver(sTO, "TO");
#endif

                bslsLogMsgCount = 0;
                logMessageTest(true, TO, TO.numPublishedRecords());

                Obj::shutDownSingleton();
            }

        }
        ASSERT(!Obj::isInitialized());

      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING 'numRecordsInUse' METHOD
        //   Ensure that the method correctly returns the number of
        //   outstanding messages.
        //
        // Concerns:
        //: 1 The method returns 0 if no records have been obtained through a
        //:   call to 'getRecord'.
        //:
        //: 2 The number of records reported increases by 1 for every call to
        //:   'getRecord', and decreases by 1 for every call to 'logRecord'.
        //
        // Plan:
        //: 1 Obtain several records using 'getRecord', calling
        //:   'numRecordsInUse' to verify the number of records increases as
        //:   expected, then call 'logMessage' and verify that
        //:   'numRecordsInUse' decreases as expected.  (C-1,2)
        //
        // Testing:
        //   int numRecordsInUse() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'numRecordsInUse' METHOD" << endl
                          << "================================" << endl;

        ball::LoggerManagerConfiguration mXC;
        ball::LoggerManager::initSingleton(mXC);

        ball::LoggerManager& mX = ball::LoggerManager::singleton();

        ball::Category& category = mX.defaultCategory();

        if (verbose) cout << "Testing obtaining and returning several records."
                          << endl;
        {
            ASSERT(0 == mX.getLogger().numRecordsInUse());

            ball::Record *r1 = mX.getLogger().getRecord("X", 1);

            ASSERT(1 == mX.getLogger().numRecordsInUse());

            ball::Record *r2 = mX.getLogger().getRecord("X", 1);

            ASSERT(2 == mX.getLogger().numRecordsInUse());

            ball::Record *r3 = mX.getLogger().getRecord("X", 1);

            ASSERT(3 == mX.getLogger().numRecordsInUse());

            mX.getLogger().logMessage(category, 1, r1);

            ASSERT(2 == mX.getLogger().numRecordsInUse());

            mX.getLogger().logMessage(category, 1, r2);

            ASSERT(1 == mX.getLogger().numRecordsInUse());

            mX.getLogger().logMessage(category, 1, r3);

            ASSERT(0 == mX.getLogger().numRecordsInUse());
        }

        ball::LoggerManager::shutDownSingleton();

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING EXTERNAL 'initSingleton' W/O OWNERSHIP
        //
        // Concerns:
        //: 1 When the 'adoptSingleton' flag is 'false':
        //:
        //:   1 The supplied logger manager is used as the singleton only if
        //:     the singleton does not already exist.
        //:
        //:   2 'shutDownSingleton' does not delete the supplied logger manager
        //:     in any case.
        //:
        //: 2 The method returns the expected status value.
        //:
        //: 3 The singleton is reinitializable using the method under test.
        //
        // Plan:
        //: 1 Create a logger manager using a custom allocator that will notice
        //:   whether it is deleted.  Supply the logger manager to the method:
        //:   (1) when the singleton already exists, and (2) when the singleton
        //:   does not already exist.  The 'adoptSingleton' flag is 'false' in
        //:   both cases.  Verify that the logger manager is used only in the
        //:   second case.  Call 'shutDownSingleton' and verify that in neither
        //:   case is the supplied logger manager deleted.  (C-1..2)
        //:
        //: 2 Repeat the second test from P-1.  (C-3)
        //
        // Testing:
        //   static int initSingleton(LoggerManager *, false);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING EXTERNAL 'initSingleton' W/O OWNERSHIP"
                 << "\n==============================================\n";

        using namespace BALL_LOGGERMANAGER_WINDOWS_SPECIFIC_INITSINGLETON;

        if (verbose) cout << "\tSingleton already exists." << endl;
        {
            const ball::LoggerManagerConfiguration XC;
            ball::LoggerManagerScopedGuard         lmGuard(XC);

            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.ptr();
            ASSERT(0 != ball::LoggerManager::initSingleton(p, false));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() != p);  // not used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(!ta.d_deleted);                           // not deleted
        }

        if (verbose) cout << "\tSingleton does not already exist." << endl;
        {
            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.ptr();
            ASSERT(0 == ball::LoggerManager::initSingleton(p));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() == p);  // used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(!ta.d_deleted);                           // not deleted
        }

        if (verbose) cout << "\t\tRepeat." << endl;
        {
            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.ptr();
            ASSERT(0 == ball::LoggerManager::initSingleton(p));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() == p);  // used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(!ta.d_deleted);                           // not deleted
        }

      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING EXTERNAL 'initSingleton' W/OWNERSHIP
        //
        // Concerns:
        //: 1 When the 'adoptSingleton' flag is 'true':
        //:
        //:   1 The supplied logger manager is used as the singleton only if
        //:     the singleton does not already exist.
        //:
        //:   2 'shutDownSingleton' deletes the supplied logger manager only if
        //:     it is used.
        //:
        //: 2 The method returns the expected status value.
        //:
        //: 3 The singleton is reinitializable using the method under test.
        //
        // Plan:
        //: 1 Create a logger manager using a custom allocator that will notice
        //:   whether it is deleted.  Supply the logger manager to the method:
        //:   (1) when the singleton already exists, and (2) when the singleton
        //:   does not already exist.  The 'adoptSingleton' flag is 'true' in
        //:   both cases.  Verify that the logger manager is used only in the
        //:   second case.  Call 'shutDownSingleton' and verify that the
        //:   supplied logger manager is deleted only if it is used.  (C-1..2)
        //:
        //: 2 Repeat the second test from P-1.  (C-3)
        //
        // Testing:
        //   static int initSingleton(LoggerManager *, true);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING EXTERNAL 'initSingleton' W/OWNERSHIP"
                 << "\n============================================\n";

        using namespace BALL_LOGGERMANAGER_WINDOWS_SPECIFIC_INITSINGLETON;

        if (verbose) cout << "\tSingleton already exists." << endl;
        {
            const ball::LoggerManagerConfiguration XC;
            ball::LoggerManagerScopedGuard         lmGuard(XC);

            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.ptr();
            ASSERT(0 != ball::LoggerManager::initSingleton(p, true));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() != p);  // not used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(!ta.d_deleted);                           // not deleted
        }

        if (verbose) cout << "\tSingleton does not already exist." << endl;
        {
            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.release().first;
            ASSERT(0 == ball::LoggerManager::initSingleton(p, true));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() == p);  // used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(ta.d_deleted);                            // deleted
        }

        if (verbose) cout << "\t\tRepeat." << endl;
        {
            TrackingAllocator ta;
            ball::StreamObserver obs(&cout);
            ball::LoggerManagerConfiguration cfg;
            bslma::ManagedPtr<ball::LoggerManager> mp;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManager::createLoggerManager(&mp, &obs, cfg, &ta);
#else
            (void)obs;
            ball::LoggerManager::createLoggerManager(&mp, cfg, &ta);
#endif
            ball::LoggerManager *p = mp.release().first;
            ASSERT(0 == ball::LoggerManager::initSingleton(p, true));
            ASSERT(ta.d_tracked_pointer_p            == p);
            ASSERT(&ball::LoggerManager::singleton() == p);  // used
            ball::LoggerManager::shutDownSingleton();
            ASSERT(ta.d_deleted);                            // deleted
        }

      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'setDefaultThresholdLevelsCallback'
        //   Verify Concurrent access to 'setDefaultThresholdLevelsCallback'.
        //
        // Concerns:
        // That multiple threads can safely invoke
        // 'setDefaultThresholdLevelsCallback' with the same callback
        // argument.
        //
        // Plan:
        // Create several threads, each of which invoke
        // 'setDefaultThresholdLevelsCallback' with the same callback argument.
        //
        // Testing:
        //   void setDefaultThresholdLevelsCallback(Dtc *cb);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl <<
             "TESTING CONCURRENT ACCESS TO 'setDefaultThresholdLevelsCallback'"
                 << endl <<
             "================================================================"
                 << endl;
        }

        using namespace BALL_LOGGERMANAGER_TEST_DEFAULTTHRESHOLDLEVELSCALLBACK;

        ball::LoggerManagerConfiguration mXC;
        ball::LoggerManagerScopedGuard   lmGuard(mXC);

        manager = &Obj::singleton();
        functionDTLC = &callback;

        manager->setDefaultThresholdLevelsCallback(&functionDTLC);

        bslmt::ThreadUtil::Handle handle;
        bslmt::ThreadUtil::create(&handle, setCategory, 0);

        executeInParallel(k_NUM_THREADS, workerThreadDTLC);

        bslmt::ThreadUtil::join(handle);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING '~LoggerManager' CALLS 'Observer::releaseRecords'
        //
        // Concerns:
        //: 1 A logger manager invokes 'releaseRecords' of its registered
        //:   observer when the logger manager is being destroyed.
        //
        // Plan:
        //: 1 Create a 'TestDestroyObserver' objects that has a count of its
        //:   'releaseRecords' called.
        //:
        //: 2 Install the observer to a logger manager object with limited life
        //:   time.
        //:
        //: 3 Destroy the logger manager.
        //:
        //: 4 Verify that the count in the observers increases by one.
        //
        // Testing:
        //   This test exercises '~ball::LoggerManager()', but does not test
        //   that function.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING '~LoggerManager' CALLS 'Observer::releaseRecords'"
                 << endl
                 << "========================================================="
                 << endl;
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        TestDestroyObserver                  observer1;
#endif
        bsl::shared_ptr<TestDestroyObserver> observer2(
                                                    new TestDestroyObserver());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERT(0 == observer1.getReleaseCnt());
#endif
        ASSERT(0 == observer2->getReleaseCnt());
        {
            ball::LoggerManagerConfiguration mXC;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManagerScopedGuard lmGuard(&observer1, mXC);
#else
            ball::LoggerManagerScopedGuard lmGuard(mXC);
#endif

            Obj::singleton().registerObserver(observer2, "observer2");
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERTV(observer1.getReleaseCnt(),  1 == observer1.getReleaseCnt());
#endif
        ASSERTV(observer2->getReleaseCnt(), 1 == observer2->getReleaseCnt());

      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING 'Logger::logMessage' (RULE BASED LOGGING)
        //
        // Concerns:
        //: 1 Verify that 'logMessage' uses the currently installed rules when
        //:   determining whether to publish a message.
        //:
        //: 2 Verify that 'logMessage' releases memory, both for records that
        //:   are published, and records that are not published.
        //
        // Plan:
        //: 1 Create a series of test threshold values and a series of
        //:   threshold-aggregate values.  Maintain, over the test, the
        //:   expected number of published records and the number of records
        //:   currently in the buffer, at any time (note that the records in
        //:   the buffered are added to the published records when a trigger
        //:   occurs).  For each threshold-aggregate value:   create a category
        //:   with that threshold-aggregate value; test the 'logMessage' method
        //:   with a variety of severity levels without a logging rule, then
        //:   for each test threshold-aggregate value, create a logging rule
        //:   that applies to the category and test the 'logMessage' method.
        //:   Compare the expected number of published records against the
        //:   actual number of published records.
        //:
        //: 2 Call 'logMessage' with severities that will, and will not,
        //:   cause a record to be published.  Use 'numRecordsInUse' to verify
        //:   the logs are released back to the pool.
        //
        // Testing:
        //   bool logMessage(const ball::Category *category,
        //                   int                  severity,
        //                   ball::Record         *record);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'Logger::logMessage' (RULE BASED LOGGING)\n"
                 << "=================================================\n";

        using namespace BloombergLP;

        int VALUES[] = { 1,
                         ball::Severity::e_FATAL - 1,
                         ball::Severity::e_FATAL,
                         ball::Severity::e_FATAL + 1,
                         ball::Severity::e_ERROR - 1,
                         ball::Severity::e_ERROR,
                         ball::Severity::e_ERROR + 1,
                         ball::Severity::e_WARN - 1,
                         ball::Severity::e_WARN,
                         ball::Severity::e_WARN + 1,
                         ball::Severity::e_INFO - 1,
                         ball::Severity::e_INFO,
                         ball::Severity::e_INFO + 1,
                         ball::Severity::e_DEBUG - 1,
                         ball::Severity::e_DEBUG,
                         ball::Severity::e_DEBUG + 1,
                         ball::Severity::e_TRACE - 1,
                         ball::Severity::e_TRACE,
                         ball::Severity::e_TRACE + 1
        };
        enum {NUM_VALUES = sizeof (VALUES) / sizeof(*VALUES) };

        bsl::vector<Thresholds> thresholds;

        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], 255, 255, 255));
            thresholds.push_back(Thresholds(255, VALUES[i], 255, 255));
            thresholds.push_back(Thresholds(255, 255, VALUES[i], 255));
            thresholds.push_back(Thresholds(255, 255, 255, VALUES[i]));
        }

        // We must keep track of the total number of published records, and
        // the number of buffered records.  The accumulated buffered records
        // will be published when logged message is at a higher severity than
        // the trigger level.
        int numPublished       = 0;
        int numBufferedRecords = 0;

        bslma::TestAllocator             ta;
        bsl::shared_ptr<ball::TestObserver> sTO =
                     bsl::allocate_shared<ball::TestObserver>(&ta, &bsl::cout);
        const ball::TestObserver        *TO = sTO.get();
        ball::LoggerManagerConfiguration mXC;
        mXC.setTriggerMarkers(
                  BloombergLP::ball::LoggerManagerConfiguration::e_NO_MARKERS);

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManager::initSingleton(sTO.get(), mXC, &ta);
#else
        ball::LoggerManager::initSingleton(mXC, &ta);
        ball::LoggerManager::singleton().registerObserver(sTO, "TO");
#endif
        ball::LoggerManager& mX = ball::LoggerManager::singleton();

        for (int i = 0; i < (int) thresholds.size(); ++i) {
            ball::Category *category =
                               mX.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                ball::Record *record = mX.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                mX.getLogger().logMessage(*category, VALUES[j], record);

                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                ASSERTV(i,j, numPublished == TO->numPublishedRecords());
            }

            // Test with rule that does not apply.
            mX.addRule(ball::Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                ball::Record *record = mX.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                mX.getLogger().logMessage(*category, VALUES[j], record);
                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                ASSERTV(i, j, numPublished == TO->numPublishedRecords());
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                mX.removeAllRules();
                ball::Rule rule("Test*",
                               thresholds[j].recordLevel(),
                               thresholds[j].passLevel(),
                               thresholds[j].triggerLevel(),
                               thresholds[j].triggerAllLevel());
                mX.addRule(rule);

                for (int k = 0; k < NUM_VALUES; ++k) {
                    ball::Record *record =
                                          mX.getLogger().getRecord("X",1);

                    // Each level is the minimum severity (maximum value)
                    // between the rule's threshold and the categories
                    // threshold.
                    int recordLvl  = bsl::max(thresholds[i].recordLevel(),
                                              thresholds[j].recordLevel());
                    int passLvl    = bsl::max(thresholds[i].passLevel(),
                                              thresholds[j].passLevel());
                    int triggerLvl = bsl::max(thresholds[i].triggerLevel(),
                                              thresholds[j].triggerLevel());
                    int trigAllLvl = bsl::max(thresholds[i].triggerAllLevel(),
                                              thresholds[j].triggerAllLevel());

                    // Figure out the various thresholds.  Note that we have
                    // only one thread, so trigger is equivalent to triggerAll.
                    bool buffer  = VALUES[k] <= recordLvl;
                    bool pass    = VALUES[k] <= passLvl;
                    bool trigger = VALUES[k] <= triggerLvl
                                || VALUES[k] <= trigAllLvl;

                    mX.getLogger().logMessage(*category,
                                                   VALUES[k],
                                                   record);

                    if (buffer) { ++numBufferedRecords; }
                    if (pass)   { ++numPublished; }
                    if (trigger) {
                        numPublished += numBufferedRecords;
                        numBufferedRecords = 0;
                    }
                    ASSERTV(i, j, k,
                            numPublished == TO->numPublishedRecords());
                }
            }
            mX.removeAllRules();
        }

        // Verify that the 3-argument 'logMessage' does not leak records when
        // records are discarded without being published.  (DRQS 64132363).

        if (verbose) cout << "Verify 'logMessage' does not leak records."
                          << endl;
        {
            ball::Category& category = mX.defaultCategory();

            ASSERT(0 == mX.getLogger().numRecordsInUse());

            ball::Record *r1 = mX.getLogger().getRecord("X", 1);

            ASSERT(1 == mX.getLogger().numRecordsInUse());

            ball::Record *r2 = mX.getLogger().getRecord("X", 1);

            ASSERT(2 == mX.getLogger().numRecordsInUse());

            mX.getLogger().logMessage(category, 1 , r1);

            ASSERT(1 == mX.getLogger().numRecordsInUse());

            mX.getLogger().logMessage(category, 254, r2);

            ASSERT(0 == mX.getLogger().numRecordsInUse());
        }

        ball::LoggerManager::shutDownSingleton();

      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'isCategoryEnabled' (RULE BASED LOGGING)
        //
        // Concerns:
        //: 1 'isCategoryEnabled' behaves correctly with respect to the rule
        //:   based logging.
        //: 2  'isCategoryEnabled' method uses the currently installed rules
        //:    when determining whether a category is enabled.
        //
        // Plan:
        //: 1 Create a series of test threshold values and a series of
        //:   threshold-aggregate values.  Test the 'isCategoryEnabled' method
        //:   for each test threshold value without a logger manager.  Then
        //:   create a logger manager.
        //: 2 For each threshold-aggregate value:
        //:   - create a category with that threshold-aggregate value
        //:   - test 'isCategoryEnabled' method with a variety of severity
        //:     levels without a logging rule
        //:   - for each test threshold-aggregate value, create a logging rule
        //:     that applies to the category
        //:   - test 'isCategoryEnabled' method.
        //:
        //: 3 Verify that 'isCategoryEnabled' method behaves correctly given
        //:   the categories threshold-aggregate, the rule's
        //:   threshold-aggregate, and the supplied severity.
        //
        // Testing:
        //   bool isCategoryEnabled(const ball::Category *, int) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'isCategoryEnabled' (RULE BASED LOGGING)" << endl
                 << "================================================" << endl;

        int VALUES[] = { 1,
                         ball::Severity::e_FATAL - 1,
                         ball::Severity::e_FATAL,
                         ball::Severity::e_FATAL + 1,
                         ball::Severity::e_ERROR - 1,
                         ball::Severity::e_ERROR,
                         ball::Severity::e_ERROR + 1,
                         ball::Severity::e_WARN - 1,
                         ball::Severity::e_WARN,
                         ball::Severity::e_WARN + 1,
                         ball::Severity::e_INFO - 1,
                         ball::Severity::e_INFO,
                         ball::Severity::e_INFO + 1,
                         ball::Severity::e_DEBUG - 1,
                         ball::Severity::e_DEBUG,
                         ball::Severity::e_DEBUG + 1,
                         ball::Severity::e_TRACE - 1,
                         ball::Severity::e_TRACE,
                         ball::Severity::e_TRACE + 1
        };
        enum { NUM_VALUES = sizeof (VALUES) / sizeof(*VALUES) };

        bsl::vector<Thresholds> thresholds;
        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], 255, 255, 255));
            thresholds.push_back(Thresholds(255, VALUES[i], 255, 255));
            thresholds.push_back(Thresholds(255, 255, VALUES[i], 255));
            thresholds.push_back(Thresholds(255, 255, 255, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tTest with and without an applicable rule."
                      << bsl::endl;
        }

        ball::LoggerManagerConfiguration mXC;
        ball::LoggerManager::initSingleton(mXC);

        Obj& mX = ball::LoggerManager::singleton();

        // Create a test category for each test threshold-aggregate
        for (int i = 0; i < (int) thresholds.size(); ++i) {
            ball::Category *category =
                               mX.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test 'isCategoryEnabled' with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                              VALUES[j] <= Thresholds::maxLevel(thresholds[i]);

                ASSERTV(i, j,
                        enabled == mX.isCategoryEnabled(category, VALUES[j]));
            }

            // Test with rule that does not apply.
            mX.addRule(ball::Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                ASSERTV(i, j,
                        enabled == mX.isCategoryEnabled(category, VALUES[j]));
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                mX.removeAllRules();
                ball::Rule rule("Test*",
                                thresholds[j].recordLevel(),
                                thresholds[j].passLevel(),
                                thresholds[j].triggerLevel(),
                                thresholds[j].triggerAllLevel());
                mX.addRule(rule);
                for (int k = 0; k < NUM_VALUES; ++k) {
                    int maxLevel =
                        bsl::max(Thresholds::maxLevel(thresholds[i]),
                                 Thresholds::maxLevel(thresholds[j]));
                    bool expectedEnabled = VALUES[k] <= maxLevel;
                    bool enabled = mX.isCategoryEnabled(category, VALUES[k]);

                    ASSERTV(i, j, k, enabled == expectedEnabled);

                    if (enabled != expectedEnabled) {
                        P_(thresholds[i]); P_(thresholds[j]); P(enabled);
                    }
                }

            }
            mX.removeAllRules();
        }
        ball::LoggerManager::shutDownSingleton();

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING LOW-LEVEL LOGGING
        //
        // Concerns:
        //: 1 Declaring the 'ball::LoggerManagerScopedGuard' results in a new
        //:   handler being installed in the 'bsls' logger
        //:
        //: 2 Writing a low-level log results in a proper log message being
        //:   sent to the 'ball::TestObserver'
        //:
        //: 3 No 'publish' message needs to be called for low-level logs to be
        //:   passed to the observer
        //:
        //: 4 Destruction of the 'ball::LoggerManagerScopedGuard' results in
        //:   the original low-level handler being installed.
        //
        // Plan:
        //: 1 Create a scoped guard
        //:
        //: 2 Confirm the low-level 'bsls' handler has been changed
        //:
        //: 3 Write a low-level log to the 'bsls' logger.
        //:
        //: 4 Confirm that the singleton's logger manager has had a new
        //:   category added.
        //:
        //: 5 Confirm that the record was passed to the observer without
        //:   requiring a use of 'publish'.  This ensures that the default
        //:   category and severity work without needing any user intervention.
        //:
        //: 6 Confirm that all the values in the passed record are the expected
        //:   values.
        //:
        //: 7 Allow the scoped guard to fall out of scope.
        //:
        //: 8 Confirm that the old low-level log message handler was properly
        //:   reinstated after the scoped guard's destruction.
        //
        // Testing:
        //   LOW-LEVEL LOGGING
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING LOW-LEVEL LOGGING" << endl
                          << "=========================" << endl;

        // Saving low-level handler
        const bsls::Log::LogMessageHandler oldBslsHandler =
                                                bsls::Log::logMessageHandler();
        {
            if (veryVerbose) cout << "\tCreate the test observer." << endl;
            bsl::shared_ptr<ball::TestObserver> observerSP =
                                   bsl::make_shared<ball::TestObserver>(&cout);
            ball::TestObserver& observer = *observerSP;

            if (veryVerbose) cout << "\tCreate scoped guard." << endl;
            ball::LoggerManagerConfiguration mXC;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManagerScopedGuard lmGuard(&observer, mXC);
#else
            ball::LoggerManagerScopedGuard lmGuard(mXC);
            ball::LoggerManager::singleton().registerObserver(observerSP,
                                                              "TO");
#endif

            Obj&       mX = ball::LoggerManager::singleton();
            const Obj& X  = mX;

            const int oldNumCategories = X.numCategories();

            if (veryVerbose)
                            cout << "\tConfirm new low-level handler." << endl;

            ASSERT(bsls::Log::logMessageHandler() != oldBslsHandler);

            if (veryVerbose)
                            cout << "\tWrite a low-level log message." << endl;

            bsls::LogSeverity::Enum severity = bsls::LogSeverity::e_ERROR;
            const char *const       file     = "TestFile.cpp";
            const int               line     = 507;
            const char *const       message  = "Some test message.";
            bsls::Log::logMessage(severity, file, line, message);

            if (veryVerbose)
                           cout << "\tConfirm a new category created." << endl;

            ASSERTV(oldNumCategories + 1,
                    X.numCategories(),
                    oldNumCategories + 1 == X.numCategories());

            if (veryVerbose) cout << "\tConfirm record passed directly.\n";

            ASSERTV(observer.numPublishedRecords(),
                    1 == observer.numPublishedRecords());

            if (veryVerbose) cout << "\tConfirm values in record." << endl;

            const ball::Record& record = observer.lastPublishedRecord();
            const ball::RecordAttributes& attributes = record.fixedFields();

            ASSERTV("BSLS.LOG", attributes.category(),
                    0 == strcmp("BSLS.LOG", attributes.category()));

            ASSERTV(ball::Severity::e_ERROR,
                    attributes.severity(),
                    ball::Severity::e_ERROR == attributes.severity());

            ASSERTV(file,
                    attributes.fileName(),
                    0 == strcmp(file, attributes.fileName()));

            ASSERTV(line,
                    attributes.lineNumber(),
                    line == attributes.lineNumber());

            ASSERTV(message,
                    attributes.message(),
                    0 == strcmp(message, attributes.message()));

            if (veryVerbose) cout << "\t\tMessage: "
                                  << attributes.message()
                                  << endl;

            if (veryVerbose) cout << "\tDestroy scoped guard." << endl;
            // Let guard fall out of this scope
        }

        if (veryVerbose) cout << "\tConfirm old handler restored." << endl;
        ASSERT(bsls::Log::logMessageHandler() == oldBslsHandler);

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'ball::LoggerManagerScopedGuard'
        //
        // Concerns:
        //: 1 We want to make sure that 'ball::LoggerManagerScopedGuard' calls
        //:   'initSingleton' on its construction and 'shutDownSingleton' on
        //:   its destruction.
        //
        // Plan:
        //: 1 We create an auxiliary class that checks the initialization
        //:   status of the logger manager singleton in both its constructor
        //:   and destructor.  Objects of this class created before the logger
        //:   manager singleton should not see the logger manager, whereas
        //:   objects created after the logger manager singleton should always
        //:   report the existence of the logger manager.
        //
        // Testing:
        //   ball::LoggerManagerScopedGuard(Obs*, const Config&, ba = 0);
        //   ~ball::LoggerManagerScopedGuard();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING 'ball::LoggerManagerScopedGuard'" << endl
                         << "========================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        class InitializationVerifier {

            bool d_isInitializedFlag;  // 'true' if logger manager is
                                       // available upon destruction

          public:
            // CREATORS
            explicit
            InitializationVerifier(bool isInitializedFlag)
            : d_isInitializedFlag(isInitializedFlag)
            {
                ASSERT(d_isInitializedFlag == Obj::isInitialized());
            }

            ~InitializationVerifier()
            {
                ASSERT(d_isInitializedFlag == Obj::isInitialized());
            }
        };

        ball::LoggerManagerConfiguration mXC;

        ASSERT(0 == da.numBlocksInUse());
        ASSERT(0 == ga.numBlocksInUse());
        {
            bsl::shared_ptr<ball::TestObserver> observerSP =
                     bsl::allocate_shared<ball::TestObserver>(&oa, &bsl::cout);
            ball::TestObserver& observer = *observerSP;

            InitializationVerifier ivPre(false);
            ASSERT(false == Obj::isInitialized());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ball::LoggerManagerScopedGuard lmGuard(&observer, mXC);
#else
            ball::LoggerManagerScopedGuard lmGuard(mXC);
            ball::LoggerManager::singleton().registerObserver(observerSP,
                                                              "TO");
#endif
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 <  ga.numBlocksInUse());

            ASSERT(true == Obj::isInitialized());
            InitializationVerifier ivPost(true);

            Obj&       mX = Obj::singleton();
            const Obj& X  = mX;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(&observer == X.observer());
#endif
            ASSERT(1         == X.numCategories());

            // add a category

            const Cat *cat = mX.addCategory("Bloomberg.Bal", 64, 48, 32, 16);
            ASSERT(cat);
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(cat == X.lookupCategory("Bloomberg.Bal"));
            ASSERT(0 == da.numBlocksInUse());

            ASSERT( 0  == bsl::strcmp("Bloomberg.Bal", cat->categoryName()));
            ASSERT(64  == cat->recordLevel());
            ASSERT(48  == cat->passLevel());
            ASSERT(32  == cat->triggerLevel());
            ASSERT(16  == cat->triggerAllLevel());

            // log a test message

            if (veryVerbose) observer.setVerbose(1);

            ball::Logger& lgr = mX.getLogger();
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(0 == observer.numPublishedRecords());
            const char *MESSAGE = "scoped guard test";
            const int   LINE    = L_ + 1;
            lgr.logMessage(*cat, cat->passLevel(), __FILE__, LINE, MESSAGE);
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(1 == observer.numPublishedRecords());
            const Rec& R = observer.lastPublishedRecord();
            if (veryVerbose) cout << R << endl;

            const Attr& A = R.fixedFields();
            ASSERT(0            == bsl::strcmp("Bloomberg.Bal", A.category()));
            ASSERT(A.severity() == cat->passLevel());
            ASSERT(0            == bsl::strcmp(__FILE__, A.fileName()));
            ASSERT(LINE         == A.lineNumber());
            ASSERT(0            == bsl::strcmp(MESSAGE, A.message()));

            const FieldValues& V = R.customFields();
            ASSERT(0 == V.length());
        }
        ASSERT(0 == ga.numBlocksInUse());

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING STATIC 'getRecord' AND 'logMessage'
        //
        // Concerns:
        //: 1 The 'getRecord' and 'logMessage' should work correctly without a
        //:   'ball::LoggerManager' instance.
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   static ball::Record *getRecord(const char *file, int line);
        //   static void logMessage(int severity, ball::Record *record);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING STATIC 'getRecord' AND 'logMessage'" << endl
                 << "===========================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        ball::Record *record1 = Obj::getRecord(F_, L_);
        ball::Record *record2 = Obj::getRecord(F_, L_);
        ball::Record *record3 = Obj::getRecord(F_, L_);
        ball::Record *record4 = Obj::getRecord(F_, L_);
        ball::Record *record5 = Obj::getRecord(F_, L_);
        ball::Record *record6 = Obj::getRecord(F_, L_);

        // The first three messages are set to strings without embedded '\0'.

        record1->fixedFields().setMessage("No Logger Manager!");
        record2->fixedFields().setMessage("No Logger Manager!");
        record3->fixedFields().setMessage("No Logger Manager!");

        // The next three messages are set to strings with embedded '\0'.
        // Note (if later more carefull check of stderr output will be
        // performed ) that messages with embedded '\0' will be truncated.

        record4->fixedFields().messageStreamBuf().pubseekpos(0);
        record4->fixedFields().messageStreamBuf().sputn(
                                               "No Logger\0 Manager\0 4!", 22);

        record5->fixedFields().messageStreamBuf().pubseekpos(0);
        record5->fixedFields().messageStreamBuf().sputn(
                                               "No Logger\0 Manager\0 5!", 22);

        record6->fixedFields().messageStreamBuf().pubseekpos(0);
        record6->fixedFields().messageStreamBuf().sputn(
                                               "No Logger\0 Manager\0 6!", 22);

#ifdef BSLS_PLATFORM_OS_UNIX
        bdls::TempDirectoryGuard tempDirGuard("ball_");
        bsl::string              fileName(tempDirGuard.getTempDirName());
        bdls::PathUtil::appendRaw(&fileName, "stderrLog");

        fflush(stderr);
        int fd = creat(fileName.c_str(), 0777);
        ASSERT(fd != -1);
        int saved_stderr_fd = dup(2);
        dup2(fd, 2);
#endif

        bsl::stringstream os;
        bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
        bsl::cerr.rdbuf(os.rdbuf());

        Obj::logMessage(ball::Severity::e_TRACE, record1);
        Obj::logMessage(ball::Severity::e_DEBUG, record2);
        Obj::logMessage(ball::Severity::e_INFO,  record3);
        Obj::logMessage(ball::Severity::e_WARN,  record4);
        Obj::logMessage(ball::Severity::e_ERROR, record5);
        Obj::logMessage(ball::Severity::e_FATAL, record6);

#ifdef BSLS_PLATFORM_OS_UNIX
        fflush(stderr);
        dup2(saved_stderr_fd, 2);

        bsl::ifstream fs(fileName.c_str(), bsl::ifstream::in);
        int           numLines = 0;
        bsl::string   line;

        while (getline(fs, line)) {
            if (veryVerbose) bsl::cout << line << bsl::endl;
            ++numLines;
        }

        fs.close();

        ASSERTV(numLines, 6 == numLines);
#endif

        ASSERT("" == os.str());
        bsl::cerr.rdbuf(cerrBuf);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING RULE MANIPULATION (TEMPORARILY LEFT BLANK)
        // --------------------------------------------------------------------
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'lookupCategory'
        //
        // Concerns:
        //: 1 'lookupCategory' is thread-safe when a category name filter is in
        //:   effect.
        //
        // Plan:
        //: 1 Initialize the logger manager with a 'toLower' category name
        //:   filter.  Add 4 categories whose names are all in lowercase.
        //:   In concurrent threads, lookup those categories using their
        //:   similar uppercase names.  Assert in the helper thread function
        //:   'workerThreadLookupCategory' that the category is found.
        //
        // Testing:
        //   const ball::Category *lookupCategory(const char *categoryName);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING CONCURRENT ACCESS TO 'lookupCategory'" << endl
                 << "=============================================" << endl;
        }

        using namespace BALL_LOGGERMANAGER_TEST_LOOKUP_CATEGORY;

        ball::LoggerManagerConfiguration mXC;
        Cnf nameFilter(&toLower);
        mXC.setCategoryNameFilterCallback(nameFilter);

        ball::LoggerManagerScopedGuard lmGuard(mXC);

        Obj& mX = Obj::singleton();

        const int NUM_DATA = sizeof LC / sizeof *LC;

        ASSERT(NUM_DATA == k_NUM_THREADS);

        int ti;
        for (ti = 0; ti < k_NUM_THREADS; ++ti) {
            if (veryVerbose) cout << LC[ti] << endl;

            ASSERT(mX.setCategory(LC[ti]));
        }

        ThreadData data[k_NUM_THREADS];

        for (ti = 0; ti < k_NUM_THREADS; ++ti) {
            data[ti].d_index = ti;
        }

        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

        for (ti = 0; ti < k_NUM_THREADS; ++ti) {
            bslmt::ThreadUtil::create(&threads[ti],
                                      workerThreadLookupCategory,
                                      (void*)&data[ti]);
        }

        for (ti = 0; ti < k_NUM_THREADS; ++ti) {
            bslmt::ThreadUtil::join(threads[ti]);
        }

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'initSingleton'
        //   Verify concurrent access to 'initSingleton'.
        //
        // Concerns:
        //: 1 Multiple threads can safely invoke 'initSingleton' with
        //:   different observers and configuration objects, and the resulting
        //:   logger manager singleton must be initialized with one of these
        //:   observers and configuration objects.
        //
        // Plan:
        //: 1 Create several threads, each of which invoke 'initSingleton' with
        //:   different observers and configuration objects.
        //
        // Testing:
        //   TESTING CONCURRENT ACCESS TO 'initSingleton'
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING CONCURRENT ACCESS TO 'initSingleton'" << endl
                 << "============================================" << endl;
        }

        using namespace BALL_LOGGERMANAGER_TEST_INIT_SINGLETON;

        ThreadData data[k_NUM_THREADS];

        int i;
        for (i = 0; i < k_NUM_THREADS; ++i) {
            data[i].d_index = i;
        }

        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

        for (i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::create(&threads[i],
                                      workerThreadInitSingleton,
                                      (void*)&data[i]);
        }

        for (i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::join(threads[i]);
        }

        Obj& mX = Obj::singleton();
        for (i = 0; i < k_NUM_THREADS; ++i) {
            ASSERTV(i, &mX == data[i].d_manager_p);
            if (mX.defaultRecordThresholdLevel() == data[i].d_index) {
                if (veryVerbose) cout << "Thread " << i << " initialized "
                                      << "the logger manager singleton"
                                      << endl;

                ASSERT(mX.defaultRecordThresholdLevel()    == data[i].d_index);
                ASSERT(mX.defaultPassThresholdLevel()      == data[i].d_index);
                ASSERT(mX.defaultTriggerThresholdLevel()   == data[i].d_index);
                ASSERT(mX.defaultTriggerAllThresholdLevel()== data[i].d_index);

                break;
            }
        }

        ASSERT(k_NUM_THREADS != i);

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'setCategory'
        //   Verify Concurrent access to 'setCategory'.
        //
        // Concerns:
        //: 1 'setCategory' with the same category argument is thread-safe.
        //
        // Plan:
        //: 1  Create several threads, each of which invoke 'setCategory' with
        //:    the same category argument.
        //
        // Testing:
        //   const ball::Category *setCategory(const char *categoryName);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING CONCURRENT ACCESS TO 'setCategory'" << endl
                 << "==========================================" << endl;
        }

        using namespace BALL_LOGGERMANAGER_CONCURRENT_TESTS;

        bsl::shared_ptr<ball::TestObserver> observerSP =
                                   bsl::make_shared<ball::TestObserver>(&cout);
        ball::TestObserver& observer = *observerSP;

        ball::LoggerManagerConfiguration mXC;

        ASSERT(false == Obj::isInitialized());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManagerScopedGuard lmGuard(&observer, mXC);
#else
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ball::LoggerManager::singleton().registerObserver(observerSP, "TO");
#endif
        ASSERT(true  == Obj::isInitialized());

        verifyLoggerManagerDefaults(Obj::singleton(), 0, &observer);

        executeInParallel(k_NUM_THREADS, workerThreadSetCategory);

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'd_defaultLoggers'
        //   Verify concurrent access to 'd_defaultLoggers' (a class member
        //   of 'ball::LoggerManager').
        //
        // Concerns:
        //: 1  Access to 'd_defaultLoggers' (which maintains the registered
        //:    loggers) is thread-safe.
        //
        // Plan:
        //: 1  'd_defaultLoggers' is primarily manipulated by 'setLogger' and
        //:    'getLogger'.  We create several threads, half of them, will
        //:    perform following operations:
        //:      - allocate a logger (say 'logger')
        //:      - run a tight loop; in each iteration, invoke
        //:        'setLogger(logger)' followed by 'getLogger'
        //:      - assert that the logger returned by 'getLogger' is same as
        //:        the 'logger'
        //:
        //:    and the other half will perform following operations:
        //:      - get the default logger by calling 'getLogger'
        //:      - run a tight loop; in each iteration, invoke 'getLogger'
        //:        and assert that it returns the default logger
        //
        // Testing:
        //   void setLogger(ball::Logger *logger);
        //   ball::Logger& getLogger();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING CONCURRENT ACCESS TO 'd_defaultLoggers'" << endl
                 << "===============================================" << endl;
        }

        using namespace BALL_LOGGERMANAGER_CONCURRENT_TESTS;

        static bslma::TestAllocator      sa("supplied", veryVeryVeryVerbose);


        bsl::shared_ptr<ball::TestObserver> observerSP =
                                   bsl::make_shared<ball::TestObserver>(&cout);
        ball::TestObserver& observer = *observerSP;

        ball::LoggerManagerConfiguration mXC;

        ASSERT(false == Obj::isInitialized());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManagerScopedGuard lmGuard(&observer, mXC, &sa);
#else
        ball::LoggerManagerScopedGuard lmGuard(mXC, &sa);
        ball::LoggerManager::singleton().registerObserver(observerSP, "TO");
#endif
        ASSERT(true  == Obj::isInitialized());

        verifyLoggerManagerDefaults(Obj::singleton(), &sa, &observer);

        executeInParallel(k_NUM_THREADS, workerThreadDefaultLoggers);

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING CONCURRENT ACCESS TO 'd_loggers'
        //   Verify concurrent access to 'd_loggers' (a class member
        //   of 'ball::LoggerManager').
        //
        // Concerns:
        //: 1 Access to 'd_loggers' (which maintains registry of allocated
        //:   loggers) is thread-safe.
        //
        // Plan:
        //: 1 'd_loggers' is primarily manipulated by 'allocateLogger',
        //:   'logMessage' (specifically, when the logged message is severe
        //:   enough to cause a trigger-all) and 'deallocateLogger'.  We
        //:   create several threads, each, in a tight loop, will perform
        //:   following operations:
        //:     - allocate a logger (say 'logger')
        //:     - log a message to 'logger', causing a trigger-all
        //:     - deallocate the 'logger'
        //:
        //: 2 Join all the threads and verify the state.
        //
        // Testing:
        //   ball::Logger *allocateLogger(ball::RecordBuffer *buffer);
        //   void deallocateLogger(ball::Logger *logger);
        //   void logMessage(const ball::Category&  category,
        //                   int                    severity,
        //                   const char            *fileName,
        //                   int                    lineNumber,
        //                   const char            *message);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING CONCURRENT ACCESS TO 'd_loggers'" << endl
                 << "========================================" << endl;
        }

        using namespace BALL_LOGGERMANAGER_CONCURRENT_TESTS;

        bslma::TestAllocator             sa("supplied", veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration mXC;

        mXC.setTriggerMarkers(
                  BloombergLP::ball::LoggerManagerConfiguration::e_NO_MARKERS);


        ASSERT(false == Obj::isInitialized());
        bsl::shared_ptr<PublishCountingObserver> observerSP =
                                   bsl::make_shared<PublishCountingObserver>();
        PublishCountingObserver& observer = *observerSP;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManagerScopedGuard lmGuard(&observer, mXC, &sa);
#else
        ball::LoggerManagerScopedGuard lmGuard(mXC, &sa);
        ball::LoggerManager::singleton().registerObserver(observerSP, "PCO");
#endif
        ASSERT(true  == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, &sa, &observer);

        // For simplicity we log the messages with severity, that will cause
        // *record* followed by *trigger-all*.

        ball::Category *cat = mX.addCategory("test-loggers",
                                             ball::Severity::e_ERROR,
                                             ball::Severity::e_FATAL,
                                             ball::Severity::e_FATAL,
                                             ball::Severity::e_ERROR);

        ASSERT(cat);

        executeInParallel(k_NUM_THREADS, workerThreadMultipleLoggers);
        int expected = observer.publishCount();

        // There are 'k_NUM_THREADS', each of which logs 'k_NUM_ITERATIONS'
        // messages.  Note that each logged message is eventually published
        // (either by a trigger-all caused by the same thread or by a different
        // thread).  Thus the publish count should be equal to
        // 'k_NUM_THREADS * k_NUM_ITERATIONS'.

        ASSERTV(expected, expected == k_NUM_THREADS * k_NUM_ITERATIONS);

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // CONCERN: PERFORMANCE IMPLICATIONS
        //
        // Concerns:
        //   'logMessage' should run efficiently when no record is stored,
        //   and relatively efficient when records need to be stored.
        //   LEGACY TEST. INCOMPLETE(?)
        //
        // Plan:
        //   Invoke 'logMessage' in a loop for the default number of times
        //   or else for an integral multiple thereof as specified in the
        //   second argument ('argv[2]') to the executable.
        //
        // Testing:
        //   CONCERN: PERFORMANCE IMPLICATIONS
        // --------------------------------------------------------------------

        Cnf nameFilter(&toLower);

        Obj::DefaultThresholdLevelsCallback
                thresholdsCallback(&inheritThresholdLevels);

        Obj::UserFieldsPopulatorCallback populator(&myUserFieldsPopulator);

        ball::LoggerManagerConfiguration mXC;
        mXC.setCategoryNameFilterCallback(nameFilter);
        mXC.setDefaultThresholdLevelsCallback(thresholdsCallback);
        mXC.setUserFieldsPopulatorCallback(populator);

        ASSERT(false == Obj::isInitialized());
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(true  == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, 0, 0);

        ball::Logger& lgr = mX.getLogger();

        Cat *cat = mX.addCategory("PerfTest", 200, 150, 100, 50);

        const char *currentFile = __FILE__;

        // Add some categories
        const int DEFAULT = 0;   // keep default standard
        int SIZE = verbose && atoi(argv[2]) > DEFAULT
                   ? atoi(argv[2])
                   : DEFAULT;

        if (verbose) cout << "SIZE = " << SIZE << endl;

        //                      vvvv Adjust Here to change default runtime.
        const int size = SIZE * 200;

        const int NUM_FEEDBACKS = 50;
        int feedback = size / NUM_FEEDBACKS;
        if (feedback <= 0) feedback = 1;

        if (verbose) cout << "Performance test for 'logMessage' "
                          << size << " times. " << endl;
        {
            if (verbose) cerr << "     Baseline -- simple calculations only:"
                              << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            int m = 29;         // a randomly picked prime number

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                m = (m * i + m) % INT_MAX;
            }
            if (verbose) cerr << "END" << endl;

            if (verbose) cerr << "     Less severe than record:" << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                lgr.logMessage(*cat, 250, currentFile, __LINE__, "Message");
            }
            if (verbose) cerr << "END" << endl;

            if (verbose) cerr << "     Less severe than pass but more severe"
                              << " than record:" << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                lgr.logMessage(*cat, 180, currentFile, __LINE__, "Message");
            }
            if (verbose) cerr << "END" << endl;

            if (verbose) cerr << "     Less severe than trigger but more"
                              << " severe than pass:" << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                lgr.logMessage(*cat, 120, currentFile, __LINE__, "Message");
            }
            if (verbose) cerr << "END" << endl;

            if (verbose) cerr << "     Less severe than trigger all"
                              << " but more severe than trigger:" << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                lgr.logMessage(*cat, 70, currentFile, __LINE__, "Message");
            }
            if (verbose) cerr << "END" << endl;

            if (verbose) cerr << "     more severe than trigger all:" << endl;
            if (verbose) cerr << "     "
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (verbose) cerr << "BEGIN";

            for (int i = size - 1; i >= 0; --i) {
                if (verbose && 0 == i % feedback) cerr << '.';
                lgr.logMessage(*cat, 20, currentFile, __LINE__, "Message");
            }
            if (verbose) cerr << "END" << endl;
        }

      } break;
      case 18: {
        // -------------------------------------------------------------------
        // TESTING LOGGER ALLOCATION
        //
        // Concerns:
        //   - Default and user-supplied message buffer sizes are correct
        //   - Different loggers can be "set" and subsequently retrieved
        //   - Deallocated loggers cannot be set
        //
        // Plan:
        //   Allocate a set S of loggers having different message buffer
        //   sizes.
        //   For each s in S, verify that the logger has the expected buffer
        //   size, and can be "set" as the active logger.
        //
        // Testing:
        //   ball::Logger *allocateLogger(*buffer)
        //   ball::Logger *allocateLogger(*buffer, int msgBufSize);
        //   ball::Logger *allocateLogger(*buffer, *observer);
        //   ball::Logger *allocateLogger(*buffer, int msgBufSize, *observer);
        //   void deallocateLogger(ball::Logger *logger);
        //   void setLogger(ball::Logger *logger);
        // -------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING LOGGER ALLOCATION" << endl
                                  << "=========================" << endl;

        bslma::TestAllocator  oa("object", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard oag(&oa);

        bslma::Allocator     *Z = &oa;

        ball::LoggerManagerConfiguration mXC;

        ASSERT(false == Obj::isInitialized());
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(true  == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, 0, 0);

        ASSERT(1 == mX.numCategories());

        if (veryVerbose) cout << "\tTest allocateLogger(*buffer)" << endl;
        {
            Logger *logger[NUM_LOGGERS];
            RecBuf *recBuf[NUM_LOGGERS];

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                recBuf[i] = new(*Z) ball::FixedSizeRecordBuffer(BUF_SIZE);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa)
                    logger[i] = mX.allocateLogger(recBuf[i]);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                mX.setLogger(logger[i]);
                ASSERT(&mX.getLogger() == logger[i]);
            }

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                mX.deallocateLogger(logger[i]);
                recBuf[i]->~RecBuf();
                Z->deallocate(recBuf[i]);
            }
        }

        if (veryVerbose)
            cout << "\tTest allocateLogger(*buffer, int msgBufSize)" << endl;
        {
            const int K = 1024;

            Logger *logger[NUM_LOGGERS];
            RecBuf *recBuf[NUM_LOGGERS];

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                recBuf[i] = new(*Z) ball::FixedSizeRecordBuffer(BUF_SIZE);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa)
                    logger[i] = mX.allocateLogger(recBuf[i], (i + 1) * K);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERT((i + 1) * K == logger[i]->messageBufferSize());
                mX.setLogger(logger[i]);
                ASSERT(&mX.getLogger() == logger[i]);
            }

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                mX.deallocateLogger(logger[i]);
                recBuf[i]->~RecBuf();
                Z->deallocate(recBuf[i]);
            }
        }

        if (veryVerbose)
            cout << "\tTest allocateLogger(*buffer, *observer)" << endl;
        {
            Logger *logger[NUM_LOGGERS];
            RecBuf *recBuf[NUM_LOGGERS];

            bsl::shared_ptr<ball::TestObserver> observer =
                                   bsl::make_shared<ball::TestObserver>(&cout);

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                recBuf[i] = new(*Z) ball::FixedSizeRecordBuffer(BUF_SIZE);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa)
                    logger[i] = mX.allocateLogger(recBuf[i], observer);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                mX.setLogger(logger[i]);
                ASSERT(&mX.getLogger() == logger[i]);
            }

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                mX.deallocateLogger(logger[i]);
                recBuf[i]->~RecBuf();
                Z->deallocate(recBuf[i]);
            }
        }

        if (veryVerbose)
            cout << "\tTest allocateLogger(*buffer, "
                 << "int msgBufSize, *observer)" << endl;
        {
            const int K = 1024;

            Logger *logger[NUM_LOGGERS];
            RecBuf *recBuf[NUM_LOGGERS];

            bsl::shared_ptr<ball::TestObserver> observer =
                                   bsl::make_shared<ball::TestObserver>(&cout);

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                recBuf[i] = new(*Z) ball::FixedSizeRecordBuffer(BUF_SIZE);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa)
                    logger[i] = mX.allocateLogger(recBuf[i],
                                                  (i + 1) * K,
                                                  observer);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERT((i + 1) * K == logger[i]->messageBufferSize());
                mX.setLogger(logger[i]);
                ASSERT(&mX.getLogger() == logger[i]);
            }

            for (int i = 0; i < NUM_LOGGERS; ++i) {
                mX.deallocateLogger(logger[i]);
                recBuf[i]->~RecBuf();
                Z->deallocate(recBuf[i]);
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING LOGMESSAGE LOG ORDER AND TRIGGER MARKERS
        //
        // Concerns:
        //   We want to ensure that log records are published in the proper
        //   order, and with the proper trigger markers as specified by the
        //   'logOrder' and 'triggerMarkers' settings of the specified
        //   'loggerManagerConfiguration'.
        //
        // Plan:
        //   Create a category with decreasing threshold levels.  Log messages
        //   with severities less than the trigger threshold.  Then log
        //   a message above the trigger threshold and verify:
        //
        //   1) If the 'loggerManagerConfiguration' is left at default, 'LIFO'
        //      ordering of logs is used, and no trigger markers are created.
        //   2) If 'LIFO' or 'FIFO' is specified in the
        //      'loggerManagerConfiguration', the respective ordering of logs
        //      is respected.
        //   3) If 'NO_MARKERS' or 'BEGIN_END_MARKERS' is specified, the
        //      respective markers (either none, or "BEGIN" / "END" pair) are
        //      properly created.
        //
        // Testing:
        //  ^ball::Logger(Obs*, *buffer, Sch*, Pop&, Pac&, *ba);
        //  ^~ball::Logger();
        //  ^void publish(ball::Transmission::Cause cause);
        //   void logMessage(category, severity, file, line, msg);
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING LOGMESSAGE LOG ORDER AND TRIGGER MARKERS" << endl
                 << "================================================" << endl;

        enum Level {
            TRIGGERALL =  32,
            TRIGGER    =  64,
            PASS       =  96,
            RECORD     = 128
        };

        typedef ball::LoggerManagerConfiguration lmc;

        static const lmc::LogOrder LOGORDER[3] = {
            lmc::e_LIFO,  // default
            lmc::e_FIFO,
            lmc::e_LIFO
        };
        enum { NUM_LOGORDERS = sizeof LOGORDER / sizeof *LOGORDER };

        static const lmc::TriggerMarkers TRIGGERMARKERS[3] = {
            lmc::e_BEGIN_END_MARKERS,         // default
            lmc::e_BEGIN_END_MARKERS,
            lmc::e_NO_MARKERS
        };
        enum { NUM_TRIGGERMARKERS = sizeof TRIGGERMARKERS /
                                                      sizeof *TRIGGERMARKERS };

        static const struct {
            const char *message;
            Level       lvl;
        } DATA[] = {
            {"RECORD1",  RECORD  },
            {"PASS1",    PASS    },
            {"PASS2",    PASS    },
            {"RECORD2",  RECORD  },
            {"PASS3",    PASS    },
            {"TRIGGER1", TRIGGER },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int expectedNumPublished = NUM_DATA;
        for (int i = 0; i < NUM_DATA; ++i) {
            if (DATA[i].lvl != RECORD) {
                ++expectedNumPublished;
            }
        }

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        for (int i = 0; i < NUM_LOGORDERS; ++i) {
            for (int j = 0; j < NUM_TRIGGERMARKERS; ++j) {

                if (veryVerbose) {
                    P_(i); P(j);
                }

                bsl::stringstream outStream;
                using BALL_LOGGERMANAGER_TEST_CASE_17::MyObserver;

                bsl::shared_ptr<MyObserver> testObserverSP(
                                                    new MyObserver(outStream));
                MyObserver& testObserver = *testObserverSP;

                ball::LoggerManagerConfiguration mXC;

                if (i) {
                    mXC.setLogOrder(LOGORDER[i]);
                }
                if (j) {
                    mXC.setTriggerMarkers(TRIGGERMARKERS[j]);
                }

                const int k_MAX_LIMIT = 1000000;
                mXC.setDefaultRecordBufferSizeIfValid(k_MAX_LIMIT);

                bslma::ManagedPtr<Obj> objPtr;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                Obj::createLoggerManager(&objPtr, &testObserver, mXC, &oa);
#else
                Obj::createLoggerManager(&objPtr, mXC, &oa);
                objPtr->registerObserver(testObserverSP, "TO");
#endif

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Set the default threshold.
                mX.setDefaultThresholdLevels(RECORD,
                                             PASS,
                                             TRIGGER,
                                             TRIGGERALL);

                mX.setCategoryThresholdsToCurrentDefaults(
                                                      &(mX.defaultCategory()));
                const Cat& defaultCat = X.defaultCategory();

                // Verify default settings for threshold.
                ASSERT(RECORD     == defaultCat.recordLevel());
                ASSERT(PASS       == defaultCat.passLevel());
                ASSERT(TRIGGER    == defaultCat.triggerLevel());
                ASSERT(TRIGGERALL == defaultCat.triggerAllLevel());

                ball::Logger& LGR = mX.getLogger();

                const int BUF_CAP = testObserver.publishCount();
                int publishCount = BUF_CAP;  // # of records published so far

                if (veryVerbose) {
                    P(publishCount);
                }

                // Generate some log messages.
                int k = 0;
                for (; k < NUM_DATA - 1; ++k) {
                    LGR.logMessage(defaultCat,
                                   DATA[k].lvl,
                                   F_,
                                   L_,
                                   DATA[k].message);
                }

                // Now log a message above trigger threshold.
                LGR.logMessage(defaultCat,
                               DATA[k].lvl,
                               F_,
                               L_,
                               DATA[k].message);
                publishCount += expectedNumPublished;

                if (TRIGGERMARKERS[j] == lmc::e_BEGIN_END_MARKERS) {
                    publishCount += 2;  // 2 for the markers
                }

                if (veryVerbose) {
                    P_(publishCount); P(testObserver.publishCount());
                }
                ASSERT(publishCount == testObserver.publishCount());

                // Construct the expected stream
                stringstream ss;

                // Regular messages published
                for (k = 0; k < NUM_DATA; ++k) {
                    if (DATA[k].lvl != RECORD) {
                        ss << "Log 1 of 1 : " << DATA[k].message << "\n";
                    }
                }

                // Trigger markers if necessary.
                if (TRIGGERMARKERS[j] == lmc::e_BEGIN_END_MARKERS) {
                    ss << "Log 1 of 1 : "
                          "--- BEGIN RECORD DUMP CAUSED BY TRIGGER ---\n";
                }

                // Trigger dump.
                if (LOGORDER[i] == lmc::e_LIFO) {
                    for (k = NUM_DATA - 1; k >= 0; --k) {
                        ss << "Log " << NUM_DATA - k << " of " << NUM_DATA
                           << " : " << DATA[k].message << "\n";
                    }
                }
                else {
                    for (k = 0; k < NUM_DATA; ++k) {
                        ss << "Log " << k + 1 << " of " << NUM_DATA
                           << " : " << DATA[k].message << "\n";
                    }
                }

                // Trigger markers if necessary.
                if (TRIGGERMARKERS[j] == lmc::e_BEGIN_END_MARKERS) {
                    ss << "Log 1 of 1 : "
                          "--- END RECORD DUMP CAUSED BY TRIGGER ---\n";
                }

                if (veryVerbose) {
                    P(ss.str());
                    P(outStream.str());
                }

                ASSERT(ss.str() == outStream.str());
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING LOGGER INTERFACE
        //
        // Concerns:
        //   We want to ensure that log records are properly stored in the
        //   buffer when necessary, and are published when triggered.
        //   LEGACY TEST.
        //
        // Plan:
        //   Create a category with decreasing threshold levels.  Log messages
        //   with severities less than record threshold, verify they are not
        //   stored.  Log messages with severities between record and pass
        //   thresholds, verify they are stored but not published.  Log
        //   messages with severities between pass and trigger thresholds,
        //   verify they are stored and also published.  Trigger and trigger
        //   all are verified in the process of verifying records are stored
        //   properly in the above tests.  Change the order of the threshold
        //   levels and verify the behavior is correct according to the
        //   threshold level values.
        //
        // Testing:
        //  ^ball::Logger(Obs*, *buffer, Sch*, Pop&, Pac&, *ba);
        //  ^~ball::Logger();
        //  ^void publish(ball::Transmission::Cause cause);
        //   void logMessage(const Cat&, int, const char*, int, const char*);
        //   void publish();
        //   void removeAll();
        //   char *messageBuffer();
        //   int messageBufferSize() const;
        //
        //  ^void publishAllImp(ball::Transmission::Cause cause);
        //   ball::Logger& getLogger();
        //   void publishAll();
        //
        //   Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING LOGGER INTERFACE" << endl
                                  << "========================" << endl;

        const char *testNames[] = {
            "Singleton\n---------",
            "Non-Singleton\n-------------"
        };

        for (int a = 0; a < 2; ++a) {
            if (veryVerbose) cout << testNames[a] << endl;

            bsl::shared_ptr<ball::TestObserver> observerSP =
                                   bsl::make_shared<ball::TestObserver>(&cout);
            ball::TestObserver& observer = *observerSP;

                Cnf nameFilter(&toLower);

            Obj::UserFieldsPopulatorCallback populator(&myUserFieldsPopulator);

            ball::LoggerManagerConfiguration mXC;
            mXC.setTriggerMarkers(
                               ball::LoggerManagerConfiguration::e_NO_MARKERS);

            const int k_MAX_LIMIT = 1000000;
            mXC.setDefaultRecordBufferSizeIfValid(k_MAX_LIMIT);

            mXC.setCategoryNameFilterCallback(nameFilter);
            mXC.setUserFieldsPopulatorCallback(populator);

            Obj                    *mX_p;
            bslma::ManagedPtr<Obj>  mX_mp;

            Obj::DefaultThresholdLevelsCallback thresholdsCallback;
            if (0 == a) {
                thresholdsCallback = &inheritThresholdLevels;

                mXC.setDefaultThresholdLevelsCallback(thresholdsCallback);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                mX_p = &(Obj::initSingleton(&observer, mXC));
#else
                mX_p = &(Obj::initSingleton(mXC));
                Obj::singleton().registerObserver(observerSP, "TO");
#endif
            }
            else {
                thresholdsCallback =
                    bdlf::BindUtil::bind(&inheritThresholdLevelsUsingManager
                                       , _1
                                       , _2
                                       , _3
                                       , _4
                                       , _5
                                       , (Obj*)0);

                mXC.setDefaultThresholdLevelsCallback(thresholdsCallback);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                Obj::createLoggerManager(&mX_mp, &observer, mXC);
#else
                Obj::createLoggerManager(&mX_mp, mXC);
                mX_mp->registerObserver(observerSP, "TO");
#endif
                mX_p = mX_mp.ptr();
                g_overrideManager = mX_p;
            }

            Obj&       mX = *mX_p;
            const Obj& X  = mX;
            ASSERT(1 == X.numCategories());

            static const struct {
                const char *d_catName;         // category name
                int         d_recLevel;        // record level (dflt, if 0)
                int         d_passLevel;       // pass level
                int         d_trigLevel;       // trigger level
                int         d_trigAllLevel;    // trigger-all level
                int         d_expRecLevel;     // expected rec level
                int         d_expPassLevel;    // expected pass level
                int         d_expTrigLevel;    // expected trigger level
                int         d_expTrigAllLevel; // expected trigger-all level
            } DATA[] = {
{ "A",             200,    150,   100,   50,     200,   150,    100,   50    },
{ "a.B",             0,      0,     0,    0,     200,   150,    100,   50    },
{ "A.B.C.D",         0,      0,     0,    0,     200,   150,    100,   50    },
{ "B.C",           210,    160,   110,   60,     210,   160,    110,   60    },
{ "B",             220,    170,   120,   70,     220,   170,    120,   70    },
{ "B.C.A",           0,      0,     0,    0,     210,   160,    110,   60    },
{ "B.c.A",           0,      0,     0,    0,     210,   160,    110,   60    },
{ "B.A.C",           0,      0,     0,    0,     220,   170,    120,   70    },

{ "AA",              0,      0,     0,    0,       0,    64,      0,    0    },
{ "BB",              0,      0,     0,    0,       0,    64,      0,    0    },
{ "Aa.BB",           0,      0,     0,    0,       0,    64,      0,    0    },

{ "A.B.C.D.E.F.G",   0,      0,     0,    0,     200,   150,    100,   50    },
          };
          enum { NUM_DATA = sizeof DATA / sizeof *DATA };

          int numCat = 1;
          for (int i = 0; i < NUM_DATA; ++i) {
              const char *CAT_NAME        = DATA[i].d_catName;
              const int   RECORD          = DATA[i].d_recLevel;
              const int   PASS            = DATA[i].d_passLevel;
              const int   TRIGGER         = DATA[i].d_trigLevel;
              const int   TRIGGERALL      = DATA[i].d_trigAllLevel;
              const int   EXPRECLEVEL     = DATA[i].d_expRecLevel;
              const int   EXPPASSLEVEL    = DATA[i].d_expPassLevel;
              const int   EXPTRIGLEVEL    = DATA[i].d_expTrigLevel;
              const int   EXPTRIGALLLEVEL = DATA[i].d_expTrigAllLevel;

              ASSERTV(i, numCat == mX.numCategories());

              if (0 == mX.lookupCategory(CAT_NAME)) {
                  ++numCat;
              }

              const Cat *cat = RECORD ? mX.addCategory(CAT_NAME,
                                                       RECORD,
                                                       PASS,
                                                       TRIGGER,
                                                       TRIGGERALL)
                                      : mX.setCategory(CAT_NAME);

              ASSERT(cat);
              ASSERTV(i, numCat == X.numCategories());

              if (RECORD) {
                  ASSERT(RECORD     == cat->recordLevel());
                  ASSERT(PASS       == cat->passLevel());
                  ASSERT(TRIGGER    == cat->triggerLevel());
                  ASSERT(TRIGGERALL == cat->triggerAllLevel());
              }
              else {
                  ASSERT(EXPRECLEVEL     == cat->recordLevel());
                  ASSERT(EXPPASSLEVEL    == cat->passLevel());
                  ASSERT(EXPTRIGLEVEL    == cat->triggerLevel());
                  ASSERT(EXPTRIGALLLEVEL == cat->triggerAllLevel());
              }
          }

          Cat *cX = mX.lookupCategory("B.C");
          ASSERT(cX);
          if (veryVeryVerbose) {
              P_(cX->categoryName());
              P_(cX->recordLevel());
              P_(cX->passLevel());
              P_(cX->triggerLevel());
              P(cX->triggerAllLevel());
          }

          ball::Logger& logger = mX.getLogger();

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          if (verbose) cout << "\tTest message buffer and size." << endl;
          {
              char *buffer = logger.messageBuffer();  ASSERT(buffer);
              ASSERT(MESSAGE_BUFFER_SIZE == logger.messageBufferSize());
              for (int i = 0; i < MESSAGE_BUFFER_SIZE; ++i) {
                  buffer[i] = (unsigned char) 0xbb;
              }
          }
#endif // BDE_OMIT_INTERNAL_DEPRECATED

          if (verbose) cout << "\tTest Threshold Level Values in Order."
                            << endl;

          // Estimate how many records the default buffer can hold.  This is an
          // "white box" estimation as we know the size of the buffer.
          for (int i = 0; i < 100; ++i) {
              // 160 (pass) < severity < 210 (record)
              logger.logMessage(*cX, 200, F_, L_, "Message");
          }

          // Publish, then get the number of records published as the estimate
          // of buffer capacity.
          logger.publish();

          const int BUF_CAP = observer.numPublishedRecords();
          int publishCount = BUF_CAP;     // # of records published so far
          if (veryVeryVerbose) { P(BUF_CAP); }

          // Verify pass through
          for (int i = 0; i < 100; ++i) {
              // 110 (trigger) < severity < 160 (pass)
              logger.logMessage(*cX, 150, F_, L_, "Message");
          }

          publishCount += 100;
          ASSERT(publishCount == observer.numPublishedRecords());

          // Publish all, verify buffer is published
          mX.publishAll();
          publishCount += BUF_CAP;

          ASSERT(publishCount == observer.numPublishedRecords());

          if (veryVeryVerbose) {
              P(publishCount);
              P(observer.numPublishedRecords());
          }

          // Verify 'removeAll' resets buffer
          for (int i = 0; i < BUF_CAP; ++i) {
              // 160 (pass) < severity < 210 (record)
              logger.logMessage(*cX, 200, F_, L_, "Message");
          }
          logger.removeAll();

          // Manually publish, no record to publish.
          logger.publish();
          ASSERT(publishCount == observer.numPublishedRecords());

          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          // Trigger, verify only 2 records are published (pass through and
          // then actually publish)
          logger.logMessage(*cX, 80, F_, L_, "trigger again");
          publishCount += 2;
          ASSERT(publishCount == observer.numPublishedRecords());

          // verify record threshold checking.
          logger.logMessage(*cX, 255, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 230, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 211, F_, L_, "Ignored Message");
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 210, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 209, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 199, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 161, F_, L_, "Recorded Message");
          publishCount += 4;
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 160, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 159, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 130, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 111, F_, L_, "Pass Through Message");
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 110, F_, L_, "Trigger Message");
          logger.logMessage(*cX, 109, F_, L_, "Trigger Message");
          logger.logMessage(*cX, 100, F_, L_, "Trigger Message");
          logger.logMessage(*cX,  99, F_, L_, "Trigger Message");
          logger.logMessage(*cX,  61, F_, L_, "Trigger Message");
          publishCount += 10;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 60, F_, L_, "Trigger All Message");
          logger.logMessage(*cX, 59, F_, L_, "Trigger All Message");
          logger.logMessage(*cX, 10, F_, L_, "Trigger All Message");
          logger.logMessage(*cX,  9, F_, L_, "Trigger All Message");
          logger.logMessage(*cX,  1, F_, L_, "Trigger All Message");
          logger.logMessage(*cX,  0, F_, L_, "Trigger All Message");
          publishCount += 12;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          if (verbose) cout << "\tTest Threshold Level Values Not in Order."
                            << endl;
          ASSERT(0 == cX->setLevels(150, 160, 100, 50));

          logger.logMessage(*cX, 255, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 200, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 161, F_, L_, "Ignored Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 160, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 159, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 151, F_, L_, "Pass Through Message");
          publishCount += 3;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 150, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 149, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 110, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 101, F_, L_, "Recorded Message");
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());

          ASSERT(0 == cX->setLevels(50, 100, 150, 200));

          logger.logMessage(*cX, 255, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 201, F_, L_, "Ignored Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 200, F_, L_, "Trigger All Message");
          logger.logMessage(*cX, 199, F_, L_, "Trigger All Message");
          logger.logMessage(*cX, 151, F_, L_, "Trigger All Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 150, F_, L_, "Trigger Message");
          logger.logMessage(*cX, 149, F_, L_, "Trigger Message");
          logger.logMessage(*cX, 101, F_, L_, "Trigger Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 100, F_, L_, "Pass Through Message");
          logger.logMessage(*cX,  99, F_, L_, "Pass Through Message");
          logger.logMessage(*cX,  51, F_, L_, "Pass Through Message");
          // each record is published only once -- pass through, since the
          // record is not stored.
          publishCount += 3;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 50, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 49, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 10, F_, L_, "Recorded Message");
          logger.logMessage(*cX,  9, F_, L_, "Recorded Message");
          logger.logMessage(*cX,  1, F_, L_, "Recorded Message");
          logger.logMessage(*cX,  0, F_, L_, "Recorded Message");
          // each record is published twice: pass through and then trigger
          publishCount += 12;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          if (verbose) cout << "\tTest Logging with Two Categories." << endl;

          ASSERT(0 == cX->setLevels(210, 160, 110, 60));

          Cat *cY = mX.lookupCategory("B");
          // Category "B"'s thresholds: 220/170/120/70 (R/P/T/TA)
          ASSERT(cY);

          // Transfer all threshold levels from the category to a
          // 'ball::ThresholdAggregate' object, and then clear the threshold
          // levels of the category.

          // verify record threshold checking.
          logger.logMessage(*cX, 255, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 230, F_, L_, "Ignored Message");
          logger.logMessage(*cX, 211, F_, L_, "Ignored Message");
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cY, 255, F_, L_, "Ignored Message");
          logger.logMessage(*cY, 230, F_, L_, "Ignored Message");
          logger.logMessage(*cY, 221, F_, L_, "Ignored Message");

          // verify that records of both categories are published already
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 210, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 209, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 199, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 161, F_, L_, "Recorded Message");
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cY, 220, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 219, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 199, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 171, F_, L_, "Recorded Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          publishCount += 8;

          // Trigger in one category also affects in others
          publishCount += 2;
          logger.logMessage(*cX, 80, F_, L_, "trigger");
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 160, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 159, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 130, F_, L_, "Pass Through Message");
          logger.logMessage(*cX, 111, F_, L_, "Pass Through Message");
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cY, 170, F_, L_, "Pass Through Message");
          logger.logMessage(*cY, 169, F_, L_, "Pass Through Message");
          logger.logMessage(*cY, 130, F_, L_, "Pass Through Message");
          logger.logMessage(*cY, 121, F_, L_, "Pass Through Message");
          publishCount += 4;
          ASSERT(publishCount == observer.numPublishedRecords());

          publishCount += 8;

          logger.logMessage(*cX, 110, F_, L_, "Trigger Message");
          publishCount += 2;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cX, 210, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 209, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 199, F_, L_, "Recorded Message");
          logger.logMessage(*cX, 161, F_, L_, "Recorded Message");
          ASSERT(publishCount == observer.numPublishedRecords());

          logger.logMessage(*cY, 220, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 219, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 199, F_, L_, "Recorded Message");
          logger.logMessage(*cY, 171, F_, L_, "Recorded Message");
          ASSERT(publishCount == observer.numPublishedRecords());
          publishCount += 8;

          logger.logMessage(*cY, 70, F_, L_, "Trigger All Message");
          publishCount += 2;
          ASSERT(publishCount == observer.numPublishedRecords());
          logger.publish();
          mX.publishAll();
          ASSERT(publishCount == observer.numPublishedRecords());

          if (0 == a) {
              ball::LoggerManager::shutDownSingleton();
          }
          else {
              g_overrideManager = 0;
          }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TEST PLACE HOLDER
        // --------------------------------------------------------------------
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TEST PLACE HOLDER
        // --------------------------------------------------------------------
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND ACCESSORS
        //   Legacy test for various manipulators and accessors.
        //
        // Concerns:
        //   We want to make sure that we can set various properties of the
        //   logger manager correctly.
        //   LEGACY TEST.
        //
        // Plan:
        // TBD doc
        //   Initialize logger manager with name filter and default threshold
        //   levels callback.  Add categories with hierarchical names, verify
        //   the inheritance of threshold values.  Add categories with invalid
        //   threshold values, verify that these add operations return fail
        //   values.  Add a category with no ancestors, verify the default
        //   values are used.  Set the default values to different values, add
        //   another category with no ancestors, verify new default values are
        //   used.  Set each of the threshold levels to all valid values,
        //   verify the threshold levels are set.  Get the *Default*
        //   *Category*; set the threshold levels of the *Default* *Category*
        //   to all valid values; verify they are set.  Reset the *Default*
        //   *Category* threshold levels (to factory default); verify they are
        //   set.  Set the maximum number of categories to a value less than
        //   the current length; verify we can still get the categories already
        //   added, but cannot add new categories.  Set the maximum number to
        //   a value 1 larger then the current number of categories; verify
        //   that we can add no more than one new category.  Set the maximum
        //   number of categories to 0; verify that we can add categories.
        //
        // Testing:
        //   Category *lookupCategory(const char *name);
        //   Category *addCategory(const char *name, int, int, int, int);
        //   const Category *setCategory(const char *name);
        //   Category *setCategory(const char *name, int, int, int, int);
        //   int setDefaultThresholdLevels(int, int, int, int);
        //   int resetDefaultThresholdLevels();
        //   void setCategoryThresholdsToCurrentDefaults(Cat *cat);
        //   void setCategoryThresholdsToFactoryDefaults(Cat *cat);
        //   void setMaxNumCategories(int);
        //   const Category *lookupCategory(const char *name) const;
        //   const Category& defaultCategory() const;
        //   const char *categoryName() const;
        //   int defaultRecordThresholdLevel() const;
        //   int defaultPassThresholdLevel() const;
        //   int defaultTriggerThresholdLevel() const;
        //   int defaultTriggerAllThresholdLevel() const;
        //   int maxNumCategories() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST PRIMARY MANIPULATORS AND ACCESSORS" << endl
                          << "=======================================" << endl;

        Cnf nameFilter(&toLower);

        Obj::DefaultThresholdLevelsCallback
                                   thresholdsCallback(&inheritThresholdLevels);

        Obj::UserFieldsPopulatorCallback populator(&myUserFieldsPopulator);

        ball::LoggerManagerConfiguration mXC;
        mXC.setCategoryNameFilterCallback(nameFilter);
        mXC.setDefaultThresholdLevelsCallback(thresholdsCallback);
        mXC.setUserFieldsPopulatorCallback(populator);

        ASSERT(false == Obj::isInitialized());
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(true  == Obj::isInitialized());

        Obj& mX = Obj::singleton();
        const Obj& X = mX;

        verifyLoggerManagerDefaults(X, 0, 0);

        const int dfltRecLevel     = mX.defaultRecordThresholdLevel();
        const int dfltPassLevel    = mX.defaultPassThresholdLevel();
        const int dfltTrigLevel    = mX.defaultTriggerThresholdLevel();
        const int dfltTrigAllLevel = mX.defaultTriggerAllThresholdLevel();

        Cat       *cat1 = mX.addCategory("BLOOMBERG.BAL", 64, 48, 32, 16);
        ASSERT(cat1);
        ASSERT(2    == X.numCategories());
        ASSERT(cat1 == X.lookupCategory("BLOOMBERG.bal"));
        ASSERT( 0   == bsl::strcmp("bloomberg.bal", cat1->categoryName()));
        ASSERT(64   == cat1->recordLevel());
        ASSERT(48   == cat1->passLevel());
        ASSERT(32   == cat1->triggerLevel());
        ASSERT(16   == cat1->triggerAllLevel());

        const Cat *cat2 = mX.setCategory("bloomberg.bal.ball");
        ASSERT(cat2);
        ASSERT(3    == X.numCategories());
        ASSERT(cat2 == X.lookupCategory("Bloomberg.Bal.Ball"));
        // verify inheritance of levels
        ASSERT(64 == cat2->recordLevel());
        ASSERT(48 == cat2->passLevel());
        ASSERT(32 == cat2->triggerLevel());
        ASSERT(16 == cat2->triggerAllLevel());

        // Testing that categories with invalid levels are not added.
        static const struct {
            int d_line;             // line number
            int d_recordLevel;      // record level
            int d_passLevel;        // pass level
            int d_triggerLevel;     // trigger level
            int d_triggerAllLevel;  // trigger all level
        } DATA[] = {
            // line  record    pass      trigger   triggerAll
            // no.   level     level     level     level
            // ----  --------  --------  --------  ----------
            {  L_,   256,      0,        0,        0          },
            {  L_,   0,        256,      0,        0          },
            {  L_,   0,        0,        256,      0          },
            {  L_,   0,        0,        0,        256        },
            {  L_,   256,      256,      256,      256        },
            {  L_,   INT_MAX,  0,        0,        0          },
            {  L_,   0,        INT_MAX,  0,        0          },
            {  L_,   0,        0,        INT_MAX,  0          },
            {  L_,   0,        0,        0,        INT_MAX    },
            {  L_,   INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX    },
            {  L_,   -1,       0,        0,        0          },
            {  L_,   0,        -1,       0,        0          },
            {  L_,   0,        0,        -1,       0          },
            {  L_,   0,        0,        0,        -1         },
            {  L_,   -1,       -1,       -1,       -1         },
            {  L_,   INT_MIN,  0,        0,        0          },
            {  L_,   0,        INT_MIN,  0,        0          },
            {  L_,   0,        0,        INT_MIN,  0          },
            {  L_,   0,        0,        0,        INT_MIN    },
            {  L_,   INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN    },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        const char *CAT_NAME = "Bloomberg.Bal.Ball";
        const int   NUM_CAT  = mX.numCategories();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE             = DATA[i].d_line;
            const int RECORD_LEVEL     = DATA[i].d_recordLevel;
            const int PASS_LEVEL       = DATA[i].d_passLevel;
            const int TRIGGER_LEVEL    = DATA[i].d_triggerLevel;
            const int TRIGGERALL_LEVEL = DATA[i].d_triggerAllLevel;

            if (veryVerbose) {
                P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
            }
            const Cat *cat = mX.setCategory(CAT_NAME,
                                             RECORD_LEVEL,
                                             PASS_LEVEL,
                                             TRIGGER_LEVEL,
                                             TRIGGERALL_LEVEL);
            ASSERTV(LINE, 0       == cat);
            ASSERTV(LINE, NUM_CAT == X.numCategories());
        }

        ASSERT(cat2 == mX.setCategory(CAT_NAME, 40, 30, 20, 10));
        ASSERT(cat2);
        ASSERT(40 == cat2->recordLevel());
        ASSERT(30 == cat2->passLevel());
        ASSERT(20 == cat2->triggerLevel());
        ASSERT(10 == cat2->triggerAllLevel());

        const Cat *cat6 = mX.setCategory("Bloomberg.Bal.Ball.logging",
                                         80, 60, 40, 20);
        ASSERT(cat6);
        ASSERT(0  == (cat2 == cat6));
        ASSERT(80 == cat6->recordLevel());
        ASSERT(60 == cat6->passLevel());
        ASSERT(40 == cat6->triggerLevel());
        ASSERT(20 == cat6->triggerAllLevel());

        const Cat *cat3 = mX.setCategory("Equities.Graphics");
        ASSERT(dfltRecLevel     == cat3->recordLevel());
        ASSERT(dfltPassLevel    == cat3->passLevel());
        ASSERT(dfltTrigLevel    == cat3->triggerLevel());
        ASSERT(dfltTrigAllLevel == cat3->triggerAllLevel());

        if (veryVeryVerbose) {
            P_(dfltRecLevel);
            P_(dfltPassLevel);
            P_(dfltTrigLevel);
            P(dfltTrigAllLevel);
        }

        mX.setDefaultThresholdLevels(192, 144, 96, 48);
        const int dfltRecLevel2     = X.defaultRecordThresholdLevel();
        const int dfltPassLevel2    = X.defaultPassThresholdLevel();
        const int dfltTrigLevel2    = X.defaultTriggerThresholdLevel();
        const int dfltTrigAllLevel2 = X.defaultTriggerAllThresholdLevel();
        ASSERT(192 == dfltRecLevel2);
        ASSERT(144 == dfltPassLevel2);
        ASSERT( 96 == dfltTrigLevel2);
        ASSERT( 48 == dfltTrigAllLevel2);

        if (veryVeryVerbose) {
            P_(dfltRecLevel2);
            P_(dfltPassLevel2);
            P_(dfltTrigLevel2);
            P(dfltTrigAllLevel2);
        }

        const Cat *cat4 = mX.setCategory("Equities.Graphics.G");
        ASSERT(cat4);
        ASSERT(dfltRecLevel     == cat4->recordLevel());
        ASSERT(dfltPassLevel    == cat4->passLevel());
        ASSERT(dfltTrigLevel    == cat4->triggerLevel());
        ASSERT(dfltTrigAllLevel == cat4->triggerAllLevel());

        Cat *cat4a = mX.lookupCategory("Equities.Graphics.G");
        ASSERT(cat4 == cat4a);
        mX.setCategoryThresholdsToCurrentDefaults(cat4a);
        ASSERT(dfltRecLevel2     == cat4a->recordLevel());
        ASSERT(dfltPassLevel2    == cat4a->passLevel());
        ASSERT(dfltTrigLevel2    == cat4a->triggerLevel());
        ASSERT(dfltTrigAllLevel2 == cat4a->triggerAllLevel());

        mX.setCategoryThresholdsToFactoryDefaults(cat4a);
        ASSERT(dfltRecLevel     == cat4a->recordLevel());
        ASSERT(dfltPassLevel    == cat4a->passLevel());
        ASSERT(dfltTrigLevel    == cat4a->triggerLevel());
        ASSERT(dfltTrigAllLevel == cat4a->triggerAllLevel());

        const Cat *cat5 = mX.setCategory("Equities");
        ASSERT(cat5);
        ASSERT(192 == cat5->recordLevel());
        ASSERT(144 == cat5->passLevel());
        ASSERT( 96 == cat5->triggerLevel());
        ASSERT( 48 == cat5->triggerAllLevel());

        mX.resetDefaultThresholdLevels();
        ASSERT(dfltRecLevel     == mX.defaultRecordThresholdLevel());
        ASSERT(dfltPassLevel    == mX.defaultPassThresholdLevel());
        ASSERT(dfltTrigLevel    == mX.defaultTriggerThresholdLevel());
        ASSERT(dfltTrigAllLevel == mX.defaultTriggerAllThresholdLevel());

        const Cat& dfltCategory = mX.defaultCategory();
        const int dfltCat_recLevel     = dfltCategory.recordLevel();
        const int dfltCat_passLevel    = dfltCategory.passLevel();
        const int dfltCat_trigLevel    = dfltCategory.triggerLevel();
        const int dfltCat_trigAllLevel = dfltCategory.triggerAllLevel();
        ASSERT(&dfltCategory == X.lookupCategory(DEFAULT_CATEGORY_NAME));

        Cat *cat7 = mX.addCategory("test_category", 0, 0, 0, 0);
        ASSERT(cat7);
        ASSERT(0 == cat7->recordLevel());
        ASSERT(0 == cat7->passLevel());
        ASSERT(0 == cat7->triggerLevel());
        ASSERT(0 == cat7->triggerAllLevel());

        for (int i = 0; i < 256; ++i) {
            ASSERT(0 == cat7->setLevels(i, 0, 0, 0));
            ASSERT(i == cat7->recordLevel());
            ASSERT(0 == cat7->passLevel());
            ASSERT(0 == cat7->triggerLevel());
            ASSERT(0 == cat7->triggerAllLevel());

            ASSERT(0 == mX.setDefaultThresholdLevels(i, 0, 0, 0));
            ASSERT(i == X.defaultRecordThresholdLevel());
            ASSERT(0 == X.defaultPassThresholdLevel());
            ASSERT(0 == X.defaultTriggerThresholdLevel());
            ASSERT(0 == X.defaultTriggerAllThresholdLevel());
            ASSERT(dfltCat_recLevel == dfltCategory.recordLevel());
        }

        for (int i = 0; i < 256; ++i) {
            ASSERT(0 == cat7->setLevels(0, i, 0, 0));
            ASSERT(0 == cat7->recordLevel());
            ASSERT(i == cat7->passLevel());
            ASSERT(0 == cat7->triggerLevel());
            ASSERT(0 == cat7->triggerAllLevel());

            ASSERT(0 == mX.setDefaultThresholdLevels(0, i, 0, 0));
            ASSERT(0 == X.defaultRecordThresholdLevel());
            ASSERT(i == X.defaultPassThresholdLevel());
            ASSERT(0 == X.defaultTriggerThresholdLevel());
            ASSERT(0 == X.defaultTriggerAllThresholdLevel());
            ASSERT(dfltCat_passLevel == dfltCategory.passLevel());
        }

        for (int i = 0; i < 256; ++i) {
            ASSERT(0 == cat7->setLevels(0, 0, i, 0));
            ASSERT(0 == cat7->recordLevel());
            ASSERT(0 == cat7->passLevel());
            ASSERT(i == cat7->triggerLevel());
            ASSERT(0 == cat7->triggerAllLevel());

            ASSERT(0 == mX.setDefaultThresholdLevels(0, 0, i, 0));
            ASSERT(0 == X.defaultRecordThresholdLevel());
            ASSERT(0 == X.defaultPassThresholdLevel());
            ASSERT(i == X.defaultTriggerThresholdLevel());
            ASSERT(0 == X.defaultTriggerAllThresholdLevel());
            ASSERT(dfltCat_trigLevel == dfltCategory.triggerLevel());
        }

        for (int i = 0; i < 256; ++i) {
            ASSERT(0 == cat7->setLevels(0, 0, 0, i));
            ASSERT(0 == cat7->recordLevel());
            ASSERT(0 == cat7->passLevel());
            ASSERT(0 == cat7->triggerLevel());
            ASSERT(i == cat7->triggerAllLevel());

            ASSERT(0 == mX.setDefaultThresholdLevels(0, 0, 0, i));
            ASSERT(0 == X.defaultRecordThresholdLevel());
            ASSERT(0 == X.defaultPassThresholdLevel());
            ASSERT(0 == X.defaultTriggerThresholdLevel());
            ASSERT(i == X.defaultTriggerAllThresholdLevel());
            ASSERT(dfltCat_trigAllLevel == dfltCategory.triggerAllLevel());
        }

        ASSERT(0 == X.maxNumCategories());
        ASSERT(8 == X.numCategories());

        mX.setMaxNumCategories(1);

        ASSERT(1 == X.maxNumCategories());
        ASSERT(8 == X.numCategories());

        // verify that we can still get existing categories...
        ASSERT(mX.lookupCategory("test_category"));
        ASSERT(mX.lookupCategory("Equities"));
        ASSERT(mX.lookupCategory("Equities.Graphics"));
        ASSERT(mX.lookupCategory("Equities.Graphics.G"));
        ASSERT(mX.lookupCategory("Bloomberg.Bal.Ball"));
        ASSERT(mX.lookupCategory("Bloomberg.Bal.Ball.Logging"));
        ASSERT(mX.lookupCategory("Bloomberg.Bal"));

        // ...but cannot add new categories
        ASSERT(0 == mX.addCategory("one_more_category", 0, 0, 0, 0));

        mX.setMaxNumCategories(9);  // currently have 7 + 1 (default)
        ASSERT(9 == X.maxNumCategories());
        ASSERT(mX.addCategory("one_more_category", 0, 0, 0, 0));
        ASSERT(9 == X.maxNumCategories());
        ASSERT(9 == X.numCategories());

        ASSERT(0 == mX.addCategory("and_another", 0, 0, 0, 0));
        ASSERT(9 == X.maxNumCategories());
        ASSERT(9 == X.numCategories());

        mX.setMaxNumCategories(0);
        ASSERT(0  != mX.addCategory("and_another", 0, 0, 0, 0));
        ASSERT(0  == X.maxNumCategories());
        ASSERT(10 == X.numCategories());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CONCERN: LOG RECORD POPULATOR CALLBACKS
        //
        // Concerns:
        //: 1 That the supplied user-fields populator callback is used to
        //:   populate a list of user fields with each logged record.
        //
        // Plan:
        //: 1 Create a logger manager and supply user fields populator callback
        //:   in the configuration.
        //:
        //: 2 Install additional observer.
        //:
        //: 3 Log a test message and verify that all observers receive the
        //:   message and the message contains populated log fields.
        //
        // Testing:
        //   CONCERN: LOG RECORD POPULATOR CALLBACKS
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nCONCERN: LOG RECORD POPULATOR CALLBACKS"
                 << "\n=======================================" << endl;

        ball::LoggerManagerConfiguration mXC;
        mXC.setUserFieldsPopulatorCallback(&testUserFieldsPopulatorCb);

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::TestObserver  observer1Holder(&cout);
        ball::TestObserver *observer1 = &observer1Holder;

        ball::LoggerManagerScopedGuard lmGuard(observer1, mXC);
#else
        bsl::shared_ptr<ball::TestObserver> observer1sp(
                                                new ball::TestObserver(&cout));
        ball::TestObserver *observer1 = observer1sp.get();

        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(0 == Obj::singleton().registerObserver(observer1sp,
                                                      "observer1"));
#endif

        Obj& mX = Obj::singleton();  const Obj& X = mX;

        verifyLoggerManagerDefaults(X, 0, observer1);

        // Install additional (non-legacy) observer.
        bsl::shared_ptr<ball::TestObserver> observer2(
                                                new ball::TestObserver(&cout));

        ASSERT(0 == mX.registerObserver(observer2, "observer2"));

        // Log a test message and verify that user fields are set by the
        // callback.

        if (veryVerbose) {
            observer1->setVerbose(1);
            observer2->setVerbose(1);
        }

        ball::Logger& logger = mX.getLogger();

        ASSERT(0 == observer1->numPublishedRecords());
        ASSERT(0 == observer2->numPublishedRecords());

        const char *MESSAGE = "Populators Callback Test";
        const int   LINE    = L_ + 1;
        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE,
                          MESSAGE);
        ASSERT(1 == observer1->numPublishedRecords());
        ASSERT(1 == observer2->numPublishedRecords());

        // Verify that the message is logged in both observers
        for (int i = 0; i < 2; ++i) {
            ball::TestObserver *observer = i ? observer1
                                             : observer2.get();

            const Rec&  R = observer->lastPublishedRecord();
            if (veryVerbose) cout << R << endl;

            const Attr& A = R.fixedFields();

            ASSERT(0            == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                               A.category()));
            ASSERT(FACTORY_PASS == A.severity());
            ASSERT(0            == bsl::strcmp(__FILE__, A.fileName()));
            ASSERT(LINE         == A.lineNumber());
            ASSERT(0            == bsl::strcmp(MESSAGE, A.message()));

            const FieldValues& V = R.customFields();
            ASSERT(2               == V.length());
            ASSERT("UFString"      == V[0].theString());
            ASSERT(1234            == V[1].theInt64());
        }
        // Install ScopedFields populator callbacks
        mX.registerAttributeCollector(&testAttributesCollector1,
                                    "dti");
        mX.registerAttributeCollector(&testAttributesCollector2,
                                    "perf");

        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE,
                          MESSAGE);
        ASSERT(2 == observer1->numPublishedRecords());
        ASSERT(2 == observer2->numPublishedRecords());

        // Verify that the message is logged in both observers
        for (int i = 0; i < 2; ++i) {
            ball::TestObserver *observer = i ? observer1
                                             : observer2.get();

            const Rec&  R = observer->lastPublishedRecord();
            if (veryVerbose) cout << R << endl;

            const Attr& A = R.fixedFields();

            ASSERT(0            == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                               A.category()));
            ASSERT(FACTORY_PASS == A.severity());
            ASSERT(0            == bsl::strcmp(__FILE__, A.fileName()));
            ASSERT(LINE         == A.lineNumber());
            ASSERT(0            == bsl::strcmp(MESSAGE, A.message()));

            const FieldValues& V = R.customFields();
            ASSERT(2            == V.length());
            ASSERT("UFString"   == V[0].theString());
            ASSERT(1234         == V[1].theInt64());

            // Checking attributes added by collectors.
            ASSERTV(2 == R.attributes().size());
            ASSERTV(ball::Attribute("spanId", 1234) == R.attributes()[0]);
            ASSERTV(ball::Attribute("dti.tDiff", "25ms") == R.attributes()[1]);
        }

        {
            ball::ScopedAttribute myAttribute("test", "value");
            logger.logMessage(X.defaultCategory(),
                              FACTORY_PASS,
                              __FILE__,
                              LINE,
                              MESSAGE);
            ASSERT(3 == observer1->numPublishedRecords());
            ASSERT(3 == observer2->numPublishedRecords());

            for (int i = 0; i < 2; ++i) {
                ball::TestObserver *observer = i ? observer1
                                                 : observer2.get();
                const Rec&  R = observer->lastPublishedRecord();
                if (veryVerbose) cout << R << endl;

                // Scoped attributes are added in front of the attribute
                // vector.
                ASSERTV(3 == R.attributes().size());
                ASSERTV(ball::Attribute("test", "value") == R.attributes()[0]);
                ASSERTV(ball::Attribute("spanId", 1234)  == R.attributes()[1]);
                ASSERTV(ball::Attribute("dti.tDiff", "25ms")
                                                         == R.attributes()[2]);
            }
        }

        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE,
                          MESSAGE);
        ASSERT(4 == observer1->numPublishedRecords());
        ASSERT(4 == observer2->numPublishedRecords());

        for (int i = 0; i < 2; ++i) {
            ball::TestObserver *observer = i ? observer1
                                             : observer2.get();
            const Rec&  R = observer->lastPublishedRecord();
            if (veryVerbose) cout << R << endl;
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CONCERN: INTERNAL BROADCAST OBSERVER
        //   Logger manager contains internal broadcast observer.
        //
        // Concerns:
        //: 1 All registered observers receive the log messages.
        //
        // Plan:
        //: 1 Construct a default singleton logger manager.
        //
        // Testing:
        //   CONCERN: INTERNAL BROADCAST OBSERVER
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "CONCERN: INTERNAL BROADCAST OBSERVER" << endl
                         << "====================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ball::LoggerManagerConfiguration config;

        ASSERT(false == Obj::isInitialized());
        ball::LoggerManagerScopedGuard lmGuard(config);
        ASSERT(true  == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, &ga, 0);

        bsl::shared_ptr<ball::TestObserver> observer1(
                                                new ball::TestObserver(&cout));

        ASSERT(0 == mX.registerObserver(observer1, "observer1"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERT(0 == X.observer());  // observer is not installed as legacy
                                    // mandatory observer
#endif

        ASSERT(observer1.get() == X.findObserver("observer1").get());

        // Log a test message and verify that the registered observer gets it.
        ball::Logger& logger = mX.getLogger();

        if (veryVerbose) observer1->setVerbose(1);

        ASSERT(0 == observer1->numPublishedRecords());

        const char *MESSAGE1 = "message 1";
        const int   LINE1    = L_ + 1;
        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE1,
                          MESSAGE1);

        ASSERT(1 == observer1->numPublishedRecords());

        const Rec&  R1 = observer1->lastPublishedRecord();
        if (veryVerbose) cout << R1 << endl;

        const Attr& A1 = R1.fixedFields();

        ASSERT(0            == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                           A1.category()));
        ASSERT(FACTORY_PASS == A1.severity());
        ASSERT(0            == bsl::strcmp(__FILE__, A1.fileName()));
        ASSERT(LINE1        == A1.lineNumber());
        ASSERT(0            == bsl::strcmp(MESSAGE1, A1.message()));

        const FieldValues& V1 = R1.customFields();
        ASSERT(0 == V1.length());

        // Add another observer.
        bsl::shared_ptr<ball::TestObserver> observer2(
                                                new ball::TestObserver(&cout));

        ASSERT(0 == mX.registerObserver(observer2, "observer2"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERT(0 == X.observer());  // observer is not installed as legacy
                                    // mandatory observer
#endif

        ASSERT(observer1.get() == X.findObserver("observer1").get());
        ASSERT(observer2.get() == X.findObserver("observer2").get());

        if (veryVerbose) observer2->setVerbose(1);

        ASSERT(0 == observer2->numPublishedRecords());

        const char *MESSAGE2 = "message 2";
        const int   LINE2    = L_ + 1;
        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE2,
                          MESSAGE2);

        ASSERT(2 == observer1->numPublishedRecords());
        ASSERT(1 == observer2->numPublishedRecords());

        // Verify that the message is logged in both observers
        for (int i = 0; i < 2; ++i) {
            ball::TestObserver *observer = i ? observer1.get()
                                             : observer2.get();

            const Rec&  R2 = observer->lastPublishedRecord();
            if (veryVerbose) cout << R2 << endl;

            const Attr& A2 = R2.fixedFields();

            ASSERT(0            == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                               A2.category()));
            ASSERT(FACTORY_PASS == A2.severity());
            ASSERT(0            == bsl::strcmp(__FILE__, A2.fileName()));
            ASSERT(LINE2        == A2.lineNumber());
            ASSERT(0            == bsl::strcmp(MESSAGE2, A2.message()));

            const FieldValues& V2 = R2.customFields();
            ASSERT(0 == V2.length());
        }

        // Deregister observer1 and verify that it no longer receives messages.
        ASSERT(0 == mX.deregisterObserver("observer1"));

        const char *MESSAGE3 = "message 3";
        const int   LINE3    = L_ + 1;
        logger.logMessage(X.defaultCategory(),
                          FACTORY_PASS,
                          __FILE__,
                          LINE3,
                          MESSAGE3);

        ASSERT(2 == observer1->numPublishedRecords());
        ASSERT(2 == observer2->numPublishedRecords());

        const Rec&  R3 = observer2->lastPublishedRecord();
        if (veryVerbose) cout << R3 << endl;

        const Attr& A3 = R3.fixedFields();

        ASSERT(0            == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                           A3.category()));
        ASSERT(FACTORY_PASS == A3.severity());
        ASSERT(0            == bsl::strcmp(__FILE__, A3.fileName()));
        ASSERT(LINE3        == A3.lineNumber());
        ASSERT(0            == bsl::strcmp(MESSAGE3, A3.message()));

        const FieldValues& V3 = R3.customFields();
        ASSERT(0 == V3.length());

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING OBSERVER REGISTRATION
        //
        // Concerns:
        //: 1 Default-constructed logger manager does not have any registered
        //:   observers.
        //:
        //: 2 An observer can be registered under any unique name.
        //:
        //: 3 Registration of the observer under already registered name fails
        //:   with no side effects on already registered observers.
        //:
        //: 4 Registered observer can be deregistered.
        //
        // Plan:
        //: 1 Construct a default singleton logger manager and verify that it
        //:   does not have any registered observers.  (C-1)
        //:
        //: 2 Register a set of observers under different names.  (C-2)
        //:
        //: 3 Register observer under already registered name and verify that
        //:   the operation fails with no side effects.  (C-3)
        //:
        //: 4 Deregister observers and verify the operation.  (C-4)
        //
        // Testing:
        //   findObserver(const bsl::string_view&);
        //   findObserver(const shared_ptr<OBSERVER>*, const string_view&);
        //   registerObserver(const shared_ptr<Observer>&, const string_view&);
        //   deregisterObserver(const bsl::string_view&);
        //   deregisterAllObservers();
        //   void visitObservers(OBSERVER_VISITOR visitor);
        //   void visitObservers(OBSERVER_VISITOR visitor) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING OBSERVER REGISTRATION" << endl
                         << "=============================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        using namespace TEST_CASE_OBSERVER_VISITOR;

        {
            ball::LoggerManagerConfiguration mXC;

            Obj        mX(mXC);
            const Obj& X = mX;

            verifyLoggerManagerDefaults(X, &ga, 0);

            bsl::shared_ptr<ball::TestObserver> observer1(
                                                new ball::TestObserver(&cout));

            ASSERT(0 == mX.registerObserver(observer1, "observer1"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());  // observer is not installed as legacy
                                        // mandatory observer
#endif

            ASSERT(observer1.get() == X.findObserver("observer1").get());

            {
                bsl::shared_ptr<ball::TestObserver> observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            // Add another observer.
            bsl::shared_ptr<ball::TestObserver> observer2(
                                                new ball::TestObserver(&cout));

            ASSERT(0 == mX.registerObserver(observer2, "observer2"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());

            {
                bsl::shared_ptr<ball::TestObserver> observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            // Add another observer.  Adding to the existing name (no effect).
            bsl::shared_ptr<ball::TestObserver> observer3(
                                                new ball::TestObserver(&cout));

            ASSERT(0 != mX.registerObserver(observer3, "observer1"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());

            {
                bsl::shared_ptr<ball::TestObserver> observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            // Adding under "default" name.  Testing that "default" does not
            // have any special meaning (and is not clashing with internal
            // name).
            ASSERT(0 == mX.registerObserver(observer3, "default"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());
            ASSERT(observer3.get() == X.findObserver("default").get());

            {
                bsl::shared_ptr<ball::TestObserver> observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "default"));
                ASSERT(observerPtr.get() == observer3.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "default"));
                ASSERT(observerPtrConst.get() == observer3.get());

                mX.visitObservers(TestObserverVisitor(&mX));
                X.visitObservers(TestObserverVisitor(&X));
            }

            // Deregistering observers.
            ASSERT(0 != mX.deregisterObserver("testObserver"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());
            ASSERT(observer3.get() == X.findObserver("default").get());

            {
                bsl::shared_ptr<ball::TestObserver>       observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "default"));
                ASSERT(observerPtr.get() == observer3.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "default"));
                ASSERT(observerPtrConst.get() == observer3.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            ASSERT(0 == mX.deregisterObserver("default"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());
            ASSERT(0               == X.findObserver("default").get());
            ASSERT(0 != mX.deregisterObserver("default"));

            {
                bsl::shared_ptr<ball::TestObserver> observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 == mX.findObserver(&observerPtr, "observer1"));
                ASSERT(observerPtr.get() == observer1.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(observerPtrConst.get() == observer1.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                ASSERT(0 != mX.findObserver(&observerPtr, "default"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "default"));
                ASSERT(0 == observerPtrConst.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            ASSERT(0 == mX.deregisterObserver("observer1"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(0               == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());
            ASSERT(0               == X.findObserver("default").get());
            ASSERT(0 != mX.deregisterObserver("observer1"));

            {
                bsl::shared_ptr<ball::TestObserver>       observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 != mX.findObserver(&observerPtr, "observer1"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(0 == observerPtrConst.get());

                ASSERT(0 == mX.findObserver(&observerPtr, "observer2"));
                ASSERT(observerPtr.get() == observer2.get());
                ASSERT(0 == X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(observerPtrConst.get() == observer2.get());

                ASSERT(0 != mX.findObserver(&observerPtr, "default"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "default"));
                ASSERT(0 == observerPtrConst.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            ASSERT(0 == mX.deregisterObserver("observer2"));
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(0 == X.findObserver("observer1").get());
            ASSERT(0 == X.findObserver("observer2").get());
            ASSERT(0 == X.findObserver("default").get());
            ASSERT(0 != mX.deregisterObserver("observer2"));

            {
                bsl::shared_ptr<ball::TestObserver>       observerPtr;
                bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

                ASSERT(0 != mX.findObserver(&observerPtr, "observer1"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "observer1"));
                ASSERT(0 == observerPtrConst.get());

                ASSERT(0 != mX.findObserver(&observerPtr, "observer2"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "observer2"));
                ASSERT(0 == observerPtrConst.get());

                ASSERT(0 != mX.findObserver(&observerPtr, "default"));
                ASSERT(0 == observerPtr.get());
                ASSERT(0 != X.findObserver(&observerPtrConst, "default"));
                ASSERT(0 == observerPtrConst.get());

                mX.visitObservers(TestObserverVisitor(&X));
                X.visitObservers(TestObserverVisitor(&X));
            }

            // Re-populate and deregister all observers.
            ASSERT(0 == mX.registerObserver(observer1, "observer1"));
            ASSERT(0 == mX.registerObserver(observer2, "observer2"));
            ASSERT(0 == mX.registerObserver(observer3, "default"));

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERT(0 == X.observer());
#endif
            ASSERT(observer1.get() == X.findObserver("observer1").get());
            ASSERT(observer2.get() == X.findObserver("observer2").get());
            ASSERT(observer3.get() == X.findObserver("default").get());

            mX.deregisterAllObservers();
            ASSERT(0 == X.findObserver("observer1").get());
            ASSERT(0 == X.findObserver("observer2").get());
            ASSERT(0 == X.findObserver("default").get());

            // Re-populate and destroy logger manager.
            ASSERT(0 == mX.registerObserver(observer1, "observer1"));
            ASSERT(0 == mX.registerObserver(observer2, "observer2"));
            ASSERT(0 == mX.registerObserver(observer3, "default"));
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCERN: DEFAULT THRESHOLD LEVELS CALLBACK
        //   Note that the callback installed for this test uses the singleton
        //   logger manager.
        //
        // Concerns:
        //: 1 That the singleton logger manager created by 'initSingleton'
        //:   uses the supplied 'DefaultThresholdLevelsCallback'
        //:
        //: 2 That the 'setDefaultThresholdLevelsCallback' sets the
        //:   default threshold levels callback used by the logger manager
        //:
        //: 3 That the logger manager uses the currently installed default
        //:   threshold levels callback to set the threshold levels of newly
        //:   created categories.
        //
        // Plan:
        //: 1 Construct a singleton logger manager, make sure the parameters
        //:   passed in are correctly hooked up.
        //:
        //: 2 Add categories with names forming hierarchies, verify that
        //:   whenever an ancestor is present, the threshold values are
        //:   inherited, otherwise, appropriate default values are used.
        //
        // Testing:
        //   CONCERN: DEFAULT THRESHOLD LEVELS CALLBACK
        //   const Aggregate& getDefaultThresholdLevels() const;
        //   Aggregate getNewCategoryThresholdLevels(const char *) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCERN: DEFAULT THRESHOLD LEVELS CALLBACK\n"
                          << "==========================================\n";

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        const ball::ThresholdAggregate cbLevels( 44, 33, 22, 11);
        const ball::ThresholdAggregate mgrLevels(40, 50, 60, 70);

        ASSERT(cbLevels != mgrLevels);

        for (int ti = 0; ti < 4; ++ti) {
            const bool DF_SET_IN_CONFIG  = !!(ti & 1);
            const bool CB_SET_IN_CONFIG  = !!(ti & 2);

            ball::LoggerManagerConfiguration mXC;
            if (DF_SET_IN_CONFIG) {
                mXC.setDefaultThresholdLevelsIfValid(
                                                  mgrLevels.recordLevel(),
                                                  mgrLevels.passLevel(),
                                                  mgrLevels.triggerLevel(),
                                                  mgrLevels.triggerAllLevel());
            }
            if (CB_SET_IN_CONFIG) {
                mXC.setDefaultThresholdLevelsCallback(&testThresholdLevelsCb);
            }

            ASSERT(false == Obj::isInitialized());
            ball::LoggerManagerScopedGuard guard(mXC, &ga);
            ASSERT(true == Obj::isInitialized());

            Obj&       mX = Obj::singleton();
            const Obj& X  = mX;

            if (!DF_SET_IN_CONFIG) {
                verifyLoggerManagerDefaults(X, &ga, 0);

                mX.setDefaultThresholdLevels(mgrLevels.recordLevel(),
                                             mgrLevels.passLevel(),
                                             mgrLevels.triggerLevel(),
                                             mgrLevels.triggerAllLevel());
            }
            if (!CB_SET_IN_CONFIG) {
                ball::LoggerManager::DefaultThresholdLevelsCallback cb(
                                                       &testThresholdLevelsCb);
                mX.setDefaultThresholdLevelsCallback(&cb);
            }

            {
                const ball::ThresholdAggregate& levels =
                                                    X.defaultThresholdLevels();
                ASSERT(mgrLevels == levels);
            }
            {
                ball::ThresholdAggregate levels;
                X.thresholdLevelsForNewCategory(&levels, "woof");
                ASSERT(cbLevels == levels);
            }

            const Cat *cat1 = mX.addCategory("Bloomberg.Bal", 64, 48, 32, 16);
            ASSERT(cat1);
            ASSERT(cat1 == X.lookupCategory("Bloomberg.Bal"));
            ASSERT( 0  == bsl::strcmp("Bloomberg.Bal", cat1->categoryName()));
            ASSERT(64  == cat1->recordLevel());
            ASSERT(48  == cat1->passLevel());
            ASSERT(32  == cat1->triggerLevel());
            ASSERT(16  == cat1->triggerAllLevel());

            ASSERT(mgrLevels.recordLevel() ==
                                          X.defaultRecordThresholdLevel());
            ASSERT(mgrLevels.passLevel() ==
                                          X.defaultPassThresholdLevel());
            ASSERT(mgrLevels.triggerLevel() ==
                                          X.defaultTriggerThresholdLevel());
            ASSERT(mgrLevels.triggerAllLevel() ==
                                          X.defaultTriggerAllThresholdLevel());

            const Cat *cat2 = mX.setCategory("setCategory(cont char *)");
            ASSERT(cat2);
            ASSERT(cat2 == X.lookupCategory("setCategory(cont char *)"));
            ASSERT( 0   == bsl::strcmp("setCategory(cont char *)",
                                       cat2->categoryName()));
            ASSERT(cbLevels.recordLevel()     == cat2->recordLevel());
            ASSERT(cbLevels.passLevel()       == cat2->passLevel());
            ASSERT(cbLevels.triggerLevel()    == cat2->triggerLevel());
            ASSERT(cbLevels.triggerAllLevel() == cat2->triggerAllLevel());

            // Verify that the threshold level values are inherited from the
            // "parent" category of the added category, or default values if
            // none exists, according to our defined callback.  Note that there
            // is no name filter installed.

            Obj::DefaultThresholdLevelsCallback inheritCb(
                                                      &inheritThresholdLevels);
            mX.setDefaultThresholdLevelsCallback(&inheritCb);

            if (veryVerbose) cout << "\tTest New Category Threshold Levels"
                                  << " with Inheritance" << endl;
            static const struct {
                const char *d_catName;         // category name
                int         d_recLevel;        // record level (dflt, if 0)
                int         d_passLevel;       // pass level (dflt, if 0)
                int         d_trigLevel;       // trigger level (dflt, if 0)
                int         d_trigAllLevel;    // trigger-all level (dflt,if 0)
                int         d_expRecLevel;     // expected record level
                int         d_expPassLevel;    // expected pass level
                int         d_expTrigLevel;    // expected trigger level
                int         d_expTrigAllLevel; // expected trigger-all level
            } DATA[] = {
// category       input   input  input  input   exp'd  exp'd   exp'd  exp'd
//   name         rec l   ps l   tg l   tgA l   rec l  ps l    tg l   tgA l
// --------       -----   -----  -----  -----   -----  -----   -----  -----
{ "A",             200,    150,   100,   50,     200,   150,    100,   50    },
{ "a.B",             0,      0,     0,    0,       0,     0,      0,    0    },
{ "A.B.C.D",         0,      0,     0,    0,     200,   150,    100,   50    },
{ "B.C",           210,    160,   110,   60,     210,   160,    110,   60    },
{ "B",             220,    170,   120,   70,     220,   170,    120,   70    },
{ "B.C.A",           0,      0,     0,    0,     210,   160,    110,   60    },
{ "B.c.A",           0,      0,     0,    0,     220,   170,    120,   70    },
{ "B.A.C",           0,      0,     0,    0,     220,   170,    120,   70    },
{ "AA",              0,      0,     0,    0,       0,     0,      0,    0    },
{ "BB",              0,      0,     0,    0,       0,     0,      0,    0    },
{ "Aa.BB",           0,      0,     0,    0,       0,     0,      0,    0    },
{ "A.B.C.D.E.F.G",   0,      0,     0,    0,     200,   150,    100,   50    },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const char *CAT_NAME = DATA[i].d_catName;
                const int expRecLevel     = DATA[i].d_expRecLevel == 0
                                          ? X.defaultRecordThresholdLevel()
                                          : DATA[i].d_expRecLevel;
                const int expPassLevel    = DATA[i].d_expPassLevel == 0
                                          ? X.defaultPassThresholdLevel()
                                          : DATA[i].d_expPassLevel;
                const int expTrigLevel    = DATA[i].d_expTrigLevel == 0
                                          ? X.defaultTriggerThresholdLevel()
                                          : DATA[i].d_expTrigLevel;
                const int expTrigAllLevel = DATA[i].d_expTrigAllLevel == 0
                                          ? X.defaultTriggerAllThresholdLevel()
                                          : DATA[i].d_expTrigAllLevel;

                const Cat *cat = DATA[i].d_recLevel
                                 ? mX.addCategory(CAT_NAME,
                                                  DATA[i].d_recLevel,
                                                  DATA[i].d_passLevel,
                                                  DATA[i].d_trigLevel,
                                                  DATA[i].d_trigAllLevel)
                                 : mX.setCategory(CAT_NAME);

                if (veryVeryVerbose) {
                    P(CAT_NAME);
                    P_(cat->recordLevel());
                    P_(cat->passLevel());
                    P_(cat->triggerLevel());
                    P(cat->triggerAllLevel());
                    P_(expRecLevel);
                    P_(expPassLevel);
                    P_(expTrigLevel);
                    P(expTrigAllLevel);
                }

                ASSERTV(i, expRecLevel     == cat->recordLevel());
                ASSERTV(i, expPassLevel    == cat->passLevel());
                ASSERTV(i, expTrigLevel    == cat->triggerLevel());
                ASSERTV(i, expTrigAllLevel == cat->triggerAllLevel());
            }

            Obj::shutDownSingleton();
            ASSERT(false == Obj::isInitialized());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'initSingleton(allocator)'
        //
        // Concerns:
        //: 1 We want to make sure that we get a valid singleton and that it is
        //:   initialized properly, and remains valid as long as we need it,
        //:   and we get access to the singleton object correctly.
        //
        // Plan:
        //: 1 Construct a default singleton logger manager
        //:
        //: 2 Verify that currently installed global allocator was used to
        //:   create singleton object.
        //:
        //: 3 Verify that the singleton object was initialized with the default
        //:   values.
        //
        // Testing:
        //   LoggerManager& initSingleton(bslma::Allocator *globalAlloc = 0);
        //   LoggerManager& singleton();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING 'initSingleton(allocator)'" << endl
                         << "==================================" << endl;

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",   veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ASSERT(false == Obj::isInitialized());
        Obj::initSingleton(&sa);
        ASSERT(true == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, &sa, 0);

        ASSERTV(sa.numBlocksInUse(), 0 != sa.numBlocksInUse());

        Obj::shutDownSingleton();

        ASSERT(false == Obj::isInitialized());
        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(ga.numBlocksTotal(), 0 == ga.numBlocksTotal());
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'initSingleton'
        //
        // Concerns:
        //: 1 We want to make sure that we get a valid singleton and that it is
        //:   initialized properly, and remains valid as long as we need it,
        //:   and we get access to the singleton object correctly.
        //
        // Plan:
        //: 1 Construct a default singleton logger manager.
        //:
        //: 2 Verify that currently installed global allocator was used to
        //:   create singleton object.
        //:
        //: 3 Verify that the singleton object was initialized with the default
        //:   values.
        //
        // Testing:
        //   LoggerManager& initSingleton(bslma::Allocator *globalAlloc = 0);
        //   LoggerManager& singleton();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "TESTING 'initSingleton'" << endl
                         << "=======================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ASSERT(false == Obj::isInitialized());
        Obj::initSingleton();
        ASSERT(true == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, &ga, 0);

        Obj::shutDownSingleton();

        ASSERT(false == Obj::isInitialized());
        ASSERTV(ga.numBlocksInUse(), 0 == ga.numBlocksInUse());
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CONCERN: CATEGORY NAME FILTER CALLBACK
        //
        // Concerns:
        //   Category name filter supplied at construction is invoked when
        //   supplied at construction.
        //
        // Plan:
        //: 1 Construct a logger manager singleton, supplying category name
        //:   filter callback in configuration.
        //:
        //: 2 Set number of categories and verify that the category name filter
        //:   is invoked.
        //
        // Testing:
        //   CONCERN: CATEGORY NAME FILTER CALLBACK
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONCERN: CATEGORY NAME FILTER" << endl
                                  << "=============================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ball::LoggerManagerConfiguration mXC;
        mXC.setCategoryNameFilterCallback(&toLower);

        if (veryVerbose) { cout << "mXC: "; mXC.print(cout, -1); }

        Obj        mX(mXC);
        const Obj& X = mX;

        verifyLoggerManagerDefaults(X, &ga, 0);

        const Cat *cat1 = mX.addCategory("Bloomberg.Bal", 0, 64, 128, 255);
        ASSERT(cat1);
        ASSERT(cat1 == X.lookupCategory("Bloomberg.Bal"));
        ASSERT(cat1 == X.lookupCategory("BLOOMBERG.bal"));
        ASSERT(cat1 == X.lookupCategory("bloomberg.BAL"));
        ASSERT(0    == bsl::strcmp("bloomberg.bal", cat1->categoryName()));

        const Cat *cat2 = mX.setCategory("setCategory(const char *)");
        ASSERT(cat2);
        ASSERT(cat2 == X.lookupCategory("setCategory(const char *)"));
        ASSERT(cat2 == X.lookupCategory("setcategory(CONST char *)"));
        ASSERT(cat2 == X.lookupCategory("SETCATEGORY(CONST CHAR *)"));
        ASSERT(0    == bsl::strcmp("setcategory(const char *)",
                                   cat2->categoryName()));

        const Cat *cat3 = mX.setCategory("sink1234567890?<>,.{}[]$%^&*()");
        ASSERT(cat3);
        ASSERT(cat3 == X.lookupCategory("sink1234567890?<>,.{}[]$%^&*()"));
        ASSERT(cat3 == X.lookupCategory("SINK1234567890?<>,.{}[]$%^&*()"));
        ASSERT(0    == bsl::strcmp("sink1234567890?<>,.{}[]$%^&*()",
                                   cat3->categoryName()));

        ball::CategoryHolder cat4Holder =
                        { { ball::CategoryHolder::e_UNINITIALIZED_CATEGORY },
                                                                { 0 }, { 0 } };
        const Cat *cat4 = mX.setCategory(&cat4Holder, "sImPle");
        ASSERT(cat4);
        ASSERT(cat4 == X.lookupCategory("simple"));
        ASSERT(cat4 == cat4Holder.category());
        ASSERT(0    == bsl::strcmp("simple", cat4->categoryName()));

        Obj::shutDownSingleton();
        ASSERT(false == Obj::isInitialized());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: DEFAULT THRESHOLD LEVELS
        //
        // Concerns:
        //   Default threshhold levels supplied in the configuration are
        //   installed correctly.
        //
        // Plan:
        //: 1 Construct a logger manager singleton, supplying default
        //:   threshold levels in configuration.
        //:
        //: 2 Verify that defaults are in effect.
        //
        // Testing:
        //   CONCERN: DEFAULT THRESHOLD LEVELS
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "CONCERN: DEFAULT THRESHOLDS LEVELS" << endl
                         << "==================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ga("global",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::Default::setGlobalAllocator(&ga);

        ball::LoggerManagerDefaults mXD;
        ASSERT(0 == mXD.setDefaultLoggerBufferSizeIfValid(2048));
        ASSERT(0 == mXD.setDefaultThresholdLevelsIfValid(19, 17, 13, 11));

        ball::LoggerManagerConfiguration mXC;
        mXC.setDefaultValues(mXD);

        if (veryVerbose) { cout << "mXC: "; mXC.print(cout, -1); }

        Obj        mX(mXC);
        const Obj& X = mX;

        const Cat& dfltCat = mX.defaultCategory();
        ASSERT(0  == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                 dfltCat.categoryName()));
        ASSERT(19 == dfltCat.recordLevel());
        ASSERT(17 == dfltCat.passLevel());
        ASSERT(13 == dfltCat.triggerLevel());
        ASSERT(11 == dfltCat.triggerAllLevel());

        const Cat *cat1 = mX.addCategory("first", 64, 48, 32, 16);
        ASSERT(cat1);
        ASSERT(64  == cat1->recordLevel());
        ASSERT(48  == cat1->passLevel());
        ASSERT(32  == cat1->triggerLevel());
        ASSERT(16  == cat1->triggerAllLevel());

        const Cat *cat2 = mX.setCategory("second");
        ASSERT(cat2);
        ASSERT(19   == cat2->recordLevel());
        ASSERT(17   == cat2->passLevel());
        ASSERT(13   == cat2->triggerLevel());
        ASSERT(11   == cat2->triggerAllLevel());

        ball::CategoryHolder cat3Holder =
                  { { ball::CategoryHolder::e_UNINITIALIZED_CATEGORY },
                                                                { 0 }, { 0 } };
        const Cat *cat3 = mX.setCategory(&cat3Holder, "third");
        ASSERT(cat3);
        ASSERT(cat3 == X.lookupCategory("third"));
        ASSERT(cat3 == cat3Holder.category());
        ASSERT(19   == cat3->recordLevel());
        ASSERT(17   == cat3->passLevel());
        ASSERT(13   == cat3->triggerLevel());
        ASSERT(11   == cat3->triggerAllLevel());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: LOGGER MANAGER DEFAULTS
        //
        // Concerns:
        //: 1 We want to make sure that we get a valid singleton and that it is
        //:   initialized properly, and remains valid as long as we need it,
        //:   and we get access to the singleton object correctly.
        //
        // Plan:
        //: 1 We construct a singleton logger manager, make sure the parameters
        //:   passed in are correctly hooked up.  We also try to use the
        //:   singleton in a destructor of a static object that is defined
        //:   prior to the initialization of the singleton.
        //
        // Testing:
        //   CONCERN: LOGGER MANAGER DEFAULTS
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "CONCERN: LOGGER MANAGER DEFAULTS" << endl
                         << "================================" << endl;

        bslma::TestAllocator ga("global", veryVeryVeryVerbose);
        bslma::Default::setGlobalAllocator(&ga);

        ball::LoggerManagerConfiguration mXC;

        Obj        mX(mXC, &ga);
        const Obj& X = mX;

        verifyLoggerManagerDefaults(X, &ga, 0);

        const Cat *cat1 = mX.addCategory("Bloomberg.Bal", 64, 48, 32, 16);
        ASSERT(cat1);
        ASSERT(cat1 == X.lookupCategory("Bloomberg.Bal"));
        ASSERT( 0   == bsl::strcmp("Bloomberg.Bal", cat1->categoryName()));
        ASSERT(64   == cat1->recordLevel());
        ASSERT(48   == cat1->passLevel());
        ASSERT(32   == cat1->triggerLevel());
        ASSERT(16   == cat1->triggerAllLevel());

        const Cat *cat2 = mX.setCategory("setCategory(cont char *)");
        ASSERT(cat2);
        ASSERT(cat2 == X.lookupCategory("setCategory(cont char *)"));
        ASSERT(   0 == bsl::strcmp("setCategory(cont char *)",
                                   cat2->categoryName()));
        ASSERT(FACTORY_RECORD     == cat2->recordLevel());
        ASSERT(FACTORY_PASS       == cat2->passLevel());
        ASSERT(FACTORY_TRIGGER    == cat2->triggerLevel());
        ASSERT(FACTORY_TRIGGERALL == cat2->triggerAllLevel());

        if (veryVerbose) cout << "\tTest New Category Names with No Name"
                              << " filter" << endl;

        // Verify that when no name filter is registered, category names are
        // added unchanged.

        for (int i = 0; i < NUM_NAMES; ++i) {
            const Cat *cat = mX.setCategory(NAMES[i]);
            ASSERT(cat);
            ASSERT(0 == bsl::strcmp(NAMES[i], cat->categoryName()));
        }

        if (veryVerbose) cout << "\tTest New Category Threshold Levels"
                              << " with No Inheritance" << endl;

        // Verify that when no threshold-level-values callback is installed,
        // all new categories take the default threshold values if they are
        // not specified.

        const char *HIERNAMES[] = {
            "A",
            "A.B",
            "A.B.C.D",
            "B.C",
            "B",
            "B.C.A",
            "B.A.C",
            "AA",
            "BB",
            "Aa.BB",
            "A.B.C.D.E.F.G"
        };
        enum { NUM_HIERNAMES = sizeof HIERNAMES / sizeof *HIERNAMES };

        const int recLevel     = X.defaultRecordThresholdLevel();
        const int passLevel    = X.defaultPassThresholdLevel();
        const int trigLevel    = X.defaultTriggerThresholdLevel();
        const int trigAllLevel = X.defaultTriggerAllThresholdLevel();

        ASSERT(FACTORY_RECORD     == recLevel);
        ASSERT(FACTORY_PASS       == passLevel);
        ASSERT(FACTORY_TRIGGER    == trigLevel);
        ASSERT(FACTORY_TRIGGERALL == trigAllLevel);

        for (int i = 0; i < NUM_HIERNAMES; ++i) {
            const char *NAME = HIERNAMES[i];
            const Cat  *cat  = mX.setCategory(NAME);

            ASSERT(cat);
            ASSERTV(i, 0            == bsl::strcmp(NAME, cat->categoryName()));
            ASSERTV(i, recLevel     == cat->recordLevel());
            ASSERTV(i, passLevel    == cat->passLevel());
            ASSERTV(i, trigLevel    == cat->triggerLevel());
            ASSERTV(i, trigAllLevel == cat->triggerAllLevel());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'createLoggerManager'
        //
        // Concerns:
        //: 1 An object created with the value constructor (with or without a
        //:   supplied allocator) has the contractually specified value.
        //:
        //: 2 If an allocator is NOT supplied to the value constructor, the
        //:   currently installed default allocator becomes the object
        //:   allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        // Plan:
        //: 1 Using a loop-based approach, create three distinct objects, in
        //:   turn, but configured differently: (a) without passing an
        //:   allocator, (b) passing a null allocator address explicitly, and
        //:   (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator.
        //:
        //:   2 Use the value constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-1).
        //:
        //:   3 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the constructed value.  (C-1)
        //:
        //:   4 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //
        // Testing:
        //   createLoggerManager(manager, configuration, *basicAllocator = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING 'createLoggerManager'" << endl
                                  << "=============================" << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator ga("global",    veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);
            bslma::Default::setGlobalAllocator(&ga);

            ball::LoggerManagerConfiguration mXC;

            bslma::ManagedPtr<Obj>  objPtr;
            bslma::TestAllocator   *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  if (veryVerbose) {
                      cout << "\tTesting with no allocator." << endl;
                  }
                  Obj::createLoggerManager(&objPtr, mXC);
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  if (veryVerbose) {
                      cout << "\tTesting with null allocator."
                           << endl;
                  }
                  Obj::createLoggerManager(&objPtr,
                                           mXC,
                                           (bslma::Allocator*)(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  if (veryVerbose) {
                      cout << "\tTesting with an supplied allocator."
                           << endl;
                  }
                  Obj::createLoggerManager(&objPtr, mXC, &sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  continue;
              }
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? ga : da;

            // Verify no allocation from the non-object allocators.
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, &oa == X.allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERTV(CONFIG, 0 == X.observer());  // no observers
#endif

            ASSERT(FACTORY_RECORD     == X.defaultRecordThresholdLevel());
            ASSERT(FACTORY_PASS       == X.defaultPassThresholdLevel());
            ASSERT(FACTORY_TRIGGER    == X.defaultTriggerThresholdLevel());
            ASSERT(FACTORY_TRIGGERALL == X.defaultTriggerAllThresholdLevel());

            // Reclaim dynamically allocated object under test.
            objPtr.reset();

            // Verify all memory is released on object destruction.
            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, ga.numBlocksInUse(), 0 == ga.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //: 1 An object created with the value constructor (with or without a
        //:   supplied allocator) has the contractually specified value.
        //:
        //: 2 If an allocator is NOT supplied to the value constructor, the
        //:   global allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        // Plan:
        //: 1 Using a loop-based approach, value-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   global.  For each of these three iterations:  (C-1..9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current global allocator.
        //:
        //:   2 Use the value constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-1); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the constructed value.  (C-1)
        //:
        //:   4 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //
        // Testing:
        //   LoggerManager(configuration, *globalAlloc = 0);
        //   LoggerManager(Observer*, configuration, *globalAlloc = 0);
        //   ~LoggerManager();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING CONSTRUCTORS" << endl
                                  << "===================" << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator ga("global",    veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);
            bslma::Default::setGlobalAllocator(&ga);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            ball::LoggerManagerConfiguration mXC;

            switch (CONFIG) {
              case 'a': {
                  if (veryVerbose) {
                      cout << "\tTesting default constructor." << endl;
                  }
                  objPtr = new (fa) Obj(mXC);
                  objAllocatorPtr = &ga;
              } break;
              case 'b': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with null allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(mXC, (bslma::Allocator*)(0));
                  objAllocatorPtr = &ga;
              } break;
              case 'c': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with an allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(mXC, &sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  continue;
              }
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? da : ga;

            // Verify no allocation from the non-object allocators.
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, &oa == X.allocator());
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
            ASSERTV(CONFIG, 0 == X.observer());  // no observers
#endif
            ASSERT(FACTORY_RECORD     == X.defaultRecordThresholdLevel());
            ASSERT(FACTORY_PASS       == X.defaultPassThresholdLevel());
            ASSERT(FACTORY_TRIGGER    == X.defaultTriggerThresholdLevel());
            ASSERT(FACTORY_TRIGGERALL == X.defaultTriggerAllThresholdLevel());

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(CONFIG, ga.numBlocksInUse(), 0 == ga.numBlocksInUse());
            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }

// TBD The constructor tested below is not even 'public' in 'bael'.  Leave this
// code here (temporarily) in case the constructor that *is* 'public' in 'bael'
// needs to be added to 'ball' (in which case the configuration and observer
// arguments will have to be swapped below).
#if 0
        // Legacy ctor taking mandatory observer
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator ga("global",    veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);
            bslma::Default::setGlobalAllocator(&ga);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            ball::StreamObserver             observer(&cout);
            ball::LoggerManagerConfiguration mXC;

            switch (CONFIG) {
              case 'a': {
                  if (veryVerbose) {
                      cout << "\tTesting default constructor." << endl;
                  }
                  objPtr = new (fa) Obj(mXC, &observer);
                  objAllocatorPtr = &ga;
              } break;
              case 'b': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with null allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(mXC,
                                        &observer,
                                        (bslma::Allocator*)(0));
                  objAllocatorPtr = &ga;
              } break;
              case 'c': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with an allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(mXC, &observer, &sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              }
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? da : ga;

            // Verify no allocation from the non-object allocators.
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, &oa       == mX.allocator());
            ASSERTV(CONFIG, &observer == mX.observer());

            ASSERT(FACTORY_RECORD     == mX.defaultRecordThresholdLevel());
            ASSERT(FACTORY_PASS       == mX.defaultPassThresholdLevel());
            ASSERT(FACTORY_TRIGGER    == mX.defaultTriggerThresholdLevel());
            ASSERT(FACTORY_TRIGGERALL == mX.defaultTriggerAllThresholdLevel());

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(CONFIG, ga.numBlocksInUse(), 0 == ga.numBlocksInUse());
            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bsl::shared_ptr<ball::TestObserver> observer =
                                   bsl::make_shared<ball::TestObserver>(&cout);

        Cnf nameFilter(&toLower);

        Obj::DefaultThresholdLevelsCallback thresholdsCallback(
                                                      &inheritThresholdLevels);

        Obj::UserFieldsPopulatorCallback populator(&myUserFieldsPopulator);

        ball::LoggerManagerConfiguration mXC;
        mXC.setCategoryNameFilterCallback(nameFilter);
        mXC.setDefaultThresholdLevelsCallback(thresholdsCallback);
        mXC.setUserFieldsPopulatorCallback(populator);

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ball::LoggerManagerScopedGuard lmGuard(observer.get(), mXC);
#else
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(0 == Obj::singleton().registerObserver(observer, "TO"));
#endif

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;


#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        ASSERT(observer.get() == X.observer());
#endif
        ASSERT(1              == X.numCategories());

        const Cat& dfltCat = mX.defaultCategory();
        ASSERT(&dfltCat == X.lookupCategory(DEFAULT_CATEGORY_NAME));
        ASSERT(0 == bsl::strcmp(DEFAULT_CATEGORY_NAME,
                                dfltCat.categoryName()));
        ASSERT(FACTORY_RECORD     == dfltCat.recordLevel());
        ASSERT(FACTORY_PASS       == dfltCat.passLevel());
        ASSERT(FACTORY_TRIGGER    == dfltCat.triggerLevel());
        ASSERT(FACTORY_TRIGGERALL == dfltCat.triggerAllLevel());

        const Cat *cat1 = mX.addCategory("BLOOMBERG.BAL", 64, 48, 32, 16);
        ASSERT(cat1);
        ASSERT(2    == X.numCategories());
        ASSERT(cat1 == X.lookupCategory("BLOOMBERG.bal"));
        ASSERT( 0   == bsl::strcmp("bloomberg.bal", cat1->categoryName()));
        ASSERT(64   == cat1->recordLevel());
        ASSERT(48   == cat1->passLevel());
        ASSERT(32   == cat1->triggerLevel());
        ASSERT(16   == cat1->triggerAllLevel());

        const Cat *cat2 = mX.setCategory("Bloomberg.BAL.LOGGING");
        ASSERT(cat2);
        ASSERT(3    == X.numCategories());
        ASSERT(cat2 == X.lookupCategory("BLOOMBERG.BAL.LOGGING"));
        ASSERT(0    == bsl::strcmp("bloomberg.bal.logging",
                                   cat2->categoryName()));
        ASSERT(64   == cat2->recordLevel());
        ASSERT(48   == cat2->passLevel());
        ASSERT(32   == cat2->triggerLevel());
        ASSERT(16   == cat2->triggerAllLevel());

        const Cat *cat3 = mX.setCategory("BLOOMBERG");
        ASSERT(cat3);
        ASSERT(4     == mX.numCategories());
        ASSERT(cat3  == mX.lookupCategory("Bloomberg"));
        ASSERT(0     == bsl::strcmp("bloomberg", cat3->categoryName()));

        ASSERT(FACTORY_RECORD     == cat3->recordLevel());
        ASSERT(FACTORY_PASS       == cat3->passLevel());
        ASSERT(FACTORY_TRIGGER    == cat3->triggerLevel());
        ASSERT(FACTORY_TRIGGERALL == cat3->triggerAllLevel());

        Cat *cat4 = mX.setCategory(
               "Bloomberg.Bal.Logging.Component.LoggerManager.Test.Breathing",
               65, 49, 33, 17);
        ASSERT(cat4);
        ASSERT(5    == X.numCategories());
        ASSERT(cat4 == X.lookupCategory(
              "BLOOMBERG.bal.LOGGING.component.LOGGERMANAGER.test.BREATHING"));
        ASSERT(0    == bsl::strcmp(
                "bloomberg.bal.logging.component.loggermanager.test.breathing",
                cat4->categoryName()));
        ASSERT(65   == cat4->recordLevel());
        ASSERT(49   == cat4->passLevel());
        ASSERT(33   == cat4->triggerLevel());
        ASSERT(17   == cat4->triggerAllLevel());

        if (veryVerbose) {
            const char *categoryName = cat1->categoryName();
            int         record       = cat1->recordLevel();
            int         pass         = cat1->passLevel();
            int         trigger      = cat1->triggerLevel();
            int         triggerAll   = cat1->triggerAllLevel();
            P(categoryName);
            T_; P_(record); P_(pass); P_(trigger); P(triggerAll);

            categoryName = cat2->categoryName();
            record       = cat2->recordLevel();
            pass         = cat2->passLevel();
            trigger      = cat2->triggerLevel();
            triggerAll   = cat2->triggerAllLevel();
            P(categoryName);
            T_; P_(record); P_(pass); P_(trigger); P(triggerAll);

            categoryName = cat3->categoryName();
            record       = cat3->recordLevel();
            pass         = cat3->passLevel();
            trigger      = cat3->triggerLevel();
            triggerAll   = cat3->triggerAllLevel();
            P(categoryName);
            T_; P_(record); P_(pass); P_(trigger); P(triggerAll);

            categoryName = cat4->categoryName();
            record       = cat4->recordLevel();
            pass         = cat4->passLevel();
            trigger      = cat4->triggerLevel();
            triggerAll   = cat4->triggerAllLevel();
            P(categoryName);
            T_; P_(record); P_(pass); P_(trigger); P(triggerAll);
        }

        ball::Logger& logger = mX.getLogger();

        if (veryVerbose) observer->setVerbose(1);
        logger.logMessage(*cat1, 64, __FILE__, __LINE__,
                          "First attempt to log to cat1.");

        logger.logMessage(*cat2, my_Severity::e_WARN, __FILE__, __LINE__,
                          "First attempt to log to cat2.");

        logger.logMessage(*cat3, my_Severity::e_WARN, __FILE__, __LINE__,
                          "First attempt to log to cat3.");

        cat4->setLevels(my_Severity::e_TRACE,
                        my_Severity::e_WARN,
                        my_Severity::e_ERROR,
                        my_Severity::e_FATAL);
        ASSERT(my_Severity::e_TRACE == cat4->recordLevel());
        ASSERT(my_Severity::e_WARN  == cat4->passLevel());
        ASSERT(my_Severity::e_ERROR == cat4->triggerLevel());
        ASSERT(my_Severity::e_FATAL == cat4->triggerAllLevel());

        logger.logMessage(*cat4, my_Severity::e_WARN, __FILE__, __LINE__,
                          "First attempt to log to cat4.");

        logger.logMessage(*cat1, my_Severity::e_WARN, __FILE__, __LINE__,
                          "Second attempt to log to cat1.");

        logger.logMessage(*cat4, my_Severity::e_ERROR, __FILE__, __LINE__,
                          "Second attempt to log to cat4.");

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CONCERN: LEGACY OBSERVER LIFETIME
        //  Legacy observer lifetime can be shorter that the lifetime of the
        //  logger manager singleton.  This test verifies that we can (under
        //  some conditions) shutdown logger manager singleton without
        //  application crash.  Note that this test can potentially crash the
        //  test driver.
        //
        // Concerns:
        //: 1 Verify that we log a message to 'stderr' if the registered
        //:   observer is destroyed before the logger manager.
        //
        // Plan:
        //: 1 Create an observer, and supply it to a logger manager, then
        //:   destroy the observer prior to destroying the logger manager.
        //:   Visually verify that output is written to stderr.  (C-1)
        //
        //  Note that this is a test of undefined behavior and must be run,
        //  and verified, manually.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCERN: LEGACY OBSERVER LIFETIME" << endl
                          << "=================================" << endl;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        {
            ball::TestObserver testObserver(&cout);
            ball::LoggerManagerConfiguration mXC;

            ball::LoggerManager::initSingleton(&testObserver, mXC);
        }
        // Registered legacy observer goes out of scope and destroyed.
        ball::LoggerManager::shutDownSingleton();
#endif

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // CONCERN: PERFORMANCE IMPLICATIONS
        //
        // Concerns:
        //   'logMessage' should run efficiently when no record is stored,
        //   and relatively efficient when records need to be stored.
        //   LEGACY TEST. INCOMPLETE(?)
        //
        // Plan:
        //   Invoke 'logMessage' in a loop for the default number of times
        //   or else for an integral multiple thereof as specified in the
        //   second argument ('argv[2]') to the executable.
        //
        // Testing:
        //   CONCERN: PERFORMANCE IMPLICATIONS
        // --------------------------------------------------------------------

        ball::LoggerManagerConfiguration mXC;

        ASSERT(false == Obj::isInitialized());
        ball::LoggerManagerScopedGuard lmGuard(mXC);
        ASSERT(true  == Obj::isInitialized());

        Obj&       mX = Obj::singleton();
        const Obj& X  = mX;

        verifyLoggerManagerDefaults(X, 0, 0);

        mX.addCategory("PerformanceTest", 200, 150, 100, 50);

        const int num_logs = verbose && atoi(argv[2]) > 0
                      ? atoi(argv[2])
                      : 0;

        if (verbose) cout << "Performance test for 'logMessage' "
                          << num_logs << " times.\n" << endl;

        if (verbose) cout << "No observers." << endl;
        doPerformanceTest(mX, num_logs, verbose);
        if (verbose) cout << "-----------------------------\n\n" << endl;

        bsl::shared_ptr<PerformanceObserver> observer1
                                                   (new PerformanceObserver());
        ASSERT(0 == mX.registerObserver(observer1, "O1"));

        if (verbose) cout << "Single observer." << endl;
        doPerformanceTest(mX, num_logs, verbose);
        if (verbose) cout << "-----------------------------\n\n" << endl;

        bsl::shared_ptr<PerformanceObserver> observer2
                                                   (new PerformanceObserver());
        ASSERT(0 == mX.registerObserver(observer2, "O2"));

        if (verbose) cout << "Two observers." << endl;
        doPerformanceTest(mX, num_logs, verbose);
        if (verbose) cout << "-----------------------------\n\n" << endl;

        bsl::shared_ptr<PerformanceObserver> observer3
                                                   (new PerformanceObserver());
        ASSERT(0 == mX.registerObserver(observer3, "O3"));

        if (verbose) cout << "Two observers." << endl;
        doPerformanceTest(mX, num_logs, verbose);
        if (verbose) cout << "-----------------------------\n\n" << endl;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
