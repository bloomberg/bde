// ball_log.t.cpp                                                     -*-C++-*-
#include <ball_log.h>

#include <ball_administration.h>
#include <ball_attribute.h>
#include <ball_attributecontainer.h>
#include <ball_attributecontainerlist.h>
#include <ball_attributecontext.h>
#include <ball_scopedattribute.h>
#include <ball_defaultattributecontainer.h>
#include <ball_fileobserver2.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_predicate.h>
#include <ball_record.h>
#include <ball_recordstringformatter.h>
#include <ball_rule.h>
#include <ball_streamobserver.h>
#include <ball_testobserver.h>
#include <ball_thresholdaggregate.h>
#include <ball_userfields.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_currenttime.h>
#include <bdlt_date.h>
#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strlen(), strcmp(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_fstream.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_utility.h>

#ifdef BSLS_PLATFORM_OS_UNIX
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#include <bsl_c_stdio.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
// Undefine some awkwardly named Windows macros that interfere with this cpp
// file, but only after the last #include.
# undef ERROR
#endif

// Warning: the following 'using' declarations interfere with the testing of
// the macros defined in this component.  Please do not un-comment them.
//
// using namespace BloombergLP;
// using namespace bsl;
//
// Also note that such a 'using' in the unnamed namespace applies to all code
// that follows the closing of the namespace.

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a large number of preprocessor macros
// and a small number of 'static' utility functions.
//
// The utility functions are merely facades for methods defined in the
// 'ball_loggermanager' component.  It is sufficient to test that each "facade"
// method correctly forwards its arguments to the corresponding method of
// 'ball_loggermanager', and that the correct value is returned.
//
// The preprocessor macros are largely implemented in terms of the utility
// functions.  Each macro is individually tested to ensure that the macro's
// arguments are correctly forwarded and that the side-effects of the macro
// match the expected behavior.
// ----------------------------------------------------------------------------
// [ 1] static char *s_cachedMessageBuffer;
// [ 1] static int s_cachedMessageBufferSize;
// [ 1] static char *messageBuffer();
// [ 1] static int messageBufferSize();
// [ 1] static void logMessage(*category, severity, *file, line, *msg);
// [ 1] static const ball::Category *setCategory(const char *categoryName);
// ----------------------------------------------------------------------------
// [ 2] BALL_LOG_SET_CATEGORY
// [ 2] BALL_LOG_CATEGORY
// [ 2] BALL_LOG_THRESHOLD
// [ 3] PRINTF-STYLE MACROS
// [ 4] OSTREAM MACROS (WITHOUT CALLBACK)
// [ 5] TESTING MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
// [ 6] TESTING THE C++ MACRO WHEN LOGGING RETURNED VALUE OF A FUNCTION
// [ 7] TESTING THE DEFAULT LOG ORDER (LIFO)
// [ 8] TESTING THE FIFO LOG ORDER
// [ 9] CONCURRENT LOGGING TEST
// [10] C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS
// [11] C++ MACRO PERFORMANCE TEST WITH 1 THREAD
// [12] PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS
// [13] PRINTF MACRO PERFORMANCE TEST WITH 1 THREAD
// [14] BALL_IS_ENABLED(SEVERITY) UTILITY MACRO
// [15] STRESS TEST
// [16] OSTREAM MACROS WITH CALLBACK
// [17] TESTING CALLBACK MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
// [18] BALL_LOG_SET_DYNAMIC_CATEGORY
// [19] ball::Log_Stream
// [20] bool isCategoryEnabled(Holder *holder, int severity);
// [21] BALL_LOG_SET_CATEGORY and BALL_LOG_TRACE WITH MULTIPLE THREADS
// [22] BALL_LOG_SET_DYNAMIC_CATEGORY and BALL_LOG_TRACE WITH MULTIPLE THREADS
// [23] BALL_LOG_SET_CATEGORY and BALL_LOGVA WITH MULTIPLE THREADS
// [24] BALL_LOG_SET_DYNAMIC_CATEGORY and BALL_LOGVA WITH MULTIPLE THREADS
// [25] RULE-BASED LOGGING: bool isCategoryEnabled(Holder *, int);
// [26] RULE-BASED LOGGING: logMessage(const Category *, int, Record *);
// [27] BALL_LOG_IS_ENABLED(SEVERITY)
// [28] BALL_LOG_SET_CLASS_CATEGORY(CATEGORY)
// [29] BALL_LOG_SET_NAMESPACE_CATEGORY(CATEGORY)
// [33] setCategoryHierarchically(const char *);
// [33] setCategoryHierarchically(CategoryHolder *, const char *);
// [34] BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY(const char *);
// [34] BALL_LOG_SET_CATEGORY_HIERARCHICALLY(const char *);
// [35] BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY(const char *);
// ----------------------------------------------------------------------------
// [30] CONCERN: 'BALL_LOG_*_BLOCK' MACROS
// [31] CONCERN: 'BALL_LOGCB_*_BLOCK' MACROS
// [32] CONCERN: DEGENERATE LOG MACROS USAGE
// [36] CONCERN: The logging macros can be used recursively
// [37] USAGE EXAMPLE
// [38] RULE-BASED LOGGING USAGE EXAMPLE
// [39] CLASS-SCOPE LOGGING USAGE EXAMPLE
// [40] BASIC LOGGING USAGE EXAMPLE

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

typedef BloombergLP::ball::Log                Obj;
typedef BloombergLP::ball::Log_Stream         LogStream;

typedef BloombergLP::ball::Category           Cat;
typedef BloombergLP::ball::CategoryHolder     Holder;
typedef BloombergLP::ball::CategoryManager    CategoryManager;
typedef BloombergLP::ball::LoggerManager      LoggerManager;
typedef BloombergLP::ball::Severity           Sev;
typedef BloombergLP::ball::TestObserver       TestObserver;
typedef BloombergLP::ball::ThresholdAggregate Thresholds;

typedef BloombergLP::bslma::TestAllocator     TestAllocator;

typedef BloombergLP::bsls::Types::IntPtr      IntPtr;

const int TRACE = Sev::e_TRACE;
const int DEBUG = Sev::e_DEBUG;
const int INFO  = Sev::e_INFO;
const int WARN  = Sev::e_WARN;
const int ERROR = Sev::e_ERROR;
const int FATAL = Sev::e_FATAL;
const int OFF   = Sev::e_OFF;

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

TestAllocator ta("u::ta");

class TempDirectoryGuard {
    // This class implements a scoped temporary directory guard.  The guard
    // tries to create a temporary directory in the system-wide temp directory
    // and falls back to the current directory.

    // DATA
    bsl::string                    d_dirName;      // path to the created
                                                   // directory

    BloombergLP::bslma::Allocator *d_allocator_p;  // memory allocator (held,
                                                   // not owned)

  private:
    // NOT IMPLEMENTED
    TempDirectoryGuard(const TempDirectoryGuard&);
    TempDirectoryGuard& operator=(const TempDirectoryGuard&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TempDirectoryGuard,
                                   BloombergLP::bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TempDirectoryGuard(
                             BloombergLP::bslma::Allocator *basicAllocator = 0)
        // Create temporary directory in the system-wide temp or current
        // directory.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
    : d_dirName(BloombergLP::bslma::Default::allocator(basicAllocator))
    , d_allocator_p(BloombergLP::bslma::Default::allocator(basicAllocator))
    {
        bsl::string tmpPath(d_allocator_p);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        char tmpPathBuf[MAX_PATH];
        GetTempPath(MAX_PATH, tmpPathBuf);
        tmpPath.assign(tmpPathBuf);
#else
        const char *envTmpPath = bsl::getenv("TMPDIR");
        if (envTmpPath) {
            tmpPath.assign(envTmpPath);
        }
#endif

        int res = BloombergLP::bdls::PathUtil::appendIfValid(&tmpPath,
                                                             "ball_");
        ASSERTV(tmpPath, 0 == res);

        res = BloombergLP::bdls::FilesystemUtil::createTemporaryDirectory(
                                                                    &d_dirName,
                                                                    tmpPath);
        ASSERTV(tmpPath, 0 == res);
    }

    ~TempDirectoryGuard()
        // Destroy this object and remove the temporary directory (recursively)
        // created at construction.
    {
        BloombergLP::bdls::FilesystemUtil::remove(d_dirName, true);
    }

    // ACCESSORS
    const bsl::string& getTempDirName() const
        // Return a 'const' reference to the name of the created temporary
        // directory.
    {
        return d_dirName;
    }
};

void executeInParallel(
                     int                                            numThreads,
                     BloombergLP::bslmt::ThreadUtil::ThreadFunction func)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Number each thread (sequentially from 0 to 'numThreads-1') by passing
    // 'i' to i'th thread.  Finally join all the threads.
{
    using namespace BloombergLP;  // okay here

    bslmt::ThreadUtil::Handle *threads =
                                     new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (IntPtr i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (IntPtr i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

const int FILL = 0xbb;

inline
char *messageBuffer()
{
    BloombergLP::bslmt::Mutex *mutex = 0;
    int                        bufferSize = 0;
    char                      *buffer =
                                 Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return buffer;
}

inline
int messageBufferSize()
{
    BloombergLP::bslmt::Mutex *mutex = 0;
    int                        bufferSize = 0;
    Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return bufferSize;
}

static bool isRecordOkay(
           const bsl::shared_ptr<BloombergLP::ball::TestObserver>&  observer,
           const BloombergLP::ball::Category                       *category,
           int                                                      severity,
           const char                                              *fileName,
           int                                                      lineNumber,
           const char                                              *message)
    // Return 'true' if the last record published to the specified 'observer'
    // includes the name of the specified 'category' and the specified
    // 'severity', 'fileName', 'lineNumber', and 'message', and 'false'
    // otherwise.
{
    const BloombergLP::ball::RecordAttributes& attributes =
                                 observer->lastPublishedRecord().fixedFields();

    bool status = 0   == bsl::strcmp(category->categoryName(),
                                     attributes.category())
        && severity   == attributes.severity()
        && 0          == bsl::strcmp(fileName, attributes.fileName())
        && lineNumber == attributes.lineNumber()
        && 0          == bsl::strcmp(message, attributes.message());

    if (!status) {
        P_(category->categoryName());  P(attributes.category());
        P_(severity);                  P(attributes.severity());
        P_(fileName);                  P(attributes.fileName());
        P_(lineNumber);                P(attributes.lineNumber());
        P(message);                    P(attributes.message());
    }

    return status;
}

static int numIncCallback = 0;
void incCallback(BloombergLP::ball::UserFields *list)
{
    ASSERT(list);
    ++numIncCallback;
    return;
}

class CerrBufferGuard {
    // Capture the 'streambuf' used by 'cerr' at this object's creation, and
    // restore that to be the 'cerr' stream buffer on this object's
    // destruction.

    // DATA
    bsl::stringstream  d_stream;
    bsl::streambuf    *d_cerrBuf;

  public:
    CerrBufferGuard()
        : d_cerrBuf(bsl::cerr.rdbuf(d_stream.rdbuf())) {}
        // Set a stream buffer associated with the 'd_stream' data member to be
        // the associated stream buffer used by 'bsl::cerr'.  Capture the
        // current stream buffer being used by 'bsl::cerr' and upon this
        // object's destruction, set it to be the associated stream buffer used
        // by 'bsl::cerr'.

    ~CerrBufferGuard() { bsl::cerr.rdbuf(d_cerrBuf); }
        // Restore the stream buffer being used by 'bsl::cerr' to that which
        // was being used on this object's construction.

    // ACCESSORS
    bsl::string str() const { return d_stream.str(); }
        // Return a copy of the 'd_stream' buffer's string.

};

void logNamespaceOverride() {
    // Override the outer logging category and log a test message.
    BALL_LOG_SET_CATEGORY("BALL_LOG.T.OVERRIDE.U");
    BALL_LOG_INFO << "INFO log in namespace BALL_LOG.T.OVERRIDE.U";
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLE 8
// ----------------------------------------------------------------------------

namespace BloombergLP {

///Example 8: Class-Scope Logging
/// - - - - - - - - - - - - - - -
// The following example demonstrates how to define and use logging categories
// that have class scope.
//
// First, we define a class, 'Thing', for which we want to do class-scope
// logging.  The use of the 'BALL_LOG_SET_CLASS_CATEGORY' macro generates the
// requisite declarations within the definition of the class.  We have used the
// macro in a 'private' section of the interface, which should be preferred,
// but 'public' (or 'protected') is fine, too:
//..
    // pckg_thing.h
    namespace pckg {

    class Thing {
        // ...

      private:
        BALL_LOG_SET_CLASS_CATEGORY("PCKG.THING");
            // class-scope category

      public:
        // ...

        // MANIPULATORS
        void outOfLineMethodThatLogs(bool useClassCategory);
            // Log to the class-scope category "PCKG.THING" if the specified
            // 'useClassCategory' flag is 'true', and to the block-scope
            // category "X.Y.Z" otherwise.

        // ...

        // ACCESSORS
        void inlineMethodThatLogs() const;
            // Log a record to the class-scope category "PCKG.THING".
    };
//..
// Next, we define the 'inlineMethodThatLogs' method 'inline' within the header
// file and log to the class-scope category using 'BALL_LOG_TRACE'.  Since
// there is no other category in scope, the record is necessarily logged to the
// "PCKG.THING" category that is within the scope of the 'Thing' class:
//..
    // ...

    // ACCESSORS
    inline
    void Thing::inlineMethodThatLogs() const
    {
        BALL_LOG_TRACE << "log to PCKG.THING";
    }

    }  // close namespace pckg
//..
// Now, we define the 'outOfLineMethodThatLogs' method within the '.cpp' file.
// On each invocation, this method logs one record using 'BALL_LOG_TRACE'.  It
// logs to the "PCKG.THING" class-scope category if 'useClassCategory' is
// 'true', and logs to the "X.Y.Z" block-scope category otherwise:
//..
    // pckg_thing.cpp
    namespace pckg {

    // ...

    // MANIPULATORS
    void Thing::outOfLineMethodThatLogs(bool useClassCategory)
    {
        if (useClassCategory) {
            BALL_LOG_TRACE << "log to PCKG.THING";
        }
        else {
            BALL_LOG_SET_CATEGORY("X.Y.Z");
            BALL_LOG_TRACE << "log to X.Y.Z";
        }
    }

    }  // close namespace pckg
//..
// Finally, note that both block-scope and class-scope categories can be logged
// to within the same block.  For example, the following block within a 'Thing'
// method would first log to "PCKG.THING" then log to "X.Y.Z":
//..
//      {
//          BALL_LOG_TRACE << "log to PCKG.THING";
//
//          BALL_LOG_SET_CATEGORY("X.Y.Z");
//
//          BALL_LOG_TRACE << "log to X.Y.Z";
//      }
//..

}  // close enterprise namespace

// ============================================================================
//                             USAGE EXAMPLE 7
// ----------------------------------------------------------------------------

namespace BloombergLP {

class Point {
    int d_x;
    int d_y;
    bsl::string d_name;
  public:
    Point() : d_x(0), d_y(0), d_name("") {}
    int x() const { return d_x; };
    int y() const { return d_y; };
    const bsl::string& name() const { return d_name; };
};

///Example 7: Logging Using a Callback
///- - - - - - - - - - - - - - - - - -
// The following example demonstrates how to register a logging callback.  The
// C++ stream-based macros that take a callback are particularly useful to
// seamlessly populate the user fields of a record, thus simplifying the
// logging line.
//
// We define a callback function 'populateUsingPoint' that appends to the
// specified 'fields' the attributes of the 'point' to log:
//..
    void populateUsingPoint(ball::UserFields *fields, const Point& point)
        // Append to the specified 'list' the name, x value, and y value of
        // the specified 'point'.
    {
        fields->appendString(point.name());
        fields->appendInt64(point.x());
        fields->appendInt64(point.y());
    }

    int validatePoint(const Point& point)
    {
        BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//..
// We now bind our callback function 'populateUsingPoint' and the supplied
// 'point' to a functor object we will pass to the logging callback.  Note
// that the callback supplied to the logging macro must match the prototype
// 'void (*)(ball::UserFields *)'.
//..
        bsl::function <void(ball::UserFields *)> callback;
        callback = bdlf::BindUtil::bind(&populateUsingPoint,
                                       bdlf::PlaceHolders::_1,
                                       point);

        int numErrors = 0;
        if (point.x() > 255) {
            BALL_LOGCB_ERROR(callback) << "X > 255";
            ++numErrors;
        }
        if (point.x() < -255) {
            BALL_LOGCB_ERROR(callback) << "X < -255";
            ++numErrors;
        }
        if (point.y() > 255) {
            BALL_LOGCB_ERROR(callback) << "Y > 255";
            ++numErrors;
        }
        if (point.y() < -255) {
            BALL_LOGCB_ERROR(callback) << "Y < -255";
            ++numErrors;
        }
        return numErrors;
    }
//..

}  // close enterprise namespace

// ============================================================================
//                             USAGE EXAMPLE 6
// ----------------------------------------------------------------------------

namespace BloombergLP {

///Example 6: Rule-Based Logging
///- - - - - - - - - - - - - - -
// The following example demonstrates the use of attributes and rules to
// conditionally enable logging.
//
// We start by defining a function, 'processData', that is passed data in a
// 'vector<char>' and information about the user who sent the data.  This
// example function performs no actual processing, but does log a single
// message at the 'ball::Severity::e_DEBUG' threshold level.  The 'processData'
// function also adds the user information passed to this function to the
// thread's attribute context.  We will use these attributes later, to create a
// logging rule that enables verbose logging only for a particular user.
//..
void processData(int                      uuid,
                 int                      luw,
                 int                      terminalNumber,
                 const bsl::vector<char>& data)
    // Process the specified 'data' associated with the specified Bloomberg
    // 'uuid', 'luw', and 'terminalNumber'.
{
    (void)data;  // suppress "unused" warning
//..
// We add our attributes using 'ball::ScopedAttribute', which adds an attribute
// container with one attribute to a list of containers.  This is easy and
// efficient if the number of attributes is small, but should not be used if
// there are a large number of attributes.  If motivated, we could use
// 'ball::DefaultAttributeContainer', which provides an efficient container for
// a large number of attributes, or even create a more efficient attribute
// container implementation specifically for these three attributes (uuid, luw,
// and terminalNumber).  See {'ball_scopedattributes'} (plural) for an example
// of using a different attribute container, and {'ball_attributecontainer'}
// for an example of creating a custom attribute container.
//..
    // We use 'ball::ScopedAttribute' here because the number of
    // attributes is relatively small.
//
    ball::ScopedAttribute uuidAttribute("uuid", uuid);
    ball::ScopedAttribute luwAttribute("luw", luw);
    ball::ScopedAttribute termNumAttribute("terminalNumber",
                                           terminalNumber);
//..
// In this simplified example we perform no actual processing, and simply log
// a message at the 'ball::Severity::e_DEBUG' level.
//..
    BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//
    BALL_LOG_DEBUG << "An example message";
//..
// Notice that if we were not using a "scoped" attribute container like that
// provided automatically by 'ball::ScopedAttribute' (e.g., if we were using a
// local 'ball::DefaultAttributeContainer' instead), then the container
// **must** be removed from the 'ball::AttributeContext' before it is
// destroyed!  See 'ball_scopedattributes' (plural) for an example.
//..
}
//..

}  // close enterprise namespace


// ============================================================================
//                         CASE 35 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_35 {

template <int DEPTH>
int recurseStreamBasedMacros(BloombergLP::ball::Severity::Level level)
    // Recursively invoke itself and pass the result to the stream-based
    // logging macro corresponding to the specified 'level' and return
    // 'level'.
{
    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOG_FATAL << "Inner FATAL["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      case Sev::e_ERROR: {
        BALL_LOG_ERROR << "Inner ERROR["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      case Sev::e_WARN: {
        BALL_LOG_WARN  << "Inner WARN["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      case Sev::e_INFO: {
        BALL_LOG_INFO  << "Inner INFO["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      case Sev::e_DEBUG: {
        BALL_LOG_DEBUG << "Inner DEBUG["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      case Sev::e_TRACE: {
        BALL_LOG_TRACE << "Inner TRACE["
                       << DEPTH
                       << "] "
                       << recurseStreamBasedMacros<DEPTH - 1>(level);
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }

    return static_cast<int>(level);
}

template <>
int recurseStreamBasedMacros<0>(BloombergLP::ball::Severity::Level level)
    // Invoke the stream-based logging macro corresponding to the specified
    // 'level' and return 'level'.
{
    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOG_FATAL << "Inner FATAL[0] " << static_cast<int>(level);
      } break;
      case Sev::e_ERROR: {
        BALL_LOG_ERROR << "Inner ERROR[0] " << static_cast<int>(level);
      } break;
      case Sev::e_WARN: {
        BALL_LOG_WARN  << "Inner WARN[0] "  << static_cast<int>(level);
      } break;
      case Sev::e_INFO: {
        BALL_LOG_INFO  << "Inner INFO[0] "  << static_cast<int>(level);
      } break;
      case Sev::e_DEBUG: {
        BALL_LOG_DEBUG << "Inner DEBUG[0] " << static_cast<int>(level);
      } break;
      case Sev::e_TRACE: {
        BALL_LOG_TRACE << "Inner TRACE[0] " << static_cast<int>(level);
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }

    return static_cast<int>(level);
}

template <int DEPTH>
int recursePrintfStyleMacros(BloombergLP::ball::Severity::Level level)
    // Recursively invoke itself and pass the result to the 'printf'-style
    // logging macro corresponding to the specified 'level' and return
    // 'level'.
{
    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOGVA_FATAL("Inner FATAL[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      case Sev::e_ERROR: {
        BALL_LOGVA_ERROR("Inner ERROR[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      case Sev::e_WARN: {
        BALL_LOGVA_WARN( "Inner WARN[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      case Sev::e_INFO: {
        BALL_LOGVA_INFO( "Inner INFO[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      case Sev::e_DEBUG: {
        BALL_LOGVA_DEBUG("Inner DEBUG[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      case Sev::e_TRACE: {
        BALL_LOGVA_TRACE("Inner TRACE[%d] %d",
                         DEPTH,
                         recursePrintfStyleMacros<DEPTH - 1>(level));
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }

    return static_cast<int>(level);
}

template<>
int recursePrintfStyleMacros<0>(BloombergLP::ball::Severity::Level level)
    // Invoke the 'printf'-style logging macro corresponding to the specified
    // 'level' and return 'level'.
{
    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOGVA_FATAL("%s %d", "Inner FATAL[0]", static_cast<int>(level));
      } break;
      case Sev::e_ERROR: {
        BALL_LOGVA_ERROR("%s %d", "Inner ERROR[0]", static_cast<int>(level));
      } break;
      case Sev::e_WARN: {
        BALL_LOGVA_WARN( "%s %d", "Inner WARN[0]",  static_cast<int>(level));
      } break;
      case Sev::e_INFO: {
        BALL_LOGVA_INFO( "%s %d", "Inner INFO[0]",  static_cast<int>(level));
      } break;
      case Sev::e_DEBUG: {
        BALL_LOGVA_DEBUG("%s %d", "Inner DEBUG[0]", static_cast<int>(level));
      } break;
      case Sev::e_TRACE: {
        BALL_LOGVA_TRACE("%s %d", "Inner TRACE[0]", static_cast<int>(level));
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }

    return static_cast<int>(level);
}

void recurseCallback(BloombergLP::ball::UserFields *fields)
    // Stub implementation of callback for testing recursive use of logging
    // macros.
{
    (void)fields;  // suppress warning
}

void recurseStreamBasedMacrosCallback(
                                    BloombergLP::ball::UserFields      *fields,
                                    BloombergLP::ball::Severity::Level  level)
    // Invoke the function, that calls stream-based logging macro and pass the
    // result to the callback logging macro corresponding to the specified
    // 'level' and return 'level'.
{
    (void)fields;  // suppress warning

    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOG_FATAL << "Inner FATAL[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      case Sev::e_ERROR: {
        BALL_LOG_ERROR << "Inner ERROR[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      case Sev::e_WARN: {
        BALL_LOG_WARN  << "Inner WARN[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      case Sev::e_INFO: {
        BALL_LOG_INFO  << "Inner INFO[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      case Sev::e_DEBUG: {
        BALL_LOG_DEBUG << "Inner DEBUG[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      case Sev::e_TRACE: {
        BALL_LOG_TRACE << "Inner TRACE[2]"
                       << recurseStreamBasedMacros<1>(level);
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }
}

void recursePrintfStyleMacrosCallback(
                                    BloombergLP::ball::UserFields      *fields,
                                    BloombergLP::ball::Severity::Level  level)
    // Invoke the function, that calls 'printf'-style logging macro and pass
    // the result to the callback logging macro corresponding to the specified
    // 'level' and return 'level'.
{
    (void)fields;  // suppress warning

    BALL_LOG_SET_CATEGORY("Recursion");

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOGVA_FATAL("Inner FATAL[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      case Sev::e_ERROR: {
        BALL_LOGVA_ERROR("Inner ERROR[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      case Sev::e_WARN: {
        BALL_LOGVA_WARN( "Inner WARN[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      case Sev::e_INFO: {
        BALL_LOGVA_INFO( "Inner INFO[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      case Sev::e_DEBUG: {
        BALL_LOGVA_DEBUG("Inner DEBUG[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      case Sev::e_TRACE: {
        BALL_LOGVA_TRACE("Inner TRACE[2] %d",
                         recursePrintfStyleMacros<1>(level));
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }
}

void recurseCallbackMacrosCallback(BloombergLP::ball::UserFields      *fields,
                                   BloombergLP::ball::Severity::Level  level)
    // Invoke the callback logging macro corresponding to the specified
    // 'level'.
{
    (void)fields;  // suppress warning

    BALL_LOG_SET_CATEGORY("Recursion");

    bsl::function <void(BloombergLP::ball::UserFields *)> callback =
                                                              &recurseCallback;

    switch (level) {
      case Sev::e_FATAL: {
        BALL_LOGCB_FATAL(callback) << "Inner FATAL";
      } break;
      case Sev::e_ERROR: {
        BALL_LOGCB_ERROR(callback) << "Inner ERROR";
      } break;
      case Sev::e_WARN: {
        BALL_LOGCB_WARN( callback) << "Inner WARN";
      } break;
      case Sev::e_INFO: {
        BALL_LOGCB_INFO( callback) << "Inner INFO";
      } break;
      case Sev::e_DEBUG: {
        BALL_LOGCB_DEBUG(callback) << "Inner DEBUG";
      } break;
      case Sev::e_TRACE: {
        BALL_LOGCB_TRACE(callback) << "Inner TRACE";
      } break;
      default: {
        ASSERT("Should not get here!" && 0);
      } break;
    }
}

}  // close namespace BALL_LOG_TEST_CASE_35

// ============================================================================
//            HIERARCHICAL CATEGORIES -- CASE 32, CASE 33, CASE 34
// ----------------------------------------------------------------------------

namespace BALL_LOG_HIERARCHICAL_CATEGORIES {

namespace Blp = BloombergLP;
typedef Blp::ball::Severity                     S;
typedef S::Level                                Lev;
typedef Blp::ball::Category                     Cat;
typedef Blp::ball::CategoryHolder               CatHolder;
typedef Blp::ball::LoggerManager::DefaultThresholdLevelsCallback
                                                DefaultThresholdLevelsCallback;
typedef Blp::ball::ThresholdAggregate           Agg;
typedef bsl::pair<const Cat *, bool>            ResultPair;

Agg callbackLevels;     // The function 'dtlCallbackRaw' below sets the values
                        // returned through the first four arguments of its
                        // parameter list to the values of the four fields of
                        // this 'struct', respectively.

void dtlCallbackRaw(int        *recordLevel,
                    int        *passLevel,
                    int        *triggerLevel,
                    int        *triggerAllLevel,
                    const char *categoryName)
    // Callback to be used by the logger manager to obtain new logging
    // threshold levels for a new category that is about to be created.  For
    // the purposes of this test, return values through the specified
    // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'.
    // Verify that no category with the specified 'categoryName' exists.  The
    // behavior is undefined if this method is called when the logger manager
    // singleton is not initialized.
{
    ASSERT(0 == Blp::ball::LoggerManager::singleton().lookupCategory(
                                                                categoryName));

    *recordLevel     = callbackLevels.recordLevel();
    *passLevel       = callbackLevels.passLevel();
    *triggerLevel    = callbackLevels.triggerLevel();
    *triggerAllLevel = callbackLevels.triggerAllLevel();
}

Blp::ball::LoggerManager::DefaultThresholdLevelsCallback dtlCallback(
                                                              &dtlCallbackRaw);
    // The method 'setDefaultThresholdLevelsCallback' won't take just a
    // function ptr, it needs a pointer to this 'bsl::function' type.

Agg getLevels(const Blp::ball::Category *category)
    // Return a 'ThresholdAggregate' object reflecting the threshold levels of
    // the specified 'category'.
{
    return Agg(category->recordLevel(),
               category->passLevel(),
               category->triggerLevel(),
               category->triggerAllLevel());
}

Agg getDefaultLevels(const Blp::ball::LoggerManager *manager)
    // Return a threshold aggregate object reflecting the default levels of the
    // specified 'manager'.
{
    return Agg(manager->defaultRecordThresholdLevel(),
               manager->defaultPassThresholdLevel(),
               manager->defaultTriggerThresholdLevel(),
               manager->defaultTriggerAllThresholdLevel());
}

void setLevels(Blp::ball::Category *category, const Agg& agg)
    // Set the threshold levels of the specified 'category' to those of the
    // specified 'agg'.
{
    category->setLevels(agg.recordLevel(),
                        agg.passLevel(),
                        agg.triggerLevel(),
                        agg.triggerAllLevel());
}

// The table 'DATA' is used the same way in test cases 32, 33, and 34.
//
// Each record of the table 'DATA' contains a line number, a threshold
// aggregate value, and two booleans, one of which indicates that the
// thresholds are to be used, in the outer 'ti' loop in each test case, to
// initialize the threshold aggregate 'callbackLevels' above, and the rest of
// the loop skipped, and another boolean indicating that in the inner 'tj' loop
// in each test case, the logger manager is to be set to use the default
// threshold level callback.

#undef SET_CB
#undef USE_CB
#undef NO_CB

#define SET_CB(record, pass, trigger, triggerAll)                             \
            Agg(S::e_ ## record, S::e_ ## pass, S::e_ ## trigger,             \
                                              S::e_ ## triggerAll), true, false

#define USE_CB(record, pass, trigger, triggerAll)                             \
            Agg(S::e_ ## record, S::e_ ## pass, S::e_ ## trigger,             \
                                              S::e_ ## triggerAll), false, true

#define NO_CB( record, pass, trigger, triggerAll)                             \
            Agg(S::e_ ## record, S::e_ ## pass, S::e_ ## trigger,             \
                                             S::e_ ## triggerAll), false, false

static const struct Data {
    int  d_line;
    Agg  d_levels;
    bool d_setCallbackThresholds;
    bool d_useCallback;
} DATA[] = {
    { L_, SET_CB(DEBUG, INFO , WARN , ERROR) },
    { L_, NO_CB( INFO , INFO , INFO , INFO ) },
    { L_, USE_CB(TRACE, DEBUG, INFO , WARN ) },
    { L_, NO_CB( WARN , ERROR, FATAL, OFF  ) },
    { L_, NO_CB( OFF  , FATAL, ERROR, WARN ) },
    { L_, NO_CB( TRACE, INFO , WARN , ERROR) },
    { L_, NO_CB( ERROR, WARN , INFO , TRACE) },
    { L_, USE_CB(TRACE, TRACE, DEBUG, INFO ) },
    { L_, SET_CB(TRACE, WARN , ERROR, FATAL) },
    { L_, NO_CB( INFO , WARN , ERROR, FATAL) },
    { L_, NO_CB( FATAL, ERROR, WARN , INFO ) },
    { L_, NO_CB( DEBUG, DEBUG, DEBUG, DEBUG) },
    { L_, NO_CB( WARN , WARN , WARN , WARN ) },
    { L_, NO_CB( ERROR, ERROR, ERROR, ERROR) },
    { L_, NO_CB( FATAL, FATAL, FATAL, FATAL) },
    { L_, USE_CB(DEBUG, DEBUG, INFO , WARN ) }
};
enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

#undef SET_CB
#undef USE_CB
#undef NO_CB

const Cat *defaultCat = 0;

namespace indices {

// In test cases 33, 34, and 35, we want to call 'setCategoryHierarchically',
// often with category holder objects passed.  Also, the d'tor of the category
// manager (part of the logger manager) will traverse all the category holders
// that have been initialized, resetting them.  So it's necessary that the
// lifetime of any of the category holders exceeds the lifetime of the logger
// manager.
//
// This problem is solved by having templates with integer arguments.  In test
// cases 33, we use template function 'testSetCategoryHierarchically', in test
// case 34, we use template function 'testLocalMacros' defined below, in test
// case 35, it is template class 'TestClassMacro' defined further below.  Thus,
// we can instantiate these templates with many different integer values, and
// get a different instantiation of the static category holder in each one.
//
// In this namespace 'indices' we have a set of distinct integer constants to
// be used as template parameters to the templates used in test cases 33, 34,
// and 35.
//
// All the calls to these functions, including 'TestClassMacro::operator()',
// are done through macros defined in the test cases in 'main'.  The names of
// the integers below are passed as an arg to these macros, and used twice --
// as an integer to parameterize the template, and the name is used (using
// preprocessor stringification, i.e., '#argName').  Before being used as
// category names, the names below have everything at or after '_' removed,
// which enables us to use multiple indexes with the same category name.  For
// example, 'woof_a', 'woof_b', 'woof_c', 'woof_d', and 'woof_e' are 5 distinct
// integer values, but all refer to the same category name "woof".

static const int base = L_ + 1;
static const int bowWow              = L_ - base;
static const int woof_a              = L_ - base;    // Creates "woof".
static const int meow                = L_ - base;
static const int meowBubble          = L_ - base;
static const int meowBark            = L_ - base;
static const int meowArf             = L_ - base;
static const int woof_b              = L_ - base;    // Finds "woof".
static const int woof_c              = L_ - base;    // Finds "woof".
static const int _a                  = L_ - base;    // Finds default category
                                                     // named "".
static const int meowPurr            = L_ - base;
static const int classDerivedDerived = L_ - base;
static const int classDerived_a      = L_ - base;
static const int classDerived_b      = L_ - base;
static const int meowBarkArf         = L_ - base;
static const int roar                = L_ - base;
static const int classDerivedRoar    = L_ - base;
static const int growl               = L_ - base;
static const int woof_d              = L_ - base;    // Finds "woof".
static const int secondClass         = L_ - base;

// All of the indexes after this are used in tests that occur with the logger
// manager full, and thus cannot create new categories -- they either match
// existing categories, or find the default category.

static const int whimper             = L_ - base;    // Finds "".
static const int woofMoan            = L_ - base;    // Finds "".
static const int woof_e              = L_ - base;    // Finds "woof', an
                                                     // existing category,
                                                     // though the logger
                                                     // manager is full.
static const int full                = L_ - base;    // Finds "".

}  // close namespace indices

const static CatHolder defaultHolder =
                     { { CatHolder::e_UNINITIALIZED_CATEGORY }, { 0 }, { 0 } };

struct PositionRec {
    // If an ASSERT fails within the 'testLocalMacros()' call or the
    // 'TestClassMacro::operator()()' call, the line number provided by ASSERT
    // will not tell us WHICH call failed, so we bundle useful information
    // about which call it was where the problem occurred, and pass an object
    // of this type to the call.  We define a streaming operator so the object
    // knows how to print itself, and then make the 'PositionRec' the first
    // argument to every 'ASSERTV' call in the test functions.

    // DATA
    int         d_line;            // line number of the call from the test
                                   // case to the test function
    int         d_il;              // the line number of the record in 'DATA'
                                   // that the 'ti' loop in the test case is
                                   // tracking
    int         d_jl;              // the line number of the record in 'DATA'
                                   // that the 'tj' loop in the test case is
                                   // tracking
    const char *d_categoryName_p;  // the name of the category to be found or
                                   // created (prior to '_*' being removed)

    // CREATOR
    PositionRec(int line, int il, int jl, const char *categoryName)
    : d_line(line)
    , d_il(il)
    , d_jl(jl)
    , d_categoryName_p(categoryName)
        // Create a 'PositionRec' object with the specified 'line', 'il', 'jl',
        // and 'categoryName'.
    {}
};

bsl::ostream& operator<<(bsl::ostream& stream, const PositionRec& pos)
    // Output the specified 'pos' to the specified 'stream'.
{
    stream << "{ line: "  << pos.d_line <<
                 " IL: "  << pos.d_il <<
                 " JL: "  << pos.d_jl <<
                 " cat: " << pos.d_categoryName_p << " }";
    return stream;
}

struct ResultRec {
    // Both of the functions 'testLocalMacros' and 'TestClassMacro::operator()'
    // return objects of this type, which contains several fields relevant to
    // how the function performed -- pointers to the category holder, to the
    // category found or created, and a 'bool' to indicate whether all the
    // tests passed.  A separate static copy of this record exists for every
    // template instantiation of 'result<int>()', which is called by both the
    // other template functions, with the same template parameter that the
    // other templates are instantiated with -- that allows us to retrieve the
    // results from any previous call to the test function at any time in the
    // loop.

    // DATA
    const CatHolder      *d_holder_p;        // the category holder, if any
                                             // initialized
    int                   d_intLevel;        // the threshold level from the
                                             // category holder, as an 'int'
    Lev                   d_level;           // the threshold level from the
                                             // category holder, as a
                                             // 'Severity::Level'
    const Cat            *d_category_p;      // the category found or created
    CatHolder            *d_next_p;          // the 'd_next_p' field from the
                                             // category holder

    // MANIPULATORS
    void clear()
        // Zero out all fields.
    {
        bsl::memset(this, 0, sizeof(*this));
    }

    void init(const CatHolder *holder_p)
        // Initialize all fields of this object relevant to the specified
        // 'holder_p'.
    {
        d_holder_p   = holder_p;
        d_intLevel   =
                 Blp::bsls::AtomicOperations::getInt(&holder_p->d_threshold);
        d_level      = static_cast<Lev>(d_intLevel);
        d_category_p = static_cast<const Blp::ball::Category *>(
                 Blp::bsls::AtomicOperations::getPtr(&holder_p->d_category_p));
        d_next_p     = static_cast<CatHolder *>(
                 Blp::bsls::AtomicOperations::getPtr(&holder_p->d_next_p));
    }
};

template <int KK>
ResultRec& result()
    // Return a separate static instance of a 'ResultRec' for each unique value
    // of 'KK'.
{
    static ResultRec ret;
    return ret;
}

// This set contains all the 'KK' values with which a couple of the following 3
// templates have been instantiated.  It is cleared at the beginning of every
// pass through the 'tj' loops in 'main' in test cases 32, 33, and 34.  In test
// case 32 it is used to determine if we are repeating a use of 'KK', in test
// cases 33 and 34 it is used for a sanity check.

bsl::set<int> instantiationSet(&u::ta);

// The following 'enum' is the type of an argument to
// 'testSetCategoryHierarchically' that determines whether we will make a call
// to the single argument (i.e., "holderless") version of
// 'setCategoryHierarchically' before or after the two argument call, or never
// call the single argument version at all.

enum HolderlessMode { e_HOLDERLESS_NONE,
                      e_HOLDERLESS_BEFORE,
                      e_HOLDERLESS_AFTER };

template <int KK>
ResultRec& testSetCategoryHierarchically(const Agg&       expectedLevels,
                                         PositionRec      pos,
                                         const CatHolder *prevCatHolder,
                                         HolderlessMode   holderlessMode)
    // Call 'ball::Log::setCategoryHierarchically', possibly multiple times.
    // If a category is found or created, verify that the logging threshold
    // levels match the specified 'expectedLevels'.  The category name to be
    // passed to the call is contained in the specified 'pos', which is also to
    // be passed as the first argument to every 'ASSERTV' in this function.
    // Call the function under test with a category holder.  Verify that the
    // 'd_next_p' field of the category holder is set to equal the specified
    // 'prevCatHolder'.  Depending on the value of the specified
    // 'holderlessMode', do an additional call to
    // 'ball::Log::setCategoryHierarchically' with no category holder passed,
    // either before the call with a category, or after, or not at all, and
    // verify that the category pointer returned matches the one obtained from
    // the call with a category holder.  If the logger manager singleton is
    // initialized, do a second call to the function under test with a second
    // category holder, and observe that the second category holder is linked
    // back to the first one.
{
    const bool repeat = instantiationSet.count(KK);
    instantiationSet.insert(KK);

    // If there's a logger manager, get a ptr to it.

    Blp::ball::LoggerManager *manager_p =
                                      Blp::ball::LoggerManager::isInitialized()
                                      ? &Blp::ball::LoggerManager::singleton()
                                      : 0;

    // 'pos.d_categoryName_p' is the name of one of the variables in the
    // 'indices' sub-namespace.  It is very desirable in this test to:
    //
    //: 1 have multiple indices with the same category name
    //:
    //: 2 test looking up the default category "" by name, and it's hard to
    //:   give a variable that name
    //
    // We accomplish both of these goals by trimming everything beginning with
    // '_' from the variable name to yield the category name.

    bsl::string categoryNameStr(pos.d_categoryName_p, &u::ta);
    bsl::size_t u = bsl::min(categoryNameStr.find('_'),
                             categoryNameStr.length());
    categoryNameStr.resize(u);
    const char *categoryName = categoryNameStr.c_str();

    const Cat *prevCat = !manager_p
                       ? 0
                       : manager_p->lookupCategory(categoryName);
    ASSERTV(pos, prevCat || !repeat);

    // If there's a logger manager, is it full?  (We expect
    // 'setCategoryHierarchically' to return the default category if it is,
    // unless the category previously existed.)

    const bool managerFull = manager_p && manager_p->numCategories() ==
                                                 manager_p->maxNumCategories();

    // What is the initial number of categories in the logger manager?

    const int  initNumCat  = manager_p ? manager_p->numCategories() : 0;

    // Do we expect a new category to be created?

    const bool expNewCat   = manager_p && !managerFull && !prevCat &&
                                                                 *categoryName;

    // How many categories do we expect there to be in the logger manager after
    // the call?

    const int  expNumCat   = initNumCat + expNewCat;

    const Cat *holderlessCat = 0;
    if (e_HOLDERLESS_BEFORE == holderlessMode) {
        holderlessCat =
                       Blp::ball::Log::setCategoryHierarchically(categoryName);
    }

    // Note that there will be a separate instance of 'holderA' for every
    // distinct value of 'KK'.

    static CatHolder holderA = defaultHolder;
    ASSERTV(pos, repeat || !bsl::memcmp(&holderA,
                                        &defaultHolder,
                                        sizeof(holderA)));

    // Call the function under test.

    const Cat *retCat =
             Blp::ball::Log::setCategoryHierarchically(&holderA, categoryName);

    ResultRec& ret = result<KK>();
    ret.clear();
    ret.init(&holderA);

    ASSERTV(prevCatHolder, ret.d_next_p, prevCatHolder == ret.d_next_p);

    if (manager_p) {
        // The logger manager singleton was initialized.  We ALWAYS expect a
        // category returned.

        ASSERTV(pos, retCat);
        ASSERTV(pos, ret.d_category_p);
        ASSERTV(pos, retCat == ret.d_category_p);

        // Verify the number of categories is as expected.

        ASSERTV(pos, initNumCat, manager_p->numCategories(), expNumCat,
                                      manager_p->numCategories() == expNumCat);
        ASSERTV(pos, getLevels(retCat), expectedLevels,
                                          getLevels(retCat) == expectedLevels);
        ASSERTV(pos, ret.d_intLevel, Agg::maxLevel(expectedLevels),
                              Agg::maxLevel(expectedLevels) == ret.d_intLevel);

        if (!managerFull || prevCat) {
            ASSERTV(pos, categoryNameStr == retCat->categoryName());
        }
        if (prevCat) {
            ASSERTV(pos, retCat == prevCat);
        }

        if (managerFull && !prevCat) {
            // The logger manager was full before the call, so we expect the
            // default category to have been returned.

            ASSERTV(pos, &manager_p->defaultCategory() == retCat);
            ASSERTV(pos, !bsl::strcmp(retCat->categoryName(), ""));
        }

        // If we just created a new category with name 'categoryName', we
        // expect 'next_p' to be 0, otherwise, we expect it to be a link to the
        // previous category holder.

        ASSERTV(pos, prevCat, ret.d_next_p, retCat->categoryName(),
                                      defaultCat == retCat || repeat ||
                                                  !!prevCat == !!ret.d_next_p);

        // Call the function under test again, with the same category name and
        // a different category holder.  Like 'holderA', we will get a separate
        // copy of 'holderB' for every distinct value of 'KK'.

        static CatHolder holderB = defaultHolder;
        ASSERTV(pos, repeat || !bsl::memcmp(&holderB,
                                            &defaultHolder,
                                            sizeof(holderB)));
        const Cat *retCatB = Blp::ball::Log::setCategoryHierarchically(
                                                                 &holderB,
                                                                 categoryName);
        ret.clear();
        ret.init(&holderB);

        ASSERTV(pos, retCatB);
        ASSERTV(pos, ret.d_category_p);
        ASSERTV(pos, retCatB == ret.d_category_p);
        ASSERTV(pos, retCatB == retCat);

        // The 2nd call shouldn't have changed the # of categories.

        ASSERTV(pos, initNumCat, manager_p->numCategories(), expNumCat,
                                      manager_p->numCategories() == expNumCat);
        ASSERTV(pos, getLevels(retCatB), expectedLevels,
                                         getLevels(retCatB) == expectedLevels);
        ASSERTV(pos, ret.d_intLevel, Agg::maxLevel(expectedLevels),
                              Agg::maxLevel(expectedLevels) == ret.d_intLevel);

        if (!managerFull || prevCat) {
            ASSERTV(pos, categoryNameStr == retCatB->categoryName());
        }
        if (prevCat) {
            ASSERTV(pos, retCatB == prevCat);
        }

        if (managerFull && !prevCat) {
            // The logger manager was full before the call, so we expect the
            // default category to have been returned.

            ASSERTV(pos, &manager_p->defaultCategory() == retCatB);
            ASSERTV(pos, !bsl::strcmp(retCatB->categoryName(), ""));
        }

        ASSERTV(pos, ret.d_next_p, &holderA, ret.d_next_p == &holderA);

        if (e_HOLDERLESS_AFTER == holderlessMode) {
            holderlessCat = Blp::ball::Log::setCategoryHierarchically(
                                                                 categoryName);
        }
    }
    else {
        // The logger manager singleton was not initialized, so the function
        // should have had no effect on 'holder'.

        ASSERTV(pos, 0 == bsl::memcmp(&holderA,
                                      &defaultHolder,
                                      sizeof(holderA)));
    }

    ASSERTV(pos, static_cast<int>(holderlessMode),
          (e_HOLDERLESS_NONE == holderlessMode ? 0 : retCat) == holderlessCat);

    return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -- Test case 33 uses only things above here.  'Flags' & 'testLocalMacros'
// are used by test case 34, and 'TestClassMacro' is used by test case 35.

// 'Flags' is the type of the last argument passed to 'testLocalMacros<int>()',
// which indicates whether the category sought is to be static (the default) or
// dynamic, and whether we've called the same template instantiation of the
// function before.

enum Flags { k_DYNAMIC_BIT          = 1,    // "dynamic" macro is to be called
             k_REPEAT_BIT           = 2,    // category holder is already
                                            // initialized

             e_STATIC               = 0,
             e_DYNAMIC              = k_DYNAMIC_BIT,
             e_REPEAT_STATIC        = k_REPEAT_BIT,
             e_REPEAT_DYNAMIC       = k_REPEAT_BIT | k_DYNAMIC_BIT };

template <int KK>
ResultRec&
testLocalMacros(const Agg&          newLevels,
                const PositionRec&  pos,
                const CatHolder    *prevCatHolder,
                const Flags         flags)
    // Depending on the specified 'flags', call either
    // 'BALL_LOG_SET_CATEGORY_HIERARCHICALLY' or
    // 'BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY' with a category name
    // field of the specified 'pos'.  Verify that the specified 'newLevels'
    // match the threshold levels of the category found or created.  Verify
    // that the 'd_next_p' field of the category holder of the macro call
    // matches the specified 'prevCatHolder'.  Verify that the "repeat" bit in
    // 'flags' matches whether this is the first time this function was called
    // with a given value of the specified 'KK' for a given pass through the
    // test loop.  All the values of 'KK' are provided by the 'int' constants
    // in the sub-namespace 'indices' within this test namespace.
{
    const bool dynamic = flags & k_DYNAMIC_BIT;
    const bool repeat  = flags & k_REPEAT_BIT;

    if (repeat) {
        ASSERTV(pos, repeat, instantiationSet.count(KK) && "non-repeat");
    }
    else {
        ASSERTV(pos, repeat, !instantiationSet.count(KK) && "called twice");
    }
    instantiationSet.insert(KK);

    // If there's a logger manager, get a ptr to it.

    Blp::ball::LoggerManager *manager_p =
                                      Blp::ball::LoggerManager::isInitialized()
                                      ? &Blp::ball::LoggerManager::singleton()
                                      : 0;

    // 'pos.d_categoryName_p' is the name of one of the variables in the
    // 'indices' sub-namespace.  It is very desirable in this test to:
    //
    //: 1 have multiple indices with the same category name
    //:
    //: 2 test looking up the default category "" by name, and it's hard to
    //:   give a variable that name
    //
    // We accomplish both of these goals by trimming everything beginning with
    // '_' from the variable name to yield the category name.

    bsl::string categoryNameStr(pos.d_categoryName_p, &u::ta);
    bsl::size_t u = bsl::min(categoryNameStr.find('_'),
                             categoryNameStr.length());
    categoryNameStr.resize(u);
    const char *categoryName = categoryNameStr.c_str();

    ResultRec& ret = result<KK>();
    ret.clear();

    const Cat *prevCat = !manager_p
                       ? 0
                       : manager_p->lookupCategory(categoryName);
    const bool managerFull = manager_p && manager_p->numCategories() ==
                                                 manager_p->maxNumCategories();

    if (dynamic) {
        BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY(categoryName);
        ret.init(&BALL_LOG_CATEGORYHOLDER);

        ASSERTV(pos, ret.d_intLevel, CatHolder::e_DYNAMIC_CATEGORY,
                              CatHolder::e_DYNAMIC_CATEGORY == ret.d_intLevel);
        ASSERTV(pos, 0 == ret.d_next_p);
    }
    else {
        BALL_LOG_SET_CATEGORY_HIERARCHICALLY(categoryName);
        ret.init(&BALL_LOG_CATEGORYHOLDER);

        if (manager_p) {
            ASSERTV(pos, ret.d_level, Agg::maxLevel(newLevels),
                                      ret.d_level == Agg::maxLevel(newLevels));
            ASSERTV(pos, newLevels, getLevels(ret.d_category_p),
                                     newLevels == getLevels(ret.d_category_p));
            ASSERTV(pos, prevCatHolder, ret.d_next_p,
                                                prevCatHolder == ret.d_next_p);
        }
        else {
            ASSERTV(pos, 0 == bsl::memcmp(ret.d_holder_p,
                                          &defaultHolder,
                                          sizeof(*ret.d_holder_p)));

            ASSERTV(pos, ret.d_intLevel, CatHolder::e_UNINITIALIZED_CATEGORY,
                        CatHolder::e_UNINITIALIZED_CATEGORY == ret.d_intLevel);
            ASSERTV(pos, 0 == ret.d_next_p);
        }
    }

    ASSERTV(pos, (!prevCat || prevCat == ret.d_category_p) && "new cat");
    if (manager_p) {
        const char *expCatName = prevCat || !managerFull ? categoryName
                                                         : "";

        ASSERTV(pos, ret.d_category_p);
        ASSERTV(pos, ret.d_category_p->categoryName(), categoryName,expCatName,
                   !bsl::strcmp(ret.d_category_p->categoryName(), expCatName));
        ASSERTV(pos, newLevels, getLevels(ret.d_category_p),
                                     newLevels == getLevels(ret.d_category_p));
    }
    else {
        ASSERTV(pos, 0 == ret.d_category_p);
    }

    return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// -- Test case 35 uses some of the things above here, and everything below
// here, while test case 33 & test case 34 use only things above here.

template <int KK>
class TestClassMacro {
    // The purpose of this 'class' is to test
    // 'BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY'.  We want to call many
    // instantiations of the macro, and by instantiating this 'class' with
    // values of the template parameter 'KK' taken from constants in the
    // 'indices' sub-namespace, we get them.  Each instantiation of the macro
    // creates a separate static category holder, so we get a separate one for
    // every instantiation of this template 'class'.
    //
    // The macro under test, 'BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY',
    // takes a 'const char *' parameter for the category name.  So how to pass
    // different category names for different class instantiations on differing
    // values of 'KK'?  It turns out that that parameter to the macro doesn't
    // have to be a string literal, it can be a function call, so we make a
    // method in this 'class', 'getClassCategoryNameBuffer', that returns a
    // static buffer, a different one for every value of 'KK'.  The same method
    // is called in the 'operator()' function before the class category is
    // searched for or created, and we populate the buffer with the desired
    // class name, so that when we later call 'ball_log_getCategoryHolder', the
    // macro calls its 'getClassCategoryNameBuffer()' argument which returns
    // the buffer that has been populated with the desired category name.
    //
    // 'operator()' either finds or creates the desired category, and verifies
    // that the category holder is linked appropriately.

    // TYPES
    enum { k_CATEGORY_NAME_BUFFER_LEN = 40 };   // 40 chars is much longer
                                                // than any of the category
                                                // names under test.

    // CLASS METHOD
    static
    char *getClassCategoryNameBuffer()
        // Return a separate buffer for each value of template parameter 'KK'.
    {
        static char s_classCategoryName[k_CATEGORY_NAME_BUFFER_LEN];
        return s_classCategoryName;
    }

    BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY(getClassCategoryNameBuffer());

  public:
    // MANIPULATOR
    ResultRec& operator()(const Agg&            classLevels,
                          const PositionRec&    pos,
                          const CatHolder      *prevClassCatHolder,
                          bool                  repeat);
        // Access the class category whose name is indicated by the specified
        // 'pos', creating it if no category with that name exists.  If we
        // access the class category, verify that its threshold levels match
        // the specified 'classLevels'.  If a class category is created, verify
        // that the 'd_next_p' field of its category holder matches the
        // specified 'prevClassCatHolder'.  Obtain a static 'ResultRec'
        // corresponding to the 'KK' template index and populate its fields
        // with the results of the run, and return a reference to it.  Verify
        // that whether this instantiation of the class has been called before
        // matches the state of the specified 'repeat'.
};

// MANIPULATOR
template <int KK>
ResultRec&
TestClassMacro<KK>::operator()(const Agg&            classLevels,
                               const PositionRec&    pos,
                               const CatHolder      *prevClassCatHolder,
                               bool                  repeat)
{
    // If this is the first time we've called this method for this value of
    // 'KK', the class category holder has not been initialized until we call
    // 'ball_log_getCategoryHolder'.

    if (repeat) {
        ASSERTV(pos, repeat, instantiationSet.count(KK) && "non-repeat");
    }
    else {
        ASSERTV(pos, repeat, !instantiationSet.count(KK) && "called twice");
    }
    instantiationSet.insert(KK);

    // If there's a logger manager, get a ptr to it.

    Blp::ball::LoggerManager *manager_p =
                                      Blp::ball::LoggerManager::isInitialized()
                                      ? &Blp::ball::LoggerManager::singleton()
                                      : 0;

    // As discussed in the 'class' documentation, the name of the category
    // that will be found or created must reside in buffer provided by
    // 'getClassCategoryNameBuffer', which is 'k_CATEGORY_NAME_BUFFER_LEN'
    // long, which is much longer than any of the category names that we will
    // use.
    //
    // 'pos.d_categoryName_p' is the name of one of the variables in the
    // 'indices' sub-namespace.  It is very desirable in this test to:
    //
    //: 1 have multiple indices with the same category name
    //:
    //: 2 test looking up the default category "" by name, and it's hard to
    //:   give a variable that name
    //
    // We accomplish both of these goals by trimming everything beginning with
    // '_' from the variable name to yield the category name.

    char *categoryName = getClassCategoryNameBuffer();
    ASSERTV(pos, bsl::strlen(pos.d_categoryName_p) <
                                                   k_CATEGORY_NAME_BUFFER_LEN);
    bsl::strcpy(categoryName, pos.d_categoryName_p);
    char *pc = strchr(categoryName, '_');
    if (pc) {
        *pc = 0;
    }

    Cat *prevCat = !manager_p ? 0 : manager_p->lookupCategory(categoryName);
    ASSERTV(pos, !prevCat ||
                          !bsl::strcmp(prevCat->categoryName(), categoryName));

    const bool managerFull = manager_p && manager_p->numCategories() ==
                                                 manager_p->maxNumCategories();

    // Here is where the 'class' category holder is populated.

    const CatHolder *holder_p = ball_log_getCategoryHolder(
                                                      BALL_LOG_CATEGORYHOLDER);

    ResultRec& ret = result<KK>();
    ret.clear();
    ret.init(holder_p);

    if (manager_p) {
        ASSERTV(pos, ret.d_category_p);
        const char *expClassCatName = categoryName;
        if (managerFull && !prevCat) {
            expClassCatName = "";
            ASSERTV(pos, defaultCat == ret.d_category_p);
        }

        ASSERTV(pos, ret.d_category_p->categoryName(), expClassCatName,
              !bsl::strcmp(ret.d_category_p->categoryName(), expClassCatName));
        ASSERTV(pos, classLevels, getLevels(ret.d_category_p),
                                   classLevels == getLevels(ret.d_category_p));
        ASSERTV(pos, ret.d_level, Agg::maxLevel(classLevels),
                                    ret.d_level == Agg::maxLevel(classLevels));
    }
    else {
        ASSERTV(pos, 0 == ret.d_category_p);
        ASSERTV(pos, ret.d_intLevel, CatHolder::e_UNINITIALIZED_CATEGORY,
                        CatHolder::e_UNINITIALIZED_CATEGORY == ret.d_intLevel);
        ASSERTV(pos, 0 == ret.d_next_p);
    }
    ASSERTV(pos, prevClassCatHolder, ret.d_next_p,
                                           prevClassCatHolder == ret.d_next_p);
    ASSERTV(pos, !prevCat || prevCat == ret.d_category_p);

    return ret;
}

}  // close namespace BALL_LOG_HIERARCHICAL_CATEGORIES

// ============================================================================
//                         CASE 29 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_29 {

void globalFunctionThatLogsToLocalCategory()
    // Log a record to the block-scope category "GLOBAL CATEGORY".
{
    BALL_LOG_SET_CATEGORY("GLOBAL CATEGORY");

    BALL_LOG_INFO << "log to local category";
}

                         // =======================
                         // class ClassScopeLoggerA
                         // =======================

class ClassScopeLoggerA {

  private:
    BALL_LOG_SET_CLASS_CATEGORY("CLASS CATEGORY A");

  public:
    // CLASS METHODS
    static void classMethodThatLogsToClassCategory();
        // Log a record to the class-scope category "CLASS CATEGORY A".

    // MANIPULATORS
    void outoflineMethodThatLogsToLocalCategory();
        // Log a record to the class-scope category "CLASS CATEGORY A", then
        // log a record to the block-scope "STATIC LOCAL CATEGORY".

    // ACCESSORS
    void inlineMethodThatLogsToClassCategory() const;
        // Log a record to the class-scope category "CLASS CATEGORY A".
};

// ACCESSORS
inline
void ClassScopeLoggerA::inlineMethodThatLogsToClassCategory() const
{
    BALL_LOG_TRACE << "TRACE log to class-scope category";
}

                         // -----------------------
                         // class ClassScopeLoggerA
                         // -----------------------

// CLASS METHODS
void ClassScopeLoggerA::classMethodThatLogsToClassCategory()
{
    bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                               &u::incCallback;

    BALL_LOGCB_DEBUG(callback) << "callback DEBUG log to class-scope category";
}

// MANIPULATORS
void ClassScopeLoggerA::outoflineMethodThatLogsToLocalCategory()
{
    BALL_LOG_INFO << "INFO log to class-scope category";

    BALL_LOG_SET_CATEGORY("STATIC LOCAL CATEGORY")

    BALL_LOG_INFO << "INFO log to static local category";
}

                         // =======================
                         // class ClassScopeLoggerB
                         // =======================

class ClassScopeLoggerB {

  public:
    // CLASS METHODS
    static void classMethodThatLogsToClassCategory();
        // Log a record to the class-scope category "CLASS CATEGORY B".

    // MANIPULATORS
    void outoflineMethodThatLogsToLocalCategory();
        // Log a record to the class-scope category "CLASS CATEGORY B", then
        // log a record to the block-scope "DYNAMIC LOCAL CATEGORY".

    // ACCESSORS
    void inlineMethodThatLogsToClassCategory() const
        // Log a record to the class-scope category "CLASS CATEGORY B".
    {
        BALL_LOG_WARN << "WARN log to class-scope category";
    }

  public:
    BALL_LOG_SET_CLASS_CATEGORY("CLASS CATEGORY B");
};

                         // -----------------------
                         // class ClassScopeLoggerB
                         // -----------------------

// CLASS METHODS
void ClassScopeLoggerB::classMethodThatLogsToClassCategory()
{
    BALL_LOGVA_ERROR("variadic ERROR log to class-scope category: %d", 77);
}

// MANIPULATORS
void ClassScopeLoggerB::outoflineMethodThatLogsToLocalCategory()
{
    BALL_LOG_FATAL << "FATAL log to class-scope category";

    BALL_LOG_SET_DYNAMIC_CATEGORY("DYNAMIC LOCAL CATEGORY")

    BALL_LOG_INFO << "DEBUG log to dynamic local category";
}

                         // ===============================
                         // class template ClassScopeLogger
                         // ===============================

template <class TYPE>
class ClassScopeLogger {

  private:
    // DATA
    TYPE *d_dummy_p;  // dummy

    BALL_LOG_SET_CLASS_CATEGORY("CLASS TEMPLATE CATEGORY");

  public:
    // CLASS METHODS
    static void classMethodThatLogsToClassCategory();
        // Log a record to the class-scope category "CLASS TEMPLATE CATEGORY".

    // MANIPULATORS
    void outoflineMethodThatLogsToLocalCategory();
        // Log a record to the class-scope category "CLASS TEMPLATE CATEGORY",
        // then log a record to the block-scope "STATIC LOCAL CATEGORY".

    // ACCESSORS
    void inlineMethodThatLogsToClassCategory() const
        // Log a record to the class-scope category "CLASS TEMPLATE CATEGORY".
    {
        BALL_LOG_TRACE << "TRACE log to class-scope category";
    }
};

                         // -------------------------------
                         // class template ClassScopeLogger
                         // -------------------------------

// CLASS METHODS
template <class TYPE>
void ClassScopeLogger<TYPE>::classMethodThatLogsToClassCategory()
{
    BALL_LOGVA_ERROR("variadic ERROR log to class-scope category: %d", 77);
}

// MANIPULATORS
template <class TYPE>
void ClassScopeLogger<TYPE>::outoflineMethodThatLogsToLocalCategory()
{
    BALL_LOG_INFO << "INFO log to class-scope category";

    BALL_LOG_SET_CATEGORY("STATIC LOCAL CATEGORY")

    BALL_LOG_INFO << "INFO log to static local category";
}

}  // close namespace BALL_LOG_TEST_CASE_29

// ============================================================================
//                         CASE 24 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_24 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
double      arg1          = 0.0;
const char *arg2          = 0;
int         numIterations = 0;

extern "C" {
void *workerThread24(void *)
{
    typedef BloombergLP::ball::Severity Severity;

    BALL_LOG_SET_DYNAMIC_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BALL_LOGVA(Severity::e_TRACE, msg, arg1, arg2);
        BALL_LOGVA(Severity::e_DEBUG, msg, arg1, arg2);
        BALL_LOGVA(Severity::e_INFO, msg, arg1, arg2);
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_24

// ============================================================================
//                         CASE 23 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_23 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
double      arg1          = 0.0;
const char *arg2          = 0;
int         numIterations = 0;

extern "C" {
void *workerThread23(void *)
{
    typedef BloombergLP::ball::Severity Severity;

    BALL_LOG_SET_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BALL_LOGVA(Severity::e_TRACE, msg, arg1, arg2);
        BALL_LOGVA(Severity::e_DEBUG, msg, arg1, arg2);
        BALL_LOGVA(Severity::e_INFO, msg, arg1, arg2);
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_23

// ============================================================================
//                         CASE 22 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_22 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
int         numIterations = 0;

extern "C" {
void *workerThread22(void *)
{
    BALL_LOG_SET_DYNAMIC_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BALL_LOG_TRACE  << msg;
        BALL_LOG_DEBUG  << msg;
        BALL_LOG_INFO   << msg;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_22

// ============================================================================
//                         CASE 21 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_21 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = 0;
const char *msg           = 0;
int         numIterations = 0;

extern "C" {
void *workerThread21(void *)
{
    BALL_LOG_SET_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BALL_LOG_TRACE  << msg;
        BALL_LOG_DEBUG  << msg;
        BALL_LOG_INFO   << msg;
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_21

// ============================================================================
//                         CASE 18 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_18 {

enum {
    NUM_THREADS  = 4,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bslmt::Mutex categoryMutex;

extern "C" {
void *workerThread18(void *)
{
    BALL_LOG_SET_DYNAMIC_CATEGORY("main category");
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        if (i % 2) {
            BALL_LOG_WARN << msg;
        }
        else {
            BALL_LOG_INFO << msg;
        }
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_18

// ============================================================================
//                         CASE 17 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_17 {

static
void macrosTest(bool                                   loggerManagerExistsFlag,
                const BloombergLP::ball::TestObserver& testObserver,
                int                                    numPublishedSoFar)
    // Invoke each of the callback macros and verify the expected logging
    // behavior based on the specified 'loggerManagerExistsFlag',
    // 'testObserver', and 'numPublishedSoFar'.
{
    ASSERT(loggerManagerExistsFlag ==
                            BloombergLP::ball::LoggerManager::isInitialized());

#ifdef BSLS_PLATFORM_OS_UNIX
    // Temporarily redirect 'stderr' to a temp file.

    u::TempDirectoryGuard tempDirGuard;

    bsl::string filename(tempDirGuard.getTempDirName());
    BloombergLP::bdls::PathUtil::appendRaw(&filename, "stderrOut");

    fflush(stderr);
    int fd = creat(filename.c_str(), 0777);
    ASSERT(fd != -1);
    int saved_stderr_fd = dup(2);
    dup2(fd, 2);
    if (verbose)
        bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

    u::CerrBufferGuard cerrBufferGuard;

    bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                               &u::incCallback;

    BALL_LOG_SET_CATEGORY("Logger Manager Comes and Goes");

    if (verbose)
        bsl::cout << "Safely invoked 'BALL_LOG_SET_CATEGORY' macro."
                  << bsl::endl;

    u::numIncCallback = 0;

    BALL_LOGCB_TRACE(callback) << "Logger Manager?";
    BALL_LOGCB_DEBUG(callback) << "Logger Manager?";
    BALL_LOGCB_INFO(callback)  << "Logger Manager?";
    BALL_LOGCB_WARN(callback)  << "Logger Manager?";
    BALL_LOGCB_ERROR(callback) << "Logger Manager?";
    BALL_LOGCB_FATAL(callback) << "Logger Manager?";

    if (loggerManagerExistsFlag) {
        numPublishedSoFar += 6;
        ASSERT(6 == u::numIncCallback);
    }
    else {
        ASSERT(3 == u::numIncCallback);
    }
    ASSERT(numPublishedSoFar == testObserver.numPublishedRecords());

    if (verbose)
        bsl::cout << "Safely invoked callback macros." << bsl::endl;

#ifdef BSLS_PLATFORM_OS_UNIX
    // Restore 'stderr' to the state it was in before we redirected it.
    fflush(stderr);
    dup2(saved_stderr_fd, 2);

    // Verify the expected number of lines were written to the temp file.
    bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
    int numLines = 0;
    bsl::string line;
    while (getline(fs, line)) {
        ++numLines;
        if (veryVerbose) bsl::cout << "\t>>" << line << "<<\n";
    }
    if (loggerManagerExistsFlag) {
        ASSERTV(numLines, 0 == numLines);
    }
    else {
        ASSERTV(numLines, 3 == numLines);
    }
    fs.close();
#endif
    ASSERT("" == cerrBufferGuard.str());
}

}  // close namespace BALL_LOG_TEST_CASE_17

// ============================================================================
//                         CASE 15 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_15 {

class my_PublishCountingObserver : public BloombergLP::ball::Observer {
    // This concrete implementation of 'ball::Observer' maintains a count of
    // the number of messages published to it and gives access to that count
    // through 'publishCount'.

    // DATA
    int d_publishCount;  // count of calls made to the 'publish' method

  public:
    // CREATORS
    my_PublishCountingObserver()
    : d_publishCount(0)
        // Create a 'publish'-counting observer having an initial count of 0.
    {
    }

    ~my_PublishCountingObserver()
        // Destroy this object.
    {
    }

    // MANIPULATORS
    void publish(const BloombergLP::ball::Record&,
                 const BloombergLP::ball::Context&)
        // Increment the count maintained by this observer by 1, and ignore any
        // arguments that were supplied.
    {
        ++d_publishCount;
    }

    // ACCESSORS
    int publishCount() const
        // Return the number of times that the 'publish' method of this
        // observer has been called since construction.
    {
        return d_publishCount;
    }
};

}  // close namespace BALL_LOG_TEST_CASE_15

// ============================================================================
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_13 {

enum {
    NUM_THREADS  = 1,      // don't change
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bslmt::Mutex categoryMutex;

extern "C" {
void *workerThread13(void *)
{
    categoryMutex.lock();
    BALL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BALL_LOGVA_INFO("%s", msg);
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_13

// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_12 {

enum {
    NUM_THREADS  = 4,
    MAX_MSG_SIZE = 4000,
    NUM_MSGS     = 200
};

char message[MAX_MSG_SIZE + 1];
int randomSizes[NUM_MSGS];
BloombergLP::bslmt::Mutex categoryMutex;
extern "C" {
void *workerThread12(void *)
{
    categoryMutex.lock();
    BALL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();
    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[MAX_MSG_SIZE-randomSizes[i]];
        BALL_LOGVA_INFO("%s", msg);
    }

    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_12

// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_11 {

enum {
    NUM_THREADS  = 1,
    MAX_MSG_SIZE = 200,
    NUM_MSGS     = 1000000
};

char                      message[MAX_MSG_SIZE + 1];
int                       severity = BloombergLP::ball::Severity::e_TRACE;
BloombergLP::bslmt::Mutex categoryMutex;

extern "C" {
void *workerThread11(void *)
{
    categoryMutex.lock();
    BALL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();

    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[0];
        BALL_LOG_INFO << msg;
    }

    return NULL;
}

void *workerThread11a(void *)
{
    categoryMutex.lock();
    BALL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();

    for (int i = 0; i < NUM_MSGS; ++i) {
        char *msg = &message[0];
        BALL_LOG_STREAM(severity) << msg;
    }

    return NULL;
}

}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_11

// ============================================================================
//                         CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_10 {

enum {
    NUM_THREADS    = 4,
    MAX_MSG_SIZE   = 200,
    NUM_MSGS_INFO  = 1000 * 1000,
    NUM_MSGS_TRACE = 200 * NUM_MSGS_INFO
};

char message[MAX_MSG_SIZE + 1];
BloombergLP::ball::Severity::Level severity =
                                          BloombergLP::ball::Severity::e_TRACE;
BloombergLP::bslmt::Mutex categoryMutex;

int numMsgs()
    // Return the number of log messages to be produced for current severity
    // level.
{
    return BloombergLP::ball::Severity::e_TRACE == severity
         ? NUM_MSGS_TRACE
         : BloombergLP::ball::Severity::e_INFO  == severity
         ? NUM_MSGS_INFO
         : 0;
}

#define BALL_OLD_STREAM (ball_lOcAl_StReAm.stream())

#define BALL_OLD_LOG_REAL_END bsl::ends;                                      \
        }                                                                     \
    }                                                                         \
}
#define BALL_OLD_LOG_END BALL_OLD_LOG_REAL_END

#define BALL_LOG_STREAM_OLD(BALL_SEVERITY) {                                  \
    using namespace BloombergLP;                                              \
    if (BALL_LOG_THRESHOLD >= (BALL_SEVERITY)) {                              \
        if (ball::Log::isCategoryEnabled(                                     \
                          ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER),\
                          BALL_SEVERITY)) {                                   \
            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,   \
                                               __LINE__, BALL_SEVERITY);      \
            BALL_OLD_STREAM

#define BALL_LOG_STREAM_OLD_UNLIKELY(BALL_SEVERITY) {                         \
    using namespace BloombergLP;                                              \
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(                                \
                                 BALL_LOG_THRESHOLD >= (BALL_SEVERITY))) {    \
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                                   \
        if (ball::Log::isCategoryEnabled(                                     \
                          ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER),\
                          BALL_SEVERITY)) {                                   \
            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,   \
                                               __LINE__, BALL_SEVERITY);      \
            BALL_OLD_STREAM

#define BALL_LOG_OLD_TRACE BALL_LOG_STREAM_OLD_UNLIKELY(                      \
                                          BloombergLP::ball::Severity::e_TRACE)

#define BALL_LOG_OLD_INFO                                                     \
                       BALL_LOG_STREAM_OLD(BloombergLP::ball::Severity::e_INFO)

struct Util {
    static void *doInfoConst(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("A");

        char *msg = &message[0];
        BALL_LOG_INFO << msg;

        return NULL;
    }

    static void *doOldTraceConst(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("C");

        char *msg = &message[0];
        BALL_LOG_OLD_TRACE << msg << BALL_OLD_LOG_END;

        return NULL;
    }

    static void *doOldInfoConst(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("D");

        char *msg = &message[0];
        BALL_LOG_OLD_INFO << msg << BALL_OLD_LOG_END;

        return NULL;
    }

    static void *doOldVar(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("F");

        char *msg = &message[0];
        BALL_LOG_STREAM_OLD_UNLIKELY(severity) << msg << BALL_OLD_LOG_END;
        return NULL;
    }

    static void *doTraceConst(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("B");

        char *msg = &message[0];
        BALL_LOG_TRACE << msg;

        return NULL;
    }

    static void *doVar(void *)
        // Log test message.
    {
        BALL_LOG_SET_CATEGORY("E");

        char *msg = &message[0];
        BALL_LOG_STREAM(severity) << msg;

        return NULL;
    }

};

extern "C" {
void *workerThread10(void *)
{
    ASSERT(BloombergLP::ball::Severity::e_TRACE == severity ||
           BloombergLP::ball::Severity::e_INFO  == severity);

    for (int i = BloombergLP::ball::Severity::e_TRACE == severity
               ? NUM_MSGS_TRACE
               : NUM_MSGS_INFO; 0 < i; --i) {
        BloombergLP::bslim::TestUtil::callFunc(NULL);
    }

    return NULL;
}

}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_10

// ============================================================================
//                         CASE 9 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_9 {

enum {
    NUM_THREADS = 4,       // number of threads

    N_TOTAL     = 1000,    // total number of message logged by each thread

    N_PUBLISH   = 10,      // frequency of messages that will cause a
                           // *publish* event (see the documentation for
                           // 'workerThread9')

    N_TRIGGER   = 100,     // frequency of messages that will cause a
                           // *trigger* event (see the documentation for
                           // 'workerThread9')

    // number of messages published to the observer, that were logged with a
    // severity causing a *Record* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice to the
    // observer.  Also the value is per thread.
    EXP_N_RECORD  = N_TOTAL - N_TOTAL/N_PUBLISH,

    // number of messages published to the observer, that were logged with a
    // severity causing a *Publish* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice to the
    // observer.  Also the value is per thread.
    EXP_N_PUBLISH = 2 * (N_TOTAL/N_PUBLISH - N_TOTAL/N_TRIGGER),

    // number of messages published to the observer, that were logged with a
    // severity causing a *Trigger* event.  Note that we need to do
    // multiplication by 2, because such messages are published twice to the
    // observer.  Also the value is per thread.
    EXP_N_TRIGGER = 2 * N_TOTAL/N_TRIGGER,

    // total number of messages published to the observer by one thread
    EXP_N_TOTAL   = N_TOTAL + N_TOTAL/N_PUBLISH,
    // note that EXP_N_TOTAL = EXP_N_RECORD + EXP_N_PUBLISH + EXP_N_TRIGGER'

    // fixed record buffer limit.  The limit is big enough to accommodate all
    // the records logged to the buffer.  400 accounts *roughly* for dynamic
    // memory allocated for each record and memory required by buffer to
    // accommodate the record handles.
    REC_BUF_LIMIT = NUM_THREADS * EXP_N_TOTAL *
                                 (sizeof (BloombergLP::ball::Record) + 400)
};

enum { RECORD, PUBLISH, TRIGGER };
class my_Observer : public BloombergLP::ball::Observer {
    // This thread-safe, concrete implementation of 'ball::Observer' stores all
    // the messages published to it in a vector and provide access to the
    // vector by 'get' method.

    // DATA
    bsl::vector<bsl::string> d_publishedMessages;
    BloombergLP::bslmt::Mutex d_mutex;

  public:
    // CREATORS
    my_Observer()  {}
    ~my_Observer() {}

    // MANIPULATORS
    void publish(const BloombergLP::ball::Record&  record,
                 const BloombergLP::ball::Context&)
    {
        d_mutex.lock();
        d_publishedMessages.push_back(record.fixedFields().message());
        d_mutex.unlock();
    }

    // ACCESSORS
    const bsl::vector<bsl::string>& get() const
    {
        return d_publishedMessages;
    }

    void print() const
    {
        for (int i = 0; i < (signed) d_publishedMessages.size(); ++i) {
            bsl::cout << d_publishedMessages[i] << bsl::endl;
        }
    }
};

BloombergLP::bslmt::Mutex categoryMutex;

extern "C" {
void *workerThread9(void *arg)
{
    // Log 'N_TOTAL' messages using a loop as following:
    //
    // (a) Every 'N_TRIGGER'th message is logged with a severity that will
    //     cause a *Trigger* event.
    // (b) Every 'N_PUBLISH'th message except those described in (a), is
    //     logged with a severity that will cause a *Publish* event.
    // (c) rest of the message are logged with a severity that will cause
    //     a *Record* event.
    // Each logged message contains following information.
    // - the loop iteration number
    // - thread id
    // - the event it will cause

    const int id = static_cast<int>(
                      reinterpret_cast<BloombergLP::bsls::Types::IntPtr>(arg));

    categoryMutex.lock();
    BALL_LOG_SET_CATEGORY("main category");
    categoryMutex.unlock();

    for (int i = 0; i < N_TOTAL; ++i) {
        if ((i + 1)%N_TRIGGER == 0) {
            BALL_LOG_ERROR << id      << " "   // thread id
                           << TRIGGER << " "   // event type
                           << i       << " ";  // iteration
        }
        else if ((i + 1)%N_PUBLISH == 0) {
            BALL_LOG_WARN  << id      << " "   // thread id
                           << PUBLISH << " "   // event type
                           << i       << " ";  // iteration
        }
        else {
            BALL_LOG_TRACE << id      << " "   // thread id
                           << RECORD  << " "   // event type
                           << i       << " ";  // iteration
        }
    }

    return NULL;
}

}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_9

// ============================================================================
//                         CASE 6 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_6 {

const char *message1 = "MESSAGE-1";
const char *message2 = "MESSAGE-2";

const char *f()
{
    BALL_LOG_SET_CATEGORY("main category");
    BALL_LOG_WARN << message1;
    return message2;
}

}  // close namespace BALL_LOG_TEST_CASE_6

// ============================================================================
//                         CASE 5 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_5 {

static
void macrosTest(bool                                   loggerManagerExistsFlag,
                const BloombergLP::ball::TestObserver& testObserver,
                int                                    numPublishedSoFar)
    // Invoke each of the callback macros and verify the expected logging
    // behavior based on the specified 'loggerManagerExistsFlag',
    // 'testObserver', and 'numPublishedSoFar'.
{
    ASSERT(loggerManagerExistsFlag ==
                            BloombergLP::ball::LoggerManager::isInitialized());

#ifdef BSLS_PLATFORM_OS_UNIX
    // Temporarily redirect 'stderr' to a temp file.

    u::TempDirectoryGuard tempDirGuard;

    bsl::string filename(tempDirGuard.getTempDirName());
    BloombergLP::bdls::PathUtil::appendRaw(&filename, "stderrOut");

    fflush(stderr);
    int fd = creat(filename.c_str(), 0777);
    ASSERT(fd != -1);
    int saved_stderr_fd = dup(2);
    dup2(fd, 2);
    if (verbose)
        bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

    u::CerrBufferGuard  cerrBufferGuard;

    BALL_LOG_SET_CATEGORY("Logger Manager Comes and Goes");

    if (verbose)
        bsl::cout << "Safely invoked 'BALL_LOG_SET_CATEGORY' macro."
                  << bsl::endl;

    BALL_LOG_TRACE << "Logger Manager?";
    BALL_LOG_DEBUG << "Logger Manager?";
    BALL_LOG_INFO  << "Logger Manager?";
    BALL_LOG_WARN  << "Logger Manager?";
    BALL_LOG_ERROR << "Logger Manager?";
    BALL_LOG_FATAL << "Logger Manager?";

    if (loggerManagerExistsFlag) {
        numPublishedSoFar += 6;
    }
    ASSERT(numPublishedSoFar == testObserver.numPublishedRecords());

    if (verbose)
        bsl::cout << "Safely invoked stream-style macros." << bsl::endl;

    BloombergLP::ball::Severity::Level severity =
                                           BloombergLP::ball::Severity::e_WARN;

    BALL_LOGVA(severity, "Hello?");
    BALL_LOGVA(severity, "Hello? %d", 1);
    BALL_LOGVA(severity, "Hello? %d %d", 1, 2);
    BALL_LOGVA(severity, "Hello? %d %d %d", 1, 2, 3);
    BALL_LOGVA(severity, "Hello? %d %d %d %d", 1, 2, 3, 4);
    BALL_LOGVA(severity, "Hello? %d %d %d %d %d", 1, 2, 3, 4, 5);
    BALL_LOGVA(severity, "Hello? %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6);
    BALL_LOGVA(severity, "Hello? %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7);
    BALL_LOGVA(severity, "Hello? %d %d %d %d %d %d %d %d",
                                                     1, 2, 3, 4, 5, 6, 7, 8);
    BALL_LOGVA(severity, "Hello? %d %d %d %d %d %d %d %d %d",
                                                  1, 2, 3, 4, 5, 6, 7, 8, 9);

    if (loggerManagerExistsFlag) {
        numPublishedSoFar += 10;
    }
    ASSERT(numPublishedSoFar == testObserver.numPublishedRecords());

    if (verbose) bsl::cout << "Safely invoked printf-style macros.\n";

#ifdef BSLS_PLATFORM_OS_UNIX
    // Restore 'stderr' to the state it was in before we redirected it.
    fflush(stderr);
    dup2(saved_stderr_fd, 2);

    // Verify the expected number of lines were written to the temp file.
    bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
    int numLines = 0;
    bsl::string line;
    while (getline(fs, line)) {
        ++numLines;
        if (veryVerbose) bsl::cout << "\t>>" << line << "<<\n";
    }
    if (loggerManagerExistsFlag) {
        ASSERTV(numLines,  0 == numLines);
    }
    else {
        ASSERTV(numLines, 13 == numLines);  // 16 - 3 (not written to stderr)
    }
    fs.close();
#endif
    ASSERT("" == cerrBufferGuard.str());
}

}  // close namespace BALL_LOG_TEST_CASE_5

// ============================================================================
//                         CASE -1 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_MINUS_1 {

struct ThreadFunctor {
    void operator()()
    {
        BALL_LOG_SET_CATEGORY("CATEGORY_5");

        typedef BloombergLP::bsls::Types::Uint64 Uint64;

        const Uint64 id = BloombergLP::bslmt::ThreadUtil::selfIdAsUint64();

        while (true) {
            BALL_LOG_ERROR << "ERROR " << id;
        }
    }
};

}  // close namespace BALL_LOG_TEST_CASE_MINUS_1

// Please note that this harmlessly looking line introduces the category for
// the whole translation unit.

BALL_LOG_SET_NAMESPACE_CATEGORY("BALL_LOG.T");

namespace {
    void logNamespaceOverride()
        // Override the outer logging category and log a test message.
    {
        BALL_LOG_SET_CATEGORY("BALL_LOG.T.OVERRIDE");
        BALL_LOG_INFO << "INFO log in namespace BALL_LOG.T.OVERRIDE";
    }
}  // close unnamed namespace

namespace BALL_LOG_TEST_NAMESPACE_LOGGING {
    BALL_LOG_SET_NAMESPACE_CATEGORY("NS.A");

    void logNamespaceCategory() {
        BALL_LOG_INFO << "INFO log in namespace A";
    }

    namespace BALL_LOG_TEST_NAMESPACE_LOGGING_B {
        BALL_LOG_SET_NAMESPACE_CATEGORY_HIERARCHICALLY("NS.A.B");

        void logNamespaceCategory() {
            BALL_LOG_INFO << "INFO log in namespace A.B";
        }

    }  // close namespace BALL_LOG_TEST_NAMESPACE_LOGGING_B

}  // close namespace BALL_LOG_TEST_NAMESPACE_LOGGING

namespace BALL_LOG_TEST_NAMESPACE_LOGGING2 {

template <class TYPE>
class Test {
  public:
    int func();
        // Log a test message into local category.
};

template<class TYPE>
int Test<TYPE>::func()
{
    BALL_LOG_SET_CATEGORY("NS.LM2");
    BALL_LOG_INFO << "func";
    return 0;
}

BALL_LOG_SET_NAMESPACE_CATEGORY("NS.L2");

void func2()
    // Log a test message into namespace category.
{
    BALL_LOG_INFO << "func2";
}

}  // close namespace BALL_LOG_TEST_NAMESPACE_LOGGING2



// ============================================================================
//                         CASE -2 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_MINUS_2 {

enum {
    NUM_THREADS = 10
};

const char *categoryName  = "Performance";
int         numIterations = 100000;

extern "C" {
void *workerThreadPerformance(void *)
{
    BALL_LOG_SET_DYNAMIC_CATEGORY(categoryName);
    for (int i = 0; i < numIterations; ++i) {
        BALL_LOGVA_FATAL("%d", i);
    }
    return NULL;
}
}  // extern "C"

}  // close namespace BALL_LOG_TEST_CASE_MINUS_2

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 40: {
        // --------------------------------------------------------------------
        // BASIC LOGGING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   BASIC LOGGING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

            using namespace BloombergLP;    // OK here

            ball::LoggerManagerConfiguration lmConfig;
            lmConfig.setDefaultThresholdLevelsIfValid(Sev::e_TRACE);
            ball::LoggerManagerScopedGuard lmGuard(lmConfig);

            bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&bsl::cout);

            ball::LoggerManager::singleton().registerObserver(observer,
                                                              "default");

if (verbose) bsl::cout << "Example 1: A Basic Logging Example" << bsl::endl;
///Example 1: A Basic Logging Example
/// - - - - - - - - - - - - - - - - -
// The following trivial example shows how to use the logging macros to log
// messages at various levels of severity.
//
// First, we initialize the log category within the context of this function.
// The logging macros such as 'BALL_LOG_ERROR' will not compile unless a
// category has been specified in the current lexical scope:
//..
    BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//..
// Then, we record messages at various levels of severity.  These messages will
// be conditionally written to the log depending on the current logging
// threshold of the category (configured using the 'ball::LoggerManager'
// singleton):
//..
    BALL_LOG_FATAL << "Write this message to the log if the log threshold "
                   << "is above 'ball::Severity::e_FATAL' (i.e., 32).";

    BALL_LOG_TRACE << "Write this message to the log if the log threshold "
                   << "is above 'ball::Severity::e_TRACE' (i.e., 192).";
//..
// Next, we demonstrate how to use proprietary code within logging macros.
// Suppose you want to add the content of a vector to the log trace:
//..
    bsl::vector<int> myVector(4, 328);
    BALL_LOG_TRACE_BLOCK {
        BALL_LOG_OUTPUT_STREAM << "myVector = [ ";
        unsigned int position = 0;
        for (bsl::vector<int>::const_iterator it  = myVector.begin(),
                                              end = myVector.end();
            it != end;
            ++it, ++position) {
            BALL_LOG_OUTPUT_STREAM << position << ':' << *it << ' ';
        }
        BALL_LOG_OUTPUT_STREAM << ']';
    }
//..
// Note that the code block will be conditionally executed depending on the
// current logging threshold of the category.  The code within the block must
// not produce any side effects, because its execution depends on the current
// logging configuration.  The special macro 'BALL_LOG_OUTPUT_STREAM' provides
// access to the log stream within the code.
      } break;
      case 39: {
        // --------------------------------------------------------------------
        // CLASS-SCOPE LOGGING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   CLASS-SCOPE LOGGING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "CLASS-SCOPE LOGGING USAGE EXAMPLE\n"
                               << "=================================\n";

        using namespace BloombergLP;  // okay here

        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            ball::LoggerManagerConfiguration lmConfig;
            lmConfig.setDefaultThresholdLevelsIfValid(
                  Sev::e_TRACE,  // record level
                  Sev::e_TRACE,  // passthrough level
                  Sev::e_ERROR,  // trigger level
                  Sev::e_FATAL); // triggerAll level
            ball::LoggerManagerScopedGuard lmg(lmConfig, &ta);

            ball::LoggerManager& manager = ball::LoggerManager::singleton();

            bsl::shared_ptr<ball::TestObserver>  observer =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            ASSERT(1 == ball::TestObserver::numInstances());
            ASSERT(0 == observer->numPublishedRecords());

            pckg::Thing mX;  const pckg::Thing& X = mX;

            mX.outOfLineMethodThatLogs(true);

            ASSERT(1 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                   "PCKG.THING",
                    observer->lastPublishedRecord().fixedFields().category()));

            mX.outOfLineMethodThatLogs(false);

            ASSERT(2 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "X.Y.Z",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogs();

            ASSERT(3 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "PCKG.THING",
                    observer->lastPublishedRecord().fixedFields().category()));
        }

      } break;
      case 38: {
        // --------------------------------------------------------------------
        // RULE-BASED LOGGING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   RULE-BASED LOGGING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "RULE-BASED LOGGING USAGE EXAMPLE\n"
                               << "================================\n";

        using namespace BloombergLP;  // okay here

// Next we demonstrate how to create a logging rule that sets the pass-through
// logging threshold to 'Sev::e_TRACE' (i.e., enables verbose
// logging) for a particular user when calling the 'processData' function
// defined above.
//
// We start by creating the singleton logger manager that we configure with
// the stream observer and a default configuration.  We then call the
// 'processData' function: This first call to 'processData' will not result in
// any logged messages because 'processData' logs its message at the
// 'ball::Severity::e_DEBUG' level, which is below the default configured
// logging threshold.
//..
    ball::LoggerManagerConfiguration lmConfig;
    ball::LoggerManagerScopedGuard   lmGuard(lmConfig);

    bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&bsl::cout);

    ball::LoggerManager::singleton().registerObserver(observer, "default");

    BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");

    bsl::vector<char> message;

    BALL_LOG_ERROR << "Processing the first message.";
    processData(3938908, 2, 9001, message);
//..
// Now we add a logging rule, setting the pass-through threshold to be
// 'ball::Severity::e_TRACE' (i.e., enabling verbose logging) if the thread's
// context contains a "uuid" of 3938908.  Note that we use the wild-card
// value '*' for the category so that the 'ball::Rule' rule will apply to all
// categories.
//..
    ball::Rule rule("*", 0, ball::Severity::e_TRACE, 0, 0);
    rule.addPredicate(ball::Predicate("uuid", 3938908));
    ball::LoggerManager::singleton().addRule(rule);

    BALL_LOG_ERROR << "Processing the second message.";
    processData(3938908, 2, 9001, message);
//..
// The final call to the 'processData' function below, passes a "uuid" of
// 2171395 (not 3938908) so the logging rule we defined will *not* apply and
// no message will be logged.
//..
    BALL_LOG_ERROR << "Processing the third message.";
    processData(2171395, 2, 9001, message);
//..
// The resulting logged output for this example looks like the following:
//..
// ERROR example.cpp:105 EXAMPLE.CATEGORY Processing the first message.
// ERROR example.cpp:117 EXAMPLE.CATEGORY Processing the second message.
// DEBUG example.cpp:35 EXAMPLE.CATEGORY An example message
// ERROR example.cpp:129 EXAMPLE.CATEGORY Processing the third message.
//..
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "USAGE EXAMPLE"
                               << bsl::endl << "============="
                               << bsl::endl;

        using namespace BloombergLP;  // okay here

        if (verbose) bsl::cout << "Initialize logger manager" << bsl::endl;

        ball::LoggerManagerConfiguration lmConfig;
        ball::LoggerManagerScopedGuard lmGuard(lmConfig);

        bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "default");

        ball::Administration::addCategory(
                                      "EQUITY.NASD",
                                      ball::Severity::e_TRACE,
                                      veryVerbose
                                      ? ball::Severity::e_INFO
                                      : ball::Severity::e_ERROR,
                                      ball::Severity::e_ERROR,
                                      ball::Severity::e_FATAL);

        ball::Administration::addCategory(
                                      "EQUITY.NASD.SUNW",
                                      ball::Severity::e_TRACE,
                                      veryVerbose
                                      ? ball::Severity::e_INFO
                                      : ball::Severity::e_ERROR,
                                      ball::Severity::e_ERROR,
                                      ball::Severity::e_FATAL);

if (verbose) bsl::cout << "stream-based macro usage" << bsl::endl;
{
///Example 3: C++ I/O Streams-Style Logging Macros
///- - - - - - - - - - - - - - - - - - - - - - - -
// The preferred logging method we use, the 'iostream'-style macros such as
// 'BALL_LOG_INFO', allow streaming via the 'bsl::ostream' 'class' and the C++
// stream operator '<<'.  An advantage the C++ streaming style has over the
// 'printf' style output (shown below in example 4) is that complex types often
// have the 'operator<<(ostream&, const TYPE&)' function overloaded so that
// they are able to be easily streamed to output.  We demonstrate this here
// using C++ streaming to stream a 'bdlt::Date' to output:
//..
    int         lotSize = 400;
    const char *ticker  = "SUNW";
    double      price   = 5.65;

    // Trading on a market that settles 3 days in the future.

    bdlt::Date settle = bdlt::CurrentTime::local().date() + 3;

    BALL_LOG_SET_CATEGORY("EQUITY.NASD")
//..
// We are logging with category "EQUITY.NASD", which is configured for a
// pass-through level of 'e_INFO', from here on.  We output a line using the
// 'BALL_LOG_INFO' macro:
//..
    BALL_LOG_INFO << "[1] " << lotSize
                  << " shares of " << ticker
                  << " sold at " << price
                  << " settlement date " << settle;
//..
// The above results in the following single-line message being output:
//..
//  <ts> <pid> <tid> INFO x.cpp 1161 EQUITY.NASD [1] 400 shares of SUNW sold
//  at 5.65 settlement date 17FEB2017
//..
// '<ts>' is the timestamp, '<pid>' is the process id, '<tid>' is the thread
// id, 'x.cpp' is the expansion of the '__FILE__' macro that is the name of the
// source file containing the call, 1161 is the line number of the call, and
// the trailing date following "settlement date" is the value of 'settle'.
//
// Next, we set the category to "EQUITY.NASD.SUNW", which has been defined with
// 'ball::Administration::addCategory' with its pass-through level set to
// 'e_INFO' and the trigger levels set at or above 'e_ERROR', so a level of
// 'e_WARN' also passes through:
//..
    {
        BALL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")

        // Now logging with category "EQUITY.NASD.SUNW".

        BALL_LOG_WARN << "[2] " << lotSize
                      << " shares of " << ticker
                      << " sold at " << price
                      << " settlement date " << settle;
    }
//..
// The above results in the following message to category "EQUITY.NASD.SUNW":
//..
//  <ts> <pid> <tid> WARN x.cpp 1185 EQUITY.NASD.SUNW [2] 400 shares of SUNW
//  sold at 5.65 settlement date 17FEB2017
//..
// Now, the category "EQUITY.NASD.SUNW" just went out of scope and category
// "EQUITY.NASD" is visible again, so it applies to the following:
//..
    BALL_LOG_INFO << "[3] " << lotSize
                  << " shares of " << ticker
                  << " sold at " << price
                  << " settlement date " << settle;
//..
// Finally, the above results in the following single-line message being
// output:
//..
//  <ts> <pid> <tid> INFO x.cpp 1198 EQUITY.NASD [3] 400 shares of SUNW sold
//  at 5.65 settlement date 17FEB2017
//..
// The settlement date was appended to the message as a simple illustration of
// the added flexibility provided by the C++ stream-based macros.  This last
// message was logged to category "EQUITY.NASD" at severity level
// 'ball::Severity::e_INFO'.
//
// The C++ stream-based macros, as opposed to the 'printf'-style macros, ensure
// at compile-time that no run-time format mismatches will occur.  Use of the
// stream-based logging style exclusively will likely lead to clearer, more
// maintainable code with fewer initial defects.
//
// Note that all uses of the log-generating macros, both 'printf'-style and C++
// stream-based, *must* occur within function scope (i.e., not at file scope).
}

if (verbose) bsl::cout << "printf-style macro usage" << bsl::endl;
{
///Example 4: 'printf'-Style Output
/// - - - - - - - - - - - - - - - -
// In the following example, we expand the 'logIt' function (defined above) to
// log two messages using the 'BALL_LOGVA_INFO' logging macro provided by this
// component.  This variadic macro takes a format string and a variable-length
// series of arguments, similar to 'printf'.
//..
    int         lotSize = 400;
    const char *ticker  = "SUNW";
    double      price   = 5.65;

    // Trading on a market that settles 3 days in the future.

    bdlt::Date settleDate = bdlt::CurrentTime::local().date() + 3;
//..
// Because we can't easily 'printf' complex types like 'bdlt::Date' or
// 'bsl::string', we have to convert 'settleDate' to a 'const char *'
// ourselves.  Note that all this additional work was unnecessary in Example 3
// when we used the C++ 'iostream'-style, rather than the 'printf'-style,
// macros.
//..
    bsl::ostringstream  settleOss;
    settleOss << settleDate;
    const bsl::string&  settleStr = settleOss.str();
    const char         *settle    = settleStr.c_str();
//..
// We set logging with category "EQUITY.NASD", which was configured for a
// pass-through severity level of 'e_INFO', and call 'BALL_LOGVA_INFO' to print
// our trade:
//..
    BALL_LOG_SET_CATEGORY("EQUITY.NASD")

    BALL_LOGVA_INFO("[4] %d shares of %s sold at %f settlement date %s\n",
                    lotSize, ticker, price, settle);
//..
// The above results in the following single-line message being output to
// category "EQUITY.NASD.SUNW" at severity level 'ball::Severity::e_INFO':
//..
//  <ts> <pid> <tid> INFO x.cpp 1256 EQUITY.NASD [4] 400 shares of SUNW sold
//  at 5.650000 settlement date 17FEB2017
//..
// In the above, '<ts>' is the timestamp, '<pid>' is the process id, '<tid>' is
// the thread id, 'x.cpp' is the expansion of the '__FILE__' macro that is the
// name of the source file containing the call, and 1256 is the line number of
// the call.
//
// Note that the first argument supplied to the 'BALL_LOGVA_INFO' macro is a
// 'printf'-style format specification.
//
// Next, we set the category to "EQUITY.NASD.SUNW", which is configured for a
// pass-through severity level of 'e_INFO':
//..
    {
        BALL_LOG_SET_CATEGORY("EQUITY.NASD.SUNW")

        // Now logging with category "EQUITY.NASD.SUNW".

        BALL_LOGVA_WARN("[5] %d shares of %s sold at %f settlement date %s\n",
                        lotSize, ticker, price, settle);
    }
//..
// The above results in the following single-line message to category
// "EQUITY.NASD.SUNW":
//..
//  <ts> <pid> <tid> WARN x.cpp 1281 EQUITY.NASD.SUNW [5] 400 shares of SUNW
//  sold at 5.650000 settlement date 17FEB2017
//..
// Now, the category "EQUITY.NASD.SUNW" just went out of scope and category
// "EQUITY.NASD" is visible again, so it applies to the following:
//..
    BALL_LOGVA_INFO("[6] %d shares of %s sold at %f settlement date %s\n",
                    lotSize, ticker, price, settle);
//..
// Finally, the above results in the following single-line message being
// output:
//..
//  <ts> <pid> <tid> INFO x.cpp 1294 EQUITY.NASD [6] 400 shares of SUNW sold
//  at 5.650000 settlement date 17FEB2017
//..
}

        // Note that the following Usage example was moved from the .h file to
        // the .cpp file so as not to encourage direct use of the utility
        // functions.

        if (verbose) bsl::cout << "Utility usage" << bsl::endl;
        {
            static const BloombergLP::ball::Category *category =
                       BloombergLP::ball::Log::setCategory("EQUITY.NASD.SUNW");
            {
                using BloombergLP::ball::Log;
                using BloombergLP::ball::Severity;
                if (category->isEnabled(ball::Severity::e_INFO)) {
                    const char *formatSpec =
                                         "[7] %d shares of %s sold at %f\n";
                    ball::Log::format(u::messageBuffer(),
                                      u::messageBufferSize(),
                                      formatSpec, 400, "SUNW", 5.65);
                    ball::Log::logMessage(category,
                                         BloombergLP::ball::Severity::e_INFO,
                                         __FILE__, __LINE__,
                                         u::messageBuffer());
                    if (veryVeryVerbose) P(u::messageBuffer());
                }
            }
        }

        if (verbose) bsl::cout << "callback macro usage (example 6)"
                               << bsl::endl;
        {
            const Point point;
            validatePoint(point);
        }

      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING RECURSIVE USE OF LOGGING MACROS
        //
        // Concerns:
        //: 1 The various logging macros can be safely invoked recursively.
        //:
        //: 2 Log messages are correctly being output during recursive macros
        //:   invocation.
        //
        // Plan:
        //: 1 Define three functions, 'recurseStreamBasedMacros',
        //:   'recursePrintfStyleMacros' and 'recurseCallbackMacros', that log
        //:   using the stream-based macros, 'printf'-style macros and calback
        //:   macros, respectively.  Call these three functions in the context
        //:   of invocations of the various logging macros and verify the
        //:   results.  (C-1, 2)
        //
        // Testing:
        //   CONCERN: The logging macros can be used recursively
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "TESTING RECURSIVE USE OF LOGGING MACROS\n"
                               << "=======================================\n";

        using namespace BALL_LOG_TEST_CASE_35;

        typedef BloombergLP::ball::FileObserver2  FileObserver;
        typedef BloombergLP::bdls::FilesystemUtil FilesystemUtil;

        TestAllocator ta(veryVeryVeryVerbose);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        LoggerManager& manager = LoggerManager::singleton();

        u::TempDirectoryGuard tempDirGuard;

        bsl::string baseName(tempDirGuard.getTempDirName());
        BloombergLP::bdls::PathUtil::appendRaw(&baseName, "testLog");

        if (veryVeryVerbose) { T_; T_; P(baseName); }

        bsl::shared_ptr<FileObserver> observer(
                                              new (ta) FileObserver(&ta), &ta);

        observer->enableFileLogging(baseName.c_str(), false);
        observer->setLogFileFunctor(
                             BloombergLP::ball::RecordStringFormatter("%m\n"));

        ASSERT(0 == manager.registerObserver(observer, "test"));
        ASSERT(0 != manager.setCategory("Recursion",
                                        Sev::e_OFF,
                                        Sev::e_TRACE,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        if (verbose)
            bsl::cout << "\tTesting correctness of the common buffer usage.\n";
        {
            // Each macro call uses common buffer to store message.  We need to
            // test that valuable data in this buffer aren't overwritten during
            // recursive logging.  But thorough test produces a large number of
            // log messages.  To avoid cluttering up the code with a long
            // sheets of expected messages to compare with, we will test this
            // concern separately with a few macro invocations.

            const char EXPECTED_LOG[] = "Inner FATAL[0] 32\n"
                                        "Inner FATAL[1] 32\n"
                                        "Inner FATAL[2] 32\n"
                                        "Outer FATAL[2] 32\n"
                                        "Inner ERROR[0] 64\n"
                                        "Inner ERROR[1] 64\n"
                                        "Inner ERROR[2] 64\n"
                                        "Outer ERROR[2] 64\n"
                                        "Inner WARN[0] 96\n"
                                        "Inner WARN[1] 96\n"
                                        "Inner WARN[2] 96\n"
                                        "Outer WARN[2] 96\n"
                                        "Inner INFO[0] 128\n"
                                        "Inner INFO[1] 128\n"
                                        "Inner INFO[2] 128\n"
                                        "Outer INFO[2] 128\n";

            const int EXPECTED_LENGTH =
                                   static_cast<int>(bsl::strlen(EXPECTED_LOG));

            BALL_LOG_SET_CATEGORY("Recursion");

            BALL_LOG_FATAL << "Outer FATAL[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_FATAL);

            BALL_LOG_ERROR << "Outer ERROR[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_ERROR);

            BALL_LOGVA_WARN("%s %d", "Outer WARN[2]",
                            recurseStreamBasedMacros<2>(Sev::e_WARN));

            BALL_LOGVA_INFO("%s %d", "Outer INFO[2]",
                            recursePrintfStyleMacros<2>(Sev::e_INFO));

            FilesystemUtil::FileDescriptor fd = FilesystemUtil::open(
                                                  baseName,
                                                  FilesystemUtil::e_OPEN,
                                                  FilesystemUtil::e_READ_ONLY);

            const size_t READ_BUFFER_SIZE = 8192;
            char         buffer[READ_BUFFER_SIZE];
            memset(buffer, 0, READ_BUFFER_SIZE);

            int numBytes = static_cast<int>(FilesystemUtil::read(
                                                fd, buffer, READ_BUFFER_SIZE));

            ASSERTV(EXPECTED_LENGTH, numBytes, EXPECTED_LENGTH == numBytes);
            ASSERTV(EXPECTED_LOG, buffer,
                    0 == bsl::strcmp(EXPECTED_LOG, buffer));

            FilesystemUtil::close(fd);
        }

        if (verbose) bsl::cout << "\tStream-based recurses to stream-based.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            BALL_LOG_FATAL << "Outer FATAL[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_FATAL);
            BALL_LOG_FATAL << "Outer FATAL[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_FATAL);
            BALL_LOG_FATAL << "Outer FATAL[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_FATAL);

            BALL_LOG_ERROR << "Outer ERROR[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_ERROR);
            BALL_LOG_ERROR << "Outer ERROR[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_ERROR);
            BALL_LOG_ERROR << "Outer ERROR[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_ERROR);

            BALL_LOG_WARN  << "Outer WARN[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_WARN);
            BALL_LOG_WARN  << "Outer WARN[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_WARN);
            BALL_LOG_WARN  << "Outer WARN[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_WARN);

            BALL_LOG_INFO  << "Outer INFO[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_INFO);
            BALL_LOG_INFO  << "Outer INFO[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_INFO);
            BALL_LOG_INFO  << "Outer INFO[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_INFO);

            BALL_LOG_DEBUG << "Outer DEBUG[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_DEBUG);
            BALL_LOG_DEBUG << "Outer DEBUG[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_DEBUG);
            BALL_LOG_DEBUG << "Outer DEBUG[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_DEBUG);

            BALL_LOG_TRACE << "Outer TRACE[0] "
                           << recurseStreamBasedMacros<0>(Sev::e_TRACE);
            BALL_LOG_TRACE << "Outer TRACE[1] "
                           << recurseStreamBasedMacros<1>(Sev::e_TRACE);
            BALL_LOG_TRACE << "Outer TRACE[2] "
                           << recurseStreamBasedMacros<2>(Sev::e_TRACE);
        }

        if (verbose) bsl::cout << "\tStream-based recurses to printf-style.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            BALL_LOG_FATAL << "Outer FATAL[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_FATAL);
            BALL_LOG_FATAL << "Outer FATAL[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_FATAL);
            BALL_LOG_FATAL << "Outer FATAL[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_FATAL);

            BALL_LOG_ERROR << "Outer ERROR[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_ERROR);
            BALL_LOG_ERROR << "Outer ERROR[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_ERROR);
            BALL_LOG_ERROR << "Outer ERROR[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_ERROR);

            BALL_LOG_WARN  << "Outer WARN[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_WARN);
            BALL_LOG_WARN  << "Outer WARN[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_WARN);
            BALL_LOG_WARN  << "Outer WARN[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_WARN);

            BALL_LOG_INFO  << "Outer INFO[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_INFO);
            BALL_LOG_INFO  << "Outer INFO[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_INFO);
            BALL_LOG_INFO  << "Outer INFO[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_INFO);

            BALL_LOG_DEBUG << "Outer DEBUG[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_DEBUG);
            BALL_LOG_DEBUG << "Outer DEBUG[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_DEBUG);
            BALL_LOG_DEBUG << "Outer DEBUG[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_DEBUG);

            BALL_LOG_TRACE << "Outer TRACE[0] "
                           << recursePrintfStyleMacros<0>(Sev::e_TRACE);
            BALL_LOG_TRACE << "Outer TRACE[1] "
                           << recursePrintfStyleMacros<1>(Sev::e_TRACE);
            BALL_LOG_TRACE << "Outer TRACE[2] "
                           << recursePrintfStyleMacros<2>(Sev::e_TRACE);
        }

        if (verbose) bsl::cout << "\tPrintf-style recurses to stream-based.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[0]",
                             recurseStreamBasedMacros<0>(Sev::e_FATAL));
            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[1]",
                             recurseStreamBasedMacros<1>(Sev::e_FATAL));
            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[2]",
                             recurseStreamBasedMacros<2>(Sev::e_FATAL));

            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[0]",
                             recurseStreamBasedMacros<0>(Sev::e_ERROR));
            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[1]",
                             recurseStreamBasedMacros<1>(Sev::e_ERROR));
            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[2]",
                             recurseStreamBasedMacros<2>(Sev::e_ERROR));

            BALL_LOGVA_WARN( "%s %d", "Outer WARN[0]",
                             recurseStreamBasedMacros<0>(Sev::e_WARN));
            BALL_LOGVA_WARN( "%s %d", "Outer WARN[1]",
                             recurseStreamBasedMacros<1>(Sev::e_WARN));
            BALL_LOGVA_WARN( "%s %d", "Outer WARN[2]",
                             recurseStreamBasedMacros<2>(Sev::e_WARN));

            BALL_LOGVA_INFO( "%s %d", "Outer INFO[0]",
                             recurseStreamBasedMacros<0>(Sev::e_INFO));
            BALL_LOGVA_INFO( "%s %d", "Outer INFO[1]",
                             recurseStreamBasedMacros<1>(Sev::e_INFO));
            BALL_LOGVA_INFO( "%s %d", "Outer INFO[2]",
                             recurseStreamBasedMacros<2>(Sev::e_INFO));

            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[0]",
                             recurseStreamBasedMacros<0>(Sev::e_DEBUG));
            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[1]",
                             recurseStreamBasedMacros<1>(Sev::e_DEBUG));
            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[2]",
                             recurseStreamBasedMacros<2>(Sev::e_DEBUG));

            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[0]",
                             recurseStreamBasedMacros<0>(Sev::e_TRACE));
            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[1]",
                             recurseStreamBasedMacros<1>(Sev::e_TRACE));
            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[2]",
                             recurseStreamBasedMacros<2>(Sev::e_TRACE));
        }

        // Deadlock check (DRQS 110401470)

        if (verbose) bsl::cout << "\tPrintf-style recurses to printf-style.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[0]",
                             recursePrintfStyleMacros<0>(Sev::e_FATAL));
            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[1]",
                             recursePrintfStyleMacros<1>(Sev::e_FATAL));
            BALL_LOGVA_FATAL("%s %d", "Outer FATAL[2]",
                             recursePrintfStyleMacros<2>(Sev::e_FATAL));

            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[0]",
                             recursePrintfStyleMacros<0>(Sev::e_ERROR));
            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[1]",
                             recursePrintfStyleMacros<1>(Sev::e_ERROR));
            BALL_LOGVA_ERROR("%s %d", "Outer ERROR[2]",
                             recursePrintfStyleMacros<2>(Sev::e_ERROR));

            BALL_LOGVA_WARN( "%s %d", "Outer WARN[0]",
                             recursePrintfStyleMacros<0>(Sev::e_WARN));
            BALL_LOGVA_WARN( "%s %d", "Outer WARN[1]",
                             recursePrintfStyleMacros<1>(Sev::e_WARN));
            BALL_LOGVA_WARN( "%s %d", "Outer WARN[2]",
                             recursePrintfStyleMacros<2>(Sev::e_WARN));

            BALL_LOGVA_INFO( "%s %d", "Outer INFO[0]",
                             recursePrintfStyleMacros<0>(Sev::e_INFO));
            BALL_LOGVA_INFO( "%s %d", "Outer INFO[1]",
                             recursePrintfStyleMacros<1>(Sev::e_INFO));
            BALL_LOGVA_INFO( "%s %d", "Outer INFO[2]",
                             recursePrintfStyleMacros<2>(Sev::e_INFO));

            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[0]",
                             recursePrintfStyleMacros<0>(Sev::e_DEBUG));
            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[1]",
                             recursePrintfStyleMacros<1>(Sev::e_DEBUG));
            BALL_LOGVA_DEBUG("%s %d", "Outer DEBUG[2]",
                             recursePrintfStyleMacros<2>(Sev::e_DEBUG));

            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[0]",
                             recursePrintfStyleMacros<0>(Sev::e_TRACE));
            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[1]",
                             recursePrintfStyleMacros<1>(Sev::e_TRACE));
            BALL_LOGVA_TRACE("%s %d", "Outer TRACE[2]",
                             recursePrintfStyleMacros<2>(Sev::e_TRACE));
      }

        if (verbose) bsl::cout << "\tCallback recurses to  printf-style.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            bsl::function <void(BloombergLP::ball::UserFields *)> callback;

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseStreamBasedMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_FATAL);
            BALL_LOGCB_FATAL(callback) << "Outer FATAL";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseStreamBasedMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_ERROR);
            BALL_LOGCB_ERROR(callback) << "Outer ERROR";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                          &recurseStreamBasedMacrosCallback,
                                          BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_WARN);
            BALL_LOGCB_WARN(callback) << "Outer WARN";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseStreamBasedMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_INFO);
            BALL_LOGCB_INFO(callback) << "Outer INFO";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseStreamBasedMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_DEBUG);
            BALL_LOGCB_DEBUG(callback) << "Outer DEBUG";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseStreamBasedMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_TRACE);
            BALL_LOGCB_TRACE(callback) << "Outer TRACE";
        }

        if (verbose) bsl::cout << "\tCallback recurses to stream-based.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            bsl::function <void(BloombergLP::ball::UserFields *)> callback;

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_FATAL);
            BALL_LOGCB_FATAL(callback) << "Outer FATAL";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_ERROR);
            BALL_LOGCB_ERROR(callback) << "Outer ERROR";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_WARN);
            BALL_LOGCB_WARN(callback) << "Outer WARN";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_INFO);
            BALL_LOGCB_INFO(callback) << "Outer INFO";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_DEBUG);
            BALL_LOGCB_DEBUG(callback) << "Outer DEBUG";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recursePrintfStyleMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_TRACE);
            BALL_LOGCB_TRACE(callback) << "Outer TRACE";
        }


        if (verbose) bsl::cout << "\tCallback recurses to callback.\n";
        {
            BALL_LOG_SET_CATEGORY("Recursion");

            bsl::function <void(BloombergLP::ball::UserFields *)> callback;

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_FATAL);
            BALL_LOGCB_FATAL(callback) << "Outer FATAL";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_ERROR);
            BALL_LOGCB_ERROR(callback) << "Outer ERROR";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_WARN);
            BALL_LOGCB_WARN(callback) << "Outer WARN";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_INFO);
            BALL_LOGCB_INFO(callback) << "Outer INFO";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_DEBUG);
            BALL_LOGCB_DEBUG(callback) << "Outer DEBUG";

            callback = BloombergLP::bdlf::BindUtil::bind(
                                           &recurseCallbackMacrosCallback,
                                           BloombergLP::bdlf::PlaceHolders::_1,
                                           Sev::e_TRACE);
            BALL_LOGCB_TRACE(callback) << "Outer TRACE";
        }
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING CLASS HIERARCHICAL CATEGORY MACROS
        //
        // Concerns:
        //: 1 That the class hierarchical macro, if called with a non-existing
        //:   category name and the logger manager has room, will result in a
        //:   new category being created.
        //:
        //: 2 The new category has the right name.
        //:
        //: 3 If the new category inherits from another category, it has the
        //:   same logging thresholds as the other category.
        //:
        //: 4 In 'BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY' the category
        //:   holder is properly linked to the previously created category
        //:   holder belonging to the same category.
        //
        // Plan:
        //: 1 We iterate through two nested loops, driving 3 distinct threshold
        //:   aggregate values 'ILEVELS', 'JLEVELS', and 'CLEVELS'.  We also
        //:   have the distinct 'WLEVELS', and 'MLEVELS', which is one of
        //:   'ILEVELS' or 'CLEVELS' and which is the default levels to which
        //:   the logger manager will configure new categories.
        //:
        //: 2 In the inner loop, initialize a logger manager singleton.
        //:
        //: 3 Use the 'CALL' macro (defined below) to call
        //:   'TC::TestClassMacro<N>::operator()' for many different values of
        //:   'N', which will call 'BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY'
        //:   under different circumstances and with different category names.
        //:   That 'operator()' function will perform many tests to confirm
        //:   that the macro functioned properly.  See the documentation of
        //:   'TC::TestClassMacro' and 'TC::indices' for more documentation.
        //:
        //: 4 The function 'TC::TestClassMacro<N>::operator()' returns a
        //:   reference to a static 'struct' of type 'ResultRec'.  It contains
        //:   a 'bool', 'd_passed' which is 'true' if all of the 'ASSERT's in
        //:   the function passed and 'false' otherwise.  It also contains a
        //:   pointer to any static category holder initialized, and a pointer
        //:   to the category created or found.  The 'ResultRec' is a static
        //:   within the templatized 'TC::result<N>()' function, so it can be
        //:   fetched later by the 'RET(N)' macro below, which is useful for
        //:   finding past category holders for comparing with new category
        //:   holders to make sure that they are linked properly.
        //
        // Testing:
        //   BALL_LOG_SET_CLASS_CATEGORY_HIERARCHICALLY(const char *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING CLASS HIERARCHICAL CATEGORY MACROS\n"
                             "==========================================\n";

        namespace TC = BALL_LOG_HIERARCHICAL_CATEGORIES;
        namespace Blp = BloombergLP;

        typedef TC::Cat                    Cat;
        typedef Blp::ball::LoggerManager   Manager;

        Blp::bslma::TestAllocator da("default", veryVeryVerbose);
        Blp::bslma::TestAllocator ga("global",  veryVeryVerbose);
        Blp::bslma::TestAllocator ta("test",    veryVeryVerbose);

        Blp::bslma::DefaultAllocatorGuard guard(&da);
        Blp::bslma::Default::setGlobalAllocator(&ga);

        // Have an 'Agg' set to 'WLEVELS' -- Wrong Levels -- that we never set
        // anything to, and that never equals any of the other levels.

        const TC::Agg WLEVELS(TC::S::e_OFF, TC::S::e_OFF,
                                                   TC::S::e_OFF, TC::S::e_OFF);

        TC::Agg& CLEVELS = TC::callbackLevels;

        for (int ti = 0; ti < TC::k_NUM_DATA; ++ti) {
            const TC::Data& IDATA         = TC::DATA[ti];
            const int       IL            = IDATA.d_line;
            const TC::Agg&  ILEVELS       = IDATA.d_levels;
            const bool      ISET_CALLBACK = IDATA.d_setCallbackThresholds;

            if (ISET_CALLBACK) {
                CLEVELS = ILEVELS;
                continue;
            }

            for (int tj = 0; tj < TC::k_NUM_DATA; ++tj) {
                const TC::Data& JDATA         = TC::DATA[tj];
                const int       JL            = JDATA.d_line;
                const TC::Agg&  JLEVELS       = JDATA.d_levels;
                const bool      JSET_CALLBACK = JDATA.d_setCallbackThresholds;
                bool            JUSE_CALLBACK = JDATA.d_useCallback;

                if (JSET_CALLBACK || IL == JL) {
                    continue;
                }

                TC::defaultCat = 0;
                TC::ResultRec *ret_p;
                TC::instantiationSet.clear();

                // 'CALL' will create an object of type 'TC::TestClassMacro'
                // instantiated on the specified integer 'N', and call
                // 'operator()()' that object, passing the specified threshold
                // aggregate 'newLevels' describing the logging thresholds the
                // category created or found is expected to have, the specified
                // 'prevHolder' that is either 0 or points to a previous
                // category holder loaded with the same category, and the
                // specified 'bool' 'repeat', which indicates whether 'CALL'
                // has been called with this value of 'N' before.
                //
                // The value of the current source file line number, and the
                // 'IL' and 'JL' values, and the variable name of 'N' in string
                // form, are used to initialize a 'TC::PositionRec' object that
                // is passed to the 'operator()()' function called and, in that
                // function, passed as the first argument to all the 'ASSERTV's
                // in the function, to make any error messages more meaningful.

#undef  CALL
#define CALL(N, newLevels, prevHolder, repeat)                                \
                ret_p = &TC::TestClassMacro<TC::indices::N>()(                \
                                       newLevels,                             \
                                       TC::PositionRec(__LINE__, IL, JL, #N), \
                                       prevHolder,                            \
                                       repeat)

                // 'RET(<integer>)' will return a ref to the 'TC::ResultRec'
                // returned by 'TC::TestClassMacro<N>::operator()()' with the
                // value 'N'.  This allows us to examine the results of ANY
                // previous call to 'CALL'.

#undef  RET
#define RET(N)  TC::result<TC::indices::N>()

                ASSERTV(IL, JL, CLEVELS != ILEVELS);
                ASSERTV(IL, JL, CLEVELS != JLEVELS);
                ASSERTV(IL, JL, CLEVELS != WLEVELS);
                ASSERTV(IL, JL, ILEVELS != JLEVELS);
                ASSERTV(IL, JL, ILEVELS != WLEVELS);
                ASSERTV(IL, JL, JLEVELS != WLEVELS);

                // Attempt without manager singleton, nothing happens.

                CALL(bowWow, WLEVELS, 0, false);
                ASSERT(!ret_p->d_category_p);

                Blp::ball::LoggerManagerConfiguration mLMC(&ta);
                ASSERT(0 == mLMC.setDefaultThresholdLevelsIfValid(
                                                   ILEVELS.recordLevel(),
                                                   ILEVELS.passLevel(),
                                                   ILEVELS.triggerLevel(),
                                                   ILEVELS.triggerAllLevel()));
                Blp::ball::LoggerManagerScopedGuard lmGuard(mLMC);
                Manager& manager = Blp::ball::LoggerManager::singleton();
                if (JUSE_CALLBACK) {
                    manager.setDefaultThresholdLevelsCallback(
                                                             &TC::dtlCallback);
                }

                const TC::Agg MLEVELS = JUSE_CALLBACK
                                        ? CLEVELS
                                        : ILEVELS;

                TC::defaultCat = &manager.defaultCategory();

                Cat *classBaseCat =manager.setCategory(
                                                    "class",
                                                    JLEVELS.recordLevel(),
                                                    JLEVELS.passLevel(),
                                                    JLEVELS.triggerLevel(),
                                                    JLEVELS.triggerAllLevel());
                ASSERTV(JL, JLEVELS, TC::getLevels(classBaseCat),
                                       JLEVELS == TC::getLevels(classBaseCat));

                CALL(woof_a, MLEVELS, 0, false);

                CALL(meow, MLEVELS, 0, false);

                Cat *meowCat = manager.lookupCategory("meow");
                ASSERT(meowCat);
                TC::setLevels(meowCat, JLEVELS);
                ASSERTV(IL, JL, JLEVELS == TC::getLevels(meowCat));

                CALL(meowBubble, JLEVELS, 0, false);

                TC::setLevels(meowCat, ILEVELS);
                ASSERTV(IL, JL, ILEVELS == TC::getLevels(meowCat));

                CALL(meowBark, ILEVELS, 0, false);

                TC::setLevels(meowCat, CLEVELS);

                CALL(meowArf, CLEVELS, 0, false);

                CALL(woof_b, MLEVELS, RET(woof_a).d_holder_p, false);

                CALL(woof_c, MLEVELS, ret_p->d_holder_p, false);

                CALL(_a, ILEVELS, 0, false);
                ASSERTV(TC::defaultCat == ret_p->d_category_p);

                CALL(meowBubble, JLEVELS, 0, true);

                CALL(meowPurr, CLEVELS, 0, false);

                CALL(classDerivedDerived, JLEVELS, 0, false);

                CALL(classDerived_a, JLEVELS, 0, false);

                CALL(classDerived_b, JLEVELS, ret_p->d_holder_p, false);

                TC::setLevels(const_cast<Cat *>(ret_p->d_category_p),
                              CLEVELS);

                CALL(meowBarkArf, ILEVELS, 0, false);

                CALL(roar, MLEVELS, 0, false);

                CALL(meowBark, ILEVELS, 0, true);

                CALL(classDerivedRoar, CLEVELS, 0, false);

                CALL(growl, MLEVELS, 0, false);

                CALL(classDerivedRoar, CLEVELS, 0, true);

                CALL(meowPurr, CLEVELS, 0, true);

                CALL(woof_d, MLEVELS, RET(woof_c).d_holder_p, false);

                CALL(secondClass, MLEVELS, 0, false);

                // Attempt to create one category more than the capacity of the
                // category manager.  This will return the default category,
                // which was created when the logger manager was created, and
                // it should have the thresholds the logger manager
                // configuration object had (ILEVELS).

                manager.setMaxNumCategories(manager.numCategories());

                CALL(whimper, ILEVELS, RET(_a).d_holder_p, false);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                // Test attempt to create a new category that inherits when the
                // logger manager is already full.

                CALL(woofMoan, ILEVELS, ret_p->d_holder_p, false);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                CALL(woof_e, MLEVELS, RET(woof_d).d_holder_p, false);
                ASSERTV(RET(woof_d).d_category_p == ret_p->d_category_p);

                CALL(full, ILEVELS, RET(woofMoan).d_holder_p, false);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                // Make sure that all the categories returned are distinct,
                // except for the ones we EXPECT to match.

                bsl::vector<const Cat *> v(&ta);
                v.push_back(TC::defaultCat);
                v.push_back(classBaseCat);
                v.push_back(RET(bowWow).d_category_p);
                v.push_back(RET(woof_a).d_category_p);
                v.push_back(RET(meow).d_category_p);
                v.push_back(RET(meowBubble).d_category_p);
                v.push_back(RET(meowBark).d_category_p);
                v.push_back(RET(meowArf).d_category_p);
                v.push_back(RET(meowPurr).d_category_p);
                v.push_back(RET(classDerived_a).d_category_p);
                v.push_back(RET(classDerivedDerived).d_category_p);
                v.push_back(RET(meowBarkArf).d_category_p);
                v.push_back(RET(roar).d_category_p);
                v.push_back(RET(classDerivedRoar).d_category_p);
                v.push_back(RET(growl).d_category_p);

                ASSERTV(static_cast<Blp::bsls::Types::IntPtr>(v.size()),
                                                      manager.numCategories(),
                        static_cast<Blp::bsls::Types::IntPtr>(v.size()) ==
                                                      manager.numCategories());

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = ii + 1; jj < v.size(); ++jj) {
                        ASSERTV(ii, jj, v[ii]->categoryName(), v[ii] != v[jj]);
                    }
                }

                v.clear();
                v.push_back(TC::defaultCat);
                v.push_back(RET(whimper).d_category_p);
                v.push_back(RET(woofMoan).d_category_p);
                v.push_back(RET(full).d_category_p);

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = 0; jj < v.size(); ++jj) {
                        ASSERT(v[ii] == v[jj]);
                    }
                }

                // "woof" categories all match.

                v.clear();
                v.push_back(RET(woof_a).d_category_p);
                v.push_back(RET(woof_b).d_category_p);
                v.push_back(RET(woof_c).d_category_p);
                v.push_back(RET(woof_d).d_category_p);
                v.push_back(RET(woof_e).d_category_p);

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = 0; jj < v.size(); ++jj) {
                        ASSERT(v[ii] == v[jj]);
                    }
                }
#undef  CALL
#undef  RET
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING LOCAL HIERARCHICAL CATEGORY MACROS
        //
        // Concerns:
        //: 1 That the 2 non-class hierarchical macros, if called with a
        //:   non-existing category name and the logger manager has room, will
        //:   result in a new category being created.
        //:
        //: 2 The new category has the right name.
        //:
        //: 3 If the new category inherits from another category, it has the
        //:   same logging thresholds as the category it inherited from.
        //:
        //: 4 In 'BALL_LOG_SET_CATEGORY_HIERARCHICALLY' the category holder is
        //:   properly linked to the previously created category holder
        //:   belonging to the same category.
        //:
        //: 5 In 'BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY', the category
        //:   holder is not linked to any other category holder.
        //
        // Plan:
        //: 1 We iterate through two nested loops, driving 3 distinct threshold
        //:   aggregate values 'ILEVELS', 'JLEVELS', and 'CLEVELS'.  We also
        //:   have the distinct 'WLEVELS', and 'MLEVELS', which is one of
        //:   'ILEVELS' or 'CLEVELS' and which is the default levels to which
        //:   the logger manager will configure new categories.
        //:
        //: 2 In the inner loop, initialize a logger manager singleton.
        //:
        //: 3 Use the 'CALL' macro (defined below) to call
        //:   'TC::TestLocalMacros<N>::operator()' for many different values of
        //:   'N', which will call either
        //:   'BALL_LOG_SET_CATEGORY_HIERARCHICALLY', which will create or find
        //:   a category and initialize a static category holder, or
        //:   'BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY' which will create
        //:   or find a category and initialize a dynamic category holder, and
        //:   conduct many tests to verify that both the category and the
        //:   category holder are as they should be.
        //:
        //: 3 The function 'TC::testLocalMacros<N>' returns reference to a
        //:   static 'struct' of type 'ResultRec'.  It contains a 'bool',
        //:   'd_passed' which is 'true' if all of the 'ASSERT's in the
        //:   function passed and 'false' otherwise.  It also contains a
        //:   pointer to any static category holder initialized, and a pointer
        //:   to the category created or found.  The 'ResultRec' is a static
        //:   within the templatized 'TC::result<N>()' function, so it can be
        //:   fetched later by the 'RET(N)' macro below, which is useful for
        //:   finding past category holders for comparing with new category
        //:   holders to make sure that they are linked properly.
        //
        // Testing:
        //   BALL_LOG_SET_CATEGORY_HIERARCHICALLY(const char *);
        //   BALL_LOG_SET_DYNAMIC_CATEGORY_HIERARCHICALLY(const char *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING LOCAL HIERARCHICAL CATEGORY MACROS\n"
                             "==========================================\n";

        namespace TC = BALL_LOG_HIERARCHICAL_CATEGORIES;
        namespace Blp = BloombergLP;

        typedef TC::Cat                     Cat;
        typedef Blp::ball::LoggerManager    Manager;

        Blp::bslma::TestAllocator da("default", veryVeryVerbose);
        Blp::bslma::TestAllocator ga("global",  veryVeryVerbose);
        Blp::bslma::TestAllocator ta("test",    veryVeryVerbose);

        Blp::bslma::DefaultAllocatorGuard guard(&da);
        Blp::bslma::Default::setGlobalAllocator(&ga);

        // Have an 'Agg' set to 'WLEVELS' -- Wrong Levels -- that we never set
        // anything to, and that never equals any of the other levels.

        const TC::Agg WLEVELS(TC::S::e_OFF, TC::S::e_OFF,
                                                   TC::S::e_OFF, TC::S::e_OFF);

        TC::Agg& CLEVELS = TC::callbackLevels;

        for (int ti = 0; ti < TC::k_NUM_DATA; ++ti) {
            const TC::Data& IDATA         = TC::DATA[ti];
            const int       IL            = IDATA.d_line;
            const TC::Agg&  ILEVELS       = IDATA.d_levels;
            const bool      ISET_CALLBACK = IDATA.d_setCallbackThresholds;

            if (ISET_CALLBACK) {
                CLEVELS = ILEVELS;
                continue;
            }

            for (int tj = 0; tj < TC::k_NUM_DATA; ++tj) {
                const TC::Data& JDATA         = TC::DATA[tj];
                const int       JL            = JDATA.d_line;
                const TC::Agg&  JLEVELS       = JDATA.d_levels;
                const bool      JSET_CALLBACK = JDATA.d_setCallbackThresholds;
                bool            JUSE_CALLBACK = JDATA.d_useCallback;

                if (JSET_CALLBACK || IL == JL) {
                    continue;
                }

                TC::defaultCat = 0;
                TC::ResultRec *ret_p;
                TC::instantiationSet.clear();

                // 'CALL' will create an object of type 'TC::TestLocalMacros'
                // instantiated on the specified integer 'N', and call
                // 'operator()()' that object, passing the specified threshold
                // aggregate 'newLevels' describing the logging thresholds the
                // category created or found is expected to have, the specified
                // 'prevHolder' that is either 0 or points to a previous
                // category holder loaded with the same category, and the
                // specified 'flags', a combination of two flags, one of which
                // indicates whether 'CALL' has already been called this
                // iteration with the specified 'N', and one of which specifies
                // whether the category holder to be initialized is to be
                // static or dynamic.
                //
                // The value of the current source file line number, and the
                // 'IL' and 'JL' values, and the variable name of 'N' in string
                // form, are used to initialize a 'TC::PositionRec' object that
                // is passed to the 'operator()()' function called and, in that
                // function, passed as the first argument to all the 'ASSERTV's
                // in the function, to make any error messages more meaningful.

#undef  CALL
#define CALL(N, newLevels, prevHolder, flags)                                 \
                ret_p = &TC::testLocalMacros<TC::indices::N>(                 \
                                       newLevels,                             \
                                       TC::PositionRec(__LINE__, IL, JL, #N), \
                                       prevHolder,                            \
                                       TC::flags)

                // 'RET(<integer>)' will return a ref to the 'TC::ResultRec'
                // returned by 'TC::TestClassMacro<N>::operator()()' with the
                // value 'N'.  This allows us to examine the results of ANY
                // previous call to 'CALL'.

#undef  RET
#define RET(N)  TC::result<TC::indices::N>()

                ASSERTV(IL, JL, CLEVELS != ILEVELS);
                ASSERTV(IL, JL, CLEVELS != JLEVELS);
                ASSERTV(IL, JL, CLEVELS != WLEVELS);
                ASSERTV(IL, JL, ILEVELS != JLEVELS);
                ASSERTV(IL, JL, ILEVELS != WLEVELS);
                ASSERTV(IL, JL, JLEVELS != WLEVELS);

                // Attempt without manager singleton, nothing happens.

                CALL(bowWow, WLEVELS, 0, e_STATIC);
                ASSERT(!ret_p->d_category_p);

                Blp::ball::LoggerManagerConfiguration mLMC(&ta);
                ASSERT(0 == mLMC.setDefaultThresholdLevelsIfValid(
                                                   ILEVELS.recordLevel(),
                                                   ILEVELS.passLevel(),
                                                   ILEVELS.triggerLevel(),
                                                   ILEVELS.triggerAllLevel()));
                Blp::ball::LoggerManagerScopedGuard lmGuard(mLMC);
                Manager& manager = Blp::ball::LoggerManager::singleton();
                if (JUSE_CALLBACK) {
                    manager.setDefaultThresholdLevelsCallback(
                                                             &TC::dtlCallback);
                }

                const TC::Agg MLEVELS = JUSE_CALLBACK
                                      ? CLEVELS
                                      : ILEVELS;

                TC::defaultCat = &manager.defaultCategory();

                Cat *classBaseCat =manager.setCategory(
                                                    "class",
                                                    JLEVELS.recordLevel(),
                                                    JLEVELS.passLevel(),
                                                    JLEVELS.triggerLevel(),
                                                    JLEVELS.triggerAllLevel());
                ASSERTV(JL, JLEVELS, TC::getLevels(classBaseCat),
                                       JLEVELS == TC::getLevels(classBaseCat));

                CALL(woof_a, MLEVELS, 0, e_STATIC);

                CALL(meow, MLEVELS, 0, e_STATIC);

                Cat *meowCat = manager.lookupCategory("meow");
                ASSERT(meowCat);
                TC::setLevels(meowCat, JLEVELS);
                ASSERTV(IL, JL, JLEVELS == TC::getLevels(meowCat));

                CALL(meowBubble, JLEVELS, 0, e_DYNAMIC);

                TC::setLevels(meowCat, ILEVELS);
                ASSERTV(IL, JL, ILEVELS == TC::getLevels(meowCat));

                CALL(meowBark, ILEVELS, 0, e_STATIC);

                TC::setLevels(meowCat, CLEVELS);

                CALL(meowArf, CLEVELS, 0, e_STATIC);

                CALL(woof_b, MLEVELS, RET(woof_a).d_holder_p, e_STATIC);
                ASSERTV(RET(woof_a).d_category_p == ret_p->d_category_p);

                CALL(woof_c, MLEVELS, 0, e_DYNAMIC);
                ASSERTV(RET(woof_a).d_category_p == ret_p->d_category_p);

                CALL(_a, ILEVELS, 0, e_STATIC);
                ASSERTV(TC::defaultCat == ret_p->d_category_p);

                CALL(meowBubble, JLEVELS, 0, e_REPEAT_DYNAMIC);

                CALL(meowPurr, CLEVELS, 0, e_STATIC);

                CALL(classDerivedDerived, JLEVELS, 0, e_DYNAMIC);

                CALL(classDerived_a, JLEVELS, 0, e_STATIC);

                CALL(classDerived_b, JLEVELS, ret_p->d_holder_p, e_STATIC);

                TC::setLevels(const_cast<Cat *>(ret_p->d_category_p),
                              CLEVELS);

                CALL(meowBarkArf, ILEVELS, 0, e_STATIC);

                CALL(roar, MLEVELS, 0, e_DYNAMIC);

                CALL(meowBark, ILEVELS, 0, e_REPEAT_STATIC);

                CALL(classDerivedRoar, CLEVELS, 0, e_DYNAMIC);

                CALL(growl, MLEVELS, 0, e_STATIC);

                CALL(classDerivedRoar, CLEVELS, 0, e_REPEAT_DYNAMIC);

                CALL(meowPurr, CLEVELS, 0, e_REPEAT_STATIC);

                CALL(woof_d, MLEVELS, RET(woof_b).d_holder_p, e_STATIC);
                ASSERTV(RET(woof_a).d_category_p == ret_p->d_category_p);

                CALL(secondClass, MLEVELS, 0, e_DYNAMIC);

                // Attempt to create one category more than the capacity of the
                // category manager.  This will return the default category,
                // which was created when the logger manager was created, and
                // it should have the thresholds the logger manager
                // configuration object had (ILEVELS).

                manager.setMaxNumCategories(manager.numCategories());

                CALL(whimper, ILEVELS, RET(_a).d_holder_p, e_STATIC);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                // Test attempt to create a new inheriting category when the
                // logger manager is already full.

                CALL(woofMoan, ILEVELS, ret_p->d_holder_p, e_DYNAMIC);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                CALL(woof_e, MLEVELS, RET(woof_d).d_holder_p, e_STATIC);
                ASSERTV(RET(woof_a).d_category_p == ret_p->d_category_p);

                CALL(full, ILEVELS, 0, e_DYNAMIC);
                ASSERT(TC::defaultCat == ret_p->d_category_p);

                // Make sure that all the categories returned are distinct,
                // except for the ones we EXPECT to match.

                bsl::vector<const Cat *> v(&ta);
                v.push_back(TC::defaultCat);
                v.push_back(classBaseCat);
                v.push_back(RET(bowWow).d_category_p);
                v.push_back(RET(woof_a).d_category_p);
                v.push_back(RET(meow).d_category_p);
                v.push_back(RET(meowBubble).d_category_p);
                v.push_back(RET(meowBark).d_category_p);
                v.push_back(RET(meowArf).d_category_p);
                v.push_back(RET(meowPurr).d_category_p);
                v.push_back(RET(classDerived_a).d_category_p);
                v.push_back(RET(classDerivedDerived).d_category_p);
                v.push_back(RET(meowBarkArf).d_category_p);
                v.push_back(RET(roar).d_category_p);
                v.push_back(RET(classDerivedRoar).d_category_p);
                v.push_back(RET(growl).d_category_p);

                ASSERTV(static_cast<Blp::bsls::Types::IntPtr>(v.size()),
                                                      manager.numCategories(),
                        static_cast<Blp::bsls::Types::IntPtr>(v.size()) ==
                                                      manager.numCategories());

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = ii + 1; jj < v.size(); ++jj) {
                        ASSERTV(ii, jj, v[ii]->categoryName(), v[ii] != v[jj]);
                    }
                }

                v.clear();
                v.push_back(TC::defaultCat);
                v.push_back(RET(whimper).d_category_p);
                v.push_back(RET(woofMoan).d_category_p);
                v.push_back(RET(full).d_category_p);

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = 0; jj < v.size(); ++jj) {
                        ASSERT(v[ii] == v[jj]);
                    }
                }

                // "woof" categories all match.

                v.clear();
                v.push_back(RET(woof_a).d_category_p);
                v.push_back(RET(woof_b).d_category_p);
                v.push_back(RET(woof_c).d_category_p);
                v.push_back(RET(woof_d).d_category_p);
                v.push_back(RET(woof_e).d_category_p);

                for (unsigned ii = 0; ii < v.size(); ++ii) {
                    for (unsigned jj = 0; jj < v.size(); ++jj) {
                        ASSERT(v[ii] == v[jj]);
                    }
                }
#undef  CALL
#undef  RET
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING HIERARCHICAL CATEGORIES
        //
        // Concerns:
        //: 1 That, if the category created inherits from another category, the
        //:   thresholds of the new category match those of the category
        //:   inherited from.
        //:
        //: 2 That, if the category created does not inherit and no default
        //:   threshold callback exists, the thresholds of the category are the
        //:   thresholds with which the logger manager was created.
        //:
        //: 3 TBD: If the category created does not inherit and a default
        //:   threshold callback is set, the new category takes the thresholds
        //:   dictated by the callback.  (NOTE: This concern will not be tested
        //:   until after 'LoggerCategoryUtil::addCategoryHierarchically' has
        //:   been fixed).
        //:
        //: 4 That, if the logger manager is at full capacity, the default
        //:   category, whose name is "", is returned, with thresholds set to
        //:   the values configured when the logger manager was created.
        //:
        //: 5 If a category holder is passed, it is set to the category
        //:   returned by the function under test.
        //:
        //: 6 If a second call is made with the name of a category that has
        //:   already been created, the same category will be returned.  If
        //:   category holders were passed to both calls, the second category
        //:   will be the linked to the first category.
        //
        // Plan:
        //: 1 See the doc for the template function
        //:   'TC::testSetCategoryHierarchically<KK>' where 'KK' is a const
        //:   'int' value, which calls 'Log::setCategoryHierarchically',
        //:   possibly multiple times, and verifies that it behaves properly,
        //:   and returns a reference to a unique static 'TC::ResultRec' for
        //:   each value of 'KK', which is loaded with the results of one of
        //:   the calls to 'Log::setCategoryHierarchically'.
        //:
        //: 2 Set up the macro 'CALL' defined and documented below, to call
        //:   'TC::testSetCategoryHierarchically<KK>' using the value and name
        //:   of one of the integral constants in namespace 'TC::indices'.
        //:
        //: 3 Set up the macro 'RET', defined and documented below, to return
        //:   a reference to the 'TC::ResultRec' returned by any previous
        //:   call to 'TC::testSetCategoryHierarchically<KK>'.
        //:
        //: 4 Do 3 nested loops to drive the 'enum' 'holderlessMode' through
        //:   all of its possible values, and populate the threshold aggregates
        //:   'CLEVELS', 'ILEVELS', and 'JLEVELS' with distinct values.
        //:
        //: 5 Call 'CALL' many times, with many of the names of the 'int'
        //:   constants in 'TC::indices', and observe it making categories, and
        //:   observed that hierarchical categories inherit the logging
        //:   threshold levels accordingly.
        //:
        //: 6 Call 'CALL' a few times with indices with which it has already
        //:   been called, and observe that the same category values as before
        //:   are returned, and that the category holders are not modified.
        //:
        //: 7 Set the max of the number of categories in the logger manager to
        //:   the current amount, and call 'CALL' a few times with new category
        //:   names, and observe that no new categories are created and the
        //:   default category is returned.
        //:
        //: 8 With the logger manager still full, call 'CALL' with an existing
        //:   category name and observe that the existing category is found.
        //:
        //: 9 Compare found categories to verify the ones expected to be
        //:   distinct are distinct, and the ones expected to match match.
        //
        // Testing:
        //   setCategoryHierarchically(const char *);
        //   setCategoryHierarchically(CategoryHolder *, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING HIERARCHICAL CATEGORIES\n"
                             "===============================\n";

        namespace TC = BALL_LOG_HIERARCHICAL_CATEGORIES;
        namespace Blp = BloombergLP;

        typedef TC::Cat                     Cat;
        typedef Blp::ball::LoggerManager    Manager;

        Blp::bslma::TestAllocator da("default", veryVeryVerbose);
        Blp::bslma::TestAllocator ga("global",  veryVeryVerbose);
        Blp::bslma::TestAllocator ta("test",    veryVeryVerbose);

        Blp::bslma::DefaultAllocatorGuard guard(&da);
        Blp::bslma::Default::setGlobalAllocator(&ga);

        // No category will ever have levels matching 'WLEVELS', we pass it
        // 'CALL' in the case where we expect no category to be created.

        const TC::Agg WLEVELS(TC::S::e_OFF, TC::S::e_OFF,
                                                   TC::S::e_OFF, TC::S::e_OFF);

        TC::Agg& CLEVELS = TC::callbackLevels;

        // Nesting the loops this way allows us to get deeply nested without
        // having to indent deeply and crush our code up against the right hand
        // side of the window.

        for (int th = 0; th < 3; ++th)
        for (int ti = 0; ti < TC::k_NUM_DATA; ++ti)
        for (int tj = 0; tj < TC::k_NUM_DATA; ++tj) {
            TC::HolderlessMode holderlessMode = 0 == th
                                              ? TC::e_HOLDERLESS_NONE
                                              : 1 == th
                                              ? TC::e_HOLDERLESS_BEFORE
                                              : TC::e_HOLDERLESS_AFTER;

            const TC::Data& IDATA         = TC::DATA[ti];
            const int       IL            = IDATA.d_line;
            const TC::Agg&  ILEVELS       = IDATA.d_levels;
            const bool      ISET_CALLBACK = IDATA.d_setCallbackThresholds;

            if (ISET_CALLBACK) {
                CLEVELS = ILEVELS;
                tj += TC::k_NUM_DATA;
                continue;
            }

            const TC::Data& JDATA         = TC::DATA[tj];
            const int       JL            = JDATA.d_line;
            const TC::Agg&  JLEVELS       = JDATA.d_levels;
            const bool      JSET_CALLBACK = JDATA.d_setCallbackThresholds;
            bool            JUSE_CALLBACK = JDATA.d_useCallback;

            if (JSET_CALLBACK || IL == JL) {
                continue;
            }

            TC::defaultCat = 0;
            TC::instantiationSet.clear();

            // Confirm that 'WLEVELS', 'CLEVELS', 'ILEVELS', and 'JLEVELS' are
            // all distinct values.

            ASSERTV(IL, JL, CLEVELS != WLEVELS);
            ASSERTV(IL, JL, CLEVELS != ILEVELS);
            ASSERTV(IL, JL, CLEVELS != JLEVELS);
            ASSERTV(IL, JL, ILEVELS != WLEVELS);
            ASSERTV(IL, JL, ILEVELS != JLEVELS);
            ASSERTV(IL, JL, JLEVELS != WLEVELS);

            const TC::Agg MLEVELS = JUSE_CALLBACK ? CLEVELS : ILEVELS;

            // The 'CALL' macro calls 'testSetCategoryHierarchically' with the
            // specified template parameter 'KK', 'KK' being an integer
            // constant in the namespace 'TC::indices'.  The specified
            // threshold aggregate 'expectedLevels' is passed to the test
            // function to be compared to the threshold levels of any category
            // created to verify that they are correct.  The specified category
            // holder pointer 'prevCatHolder' is passed to the test function to
            // be compared to the 'd_next_p' field in the category holder
            // initialized by the function under test.  The variable
            // 'holderlessMode' is passed as the last argument to the test
            // function to indicate whether, and when, a redundant call to the
            // function under test with no category holder is made.  The test
            // function returns a reference to a static 'ResultRec' object,
            // which was returned by the 'TC::result' function.

            TC::ResultRec *ret_p;
#undef  CALL
#define CALL(KK, expectedLevels, prevCatHolder)                               \
            ret_p = &TC::testSetCategoryHierarchically<TC::indices::KK>(      \
                                      expectedLevels,                         \
                                      TC::PositionRec(__LINE__, IL, JL, #KK), \
                                      prevCatHolder,                          \
                                      holderlessMode)

            // The macro 'RET(N)' will return a reference to static record
            // returned by the call made by 'CALL(N, ...)'.

#undef  RET
#define RET(KK)  TC::result<TC::indices::KK>()

            // Call with no logger manager, will create no category and have
            // no effect.

            CALL(bowWow, WLEVELS, 0);
            ASSERTV(0 == ret_p->d_category_p);
            ASSERTV(0 == bsl::memcmp(ret_p->d_holder_p,
                                     &TC::defaultHolder,
                                     sizeof(TC::defaultHolder)));

            // Start the logger manager singleton.

            Blp::ball::LoggerManagerConfiguration mLMC(&ta);
            ASSERT(0 == mLMC.setDefaultThresholdLevelsIfValid(
                                                   ILEVELS.recordLevel(),
                                                   ILEVELS.passLevel(),
                                                   ILEVELS.triggerLevel(),
                                                   ILEVELS.triggerAllLevel()));
            Blp::ball::LoggerManagerScopedGuard lmGuard(mLMC);
            Manager& manager = Blp::ball::LoggerManager::singleton();
            if (JUSE_CALLBACK) {
                manager.setDefaultThresholdLevelsCallback(&TC::dtlCallback);
            }

            TC::defaultCat = &manager.defaultCategory();

            // Now, call 'CALL' many times to attempt to find/create categories
            // with different names, some of whom may expect to inherit
            // threshold levels from others.

            CALL(meow, MLEVELS, 0);
            Cat *meowCat = manager.lookupCategory("meow");
            TC::setLevels(meowCat, JLEVELS);
            ASSERTV(JL, JLEVELS, TC::getLevels(meowCat),
                                            JLEVELS == TC::getLevels(meowCat));

            CALL(woof_a, MLEVELS, 0);
            const Cat *woofCat = ret_p->d_category_p;

            CALL(meowBubble, JLEVELS, 0);

            CALL(meowBark, JLEVELS, 0);
            const Cat *meowBarkCat = ret_p->d_category_p;

            TC::setLevels(meowCat, CLEVELS);

            CALL(meowArf, CLEVELS, 0);

            CALL(woof_b, MLEVELS, RET(woof_a).d_holder_p);
            ASSERT(ret_p->d_category_p == woofCat);

            CALL(woof_c, MLEVELS, RET(woof_b).d_holder_p);
            ASSERT(ret_p->d_category_p == woofCat);

            CALL(_a, ILEVELS, 0);
            ASSERTV(TC::defaultCat == ret_p->d_category_p);

            CALL(meowPurr, CLEVELS, 0);

            CALL(meowBarkArf, JLEVELS, 0);

            CALL(roar, MLEVELS, 0);

            CALL(woof_d, MLEVELS, RET(woof_c).d_holder_p);
            ASSERT(ret_p->d_category_p == woofCat);

            // Repeat a few calls with the category holders already set, the
            // category holders should not be modified.

            TC::CatHolder catHolderX;

            bsl::memcpy(&catHolderX,
                        RET(woof_b).d_holder_p,
                        sizeof(catHolderX));
            CALL(woof_b, MLEVELS, RET(woof_a).d_holder_p);
            ASSERT(ret_p->d_category_p == woofCat);
            ASSERT(!bsl::memcmp(&catHolderX,
                                ret_p->d_holder_p,
                                sizeof(*ret_p->d_holder_p)));

            bsl::memcpy(&catHolderX,
                        RET(meowBark).d_holder_p,
                        sizeof(catHolderX));
            CALL(meowBark, JLEVELS, 0);
            ASSERT(ret_p->d_category_p == meowBarkCat);
            ASSERT(!bsl::memcmp(&catHolderX,
                                ret_p->d_holder_p,
                                sizeof(*ret_p->d_holder_p)));

            bsl::memcpy(&catHolderX,
                        RET(meow).d_holder_p,
                        sizeof(catHolderX));
            CALL(meow, CLEVELS, 0);
            ASSERT(ret_p->d_category_p == meowCat);
            ASSERT(!bsl::memcmp(&catHolderX,
                                ret_p->d_holder_p,
                                sizeof(*ret_p->d_holder_p)));

            // Attempt to create one category more than the capacity of the
            // category manager.  This will return the default category, which
            // was created when the logger manager was created, and it should
            // have the thresholds the logger manager configuration object had
            // (ILEVELS).

            manager.setMaxNumCategories(manager.numCategories());

            CALL(whimper, ILEVELS, RET(_a).d_holder_p);
            ASSERT(TC::defaultCat == ret_p->d_category_p);
            ASSERT(!bsl::strcmp("", ret_p->d_category_p->categoryName()));

            // Test attempt to create a new category that will inherit from an
            // existing category when the logger manager is already full.

            CALL(woofMoan, ILEVELS, RET(whimper).d_holder_p);
            ASSERT(TC::defaultCat == ret_p->d_category_p);
            ASSERT(!bsl::strcmp("", ret_p->d_category_p->categoryName()));

            // Test attempt to set an already existing category with the
            // category manager full.

            CALL(woof_e, MLEVELS, RET(woof_d).d_holder_p);
            ASSERT(ret_p->d_category_p == woofCat);

            // Make sure that all the categories returned are distinct, except
            // that all the 'woof_*'s match, and several categories match the
            // default category (except that all the 'woof*'s match, and
            // 'whimperCat == woofMoanCat == TC::defaultCat').

            bsl::vector<const void *> v(&ta);
            v.push_back(RET(meow).d_category_p);
            v.push_back(RET(woof_a).d_category_p);
            v.push_back(RET(meowBubble).d_category_p);
            v.push_back(RET(meowBark).d_category_p);
            v.push_back(RET(meowArf).d_category_p);
            v.push_back(RET(meowPurr).d_category_p);
            v.push_back(RET(meowBarkArf).d_category_p);
            v.push_back(RET(roar).d_category_p);
            v.push_back(TC::defaultCat);
            ASSERT(static_cast<IntPtr>(v.size()) == manager.numCategories());

            for (unsigned ii = 0; ii < v.size(); ++ii) {
                for (unsigned jj = 0; jj < ii; ++jj) {
                    ASSERT(v[ii] != v[jj]);
                }
            }

            v.clear();
            v.push_back(RET(woof_a).d_category_p);
            v.push_back(RET(woof_b).d_category_p);
            v.push_back(RET(woof_c).d_category_p);
            v.push_back(RET(woof_d).d_category_p);
            v.push_back(RET(woof_e).d_category_p);

            for (unsigned ii = 0; ii < v.size(); ++ii) {
                for (unsigned jj = 0; jj < v.size(); ++jj) {
                    ASSERT(v[ii] == v[jj]);
                }
            }

            v.clear();
            v.push_back(TC::defaultCat);
            v.push_back(RET(whimper).d_category_p);
            v.push_back(RET(woofMoan).d_category_p);

            for (unsigned ii = 0; ii < v.size(); ++ii) {
                for (unsigned jj = 0; jj < v.size(); ++jj) {
                    ASSERT(v[ii] == v[jj]);
                }
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING DEGENERATE LOG MACROS USAGE
        //  Sanity test of the degenerate log macros use cases.
        //
        // Concern:
        //: 1 'BALL_LOG_END' can be optionally appended to the log stream
        //:   output.
        //:
        //: 2 Empty log blocks and noop stream output are valid expressions.
        //
        // Plan:
        //: 1 Using brute-force, exercise block-scope macros.  (C-1,2)
        //
        // Testing:
        //   CONCERN: DEGENERATE LOG MACROS USAGE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING DEGENERATE LOG MACROS USAGE"
                               << "\n==================================="
                               << bsl::endl;

        namespace Blp = BloombergLP;

        bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                               &u::incCallback;
        Blp::bslma::TestAllocator ta(veryVeryVeryVerbose);

        Blp::ball::LoggerManagerConfiguration lmc;
        Blp::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        Blp::ball::LoggerManager& manager =
                                         Blp::ball::LoggerManager::singleton();

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        Sev::e_OFF,
                                        Sev::e_TRACE,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassOFF",
                                        Sev::e_OFF,
                                        Sev::e_OFF,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        {
            BALL_LOG_SET_CATEGORY("PassTRACE");

            BALL_LOG_TRACE;
            BALL_LOG_TRACE << BALL_LOG_END;
            BALL_LOG_TRACE_BLOCK {}

            BALL_LOG_DEBUG;
            BALL_LOG_DEBUG << BALL_LOG_END;
            BALL_LOG_DEBUG_BLOCK {}

            BALL_LOG_INFO;
            BALL_LOG_INFO << BALL_LOG_END;
            BALL_LOG_INFO_BLOCK {}

            BALL_LOG_WARN;
            BALL_LOG_WARN << BALL_LOG_END;
            BALL_LOG_WARN_BLOCK {}

            BALL_LOG_ERROR;
            BALL_LOG_ERROR << BALL_LOG_END;
            BALL_LOG_ERROR_BLOCK {}

            BALL_LOG_FATAL;
            BALL_LOG_FATAL << BALL_LOG_END;
            BALL_LOG_FATAL_BLOCK {}

            int severity = Sev::e_TRACE;

            BALL_LOG_STREAM(severity - 1);
            BALL_LOG_STREAM(severity);
            BALL_LOG_STREAM(severity + 1);
            BALL_LOG_STREAM_BLOCK(severity - 1) {}
            BALL_LOG_STREAM_BLOCK(severity) {}
            BALL_LOG_STREAM_BLOCK(severity + 1) {}

            BALL_LOGCB_TRACE(callback);
            BALL_LOGCB_TRACE(callback) << BALL_LOG_END;
            BALL_LOGCB_TRACE_BLOCK(callback) {}

            BALL_LOGCB_DEBUG(callback);
            BALL_LOGCB_DEBUG(callback) << BALL_LOG_END;
            BALL_LOGCB_DEBUG_BLOCK(callback) {}

            BALL_LOGCB_INFO(callback);
            BALL_LOGCB_INFO(callback) << BALL_LOG_END;
            BALL_LOGCB_INFO_BLOCK(callback) {}

            BALL_LOGCB_WARN(callback);
            BALL_LOGCB_WARN(callback) << BALL_LOG_END;
            BALL_LOGCB_WARN_BLOCK(callback) {}

            BALL_LOGCB_ERROR(callback);
            BALL_LOGCB_ERROR(callback) << BALL_LOG_END;
            BALL_LOGCB_ERROR_BLOCK(callback) {}

            BALL_LOGCB_FATAL(callback);
            BALL_LOGCB_FATAL(callback) << BALL_LOG_END;
            BALL_LOGCB_FATAL_BLOCK(callback) {}

            BALL_LOGCB_STREAM(severity - 1, callback);
            BALL_LOGCB_STREAM(severity, callback);
            BALL_LOGCB_STREAM(severity + 1, callback);
            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {}
            BALL_LOGCB_STREAM_BLOCK(severity, callback) {}
            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {}
        }

        {
            BALL_LOG_SET_CATEGORY("PassOFF");

            BALL_LOG_TRACE;
            BALL_LOG_TRACE << BALL_LOG_END;
            BALL_LOG_TRACE_BLOCK {}

            BALL_LOG_DEBUG;
            BALL_LOG_DEBUG << BALL_LOG_END;
            BALL_LOG_DEBUG_BLOCK {}

            BALL_LOG_INFO;
            BALL_LOG_INFO << BALL_LOG_END;
            BALL_LOG_INFO_BLOCK {}

            BALL_LOG_WARN;
            BALL_LOG_WARN << BALL_LOG_END;
            BALL_LOG_WARN_BLOCK {}

            BALL_LOG_ERROR;
            BALL_LOG_ERROR << BALL_LOG_END;
            BALL_LOG_ERROR_BLOCK {}

            BALL_LOG_FATAL;
            BALL_LOG_FATAL << BALL_LOG_END;
            BALL_LOG_FATAL_BLOCK {}

            int severity = Sev::e_FATAL;

            BALL_LOG_STREAM(severity - 1);
            BALL_LOG_STREAM(severity);
            BALL_LOG_STREAM(severity + 1);
            BALL_LOG_STREAM_BLOCK(severity - 1) {}
            BALL_LOG_STREAM_BLOCK(severity) {}
            BALL_LOG_STREAM_BLOCK(severity + 1) {}

            BALL_LOGCB_TRACE(callback);
            BALL_LOGCB_TRACE(callback) << BALL_LOG_END;
            BALL_LOGCB_TRACE_BLOCK(callback) {}

            BALL_LOGCB_DEBUG(callback);
            BALL_LOGCB_DEBUG(callback) << BALL_LOG_END;
            BALL_LOGCB_DEBUG_BLOCK(callback) {}

            BALL_LOGCB_INFO(callback);
            BALL_LOGCB_INFO(callback) << BALL_LOG_END;
            BALL_LOGCB_INFO_BLOCK(callback) {}

            BALL_LOGCB_WARN(callback);
            BALL_LOGCB_WARN(callback) << BALL_LOG_END;
            BALL_LOGCB_WARN_BLOCK(callback) {}

            BALL_LOGCB_ERROR(callback);
            BALL_LOGCB_ERROR(callback) << BALL_LOG_END;
            BALL_LOGCB_ERROR_BLOCK(callback) {}

            BALL_LOGCB_FATAL(callback);
            BALL_LOGCB_FATAL(callback) << BALL_LOG_END;
            BALL_LOGCB_FATAL_BLOCK(callback) {}

            BALL_LOGCB_STREAM(severity - 1, callback);
            BALL_LOGCB_STREAM(severity, callback);
            BALL_LOGCB_STREAM(severity + 1, callback);
            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {}
            BALL_LOGCB_STREAM_BLOCK(severity, callback) {}
            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {}
        }
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING 'BALL_LOGCB_*_BLOCK' MACROS
        //
        // Concerns:
        //: 1 Block macros provide access to 'BALL_LOG_OUTPUT_STREAM'.
        //
        // Plan:
        //: 1 Using brute-force, exercise block-scope macros.  (C-1)
        //
        // Testing:
        //   CONCERN: 'BALL_LOGCB_*_BLOCK' MACROS
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'BALL_LOGCB_*_BLOCK' MACROS"
                               << "\n==================================="
                               << bsl::endl;

        namespace Blp = BloombergLP;
        typedef Blp::ball::Severity Sev;

        bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                               &u::incCallback;

        Blp::bslma::TestAllocator ta(veryVeryVeryVerbose);

        Blp::ball::LoggerManagerConfiguration lmc;
        Blp::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        bsl::shared_ptr<Blp::ball::TestObserver> observer(
                       new (ta) Blp::ball::TestObserver(&bsl::cout, &ta), &ta);

        Blp::ball::LoggerManager& manager =
                                         Blp::ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        Sev::e_OFF,
                                        Sev::e_TRACE,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassDEBUG",
                                        Sev::e_OFF,
                                        Sev::e_DEBUG,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassINFO",
                                        Sev::e_OFF,
                                        Sev::e_INFO,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassWARN",
                                        Sev::e_OFF,
                                        Sev::e_WARN,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassERROR",
                                        Sev::e_OFF,
                                        Sev::e_ERROR,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassFATAL",
                                        Sev::e_OFF,
                                        Sev::e_FATAL,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        {
            BALL_LOG_SET_CATEGORY("PassTRACE");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == u::numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_TRACE);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == u::numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(8     == u::numIncCallback);
            ASSERTV(N + 8 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(8     == u::numIncCallback);
            ASSERTV(N + 8 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassDEBUG");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_DEBUG);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == u::numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == u::numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == u::numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassINFO");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_INFO);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == u::numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == u::numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassWARN");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_WARN);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == u::numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassERROR");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_ERROR);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == u::numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassFATAL");

            int count = 0;
            const int N = observer->numPublishedRecords();

            u::numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == u::numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_FATAL);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == u::numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == u::numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING 'BALL_LOG_*_BLOCK' MACROS
        //
        // Concerns:
        //: 1 Block macros provide access to 'BALL_LOG_OUTPUT_STREAM'.
        //
        // Plan:
        //: 1 Using brute-force, exercise block-scope macros.  (C-1)
        //
        // Testing:
        //   CONCERN: 'BALL_LOG_*_BLOCK' MACROS
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'BALL_LOG_*_BLOCK' MACROS"
                               << "\n================================="
                               << bsl::endl;

        namespace Blp = BloombergLP;

        Blp::bslma::TestAllocator ta(veryVeryVeryVerbose);

        Blp::ball::LoggerManagerConfiguration lmc;
        Blp::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        bsl::shared_ptr<Blp::ball::TestObserver> observer(
                       new (ta) Blp::ball::TestObserver(&bsl::cout, &ta), &ta);

        Blp::ball::LoggerManager& manager =
                                         Blp::ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        Sev::e_OFF,
                                        Sev::e_TRACE,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassDEBUG",
                                        Sev::e_OFF,
                                        Sev::e_DEBUG,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassINFO",
                                        Sev::e_OFF,
                                        Sev::e_INFO,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassWARN",
                                        Sev::e_OFF,
                                        Sev::e_WARN,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassERROR",
                                        Sev::e_OFF,
                                        Sev::e_ERROR,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        ASSERT(0 != manager.setCategory("PassFATAL",
                                        Sev::e_OFF,
                                        Sev::e_FATAL,
                                        Sev::e_OFF,
                                        Sev::e_OFF));

        {
            BALL_LOG_SET_CATEGORY("PassTRACE");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(N + 6 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_TRACE);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(N + 7 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(N + 8 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(N + 8 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassDEBUG");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_DEBUG);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(N + 6 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(N + 7 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(N + 7 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassINFO");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_INFO);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(N + 6 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(N + 6 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassWARN");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_WARN);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassERROR");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_ERROR);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassFATAL");

            int count = 0;
            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(Sev::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(Sev::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(Sev::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(Sev::e_FATAL);

            BALL_LOG_STREAM_BLOCK(severity - 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity - 1));

            BALL_LOG_STREAM_BLOCK(severity) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(severity));

            BALL_LOG_STREAM_BLOCK(severity + 1) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(severity + 1));
        }

        {
            BALL_LOG_SET_CATEGORY("PassTRACE");

            const int N = observer->numPublishedRecords();

            BALL_LOG_FATAL_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";

            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOG_ERROR_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";

            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOG_WARN_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";

            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOG_INFO_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";

            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOG_DEBUG_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";


            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOG_TRACE_BLOCK
                BALL_LOG_OUTPUT_STREAM << "message";

            ASSERTV(N + 6 == observer->numPublishedRecords());
        }
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING NAMESPACE-SCOPE LOGGING
        //
        // Concerns:
        //: 1 That no more than one namespace-scope category can be defined in
        //:   a namespace.
        //:
        //: 2 That all manner of logging macros (stream-based, callback-based,
        //:   variadic) work with namespace-scope categories.
        //:
        //: 3 That static and dynamic categories hide namespace-scope
        //:   categories.
        //
        // Plan:
        //: 1 Using brute-force, define two namespace-scope categories that
        //:   together exercise all of the concerns.  (C-1..3)
        //
        // Testing:
        //   BALL_LOG_SET_NAMESPACE_CATEGORY(CATEGORY)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING NAMESPACE-SCOPE LOGGING"
                               << "\n==============================="
                               << bsl::endl;

        using namespace BloombergLP;  // okay here

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_OFF,    // record level
                                 ball::Severity::e_TRACE,  // passthrough level
                                 ball::Severity::e_OFF,    // trigger level
                                 ball::Severity::e_OFF);   // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::TestObserver>  observer =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        ASSERT(1 == ball::TestObserver::numInstances());

        // This log should pick up the global namespace category.
        BALL_LOG_INFO << "INFO log in the top level namespace category";

        ASSERT(1 == observer->numPublishedRecords());
        ASSERT(0 == bsl::strcmp(
                    "BALL_LOG.T",
                    observer->lastPublishedRecord().fixedFields().category()));


        logNamespaceOverride();
        ASSERT(2 == observer->numPublishedRecords());
        ASSERT(0 == bsl::strcmp(
                    "BALL_LOG.T.OVERRIDE",
                    observer->lastPublishedRecord().fixedFields().category()));

        u::logNamespaceOverride();
        ASSERT(3 == observer->numPublishedRecords());
        ASSERT(0 == bsl::strcmp(
                    "BALL_LOG.T.OVERRIDE.U",
                    observer->lastPublishedRecord().fixedFields().category()));

        // Exercise logging in BALL_LOG_TEST_NAMESPACE_LOGGING
        {
            using namespace BALL_LOG_TEST_NAMESPACE_LOGGING;
            logNamespaceCategory();

            ASSERT(4 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "NS.A",
                    observer->lastPublishedRecord().fixedFields().category()));

            // Looking up and changing category threshold levels.
            ball::Category *category = manager.lookupCategory("NS.A");
            ASSERT(0 != category);

            int rc = category->setLevels(ball::Severity::e_TRACE,
                                         ball::Severity::e_TRACE,
                                         ball::Severity::e_ERROR,
                                         ball::Severity::e_FATAL);
            ASSERT(0 == rc);

            // Nested namespace logging
            BALL_LOG_TEST_NAMESPACE_LOGGING_B::logNamespaceCategory();

            ASSERT(5 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "NS.A.B",
                    observer->lastPublishedRecord().fixedFields().category()));

            // Checking that category levels were inherited from the parent
            category = manager.lookupCategory("NS.A.B");
            ASSERT(0 != category);
            ASSERT(ball::Severity::e_TRACE == category->recordLevel());
            ASSERT(ball::Severity::e_TRACE == category->passLevel());
            ASSERT(ball::Severity::e_ERROR == category->triggerLevel());
            ASSERT(ball::Severity::e_FATAL == category->triggerAllLevel());

            // Testing that local/class scopes take precedence.
            BALL_LOG_TEST_CASE_29::ClassScopeLoggerA::
                                          classMethodThatLogsToClassCategory();

            ASSERT(6 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY A",
                    observer->lastPublishedRecord().fixedFields().category()));

            BALL_LOG_TEST_CASE_29::ClassScopeLoggerA mX;
            const BALL_LOG_TEST_CASE_29::ClassScopeLoggerA& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(8 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "STATIC LOCAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(9 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY A",
                    observer->lastPublishedRecord().fixedFields().category()));

            BALL_LOG_TEST_CASE_29::globalFunctionThatLogsToLocalCategory();

            ASSERT(10 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "GLOBAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise logging in BALL_LOG_TEST_NAMESPACE_LOGGING2
        {
            BALL_LOG_TEST_NAMESPACE_LOGGING2::Test<int> a;
            a.func();
            ASSERT(11 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "NS.LM2",
                    observer->lastPublishedRecord().fixedFields().category()));

            BALL_LOG_TEST_NAMESPACE_LOGGING2::func2();
            ASSERT(12 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "NS.L2",
                    observer->lastPublishedRecord().fixedFields().category()));
        }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING CLASS-SCOPE LOGGING
        //
        // Concerns:
        //: 1 That a class-scope category can be declared in the 'public',
        //:   'private', or 'protected' interface of a class.
        //:
        //: 2 That more than one class-scope category can be defined in a
        //:   translation unit.
        //:
        //: 3 That all manner of logging macros (stream-based, callback-based,
        //:   variadic) work with class-scope categories.
        //:
        //: 4 That all methods of a class can log to a class-scope category, in
        //:   particular, 'inline' methods regardless of where they are defined
        //:   (within the class definition or outside; before or after the use
        //:   of the 'BALL_LOG_SET_CLASS_CATEGORY' macro).
        //:
        //: 5 That static and dynamic categories hide class-scope categories.
        //:
        //: 6 That class-scope categories can be used in class templates.
        //
        // Plan:
        //: 1 Using brute-force, define two classes for which class-scope
        //:   categories are defined that together exercise all of the
        //:   concerns.  (C-1..5)
        //
        // Testing:
        //   BALL_LOG_SET_CLASS_CATEGORY(CATEGORY)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING CLASS-SCOPE LOGGING"
                               << "\n==========================="
                               << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_29;
        using namespace BloombergLP;  // okay here

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 Sev::e_OFF,    // record level
                                 Sev::e_TRACE,  // passthrough level
                                 Sev::e_OFF,    // trigger level
                                 Sev::e_OFF);   // triggerAll level
        ball::LoggerManagerScopedGuard guard(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::TestObserver>  observer =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        ASSERT(1 == ball::TestObserver::numInstances());

        // Exercise 'ClassScopeLoggerA'
        {
            u::numIncCallback = 0;
            ClassScopeLoggerA::classMethodThatLogsToClassCategory();

            ASSERT(1 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY A",
                    observer->lastPublishedRecord().fixedFields().category()));
            ASSERT(1 == u::numIncCallback);

            ClassScopeLoggerA mX;  const ClassScopeLoggerA& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(3 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "STATIC LOCAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(4 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY A",
                    observer->lastPublishedRecord().fixedFields().category()));

            globalFunctionThatLogsToLocalCategory();

            ASSERT(5 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "GLOBAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLoggerB'
        {
            u::numIncCallback = 0;
            ClassScopeLoggerB::classMethodThatLogsToClassCategory();

            ASSERT(6 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY B",
                    observer->lastPublishedRecord().fixedFields().category()));
            ASSERT(0 == u::numIncCallback);  // callback not relevant here

            ClassScopeLoggerB mX;  const ClassScopeLoggerB& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a dynamic
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(8 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "DYNAMIC LOCAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(9 == observer->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                    "CLASS CATEGORY B",
                    observer->lastPublishedRecord().fixedFields().category()));

            globalFunctionThatLogsToLocalCategory();

            ASSERT(10 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "GLOBAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLogger<int>'
        {
            typedef ClassScopeLogger<int> ClassInt;

            ClassInt::classMethodThatLogsToClassCategory();

            ASSERT(11 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "CLASS TEMPLATE CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            ClassInt mX;  const ClassInt& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(13 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "STATIC LOCAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(14 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "CLASS TEMPLATE CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLogger<double>'
        {
            typedef ClassScopeLogger<double> ClassDouble;

            ClassDouble::classMethodThatLogsToClassCategory();

            ASSERT(15 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "CLASS TEMPLATE CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            ClassDouble mX;  const ClassDouble& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(17 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "STATIC LOCAL CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(18 == observer->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                    "CLASS TEMPLATE CATEGORY",
                    observer->lastPublishedRecord().fixedFields().category()));
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'BALL_LOG_IS_ENABLED(SEVERITY)'
        //
        // Concerns:
        //:  1. If the logger manager is not initialized,
        //:     'BALL_LOG_IS_ENABLED' returns 'true' for severities higher
        //:     than 'WARN', and 'false' otherwise.
        //:
        //:  2. If the logger manager is initialized, 'BALL_LOG_IS_ENABLED'
        //:     returns 'true' if any of the thresholds configured for the
        //:     current category are higher than the severity.
        //:
        //:  3. 'BALL_LOG_IS_ENABLED' tests the thresholds configured for the
        //:     current category by rule-based logging.
        //
        // Plan:
        //   1. Do not initialize a logger manager, and test calling
        //      'BALL_LOG_IS_ENABLED' with various severities.  (C-1)
        //
        //   2. Initialize a logger manager, and for a set of possible
        //      severities, exhaustively test each combination of threshold
        //      severity value with 'BALL_LOG_IS_ENABLED' severity value, for
        //      each of the 4 thresholds (record, passthrough, trigger, and
        //      trigger-all). (C-2)
        //
        //   3. Exhaustively test each combination of a rule-enabled threshold
        //      severity value with 'BALL_LOG_IS_ENABLED' severity value. (C-3)
        //
        // Testing:
        //   BALL_IS_ENABLED(SEVERITY)
        // --------------------------------------------------------------------

        using namespace BloombergLP;  // okay here

        if (verbose) bsl::cout << "\nTESTING 'BALL_LOG_IS_ENABLED(SEVERITY)'"
                               << "\n======================================="
                               << bsl::endl;


        if (verbose) bsl::cout << "\tTest without a logger manager.\n";

        {
            BALL_LOG_SET_CATEGORY("TEST.CATEGORY1");
            ASSERT(!BALL_LOG_IS_ENABLED(TRACE));
            ASSERT(!BALL_LOG_IS_ENABLED(DEBUG));
            ASSERT(!BALL_LOG_IS_ENABLED(INFO));
            ASSERT( BALL_LOG_IS_ENABLED(WARN));
            ASSERT( BALL_LOG_IS_ENABLED(ERROR));
            ASSERT( BALL_LOG_IS_ENABLED(FATAL));
        }

        const int DATA[] = { OFF, TRACE, DEBUG, INFO, WARN, ERROR, FATAL };
        enum { NUM_DATA = sizeof (DATA) / sizeof (*DATA) };

        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(OFF, OFF, OFF, OFF);

        ball::LoggerManagerScopedGuard guard(configuration, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        manager.registerObserver(observer, "default");

        if (verbose) bsl::cout << "\tExhaustively test w/o logging rules.\n";
        {
            BALL_LOG_SET_CATEGORY("TEST.CATEGORY2");
            for (int thresholdIdx = 0; thresholdIdx < 4; ++thresholdIdx) {
                for (int i = 0; i < NUM_DATA; ++i) {
                    Thresholds threshold(OFF, OFF, OFF, OFF);
                    switch (thresholdIdx) {
                      case 0: threshold.setRecordLevel(DATA[i]); break;
                      case 1: threshold.setPassLevel(DATA[i]); break;
                      case 2: threshold.setTriggerLevel(DATA[i]); break;
                      case 3: threshold.setTriggerAllLevel(DATA[i]); break;
                      default: BSLS_ASSERT(false);
                    }
                    manager.setCategory("TEST.CATEGORY2",
                                        threshold.recordLevel(),
                                        threshold.passLevel(),
                                        threshold.triggerLevel(),
                                        threshold.triggerAllLevel());
                    for (int j = 1; j < NUM_DATA; ++j) {
                        bool EXP = DATA[j] <= DATA[i];
                        ASSERTV(EXP, DATA[i], DATA[j],
                                EXP == BALL_LOG_IS_ENABLED(DATA[j]));
                    }
                }
            }
        }

        if (verbose) bsl::cout << "\tTest w/ logging rules." << bsl::endl;
        {
            BALL_LOG_SET_CATEGORY("TEST.CATEGORY3");
            for (int i = 0; i < NUM_DATA; ++i) {
                BloombergLP::ball::Rule rule("TEST.CATEGORY3",
                                             OFF, DATA[i], OFF, OFF);
                manager.addRule(rule);
                for (int j = 1;  j < NUM_DATA; ++j) {
                    bool EXP = DATA[j] <= DATA[i];
                    ASSERTV(EXP, DATA[i], DATA[j],
                            EXP == BALL_LOG_IS_ENABLED(DATA[j]));
                }
                manager.removeAllRules();
            }
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING RULE-BASED LOGGING: 'logMessage'
        //
        // Concerns:
        //   That the 'logMessage' method uses the current installed rules when
        //   determining whether to log a message.
        //
        // Plan:
        //   Create a series of test threshold values and a series of
        //   threshold-aggregate values.  Maintain, over the test, the
        //   expected  number of published records and the number of records
        //   currently in the buffer, at any time (note that the records in
        //   the buffered are added to the published records when a trigger
        //   occurs).  For each threshold-aggregate value: create a category
        //   with that threshold-aggregate value; test the 'logMessage' method
        //   with a variety of severity levels without a logging rule, then for
        //   each test threshold-aggregate value, create a logging rule that
        //   applies to the category and test the 'logMessage' method.  Compare
        //   the expected number of published records against the actual number
        //   of published records.
        //
        // Testing:
        //   RULE-BASED LOGGING: logMessage(const Category *, int, Record *);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING RULE-BASED LOGGING: 'logMessage'\n"
                      << "========================================\n";

        using namespace BloombergLP;  // okay here

        int VALUES[] = { 1,
                         Sev::e_FATAL - 1,
                         Sev::e_FATAL,
                         Sev::e_FATAL + 1,
                         Sev::e_ERROR - 1,
                         Sev::e_ERROR,
                         Sev::e_ERROR + 1,
                         Sev::e_WARN - 1,
                         Sev::e_WARN,
                         Sev::e_WARN + 1,
                         Sev::e_INFO - 1,
                         Sev::e_INFO,
                         Sev::e_INFO + 1,
                         Sev::e_DEBUG - 1,
                         Sev::e_DEBUG,
                         Sev::e_DEBUG + 1,
                         Sev::e_TRACE - 1,
                         Sev::e_TRACE,
                         Sev::e_TRACE + 1
        };
        enum { NUM_VALUES = sizeof (VALUES)/sizeof(*VALUES) };
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
        ball::LoggerManagerConfiguration lmc;
        lmc.setTriggerMarkers(
                             ball::LoggerManagerConfiguration::e_NO_MARKERS);

        ball::LoggerManagerScopedGuard guard(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::TestObserver>  observer =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        for (int i = 0; i < (int) thresholds.size(); ++i) {
            ball::Category *category =
                          manager.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                ball::Record *record = manager.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                Obj::logMessage(category, VALUES[j], record);
                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                ASSERTV(i,j, numPublished == observer->numPublishedRecords());
            }

            // Test with rule that does not apply.
            manager.addRule(ball::Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                ball::Record *record = manager.getLogger().getRecord("X", 1);

                // Figure out the various thresholds.  Note that we have only
                // one thread, so trigger is equivalent to triggerAll.
                bool buffer  = VALUES[j] <= thresholds[i].recordLevel();
                bool pass    = VALUES[j] <= thresholds[i].passLevel();
                bool trigger = VALUES[j] <= thresholds[i].triggerLevel()
                            || VALUES[j] <= thresholds[i].triggerAllLevel();

                Obj::logMessage(category, VALUES[j], record);
                if (buffer) { ++numBufferedRecords; }
                if (pass)   { ++numPublished; }
                if (trigger) {
                    numPublished += numBufferedRecords;
                    numBufferedRecords = 0;
                }
                ASSERTV(i,j, numPublished == observer->numPublishedRecords());
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                manager.removeAllRules();
                ball::Rule rule("Test*",
                                thresholds[j].recordLevel(),
                                thresholds[j].passLevel(),
                                thresholds[j].triggerLevel(),
                                thresholds[j].triggerAllLevel());
                manager.addRule(rule);
                for (int k = 0; k < NUM_VALUES; ++k) {
                    ball::Record *record =
                                         manager.getLogger().getRecord("X",1);

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

                    Obj::logMessage(category, VALUES[k], record);
                    if (buffer) { ++numBufferedRecords; }
                    if (pass)   { ++numPublished; }
                    if (trigger) {
                        numPublished += numBufferedRecords;
                        numBufferedRecords = 0;
                    }
                    ASSERTV(i, j, k,
                            numPublished == observer->numPublishedRecords());
                }
            }
            manager.removeAllRules();
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING RULE-BASED LOGGING: 'isCategoryEnabled'
        //
        // Concerns:
        //   That the 'isCategoryEnabled' method is using the current installed
        //   rules when determining whether a category is enabled.
        //
        // Plan:
        //   Create a series of test threshold values and a series of
        //   threshold-aggregate values.  Test the 'isCategoryEnabled' method
        //   for each test threshold value without a logger manager.  Then
        //   create a logger manager.  For each threshold-aggregate value:
        //   create a category with that threshold-aggregate value, test the
        //   'isCategoryEnabled' method with a variety of severity levels
        //   without a logging rule, then for each test threshold-aggregate
        //   value, create a logging rule that applies to the category and test
        //   the 'isCategoryEnabled' method.  Verify that the
        //   'isCategoryEnabled' method behaves correctly given the categories
        //   threshold-aggregate, the rule's threshold-aggregate, and the
        //   supplied severity.
        //
        // Testing:
        //   RULE-BASED LOGGING: bool isCategoryEnabled(Holder *, int);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING RULE-BASED LOGGING: 'isCategoryEnabled'\n"
                      << "===============================================\n";

        using namespace BloombergLP;  // okay here

        int VALUES[] = { 1,
                         Sev::e_FATAL - 1,
                         Sev::e_FATAL,
                         Sev::e_FATAL + 1,
                         Sev::e_ERROR - 1,
                         Sev::e_ERROR,
                         Sev::e_ERROR + 1,
                         Sev::e_WARN - 1,
                         Sev::e_WARN,
                         Sev::e_WARN + 1,
                         Sev::e_INFO - 1,
                         Sev::e_INFO,
                         Sev::e_INFO + 1,
                         Sev::e_DEBUG - 1,
                         Sev::e_DEBUG,
                         Sev::e_DEBUG + 1,
                         Sev::e_TRACE - 1,
                         Sev::e_TRACE,
                         Sev::e_TRACE + 1
        };
        enum { NUM_VALUES = sizeof (VALUES)/sizeof(*VALUES) };

        bsl::vector<Thresholds> thresholds;
        for (int i = 0; i < NUM_VALUES; ++i) {
            thresholds.push_back(Thresholds(VALUES[i], 255, 255, 255));
            thresholds.push_back(Thresholds(255, VALUES[i], 255, 255));
            thresholds.push_back(Thresholds(255, 255, VALUES[i], 255));
            thresholds.push_back(Thresholds(255, 255, 255, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tSanity test without a logger manager" << bsl::endl;
        }
        for (int i = 0; i < NUM_VALUES; ++i) {
            bool enabled = VALUES[i] <= Sev::e_WARN;
            Holder holder = { { 0 }, { 0 }, { 0 } };
            ASSERT(enabled == Obj::isCategoryEnabled(&holder, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tTest with and without an applicable rule."
                      << bsl::endl;
        }

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard   guard(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::TestObserver>  observer =
                              bsl::make_shared<ball::TestObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Create a test category for each test threshold-aggregate
        for (int i = 0; i < (int) thresholds.size(); ++i) {
            ball::Category *category =
                          manager.setCategory("TestCategory",
                                              thresholds[i].recordLevel(),
                                              thresholds[i].passLevel(),
                                              thresholds[i].triggerLevel(),
                                              thresholds[i].triggerAllLevel());

            // Test 'isCategoryEnabled' with no rules.
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                Holder holder = { { 0 }, { category }, { 0 } };
                ASSERTV(i, j, enabled ==
                                   Obj::isCategoryEnabled(&holder, VALUES[j]));
            }

            // Test with rule that does not apply.
            manager.addRule(ball::Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                Holder holder = { { 0 }, { category } , { 0 } };
                ASSERTV(i, j, enabled ==
                                   Obj::isCategoryEnabled(&holder, VALUES[j]));
            }

            // Test with a rule that does apply.
            for (int j = 0; j < (int) thresholds.size(); ++j) {
                manager.removeAllRules();
                ball::Rule rule("Test*",
                               thresholds[j].recordLevel(),
                               thresholds[j].passLevel(),
                               thresholds[j].triggerLevel(),
                               thresholds[j].triggerAllLevel());
                manager.addRule(rule);
                for (int k = 0; k < NUM_VALUES; ++k) {
                    int maxLevel =
                        bsl::max(Thresholds::maxLevel(thresholds[i]),
                                 Thresholds::maxLevel(thresholds[j]));
                    bool enabled = VALUES[k] <= maxLevel;
                    Holder holder = { { 0 }, { category }, { 0 } };
                    ASSERTV(i, j, k,
                            enabled ==
                                   Obj::isCategoryEnabled(&holder, VALUES[k]));
                    if (enabled !=
                        Obj::isCategoryEnabled(&holder, VALUES[k])) {
                        P_(thresholds[i]); P_(thresholds[j]); P(enabled);
                    }
                }
            }
            manager.removeAllRules();
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_SET_DYNAMIC_CATEGORY and BALL_LOGVA in multiple
        // threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BALL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BALL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BALL_LOG_TEST_CASE_24;

        numIterations = 10;
        arg1          = -99.244;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            u::executeInParallel(NUM_THREADS, workerThread24);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);
            BloombergLP::ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);
            BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

            bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            u::executeInParallel(NUM_THREADS, workerThread24);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            u::CerrBufferGuard   cerrBufferGuard;

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            u::executeInParallel(NUM_THREADS, workerThread24);
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_SET_CATEGORY and BALL_LOGVA in multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BALL_LOG_SET_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BALL_LOG_SET_CATEGORY\n"
                      << "=============================\n";

        using namespace BALL_LOG_TEST_CASE_23;

        numIterations = 10;
        arg1          = -99.234;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            u::executeInParallel(NUM_THREADS, workerThread23);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);
            BloombergLP::ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);
            BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

            bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            u::executeInParallel(NUM_THREADS, workerThread23);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            u::executeInParallel(NUM_THREADS, workerThread23);
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_SET_DYNAMIC_CATEGORY and BALL_LOG_TRACE in
        // multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BALL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BALL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BALL_LOG_TEST_CASE_22;

        numIterations = 10;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            u::executeInParallel(NUM_THREADS, workerThread22);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);
            BloombergLP::ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);
            BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

            bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            u::executeInParallel(NUM_THREADS, workerThread22);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            u::executeInParallel(NUM_THREADS, workerThread22);
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_SET_CATEGORY and BALL_LOG_TRACE in multiple threads
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BALL_LOG_SET_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BALL_LOG_SET_CATEGORY\n"
                      << "=============================\n";

        using namespace BALL_LOG_TEST_CASE_21;

        numIterations = 10;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            u::executeInParallel(NUM_THREADS, workerThread21);
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);
            BloombergLP::ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
               BloombergLP::ball::Severity::e_TRACE,  // record level
               BloombergLP::ball::Severity::e_WARN,   // passthrough level
               BloombergLP::ball::Severity::e_ERROR,  // trigger level
               BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);
            BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

            bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            u::executeInParallel(NUM_THREADS, workerThread21);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            u::CerrBufferGuard cerrBufferGuard;

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            u::executeInParallel(NUM_THREADS, workerThread21);
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING STATIC FUNCTIONS
        //
        // Concerns:
        //   TBD
        //
        // Plan:
        //   TBD
        //
        // Testing:
        //   bool isCategoryEnabled(Holder *holder, int severity)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing static functions\n"
                      << "========================\n";

        using namespace BloombergLP;  // okay here

        const int UC = Holder::e_UNINITIALIZED_CATEGORY;
        const int DC = Holder::e_DYNAMIC_CATEGORY;

        // Warning: Do not change the order of the test cases.  This test case
        // relies on having all the test cases that require the logger manager
        // to be initialized to be after all the cases that don't require it.
        static const struct {
            int         d_line;            // line number
            bool        d_useLM;           // is logger manager initialized
            int         d_threshold;       // threshold
            bool        d_useCategory;     // use category
            const char *d_name_p;          // category name
            int         d_severity;        // severity
            bool        d_expResult;       // result
        } DATA[] = {
          // Line  lmInit Threshold  Category  name    severity   result
          // ----  ------ ---------  --------  ----    --------   ------
            { L_, false,       0,     false,    "",        1,       true   },
            { L_, false,       5,     true,     "AA",     32,       true   },
            { L_, false,       5,     false,    "AB",     64,       true   },
            { L_, false,       5,     false,    "AC",     96,       true   },
            { L_, false,       5,     false,    "AD",    128,       false  },
            { L_, false,      32,     true,     "ZA",     32,       true   },
            { L_, false,      64,     false,    "ZB",     64,       true   },
            { L_, false,     128,     false,    "ZC",     96,       true   },
            { L_, false,     128,     false,    "ZD",     97,       false  },
            { L_, false,     255,     false,    "ZE",    128,       false  },
            { L_, false,      UC,     true,     "AE",      1,       true   },
            { L_, false,      UC,     false,    "AF",      1,       true   },
            { L_, false,      UC,     true,     "AG",     32,       true   },
            { L_, false,      UC,     false,    "AH",     32,       true   },
            { L_, false,      UC,     true,     "BE",     64,       true   },
            { L_, false,      UC,     false,    "BF",     64,       true   },
            { L_, false,      UC,     true,     "CE",     95,       true   },
            { L_, false,      UC,     false,    "CF",     95,       true   },
            { L_, false,      UC,     true,     "DE",     96,       true   },
            { L_, false,      UC,     false,    "DF",     96,       true   },
            { L_, false,      UC,     true,     "EE",     97,       false  },
            { L_, false,      UC,     false,    "EF",     97,       false  },
            { L_, false,      DC,     false,    "GA",      1,       true   },
            { L_, false,      DC,     false,    "GB",     95,       true   },
            { L_, false,      DC,     false,    "GC",     96,       true   },
            { L_, false,      DC,     false,    "GD",     97,       false  },
            { L_, false,      DC,     true,     "GE",      1,       true   },
            { L_, false,      DC,     true,     "GF",     95,       true   },
            { L_, false,      DC,     true,     "GG",     96,       true   },
            { L_, false,      DC,     true,     "GH",     97,       false  },
            { L_, true,       DC,     false,    "GI",      1,       true   },
            { L_, true,       DC,     false,    "GJ",     95,       true   },
            { L_, true,       DC,     false,    "GK",     96,       true   },
            { L_, true,       DC,     false,    "GL",     97,       false  },
            { L_, true,       DC,     true,     "GM", DC - 3,       true  },
            { L_, true,       DC,     true,     "GN", DC - 2,       true  },
            { L_, true,       DC,     true,     "GO", DC - 1,       false  },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        CategoryManager CM;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const bool    USE_LM      = DATA[i].d_useLM;
            const int     THRESHOLD   = DATA[i].d_threshold;
            const bool    USE_CAT     = DATA[i].d_useCategory;
            const char   *CAT_NAME    = DATA[i].d_name_p;
            const int     SEVERITY    = DATA[i].d_severity;
            const bool    RESULT      = DATA[i].d_expResult;

            Holder mH = { { THRESHOLD }, { 0 }, { 0 } };
            if (USE_CAT) {
                const Cat *CATEGORY = CM.addCategory(CAT_NAME, THRESHOLD - 2,
                                                     1, 1, 1);
                ASSERTV(LINE, CAT_NAME, CATEGORY);
                mH.setCategory(CATEGORY);
            }
            static bool LM_INITIALIZED = false;
            if (USE_LM && !LM_INITIALIZED) {
                LM_INITIALIZED = true;
                ball::LoggerManagerConfiguration lmc;
                ball::LoggerManager::initSingleton(lmc, &ta);
            }
            bool result = ball::Log::isCategoryEnabled(&mH, SEVERITY);
            ASSERTV(LINE, RESULT == result);
        }

        ASSERT(ball::LoggerManager::isInitialized());

        // Test getRecord
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const bool    USE_CAT     = DATA[i].d_useCategory;
            const char   *FILENAME    = DATA[i].d_name_p;

            const Cat    *CATEGORY = USE_CAT ? (Cat *) 0 : (Cat *) 1;
            ball::Record *RECORD   = ball::Log::getRecord(CATEGORY,
                                                          FILENAME,
                                                          LINE);
            ASSERTV(LINE, LINE == RECORD->fixedFields().lineNumber());
            ASSERTV(LINE, 0 == bsl::strcmp(FILENAME,
                                           RECORD->fixedFields().fileName()));
        }

        ball::LoggerManager::shutDownSingleton();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ball::Log_Stream
        //
        // Concerns:
        //   The basic concern is that the creators, manipulators and
        //   accessors operate as expected.
        //
        // Plan:
        //   Specify a large set of data values that can be used to construct
        //   a ball::Log_Stream object.  Initialize an object X by calling the
        //   constructor using one of the specified data values.  Using the
        //   accessor functions verify that X was correctly initialized.  Then
        //   use the manipulator functions to put X into a different state.
        //   Again use the accessor functions to confirm the state of X.
        //
        // Testing:
        //   ball::Log_Stream
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Test ball::Log_Stream" << bsl::endl
                               << "=====================" << bsl::endl;

        using namespace BloombergLP;  // okay here

        static const struct {
            int         d_line;            // line number
            int         d_recordLevel;     // record level
            int         d_passLevel;       // pass level
            int         d_triggerLevel;    // trigger level
            int         d_triggerAllLevel; // trigger all level
            const char *d_name_p;          // category name
            const char *d_fileName_p;      // filename to use for logging
            int         d_severity;        // logging severity
        } DATA[] = {
 // line  record    pass     trigger triggerAll cat         file   severity
 // no.   level     level    level   level      name        name
 // ----  ------    ------   ------  -----      ------      ----   --------
  {  L_,  0,        0,        0,        0,       "",        "",           1 },
  {  L_,  1,        0,        0,        0,       "A1",      "a.c",        1 },
  {  L_,  0,        1,        0,        0,       "A2",      "h.f",        2 },
  {  L_,  0,        0,        1,        0,       "AA1",     "z.cpp",      3 },
  {  L_,  0,        0,        0,        1,       "AA2",     "hello.c",    4 },
  {  L_,  16,       32,       48,       64,      "AB",      __FILE__,     5 },
  {  L_,  64,       48,       32,       16,      "AC",      "ball",       6 },
  {  L_,  16,       32,       64,       48,      "AD",      "ballu",      7 },
  {  L_,  16,       48,       32,       64,      "AAA",     "aaa",        8 },
  {  L_,  32,       16,       48,       64,      "ABC",     "ABCD",       9 },
  {  L_,  255,      0,        0,        0,       "ABCD1",   "w.c",        10 },
  {  L_,  0,        255,      0,        0,       "ABCD2",   "a.h",        11 },
  {  L_,  0,        0,        255,      0,       "DEFG",    "b.c",        12 },
  {  L_,  0,        0,        0,        255,     "HIJK",    "c.d",        13 },
  {  L_,  255,      255,      255,      255,     "ALL",     "e.f",        14 },
  {  L_,  254,      0,        0,        0,       "ALL.FS",  __FILE__,     15 },
  {  L_,  0,        254,      0,        0,       "ALL.TCP", __FILE__,     16 },
  {  L_,  0,        0,        254,      0,       "ALL.1.2", __FILE__,     17 },
  {  L_,  0,        0,        0,        254,     "BALL1",   __FILE__,     18 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        TestAllocator ta(veryVeryVeryVerbose);
        TestAllocator da(veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        CategoryManager CM(&ta);
        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(lmc);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const int     RECORD      = DATA[i].d_recordLevel;
            const int     PASS        = DATA[i].d_passLevel;
            const int     TRIGGER     = DATA[i].d_triggerLevel;
            const int     TRIGGER_ALL = DATA[i].d_triggerAllLevel;
            const char   *CAT_NAME    = DATA[i].d_name_p;
            const char   *FILENAME    = DATA[i].d_fileName_p;
            const int     SEVERITY    = DATA[i].d_severity;

            const Cat *CATEGORY = CM.addCategory(CAT_NAME,
                                                 RECORD,
                                                 PASS,
                                                 TRIGGER,
                                                 TRIGGER_ALL);

            ASSERT(CATEGORY);

            LogStream mL(CATEGORY, FILENAME, LINE, SEVERITY);
            const LogStream& L = mL;

            ASSERTV(CATEGORY, L.category(), CATEGORY == L.category());
            ASSERTV(SEVERITY, L.severity(), SEVERITY == L.severity());
            ASSERTV(LINE, LINE == L.record()->fixedFields().lineNumber());
            ASSERTV(LINE, 0 ==
                            bsl::strcmp(FILENAME,
                                        L.record()->fixedFields().fileName()));
            ASSERTV(LINE, LINE == mL.record()->fixedFields().lineNumber());
            ASSERTV(LINE, 0 == bsl::strcmp(FILENAME,
                                       mL.record()->fixedFields().fileName()));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_SET_DYNAMIC_CATEGORY
        //
        // Concerns:
        //   This macro must be safe in absence of a logger manager.  This
        //   macro must be correct when 'ball::LoggerManager::setCategory' is
        //   called to change the threshold levels of the dynamic category
        //   before logging macros are encountered.
        //
        // Plan:
        //   Intentionally invoke the macro without first initializing the
        //   logger manager.  Verify that the number of records going to
        //   'bsl::cout' is expected.  Then invoke the macro in presence of a
        //   logger manager and call 'ball::LoggerManager::setCategory' to
        //   change the underlying category.  Verify that the number of
        //   published records is expected.
        //
        // Testing:
        //   BALL_LOG_SET_DYNAMIC_CATEGORY
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "Testing BALL_LOG_SET_DYNAMIC_CATEGORY\n"
                      << "=====================================\n";

        using namespace BALL_LOG_TEST_CASE_18;
        namespace Blp = BloombergLP;

        for (int i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (int i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {

#ifdef BSLS_PLATFORM_OS_UNIX
            u::TempDirectoryGuard tempDirGuard;

            bsl::string filename(tempDirGuard.getTempDirName());
            Blp::bdls::PathUtil::appendRaw(&filename, "stderrOut");

            fflush(stderr);
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            u::CerrBufferGuard cerrBufferGuard;

            BALL_LOG_SET_DYNAMIC_CATEGORY("ThereIsNoLoggerManager");

            BALL_LOG_TRACE << "No Logger Manager1!";
            BALL_LOG_DEBUG << "No Logger Manager2!";
            BALL_LOG_INFO  << "No Logger Manager3!";
            BALL_LOG_WARN  << "No Logger Manager4!";
            BALL_LOG_ERROR << "No Logger Manager5!";
            BALL_LOG_FATAL << "No Logger Manager6!";

#ifdef BSLS_PLATFORM_OS_UNIX
            fflush(stderr);
            dup2(saved_stderr_fd, 2);

            bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
            int numLines = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++numLines;
                if (veryVerbose)
                    bsl::cout << "\t>>" << line << "<<" << bsl::endl;
            }
            fs.close();

            // Only FATAL, ERROR, and WARN messages are published in this test.
            ASSERTV(numLines, 3 == numLines);
#endif
        }

        if (verbose)
            bsl::cout << "\tTesting macro correctness with a logger manager."
                      << bsl::endl;
        {
            BloombergLP::ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc);
            BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

            bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

            ASSERT(0 == manager.registerObserver(observer, "test"));

            // Re "sieve" category: (1) if recorded, then also published;
            // (2) never triggered.

            BloombergLP::ball::Administration::addCategory(
                                         "sieve",
                                         BloombergLP::ball::Severity::e_INFO,
                                         BloombergLP::ball::Severity::e_INFO,
                                         0,
                                         0);
            BALL_LOG_SET_DYNAMIC_CATEGORY("sieve")

            int numPublishedRecords = observer->numPublishedRecords();

            BALL_LOG_TRACE << "message11";
            BALL_LOG_DEBUG << "message12";
            BALL_LOG_INFO  << "message13";
            BALL_LOG_WARN  << "message14";
            BALL_LOG_ERROR << "message15";
            BALL_LOG_FATAL << "message16";

            ASSERT(numPublishedRecords + 4 == observer->numPublishedRecords());

            numPublishedRecords = observer->numPublishedRecords();

            manager.setCategory("sieve",
                                0,
                                BloombergLP::ball::Severity::e_WARN,
                                0,
                                0);

            BALL_LOG_TRACE << "message31";
            BALL_LOG_DEBUG << "message32";
            BALL_LOG_INFO  << "message33";
            BALL_LOG_WARN  << "message34";
            BALL_LOG_ERROR << "message35";
            BALL_LOG_FATAL << "message36";

            ASSERT(numPublishedRecords + 3 == observer->numPublishedRecords());

            numPublishedRecords = observer->numPublishedRecords();

            u::executeInParallel(NUM_THREADS, workerThread18);

            ASSERT(numPublishedRecords + NUM_MSGS / 2 * NUM_THREADS
                                           == observer->numPublishedRecords());
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {

#ifdef BSLS_PLATFORM_OS_UNIX
            u::TempDirectoryGuard tempDirGuard;

            bsl::string filename(tempDirGuard.getTempDirName());
            Blp::bdls::PathUtil::appendRaw(&filename, "stderrOut");

            fflush(stderr);
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            u::CerrBufferGuard cerrBufferGuard;

            BALL_LOG_SET_DYNAMIC_CATEGORY("ThereIsNoLoggerManager");

            BALL_LOG_TRACE << "No Logger Manager1!";
            BALL_LOG_DEBUG << "No Logger Manager2!";
            BALL_LOG_INFO  << "No Logger Manager3!";
            BALL_LOG_WARN  << "No Logger Manager4!";
            BALL_LOG_ERROR << "No Logger Manager5!";
            BALL_LOG_FATAL << "No Logger Manager6!";

#ifdef BSLS_PLATFORM_OS_UNIX
            fflush(stderr);
            dup2(saved_stderr_fd, 2);

            bsl::ifstream fs(filename.c_str(), bsl::ifstream::in);
            int numLines = 0;
            bsl::string line;
            while (getline(fs, line)) {
                ++numLines;
                if (veryVerbose)
                    bsl::cout << "\t>>" << line << "<<" << bsl::endl;
            }
            fs.close();

            // Only FATAL, ERROR, and WARN messages are published in this test.
            ASSERT(3 == numLines);
#endif
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING CALLBACK MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
        //
        // Concerns:
        //: 1 The callback-based logging macros must be safe whether invoked
        //:   before the logger manager singleton is initialized, while it
        //:   exists, or after it has been destroyed.
        //
        // Plan:
        //: 1 Within a loop, invoke the logging macros (1) before the singleton
        //:   has been (re)initialized, (2) while it exists, and (3) after it
        //:   has been destroyed, each time verifying that the expected number
        //:   of messages are either published to the test observer or written
        //:   by the 'bsls::Log' message handler.  (C-1)
        //
        // Testing:
        //   BALL_LOGCB_TRACE
        //   BALL_LOGCB_DEBUG
        //   BALL_LOGCB_INFO
        //   BALL_LOGCB_WARN
        //   BALL_LOGCB_ERROR
        //   BALL_LOGCB_FATAL
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
          << "TESTING CALLBACK MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER"
                      << bsl::endl
          << "================================================================"
                      << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_17;

        BloombergLP::ball::TestObserver  testObserver(&bsl::cout);
        BloombergLP::ball::TestObserver *TO = &testObserver;

        bsl::shared_ptr<BloombergLP::ball::Observer> observerWrapper(
                                    TO,
                                    BloombergLP::bslstl::SharedPtrNilDeleter(),
                                    &ta);

        ASSERT(0 == TO->numPublishedRecords());

        for (int i = 0; i < 5; ++i) {
            macrosTest(false, *TO, TO->numPublishedRecords());

            {
                 BloombergLP::ball::LoggerManagerConfiguration lmc;
                 ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(0,
                                                                  TRACE,
                                                                  0,
                                                                  0));

                 BloombergLP::ball::LoggerManagerScopedGuard guard(lmc);

                 LoggerManager::singleton().registerObserver(observerWrapper,
                                                             "default");

                 macrosTest(true, *TO, TO->numPublishedRecords());
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING OSTREAM MACROS WITH CALLBACK
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   OSTREAM MACROS WITH CALLBACK
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "TESTING OSTREAM MACROS WITH CALLBACK"
                               << bsl::endl
                               << "===================================="
                               << bsl::endl;

        bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                               &u::incCallback;
        u::numIncCallback = 0;

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);
        BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

        bsl::shared_ptr<BloombergLP::ball::TestObserver> observer =
                 bsl::make_shared<BloombergLP::ball::TestObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                          "sieve",
                                          BloombergLP::ball::Severity::e_TRACE,
                                          BloombergLP::ball::Severity::e_TRACE,
                                          0,
                                          0);
        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const Cat  *CAT   = BALL_LOG_CATEGORY;
        const char *FILE  = __FILE__;

        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory(
                                  "noTRACE",
                                  BloombergLP::ball::Severity::e_TRACE - 1,
                                  BloombergLP::ball::Severity::e_TRACE - 1,
                                  BloombergLP::ball::Severity::e_TRACE - 1,
                                  BloombergLP::ball::Severity::e_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_TRACE'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_TRACE(callback) << "message";

                ASSERT(0 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_TRACE(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(1 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noDEBUG",
                                   BloombergLP::ball::Severity::e_DEBUG - 1,
                                   BloombergLP::ball::Severity::e_DEBUG - 1,
                                   BloombergLP::ball::Severity::e_DEBUG - 1,
                                   BloombergLP::ball::Severity::e_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_DEBUG'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_DEBUG(callback) << "message";

                ASSERT(1 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
           }

            const int LINE = L_ + 1;
            BALL_LOGCB_DEBUG(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(2 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                    "noINFO",
                                    BloombergLP::ball::Severity::e_INFO - 1,
                                    BloombergLP::ball::Severity::e_INFO - 1,
                                    BloombergLP::ball::Severity::e_INFO - 1,
                                    BloombergLP::ball::Severity::e_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_INFO'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_INFO(callback) << "message";

                ASSERT(2 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_INFO(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2];
            ASSERT(3 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noWARN",
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_WARN'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_WARN(callback) << "message";

                ASSERT(3 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_WARN(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2];
            ASSERT(4 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noERROR",
                                   BloombergLP::ball::Severity::e_ERROR - 1,
                                   BloombergLP::ball::Severity::e_ERROR - 1,
                                   BloombergLP::ball::Severity::e_ERROR - 1,
                                   BloombergLP::ball::Severity::e_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_ERROR'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_ERROR(callback) << "message";

                ASSERT(4 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_ERROR(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(5 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGCB_FATAL'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOGCB_FATAL(callback) << "message";

                ASSERT(5 == u::numIncCallback);
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_FATAL(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(6 == u::numIncCallback);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = u::messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longStr    = new char[N];
            bsl::fill(longStr + 0, longStr + N, 'x');
            longStr[N-1] = '\0';
            char *cpyString  = new char[N];
            bsl::strcpy(cpyString, longStr);

            {
                const int LINE = L_ + 1;
                BALL_LOGCB_TRACE(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_DEBUG(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_INFO(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_WARN(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_ERROR(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_FATAL(callback) << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            delete [] longStr;
            delete [] cpyString;
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // STRESS TEST:
        //
        // Concerns:
        //   That logger is in good state after logging large number of
        //   messages.
        //
        // Plan:
        //   Initialize the logger manager with an observer that keeps a
        //   count of number of messages published to it.  Log a large
        //   number of messages with record level, log 1 message at
        //   trigger level and ensure that at least 1 message of record
        //   level is published.
        //
        // Testing:
        //   STRESS TEST
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl << "STRESS TEST"
                      << bsl::endl << "===========" << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_15;
        using namespace BloombergLP;

        ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                             ball::Severity::e_TRACE,  // record level
                             ball::Severity::e_WARN,   // passthrough level
                             ball::Severity::e_ERROR,  // trigger level
                             ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(configuration);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<my_PublishCountingObserver> observer =
                                bsl::make_shared<my_PublishCountingObserver>();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        enum { NUM_MESSAGES = 100000 };
        BALL_LOG_SET_CATEGORY("TEST.CATEGORY");

        for (int i = 0; i < NUM_MESSAGES; ++i) {
            BALL_LOG_TRACE << "DUMMY MESSAGE";
        }

        BALL_LOG_ERROR << "DUMMY MESSAGE";

        if (verbose) P(observer->publishCount());

        ASSERT(observer->publishCount() > 1); // because the triggering message
                                              // is always published
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // BALL_IS_ENABLED(SEVERITY) UTILITY MACRO TEST:
        //
        // Concerns:
        //   We are concerned that the macro can distinguish between severity
        //   levels that are and are not "enabled" for the category that is
        //   in effect in a given scope.
        //
        // Plan:
        //   Initialize the logger manager with a configuration such that
        //   'TRACE' is enabled (for "record", for definiteness) and verify
        //   that all of the six named severities are enabled.  Then
        //   reconfigure the threshold levels for the category in effect (using
        //   the 'setCategory' method) such that 'TRACE', 'DEBUG', and 'INFO'
        //   are not enabled, and confirm that only more severe levels are
        //   enabled.  Finally, reconfigure the threshold levels such that
        //   they are all 'OFF', and confirm that none of the severities is
        //   enabled.
        //
        // Testing:
        //   BALL_IS_ENABLED(SEVERITY)
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nBALL_IS_ENABLED(SEVERITY) Utility MACRO TEST"
                      << "\n============================================"
                      << bsl::endl;
        }

        using namespace BloombergLP;

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard lmg(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        {
            BALL_LOG_SET_CATEGORY("TEST.CATEGORY");

            BALL_LOG_TRACE << "This will load the category";

            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                ball::Severity::e_WARN,
                                ball::Severity::e_ERROR,
                                ball::Severity::e_FATAL,
                                ball::Severity::e_FATAL);

            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));
            ASSERT(BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                ball::Severity::e_OFF,
                                ball::Severity::e_OFF,
                                ball::Severity::e_OFF,
                                ball::Severity::e_OFF);

            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));
            ASSERT(!BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // PRINTF MACRO PERFORMANCE TEST WITH 1 THREAD:
        //
        // Concerns:
        //   That when a single thread is logging messages using printf
        //   macro, the logging should be efficient when messages are
        //   not published to the observer.
        //
        // Plan:
        //   In a loop, log random size messages using printf macro.
        //   Measure the time to log all the messages.
        //
        // Testing:
        //   performance of printf macro with one thread
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nPRINTF MACRO PERF TEST WITH 1 THREADS"
                      << "\n====================================="
                      << bsl::endl;
        }

        using namespace BALL_LOG_TEST_CASE_13;
        using namespace BloombergLP;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc);

        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        bsls::Types::Int64 t = bsls::TimeUtil::getTimer();

        u::executeInParallel(NUM_THREADS, workerThread13);

        t = bsls::TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with printf style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS:
        //
        // Concerns:
        //   That when multiple threads are logging messages (to the same
        //   logger) using printf macro, the logging should be efficient when
        //   messages are not published to the observer.
        //
        // Plan:
        //   create several threads, each, in a tight loop, logs random
        //   size messages using printf macro.  Finally join all the
        //   messages.  Measure the time to log all the messages.
        //
        // Testing:
        //   performance of printf macro with one thread
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nPRINTF MACRO PERF TEST WITH MULTIPLE THREADS"
                      << "\n============================================"
                      << bsl::endl;
        }

        using namespace BALL_LOG_TEST_CASE_12;
        using namespace BloombergLP;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc);

        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        bsls::Types::Int64 t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread12);
        t = bsls::TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is chosen randomly from range 0 to "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with printf style macro = "
                 << t << " nanoseconds" << bsl::endl;
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // C++ MACRO PERFORMANCE TEST WITH 1 THREAD:
        //
        // Concerns:
        //   That when a single thread is logging messages using c++
        //   macro, the logging should be efficient when messages are
        //   not published to the observer.
        //
        // Plan:
        //   In a loop, log random size messages using c++ macro.
        //   Measure the time to log all the messages.
        //
        // Testing:
        //   performance of c++ macro with one thread
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nC++ MACRO PERFORMANCE TEST WITH 1 THREADS"
                      << "\n========================================="
                      << bsl::endl;
        }

        if (!verbose) break;

        using namespace BALL_LOG_TEST_CASE_11;
        using namespace BloombergLP;

        for (int i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        severity = veryVerbose ? ball::Severity::e_INFO
                               : ball::Severity::e_DEBUG;

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc);

        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        bsls::Types::Int64 t = bsls::TimeUtil::getTimer();

        u::executeInParallel(NUM_THREADS, workerThread11a);
        t = bsls::TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with c++ style var macro = "
                 << t/(NUM_MSGS*NUM_THREADS) << bsl::endl;
        }

        t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread11);
        t = bsls::TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with c++ style const macro = "
                 << t/(NUM_MSGS*NUM_THREADS) << bsl::endl;
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS:
        //
        // Concerns:
        //   That when multiple threads are logging messages (to the same
        //   logger) using c++ macro, the logging should be efficient when
        //   messages are not published to the observer.
        //
        // Plan:
        //   create several threads, each, in a tight loop, logs random
        //   size messages using c++ macro.  Finally join all the
        //   messages.  Measure the time to log all the messages.
        //
        // Testing:
        //   performance of c++ macro with one thread
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                      << "C++ MACRO PERFORMANCE TEST WITH MULTIPLE THREADS"
                      << bsl::endl
                      << "================================================"
                      << bsl::endl;

        if (!verbose) break;

        using namespace BALL_LOG_TEST_CASE_10;
        using namespace BloombergLP;

        for (int i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        severity = veryVerbose ? ball::Severity::e_INFO
                               : ball::Severity::e_TRACE;
        if (verbose) P(severity);

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_DEBUG,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc);

        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        bsls::Types::Int64 t;

        bslim::TestUtil::setFunc(ball::Severity::e_TRACE == severity
                                 ? &Util::doOldTraceConst
                                 : &Util::doOldInfoConst);

        t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread10);
        t = bsls::TimeUtil::getTimer() - t;

        const double oldConstTime = static_cast<double>(t);

        bslim::TestUtil::setFunc(&Util::doOldVar);
        t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread10);
        t = bsls::TimeUtil::getTimer() - t;

        const double oldVarTime = static_cast<double>(t);

        bslim::TestUtil::setFunc(ball::Severity::e_TRACE == severity
                                 ? &Util::doTraceConst
                                 : &Util::doInfoConst);

        t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread10);
        t = bsls::TimeUtil::getTimer() - t;

        const double newConstTime = static_cast<double>(t);

        bslim::TestUtil::setFunc(&Util::doVar);

        t = bsls::TimeUtil::getTimer();
        u::executeInParallel(NUM_THREADS, workerThread10);
        t = bsls::TimeUtil::getTimer() - t;

        const double newVarTime = static_cast<double>(t);

        const double constVarSpeedup = 100 * (newVarTime - newConstTime) /
                                                                    newVarTime;
        const double oldNewConstSpeedup = 100 * (oldConstTime - newConstTime) /
                                                                  oldConstTime;

        const double oldNewVarSpeedup   = 100 * (oldVarTime - newVarTime) /
                                                                    oldVarTime;

        if (verbose) {
            cout << "\nNew Const: " << newConstTime / numMsgs()
                 << "\nOld Const: " << oldConstTime / numMsgs()
                 << "\nNew Var  : " << newVarTime   / numMsgs()
                 << "\nOld Var  : " << oldVarTime   / numMsgs() << endl;
        }

        if (verbose) {
            cout << "Const speedup over var: ";
            cout << constVarSpeedup << (0.0 < constVarSpeedup
                                       ? "% (speedup)\n"
                                       : "% (slowdown)\n");
        }

        if (verbose) {
            cout << "New (const) speedup over old: ";
            cout << oldNewConstSpeedup << (0.0 < oldNewConstSpeedup
                                          ? "% (speedup)\n"
                                          : "% (slowdown)\n");
        }

        if (verbose) {
            cout << "New (var) speedup over old: ";
            cout << oldNewVarSpeedup << (0.0 < oldNewVarSpeedup
                                          ? "% (speedup)\n"
                                          : "% (slowdown)\n");
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCURRENT LOGGING TEST
        //   Verify concurrent logging.
        //
        // Concerns:
        //   That multiple threads can concurrently log messages (with
        //   varying severities) to the same logger without introducing
        //   race conditions.
        //
        // Plan:
        //   Create several threads, each, in a tight loop, logs messages
        //   with varying severities.  Finally join all the threads and
        //   verify the state.
        //
        // Testing:
        //   Testing concurrent logging.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nCONCURRENT LOGGING TEST"
                      << "\n=======================" << bsl::endl;
        }

        using namespace BALL_LOG_TEST_CASE_9;
        using namespace BloombergLP;

        ASSERT(EXP_N_TOTAL == EXP_N_RECORD + EXP_N_PUBLISH + EXP_N_TRIGGER);

        ball::LoggerManagerConfiguration lmc;
        lmc.setLogOrder(ball::LoggerManagerConfiguration::e_FIFO);
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level
        lmc.setDefaultRecordBufferSizeIfValid(REC_BUF_LIMIT);
        lmc.setTriggerMarkers(ball::LoggerManagerConfiguration::e_NO_MARKERS);

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        ball::LoggerManagerScopedGuard guard(lmc, &ta);

        bsl::shared_ptr<my_Observer> observer =
                                               bsl::make_shared<my_Observer>();

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        u::executeInParallel(NUM_THREADS, workerThread9);
        if (veryVeryVerbose) observer->print();

        bsl::vector<bsl::pair<int, int> > vv[NUM_THREADS];
        // 'vv[i]' contains the message information for i'th thread

        const bsl::vector<bsl::string>& v = observer->get();
        ASSERT(v.size() == NUM_THREADS * EXP_N_TOTAL);
        // ignore any unconditional printing
        for (int i = 0; i < (int)v.size(); ++i) {
            bsl::istringstream is(v[i]);
            int id, type, iter;
            is >> id; is >> type; is >> iter;
            vv[id].push_back(bsl::pair<int, int> (type, iter));
        }

        for (int i = 0; i < NUM_THREADS; ++i) {
            bsl::vector<bsl::pair<int, int> >& v = vv[i];
            ASSERT(v.size() == EXP_N_TOTAL);
            int n_record = 0, n_publish = 0, n_trigger = 0;
            for (int j = 0; j < (int)v.size(); ++j) {
                if      (v[j].first == RECORD)  ++n_record;
                else if (v[j].first == PUBLISH) ++n_publish;
                else if (v[j].first == TRIGGER) ++n_trigger;
                else    ASSERT(0);
            }
            ASSERT(n_record  == EXP_N_RECORD);
            ASSERT(n_publish == EXP_N_PUBLISH);
            ASSERT(n_trigger == EXP_N_TRIGGER);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING THE FIFO LOG ORDER:
        //   Verify the FIFO log order.
        //
        // Concerns:
        //   That when the logger manager is configured with the FIFO
        //   (first in first out) log order, it publishes the logged
        //   messages in FIFO order.
        //
        // Plan:
        //   Create a logger manager configured with the default log
        //   order, log several messages to it and verify that they get
        //   published in the desired order.
        //
        // Testing:
        //   the FIFO log order
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING THE FIFO LOG ORDER"
                      << "\n==========================" << bsl::endl;
        }

        using namespace BloombergLP;

        ball::LoggerManagerConfiguration lmc;

        // for simplicity we keep the passthrough level to be 'FATAL', so that
        // on trigger event, the message is published only once.

        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_FATAL,  // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc, &ta);

        bsl::ostringstream os;
        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&os);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        BALL_LOG_SET_CATEGORY("main category");
        char helloWorld1[] = "hello world 1";
        char helloWorld2[] = "hello world 2";
        char helloWorld3[] = "hello world 3";
        char helloWorld4[] = "hello world 4";

        BALL_LOG_INFO <<  helloWorld1;
        BALL_LOG_INFO <<  helloWorld2;
        BALL_LOG_INFO <<  helloWorld3;
        BALL_LOG_ERROR << helloWorld4;

        bsl::string s = os.str();
        const char *ptr4 = bsl::strstr(s.c_str(), helloWorld4);
        ASSERT(ptr4 != NULL);
        const char *ptr3 = bsl::strstr(ptr4+1,    helloWorld3);
        ASSERT(ptr3 != NULL);
        const char *ptr2 = bsl::strstr(ptr3+1,    helloWorld2);
        ASSERT(ptr2 != NULL);
        const char *ptr1 = bsl::strstr(ptr2+1,    helloWorld1);
        ASSERT(ptr1 != NULL);

        ASSERT(ptr4  < ptr3);
        ASSERT(ptr3  < ptr2);
        ASSERT(ptr2  < ptr1);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING THE DEFAULT LOG ORDER (LIFO):
        //   Verify the default log order.
        //
        // Concerns:
        //: 1 That when the logger manager is configured with the default
        //:   log order (LIFO), it publishes the logged messages in LIFO
        //:   (last in first out) order.
        //:
        //: 2 'BALL_LOG_STREAM' also works where the severity is determined at
        //:   run time.
        //
        // Plan:
        //: 1 Create a logger manager configured with the default log order,
        //:   log several messages to it and verify that they get published in
        //:   the desired order.
        //
        // Testing:
        //   the default log order
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING THE DEFAULT LOG ORDER (LIFO)"
                      << "\n====================================" << bsl::endl;
        }

        using namespace BloombergLP;

        char helloWorld1[] = "hello world 1";
        char helloWorld2[] = "hello world 2";
        char helloWorld3[] = "hello world 3";
        char helloWorld4[] = "hello world 4";

        static const struct Data {
            ball::Severity::Level  d_severity;
            const char            *d_string_p;
        } DATA[] = {
            { ball::Severity::e_INFO,  helloWorld1 },
            { ball::Severity::e_INFO,  helloWorld2 },
            { ball::Severity::e_INFO,  helloWorld3 },
            { ball::Severity::e_ERROR, helloWorld4 }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "Testing with fixed-severity macros\n";
        {
            ball::LoggerManagerConfiguration lmc;

            lmc.setLogOrder(ball::LoggerManagerConfiguration::e_FIFO);

            // for simplicity we keep the passthrough level to be 'FATAL', so
            // that on trigger event, the message is published only once.

            lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_FATAL,  // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(lmc, &ta);

            bsl::ostringstream os;
            bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&os);

            ball::LoggerManager::singleton().registerObserver(observer,
                                                              "test");

            BALL_LOG_SET_CATEGORY("main category");
            BALL_LOG_INFO  << helloWorld1;
            BALL_LOG_INFO  << helloWorld2;
            BALL_LOG_INFO  << helloWorld3;
            BALL_LOG_ERROR << helloWorld4;

            bsl::string s = os.str();
            const char *ptr1 = bsl::strstr(s.c_str(), helloWorld1);
            ASSERT(ptr1 != NULL);
            const char *ptr2 = bsl::strstr(ptr1+1,    helloWorld2);
            ASSERT(ptr2 != NULL);
            const char *ptr3 = bsl::strstr(ptr2+1,    helloWorld3);
            ASSERT(ptr3 != NULL);
            const char *ptr4 = bsl::strstr(ptr3+1,    helloWorld4);
            ASSERT(ptr4 != NULL);

            ASSERT(ptr1 < ptr2);
            ASSERT(ptr2 < ptr3);
            ASSERT(ptr3 < ptr4);
        }

        if (verbose) cout << "Testing with variable-severity macros\n";
        {
            ball::LoggerManagerConfiguration lmc;
            lmc.setLogOrder(ball::LoggerManagerConfiguration::e_FIFO);

            // for simplicity we keep the passthrough level to be 'FATAL', so
            // that on trigger event, the message is published only once.

            lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_FATAL,  // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

            ball::LoggerManagerScopedGuard guard(lmc, &ta);

            bsl::ostringstream os;
            bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&os);

            ball::LoggerManager::singleton().registerObserver(observer,
                                                              "test");

            BALL_LOG_SET_CATEGORY("main category");
            for (int ii = 0; ii < k_NUM_DATA; ++ii) {
                BALL_LOG_STREAM(DATA[ii].d_severity) << DATA[ii].d_string_p;
            }

            bsl::string s = os.str();
            const char *ptr1 = bsl::strstr(s.c_str(), helloWorld1);
            ASSERT(ptr1 != NULL);
            const char *ptr2 = bsl::strstr(ptr1+1,    helloWorld2);
            ASSERT(ptr2 != NULL);
            const char *ptr3 = bsl::strstr(ptr2+1,    helloWorld3);
            ASSERT(ptr3 != NULL);
            const char *ptr4 = bsl::strstr(ptr3+1,    helloWorld4);
            ASSERT(ptr4 != NULL);

            ASSERT(ptr1 < ptr2);
            ASSERT(ptr2 < ptr3);
            ASSERT(ptr3 < ptr4);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING THE C++ MACRO WHEN LOGGING RETURNED VALUE OF A FUNCTION:
        //   Verify the c++ macro when logging returned value of a function
        //   that internally log some other message using c++ macro.
        //
        // Concerns:
        //   Suppose a function 'f()' logs some data (say "message1")
        //   using c++ macro, and return some data (say "message2"), then
        //   'BALL_LOG_SEVERITY << f()' should result in
        //   logging "message1" followed by "message2".
        //
        // Plan:
        //   Invoke the function 'f()' described in Concern: section, and
        //   verify the order of logged messages.
        //
        // Testing:
        //   the c++ macro when logging returned value of a function.
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTESTING THE C++ MACRO WHEN LOGGING "
                      << "\nRETURNED VALUE OF A FUNCTION"
                      << "\n====================================" << bsl::endl;
        }

        using namespace BALL_LOG_TEST_CASE_6;
        using namespace BloombergLP;

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_TRACE,  // record level
                                 ball::Severity::e_WARN,   // passthrough level
                                 ball::Severity::e_ERROR,  // trigger level
                                 ball::Severity::e_FATAL); // triggerAll level

        ball::LoggerManagerScopedGuard guard(lmc, &ta);

        bsl::ostringstream os;
        bsl::shared_ptr<ball::StreamObserver> observer =
                                   bsl::make_shared<ball::StreamObserver>(&os);

        ball::LoggerManager::singleton().registerObserver(observer, "test");

        BALL_LOG_SET_CATEGORY("main category");

        BALL_LOG_WARN << f();
        bsl::string s = os.str();
        if (verbose)  bsl::cout << s << bsl::endl;
        const char *msg1 = bsl::strstr(s.c_str(), message1);
        ASSERT(msg1 != NULL);
        const char *msg2 = bsl::strstr(msg1+1,    message2);
        ASSERT(msg2 != NULL);

        ASSERT(msg1 < msg2);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER
        //
        // Concerns:
        //: 1 The non-callback-based logging macros must be safe whether
        //:   invoked before the logger manager singleton is initialized, while
        //:   it exists, or after it has been destroyed.
        //
        // Plan:
        //: 1 Within a loop, invoke the logging macros (1) before the singleton
        //:   has been (re)initialized, (2) while it exists, and (3) after it
        //:   has been destroyed, each time verifying that the expected number
        //:   of messages are either published to the test observer or written
        //:   by the 'bsls::Log' message handler.  (C-1)
        //
        // Testing:
        //   BALL_LOG_TRACE
        //   BALL_LOG_DEBUG
        //   BALL_LOG_INFO
        //   BALL_LOG_WARN
        //   BALL_LOG_ERROR
        //   BALL_LOG_FATAL
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << bsl::endl
                  << "TESTING MACRO SAFETY IN THE ABSENCE OF A LOGGER MANAGER"
                      << bsl::endl
                  << "======================================================="
                      << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_5;

        BloombergLP::ball::TestObserver  testObserver(&bsl::cout);
        BloombergLP::ball::TestObserver *TO = &testObserver;

        bsl::shared_ptr<BloombergLP::ball::Observer> observerWrapper(
                                    TO,
                                    BloombergLP::bslstl::SharedPtrNilDeleter(),
                                    &ta);

        ASSERT(0 == TO->numPublishedRecords());

        for (int i = 0; i < 5; ++i) {
            macrosTest(false, *TO, TO->numPublishedRecords());

            {
                 BloombergLP::ball::LoggerManagerConfiguration lmc;
                 ASSERT(0 == lmc.setDefaultThresholdLevelsIfValid(0,
                                                                  TRACE,
                                                                  0,
                                                                  0));

                 BloombergLP::ball::LoggerManagerScopedGuard guard(lmc);

                 LoggerManager::singleton().registerObserver(observerWrapper,
                                                             "default");

                 macrosTest(true, *TO, TO->numPublishedRecords());
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OSTREAM MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   OSTREAM MACROS (WITHOUT CALLBACK)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "TESTING OSTREAM MACROS" << bsl::endl
                               << "======================" << bsl::endl;

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        bsl::shared_ptr<BloombergLP::ball::TestObserver> observer(
               new (ta) BloombergLP::ball::TestObserver(&bsl::cout, &ta), &ta);

        BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "sieve",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);
        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const Cat  *CAT  = BALL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory(
                           "noTRACE",
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_TRACE'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_TRACE << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_TRACE << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MESSAGE));

        }

        BloombergLP::ball::Administration::addCategory(
                              "noDEBUG",
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_DEBUG'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_DEBUG << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_DEBUG << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MESSAGE));

        }

        BloombergLP::ball::Administration::addCategory(
                               "noINFO",
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_INFO'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_INFO << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_INFO << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                  "noWARN",
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_WARN'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_WARN << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_WARN << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                "noERROR",
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_ERROR'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_ERROR << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_ERROR << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOG_FATAL'" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                int numPublishedRecords = observer->numPublishedRecords();

                BALL_LOG_FATAL << "message";
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_FATAL << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = u::messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longStr    = new char[N];
            bsl::fill(longStr + 0, longStr + N, 'x');
            longStr[N-1] = '\0';
            char *cpyString  = new char[N];
            bsl::strcpy(cpyString, longStr);

            {
                const int LINE = L_ + 1;
                BALL_LOG_TRACE << longStr;

                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_DEBUG << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_INFO << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_WARN << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_ERROR << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_FATAL << longStr;
                ASSERT(0 == bsl::strcmp(cpyString, longStr));
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            delete [] longStr;
            delete [] cpyString;
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRINTF-STYLE MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BALL_LOGVA
        //   BALL_LOGVA_TRACE
        //   BALL_LOGVA_DEBUG
        //   BALL_LOGVA_INFO
        //   BALL_LOGVA_WARN
        //   BALL_LOGVA_ERROR
        //   BALL_LOGVA_FATAL
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Testing 'printf-style' Macros" << bsl::endl
                               << "=============================" << bsl::endl;

        BloombergLP::bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        BloombergLP::bslma::DefaultAllocatorGuard taGuard(&testAllocator);

        const int MAX_ARGS = 9;

        // Note: some compilers warn when the following aren't string literals:
        //   warning: format string is not a string literal (potentially
        //    insecure) [-Wformat-security]

        #define FORMAT_SPEC_0_ARGS "message"
        #define FORMAT_SPEC_1_ARGS "message:%d"
        #define FORMAT_SPEC_2_ARGS "message:%d:%d"
        #define FORMAT_SPEC_3_ARGS "message:%d:%d:%d"
        #define FORMAT_SPEC_4_ARGS "message:%d:%d:%d:%d"
        #define FORMAT_SPEC_5_ARGS "message:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_6_ARGS "message:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_7_ARGS "message:%d:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_8_ARGS "message:%d:%d:%d:%d:%d:%d:%d:%d"
        #define FORMAT_SPEC_9_ARGS "message:%d:%d:%d:%d:%d:%d:%d:%d:%d"

        const char *MSG[] = {
            "message",
            "message:1",
            "message:1:2",
            "message:1:2:3",
            "message:1:2:3:4",
            "message:1:2:3:4:5",
            "message:1:2:3:4:5:6",
            "message:1:2:3:4:5:6:7",
            "message:1:2:3:4:5:6:7:8",
            "message:1:2:3:4:5:6:7:8:9"
        };
        ASSERT(MAX_ARGS + 1 == sizeof MSG / sizeof *MSG);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        bsl::shared_ptr<BloombergLP::ball::TestObserver> observer(
               new (ta) BloombergLP::ball::TestObserver(&bsl::cout, &ta), &ta);

        BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory("pass", 0, TRACE, 0, 0);

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory("sieve",
                                                       TRACE,
                                                       TRACE,
                                                       0,
                                                       0);

        BloombergLP::ball::Administration::addCategory(
                                   "noTRACE",
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1,
                                   BloombergLP::ball::Severity::e_TRACE - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noDEBUG",
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1,
                                  BloombergLP::ball::Severity::e_DEBUG - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noINFO",
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1,
                                  BloombergLP::ball::Severity::e_INFO - 1);

        BloombergLP::ball::Administration::addCategory(
                                   "noWARN",
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1,
                                   BloombergLP::ball::Severity::e_WARN - 1);

        BloombergLP::ball::Administration::addCategory(
                                  "noERROR",
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1,
                                  BloombergLP::ball::Severity::e_ERROR - 1);

        BloombergLP::ball::Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1);

        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const Cat  *CAT  = BALL_LOG_CATEGORY;
        const char *FILE = __FILE__;

        if (verbose) bsl::cout << "Now test the variadic '*_LOGVA_*' macros"
                                                  " with varying arguments.\n";

        ASSERT(!bsl::strcmp("sieve", CAT->categoryName()));

        if (veryVerbose) bsl::cout << "\tTesting exception safety"
                                   << bsl::endl;
        {
            BALL_LOG_SET_CATEGORY("pass")

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 0"
                                           << bsl::endl;

            BALL_LOGVA(TRACE, FORMAT_SPEC_0_ARGS);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_0_ARGS);
            BALL_LOGVA(TRACE, FORMAT_SPEC_0_ARGS);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 1"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_1_ARGS, 1);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 2"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_2_ARGS, 1, 2);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 3"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 4"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 5"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 6"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 7"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 8"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 9"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            BALL_LOGVA(TRACE, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 0\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_0_ARGS);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[0]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 1\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[1]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 2\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[2]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 3\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[3]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 4\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[4]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 5\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[5]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 6\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[6]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 7\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[7]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 8\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[8]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 9\n";
        {
            const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
            BALL_LOGVA(255, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());

            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_TRACE(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 4`\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_INFO(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_WARN(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")

                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_ERROR(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MSG[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")

                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_0_ARGS);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_0_ARGS);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const BloombergLP::ball::Record PREVIOUS_RECORD =
                                               observer->lastPublishedRecord();
                BALL_LOGVA_FATAL(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(PREVIOUS_RECORD == observer->lastPublishedRecord());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MSG[9]));
        }

        if (veryVerbose) bsl::cout <<
                             "\tTesting Buffer Overflow with 'printf' Macro\n";
        {
            const int BUFLEN = u::messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longStr    = new char[N];
            bsl::fill(longStr + 0, longStr + N, 'x');
            longStr[N-1] = '\0';

            // Note: when the "long string" is to be used as the format
            // specification, we need a string literal instead to avoid
            // possible compiler warning:
            //   warning: format string is not a string literal (potentially
            //    insecure) [-Wformat-security]

#define OVERLY_LONG_STRING                                                    \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" \
           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx!?" \
           "****************************************************************"

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'b';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'c';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'd';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'e';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'f';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'g';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d",
                                  longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'h';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d",
                                  longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'i';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d %d",
                                  longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'j';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d %d %d",
                                  longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'l';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'm';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'n';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'o';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'p';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'q';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'r';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 's';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 't';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'v';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'w';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'x';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'y';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'z';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'A';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'B';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'C';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'D';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'F';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'G';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'H';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'I';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'J';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'K';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'L';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'M';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'N';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'P';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'Q';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'R';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'S';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'T';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'U';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'V';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'W';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'X';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'Z';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '0';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '1';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '2';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '3';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '4';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '5';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '6';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '7';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '!';
                longStr[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL(OVERLY_LONG_STRING);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = '9';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s", longStr);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'a';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d", longStr, 2);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'b';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d", longStr, 2, 3);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'c';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d %d", longStr, 2, 3, 4);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'd';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d %d %d", longStr, 2, 3, 4, 5);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'e';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'f';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'g';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            {
                longStr[BUFLEN - 2] = 'h';
                longStr[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d %d %d",
                                 longStr, 2, 3, 4, 5, 6, 7, 8, 9);
                longStr[BUFLEN - 1] = '\0';
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, longStr));
            }

            delete [] longStr;

#undef OVERLY_LONG_STRING
        }

        // Note that the following is expressly meant to test the modifications
        // made to the 'printf'-style macros to use the 'do { ... } while(0)'
        // idiom (DRQS 13261698).  In particular, the 'if' statements are
        // *INTENTIONALLY* *NOT* fully bracketed ('{}'-enclosed), contrary to
        // the BDE coding standard.

        if (veryVerbose)
            bsl::cout << "\tTesting macros in unbracketed context."
                      << bsl::endl;
        {
            int unbracketedLoggingFlag = 1;

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_TRACE(FORMAT_SPEC_0_ARGS);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, TRACE,
                                       FILE, LINE, MSG[0]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, DEBUG,
                                       FILE, LINE, MSG[1]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, INFO,
                                       FILE, LINE, MSG[2]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, WARN,
                                       FILE, LINE, MSG[3]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, ERROR,
                                       FILE, LINE, MSG[4]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(u::isRecordOkay(observer, CAT, FATAL,
                                       FILE, LINE, MSG[5]));
            }

            ASSERT(1 == unbracketedLoggingFlag);
        }

        #undef FORMAT_SPEC_0_ARGS
        #undef FORMAT_SPEC_1_ARGS
        #undef FORMAT_SPEC_2_ARGS
        #undef FORMAT_SPEC_3_ARGS
        #undef FORMAT_SPEC_4_ARGS
        #undef FORMAT_SPEC_5_ARGS
        #undef FORMAT_SPEC_6_ARGS
        #undef FORMAT_SPEC_7_ARGS
        #undef FORMAT_SPEC_8_ARGS
        #undef FORMAT_SPEC_9_ARGS

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BALL_LOG_* MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   BALL_LOG_SET_CATEGORY
        //   BALL_LOG_CATEGORY
        //   BALL_LOG_THRESHOLD
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
            << "Testing BALL_LOG_* Macros" << bsl::endl
            << "=========================" << bsl::endl;
        }

        ASSERT(0 == bsl::strcmp(__FILE__, __FILE__));

        // Sanity check BALL_LOG_CATEGORY & BALL_LOG_THRESHOLD
        {
            const int UC = Holder::e_UNINITIALIZED_CATEGORY;
            const int DC = Holder::e_DYNAMIC_CATEGORY;

            struct {
                const int  d_line;
                const int  d_threshold;
                const Cat *d_category;
            } DATA[] = {
                // Line           Threshold      Category
                // ----           ---------      --------
                { L_,                0,        (Cat *)  0    },
                { L_,                1,        (Cat *)  0    },
                { L_,                1,        (Cat *) 32    },
                { L_,               UC,        (Cat *) 32    },
                { L_,               DC,        (Cat *) 64    },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int  LINE      = DATA[i].d_line;
                const int  THRESHOLD = DATA[i].d_threshold;
                const Cat *CATEGORY  = DATA[i].d_category;

                Holder BALL_LOG_CATEGORYHOLDER = {
                    { THRESHOLD },
                    { const_cast<Cat *>(CATEGORY) },
                    { 0 }
                };

                ASSERTV(LINE, THRESHOLD, BALL_LOG_THRESHOLD,
                        THRESHOLD == BALL_LOG_THRESHOLD);
                ASSERTV(LINE, CATEGORY, BALL_LOG_CATEGORY,
                        CATEGORY == BALL_LOG_CATEGORY);
            }
        }

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        {
            const char *CATEGORY_NAME1 = "EQUITY.NASD";
            BALL_LOG_SET_CATEGORY(CATEGORY_NAME1)  // creates new category

            const Cat  *CATEGORY1  = BALL_LOG_CATEGORYHOLDER.category();
            const int   MAX_LEVEL1 = CATEGORY1->maxLevel();

            ASSERTV(CATEGORY_NAME1, CATEGORY1->categoryName(),
                    0 == bsl::strcmp(CATEGORY_NAME1,
                                    CATEGORY1->categoryName()));
            ASSERTV(MAX_LEVEL1, BALL_LOG_CATEGORYHOLDER.threshold(),
                    MAX_LEVEL1 == BALL_LOG_CATEGORYHOLDER.threshold());

            BALL_LOG_TRACE << "This will load the category";

            BloombergLP::ball::Administration::setMaxNumCategories(3);
            ASSERT(3 == BloombergLP::ball::Administration::maxNumCategories());
            {
                const char *CATEGORY_NAME2 = "EQUITY.NYSE";
                BALL_LOG_SET_CATEGORY(CATEGORY_NAME2)

                const Cat  *CATEGORY2  = BALL_LOG_CATEGORYHOLDER.category();
                const int   MAX_LEVEL2 = CATEGORY2->maxLevel();

                ASSERTV(CATEGORY_NAME2, CATEGORY2->categoryName(),
                        0 == bsl::strcmp(CATEGORY_NAME2,
                                         CATEGORY2->categoryName()));
                ASSERTV(MAX_LEVEL2, BALL_LOG_CATEGORYHOLDER.threshold(),
                        MAX_LEVEL2 == BALL_LOG_CATEGORYHOLDER.threshold());
                ASSERT(0 == bsl::strcmp(CATEGORY_NAME2,
                                        BALL_LOG_CATEGORY->categoryName()));
                ASSERT(0 != bsl::strcmp(CATEGORY_NAME1,
                                        BALL_LOG_CATEGORY->categoryName()));

                BALL_LOG_TRACE << "This will load the category";

                {
                    const char *CATEGORY_NAME3 = "EQUITY.DOW";
                    BALL_LOG_SET_CATEGORY(CATEGORY_NAME3)

                    const Cat *CATEGORY3  = BALL_LOG_CATEGORYHOLDER.category();
                    const int  MAX_LEVEL3 = CATEGORY3->maxLevel();

                    ASSERTV(CATEGORY_NAME3, CATEGORY3->categoryName(),
                            0 != bsl::strcmp(CATEGORY_NAME3,
                                             CATEGORY3->categoryName()));
                    ASSERTV(MAX_LEVEL3,
                            BALL_LOG_CATEGORYHOLDER.threshold(),
                            MAX_LEVEL3 == BALL_LOG_CATEGORYHOLDER.threshold());
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME3,
                                           BALL_LOG_CATEGORY->categoryName()));
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME2,
                                           BALL_LOG_CATEGORY->categoryName()));
                    ASSERT(0 != bsl::strcmp(CATEGORY_NAME1,
                                           BALL_LOG_CATEGORY->categoryName()));

                    BALL_LOG_TRACE << "This will load the default category";
                }

                ASSERT(0 == bsl::strcmp(CATEGORY_NAME2,
                                        BALL_LOG_CATEGORY->categoryName()));

            }

            ASSERT(0 == bsl::strcmp(CATEGORY_NAME1,
                                    BALL_LOG_CATEGORY->categoryName()));
        }
        for (int i = 0; i < 2; ++i) {
            BloombergLP::ball::Administration::setMaxNumCategories(4 + i);

            bsl::ostringstream os;
            os << "CAT" << i;

            bsl::string cat1 = os.str();
            os.str("");

            BALL_LOG_SET_DYNAMIC_CATEGORY(cat1.c_str());// creates new category
            ASSERT(0 == bsl::strcmp(cat1.c_str(),
                                    BALL_LOG_CATEGORY->categoryName()));

            {
                os << "CAT.NESTED" << i;
                bsl::string cat2 = os.str();
                os.str("");

                BALL_LOG_SET_DYNAMIC_CATEGORY(cat2.c_str()); // get default
                ASSERT(0 != bsl::strcmp(cat1.c_str(),
                                        BALL_LOG_CATEGORY->categoryName()));
                ASSERT(0 != bsl::strcmp(cat2.c_str(),
                                        BALL_LOG_CATEGORY->categoryName()));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING UTILITY FUNCTIONS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //   (1) Verify that 'u::messageBuffer' returns non-null and that
        //       'u::messageBufferSize' bytes may be overwritten
        //
        // Testing:
        //   void logMessage(*category, severity, *file, line, *msg);
        //   char *u::messageBuffer();
        //   int u::messageBufferSize();
        //   const ball::Category *setCategory(Holder *, const char *);
        //   const ball::Category *setCategory(const char *categoryName);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << "\nTesting Utility Functions"
                      << "\n=========================" << bsl::endl;
        }

        using namespace BloombergLP;

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard   lmg(lmc);

        bsl::shared_ptr<ball::TestObserver> observer(
               new (ta) ball::TestObserver(&bsl::cout, &ta), &ta);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        if (veryVerbose) {
            bsl::cout << "\tTesting 'messageBuffer' and 'messageBufferSize'"
                      << bsl::endl;
        }

        const ball::Category& defaultCategory = manager.defaultCategory();

        const int DEFAULT_CAT_MAX_LEVEL = defaultCategory.maxLevel();

        manager.setDefaultThresholdLevels(192, 96, 64, 32);
        ASSERT(DEFAULT_CAT_MAX_LEVEL != 192);

        if (veryVerbose) bsl::cout << "\tTesting 'setCategory'" << bsl::endl;
        {
            const ball::Category *category;
            category = ball::Log::setCategory("EQUITY.NASD");  // creates new
                                                               // category
            ASSERT(0   == bsl::strcmp("EQUITY.NASD",
                                      category->categoryName()));
            ASSERT(192 == category->maxLevel());

            ball::Administration::setMaxNumCategories(2);
            ASSERT(2 == ball::Administration::maxNumCategories());

            category = ball::Log::setCategory("EQUITY.NYSE"); // gets *Default*
                                                              // *Category*
            ASSERT(0 == bsl::strcmp(defaultCategory.categoryName(),
                                    category->categoryName()));
            ASSERT(DEFAULT_CAT_MAX_LEVEL == category->maxLevel());
        }

        if (veryVerbose) bsl::cout << "\tTesting 'setCategory' taking a holder"
                                   << bsl::endl;
        {
            ball::Administration::setMaxNumCategories(3);
            ASSERT(3 == ball::Administration::maxNumCategories());

            // Holders must be declared 'static' so that their lifetimes exceed
            // that of the logger manager singleton.

            static Holder mH = { { Holder::e_UNINITIALIZED_CATEGORY }, { 0 },
                                                                       { 0 } };
            const Holder& H = mH;

            ball::Log::setCategory(&mH, "EQUITY.NYSE"); // creates new category
            ASSERT(  0 == bsl::strcmp("EQUITY.NYSE",
                                      H.category()->categoryName()));
            ASSERT(192 == H.threshold());

            static Holder mH2 = { { Holder::e_UNINITIALIZED_CATEGORY }, { 0 },
                                                                       { 0 } };
            const Holder& H2 = mH2;

            ball::Log::setCategory(&mH2, "EQUITY.IEX");  // gets *Default*
                                                         // *Category*
            ASSERT(0 == bsl::strcmp(defaultCategory.categoryName(),
                                    H2.category()->categoryName()));
            ASSERT(DEFAULT_CAT_MAX_LEVEL == H2.threshold());
        }

         if (veryVerbose) bsl::cout << "\tTesting 'logMessage'" << bsl::endl;
         {
             const Cat  *CAT  = ball::Log::setCategory("EQUITY.NASD");
             const int   SEV  = BloombergLP::ball::Severity::e_WARN;
             const char *FILE = __FILE__;
             const int   LINE = 1066;
             const char *MSG  = "logMessage test";

             const int NREC = observer->numPublishedRecords();
             ball::Log::logMessage(CAT, SEV, FILE, LINE, MSG);
             ASSERT(NREC + 1 == observer->numPublishedRecords());
             ASSERT(u::isRecordOkay(observer, CAT, SEV, FILE, LINE, MSG));
         }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // IBM MULTITHREADED SPEWING TEST
        //
        // Concerns:
        //   That multiple threads writing with ball will not crash.  Streams
        //   on ibm have a problem where they are not thread-safe.  Ibm solved
        //   this by having ONE global mutex shared by all streams, including
        //   cin, cout, cerr and even stringstreams, which undermined
        //   multithreading.  We set a compilation flag to not use this
        //   mutex, and we changed ball to use 'FILE *' i/o instead of streams
        //   wherever possible.  There are some who believe the thread-unsafe
        //   part was only to do with locales, which are going to be completely
        //   set up before any multithreading begins, so it shouldn't be a
        //   problem.
        //
        // Plan:
        //   Have 10 threads, each in an infinite loop spewing BALL messages,
        //   make sure that the task doesn't crash.  Run this test with
        //   output redirected to /dev/null leave it for hours, and see if it
        //   crashes.  This test need only be performed on ibm.
        // --------------------------------------------------------------------

        using namespace BALL_LOG_TEST_CASE_MINUS_1;
        using namespace BloombergLP;

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard   lmg(lmc);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<ball::StreamObserver> observer =
                        bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        bslmt::ThreadAttributes attributes;
        bslmt::ThreadUtil::Handle handles[10];
        for (int i = 0; i < 10; ++i) {
            bslmt::ThreadUtil::create(&handles[i],
                                      attributes,
                                      ThreadFunctor());
        }

        char buffer[256];
        bsl::string input;
        do {
            bsl::cout << "Enter something: ";
            bsl::cin.getline(buffer, 256);
            input = buffer;
        } while (input != "exit");

        // just exit the program, which will kill the threads
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //: 1 This test measures the time of performing multiple logging macro
        //:   invocations which makes it possible to compare the performance of
        //:   different implementations.
        //
        // Plan
        //: 1 Several times in a row create 10 threads, each spewing a large a
        //:   mount of BALL messages. Measure the total time of execution and
        //:   publish it.
        // --------------------------------------------------------------------

        using namespace BALL_LOG_TEST_CASE_MINUS_2;
        using namespace BloombergLP;

        const int CYCLES_NUM     = 5;
        double    totalTimes     = 0;

        TestAllocator ta(veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_ERROR,  // record level
                                 ball::Severity::e_OFF,    // passthrough level
                                 ball::Severity::e_OFF,    // trigger level
                                 ball::Severity::e_OFF);   // triggerAll level

        ball::LoggerManagerScopedGuard  lmg(lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        bsl::shared_ptr<TestObserver> observer(
                                  new (ta) TestObserver(&bsl::cout, &ta), &ta);

        ASSERT(0 == manager.registerObserver(observer, "test"));

        bsls::Stopwatch timer;
        timer.start();

        for (int i = 0; i < CYCLES_NUM; ++i) {
            u::executeInParallel(NUM_THREADS, workerThreadPerformance);
        }

        timer.stop();

        totalTimes += timer.accumulatedWallTime();

        bsl::cout << "Total time: "
                  << totalTimes
                  << " seconds."
                  << bsl::endl;
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
