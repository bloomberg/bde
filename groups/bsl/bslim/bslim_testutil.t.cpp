// bslim_testutil.t.cpp                                               -*-C++-*-
#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_sstream.h>

#include <fcntl.h>
#include <limits.h>     // 'PATH_MAX' on Linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // 'strlen'

#include <sys/types.h>  // for 'struct stat'; required on Sun and Windows only
#include <sys/stat.h>   // (ditto)

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# include <windows.h>   // 'MAX_PATH'
# include <io.h>        // '_dup2'
# define snprintf _snprintf
#else
# include <unistd.h>
#endif

// Ensure that the 'BSLIM_TESTUTIL_*' macros do not require:
//  using namespace bsl;

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a set of macros that write to the
// standard output ('bsl::cout'), and a set of overloads for the output
// operator ('<<') to write objects of the 'bsltf' test types to output
// streams.
//
// The macros provided mirror the standard test macros normally used in test
// drivers.  The intention is that the standard test macros should be
// implemented as aliases of the 'BSLIM_TESTUTIL_*' macros, as illustrated in
// the first usage example.  As a result, the identifiers normally used in a
// test driver conflict with the identifiers used in the usage example.
// Therefore, this test driver avoids the standard test macros (and support
// functions), and uses the following instead:
//..
//      STANDARD        bslim_testutil.t.cpp
//  ----------------    --------------------
//  LOOP_ASSERT         REALLOOP1_ASSERT
//  LOOP2_ASSERT        REALLOOP2_ASSERT
//  LOOP3_ASSERT        REALLOOP3_ASSERT
//  LOOP4_ASSERT        REALLOOP4_ASSERT
//  LOOP5_ASSERT        not used
//  LOOP6_ASSERT        not used
//  Q                   not used
//  P                   REALP
//  P_                  REALT_
//  T_                  REALT_
//  L_                  not used
//  'void aSsErT()'     'void realaSsErT()'
//  'int testStatus'    'int realTestStatus'
//..
// All of the methods write output to 'stdout' and do nothing more.  Therefore,
// there are no Primary Manipulators or Basic Accessors to test.  All of the
// tests fall under the category of Print Operations.
//
// The main difficulty with writing this test driver is capturing the output of
// the methods under test so that it can be checked for accuracy.  In addition,
// error messages and other output produced by the test driver itself must
// still appear on 'stdout' for compatibility with the standard building and
// testing scripts.  For this purpose, a support class named 'OutputRedirector'
// is provided.  'OutputRedirector' redirects 'stdout' to a temporary file and
// verifies that the contents of the temporary file match the character buffers
// provided by the user.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 7] bool compareText(StringRef, StringRef, bsl::ostream&);
// [ 9] void *callFunc(void *arg);
// [ 9] void setFunc(Func func);
//
// MACROS
// [ 6] BSLIM_TESTUTIL_ASSERT(X)
// [ 6] BSLIM_TESTUTIL_LOOP_ASSERT(I,X)
// [ 6] BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)
// [ 6] BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)
// [ 6] BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
// [ 6] BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
// [ 6] BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
// [ 6] BSLIM_TESTUTIL_ASSERTV(...)
// [ 5] BSLIM_TESTUTIL_Q(X)
// [ 4] BSLIM_TESTUTIL_P(X)
// [ 4] BSLIM_TESTUTIL_P_(X)
// [ 3] BSLIM_TESTUTIL_L_
// [ 3] BSLIM_TESTUTIL_T_
//
// FREE OPERATORS
// [ 8] ostream& operator<<(str, const AllocBitwiseMoveableTestType&);
// [ 8] ostream& operator<<(str, const AllocTestType&);
// [ 8] ostream& operator<<(str, const BitwiseCopyableTestType&);
// [ 8] ostream& operator<<(str, const BitwiseMoveableTestType&);
// [ 8] ostream& operator<<(str, const EnumeratedTestType::Enum&);
// [ 8] ostream& operator<<(str, const MovableAllocTestType&);
// [ 8] ostream& operator<<(str, const MovableTestType&);
// [ 8] ostream& operator<<(str, const MoveOnlyAllocTestType&);
// [ 8] ostream& operator<<(str, const NonAssignableTestType&);
// [ 8] ostream& operator<<(str, const NonCopyConstructibleTestType&);
// [ 8] ostream& operator<<(str, const NonDefaultConstructibleTestType&);
// [ 8] ostream& operator<<(str, const NonEqualComparableTestType&);
// [ 8] ostream& operator<<(str, const NonTypicalOverloadsTestType&);
// [ 8] ostream& operator<<(str, const SimpleTestType&);
// [ 8] ostream& operator<<(str, const UnionTestType&);
//-----------------------------------------------------------------------------
// [10] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS

// ============================================================================
//                VARIATIONS ON STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// In order to accommodate the use of the identifiers 'testStatus' and 'aSsErT'
// in the usage example, the rest of the test driver uses the identifiers
// 'realTestStatus' and 'realaSsErT' instead.
//
// Additionally, in order to allow capturing the output of the
// 'BSLIM_TESTUTIL_*' macros, the standard macros output to 'stderr' instead of
// 'stdout'.

static int realTestStatus = 0;

static void realaSsErT(bool b, const char *s, int i)
{
    if (b) {
        bsl::cerr << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (realTestStatus >= 0 && realTestStatus <= 100) ++realTestStatus;
    }
}
// The standard 'ASSERT' macro definition is deferred until after the usage
// example code:
//..
// #define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }
//..

#define REALLOOP_ASSERT(I,X)                                                  \
    if (!(X)) { bsl::cerr << #I << ": " << I << "\n";                         \
                realaSsErT(1, #X, __LINE__); }

#define REALLOOP2_ASSERT(I,J,X)                                               \
    if (!(X)) { bsl::cerr << #I << ": " << I << "\t"                          \
                          << #J << ": " << J << "\n";                         \
                realaSsErT(1, #X, __LINE__); }

#define REALLOOP3_ASSERT(I,J,K,X)                                             \
    if (!(X)) { bsl::cerr << #I << ": " << I << "\t"                          \
                          << #J << ": " << J << "\t"                          \
                          << #K << ": " << K << "\n";                         \
                realaSsErT(1, #X, __LINE__); }

#define REALLOOP4_ASSERT(I,J,K,L,X)                                           \
    if (!(X)) { bsl::cerr << #I << ": " << I << "\t"                          \
                          << #J << ": " << J << "\t"                          \
                          << #K << ": " << K << "\t"                          \
                          << #L << ": " << L << "\n";                         \
                realaSsErT(1, #X, __LINE__); }

#define REALP(X)                                                              \
    bsl::cerr << #X " = " << (X) << bsl::endl;

#define REALP_(X)                                                             \
    bsl::cerr << #X " = " << (X) << ", " << bsl::flush;

#define REALT_                                                                \
    bsl::cerr << "\t" << bsl::flush;

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------

namespace {

///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write an elided component to test, which provides a utility class:
//..
    namespace bdlabc {

    struct ExampleUtil {
        // This utility class provides sample functionality to demonstrate how
        // a test driver might be written validating its only method.

        // CLASS METHODS
        static int fortyTwo();
            // Return the integer value 42.
    };

    // CLASS METHODS
    inline
    int ExampleUtil::fortyTwo()
    {
        return 42;
    }

    }  // close namespace bdlabc
//..
// Then, we can write an elided test driver for this component.  We start by
// providing the standard BDE assert test macro:
//..
    //=========================================================================
    //                       STANDARD BDE ASSERT TEST MACRO
    //-------------------------------------------------------------------------
    static int testStatus = 0;

    static void aSsErT(int c, const char *s, int i)
    {
        if (c) {
            bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                      << "    (failed)" << bsl::endl;
            if (testStatus >= 0 && testStatus <= 100) ++testStatus;
        }
    }
//..
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
    //=========================================================================
    //                       STANDARD BDE TEST DRIVER MACROS
    //-------------------------------------------------------------------------
    #define ASSERT       BSLIM_TESTUTIL_ASSERT
    #define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
    #define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
    #define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
    #define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
    #define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
    #define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
    #define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
    #define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
    #define ASSERTV      BSLIM_TESTUTIL_ASSERTV

    #define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
    #define P   BSLIM_TESTUTIL_P   // Print identifier and value.
    #define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
    #define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
    #define L_  BSLIM_TESTUTIL_L_  // current Line number

//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in the test driver:
//..
    void testFortyTwo(bool verbose)
        // Test 'bdlabc::ExampleUtil::fortyTwo' in the specified 'verbose'
        // verbosity level.
    {
        const int value = bdlabc::ExampleUtil::fortyTwo();
        if (verbose) P(value);
        LOOP_ASSERT(value, 42 == value);
    }
//..
// Finally, when 'testFortyTwo' is called from a test case in verbose mode we
// observe the console output:
//..
//  value = 42
//..

}  // close unnamed namespace

//=============================================================================
//                    UNDEFINE STANDARD TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#undef ASSERT

#undef LOOP_ASSERT
#undef LOOP2_ASSERT
#undef LOOP3_ASSERT
#undef LOOP4_ASSERT
#undef LOOP5_ASSERT
#undef LOOP6_ASSERT
#undef ASSERTV

#undef Q
#undef P
#undef P_
#undef T_
#undef L_

#define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslim::TestUtil Obj;

enum { FORMAT_STRING_SIZE = 256 };  // Size of temporary format string buffers
                                    // used for output formatting.

enum {
    // Enumeration defining sizes for the buffers used by the output
    // redirection apparatus.

    OUTPUT_BUFFER_SIZE = 4096,

#ifdef BSLS_PLATFORM_OS_WINDOWS
    PATH_BUFFER_SIZE   = MAX_PATH
#elif defined(BSLS_PLATFORM_OS_HPUX)
    PATH_BUFFER_SIZE   = L_tmpnam
#else
    PATH_BUFFER_SIZE   = PATH_MAX
#endif
};

static bool verbose, veryVerbose, veryVeryVerbose;

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BSLIM_TESTUTIL_TEST_FUNCTION_CALL {

int callCount = 0;

void *testFunctionAdd(void *arg)
    // Increment call counter and return the specified 'arg'.
{
    ++callCount;
    return arg;
}

void *testFunctionSub(void *arg)
    // Decrement call counter and return the specified 'arg'.
{
    --callCount;
    return arg;
}
}  // close namespace BSLIM_TESTUTIL_TEST_FUNCTION_CALL

namespace {

bool tempFileName(char *result)
    // Create a temporary file and load its name into the user-supplied buffer
    // at the address pointed to by the specified 'result'.  Return 'true' if
    // the temporary file was successfully created, and 'false' otherwise.  The
    // behavior is undefined unless the buffer pointed to by 'result' is at
    // least 'PATH_BUFFER_SIZE' bytes long.
{
    ASSERT(result);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    char tmpPathBuf[MAX_PATH];
    if (!GetTempPath(MAX_PATH, tmpPathBuf)
     || !GetTempFileName(tmpPathBuf, "bslim", 0, result)) {
        return false;                                                 // RETURN
    }
#elif defined(BSLS_PLATFORM_OS_HPUX)
    if(!tempnam(result, "bslim")) {
        return false;                                                 // RETURN
    }
#else
    char *fn = tempnam(0, "bslim");
    if (fn) {
        strncpy(result, fn, PATH_BUFFER_SIZE);
        result[PATH_BUFFER_SIZE - 1] = '\0';
        free(fn);
    } else {
        return false;                                                 // RETURN
    }
#endif

    if (veryVerbose) printf("\tUse '%s' as a base filename.\n", result);

    ASSERT('\0' != result[0]);  // result is not empty

    return true;
}

class OutputRedirector {
    // This class provides a facility for redirecting 'stdout' to a temporary
    // file, retrieving output from the temporary file, and comparing the
    // output to user-supplied character buffers.  An 'OutputRedirector' object
    // can exist in one of two states, un-redirected or redirected.  In the
    // un-redirected state, the process' 'stdout' and 'stderr' are connected to
    // their normal targets.  In the redirected state, the process' 'stdout' is
    // connected to a temporary file, and the process' 'stderr' is connected to
    // the original target of 'stdout'.  The redirected state of an
    // 'OutputRedirector' object can be tested by calling 'isRedirected'.  An
    // 'OutputRedirector' object has the concept of a scratch buffer, where
    // output captured from the process' 'stdout' stream is stored when the
    // 'OutputRedirector' object is in the redirected state.  Throughout this
    // class, the term "captured output" refers to data that has been written
    // to the 'stdout' stream and is waiting to be loaded into the scratch
    // buffer.  Each time the 'load' method is called, the scratch buffer is
    // truncated, and the captured output is moved into the scratch buffer.
    // When this is done, there is no longer any captured output.

    // DATA
    char d_fileName[PATH_BUFFER_SIZE];        // name of temporary capture file

    char d_outputBuffer[OUTPUT_BUFFER_SIZE];  // scratch buffer for holding
                                              // captured output

    bool d_isRedirectedFlag;                  // has 'stdout' been redirected

    bool d_isFileCreatedFlag;                 // has a temp file been created

    bool d_isOutputReadyFlag;                 // has output been read from the
                                              // temp file

    long d_outputSize;                        // size of output loaded into
                                              // 'd_outputBuffer'

    struct stat d_originalStdoutStat;         // status information for
                                              // 'stdout' just before
                                              // redirection

  private:
    // PRIVATE CLASS METHODS
    static int redirectStream(FILE *from, FILE *to);
        // Redirect the specified 'from' stream to the specified 'to' stream.
        // Return 0 on success, and a negative value otherwise.

    // PRIVATE MANIPULATORS
    void cleanup();
        // Close 'stdout', if redirected, and delete the temporary output
        // capture file.

  private:
    // NOT IMPLEMENTED
    OutputRedirector(const OutputRedirector&);
    OutputRedirector& operator=(const OutputRedirector&);

  public:
    // CREATORS
    OutputRedirector();
        // Create an 'OutputRedirector' in an un-redirected state having an
        // empty scratch buffer.

    ~OutputRedirector();
        // Destroy this 'OutputRedirector' object.  If the object is in a
        // redirected state, 'stdout' will be closed and the temporary file to
        // which 'stdout' was redirected will be removed.

    // MANIPULATORS
    char *buffer();
        // Return the address providing modifiable access to the scratch
        // buffer.  Note that this method is used only for error reporting and
        // to test the correctness of 'OutputRedirector'.

    bool load();
        // Read captured output into the scratch buffer.  Return 'true' if all
        // captured output was successfully loaded, and 'false' otherwise.  The
        // behavior is undefined unless 'redirect' has been previously called
        // successfully.  Note that captured output is allowed to have 0
        // length.

    void redirect();
        // Redirect 'stdout' to a temp file, and 'stderr' to the original
        // 'stdout', putting this 'OutputRedirector' object into the
        // redirected state.  The temp file to which 'stdout' is redirected
        // will be created the first time 'redirect' is called, and will be
        // removed when this object is destroyed.  Subsequent calls to this
        // method no effect on 'stdout' and 'stderr'.  If this method fails to
        // redirect either 'stdout' or 'stderr', 'std::abort' is called.

    void reset();
        // Reset the scratch buffer to empty.  The behavior is undefined unless
        // 'redirect' has been previously called successfully.

    // ACCESSORS
    int compare(const char *expected) const;
        // Compare the specified 'expected' character buffer with any output
        // that has been loaded into the scratch buffer.  'expected' is assumed
        // to be an NTBS, and its length is taken to be the string length of
        // the NTBS.  Return 0 if 'expected' has the same length and contents
        // as the scratch buffer, and non-zero otherwise.  The behavior is
        // undefined unless 'redirect' has been previously called successfully.

    int compare(const char *expected, size_t expectedLength) const;
        // Compare the specified 'expected' character buffer of the specified
        // 'expectedLength' with any output that has been loaded into the
        // scratch buffer.  Return 0 if 'expected' has the same length and
        // contents as the scratch buffer, and non-zero otherwise.  The
        // behavior is undefined unless 'redirect' has been previously called
        // successfully.  Note that 'expected' may contain embedded null
        // characters.

    bool isOutputReady() const;
        // Return 'true' if captured output been loaded into the scratch
        // buffer, and 'false' otherwise.

    bool isRedirected() const;
        // Return 'true' if 'stdout' and 'stderr' have been successfully
        // redirected, and 'false' otherwise.

    const struct stat& originalStdoutStat() const;
        // Return a reference providing non-modifiable access to the status
        // information for 'stdout' collected just before redirection.  Note
        // that this method is used only to test the correctness of
        // 'OutputRedirector'.

    size_t outputSize() const;
        // Return the number of bytes currently loaded into the scratch buffer.
};

// PRIVATE CLASS METHODS
int OutputRedirector::redirectStream(FILE *from, FILE *to)
{
    ASSERT(from);
    ASSERT(to);

    // The canonical way to redirect 'stderr' to 'stdout' is:
    //..
    //  freopen("/dev/stdout", "w", stderr);
    //..
    // but we use 'dup2' instead of 'freopen', because 'freopen' fails on AIX
    // with 'errno == 13' ("Permission denied") when redirecting 'stderr'.

#if defined(BSLS_PLATFORM_OS_AIX)
    int redirected = dup2(fileno(from), fileno(to));
    return redirected == fileno(to) ? 0 : -1;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    return _dup2(_fileno(from), _fileno(to));
#else
    return (stderr == freopen("/dev/stdout", "w", stderr)) ? 0 : -1;
#endif
}

// PRIVATE MANIPULATORS
void OutputRedirector::cleanup()
{
    if (d_isRedirectedFlag) {
        fclose(stdout);
    }

    if (d_isFileCreatedFlag) {
        unlink(d_fileName);
    }
}

// CREATORS
OutputRedirector::OutputRedirector()
: d_isRedirectedFlag(false)
, d_isFileCreatedFlag(false)
, d_isOutputReadyFlag(false)
, d_outputSize(0L)
{
    d_fileName[0] = '\0';
    memset(&d_originalStdoutStat, 0, sizeof(struct stat));
}

OutputRedirector::~OutputRedirector()
{
    cleanup();
}

// MANIPULATORS
char *OutputRedirector::buffer()
{
    return d_outputBuffer;
}

bool OutputRedirector::load()
{
    ASSERT(d_isRedirectedFlag);
    ASSERT(!ferror(stdout));

    d_outputSize = ftell(stdout);

    if (d_outputSize + 1 > OUTPUT_BUFFER_SIZE) {

        // Refuse to load output if it will not all fit in the scratch buffer.

        if (veryVerbose) {
            REALP(d_outputSize);
            fprintf(stderr,
                    "Error "
                        __FILE__
                        "(%d): Captured output exceeds read buffer size\n",
                    __LINE__);
        }
        d_outputSize = 0L;
        return false;                                                 // RETURN
    }

    rewind(stdout);

    long charsRead = fread(d_outputBuffer, sizeof(char), d_outputSize, stdout);

    if (d_outputSize != charsRead) {

        // We failed to read all output from the capture file.

        if (veryVerbose) {
            REALP_(d_outputSize); REALP(charsRead);
            if (ferror(stdout)) {

                // We encountered a file error, not 'EOF'.

                perror("\tError message: ");
                clearerr(stdout);
            }
            fprintf(stderr,
                    "Error "
                        __FILE__
                        "(%d): Could not read all captured output\n",
                    __LINE__);
        }

        d_outputBuffer[charsRead] = '\0';

            // ... to ensure that direct inspection of buffer does not overflow

        return false;                                                 // RETURN
    } else {

        // We have read all output from the capture file.

        d_outputBuffer[d_outputSize] = '\0';

        // ... to ensure that direct inspection of buffer does not overflow
    }

    d_isOutputReadyFlag = true;

    return true;
}

void OutputRedirector::redirect()
{
    if (d_isRedirectedFlag) {

        // Do not redirect anything if we have already redirected.

        if (veryVerbose) {
            fprintf(stdout,
                    "Warning " __FILE__ "(%d): Output already redirected\n",
                    __LINE__);
        }

        return;                                                       // RETURN
    }

    // Retain information about the original 'stdout' file descriptor for use
    // in later tests.

    int originalStdoutFD = fileno(stdout);
    ASSERT(-1 != originalStdoutFD);
    ASSERT(0 == fstat(originalStdoutFD, &d_originalStdoutStat));

    if (0 != redirectStream(stderr, stdout)) {
        // Redirect 'stderr' to 'stdout'.

        // We want 'stderr' to point to 'stdout', so we have to redirect it
        // before we change the meaning of 'stdout'.

        if (veryVerbose) {

            // Note that we print this error message on 'stdout' instead of
            // 'stderr', because 'stdout' has not been redirected.

            fprintf(stdout,
                    "Error " __FILE__ "(%d): Failed to redirect stderr\n",
                    __LINE__);
        }
        std::abort();
    }

    if (!tempFileName(d_fileName)) {

        // Get temp file name.

        if (veryVerbose) {

            // Note that we print this error message on 'stdout' instead of
            // 'stderr', because 'stdout' has not been redirected.

            fprintf(stdout,
                    "Error "
                    __FILE__
                    "(%d): Failed to get temp file name for stdout capture\n",
                    __LINE__);
        }
        std::abort();
    }

    if (!freopen(d_fileName, "w+", stdout)) {

        // Redirect 'stdout'.

        if (veryVerbose) {

            // Note that we print this error message on 'stderr', because we
            // have just redirected 'stdout' to the capture file.

            REALP(d_fileName);
            fprintf(stderr,
                    "Error " __FILE__ "(%d): Failed to redirect stdout\n",
                    __LINE__);
        }
        cleanup();
        std::abort();
    }

    // 'stderr' and 'stdout' have been successfully redirected.

#if defined(BSLS_PLATFORM_OS_WINDOWS)
    if (-1 == _setmode(_fileno(stdout), _O_BINARY)) {
        ASSERT(0 == "Failed to set stdout to binary mode.");
        cleanup();
        std::abort();
    }
#endif

    d_isFileCreatedFlag = true;
    d_isRedirectedFlag  = true;

    if (EOF == fflush(stdout)) {

        // Not flushing 'stdout' is not a fatal condition, so we print out a
        // warning, but do not abort.

        if (veryVerbose) {

            // Note that we print this error message on 'stderr', because we
            // have just redirected 'stdout' to the capture file.

            perror("Error message: ");
            fprintf(stderr,
                    "Warning " __FILE__ "(%d): Error flushing stdout\n",
                    __LINE__);
        }
    }
}

void OutputRedirector::reset()
{
    ASSERT(d_isRedirectedFlag);

    d_outputSize        = 0L;
    d_isOutputReadyFlag = false;
    d_outputBuffer[0]   = '\0';
    rewind(stdout);
}

// ACCESSORS
int OutputRedirector::compare(const char *expected) const
{
    ASSERT(expected);

    return compare(expected, strlen(expected));
}

int OutputRedirector::compare(const char *expected,
                              size_t      expectedLength) const
{
    ASSERT(d_isRedirectedFlag);
    ASSERT(expected || !expectedLength);

    if (!d_isOutputReadyFlag) {
        if (veryVerbose) {
            REALP(expected);
            fprintf(stderr,
                    "Error " __FILE__ "(%d): No captured output available\n",
                    __LINE__);
        }
        return -1;                                                    // RETURN
    }

    // Use 'memcmp' instead of 'strncmp' to compare 'd_outputBuffer' with
    // 'expected' because 'expected' may contain embedded null characters.

    return d_outputSize != static_cast<long>(expectedLength)
        || memcmp(d_outputBuffer, expected, expectedLength);
}

bool OutputRedirector::isOutputReady() const
{
    return d_isOutputReadyFlag;
}

bool OutputRedirector::isRedirected() const
{
    return d_isRedirectedFlag;
}

const struct stat& OutputRedirector::originalStdoutStat() const
{
    return d_originalStdoutStat;
}

size_t OutputRedirector::outputSize() const
{
    return static_cast<size_t>(d_outputSize);
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test        = argc > 1 ? atoi(argv[1]) : 0;
    verbose         = argc > 2;
    veryVerbose     = argc > 3;
    veryVeryVerbose = argc > 4;

    // Capture 'stdout', and send 'stderr' to 'stdout', unless we are running
    // the usage example.
    OutputRedirector output;
    if (test != 8 && test != 0) {
        output.redirect();
    }

    bsl::cerr << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------


        if (verbose) bsl::cerr << bsl::endl
                               << "USAGE EXAMPLE" << bsl::endl
                               << "=============" << bsl::endl;

        testFortyTwo(verbose);

///Example 2: Print The Value of A Test Type
///- - - - - - - - - - - - - - - - - - - - -
// Suppose we want to print the value of an object of a test type defined the
// 'bsltf' package using 'bsl::cout'.  This component supplies the necessary
// overloads of the insertion operator for this to be done directly.
//
// First, include the header of this component:
//..
//  #include <bslim_testutil.h>
//..
// Now, we construct a 'bsltf::SimpleTestType' object and stream its value to
// 'bsl::cout' using the '<<' operator:
//..
    bsltf::SimpleTestType a(10);
    bsl::cout << a;
//..
// Finally, we observe the following console output:
//..
//  10
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'callFunc' AND 'setFunc' METHODS
        //
        // Concerns:
        //: 1 A custom function with external linkage can be installed and
        //:   later called via 'bslim' test utility component.
        //
        // Plan:
        //: 1 Install custom function using 'setFunc' method.
        //:
        //: 2 Invoke previously installed function and verify the result of
        //:   its invocation.
        //
        // Testing:
        //   void *callFunc(void *arg);
        //   void setFunc(Func func);
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTESTING 'callFunc' AND 'setFunc' METHODS"
                               << "\n========================================"
                               << bsl::endl;

        using namespace BSLIM_TESTUTIL_TEST_FUNCTION_CALL;

        ASSERT(0 == callCount);

        Obj::setFunc(testFunctionAdd);

        ASSERT(0 == callCount);

        int   refValue;
        void *INPUT = &refValue;
        void *result = Obj::callFunc(INPUT);

        ASSERT(1 == callCount);
        ASSERT(&refValue == result);

        result = Obj::callFunc(INPUT);

        ASSERT(2 == callCount);
        ASSERT(&refValue == result);

        Obj::setFunc(testFunctionSub);

        ASSERT(2 == callCount);

        result = Obj::callFunc(INPUT);

        ASSERT(1 == callCount);
        ASSERT(&refValue == result);

        result = Obj::callFunc(INPUT);

        ASSERT(0 == callCount);
        ASSERT(&refValue == result);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // OUTPUT ('<<') OPERATORS
        //
        // Concerns:
        //: 1 The output operator correctly streams the value of an object of
        //:   any test type in the 'bsltf' package into a specified stream.
        //
        // Plan:
        //: 1 For each 'bsltf' test type, create an object of such a type
        //:   having a unique value and stream its value into a
        //:   'bsl::ostringstream'.  Verify that the string output to the
        //:   stream has the expected value.  (C-1)
        //
        // Testing:
        //   ostream& operator<<(str, const AllocBitwiseMoveableTestType&);
        //   ostream& operator<<(str, const AllocTestType&);
        //   ostream& operator<<(str, const BitwiseCopyableTestType&);
        //   ostream& operator<<(str, const BitwiseMoveableTestType&);
        //   ostream& operator<<(str, const EnumeratedTestType::Enum&);
        //   ostream& operator<<(str, const MovableAllocTestType&);
        //   ostream& operator<<(str, const MovableTestType&);
        //   ostream& operator<<(str, const MoveOnlyAllocTestType&);
        //   ostream& operator<<(str, const NonAssignableTestType&);
        //   ostream& operator<<(str, const NonCopyConstructibleTestType&);
        //   ostream& operator<<(str, const NonDefaultConstructibleTestType&);
        //   ostream& operator<<(str, const NonEqualComparableTestType&);
        //   ostream& operator<<(str, const NonTypicalOverloadsTestType&);
        //   ostream& operator<<(str, const SimpleTestType&);
        //   ostream& operator<<(str, const UnionTestType&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "OUTPUT ('<<') OPERATORS" << bsl::endl
                               << "=======================" << bsl::endl;

        using namespace BloombergLP::bsltf;

        AllocBitwiseMoveableTestType o1 =
                 TemplateTestFacility::create<AllocBitwiseMoveableTestType>(1);

        AllocTestType o2 = TemplateTestFacility::create<AllocTestType>(2);

        BitwiseCopyableTestType o3 =
                      TemplateTestFacility::create<BitwiseCopyableTestType>(3);

        BitwiseMoveableTestType o4 =
                      TemplateTestFacility::create<BitwiseMoveableTestType>(4);

        EnumeratedTestType::Enum o5 =
                     TemplateTestFacility::create<EnumeratedTestType::Enum>(5);

        MovableAllocTestType o6 =
                         TemplateTestFacility::create<MovableAllocTestType>(6);

        MovableTestType o7 = TemplateTestFacility::create<MovableTestType>(7);

        MoveOnlyAllocTestType o8(8);

        NonAssignableTestType o9 =
                        TemplateTestFacility::create<NonAssignableTestType>(9);

        NonCopyConstructibleTestType o10(10);

        NonDefaultConstructibleTestType o11 =
             TemplateTestFacility::create<NonDefaultConstructibleTestType>(11);

        NonEqualComparableTestType o12 =
                  TemplateTestFacility::create<NonEqualComparableTestType>(12);

        NonTypicalOverloadsTestType o13 =
                 TemplateTestFacility::create<NonTypicalOverloadsTestType>(13);

        SimpleTestType o14 = TemplateTestFacility::create<SimpleTestType>(14);

        UnionTestType o15 = TemplateTestFacility::create<UnionTestType>(15);

        bsl::ostringstream oss;

        oss << o1;
        ASSERT( "1" == oss.str());

        oss.str("");
        oss << o2;
        ASSERT( "2" == oss.str());

        oss.str("");
        oss << o3;
        ASSERT( "3" == oss.str());

        oss.str("");
        oss << o4;
        ASSERT( "4" == oss.str());

        oss.str("");
        oss << o5;
        ASSERT( "5" == oss.str());

        oss.str("");
        oss << o6;
        ASSERT( "6" == oss.str());

        oss.str("");
        oss << o7;
        ASSERT( "7" == oss.str());

        oss.str("");
        oss << o8;
        ASSERT( "8" == oss.str());

        oss.str("");
        oss << o9;
        ASSERT( "9" == oss.str());

        oss.str("");
        oss << o10;
        ASSERT("10" == oss.str());

        oss.str("");
        oss << o11;
        ASSERT("11" == oss.str());

        oss.str("");
        oss << o12;
        ASSERT("12" == oss.str());

        oss.str("");
        oss << o13;
        ASSERT("13" == oss.str());

        oss.str("");
        oss << o14;
        ASSERT("14" == oss.str());

        oss.str("");
        oss << o15;
        ASSERT("15" == oss.str());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'compareText'
        //
        // Concerns:
        //: 1 The function returns 'true' if the strings are both empty
        //:
        //: 2 The function returns 'true' if the strings have the same value
        //:
        //: 3 The function returns 'false' if the left string is shorter
        //:   than the right.
        //:
        //: 4 The function returns 'false' if the right string is shorter
        //:   than the left.
        //:
        //: 5 The function returns 'false' if the strings are the same length
        //:   but differ in their contents
        //:
        //: 6 The function writes a message to the supplied stream containing
        //:   information about the character where they differ, if the two
        //:   strings are not the same.
        //:
        //: 7 If no stream is explicitly supplied, error messages are written
        //:   to 'stdout'.
        //:
        //: 8 The function correctly handles embedded non-ascii and null
        //:   characters.
        //
        // Plan:
        //: 1 Call 'compareText' with a data set containing a variety of
        //:   strings, and compare the results to 'strcmp'.  (C-1-6)
        //:
        //: 2 Call 'compareText' with a data set containing embedded nulls and
        //:   compare the results against
        //:   'bslstl::StringRef::operator=='. (C-8)
        //:
        //: 3 Call 'compareText' with types of un-equal strings that will
        //:   produce different error messages, and verify the produced error
        //:   message meets the expected error message. (C-7)
        //
        // Testing:
        //   bool compareText(StringRef, StringRef, bsl::ostream&);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "TESTING 'compareText'" << bsl::endl
                               << "=====================" << bsl::endl;

        if (verbose) bsl::cout << "Compare against 'strcmp'." << bsl::endl;
        {
            const char *DATA[] = {
                "",
                "a",
                "z",
                "A",
                "Z",
                "\x01",
                "\x20",
                "\x40",
                "\x60",
                "\x80",
                "\xA0",
                "\xC0",
                "\xE0",
                "\xF0",
                " a",
                " z",
                " A",
                " Z",
                " \x01",
                " \x50",
                " \xA0",
                " \xF0",
                "aa",
                "az",
                "aA",
                "aZ",
                "a\x01",
                "a\x50",
                "a\xA0",
                "a\xF0",
                "Za",
                "Zz",
                "ZA",
                "ZZ",
                "Z\x01",
                "Z\x50",
                "Z\xA0",
                "Z\xF0",
                "\xA0""a",
                "\xA0""z",
                "\xA0""A",
                "\xA0""Z",
                "\xA0""\x01",
                "\xA0""\x50",
                "\xA0""\xA0""",
                "\xA0""\xF0",
                "  a",
                "  z",
                "  A",
                "  Z",
                "  \x01",
                "  \x50",
                "  \xA0",
                "  \xF0",
                "aaa",
                "aaz",
                "aaA",
                "aaZ",
                "aa\x01",
                "aa\x50",
                "aa\xA0",
                "aa\xF0",
                "aZa",
                "aZz",
                "aZA",
                "aZZ",
                "aZ\x01",
                "aZ\x50",
                "aZ\xA0",
                "aZ\xF0",
                "a\xA0""a",
                "a\xA0""z",
                "a\xA0""A",
                "a\xA0""Z",
                "a\xA0""\x01",
                "a\xA0""\x50",
                "a\xA0""\xA0",
                "a\xA0""\xF0",
                "a a",
                "a z",
                "a A",
                "a Z",
                "a \x01",
                "a \x50",
                "a \xA0",
                "a \xF0",
                "aaa",
                "aaz",
                "aaA",
                "aaZ",
                "aa\x01",
                "aa\x50",
                "aa\xA0",
                "aa\xF0",
                "aZa",
                "aZz",
                "aZA",
                "aZZ",
                "aZ\x01",
                "aZ\x50",
                "aZ\xA0",
                "aZ\xF0",
                "a\xA0""a",
                "a\xA0""z",
                "a\xA0""A",
                "a\xA0""Z",
                "a\xA0""\x01",
                "a\xA0""\x50",
                "a\xA0""\xA0",
                "a\xA0""\xF0",
                "aaa",
                "aaz",
                "aaA",
                "aaZ",
                "aa\x01",
                "aa\x50",
                "aa\xA0",
                "aa\xF0",
                "aZa",
                "aZz",
                "aZA",
                "aZZ",
                "aZ\x01",
                "aZ\x50",
                "aZ\xA0",
                "aZ\xF0",
                "a\xA0""a",
                "a\xA0""z",
                "a\xA0""A",
                "a\xA0""Z",
                "a\xA0""\x01",
                "a\xA0""\x50",
                "a\xA0""\xA0",
                "a\xA0""\xF0",
                "\xF0"" a",
                "\xF0"" z",
                "\xF0"" A",
                "\xF0"" Z",
                "\xF0"" \x01",
                "\xF0"" \x50",
                "\xF0"" \xA0",
                "\xF0"" \xF0",
                "\xF0""aa",
                "\xF0""az",
                "\xF0""aA",
                "\xF0""aZ",
                "\xF0""a\x01",
                "\xF0""a\x50",
                "\xF0""a\xA0",
                "\xF0""a\xF0",
                "\xF0""Za",
                "\xF0""Zz",
                "\xF0""ZA",
                "\xF0""ZZ",
                "\xF0""Z\x01",
                "\xF0""Z\x50",
                "\xF0""Z\xA0",
                "\xF0""Z\xF0",
                "\xF0""\xA0""a",
                "\xF0""\xA0""z",
                "\xF0""\xA0""A",
                "\xF0""\xA0""Z",
                "\xF0""\xA0""\x01",
                "\xF0""\xA0""\x50",
                "\xF0""\xA0""\xA0",
                "\xF0""\xA0""\xF0""",
                "aaaaaaaaa",
                "aaaaaaaa ",
                "aaaaaaaa",
                "zzzzzaaaaaaaa",
            };

            const int NUM_DATA  = sizeof(DATA)/sizeof(*DATA);

            for (int lhs = 0; lhs < NUM_DATA; ++lhs) {
                for (int rhs = 0; rhs < NUM_DATA; ++rhs) {

                    bsl::ostringstream output;

                    const char *lhsString = DATA[lhs];
                    const char *rhsString = DATA[rhs];

                    bool equal = 0 == strcmp(lhsString, rhsString);

                    ASSERT(equal ==
                           Obj::compareText(lhsString, rhsString, output));
                    ASSERT(equal == output.str().empty());
                }
            }

        }

        if (verbose) bsl::cout << "Compare data with embedded nulls."
                               << bsl::endl;
        {
            struct TestData {
                char d_string[5];
                int  d_size;
            } DATA[] = {
                { { }                            , 0 },
                { {    0 }                       , 1 },
                { { 0x10 }                       , 1 },
                { { 'a'  }                       , 1 },
                { { 'a'  , 'a',    0, 0x10, 'b' }, 5 },
                { {  0   , 'a',  'b', 0x10, 'b' }, 5 },
                { {  0   , 'a',  'b', 0x10, 'c' }, 5 },
                { {  0   , 'a',  'b', 0x10,  0  }, 5 }
            };
            const int NUM_DATA  = sizeof(DATA)/sizeof(*DATA);

            for (int lhs = 0; lhs < NUM_DATA; ++lhs) {
                for (int rhs = 0; rhs < NUM_DATA; ++rhs) {

                    bsl::ostringstream output;

                    bslstl::StringRef lhsString(DATA[lhs].d_string,
                                                DATA[lhs].d_size);
                    bslstl::StringRef rhsString(DATA[rhs].d_string,
                                                DATA[rhs].d_size);

                    bool equal = lhsString == rhsString;
                    ASSERT(equal ==
                           Obj::compareText(lhsString, rhsString, output));
                    ASSERT(equal == output.str().empty());
                }
            }
        }

        if (verbose) bsl::cout << "verify error message." << bsl::endl;
        {
            bsl::string INDEX_MARKER("index (");
            bsl::string LHS_MARKER("lhs[i] = ");
            bsl::string RHS_MARKER("rhs[i] = ");
            {
                bsl::ostringstream  error;
                const char         *A = "aa";
                const char         *B = "ab";

                ASSERT(false == Obj::compareText(A, B, error));

                bsl::string errorMsg = error.str();

                ASSERT(0 < errorMsg.size());

                ASSERT(bsl::string::npos != errorMsg.find(INDEX_MARKER));
                ASSERT('1' == errorMsg[
                           errorMsg.find(INDEX_MARKER) + INDEX_MARKER.size()]);

                ASSERT(bsl::string::npos != errorMsg.find(LHS_MARKER));
                ASSERT('a' == errorMsg[
                           errorMsg.find(LHS_MARKER) + LHS_MARKER.size()]);

                ASSERT('b' == errorMsg[
                           errorMsg.find(RHS_MARKER) + RHS_MARKER.size()]);

            }

            {
                bsl::ostringstream  error;
                const char         *A = "a";
                const char         *B = "aa";

                ASSERT(false == Obj::compareText(A, B, error));

                bsl::string errorMsg = error.str();

                ASSERT(0 < errorMsg.size());

                ASSERT(bsl::string::npos != errorMsg.find(INDEX_MARKER));
                ASSERT('1' == errorMsg[
                           errorMsg.find(INDEX_MARKER) + INDEX_MARKER.size()]);

                ASSERT(bsl::string::npos != errorMsg.find(LHS_MARKER));
                ASSERT('E' == errorMsg[
                           errorMsg.find(LHS_MARKER) + LHS_MARKER.size()]);

                ASSERT('a' == errorMsg[
                           errorMsg.find(RHS_MARKER) + RHS_MARKER.size()]);

            }
            {
                bsl::ostringstream  error;
                const char         *A = "aa";
                const char         *B = "a";

                ASSERT(false == Obj::compareText(A, B, error));

                bsl::string errorMsg = error.str();

                ASSERT(0 < errorMsg.size());

                ASSERT(bsl::string::npos != errorMsg.find(INDEX_MARKER));
                ASSERT('1' == errorMsg[
                           errorMsg.find(INDEX_MARKER) + INDEX_MARKER.size()]);

                ASSERT(bsl::string::npos != errorMsg.find(LHS_MARKER));
                ASSERT('a' == errorMsg[
                           errorMsg.find(LHS_MARKER) + LHS_MARKER.size()]);

                ASSERT('E' == errorMsg[
                           errorMsg.find(RHS_MARKER) + RHS_MARKER.size()]);

            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // BSLIM_TESTUTIL_LOOP*_ASSERT AND BSLIM_ASSERTV MACROS
        //
        // Concerns:
        //: 1 Macros do not call 'aSsErT' and emit no output when the assertion
        //:   is 'true'.
        //:
        //: 2 Macros call 'aSsErT' and emit output each time the assertion is
        //:   'false'.
        //:
        //: 3 Macros emit properly formatted output for each loop variable
        //:   supplied.
        //
        // Plan:
        //: 1 Loop through an arbitrary number of iterations, calling one of
        //:   the loop assert macros with distinct values for each loop
        //:   variable and an assertion that evaluates to 'true'.  Confirm that
        //:   the value of 'testStatus' does not change, and that no output is
        //:   captured by the output redirection apparatus.  (C-1)
        //:
        //: 2 Loop through an arbitrary number of iterations, calling one of
        //:   the loop assert macros with distinct values for each loop
        //:   variable and an assertion that evaluates to 'false'.  Confirm
        //:   that 'testStatus' increments each time the loop assert macro is
        //:   called, and that the expected error output is captured by the
        //:   output redirection apparatus.  Note that using distinct values
        //:   for each loop variable allows us to detect omissions,
        //:   repetitions, or mis-ordering of the loop assert macro's
        //:   arguments.  Also note that we test the loop assert macro with
        //:   only one set of variable types, since we test separately in test
        //:   case 3 the ability of the underlying apparatus to identify and
        //:   correctly format each primitive type.  (C-2..3)
        //
        // Testing:
        //   BSLIM_TESTUTIL_ASSERT(X)
        //   BSLIM_TESTUTIL_LOOP_ASSERT(I,X)
        //   BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)
        //   BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)
        //   BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
        //   BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
        //   BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
        //   BSLIM_TESTUTIL_ASSERTV(...)
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cerr << bsl::endl
                      << "BSLIM_TESTUTIL_LOOP*_ASSERT AND BSLIM_ASSERTV MACROS"
                      << bsl::endl
                      << "===================================================="
                      << bsl::endl;

        enum {
            BUFFER_SIZE    = 1024,  // size of the buffer used to store
                                    // captured output

            NUM_ITERATIONS =   10   // number of iterations to use when testing
                                    // loop assert macros
        };

        static char s_expectedOutput[BUFFER_SIZE];  // scratch area for
                                                    // assembling model output
                                                    // that will be compared to
                                                    // real output captured
                                                    // from 'stdout'

        // 'BSLIM_TESTUTIL_LOOP_ASSERT(I,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP_ASSERT(I, idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, idx < NUM_ITERATIONS);

                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP_ASSERT(I, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }

        // 'BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP2_ASSERT(I, J, idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, J, idx < NUM_ITERATIONS);
                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP2_ASSERT(I, J, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, J, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }

        // 'BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP3_ASSERT(I, J, K,
                                            idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, J, K, idx < NUM_ITERATIONS);
                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP3_ASSERT(I, J, K, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, J, K, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }

        // 'BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP4_ASSERT(I, J, K, L,
                                            idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, idx < NUM_ITERATIONS);
                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP4_ASSERT(I, J, K, L, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }

        // 'BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M,
                                            idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, M, idx < NUM_ITERATIONS);
                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M,
                                            idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L, M,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;

                if (veryVerbose) { REALP(idx); }

                output.reset();

                // On AIX, the line number printed from the
                // 'BSLIM_TESTUTIL_ASSERTV' macro is the line number of the
                // first line of the call statement, even if the statement is
                // split over multiple lines.  This behavior is different from
                // the equivalent loop-assert alternative, which prints the
                // last line of the statement.  The behavior of 'ASSERTV' and
                // regular loop-assert is consistent on other platforms.  So
                // here we make sure that the call to 'BSLIM_TESTUTIL_ASSERTV'
                // fits on a single line to ensure that the output is the same
                // on all platforms.

                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, M, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\nError %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L, M,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }

        // 'BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)'
        {
            ASSERT(0 == testStatus);
            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;
                const int N = idx + 5;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                BSLIM_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N,
                                            idx < NUM_ITERATIONS);
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, M, N, idx < NUM_ITERATIONS);
                REALLOOP_ASSERT(testStatus, 0 == testStatus);
                ASSERT(output.load());
                REALLOOP_ASSERT(output.buffer(), 0 == output.compare(""));
            }
            ASSERT(0 == testStatus);

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;
                const int N = idx + 5;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N,
                                            idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\tN: %d\n"
                         "Error %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L, M, N,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;

            // Repeat for 'ASSERTV'.

            for (int idx = 0; idx < NUM_ITERATIONS; ++idx) {
                const int I = idx;
                const int J = idx + 1;
                const int K = idx + 2;
                const int L = idx + 3;
                const int M = idx + 4;
                const int N = idx + 5;

                if (veryVerbose) { REALP(idx); }

                output.reset();
                const int LINE = __LINE__ + 1;
                BSLIM_TESTUTIL_ASSERTV(I, J, K, L, M, N, idx > NUM_ITERATIONS);
                REALLOOP2_ASSERT(testStatus, idx, testStatus == idx + 1);
                ASSERT(output.load());
                snprintf(s_expectedOutput,
                         BUFFER_SIZE,
                         "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\tN: %d\n"
                         "Error %s(%d):"
                         " idx > NUM_ITERATIONS    (failed)\n",
                         I, J, K, L, M, N,
                         __FILE__,
                         LINE);
                REALLOOP2_ASSERT(s_expectedOutput,
                                 output.buffer(),
                                 0 == output.compare(s_expectedOutput));
            }
            ASSERT(NUM_ITERATIONS == testStatus);
            testStatus = 0;
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'Q' (IDENTIFIER OUTPUT MACRO)
        //
        // Concerns:
        //: 1 The identifier output macro emits the correct output,
        //:   '<| [macro argument] |>', where '[macro argument]' is the
        //:   tokenization of the text supplied as an argument to the macro.
        //
        // Plan:
        //: 1 Call 'BSLIM_TESTUTIL_Q' with a series of arbitrary identifiers
        //:   containing single and multiple tokens, with and without initial,
        //:   final, and repeated whitespace and compare each captured output
        //:   to the expected output.  (C-1)
        //
        // Testing:
        //   BSLIM_TESTUTIL_Q(X)
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "'Q' (IDENTIFIER OUTPUT MACRO)" << bsl::endl
                               << "=============================" << bsl::endl;
        {
            output.reset();
            BSLIM_TESTUTIL_Q(sample);
            ASSERT(output.load());
            ASSERT(0 == output.compare("<| sample |>\n"));
        }

        {
            output.reset();
            BSLIM_TESTUTIL_Q(embedded white   space);
            ASSERT(output.load());
            ASSERT(0 == output.compare("<| embedded white space |>\n"));
        }

        {
            output.reset();
            BSLIM_TESTUTIL_Q(   initial whitespace);
            ASSERT(output.load());
            ASSERT(0 == output.compare("<| initial whitespace |>\n"));
        }

        {
            output.reset();
            BSLIM_TESTUTIL_Q(final whitespace   );
            ASSERT(output.load());
            ASSERT(0 == output.compare("<| final whitespace |>\n"));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'P' AND 'P_' (VALUE OUTPUT MACROS)
        //
        // Concerns:
        //: 1 The value output macros emit output.
        //:
        //: 2 Output emitted is in the correct format for the standard 'P' and
        //:   'P_' macros, i.e., 'identifier = value' (with following newline
        //:   in the case of 'BSLIM_TESTUTIL_P') where 'identifier' is the name
        //:   of the argument supplied to the macro, and 'value' is the value
        //:   of that argument.
        //
        // Plan:
        //: 1 Call the value output macros on a variable of known value, and
        //:   confirm that the captured output is in the correct format.  Note
        //:   that it is only necessary to conduct this test once with a single
        //:   variable type because the underlying type-differentiation and
        //:   formatting mechanisms are handled by the input operators.
        //:   (C-1..2)
        //
        // Testing:
        //   BSLIM_TESTUTIL_P(X)
        //   BSLIM_TESTUTIL_P_(X)
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "'P' AND 'P_' (VALUE OUTPUT MACROS)"
                               << bsl::endl
                               << "=================================="
                               << bsl::endl;

        // 'BSLIM_TESTUTIL_P(X)'
        {
            const int INPUT = 42;

            output.reset();
            BSLIM_TESTUTIL_P(INPUT);
            ASSERT(output.load());
            REALLOOP_ASSERT(output.buffer(),
                            0 == output.compare("INPUT = 42\n"));
        }

        // 'BSLIM_TESTUTIL_P_(X)'
        {
            const int INPUT = 42;

            output.reset();
            BSLIM_TESTUTIL_P_(INPUT);
            ASSERT(output.load());
            REALLOOP_ASSERT(output.buffer(),
                            0 == output.compare("INPUT = 42, "));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'L_' AND 'T_' (STATIC MACROS)
        //
        // Concerns:
        //: 1 The line number macro has the correct value.
        //:
        //: 2 The tab output macro emits output.
        //:
        //: 3 The output of the tab output macro is emitted in the correct
        //:   format.
        //
        // Plan:
        //: 1 Compare the value of the line number macro to '__LINE__'.  (C-1)
        //:
        //: 2 Call the tab output macro and confirm that the captured output
        //:   is in the correct format.  (C-2..3)
        //
        // Testing:
        //   BSLIM_TESTUTIL_L_
        //   BSLIM_TESTUTIL_T_
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "'L_' AND 'T_' (STATIC MACROS)" << bsl::endl
                               << "=============================" << bsl::endl;

        // 'BSLIM_TESTUTIL_L_'
        {
            // Line spacing is significant as it assures 'BSLIM_TESTUTIL_L_' is
            // not a simple sequence.

            ASSERT(__LINE__ == BSLIM_TESTUTIL_L_);


            ASSERT(__LINE__ == BSLIM_TESTUTIL_L_);
            ASSERT(__LINE__ == BSLIM_TESTUTIL_L_);



            ASSERT(__LINE__ == BSLIM_TESTUTIL_L_);

            ASSERT(__LINE__ == BSLIM_TESTUTIL_L_);
        }

        // 'BSLIM_TESTUTIL_T_'
        {
            output.reset();
            BSLIM_TESTUTIL_T_
            ASSERT(output.load());
            ASSERT(0 == output.compare("\t"));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 Output is redirected.
        //:
        //: 2 Captured output is readable.
        //:
        //: 3 'load' works.
        //:
        //: 4 'reset' works.
        //:
        //: 5 'compare' works.
        //:
        //: 6 Incorrect output is correctly diagnosed.
        //:
        //: 7 Embedded newlines work.
        //:
        //: 8 Empty output works.
        //:
        //: 9 Embedded null characters work.
        //:
        //:10 Filesystem-dependent control sequences work.
        //:
        //:11 'stderr' points to the original target of 'stdout'.
        //
        // Plan:
        //: 1 Confirm that 'ftell(stdout)' succeeds.  This demonstrates that
        //:   'stdout' is a seekable file.  (C-1)
        //:
        //: 2 Write a string to 'stdout' and confirm that 'stdout's seek
        //:   position has changed; read back the contents of 'stdout' and
        //:   compare them to the original string.  (C-2)
        //:
        //: 3 Write a string to 'stdout'.  Confirm that
        //:   'OutputRedirector::load' changes the contents of the output
        //:   buffer and that it changes the result of
        //:   'OutputRedirector::isOutputReady' from 'false' to 'true'.
        //:   Confirm that the contents of the output buffer match the
        //:   original string.  (C-3)
        //:
        //: 4 Write a string to 'stdout' and load it with
        //:   'OutputRedirector::load'.  Confirm that
        //:   'OutputRedirector::reset' rewinds 'stdout', changes the output
        //:   of 'OutputRedirector::isOutputReady' from 'true' to 'false', and
        //:   sets the length of the output buffer to 0.  (C-4)
        //:
        //: 5 Write a string to 'stdout' and load it with
        //:   'OutputRedirector::load'.  Confirm that
        //:   'OutputRedirector::compare' gives the correct results when the
        //:   captured output is compared with the following data:  (C-5)
        //:
        //:       Data                           Comparison Result
        //:   ------------                   -------------------------
        //:   input string                           true
        //:   input string with appended data        false
        //:   input string truncated                 false
        //:   string different from input:
        //:     at beginning                         false
        //:     at end                               false
        //:     elsewhere                            false
        //:
        //: 6 Confirm that 'load' fails when there is more data in 'stdout'
        //:   than can fit in the capture buffer.  Confirm that 'compare' fails
        //:   if 'load' has not been first called to read data into the capture
        //:   buffer.  (C-6)
        //:
        //: 7 Confirm that strings containing embedded newlines are correctly
        //:   captured and correctly identified by 'compare'.  (C-7)
        //:
        //: 8 Write an empty string to 'stdout'.  Confirm that it can be
        //:   correctly loaded and compared with the original.  (C-8)
        //:
        //: 9 Write a series of strings to 'stdout' containing '\0' at the
        //:   beginning, end, or interior of the string.  Confirm that the
        //:   captured output can be correctly loaded and compared with the
        //:   original input.  (C-9)
        //:
        //:10 Write a series of strings to 'stdout' containing '^D' and
        //:   '<CRLF>' and confirm that these strings are correctly captured
        //:   and loaded.  (C-10)
        //:
        //:11 Use 'fstat' to find out the device and inode of the current
        //:   (post-redirection) 'stderr'.  Compare these values to the device
        //:   and inode of 'stdout' before redirection.  (C-11)
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "TEST APPARATUS" << bsl::endl
                               << "==============" << bsl::endl;

        {
            // 1) Output is redirected.

            ASSERT(-1 != ftell(stdout));
        }

        {
            // 2) Captured output is readable.

            enum { TEST_STRING_SIZE = 15 };
            const char *testString = "This is output";
            char        buffer[TEST_STRING_SIZE];

            ASSERT(TEST_STRING_SIZE == strlen(testString) + 1);

            rewind(stdout);
            long initialStdoutPosition = ftell(stdout);
            ASSERT(0 == initialStdoutPosition);
            printf("%s", testString);
            long finalStdoutPosition = ftell(stdout);
            ASSERT(-1 != finalStdoutPosition);
            ASSERT(finalStdoutPosition > initialStdoutPosition);
            long outputSize = finalStdoutPosition - initialStdoutPosition;
            ASSERT(outputSize + 1 == TEST_STRING_SIZE);
            rewind(stdout);
            size_t bytesWritten =
                               fread(buffer, sizeof(char), outputSize, stdout);
            ASSERT(static_cast<long>(bytesWritten) == outputSize);
            buffer[TEST_STRING_SIZE - 1] = '\0';
            ASSERT(0 == strcmp(testString, buffer));
        }

        {
            // 3) 'load' works.

            const char *testString       = "This is output";
            size_t      testStringLength = strlen(testString);

            rewind(stdout);
            printf("%s", testString);
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(false == output.isOutputReady());
            ASSERT(0 == output.outputSize());
            ASSERT(output.load());
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(true == output.isOutputReady());
            ASSERT(testStringLength == output.outputSize());
            ASSERT(0 == memcmp(testString,
                               output.buffer(),
                               output.outputSize()));
        }

        {
            // 4) 'reset' works.

            const char *testString       = "This is output";
            size_t      testStringLength = strlen(testString);

            rewind(stdout);
            printf("%s", testString);
            output.load();
            ASSERT(static_cast<long>(testStringLength) == ftell(stdout));
            ASSERT(true == output.isOutputReady());
            ASSERT(testStringLength == output.outputSize());
            output.reset();
            ASSERT(0 == ftell(stdout));
            ASSERT(true != output.isOutputReady());
            ASSERT(0 == output.outputSize());
        }

        {
            // 5) 'compare' works.

            const char *testString            = "This is output";
            const char *longString            = "This is outputA";
            const char *shortString           = "This is outpu";
            const char *differentStartString  = "Xhis is output";
            const char *differentEndString    = "This is outpuy";
            const char *differentMiddleString = "This iz output";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(!!strcmp(testString, testString) ==
                       !!output.compare(testString));

            ASSERT(!!strcmp(testString, shortString) ==
                       !!output.compare(shortString));
            ASSERT(!!strcmp(testString, longString) ==
                       !!output.compare(longString));
            ASSERT(!!strcmp(testString, differentStartString) ==
                       !!output.compare(differentStartString));
            ASSERT(!!strcmp(testString, differentEndString) ==
                       !!output.compare(differentEndString));
            ASSERT(!!strcmp(testString, differentMiddleString) ==
                       !!output.compare(differentMiddleString));
        }

        {
            // 6) Incorrect output is correctly diagnosed.

            // Reset verbosity levels to suppress expected error output.
            const bool tempVeryVerbose     = veryVerbose;
            const bool tempVeryVeryVerbose = veryVeryVerbose;
            veryVerbose     = false;
            veryVeryVerbose = false;

            const char *testString = "This is good output";

            output.reset();
            int stringLength = strlen(testString);
            for (int idx = 0; idx * stringLength < OUTPUT_BUFFER_SIZE; ++idx) {
                printf("%s", testString);
            }
            printf("%s", testString);
            ASSERT(!output.load());

            output.reset();
            printf("%s", testString);
            ASSERT(0 != output.compare(testString));

            output.reset();
            ASSERT(0 != output.compare("", 0));

            veryVerbose     = tempVeryVerbose;
            veryVeryVerbose = tempVeryVeryVerbose;
        }

        {
            // 7) Embedded newlines work.

            const char *testString = "This has an\nembedded newline";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString));

            const char *twoNewlineTestString =
                "This has two\nembedded newlines\n";

            output.reset();
            printf("%s", twoNewlineTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(twoNewlineTestString));
        }

        {
            // 8) Empty output works.

            const char *testString = "";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString));
        }

        {
            // 9) Embedded null characters work.

            const char *testString1 = "abc\0def";
            const char *testString2 = "\0def";
            const char *testString3 = "abc\0";

            output.reset();
            fwrite(testString1, sizeof(char), 7, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString1, 7));

            output.reset();
            fwrite(testString2, sizeof(char), 4, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString2, 4));

            output.reset();
            fwrite(testString3, sizeof(char), 4, stdout);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString3, 4));
        }

        {
            // 10) Filesystem-dependent control sequences work.

            const char *crnlTestString = "ab\r\ncd";
            const char *ctrlDTestString = "ab" "\x04" "cd";

            output.reset();
            printf("%s", crnlTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(crnlTestString));

            output.reset();
            printf("%s", ctrlDTestString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(ctrlDTestString));
        }

        {
            // 11) 'stderr' points to the original target of 'stdout'.

            int newStderrFD = fileno(stderr);
            ASSERT(-1 != newStderrFD);
            struct stat stderrStat;
            stderrStat.st_dev = output.originalStdoutStat().st_dev;
            stderrStat.st_rdev = output.originalStdoutStat().st_rdev;
            ASSERT(-1 != fstat(newStderrFD, &stderrStat));
#if !defined(BSLS_PLATFORM_OS_WINDOWS)
            // 'st_dev' and 'st_rdev' are not stable on Windows.
            ASSERT(stderrStat.st_dev == output.originalStdoutStat().st_dev);
            ASSERT(stderrStat.st_rdev == output.originalStdoutStat().st_rdev);
#endif
            ASSERT(stderrStat.st_ino == output.originalStdoutStat().st_ino);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The macros are sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform an ad-hoc test of the macros.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cerr << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

        const bool    b = true;
        const char    c = 'c';
        const int     i = 123;
        const double  d = 123.56;
        const float   f = 789.01f;
        const char   *s = "hello";

        if (verbose) {
            BSLIM_TESTUTIL_Q(BSLIM_TESTUTIL_L_);
            BSLIM_TESTUTIL_P_(b);
            BSLIM_TESTUTIL_T_;
            BSLIM_TESTUTIL_P_(c);
            BSLIM_TESTUTIL_T_;
            BSLIM_TESTUTIL_P(i);

            BSLIM_TESTUTIL_ASSERT(!b);
            BSLIM_TESTUTIL_LOOP_ASSERT(b, !b);
            ASSERT(testStatus == 2);

            testStatus = 0;
            BSLIM_TESTUTIL_LOOP0_ASSERT(!b);
            BSLIM_TESTUTIL_LOOP1_ASSERT(b, !b);
            BSLIM_TESTUTIL_LOOP2_ASSERT(b, c, !b);
            BSLIM_TESTUTIL_LOOP3_ASSERT(b, c, i, !b);
            BSLIM_TESTUTIL_LOOP4_ASSERT(b, c, i, d, !b);
            BSLIM_TESTUTIL_LOOP5_ASSERT(b, c, i, d, f, !b);
            BSLIM_TESTUTIL_LOOP6_ASSERT(b, c, i, d, f, s, !b);
            ASSERT(testStatus == 7);

            testStatus = 0;
            BSLIM_TESTUTIL_ASSERTV(!b);
            BSLIM_TESTUTIL_ASSERTV(b, !b);
            BSLIM_TESTUTIL_ASSERTV(b, c, !b);
            BSLIM_TESTUTIL_ASSERTV(b, c, i, !b);
            BSLIM_TESTUTIL_ASSERTV(b, c, i, d, !b);
            BSLIM_TESTUTIL_ASSERTV(b, c, i, d, f, !b);
            BSLIM_TESTUTIL_ASSERTV(b, c, i, d, f, s, !b);
            ASSERT(testStatus == 7);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        realTestStatus = -1;
      }
    }

    if (realTestStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", realTestStatus);
    }
    return realTestStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
