// ball_log.t.cpp                                                     -*-C++-*-
#include <ball_log.h>

#include <ball_administration.h>
#include <ball_attribute.h>
#include <ball_attributecontainer.h>
#include <ball_attributecontainerlist.h>
#include <ball_attributecontext.h>
#include <ball_defaultattributecontainer.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_predicate.h>
#include <ball_record.h>
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
#include <bsl_sstream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>
#include <bsl_vector.h>

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
// ----------------------------------------------------------------------------
// [32] USAGE EXAMPLE
// [33] RULE-BASED LOGGING USAGE EXAMPLE
// [34] CLASS-SCOPE LOGGING USAGE EXAMPLE
// [35] BASIC LOGGING USAGE EXAMPLE
// [29] CONCERN: 'BALL_LOG_*_BLOCK' MACROS
// [30] CONCERN: 'BALL_LOGCB_*_BLOCK' MACROS
// [31] CONCERN: DEGENERATE LOG MACROS USAGE

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
typedef BloombergLP::ball::ThresholdAggregate Thresholds;

typedef BloombergLP::bslma::TestAllocator     TestAllocator;

typedef BloombergLP::bsls::Types::IntPtr      IntPtr;
typedef BloombergLP::bsls::Types::Int64       Int64;

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

using namespace BloombergLP;

class TempDirectoryGuard {
    // This class implements a scoped temporary directory guard.  The guard
    // tries to create a temporary directory in the system-wide temp directory
    // and falls back to the current directory.

    // DATA
    bsl::string       d_dirName;      // path to the created directory
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    TempDirectoryGuard(const TempDirectoryGuard&);
    TempDirectoryGuard& operator=(const TempDirectoryGuard&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TempDirectoryGuard,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TempDirectoryGuard(bslma::Allocator *basicAllocator = 0)
        // Create temporary directory in the system-wide temp or current
        // directory.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
    : d_dirName(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
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

        int res = bdls::PathUtil::appendIfValid(&tmpPath, "ball_");
        ASSERTV(tmpPath, 0 == res);

        res = bdls::FilesystemUtil::createTemporaryDirectory(&d_dirName,
                                                             tmpPath);
        ASSERTV(tmpPath, 0 == res);
    }

    ~TempDirectoryGuard()
        // Destroy this object and remove the temporary directory (recursively)
        // created at construction.
    {
        bdls::FilesystemUtil::remove(d_dirName, true);
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
    int bufferSize = 0;
    char *buffer = Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return buffer;
}

inline
int messageBufferSize()
{
    BloombergLP::bslmt::Mutex *mutex = 0;
    int bufferSize = 0;
    Obj::obtainMessageBuffer(&mutex, &bufferSize);
    Obj::releaseMessageBuffer(mutex);
    return bufferSize;
}

inline
static void scribbleBuffer()
    // Assign 'FILL' to each of the 'messageBufferSize' bytes starting at the
    // address returned by 'messageBuffer'.
{
    bsl::memset(messageBuffer(), FILL, messageBufferSize());
}

static bool isBufferUnchangedSinceScribbled()
    // Return 'true' if no portion of the buffer returned by 'messageBuffer'
    // has been overwritten since 'scribbleBuffer' was last called, and 'false'
    // otherwise.
{
    const char *p   = messageBuffer();
    const char *end = p + messageBufferSize();

    while (p < end) {
        if (*p++ != (char)FILL) {
            return false;                                             // RETURN
        }
    }
    return true;
}

static bool isRecordOkay(const BloombergLP::ball::TestObserver&  observer,
                         const BloombergLP::ball::Category      *category,
                         int                                     severity,
                         const char                             *fileName,
                         int                                     lineNumber,
                         const char                             *message)
    // Return 'true' if the last record published to the specified 'observer'
    // includes the name of the specified 'category' and the specified
    // 'severity', 'fileName', 'lineNumber', and 'message', and 'false'
    // otherwise.
{
    const BloombergLP::ball::RecordAttributes& attributes =
                                  observer.lastPublishedRecord().fixedFields();

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
    // Capture the 'streambuf' used by 'cerr' at this objects creation, and
    // restore that to be the 'cerr' read buffer on this objects destruction.

    bsl::streambuf *d_cerrBuf;

  private:
    // NOT IMPLEMENTED
    CerrBufferGuard(const CerrBufferGuard&);
    CerrBufferGuard& operator=(const CerrBufferGuard&);
  public:
    CerrBufferGuard() : d_cerrBuf(bsl::cerr.rdbuf()) {}
        // Capture the current 'streambuf' being used by 'cerr', and upon this
        // objects destruction, set it to be the 'streambuf' used by 'cerr'.

    ~CerrBufferGuard() { bsl::cerr.rdbuf(d_cerrBuf); }
        // Restore the 'streambuf' being used by 'cerr' to that which was
        // being used on this objects construction.
};

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
// The following example demonstrates using rules and attributes to
// conditionally enable logging particular messages.
//
// We start by defining a function, 'processData', that is passed data in a
// 'vector<char>' and information about the user who sent the data.  This
// example function performs no actual processing, but does log a single
// message at the 'ball::Severity::DEBUG' threshold level.  The 'processData'
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
// We add our attributes to the generic "default" attribute container.  In
// practice we could create a more efficient attribute container
// implementation specifically for these three attributes (uuid, luw, and
// terminalNumber).  See the 'ball::attributeContainer' component documentation
// for an example.
//..
      ball::DefaultAttributeContainer attributes;
      attributes.addAttribute(ball::Attribute("uuid", uuid));
      attributes.addAttribute(ball::Attribute("luw", luw));
      attributes.addAttribute(ball::Attribute("terminalNumber",
                                             terminalNumber));
      ball::AttributeContext *context = ball::AttributeContext::getContext();
      ball::AttributeContext::iterator it =
                                         context->addAttributes(&attributes);
//..
// In this simplified example we perform no actual processing, and simply log
// a message at the 'ball::Severity::DEBUG' level.
//..
      BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");

      BALL_LOG_DEBUG << "An example message";
//..
// Because 'attributes' is defined on this thread's stack, it must be removed
// from this thread's attribute context before exiting the function.
//..
      context->removeAttributes(it);
  }
//..

}  // close enterprise namespace

// ============================================================================
//                         CASE 28 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace BALL_LOG_TEST_CASE_28 {

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
                                                                  &incCallback;

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

}  // close namespace BALL_LOG_TEST_CASE_28

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

    TempDirectoryGuard tempDirGuard;

    bsl::string filename(tempDirGuard.getTempDirName());
    bdls::PathUtil::appendRaw(&filename, "stderrOut");

    fflush(stderr);
    int fd = creat(filename.c_str(), 0777);
    ASSERT(fd != -1);
    int saved_stderr_fd = dup(2);
    dup2(fd, 2);
    if (verbose)
        bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

    bsl::stringstream os;
    bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
    bsl::cerr.rdbuf(os.rdbuf());

    bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                                  &incCallback;

    BALL_LOG_SET_CATEGORY("Logger Manager Comes and Goes");

    if (verbose)
        bsl::cout << "Safely invoked 'BALL_LOG_SET_CATEGORY' macro."
                  << bsl::endl;

    numIncCallback = 0;

    BALL_LOGCB_TRACE(callback) << "Logger Manager?";
    BALL_LOGCB_DEBUG(callback) << "Logger Manager?";
    BALL_LOGCB_INFO(callback)  << "Logger Manager?";
    BALL_LOGCB_WARN(callback)  << "Logger Manager?";
    BALL_LOGCB_ERROR(callback) << "Logger Manager?";
    BALL_LOGCB_FATAL(callback) << "Logger Manager?";

    if (loggerManagerExistsFlag) {
        numPublishedSoFar += 6;
        ASSERT(6 == numIncCallback);
    }
    else {
        ASSERT(3 == numIncCallback);
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
        ASSERT(0 == numLines);
    }
    else {
        ASSERT(3 == numLines);
    }
    fs.close();
#endif
    ASSERT("" == os.str());
    bsl::cerr.rdbuf(cerrBuf);
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

char message[MAX_MSG_SIZE + 1];
int severity = BloombergLP::ball::Severity::e_TRACE;
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

    TempDirectoryGuard tempDirGuard;

    bsl::string filename(tempDirGuard.getTempDirName());
    bdls::PathUtil::appendRaw(&filename, "stderrOut");

    fflush(stderr);
    int fd = creat(filename.c_str(), 0777);
    ASSERT(fd != -1);
    int saved_stderr_fd = dup(2);
    dup2(fd, 2);
    if (verbose)
        bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

    bsl::stringstream os;
    bsl::streambuf *cerrBuf = bsl::cerr.rdbuf();
    bsl::cerr.rdbuf(os.rdbuf());

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

    scribbleBuffer();
    BALL_LOGVA(severity, "Hello?");
    ASSERT(!isBufferUnchangedSinceScribbled());
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
    ASSERT("" == os.str());
    bsl::cerr.rdbuf(cerrBuf);
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

    BloombergLP::ball::TestObserver  testObserver(&bsl::cout);
    BloombergLP::ball::TestObserver *TO = &testObserver;

    TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.

      case 35: {
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
        //   BASIC LOGGING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

            ball::StreamObserver observer(&bsl::cout);

            ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  ball::Severity::e_OFF,    // record level
                  ball::Severity::e_TRACE,  // passthrough level
                  ball::Severity::e_OFF,    // trigger level
                  ball::Severity::e_OFF);   // triggerAll level
            ball::LoggerManagerScopedGuard lmg(&observer, lmc);

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
      case 34: {
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
            ball::LoggerManagerConfiguration lmc;
            lmc.setDefaultThresholdLevelsIfValid(
                  ball::Severity::e_TRACE,  // record level
                  ball::Severity::e_TRACE,  // passthrough level
                  ball::Severity::e_ERROR,  // trigger level
                  ball::Severity::e_FATAL); // triggerAll level
            ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            ASSERT(1 == ball::TestObserver::numInstances());
            ASSERT(0 == TO->numPublishedRecords());

            pckg::Thing mX;  const pckg::Thing& X = mX;

            mX.outOfLineMethodThatLogs(true);

            ASSERT(1 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "PCKG.THING",
                          TO->lastPublishedRecord().fixedFields().category()));

            mX.outOfLineMethodThatLogs(false);

            ASSERT(2 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "X.Y.Z",
                          TO->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogs();

            ASSERT(3 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "PCKG.THING",
                          TO->lastPublishedRecord().fixedFields().category()));
        }

      } break;
      case 33: {
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

//..
// Next we demonstrate how to create a logging rule that sets the passthrough
// logging threshold to 'ball::Severity::TRACE' (i.e., enables more verbose
// logging) for a particular user when calling the 'processData' function
// above.
//
// We start by creating the singleton logger manager that we configure with
// the stream observer and default configuration.  We then call the
// 'processData' function.  This first call to 'processData' will not result in
// any logged messages because the 'processData' function logs its message at
// the 'ball::Severity::DEBUG' level, which is below the default configured
// logging threshold.
//..
    ball::StreamObserver observer(&bsl::cout);
    ball::LoggerManagerConfiguration lmc;
    ball::LoggerManagerScopedGuard lmg(&observer, lmc);

    BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");

    bsl::vector<char> message;

    BALL_LOG_ERROR << "Processing the first message.";
    processData(3938908, 2, 9001, message);
//..
// Now we add a logging rule to set the passthrough threshold to be
// 'ball::Severity::TRACE' (i.e., enable more verbose logging) if the thread's
// context contains a "uuid" of '3938908'.  Note that the category for the
// 'ball::Rule' is the wild-card value '*' so that the rule will apply to all
// categories.
//..
    ball::Rule rule("*", 0, ball::Severity::e_TRACE, 0, 0);
    rule.addPredicate(ball::Predicate("uuid", 3938908));
    ball::LoggerManager::singleton().addRule(rule);

    BALL_LOG_ERROR << "Processing the second message.";
    processData(3938908, 2, 9001, message);

    BALL_LOG_ERROR << "Processing the third message.";
    processData(2171395, 2, 9001, message);
//..
// The final call to the 'processData' function above, passes a "uuid" of
// '2171395' (not '3938908') so the default logging threshold will apply and no
// message will be logged.
//
// The resulting logged output for this example looks like the following:
//..
// ERROR example.cpp:105 EXAMPLE.CATEGORY Processing the first message.
// ERROR example.cpp:117 EXAMPLE.CATEGORY Processing the second message.
// DEBUG example.cpp:35 EXAMPLE.CATEGORY An example message
// ERROR example.cpp:129 EXAMPLE.CATEGORY Processing the third message.
//..
      } break;
      case 32: {
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

        ball::StreamObserver observer(&cout);
        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard lmg(&observer, lmc);

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
                    ball::Log::format(messageBuffer(),
                                      messageBufferSize(),
                                      formatSpec, 400, "SUNW", 5.65);
                    ball::Log::logMessage(category,
                                         BloombergLP::ball::Severity::e_INFO,
                                         __FILE__, __LINE__,
                                         messageBuffer());
                    if (veryVeryVerbose) P(messageBuffer());
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
      case 31: {
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

        bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                                  &incCallback;
        bslma::TestAllocator ta(veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard lmg(lmc, &ta);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_TRACE,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassOFF",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

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

            int severity = ball::Severity::e_TRACE;

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

            int severity = ball::Severity::e_FATAL;

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
      case 30: {
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

        bsl::function<void(BloombergLP::ball::UserFields *)> callback =
                                                                  &incCallback;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard lmg(lmc, &ta);

        bsl::shared_ptr<ball::TestObserver> observer(
                            new (ta) ball::TestObserver(&bsl::cout, &ta), &ta);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_TRACE,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassDEBUG",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_DEBUG,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassINFO",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_INFO,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassWARN",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_WARN,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassERROR",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_ERROR,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassFATAL",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_FATAL,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        {
            BALL_LOG_SET_CATEGORY("PassTRACE");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_TRACE);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(8     == numIncCallback);
            ASSERTV(N + 8 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(8     == count);
            ASSERTV(8     == numIncCallback);
            ASSERTV(N + 8 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassDEBUG");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_DEBUG);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(7     == count);
            ASSERTV(7     == numIncCallback);
            ASSERTV(N + 7 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassINFO");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_INFO);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(6     == numIncCallback);
            ASSERTV(N + 6 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassWARN");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_WARN);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(5     == numIncCallback);
            ASSERTV(N + 5 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassERROR");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_ERROR);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(4     == numIncCallback);
            ASSERTV(N + 4 == observer->numPublishedRecords());
        }

        {
            BALL_LOG_SET_CATEGORY("PassFATAL");

            int count = 0;
            const int N = observer->numPublishedRecords();

            numIncCallback = 0;

            BALL_LOGCB_FATAL_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_ERROR_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_WARN_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_INFO_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_DEBUG_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            BALL_LOGCB_TRACE_BLOCK(callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(1     == numIncCallback);
            ASSERTV(N + 1 == observer->numPublishedRecords());

            // Testing BALL_LOGCB_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_FATAL);

            BALL_LOGCB_STREAM_BLOCK(severity - 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(2     == numIncCallback);
            ASSERTV(N + 2 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());

            BALL_LOGCB_STREAM_BLOCK(severity + 1, callback) {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(3     == numIncCallback);
            ASSERTV(N + 3 == observer->numPublishedRecords());
        }
      } break;
      case 29: {
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

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManagerScopedGuard lmg(lmc, &ta);

        bsl::shared_ptr<ball::TestObserver> observer(
                            new (ta) ball::TestObserver(&bsl::cout, &ta), &ta);

        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Adding categories with various passthrough levels.
        ASSERT(0 != manager.setCategory("PassTRACE",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_TRACE,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassDEBUG",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_DEBUG,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassINFO",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_INFO,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassWARN",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_WARN,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassERROR",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_ERROR,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

        ASSERT(0 != manager.setCategory("PassFATAL",
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_FATAL,
                                        ball::Severity::e_OFF,
                                        ball::Severity::e_OFF));

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(6     == count);
            ASSERTV(N + 6 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_TRACE);

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(5     == count);
            ASSERTV(N + 5 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_DEBUG);

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(4     == count);
            ASSERTV(N + 4 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_INFO);

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(3     == count);
            ASSERTV(N + 3 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_WARN);

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(2     == count);
            ASSERTV(N + 2 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_ERROR);

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
            ASSERTV(true  == BALL_LOG_IS_ENABLED(ball::Severity::e_FATAL));

            BALL_LOG_ERROR_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_ERROR));

            BALL_LOG_WARN_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_WARN));

            BALL_LOG_INFO_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_INFO));

            BALL_LOG_DEBUG_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false  == BALL_LOG_IS_ENABLED(ball::Severity::e_DEBUG));

            BALL_LOG_TRACE_BLOCK {
                ++count;
                BALL_LOG_OUTPUT_STREAM << "message";
            }

            ASSERTV(1     == count);
            ASSERTV(N + 1 == observer->numPublishedRecords());
            ASSERTV(false == BALL_LOG_IS_ENABLED(ball::Severity::e_TRACE));

            // Testing BALL_LOG_STREAM_BLOCK macro.
            int severity = static_cast<int>(ball::Severity::e_FATAL);

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

        using namespace BALL_LOG_TEST_CASE_28;
        using namespace BloombergLP;  // okay here

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        ball::LoggerManagerConfiguration lmc;
        lmc.setDefaultThresholdLevelsIfValid(
                                 ball::Severity::e_OFF,    // record level
                                 ball::Severity::e_TRACE,  // passthrough level
                                 ball::Severity::e_OFF,    // trigger level
                                 ball::Severity::e_OFF);   // triggerAll level
        ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        ASSERT(1 == ball::TestObserver::numInstances());

        // Exercise 'ClassScopeLoggerA'
        {
            numIncCallback = 0;
            ClassScopeLoggerA::classMethodThatLogsToClassCategory();

            ASSERT(1 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "CLASS CATEGORY A",
                          TO->lastPublishedRecord().fixedFields().category()));
            ASSERT(1 == numIncCallback);

            ClassScopeLoggerA mX;  const ClassScopeLoggerA& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(3 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "STATIC LOCAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(4 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "CLASS CATEGORY A",
                          TO->lastPublishedRecord().fixedFields().category()));

            globalFunctionThatLogsToLocalCategory();

            ASSERT(5 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "GLOBAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLoggerB'
        {
            numIncCallback = 0;
            ClassScopeLoggerB::classMethodThatLogsToClassCategory();

            ASSERT(6 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "CLASS CATEGORY B",
                          TO->lastPublishedRecord().fixedFields().category()));
            ASSERT(0 == numIncCallback);  // callback not relevant here

            ClassScopeLoggerB mX;  const ClassScopeLoggerB& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a dynamic
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(8 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "DYNAMIC LOCAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(9 == TO->numPublishedRecords());
            ASSERT(0 == bsl::strcmp(
                          "CLASS CATEGORY B",
                          TO->lastPublishedRecord().fixedFields().category()));

            globalFunctionThatLogsToLocalCategory();

            ASSERT(10 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "GLOBAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLogger<int>'
        {
            typedef ClassScopeLogger<int> ClassInt;

            ClassInt::classMethodThatLogsToClassCategory();

            ASSERT(11 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "CLASS TEMPLATE CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            ClassInt mX;  const ClassInt& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(13 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "STATIC LOCAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(14 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "CLASS TEMPLATE CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));
        }

        // Exercise 'ClassScopeLogger<double>'
        {
            typedef ClassScopeLogger<double> ClassDouble;

            ClassDouble::classMethodThatLogsToClassCategory();

            ASSERT(15 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "CLASS TEMPLATE CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            ClassDouble mX;  const ClassDouble& X = mX;

            // 'outoflineMethodThatLogsToLocalCategory' logs two messages, the
            // first to the class-scope category and the second to a static
            // category defined at block scope.

            mX.outoflineMethodThatLogsToLocalCategory();

            ASSERT(17 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "STATIC LOCAL CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));

            X.inlineMethodThatLogsToClassCategory();

            ASSERT(18 == TO->numPublishedRecords());
            ASSERT( 0 == bsl::strcmp(
                          "CLASS TEMPLATE CATEGORY",
                          TO->lastPublishedRecord().fixedFields().category()));
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

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(OFF, OFF, OFF, OFF);

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                          configuration,
                                                          &ta);
        BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

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
                        LOOP3_ASSERT(EXP, DATA[i], DATA[j],
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
                    LOOP3_ASSERT(EXP, DATA[i], DATA[j],
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
        //   That the 'logMessages' method uses the current installed rules
        //   when determining whether to log a message.
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

        ball::LoggerManager::initSingleton(TO, lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();
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
                LOOP2_ASSERT(i,j, numPublished == TO->numPublishedRecords());
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
                LOOP2_ASSERT(i,j, numPublished == TO->numPublishedRecords());
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
                    LOOP3_ASSERT(i, j, k,
                                 numPublished == TO->numPublishedRecords());
                }
            }
            manager.removeAllRules();
        }
        ball::LoggerManager::shutDownSingleton();

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
            Holder holder = { 0, 0, 0 };
            ASSERT(enabled == Obj::isCategoryEnabled(&holder, VALUES[i]));
        }

        if (veryVerbose) {
            bsl::cout << "\tTest with and without an applicable rule."
                      << bsl::endl;
        }

        ball::LoggerManagerConfiguration lmc;
        ball::LoggerManager::initSingleton(TO, lmc, &ta);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

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
                Holder holder = { 0, category, 0 };
                LOOP2_ASSERT(i, j,
                             enabled ==
                             Obj::isCategoryEnabled(&holder, VALUES[j]));
            }

            // Test with rule that does not apply.
            manager.addRule(ball::Rule("JUNK", 255, 255, 255, 255));
            for (int j = 0; j < NUM_VALUES; ++j) {
                bool enabled =
                        VALUES[j] <= Thresholds::maxLevel(thresholds[i]);
                Holder holder = { 0, category, 0 };
                LOOP2_ASSERT(i, j,
                             enabled ==
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
                    Holder holder = { 0, category, 0 };
                    LOOP3_ASSERT(i, j, k,
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
        ball::LoggerManager::shutDownSingleton();

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

        CerrBufferGuard cerrBufferGuard;

        numIterations = 10;
        arg1          = -99.244;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread24);
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
            BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread24);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread24);
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

        CerrBufferGuard cerrBufferGuard;

        numIterations = 10;
        arg1          = -99.234;
        arg2          = "Hello World";

        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread23);
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
            BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread23);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread23);
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
        CerrBufferGuard cerrBufferGuard;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread22);
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
            BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread22);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread22);
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
        CerrBufferGuard cerrBufferGuard;
        if (verbose)
            bsl::cout << "\tTesting macro safety without a logger manager."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "NoLoggerManager";
            msg           = "There is currently no logger manager";

            executeInParallel(NUM_THREADS, workerThread21);
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
            BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

            categoryName  = "WithLoggerManager";
            msg           = "There is a logger manager";

            executeInParallel(NUM_THREADS, workerThread21);
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {
            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

            categoryName  = "AfterLoggerManager";
            msg           = "There is no logger manager again";

            executeInParallel(NUM_THREADS, workerThread21);
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

            Holder mH = { THRESHOLD, 0, 0 };
            if (USE_CAT) {
                const Cat *CATEGORY = CM.addCategory(CAT_NAME, THRESHOLD - 2,
                                                     1, 1, 1);
                LOOP2_ASSERT(LINE, CAT_NAME, CATEGORY);
                mH.setCategory(CATEGORY);
            }
            static bool LM_INITIALIZED = false;
            if (USE_LM && !LM_INITIALIZED) {
                LM_INITIALIZED = true;
                BloombergLP::ball::LoggerManagerConfiguration lmc;
                BloombergLP::ball::LoggerManager::initSingleton(TO, lmc, &ta);
            }
            bool result = ball::Log::isCategoryEnabled(&mH, SEVERITY);
            LOOP_ASSERT(LINE, RESULT == result);
        }

        ASSERT(ball::LoggerManager::isInitialized());

        // Test getRecord
        for (int i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_line;
            const bool    USE_CAT     = DATA[i].d_useCategory;
            const char   *FILENAME    = DATA[i].d_name_p;

            const Cat   *CATEGORY = USE_CAT ? (Cat *) 0 : (Cat *) 1;
            ball::Record *RECORD   = ball::Log::getRecord(CATEGORY,
                                                        FILENAME,
                                                        LINE);
            LOOP_ASSERT(LINE, LINE == RECORD->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(
                            FILENAME,
                            RECORD->fixedFields().fileName()));
        }

        BloombergLP::ball::LoggerManager::shutDownSingleton();

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
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc);

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

            LOOP2_ASSERT(CATEGORY, L.category(), CATEGORY == L.category());
            LOOP2_ASSERT(SEVERITY, L.severity(), SEVERITY == L.severity());
            LOOP_ASSERT(LINE, LINE == L.record()->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(FILENAME,
                                    L.record()->fixedFields().fileName()));
            LOOP_ASSERT(LINE, LINE == mL.record()->fixedFields().lineNumber());
            LOOP_ASSERT(LINE, 0 == bsl::strcmp(FILENAME,
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
            TempDirectoryGuard tempDirGuard;

            bsl::string filename(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&filename, "stderrOut");

            fflush(stderr);
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

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
            BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc);

            // Re "sieve" category: (1) if recorded, then also published;
            // (2) never triggered.

            BloombergLP::ball::Administration::addCategory(
                                         "sieve",
                                         BloombergLP::ball::Severity::e_INFO,
                                         BloombergLP::ball::Severity::e_INFO,
                                         0,
                                         0);
            BALL_LOG_SET_DYNAMIC_CATEGORY("sieve")

            int numPublishedRecords = TO->numPublishedRecords();

            BALL_LOG_TRACE << "message11";
            BALL_LOG_DEBUG << "message12";
            BALL_LOG_INFO  << "message13";
            BALL_LOG_WARN  << "message14";
            BALL_LOG_ERROR << "message15";
            BALL_LOG_FATAL << "message16";

            ASSERT(numPublishedRecords + 4 == TO->numPublishedRecords());

            numPublishedRecords = TO->numPublishedRecords();

            BloombergLP::ball::LoggerManager::singleton().setCategory(
                                         "sieve",
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

            ASSERT(numPublishedRecords + 3 == TO->numPublishedRecords());

            numPublishedRecords = TO->numPublishedRecords();

            executeInParallel(NUM_THREADS, workerThread18);

            ASSERT(numPublishedRecords + NUM_MSGS / 2 * NUM_THREADS
                                                == TO->numPublishedRecords());
        }

        if (verbose)
            bsl::cout << "\tTesting macro safety after the logger manager "
                      << "has been destroyed."
                      << bsl::endl;
        {

#ifdef BSLS_PLATFORM_OS_UNIX
            TempDirectoryGuard tempDirGuard;

            bsl::string filename(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&filename, "stderrOut");

            fflush(stderr);
            int fd = creat(filename.c_str(), 0777);
            ASSERT(fd != -1);
            int saved_stderr_fd = dup(2);
            dup2(fd, 2);
            if (veryVerbose)
                bsl::cout << "STDERR redirected to " << filename << bsl::endl;
#endif

            bsl::stringstream os;
            bsl::cerr.rdbuf(os.rdbuf());

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
                                                                  &incCallback;
        numIncCallback = 0;

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_TRACE(callback) << "message";

                ASSERT(0 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_TRACE(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(1 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_DEBUG(callback) << "message";

                ASSERT(1 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
           }

            const int LINE = L_ + 1;
            BALL_LOGCB_DEBUG(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(2 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_INFO(callback) << "message";

                ASSERT(2 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_INFO(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2];
            ASSERT(3 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_WARN(callback) << "message";

                ASSERT(3 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_WARN(callback) << "message" << SEP << ARGS[0]
                                      << SEP << ARGS[1]
                                      << SEP << ARGS[2];
            ASSERT(4 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_ERROR(callback) << "message";

                ASSERT(4 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_ERROR(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(5 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOGCB_FATAL(callback) << "message";

                ASSERT(5 == numIncCallback);
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOGCB_FATAL(callback) << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(6 == numIncCallback);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            bsl::fill(longString + 0, longString + N, 'x');
            longString[N-1] = '\0';
            char *cpyString  = new char[N];
            bsl::strcpy(cpyString, longString);

            {
                const int LINE = L_ + 1;
                BALL_LOGCB_TRACE(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_DEBUG(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_INFO(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_WARN(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_ERROR(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';

                const int LINE = L_ + 1;
                BALL_LOGCB_FATAL(callback) << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;
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

        my_PublishCountingObserver observer;
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                 BloombergLP::ball::Severity::e_TRACE,  // record level
                 BloombergLP::ball::Severity::e_WARN,   // passthrough level
                 BloombergLP::ball::Severity::e_ERROR,  // trigger level
                 BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration);

        // int NUM_MESSAGES = bsl::atoi(argv[2]);
        enum { NUM_MESSAGES = 100000 };
        BALL_LOG_SET_CATEGORY("TEST.CATEGORY");
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            BALL_LOG_TRACE << "DUMMY MESSAGE";
        }
        BALL_LOG_ERROR << "DUMMY MESSAGE";

        if (verbose) P(observer.publishCount());

        ASSERT(observer.publishCount() > 1); // because the triggering message
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

        if (verbose)
            bsl::cout << bsl::endl
                      << "BALL_IS_ENABLED(SEVERITY) Utility MACRO TEST"
                      << bsl::endl
                      << "============================================"
                      << bsl::endl;

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        BloombergLP::ball::LoggerManager& manager =
            BloombergLP::ball::LoggerManager::singleton();

        {
            BALL_LOG_SET_CATEGORY("TEST.CATEGORY");

            BALL_LOG_TRACE << "This will load the category";

            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_TRACE));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_DEBUG));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_INFO));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_WARN));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_ERROR));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                BloombergLP::ball::Severity::e_WARN,
                                BloombergLP::ball::Severity::e_ERROR,
                                BloombergLP::ball::Severity::e_FATAL,
                                BloombergLP::ball::Severity::e_FATAL);

            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_TRACE));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_DEBUG));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_INFO));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_WARN));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_ERROR));
            ASSERT(BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_FATAL));

            manager.setCategory("TEST.CATEGORY",
                                BloombergLP::ball::Severity::e_OFF,
                                BloombergLP::ball::Severity::e_OFF,
                                BloombergLP::ball::Severity::e_OFF,
                                BloombergLP::ball::Severity::e_OFF);

            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_TRACE));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_DEBUG));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_INFO));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_WARN));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_ERROR));
            ASSERT(!BALL_LOG_IS_ENABLED(BloombergLP::ball::Severity::e_FATAL));

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

        if (verbose)
            bsl::cout << bsl::endl
                      << "PRINTF MACRO PERFORMANCE TEST WITH 1 THREADS"
                      << bsl::endl
                      << "============================================"
                      << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_13;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls::Types::Int64 t =
                         BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread13);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

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

        if (verbose)
            bsl::cout << bsl::endl
                      << "PRINTF MACRO PERFORMANCE TEST WITH MULTIPLE THREADS"
                      << bsl::endl
                      << "==================================================="
                      << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_12;

        int i;
        for (i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        for (i = 0; i < NUM_MSGS; ++i) {
            randomSizes[i] = bsl::rand() % (MAX_MSG_SIZE + 1);
        }

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
               BloombergLP::ball::Severity::e_TRACE,  // record level
               BloombergLP::ball::Severity::e_WARN,   // passthrough level
               BloombergLP::ball::Severity::e_ERROR,  // trigger level
               BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls::Types::Int64 t =
                          BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread12);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

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

        if (verbose)
            bsl::cout << bsl::endl
                      << "C++ MACRO PERFORMANCE TEST WITH 1 THREADS"
                      << bsl::endl
                      << "========================================="
                      << bsl::endl;

        if (!verbose) break;

        using namespace BALL_LOG_TEST_CASE_11;

        for (int i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        severity = veryVerbose ? BloombergLP::ball::Severity::e_INFO
                               : BloombergLP::ball::Severity::e_DEBUG;

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                BloombergLP::ball::Severity::e_TRACE,  // record level
                BloombergLP::ball::Severity::e_WARN,   // passthrough level
                BloombergLP::ball::Severity::e_ERROR,  // trigger level
                BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls::Types::Int64 t;

        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread11a);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

        if (verbose) {
            bsl::cout << "number of threads = " << NUM_THREADS << bsl::endl;

            bsl::cout << "number of logged messages per thread = "
                 << NUM_MSGS << bsl::endl;

            bsl::cout << "messages length is "
                 << MAX_MSG_SIZE << bsl::endl;

            bsl::cout << "total logging time with c++ style var macro = "
                 << t/(NUM_MSGS*NUM_THREADS) << bsl::endl;
        }

        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread11);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

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

        for (int i = 0; i < MAX_MSG_SIZE; ++i) {
            message[i] = 'X';
        }
        message[MAX_MSG_SIZE] = '\0';

        severity = veryVerbose ? BloombergLP::ball::Severity::e_INFO
                               : BloombergLP::ball::Severity::e_TRACE;
        if (verbose) P(severity);

        BloombergLP::ball::StreamObserver observer(&bsl::cout);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_DEBUG,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration);
        BloombergLP::bsls::Types::Int64 t;

        BloombergLP::bslim::TestUtil::setFunc(
                               BloombergLP::ball::Severity::e_TRACE == severity
                             ? &Util::doOldTraceConst
                             : &Util::doOldInfoConst);
        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread10);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

        const double oldConstTime = static_cast<double>(t);

        BloombergLP::bslim::TestUtil::setFunc(&Util::doOldVar);
        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread10);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

        const double oldVarTime = static_cast<double>(t);

        BloombergLP::bslim::TestUtil::setFunc(
                               BloombergLP::ball::Severity::e_TRACE == severity
                             ? &Util::doTraceConst
                             : &Util::doInfoConst);

        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread10);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

        const double newConstTime = static_cast<double>(t);

        BloombergLP::bslim::TestUtil::setFunc(&Util::doVar);
        t = BloombergLP::bsls::TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread10);
        t = BloombergLP::bsls::TimeUtil::getTimer() - t;

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

        if (verbose)
            bsl::cout << bsl::endl
                      << "CONCURRENT LOGGING TEST" << bsl::endl
                      << "=======================" << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_9;

        ASSERT(EXP_N_TOTAL == EXP_N_RECORD + EXP_N_PUBLISH + EXP_N_TRIGGER);

        my_Observer observer;
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setLogOrder(
            BloombergLP::ball::LoggerManagerConfiguration::e_FIFO);
        configuration.setDefaultThresholdLevelsIfValid(
               BloombergLP::ball::Severity::e_TRACE,  // record level
               BloombergLP::ball::Severity::e_WARN,   // passthrough level
               BloombergLP::ball::Severity::e_ERROR,  // trigger level
               BloombergLP::ball::Severity::e_FATAL); // triggerAll level
        configuration.setDefaultRecordBufferSizeIfValid(REC_BUF_LIMIT);
        configuration.setTriggerMarkers(
               BloombergLP::ball::LoggerManagerConfiguration::e_NO_MARKERS);
        BloombergLP::bslma::TestAllocator ta(veryVeryVeryVerbose);

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
        executeInParallel(NUM_THREADS, workerThread9);
        if (veryVeryVerbose) observer.print();

        bsl::vector<bsl::pair<int, int> > vv[NUM_THREADS];
        // 'vv[i]' contains the message information for i'th thread

        const bsl::vector<bsl::string>& v = observer.get();
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

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE FIFO LOG ORDER" << bsl::endl
                      << "==========================" << bsl::endl;

        bsl::ostringstream os;
        BloombergLP::ball::StreamObserver observer(&os);
        BloombergLP::ball::LoggerManagerConfiguration configuration;

        // for simplicity we keep the passthrough level to be 'FATAL', so that
        // on trigger event, the message is published only once.

        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_FATAL,  // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
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

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE DEFAULT LOG ORDER (LIFO)" << bsl::endl
                      << "====================================" << bsl::endl;

        char helloWorld1[] = "hello world 1";
        char helloWorld2[] = "hello world 2";
        char helloWorld3[] = "hello world 3";
        char helloWorld4[] = "hello world 4";

        static const struct Data {
            BloombergLP::ball::Severity::Level  d_severity;
            const char                         *d_string_p;
        } DATA[] = {
            { BloombergLP::ball::Severity::e_INFO,  helloWorld1 },
            { BloombergLP::ball::Severity::e_INFO,  helloWorld2 },
            { BloombergLP::ball::Severity::e_INFO,  helloWorld3 },
            { BloombergLP::ball::Severity::e_ERROR, helloWorld4 }
        };
        enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "Testing with fixed-severity macros\n";
        {
            bsl::ostringstream os;
            BloombergLP::ball::StreamObserver observer(&os);
            BloombergLP::ball::LoggerManagerConfiguration configuration;
            configuration.setLogOrder(
                      BloombergLP::ball::LoggerManagerConfiguration::e_FIFO);

            // for simplicity we keep the passthrough level to be 'FATAL', so
            // that on trigger event, the message is published only once.

            configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_FATAL,  // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                             configuration,
                                                             &ta);
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
            bsl::ostringstream os;
            BloombergLP::ball::StreamObserver observer(&os);
            BloombergLP::ball::LoggerManagerConfiguration configuration;
            configuration.setLogOrder(
                      BloombergLP::ball::LoggerManagerConfiguration::e_FIFO);

            // for simplicity we keep the passthrough level to be 'FATAL', so
            // that on trigger event, the message is published only once.

            configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_FATAL,  // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

            BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                             configuration,
                                                             &ta);
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

        if (verbose)
            bsl::cout << bsl::endl
                      << "TESTING THE C++ MACRO WHEN LOGGING "
                      << "RETURNED VALUE OF A FUNCTION"         << bsl::endl
                      << "====================================" << bsl::endl;

        using namespace BALL_LOG_TEST_CASE_6;

        bsl::ostringstream os;
        BloombergLP::ball::StreamObserver observer(&os);
        BloombergLP::ball::LoggerManagerConfiguration configuration;
        configuration.setDefaultThresholdLevelsIfValid(
                  BloombergLP::ball::Severity::e_TRACE,  // record level
                  BloombergLP::ball::Severity::e_WARN,   // passthrough level
                  BloombergLP::ball::Severity::e_ERROR,  // trigger level
                  BloombergLP::ball::Severity::e_FATAL); // triggerAll level

        BloombergLP::ball::LoggerManagerScopedGuard guard(&observer,
                                                         configuration,
                                                         &ta);
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
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_TRACE << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_TRACE << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE));

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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_DEBUG << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_DEBUG << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE));

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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_INFO << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_INFO << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_WARN << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_WARN << "message" << SEP << ARGS[0]
                                       << SEP << ARGS[1]
                                       << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_ERROR << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_ERROR << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE));
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
                int numPublishedRecords = TO->numPublishedRecords();

                BALL_LOG_FATAL << "message";
                ASSERT(numPublishedRecords == TO->numPublishedRecords());
            }

            const int LINE = L_ + 1;
            BALL_LOG_FATAL << "message" << SEP << ARGS[0]
                                        << SEP << ARGS[1]
                                        << SEP << ARGS[2];
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting Buffer Overflow with 'ostream' Macro"
                      << bsl::endl;
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            bsl::fill(longString + 0, longString + N, 'x');
            longString[N-1] = '\0';
            char *cpyString  = new char[N];
            bsl::strcpy(cpyString, longString);

            {
                const int LINE = L_ + 1;
                BALL_LOG_TRACE << longString;

                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_DEBUG << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_INFO << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_WARN << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_ERROR << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOG_FATAL << longString;
                ASSERT(0 == bsl::strcmp(cpyString, longString));
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;
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

        const char *MESSAGE[] = {
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
        ASSERT(MAX_ARGS + 1 == sizeof MESSAGE / sizeof *MESSAGE);

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

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
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_0_ARGS);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 1"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 2"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 3"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 4"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 5"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 6"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 7"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 8"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            if (veryVeryVerbose) bsl::cout << "\t\tTesting BALL_LOGVA - 9"
                                           << bsl::endl;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            scribbleBuffer();
            BALL_LOGVA(TRACE, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(0 == isBufferUnchangedSinceScribbled());
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 0\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_0_ARGS);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 1\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 2\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 3\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 4\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 5\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 6\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 7\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 8\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose) bsl::cout << "\tTesting 'BALL_LOGVA' - 9\n";
        {
            scribbleBuffer();
            BALL_LOGVA(255, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(1 == isBufferUnchangedSinceScribbled());
            const int LINE = L_ + 1;
            BALL_LOGVA(INFO, FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_TRACE' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                scribbleBuffer();
                BALL_LOGVA_TRACE(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_TRACE(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 4`\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_DEBUG' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                scribbleBuffer();
                BALL_LOGVA_DEBUG(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_DEBUG(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_INFO' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                scribbleBuffer();
                BALL_LOGVA_INFO(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_INFO(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_WARN' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                scribbleBuffer();
                BALL_LOGVA_WARN(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_WARN(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_ERROR' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                scribbleBuffer();
                BALL_LOGVA_ERROR(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_ERROR(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 0\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_0_ARGS);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_0_ARGS);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[0]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 1\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_1_ARGS, 1);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_1_ARGS, 1);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[1]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 2\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_2_ARGS, 1, 2);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_2_ARGS, 1, 2);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[2]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 3\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_3_ARGS, 1, 2, 3);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[3]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 4\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[4]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 5\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[5]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 6\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_6_ARGS, 1, 2, 3, 4, 5, 6);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[6]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 7\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_7_ARGS, 1, 2, 3, 4, 5, 6, 7);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[7]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 8\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_8_ARGS, 1, 2, 3, 4, 5, 6, 7, 8);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[8]));
        }

        if (veryVerbose)
            bsl::cout << "\tTesting 'BALL_LOGVA_FATAL' - 9\n";
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                scribbleBuffer();
                BALL_LOGVA_FATAL(FORMAT_SPEC_9_ARGS,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9);
                ASSERT(1 == isBufferUnchangedSinceScribbled());
            }

            const int LINE = L_ + 1;
            BALL_LOGVA_FATAL(FORMAT_SPEC_9_ARGS, 1, 2, 3, 4, 5, 6, 7, 8, 9);
            ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[9]));
        }

        if (veryVerbose) bsl::cout <<
                             "\tTesting Buffer Overflow with 'printf' Macro\n";
        {
            const int BUFLEN = messageBufferSize();
            const int EXCESS = 128;
            const int N      = BUFLEN + EXCESS;
            char *longString = new char[N];
            bsl::fill(longString + 0, longString + N, 'x');
            longString[N-1] = '\0';

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
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'b';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'c';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'd';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'e';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'f';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'g';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d",
                                  longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'h';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d",
                                  longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'i';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d %d",
                                  longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'j';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA(DEBUG, "%s %d %d %d %d %d %d %d %d",
                                  longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'l';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'm';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'n';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'o';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'p';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_TRACE("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'q';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'r';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 's';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 't';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_TRACE("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'v';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'w';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'x';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'y';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'z';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_DEBUG("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'A';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'B';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'C';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'D';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_DEBUG("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'F';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'G';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'H';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'I';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'J';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_INFO("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'K';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'L';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'M';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'N';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_INFO("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'P';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'Q';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'R';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'S';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'T';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_WARN("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'U';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'V';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'W';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'X';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_WARN("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'Z';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '0';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '1';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '2';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '3';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_ERROR("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '4';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '5';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '6';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '7';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_ERROR("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '!';
                longString[BUFLEN - 1] = '*';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL(OVERLY_LONG_STRING);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = '9';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s", longString);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'a';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d", longString, 2);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'b';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d", longString, 2, 3);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'c';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d %d", longString, 2, 3, 4);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'd';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 1;
                BALL_LOGVA_FATAL("%s %d %d %d %d", longString, 2, 3, 4, 5);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'e';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'f';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'g';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            {
                longString[BUFLEN - 2] = 'h';
                longString[BUFLEN - 1] = 'x';
                const int LINE = L_ + 2;
                BALL_LOGVA_FATAL("%s %d %d %d %d %d %d %d %d",
                                 longString, 2, 3, 4, 5, 6, 7, 8, 9);
                longString[BUFLEN - 1] = '\0';
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, longString));
            }

            delete [] longString;

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
                ASSERT(isRecordOkay(*TO, CAT, TRACE, FILE, LINE, MESSAGE[0]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_DEBUG(FORMAT_SPEC_1_ARGS, 1);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, DEBUG, FILE, LINE, MESSAGE[1]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_INFO(FORMAT_SPEC_2_ARGS, 1, 2);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, INFO, FILE, LINE, MESSAGE[2]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_WARN(FORMAT_SPEC_3_ARGS, 1, 2, 3);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, WARN, FILE, LINE, MESSAGE[3]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_ERROR(FORMAT_SPEC_4_ARGS, 1, 2, 3, 4);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, ERROR, FILE, LINE, MESSAGE[4]));
            }

            {
                const int LINE = L_ + 2;
                if (unbracketedLoggingFlag)  // *INTENTIONALLY* *NOT* '{}'ed
                    BALL_LOGVA_FATAL(FORMAT_SPEC_5_ARGS, 1, 2, 3, 4, 5);
                else
                    ++unbracketedLoggingFlag;
                ASSERT(isRecordOkay(*TO, CAT, FATAL, FILE, LINE, MESSAGE[5]));
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
                    THRESHOLD,
                    const_cast<Cat *>(CATEGORY),
                    0
                };

                LOOP3_ASSERT(LINE, THRESHOLD, BALL_LOG_THRESHOLD,
                             THRESHOLD == BALL_LOG_THRESHOLD);
                LOOP3_ASSERT(LINE, CATEGORY, BALL_LOG_CATEGORY,
                             CATEGORY == BALL_LOG_CATEGORY);
            }
        }

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc, &ta);

        {
            const char *CATEGORY_NAME1 = "EQUITY.NASD";
            BALL_LOG_SET_CATEGORY(CATEGORY_NAME1)  // creates new category

            const Cat  *CATEGORY1  = BALL_LOG_CATEGORYHOLDER.category();
            const int   MAX_LEVEL1 = CATEGORY1->maxLevel();

            LOOP2_ASSERT(CATEGORY_NAME1, CATEGORY1->categoryName(),
                         0 == bsl::strcmp(CATEGORY_NAME1,
                                          CATEGORY1->categoryName()));
            LOOP2_ASSERT(MAX_LEVEL1, BALL_LOG_CATEGORYHOLDER.threshold(),
                         MAX_LEVEL1 == BALL_LOG_CATEGORYHOLDER.threshold());

            BALL_LOG_TRACE << "This will load the category";

            BloombergLP::ball::Administration::setMaxNumCategories(3);
            ASSERT(3 == BloombergLP::ball::Administration::maxNumCategories());
            {
                const char *CATEGORY_NAME2 = "EQUITY.NYSE";
                BALL_LOG_SET_CATEGORY(CATEGORY_NAME2)

                const Cat  *CATEGORY2  = BALL_LOG_CATEGORYHOLDER.category();
                const int   MAX_LEVEL2 = CATEGORY2->maxLevel();

                LOOP2_ASSERT(CATEGORY_NAME2, CATEGORY2->categoryName(),
                             0 == bsl::strcmp(CATEGORY_NAME2,
                                          CATEGORY2->categoryName()));
                LOOP2_ASSERT(MAX_LEVEL2, BALL_LOG_CATEGORYHOLDER.threshold(),
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

                    LOOP2_ASSERT(CATEGORY_NAME3, CATEGORY3->categoryName(),
                                 0 != bsl::strcmp(CATEGORY_NAME3,
                                                  CATEGORY3->categoryName()));
                    LOOP2_ASSERT(MAX_LEVEL3,
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
        //   (1) Verify that 'messageBuffer' returns non-null and that
        //       'messageBufferSize' bytes may be overwritten
        //
        // Testing:
        //   void logMessage(*category, severity, *file, line, *msg);
        //   char *messageBuffer();
        //   int messageBufferSize();
        //   const ball::Category *setCategory(Holder *, const char *);
        //   const ball::Category *setCategory(const char *categoryName);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl << "Testing Utility Functions"
                               << bsl::endl << "========================="
                               << bsl::endl;

        using namespace BloombergLP;  // okay here

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard lmg(TO, lmc);
        BloombergLP::ball::LoggerManager& lm =
                                 BloombergLP::ball::LoggerManager::singleton();

        if (veryVerbose) {
            bsl::cout << "\tTesting 'messageBuffer' and 'messageBufferSize'"
                      << bsl::endl;
        }

        const ball::Category& defaultCategory       = lm.defaultCategory();
        const int             DEFAULT_CAT_MAX_LEVEL =
                                                    defaultCategory.maxLevel();

        lm.setDefaultThresholdLevels(192, 96, 64, 32);
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

            static Holder mH = { Holder::e_UNINITIALIZED_CATEGORY, 0, 0 };
            const Holder& H = mH;

            ball::Log::setCategory(&mH, "EQUITY.NYSE"); // creates new category
            ASSERT(  0 == bsl::strcmp("EQUITY.NYSE",
                                      H.category()->categoryName()));
            ASSERT(192 == H.threshold());

            static Holder mH2 = { Holder::e_UNINITIALIZED_CATEGORY, 0, 0 };
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

             const int NREC = TO->numPublishedRecords();
             ball::Log::logMessage(CAT, SEV, FILE, LINE, MSG);
             ASSERT(NREC + 1 == TO->numPublishedRecords());
             ASSERT(isRecordOkay(*TO, CAT, SEV, FILE, LINE, MSG));
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
        using namespace BloombergLP;  // okay here

        ball::StreamObserver observer(&bsl::cout);
        ball::LoggerManager::initSingleton(&observer, 0);

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
