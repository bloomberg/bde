/// bsls_bsltestutil.t.cpp                                            -*-C++-*-

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strnlen
#include <limits.h> // PATH_MAX on linux
#include <float.h>  // FLT_MAX, etc.
#include <unistd.h>
#include <fcntl.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//
// The component under test implements a single utility struct and 11 macros
// that call methods of the utility struct.  All of the methods write output to
// stdout and do nothing more.  Therefore, there are no Primary Manipulators or
// Basic Accessors to test.  All of the tests fall under the category of Print
// Operations.
//
// The macros provided by the component under test mirror the standard test
// macros normally used in test drivers.  The intention is that the standard
// test macros should be implemented as aliases of the 'BSLS_BSLTESTUTIL_*'
// macros, as illustrated in the usage example.  As a result, the identifiers
// normally used in a test driver conflict with the identifiers used in the
// usage example.  Therefore, this test driver avoids the standard test macros
// (and support functions), and uses the following instead:
//
// STANDARD              BSLS_BSLTESTUTIL.T.CPP
// --------              ----------------------
// 'LOOP_ASSERT'         'ANNOTATED_ASSERT'
// 'LOOP2_ASSERT'        'ANNOTATED2_ASSERT'
// 'LOOP3_ASSERT'        'ANNOTATED3_ASSERT'
// 'LOOP4_ASSERT'        'ANNOTATED4_ASSERT'
// 'LOOP5_ASSERT'        not used
// 'LOOP6_ASSERT'        not used
// 'Q'                   not used
// 'P'                   'PRINT'
// 'P_'                  'PRINT_'
// 'T_'                  'TAB_'
// 'L_'                  not used
// 'void aSsErT()'       'void realaSsErT()'
// 'int testStatus'      'int realTestStatus'
//
// Note that the 'ANNOTATED*_ASSERT' macros are only rough equivalents of the
// standard 'LOOP*_ASSERT' macros.  Because 'std::cout' is not available in
// this test driver, there is no way to automatically format values based on
// type, at least not without re-implementing the component under test in the
// test driver.  Therefore the 'ANNOTATED*_ASSERT' macros take as arguments
// pairs of identifiers and 'printf'-style format strings.  This is the reason
// why they have been given completely different names from the standard
// macros.
//
// The main difficulty with writing the test driver is capturing the output of
// the methods under test so that it can be checked for accuracy.  In addition,
// error messages and other output produced by the test driver itself must
// still appear on 'stdout' for compatibility with the standard build and
// testing scripts.  For this purpose, a support class name 'OutputRedirector'
// is provided.  'OutputRedirector' will redirect 'stdout' to a temporary file
// and check the contents of the temporary file to character buffers provided
// by the user.
//
// Global Concerns:
//: o The test driver can reliably capture 'stdout' and play back captured
//:   output.
//: o The test driver's own output is written to the calling environment's
//:   'stdout'.
//
//-----------------------------------------------------------------------------
// [ 8] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
// [ 8] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
// [ 7] BSLS_BSLTESTUTIL_Q(X)
// [ 6] BSLS_BSLTESTUTIL_P(X)
// [ 6] BSLS_BSLTESTUTIL_P_(X)
// [ 5] BSLS_BSLTESTUTIL_L_
// [ 5] BSLS_BSLTESTUTIL_T_
// [ 3] static void debugPrint(const char *s, bool b, const char *t);
// [ 3] static void debugPrint(const char *s, char c, const char *t);
// [ 3] static void debugPrint(const char *s, signed char v, const char *t);
// [ 3] static void debugPrint(const char *s, unsigned char v, const char *t);
// [ 3] static void debugPrint(const char *s, short v, const char *t);
// [ 3] static void debugPrint(const char *s, unsigned short v, const char *t);
// [ 3] static void debugPrint(const char *s, int v, const char *t);
// [ 3] static void debugPrint(const char *s, unsigned int v, const char *t);
// [ 3] static void debugPrint(const char *s, long v, const char *t);
// [ 3] static void debugPrint(const char *s, unsigned long v, const char *t);
// [ 3] static void debugPrint(const char *s, long long v, const char *t);
// [ 3] static void debugPrint(const char *s, unsigned long long v,
//                             const char *t);
// [ 3] static void debugPrint(const char *s, float v, const char *t);
// [ 3] static void debugPrint(const char *s, double v, const char *t);
// [ 3] static void debugPrint(const char *s, long double v, const char *t);
// [ 3] static void debugPrint(const char *s, char *str, const char *t);
// [ 3] static void debugPrint(const char *s, const char *str, const char *t);
// [ 3] static void debugPrint(const char *s, void *p, const char *t);
// [ 3] static void debugPrint(const char *s, const void *p, const char *t);
// [ 4] static void printStringNoFlush(const char *s);
// [ 4] static void printTab();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST APPARATUS: class 'OutputRedirector'
// [ 9] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                VARIATIONS ON STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// In order to accomodate the use of the identifiers 'testStatus' and 'aSsErT'
// in the usage example, the rest of the test driver uses the identifiers
// 'realTestStatus' and 'realaSsErT' instead.
//
// Additionally, in order to allow capturing the output of the
// 'BSLS_BSLTESTUTIL_*' macros, the standard macros output to 'stderr' instead
// of 'stdout'.

static int realTestStatus = 0;

static void realaSsErT(int c, const char *s, int i)
{
    if (c) {
        fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (realTestStatus >= 0 && realTestStatus <= 100) ++realTestStatus;
    }
}

// The standard 'ASSERT' macro definition is deferred until after the usage
// example code
// #define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

// ============================================================================
//               VARIATIONS ON STANDARD BDE LOOP_ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// The standard BDE 'LOOP_ASSERT' test macros rely on overloads of 'operator<<'
// ostream to provide a uniform interface for all types.  The purpose of
// 'bsls_bsltestutil' is precisely to provide a similar overloading mechanism
// for 'printf'-based output.  In order to use the standard macros in this test
// driver, we would have to define a test appartus of equivalent complexity to
// the class we are testing.  Therefore, instead of the standard test macros we
// define alternate macros that take pairs of variables and 'printf'-style
// format strings instead of just variables alone.  For convenience, all of the
// macros are defined in terms of a free function named 'printDatum' that
// prints a single variable's identifier and value separated by a user-supplied
// string.

#define ANNOTATED_ASSERT(I,IF,X) do {                                         \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\n",(I));      \
                    realaSsErT(1, #X, __LINE__);}                             \
    } while (0)

#define ANNOTATED2_ASSERT(I,IF,J,JF,X) do {                                   \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));      \
                    printDatum(stderr, #J,": ",(JF),"\n",(J));      \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define ANNOTATED3_ASSERT(I,IF,J,JF,K,KF,X) do {                              \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));      \
                    printDatum(stderr, #J,": ",(JF),"\t",(J));      \
                    printDatum(stderr, #K,": ",(KF),"\n",(K));      \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define ANNOTATED4_ASSERT(I,IF,J,JF,K,KF,L,LF,X) do {                         \
        if (!(X)) { printDatum(stderr, #I,": ",(IF),"\t",(I));      \
                    printDatum(stderr, #J,": ",(JF),"\t",(J));      \
                    printDatum(stderr, #K,": ",(KF),"\t",(K));      \
                    printDatum(stderr, #L,": ",(LF),"\n",(L));      \
                    realaSsErT(1, #X, __LINE__); }                            \
    } while (0)

#define PRINT(X,XF) do {                                                      \
        printDatum(stderr,#X," = ",(XF),"\n",(X));                  \
    } while(0)

#define PRINT_(X,XF) do {                                                     \
        printDatum(stderr,#X," = ",(XF),", ",(X));                  \
    } while(0)

#define TAB_ do {                                                             \
        fprintf(stderr, "\t");                                                \
    } while(0)

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------
//
// Usage example code assumes that 'BSLS_BSLTESTUTIL_*' macros have been
// renamed to replace the standard test macros.  In order to simplify the rest
// of the test driver, the standard macros are redefined after 'main', and the
// usage example case just calls a funtion, 'executeUsageExample'.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
    struct BslExampleUtil {
        // This utility class provides sample functionality to demonstrate how
        // a test driver might be written validating its only method.

        static int fortyTwo();
            // Return the integer value '42'.
    };

    inline
    int BslExampleUtil::fortyTwo()
    {
        return 42;
    }
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro:
//..
    //=========================================================================
    //                       STANDARD BDE ASSERT TEST MACRO
    //-------------------------------------------------------------------------
    static int testStatus = 0;

    static void aSsErT(bool b, const char *s, int i)
    {
        if (b) {
            printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
            if (testStatus >= 0 && testStatus <= 100) ++testStatus;
        }
    }

    # define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//..
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
    //=========================================================================
    //                       STANDARD BDE TEST DRIVER MACROS
    //-------------------------------------------------------------------------
    #define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
    #define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
    #define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
    #define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
    #define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
    #define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

    #define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
    #define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
    #define P_  BSLS_BSLTESTUTIL_P_  // 'P(X)' without '\n'.
    #define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
    #define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Finally, we write the test case for the 'static' 'fortyTwo' method, using
// the (standard) abbreviated macro names we have just defined.
//..
    void executeUsageExample(bool verbose,
                             bool veryVerbose,
                             bool veryVeryVerbose)
    {
        (void) veryVerbose;
        (void) veryVeryVerbose;

        const int value = BslExampleUtil::fortyTwo();
        if (verbose) P(value);
        LOOP_ASSERT(value, 42 == value);
    }

//=============================================================================
//                    CLEANUP STANDARD TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#undef ASSERT

#undef LOOP_ASSERT
#undef LOOP2_ASSERT
#undef LOOP3_ASSERT
#undef LOOP4_ASSERT
#undef LOOP5_ASSERT
#undef LOOP6_ASSERT

#undef Q
#undef P
#undef P_
#undef T_
#undef L_

#define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { FORMAT_STRING_SIZE = 256 }; // Size of temporary format string buffers
                                   // used for output formatting

enum {
    // Enumeration used to store sizes for the buffers used by the output
    // redirection apparatus.

    OUTPUT_BUFFER_SIZE = 4096,

#ifdef BSLS_PLATFORM__OS_WINDOWS
    PATH_BUFFER_SIZE   = MAX_PATH
#elif defined(BSLS_PLATFORM__OS_HPUX)
    PATH_BUFFER_SIZE   = L_tmpnam
#else
    PATH_BUFFER_SIZE   = PATH_MAX
#endif
};

// The constants 'PREFIX' and 'SUFFIX' are defined as macros because they are
// used in the initialization lists for the 'DataRow<>' arrays in test case 3.

#define PREFIX "<"
    // Standard prefix for use as 's' in 'debugPrint' tests
#define SUFFIX ">"
    // Standard suffix for use as 't' in 'debugPrint' tests

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose;

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <typename ITYPE>
int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               const ITYPE  valueI);
    // Print the specified 'identifierI' identifier name and specified value
    // 'valueI' of (parameter template) type 'ITYPE' to the specified
    // 'outStream', and separated by the specified 'connector' string and
    // followed by the specified 'suffix' string, using the specified 'formatI'
    // format string to format 'valueI' according to the rules of 'printf'.

template <typename ITYPE>
int printDatum(FILE        *outStream,
               const char  *identifierI,
               const char  *connector,
               const char  *formatI,
               const char  *suffix,
               const ITYPE  valueI)
{
    char tempFormatString[FORMAT_STRING_SIZE];

    int charsWritten = snprintf(tempFormatString,
                                FORMAT_STRING_SIZE,
                                "%%s%%s%s%%s",
                                formatI);
    ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
    return fprintf(outStream,
                   tempFormatString,
                   identifierI,
                   connector,
                   valueI,
                   suffix);
}

bool tempFileName(char *result);
    // Store an available temp file name in the user-supplied buffer at the
    // address pointed to by the specified 'result'.  Return 'true' if
    // successful, and 'false' otherwise.  The buffer pointed to by the
    // specified 'result' must be at least 'PATH_BUFFER_SIZE' bytes long.

bool tempFileName(char *result)
{
    ASSERT(result);

#ifdef BSLS_PLATFORM__OS_WINDOWS
    char tmpPathBuf[MAX_PATH];
    if (! GetTempPath(MAX_PATH, tmpPathBuf) ||
        ! GetTempFileName(tmpPathBuf, "bsls", 0, result)) {
        return false;                                                 // RETURN
    }
#elif defined(BSLS_PLATFORM__OS_HPUX)
    if(! tempnam(result, "bsls")) {
        return false;
    }
#else
    char *fn = tempnam(0, "bsls");
    if (fn) {
        strncpy(result, fn, PATH_BUFFER_SIZE);
        result[PATH_BUFFER_SIZE - 1] = '\0';
        free(fn);
    } else {
        return false;
    }
#endif

    if (veryVerbose) printf("\tUse '%s' as a base filename.\n", result);

    ASSERT('\0' != result[0]); // result not empty

    return true;
}

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class OutputRedirector {
    // This class redirects 'stdout' to a temporary file and provides
    // facilities for retrieving output from the temporary file and comparing
    // it to user-supplied character buffers.

  private:
    // DATA
    char d_fileName[PATH_BUFFER_SIZE];        // Name of temporary capture file

    char d_outputBuffer[OUTPUT_BUFFER_SIZE];  // Scratch buffer for holding
                                              // captured output

    bool d_isRedirectedFlag;                  // Has 'stdout' been redirected

    bool d_isFileCreatedFlag;                 // Has a temp file been created

    long d_outputSize;                        // Size of output loaded into
                                              // 'd_outputBuffer'

    bool d_isOutputReadyFlag;                 // Has output been read from
                                              // temp file

    struct stat d_originalStdoutStat;         // Status information for
                                              // 'stdout' just before
                                              // redirection.

  private:
    // NOT IMPLEMENTED
    OutputRedirector(const OutputRedirector&);
    OutputRedirector& operator=(const OutputRedirector&);

  public:
    // CREATORS
    explicit OutputRedirector();
        // Create this object.

    ~OutputRedirector();
        // Destroy this object.

    // MANIPULATORS
    bool redirect();
        // Redirect 'stdout' to a temp file, and stderr to the original
        // 'stdout'.  Return 'true' if successful, and 'false' otherwise.

    void reset();
        // Reset the scratch buffer to empty.

    bool load();
        // Read captured output into the scratch buffer.  Return 'true' if
        // there was output to be read and it was successfully loaded, and
        // 'false' otherwise.

    // ACCESSORS
    bool isRedirected();
        // Return 'true' if 'stdout' and 'stderr' have been successfully
        // redirected, and 'false' otherwise.

    bool isOutputReady();
        // Return 'true' if captured output been loaded into the scratch
        // buffer, and 'false' otherwise.

    char *getOutput();
        // Return the address of the scratch buffer.  This method is only used
        // for error reporting and to test the correctness of
        // 'OutputRedirector'.

    size_t outputSize();
        // Return the number of bytes currently loaded into the scratch buffer.

    int compare(const char *expected, size_t expectedLength);
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The length of the 'expected' buffer is supplied in the
        // specified 'expectedLength'.  Return 0 if the 'expected' buffer has
        // the same length and contents as the scratch buffer, and non-zero
        // otherwise.  Note that the 'expected' buffer is allowed to contain
        // embedded nulls.

    int compare(const char *expected);
        // Compare the character buffer pointed to by the specified pointer
        // 'expected' with any output that has been loaded into the scratch
        // buffer.  The 'expected' buffer is assumed to be a NTBS, and and its
        // length is taken to be the string length of the NTBS.  Return 0 if
        // the 'expected' buffer has the same length and contents as the
        // scratch buffer, and non-zero otherwise.

    const struct stat& originalStdoutStat();
        // Return a reference to the status information for 'stdout' collected
        // just before redirection.  This method is only used to test the
        // correctness of 'OutputRedirector'.
};

OutputRedirector::OutputRedirector()
    : d_isRedirectedFlag(false)
    , d_isFileCreatedFlag(false)
    , d_outputSize(0L)
    , d_isOutputReadyFlag(false)
{
    d_fileName[0] = '\0';
    memset(&d_originalStdoutStat, 0, sizeof(struct stat));
}

OutputRedirector::~OutputRedirector()
{
    if (d_isRedirectedFlag) {
        fclose(stdout);
    }

    if (d_isFileCreatedFlag) {
        unlink(d_fileName);
    }
}

bool OutputRedirector::redirect()
{
    bool success = false;

    if (d_isRedirectedFlag) {

        // Do not redirect anything if we have already redirected.

        if (veryVerbose) {
            fprintf(stdout,
                    "Warning " __FILE__ "(%d): Output already redirected\n",
                    __LINE__);
        }
        success = true;
    } else {

        // Retain information about original 'stdout' file descriptor for
        // use in later tests.
        {
            int originalStdoutFD = fileno(stdout);
            ASSERT(-1 != originalStdoutFD);
            ASSERT(0 == fstat(originalStdoutFD, &d_originalStdoutStat));
        }


        if (! freopen("/dev/stdout", "w", stderr)) {

            // Redirect 'stderr' first.

            // We will not be able to redirect 'stderr' after we have
            // redirected 'stdout'.

            if (veryVerbose) {

                // Note that we print this error message on 'stdout' instead of
                // 'stderr', because 'stdout' has not been redirected.

                fprintf(stdout,
                        "Error " __FILE__ "(%d): Failed to redirect stderr\n",
                        __LINE__);
            }
        } else if (! tempFileName(d_fileName)) {

            // Get temp file name

            if (veryVerbose) {

                // Note that we print this error message on 'stdout' instead of
                // 'stderr', because 'stdout' has not been redirected.

                fprintf(stdout,
                    "Error "
                    __FILE__
                    "(%d): Failed to get temp file name for stdout capture\n",
                    __LINE__);
            }
        } else if (! freopen(d_fileName, "w+", stdout)) {

            // Redirect 'stdout'

            if (veryVerbose) {

                // Note that we print this error message on 'stderr', because
                // we have just redirected 'stdout' to the capture file.

                PRINT(d_fileName, "%s");
                fprintf(stderr,
                        "Error " __FILE__ "(%d): Failed to redirect stdout\n",
                        __LINE__);
            }
        } else {

            // 'stderr' and 'stdout' have been successfully redirected.

            d_isFileCreatedFlag = true;
            d_isRedirectedFlag = true;

            if (EOF == fflush(stdout)) {
                if (veryVerbose) {

                    // Note that we print this error message on 'stderr',
                    // because we have just redirected 'stdout' to the capture
                    // file.

                    perror("Error message: ");
                    fprintf(stderr,
                            "Error " __FILE__ "(%d): Error flushing stdout\n",
                            __LINE__);
                }
            }

            success = true;
        }
    }

    return success;
}

void OutputRedirector::reset()
{
    d_outputSize = 0L;
    d_isOutputReadyFlag = false;
    d_outputBuffer[0] = '\0';
    rewind(stdout);
}

bool OutputRedirector::load()
{
    ASSERT(d_isRedirectedFlag);
    ASSERT(!ferror(stdout));

    d_outputSize = ftell(stdout);

    if (d_outputSize + 1 > OUTPUT_BUFFER_SIZE) {

        // Refuse to load output if it will not all fit in the scratch buffer.

        if (veryVerbose) {
            PRINT(d_outputSize, "%ld");
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
            PRINT_(d_outputSize, "%ld"); PRINT(charsRead, "%ld");
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
        return false;                                                 // RETURN
    } else {

        // We have read all output from the capture file.

        d_outputBuffer[d_outputSize] = '\0';

            // ...to ensure that direct inspection of buffer does not overflow
    }

    d_isOutputReadyFlag = true;

    return true;
}

bool OutputRedirector::isRedirected()
{
    return d_isRedirectedFlag;
}

bool OutputRedirector::isOutputReady()
{
    return d_isOutputReadyFlag;
}

char *OutputRedirector::getOutput()
{
    return d_outputBuffer;
}

size_t OutputRedirector::outputSize()
{
    return static_cast<size_t>(d_outputSize);
}

int OutputRedirector::compare(const char *expected)
{
    return compare(expected, strlen(expected));
}

int OutputRedirector::compare(const char *expected, size_t expectedLength)
{
    ASSERT(d_isRedirectedFlag);

    if (!d_isOutputReadyFlag) {
        if (veryVerbose) {
            PRINT(expected, "'%s'");
            fprintf(stderr,
                    "Error " __FILE__ "(%d): No captured output available\n",
                    __LINE__);
        }
        return -1;                                                    // RETURN
    }

    // Use 'memcmp' instead of 'strncmp' to compare 'd_outputBuffer' to
    // 'expected', because 'expected' is allowed to contain embedded nulls.

    return d_outputSize != static_cast<long>(expectedLength) ||
           memcmp(d_outputBuffer, expected, expectedLength);
}

const struct stat& OutputRedirector::originalStdoutStat()
{
    return d_originalStdoutStat;
}

template <typename TEST_TYPE>
struct DataRow {
    int         d_line;              // line number

    TEST_TYPE   d_input;             // value that was printed to 'stdout'

    const char *d_expectedOutput_p;  // expected output string, or '0' if
                                     // expected output should be generated
                                     // with 'printf'

    const char *d_description_p;     // description of this test case
};

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

struct TestDriver {
    enum {
        EXPECTED_OUTPUT_BUFFER_SIZE = 1024, // Size of the buffer used to store
                                            // captured output

        LOOP_ITERATIONS             = 10    // Number of iterations to use when
                                            // testing loop assert macros
    };

    static char expectedOutput[EXPECTED_OUTPUT_BUFFER_SIZE];

    // TEST APPARATUS

    template <typename TEST_TYPE>
    static int generateExpectedOutput(const char      *formatString,
                                      const TEST_TYPE  input);
        // Populate 'expectedOutput' with the result of calling 'printf' with
        // the specified 'formatString' and 'input'.

    // TEST CASES

    static void testCase8(OutputRedirector *output);
        // Test 'BSLS_BSLTESTUTIL_LOOP*_ASSERT' macros.

    template <typename TEST_TYPE>
    static void testCase3(OutputRedirector         *output,
                          const DataRow<TEST_TYPE> *DATA,
                          const int                 NUM_DATA,
                          const char               *formatString);
        // Test 'debugPrint'.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

char TestDriver::expectedOutput[EXPECTED_OUTPUT_BUFFER_SIZE];

template <typename TEST_TYPE>
int TestDriver::generateExpectedOutput(const char      *formatString,
                                       const TEST_TYPE  input)
{
    char tempFormatString[FORMAT_STRING_SIZE];

    int charsWritten = snprintf(tempFormatString,
                                FORMAT_STRING_SIZE,
                                "%%s%s%%s",
                                formatString);
    ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
    charsWritten = snprintf(expectedOutput,
                            EXPECTED_OUTPUT_BUFFER_SIZE,
                            tempFormatString,
                            PREFIX,
                            input,
                            SUFFIX);
    ASSERT(charsWritten >= 0 &&
           charsWritten < EXPECTED_OUTPUT_BUFFER_SIZE);

    return charsWritten;
}

                                 // ----------
                                 // TEST CASES
                                 // ----------

void TestDriver::testCase8(OutputRedirector *output)
{
    // ------------------------------------------------------------------------
    // TESTING BSLS_BSLTESTUTIL_LOOP*_ASSERT MACROS
    //
    // Concerns:
    //: 1 Macros do not call 'aSsErT' and emit no output when the assertion is
    //:   'true'.
    //: 2 Macros call 'aSsErT' and emit output each time the assertion is
    //:   'false'.
    //: 3 Macros emit properly formatted output for each loop variable
    //:   supplied.
    //
    // Plan:
    //: 1 Loop through an arbitrary number of iterations, calling one of the
    //:   loop assert macros with distinct values for each loop variable and an
    //:   assertion that evaluates to 'true'.  Confirm that the value of
    //:   'testStatus' does not change, and that no output is captured by the
    //:   output redirection apparatus.  (C-1)
    //: 2 Loop through an arbitrary number of iterations, calling one of the
    //:   loop assert macros with distinct values for each loop variable and an
    //:   assertion that evaluates to 'false'.  Confirm that 'testStatus'
    //:   increments each time the loop assert macro is called, and that the
    //:   expected error output is captured by the output redirection
    //:   apparatus.  Note that using distinct values for each loop variable
    //:   allows us to detect omissions, repititions or mis-ordering of the
    //:   loop assert macro's arguments.  Also note that we only need to test
    //:   the loop assert macro with one set of variable types, since we test
    //:   separately in test case 3 the ability of the underlying apparatus to
    //:   identify and correctly format each primitive type. (C-2,3)
    //
    // Testing:
    //     BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
    //     BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
    //     BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
    //     BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
    //     BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
    //     BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
    // ------------------------------------------------------------------------

    // [ 8] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP_ASSERT"
                    "\n------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP_ASSERT(I, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(),
                             "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP_ASSERT(I, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput,
                    "%s",
                    output->getOutput(),
                    "%s",
                    0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP2_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(I, J, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\tJ: %d\nError %s(%d): idx > LOOP_ITERATIONS"
                         "    (failed)\n",
                     I,
                     J,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP3_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(I, J, K, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput, "%s", output->getOutput(), "%s",
                              0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP4_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(I, J, K, L, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d",
                              idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput, "%s", output->getOutput(), "%s",
                              0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP5_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(I, J, K, L, M,
                                          idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     M,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }

    // [ 8] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
    {
        if (verbose) {
            fprintf(stderr,
                    "\nTESTING BSLS_BSLTESTUTIL_LOOP6_ASSERT"
                    "\n-------------------------------------\n");
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;
            const int N = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N,
                                          idx < LOOP_ITERATIONS);
            ANNOTATED_ASSERT(testStatus, "%d", testStatus == 0);
            ASSERT(output->load());
            ANNOTATED_ASSERT(output->getOutput(), "%s",
                             0 == output->compare(""));
        }

        ASSERT(testStatus == 0);
        for (int idx = 0; idx < LOOP_ITERATIONS; ++idx) {
            const int I = idx;
            const int J = idx + 1;
            const int K = idx + 2;
            const int L = idx + 3;
            const int M = idx + 3;
            const int N = idx + 3;

            if (veryVerbose) {
                PRINT(idx, "%d");
            }

            output->reset();
            const int LINE = __LINE__ + 1;
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N, idx > LOOP_ITERATIONS);
            ANNOTATED2_ASSERT(testStatus, "%d", idx, "%d",
                              testStatus == idx + 1);
            ASSERT(output->load());
            snprintf(expectedOutput,
                     EXPECTED_OUTPUT_BUFFER_SIZE,
                     "I: %d\tJ: %d\tK: %d\tL: %d\tM: %d\tN: %d\nError %s(%d):"
                         " idx > LOOP_ITERATIONS    (failed)\n",
                     I,
                     J,
                     K,
                     L,
                     M,
                     N,
                     __FILE__,
                     LINE);
            ANNOTATED2_ASSERT(expectedOutput, "%s",
                              output->getOutput(), "%s",
                              0 == output->compare(expectedOutput));
        }
        testStatus = 0;
    }
}

template <typename TEST_TYPE>
void TestDriver::testCase3(OutputRedirector                    *output,
                                      const DataRow<TEST_TYPE> *DATA,
                                      const int                 NUM_DATA,
                                      const char               *formatString)
{
    // ------------------------------------------------------------------------
    // FORMATTED OUTPUT TEST
    //   Ensure that the 'debugPrint' formatted output methods write values to
    //   'stdout' in the expected form
    //
    // Concerns:
    //: 1 The 'debugPrint' method writes the value to 'stdout'.
    //:
    //: 2 The 'debugPrint' method writes the value in the intended format.
    //
    // Plan:
    //: 1 Using the table-driven technique:  (C-1, 2)
    //:
    //:   For each overload of debugPrint, choose a number of significant
    //:   values for the second parameter, and check that each is written to
    //:   'stdout' in the intended format as follows:
    //:
    //:   Reset the output redirector, invoke the method under test with the
    //:   appropriate value for the second parameter, then compare the contents
    //:   of the redirector with the expected form.
    //
    // Testing:
    //   static void debugPrint(const char *s, bool b, const char *t);
    //   static void debugPrint(const char *s, char c, const char *t);
    //   static void debugPrint(const char *s, signed char v, const char *t);
    //   static void debugPrint(const char *s, unsigned char v, const char *t);
    //   static void debugPrint(const char *s, short v, const char *t);
    //   static void debugPrint(const char *s, unsigned short v,
    //                          const char *t);
    //   static void debugPrint(const char *s, int v, const char *t);
    //   static void debugPrint(const char *s, unsigned int v, const char *t);
    //   static void debugPrint(const char *s, long v, const char *t);
    //   static void debugPrint(const char *s, unsigned long v, const char *t);
    //   static void debugPrint(const char *s, long long v, const char *t);
    //   static void debugPrint(const char *s, unsigned long long v,
    //                          const char *t);
    //   static void debugPrint(const char *s, float v, const char *t);
    //   static void debugPrint(const char *s, double v, const char *t);
    //   static void debugPrint(const char *s, long double v, const char *t);
    //   static void debugPrint(const char *s, char *str, const char *t);
    //   static void debugPrint(const char *s, const char *str, const char *t);
    //   static void debugPrint(const char *s, void *p, const char *t);
    //   static void debugPrint(const char *s, const void *p, const char *t);
    // ------------------------------------------------------------------------

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE =       DATA[ti].d_line;
        //ARB: Should INPUT not be capitalized, because it is not necessarily
        // constant?
        TEST_TYPE INPUT =      DATA[ti].d_input;
        const char *EXPECTED = DATA[ti].d_expectedOutput_p;

        int expectedSize = 0;

        if (!EXPECTED) {
            expectedSize = generateExpectedOutput(formatString, INPUT);
            EXPECTED = expectedOutput;
        } else {
            expectedSize = strlen(EXPECTED);
        }

        if (veryVerbose) {
            PRINT_(ti, "%d");
            PRINT_(INPUT, formatString);
            PRINT(EXPECTED, "%s");
        }

        output->reset();
        bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);

        ANNOTATED2_ASSERT(LINE, "%d", INPUT, formatString, output->load());
        ANNOTATED4_ASSERT(LINE, "%d",
                          INPUT, formatString,
                          EXPECTED, "%s",
                          output->getOutput(), "%s",
                          0 == output->compare(EXPECTED, expectedSize));
    }
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    fprintf(stderr, "TEST " __FILE__ " CASE %d\n", test);

    // Capture 'stdout', and send 'stderr' to 'stdout'
    OutputRedirector output;
    if (!output.redirect()) {
        ASSERT(output.isRedirected());
        return 1;                                                     // RETURN
    }

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate the usage example from the header into the test
        //    driver, remove leading comment characters,E  and replace 'assert'
        //    with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING USAGE EXAMPLE"
                    "\n---------------------\n");
        }

        // The actual usage example code is encapsulated in a free function,
        // 'executeUsageExample', so that it can be relocated to the section of
        // the source file where the standard test macros have been defined in
        // terms of the macros supplied by the component under test.

        executeUsageExample(verbose, veryVerbose, veryVeryVerbose);
      } break;
      case 8: {
          // ------------------------------------------------------------------
          // TESTING BSLS_BSLTESTUTIL_LOOP*_ASSERT MACROS
          //
          // Testing:
          //     BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
          //     BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
          //     BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
          //     BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
          //     BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
          //     BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
          // ------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING LOOP ASSERT MACROS"
                    "\n--------------------------\n");
        }

        TestDriver::testCase8(&output);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // IDENTIFIER OUTPUT MACRO TEST
        //
        // Concerns:
        //: 1 Identifier output macro emits output.
        //: 2 Output emitted is in correct format
        //
        // Plan:
        //: 1 Call 'BSLS_BSLTESTUTIL_Q' with a series of arbitrary identifiers
        //:   containing single, and multiple tokens, with an without initial,
        //:   final, and repeated whitespace and compare the captured output to
        //:   a model string. (C-1,2)

        //
        // Testing:
        //    BSLS_BSLTESTUTIL_Q(X)
        // --------------------------------------------------------------------

        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_Q"
                        "\n--------------------------\n");
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(sample);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| sample |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(embedded white   space);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| embedded white space |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(   initial whitespace);
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| initial whitespace |>\n"));
            }

            {
                output.reset();
                BSLS_BSLTESTUTIL_Q(final whitespace   );
                ASSERT(output.load());
                ASSERT(0 == output.compare("<| final whitespace |>\n"));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VALUE OUTPUT MACRO TEST
        //
        // Concerns:
        //: 1 Value output macros emit output.
        //: 2 Output emitted is in correct format
        //
        // Plan
        //: 1 Call the value output macros on a variable of known value, and
        //:   confirm that the captured output is in the correct format.  Note
        //:   that it is only necessary to conduct this test once with a single
        //:   variable type, because the underlying type-differentiation and
        //:   formatting mechanisms are tested in test case 3. (C-1,2)
        //
        // Testing:
        //    BSLS_BSLTESTUTIL_P(X)
        //    BSLS_BSLTESTUTIL_P_(X)
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING VALUE OUTPUT MACROS"
                    "\n---------------------------\n");
        }

        // [ 6] BSLS_BSLTESTUTIL_P(X)
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_P"
                        "\n--------------------------\n");
            }

            const int INPUT = 42;

            output.reset();
            BSLS_BSLTESTUTIL_P(INPUT);
            ASSERT(output.load());
            ANNOTATED_ASSERT(output.getOutput(), "%s",
                             0 == output.compare("INPUT = 42\n"));
        }

        // [ 6] BSLS_BSLTESTUTIL_P_(X)
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_P_"
                        "\n---------------------------\n");
            }

            const int INPUT = 42;

            output.reset();
            BSLS_BSLTESTUTIL_P_(INPUT);
            ASSERT(output.load());
            ANNOTATED_ASSERT(output.getOutput(), "%s",
                             0 == output.compare("INPUT = 42, "));
        }

     } break;
      case 5: {
        // --------------------------------------------------------------------
        // STATIC MACRO TEST
        //
        // Concerns:
        //: 1 Line number macro has the correct value.
        //: 2 Tab output macro emits output.
        //: 3 Tab output macro output emitted is in correct format
        //
        // Plan
        //: 1 Compare the value of the line number macro to '__LINE__'. (C-1)
        //: 2 Call the tab output macro, and confirm that the captured output
        //:   is in the correct format.  (C-2,3)
        //
        // Testing:
        //    BSLS_BSLTESTUTIL_L_
        //    BSLS_BSLTESTUTIL_T_
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING STATIC MACROS"
                    "\n---------------------\n");
        }

        // [ 5] BSLS_BSLTESTUTIL_L_
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_L_"
                        "\n---------------------------\n");
            }

            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);
        }

        // [ 5] BSLS_BSLTESTUTIL_T_
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING BSLS_BSLTESTUTIL_T_"
                        "\n---------------------------\n");
            }

            output.reset();
            BSLS_BSLTESTUTIL_T_
            ASSERT(output.load());
            ASSERT(0 == output.compare("\t"));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // UNFORMATTED OUTPUT TEST
        //
        // Concerns:
        //: 1 Unformatted output methods emit output.
        //: 2 Output emitted is in correct format
        //: 3 'printStringNoFlush' does not flush output between calls
        //
        // Plan
        //: 1 Using the table-driven technique, call 'printStringNoFlush' with
        //:   a variety of arguments, and check that the captured output is
        //:   correct.  (C-1,2)
        //: 2 Call 'printStringNoFlush' twice in succession, and check that the
        //:   captured output matches the concatenation of the input strings,
        //:   with no embedded nulls or newlines.  (C-3)
        //: 3 Call 'printTab' and check that the captured output is correct.
        //:   (C-1,2)
        //
        // Testing:
        //    static void printStringNoFlush(const char *s);
        //    static void printTab();
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING UNFORMATTED OUPUT METHODS"
                    "\n---------------------------------\n");
        }

        // [ 4] static void printStringNoFlush(const char *s);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING printStringNoFlush"
                        "\n--------------------------\n");
            }

            static const DataRow<const char *> DATA[] =
            {
                //LINE       INPUT    OUTPUT    DESC
                //---------- -----    ------    ----

                { __LINE__, "",       "",       "empty string" },
                { __LINE__, "a",      "a",      "non-empty string" },
                { __LINE__, "a\nb",   "a\nb",   "embedded newline" },
                { __LINE__, "a\r\nb", "a\r\nb", "embedded <CRLF>" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const char *INPUT =    DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                if (veryVerbose) {
                    PRINT_(ti, "%d"); PRINT(INPUT, "'%s'");
                }

                output.reset();
                bsls::BslTestUtil::printStringNoFlush(INPUT);
                ANNOTATED2_ASSERT(LINE, "%d", INPUT, "%s", output.load());
                ANNOTATED4_ASSERT(LINE, "%d",
                                  INPUT, "%s",
                                  EXPECTED, "%s",
                                  output.getOutput(), "%s",
                                  0 == output.compare(EXPECTED));
            }

            output.reset();
            bsls::BslTestUtil::printStringNoFlush("abc");
            bsls::BslTestUtil::printStringNoFlush("def");
            ASSERT(output.load());
            ASSERT(0 == output.compare("abcdef"));
        }

        // [ 4] static void printTab();
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING printTab"
                        "\n----------------\n");
            }

            output.reset();
            bsls::BslTestUtil::printTab();
            ASSERT(output.load());
            ASSERT(0 == output.compare("\t"));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FORMATTED OUTPUT TEST
        //
        // Testing:
        //   static void debugPrint(const char *s, bool b, const char *t);
        //   static void debugPrint(const char *s, char c, const char *t);
        //   static void debugPrint(const char *s, signed char v,
        //                          const char *t);
        //   static void debugPrint(const char *s, unsigned char v,
        //                          const char *t);
        //   static void debugPrint(const char *s, short v, const char *t);
        //   static void debugPrint(const char *s, unsigned short v,
        //                          const char *t);
        //   static void debugPrint(const char *s, int v, const char *t);
        //   static void debugPrint(const char *s, unsigned int v,
        //                          const char *t);
        //   static void debugPrint(const char *s, long v, const char *t);
        //   static void debugPrint(const char *s, unsigned long v,
        //                          const char *t);
        //   static void debugPrint(const char *s, long long v, const char *t);
        //   static void debugPrint(const char *s, unsigned long long v,
        //                          const char *t);
        //   static void debugPrint(const char *s, float v, const char *t);
        //   static void debugPrint(const char *s, double v, const char *t);
        //   static void debugPrint(const char *s, long double v,
        //                          const char *t);
        //   static void debugPrint(const char *s, char *str, const char *t);
        //   static void debugPrint(const char *s, const char *str,
        //                          const char *t);
        //   static void debugPrint(const char *s, void *p, const char *t);
        //   static void debugPrint(const char *s, const void *p,
        //                          const char *t);
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING FORMATTED OUTPUT METHODS"
                    "\n--------------------------------\n");
        }

        // [ 3] static void debugPrint(const char *s, bool b, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR bool"
                        "\n---------------------------\n");
            }

            static const DataRow<bool> DATA[] =
            {
                //LINE       INPUT  OUTPUT                 DESC
                //---------- -----  ------                 ----

                { __LINE__,  true,  PREFIX "true" SUFFIX,  "true" },
                { __LINE__,  false, PREFIX "false" SUFFIX, "false" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<bool>(&output, DATA, NUM_DATA, "%d");
        }

        // [ 3] static void debugPrint(const char *s, char c, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR char"
                        "\n---------------------------\n");
            }

            static const DataRow<char> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  '\0',     0,     "\\0" },
                { __LINE__,  CHAR_MIN, 0,     "CHAR_MIN" },
                { __LINE__,  CHAR_MAX, 0,     "CHAR_MAX" },
                { __LINE__,  '\x01',   0,     "\\x01" },
                { __LINE__,  '\t',     0,     "tab character" },
                { __LINE__,  '\n',     0,     "newline character" },
                { __LINE__,  '\r',     0,     "carriage return character" },
                { __LINE__,  'A',      0,     "positive signed character" },
                { __LINE__,  '\xcc',   0,     "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<char>(&output, DATA, NUM_DATA, "'%c'");
        }

        // [ 3] static void debugPrint(const char *s, signed char v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR signed char"
                        "\n----------------------------------\n");
            }

            static const DataRow<signed char> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  '\0',      0,     "null character" },
                { __LINE__,  '\x01',    0,     "\\x01" },
                { __LINE__,  SCHAR_MAX, 0,     "SCHAR_MAX" },
                { __LINE__,  SCHAR_MIN, 0,     "SCHAR_MIN" },
                { __LINE__,  '\x50',    0,     "positive signed character" },
                { __LINE__,  '\xcc',    0,     "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<signed char>(&output,
                                               DATA,
                                               NUM_DATA,
                                               "%hhd");
        }

        // [ 3] static void debugPrint(const char *s, unsigned char v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR unsigned char"
                        "\n------------------------------------\n");
            }

            static const DataRow<unsigned char> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  '\0',      0,     "null character" },
                { __LINE__,  '\x01',    0,     "\\x01" },
                { __LINE__,  UCHAR_MAX, 0,     "UCHAR_MAX" },
                { __LINE__,  '\x50',    0,     "positive signed character" },
                { __LINE__,  '\xcc',    0,     "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<unsigned char>(&output,
                                                 DATA,
                                                 NUM_DATA,
                                                 "%hhu");
        }

        // [ 3] static void debugPrint(const char *s, short v, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR short"
                        "\n----------------------------\n");
            }

            static const DataRow<short> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  -1,       0,     "-1" },
                { __LINE__,  SHRT_MAX, 0,     "SHRT_MAX" },
                { __LINE__,  SHRT_MIN, 0,     "SHRT_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<short>(&output, DATA, NUM_DATA, "%hd");
        }

        // [ 3] static void debugPrint(const char *s, unsigned short v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR unsigned short"
                        "\n-------------------------------------\n");
            }

            static const DataRow<unsigned short> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0,         0,     "0" },
                { __LINE__,  1,         0,     "1" },
                { __LINE__,  USHRT_MAX, 0,     "USHRT_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<unsigned short>(&output,
                                                  DATA,
                                                  NUM_DATA,
                                                  "%hu");
        }

        // [ 3] static void debugPrint(const char *s, int v, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR int"
                        "\n--------------------------\n");
            }

            static const DataRow<int> DATA[] =
            {
                //LINE       INPUT    OUTPUT DESC
                //---------- -----    ------ ----

                { __LINE__,  0,       0,     "0" },
                { __LINE__,  1,       0,     "1" },
                { __LINE__,  -1,      0,     "-1" },
                { __LINE__,  INT_MAX, 0,     "INT_MAX" },
                { __LINE__,  INT_MIN, 0,     "INT_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<int>(&output, DATA, NUM_DATA, "%d");
        }

        // [ 3] static void debugPrint(const char *s, unsigned int v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR unsigned int"
                        "\n-----------------------------------\n");
            }

            static const DataRow<unsigned int> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0,        0,     "0" },
                { __LINE__,  1,        0,     "1" },
                { __LINE__,  UINT_MAX, 0,     "UINT_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<unsigned int>(&output, DATA, NUM_DATA, "%u");
        }

        // [ 3] static void debugPrint(const char *s, long v, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR long"
                        "\n---------------------------\n");
            }

            static const DataRow<long> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  0L,       0,     "0L" },
                { __LINE__,  1L,       0,     "1L" },
                { __LINE__,  -1L,      0,     "-1L" },
                { __LINE__,  LONG_MAX, 0,     "LONG_MAX" },
                { __LINE__,  LONG_MIN, 0,     "LONG_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<long>(&output, DATA, NUM_DATA, "%ld");
        }

        // [ 3] static void debugPrint(const char *s, unsigned long v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR unsigned long"
                        "\n------------------------------------\n");
            }

            static const DataRow<unsigned long> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0UL,       0,     "0UL" },
                { __LINE__,  1UL,       0,     "1UL" },
                { __LINE__,  ULONG_MAX, 0,     "ULONG_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<unsigned long>(&output,
                                                 DATA,
                                                 NUM_DATA,
                                                 "%lu");
        }

        // [ 3] static void debugPrint(const char *s, long long v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR long long"
                        "\n--------------------------------\n");
            }

            static const DataRow<long long> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0LL,       0,     "0LL" },
                { __LINE__,  1LL,       0,     "1LL" },
                { __LINE__,  -1LL,      0,     "-1LL" },
                { __LINE__,  LLONG_MAX, 0,     "LLONG_MAX" },
                { __LINE__,  LLONG_MIN, 0,     "LLONG_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<long long>(&output, DATA, NUM_DATA, "%lld");
        }

        // [ 3] static void debugPrint(const char *s, unsigned long long v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR unsigned long long"
                        "\n-----------------------------------------\n");
            }

            static const DataRow<unsigned long long> DATA[] =
            {
                //LINE       INPUT       OUTPUT DESC
                //---------- -----       ------ ----

                { __LINE__,  0ULL,       0,     "0ULL" },
                { __LINE__,  1ULL,       0,     "1ULL" },
                { __LINE__,  ULLONG_MAX, 0,     "ULLONG_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<unsigned long long>(&output,
                                                      DATA,
                                                      NUM_DATA,
                                                      "%llu");
        }

        // [ 3] static void debugPrint(const char *s, float v, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR float"
                        "\n----------------------------\n");
            }

            static const DataRow<float> DATA[] =
            {
                //LINE       INPUT        OUTPUT DESC
                //---------- -----        ------ ----

                { __LINE__,  0.0F,        0,     "0.0F" },
                { __LINE__,  1.0F,        0,     "1.0F" },
                { __LINE__,  -1.0F,       0,     "-1.0F" },
                { __LINE__,  1.0F / 3.0F, 0,     "(float) 1/3" },
                { __LINE__,  FLT_MAX,     0,     "FLT_MAX" },
                { __LINE__,  FLT_MIN,     0,     "FLT_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<float>(&output, DATA, NUM_DATA, "%f");
        }

        // [ 3] static void debugPrint(const char *s, double v, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR double"
                        "\n-----------------------------\n");
            }

            static const DataRow<double> DATA[] =
            {
                //LINE       INPUT      OUTPUT DESC
                //---------- -----      ------ ----

                { __LINE__,  0.0,       0,     "0.0" },
                { __LINE__,  1.0,       0,     "1.0" },
                { __LINE__,  -1.0,      0,     "-1.0" },
                { __LINE__,  1.0 / 3.0, 0,     "(double) 1/3" },
                { __LINE__,  DBL_MAX,   0,     "DBL_MAX" },
                { __LINE__,  DBL_MIN,   0,     "DBL_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<double>(&output, DATA, NUM_DATA, "%g");
        }

        // [ 3] static void debugPrint(const char *s, long double v,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR long double"
                        "\n----------------------------------\n");
            }

            static const DataRow<long double> DATA[] =
            {
                //LINE       INPUT        OUTPUT DESC
                //---------- -----        ------ ----

                { __LINE__,  0.0L,        0,     "0.0L" },
                { __LINE__,  1.0L,        0,     "1.0L" },
                { __LINE__,  -1.0L,       0,     "-1.0L" },
                { __LINE__,  1.0L / 3.0L, 0,     "(long double) 1/3" },
                { __LINE__,  LDBL_MAX,    0,     "LDBL_MAX" },
                { __LINE__,  LDBL_MIN,    0,     "LDBL_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver::testCase3<long double>(&output, DATA, NUM_DATA, "%Lg");
        }

        // [ 3] static void debugPrint(const char *s, char *str,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR char *"
                        "\n-----------------------------\n");
            }

            static const DataRow<char *> DATA[] =
            {
                //LINE       INPUT                         OUTPUT          DESC
                //---------- -----                         ------          ----

                { __LINE__,  const_cast<char *>(""),       0,
                                                              "empty string" },
                { __LINE__,  const_cast<char *>("a"),      0,
                                                          "non-empty string" },
                { __LINE__,  const_cast<char *>("a\nb"),   0,
                                              "string with embedded newline" },
                { __LINE__,  const_cast<char *>("a\r\nb"), 0,
                                               "string with embedded <CRLF>" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver::testCase3<char *>(&output, DATA, NUM_DATA, "\"%s\"");
        }

        // [ 3] static void debugPrint(const char *s, const char *str,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR const char *"
                        "\n-----------------------------------\n");
            }

            static const DataRow<const char *> DATA[] =
            {
                //LINE       INPUT     OUTPUT DESC
                //---------- -----     ------ ----

                { __LINE__,  "",       0,     "empty string" },
                { __LINE__,  "a",      0,     "non-empty string" },
                { __LINE__,  "a\nb",   0,     "string with embedded newline" },
                { __LINE__,  "a\r\nb", 0,     "string with embedded <CRLF>" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver::testCase3<const char *>(&output,
                                                DATA,
                                                NUM_DATA,
                                                "\"%s\"");
        }

        // [ 3] static void debugPrint(const char *s, void *p, const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR void *"
                        "\n-----------------------------\n");
            }

            static const DataRow<void *> DATA[] =
            {
                //LINE       INPUT             OUTPUT DESC
                //---------- -----             ------ ----

                { __LINE__,  (void *) 0,       0,     "NULL pointer" },
                { __LINE__,  (void *) &output, 0,     "valid address" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver::testCase3<void *>(&output, DATA, NUM_DATA, "%p");
        }
        // [ 3] static void debugPrint(const char *s, const void *p,
        //                             const char *t);
        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING debugPrint FOR const void *"
                        "\n-----------------------------------\n");
            }

            static const DataRow<const void *> DATA[] =
            {
                //LINE       INPUT                   OUTPUT DESC
                //---------- -----                   ------ ----

                { __LINE__,  (const void *) 0,       0,     "NULL pointer" },
                { __LINE__,  (const void *) &output, 0,     "valid address" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver::testCase3<const void *>(&output, DATA, NUM_DATA, "%p");
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS TEST
        //
        // Concerns:
        //:  1 Output is redirected
        //:  2 Captured output is readable
        //:  3 'load' works
        //:  4 'reset' works
        //:  5 'compare' works
        //:  6 Incorrect output is correctly diagnosed
        //:  7 Embedded newlines work
        //:  8 Empty output works
        //:  9 Embedded nulls work
        //: 10 Filesystem-dependent control sequences work
        //: 11 stderr points to original target of 'stdout'
        //
        // Plan:
        //:  1 Confirm that 'ftell(stdout)' succeeds.  This demonstrates that
        //:    'stdout' is a seekable file. (C-1)
        //:  2 Write a string to 'stdout', confirm that 'stdout's seek position
        //:    has changed, read back the contents of 'stdout' and compare them
        //:    to the original string.  (C-2)
        //:  3 Write a string to 'stdout'.  Confirm that
        //:    'OutputRedirector::load' changes the contents of the output
        //:    buffer and that it changes the result of
        //:    'OutputRedirector::isOutputReady' from 'false' to 'true'.
        //:    Confirm that the contents of the output buffer match the
        //:    original string.  (C-3)
        //:  4 Write a string to 'stdout' and load it with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::reset' rewinds 'stdout', changes the output
        //:    of 'OutputRedirector::isOutputReady' from 'true' to 'false' and
        //:    sets the length of the output buffer to 0.  (C-4)
        //:  5 Write a string to 'stdout' and read it back with
        //:    'OutputRedirector::load'.  Confirm that
        //:    'OutputRedirector::compare' gives the correct results when the
        //:    captured output is compared with the following data:  (C-5)
        //:
        //:        Data                           Comparison Result
        //:    ------------                   -------------------------
        //:    input string                           true
        //:    input string with appended data        false
        //:    input string truncated                 false
        //:    string different from input:
        //:    at beginning                           false
        //:    at end                                 false
        //:    elsewhere                              false
        //:
        //:  6 Confirm that 'load' fails when there is more data in 'stdout'
        //:    than can be fit in the capture buffer.  Confirm that 'compare'
        //:    fails if 'load' has not been first called to read data into the
        //:    capture buffer.  (C-6)
        //:  7 Confirm that strings containing embedded newlines are correctly
        //:    captured and correctly identified by 'compare'.  (C-7)
        //:  8 Write an empty string to 'stdout'.  Confirm that it can be
        //:    correctly loaded and compared with the original.  (C-8)
        //:  9 Write a series of strings to 'stdout', containing '\0' at the
        //:    beginning, end or interior of the string.  Confirm that the
        //:    captured output can be correctly loaded and compared with the
        //:    original input.  (C-9)
        //: 10 Write a series of strings to 'stdout' containing '^D' and
        //:    '<CRLF>' and confirm that these strings are correctly captured
        //:    and loaded.  (C-10)
        //: 11 Use 'fstat' to find out the device and inode of the current
        //:    (post-redirection) 'stderr'.  Compare these values to the device
        //:    and inode of 'stdout' before redirection.  (C-11)
        // --------------------------------------------------------------------

        if (verbose) {
            fprintf(stderr,
                    "\nTESTING TEST APPARATUS"
                    "\n----------------------\n");
        }

        {
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT REDIRECTION"
                        "\n--------------------------\n");
            }

            // 1 Output is redirected
            ASSERT(-1 != ftell(stdout));
        }

        {
            // 2 Captured output is readable
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE"
                        "\n----------------------\n");
            }

            enum { TEST_STRING_SIZE = 15 };
            const char *testString = "This is output";
            char buffer[TEST_STRING_SIZE];

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
            // 3 'load' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE LOAD"
                        "\n---------------------------\n");
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

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
                               output.getOutput(),
                               output.outputSize()));
        }

        {
            // 4 'reset' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE RESET"
                        "\n----------------------------\n");
            }

            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

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
            // 5 'compare' works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE VERIFICATION"
                        "\n-----------------------------------\n");
            }

            const char *testString = "This is output";
            const char *longString = "This is outputA";
            const char *shortString = "This is outpu";
            const char *differentStartString = "Xhis is output";
            const char *differentEndString = "This is outpuy";
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
            // 6 Incorrect output is correctly diagnosed
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING OUTPUT CAPTURE ERROR"
                        "\n----------------------------\n");
            }

            // Reset verbosity levels to suppress expected error output
            const bool tempVeryVerbose = veryVerbose;
            const bool tempVeryVeryVerbose = veryVeryVerbose;
            veryVerbose = false;
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

            veryVerbose = tempVeryVerbose;
            veryVeryVerbose = tempVeryVeryVerbose;
        }

        {
            // 7 Embedded newlines work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMBEDDED NEWLINES"
                        "\n-------------------------\n");
            }

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
            // 8 Empty output works
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMPTY OUTPUT"
                        "\n--------------------\n");
            }

            const char *testString = "";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.compare(testString));
        }

        {
            // 9 Embedded nulls work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING EMBEDDED NULLS"
                        "\n----------------------\n");
            }

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
            // 10 Filesystem-dependent control sequences work
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING CONTROL SEQUENCES"
                        "\n-------------------------\n");
            }

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
            //: 11 stderr points to original target of stdout
            if (verbose) {
                fprintf(stderr,
                        "\nTESTING STDERR REDIRECTION"
                        "\n--------------------------\n");
            }

            int newStderrFD = fileno(stderr);
            ASSERT(-1 != newStderrFD);
            struct stat stderrStat;
            ASSERT(-1 != fstat(newStderrFD, &stderrStat));
            ASSERT(stderrStat.st_dev == output.originalStdoutStat().st_dev);
            ASSERT(stderrStat.st_ino == output.originalStdoutStat().st_ino);
            ASSERT(stderrStat.st_rdev == output.originalStdoutStat().st_rdev);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component.
        //
        // Plan:
        //   Invoke each of the "print" macros, protected by a check for
        //   'verbose' mode, and manually inspect the output to the console.
        //   Invoke each 'LOOPx_ASSERT' macro with a test that passes, and
        //   again with a test that fails only in verbose mode.  Then reset the
        //   'testStatus' count to reflect the expected number of failed
        //   'LOOPx_ASSERT's.
        //
        // Testing:
        //   BREATHING TEST
        //   BSLS_BSLTESTUTIL_P()
        //   BSLS_BSLTESTUTIL_P_()
        //   BSLS_BSLTESTUTIL_Q()
        //   BSLS_BSLTESTUTIL_T_
        //   BSLS_BSLTESTUTIL_L_
        //   BSLS_BSLTESTUTIL_LOOP_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP2_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP3_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP4_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP5_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP6_ASSERT
        // --------------------------------------------------------------------

        if (verbose) fprintf(stderr, "\nBREATHING TEST"
                                     "\n==============\n");

        const bool                b       = true;
        const char                c       = 'c';
        const signed char         sc      = -42;
        const unsigned char       uc      = 142;
        const short               ss      = -1234;
        const unsigned short      us      = 1234;
        const int                 si      = -123456;
        const unsigned int        ui      = 123456;
        const long                sl      = -7654321l;
        const unsigned long       ul      = 7654321ul;
        const long long           sll     = -5123467890ll;
        const unsigned long long  ull     = 9876543210ull;
        const float               f       = 2.78f;
        const double              d       = 3.14159268;
        const long double         ld      = 1.608e300L;
        char                      hello[] = { 'h', 'e', 'l', 'l', 'o', '\0' };
        char                     *s       = hello;
        const char                world[] = { 'w', 'o', 'r', 'l', 'd', '\0' };
        const char               *cs      = world;
        void                     *pv      = hello;
        const void               *pcv     = &si;

        // Check verbose so that we do not unexpectedly write to the console
        if (verbose) {
            BSLS_BSLTESTUTIL_P(b)
            BSLS_BSLTESTUTIL_P(c)
            BSLS_BSLTESTUTIL_P(sc)
            BSLS_BSLTESTUTIL_P(uc)
            BSLS_BSLTESTUTIL_P(ss)
            BSLS_BSLTESTUTIL_P(us)
            BSLS_BSLTESTUTIL_P(si)
            BSLS_BSLTESTUTIL_P(ui)
            BSLS_BSLTESTUTIL_P(sl)
            BSLS_BSLTESTUTIL_P(ul)
            BSLS_BSLTESTUTIL_P(sll)

            BSLS_BSLTESTUTIL_P(ull)
            BSLS_BSLTESTUTIL_P(f)
            BSLS_BSLTESTUTIL_P(d)
            BSLS_BSLTESTUTIL_P(ld)
            BSLS_BSLTESTUTIL_P(s)
            BSLS_BSLTESTUTIL_P(cs)
            BSLS_BSLTESTUTIL_P(pv)
            BSLS_BSLTESTUTIL_P(pcv)

            BSLS_BSLTESTUTIL_P_(b)
            BSLS_BSLTESTUTIL_P_(c)
            BSLS_BSLTESTUTIL_P_(sc)
            BSLS_BSLTESTUTIL_P_(uc)
            BSLS_BSLTESTUTIL_P_(ss)
            BSLS_BSLTESTUTIL_P_(us)
            BSLS_BSLTESTUTIL_P_(si)
            BSLS_BSLTESTUTIL_P_(ui)
            BSLS_BSLTESTUTIL_P_(sl)

            BSLS_BSLTESTUTIL_P_(ul)
            BSLS_BSLTESTUTIL_P_(sll)
            BSLS_BSLTESTUTIL_P_(ull)
            BSLS_BSLTESTUTIL_P_(f)
            BSLS_BSLTESTUTIL_P_(d)
            BSLS_BSLTESTUTIL_P_(ld)
            BSLS_BSLTESTUTIL_P_(s)
            BSLS_BSLTESTUTIL_P_(cs)
            BSLS_BSLTESTUTIL_P_(pv)

            BSLS_BSLTESTUTIL_P_(pcv)

            BSLS_BSLTESTUTIL_Q(BSLS_BSLTESTUTIL_L_)
            BSLS_BSLTESTUTIL_T_
            BSLS_BSLTESTUTIL_P(BSLS_BSLTESTUTIL_L_)
        }

        BSLS_BSLTESTUTIL_LOOP_ASSERT(b, true == b);
        BSLS_BSLTESTUTIL_LOOP2_ASSERT(c, sc, 'c' == c && -42 == sc);
        BSLS_BSLTESTUTIL_LOOP3_ASSERT(uc, ss, us,
                       142 == uc &&
                     -1234 == ss &&
                      1234 == us);
        BSLS_BSLTESTUTIL_LOOP4_ASSERT(si, ui, sl, ul,
                   -123456 == si &&
                    123456 == ui &&
                  -7654321 == sl &&
                   7654321 == ul);
        BSLS_BSLTESTUTIL_LOOP5_ASSERT(si, ui, sl, ul, sll,
                         -123456   == si &&
                          123456   == ui &&
                        -7654321l  == sl &&
                         7654321ul == ul &&
                     -5123467890ll == sll);
        BSLS_BSLTESTUTIL_LOOP6_ASSERT(si, ui, sl, ul, sll, ull,
                        -123456    == si  &&
                         123456    == ui  &&
                       -7654321l   == sl  &&
                        7654321ul  == ul  &&
                    -5123467890ll  == sll &&
                     9876543210ull == ull);

        ASSERT(testStatus == 0);

        if (verbose) {
            BSLS_BSLTESTUTIL_LOOP_ASSERT(verbose, !verbose);
            BSLS_BSLTESTUTIL_LOOP2_ASSERT(verbose, c, !verbose);
            BSLS_BSLTESTUTIL_LOOP3_ASSERT(verbose, uc, sc, !verbose);
            BSLS_BSLTESTUTIL_LOOP4_ASSERT(verbose, f, d, ld, !verbose);
            BSLS_BSLTESTUTIL_LOOP5_ASSERT(verbose, s, cs, pv, pcv, !verbose);
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(verbose,
                                          ss,
                                          us,
                                          sll,
                                          ull,
                                          si,
                                          !verbose);

            ASSERT(testStatus == 6);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
