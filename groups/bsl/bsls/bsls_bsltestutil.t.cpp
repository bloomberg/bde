// bsls_bsltestutil.t.cpp                                             -*-C++-*-

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strnlen
#include <limits.h> // PATH_MAX on linux
#include <unistd.h>
#include <fcntl.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single utility struct and 11 macros
// that call methods of the utility struct.  All of the methods write output to
// stdout and do nothing more.  Therefore, there are no Primary Manipulators or
// Basic Accessors to test.  All of the tests fall under the category of Print
// Operations.
//
// The main difficulty with writing the test driver is capturing the output of
// the methods under test so that it can be checked for accuracy.  In addition,
// error messages and other output produced by the test driver itself must
// still appear on stdout for compatibility with the standard build and testing
// scripts.
//
// Global Concerns:
//: o The test driver can reliably capture stdout and play back captured output.
//: o The test driver's own output is written to the calling environment's stdout.
//
//-----------------------------------------------------------------------------
// [ 7] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
// [ 7] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
// [ 7] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
// [ 7] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
// [ 7] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
// [ 7] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
// [ 6] BSLS_BSLTESTUTIL_Q(X)
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
// [ 3] static void debugPrint(const char *s, unsigned long long v, const char *t);
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
// [ 1] Breathing test
// [ 2] Test Apparatus test
// [ 0] Usage example
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
// BSLS_BSLTESTUTIL_* macros, the standard macros output to stderr instead of
// stdout.

static int realTestStatus = 0;

static void realaSsErT(int c, const char *s, int i)
{
    if (c) {
        fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (realTestStatus >= 0 && realTestStatus <= 100) ++realTestStatus;
    }
}

// The standard ASSERT macro definition is deferred until after the usage
// example code
// #define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

// ============================================================================
//               VARIATIONS ON STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// The standard BDE LOOP-ASSERT test macros rely on overloads of 'operator<<'
// ostream to provide a uniform interface for all types.  The purpose of
// bsls_testutil is precisely to provide a similar overloading mechanism for
// printf-based output.  In order to use the standard macros in this test
// driver, we would have to define a test appartus of equivalent complexity to
// the class we are testing.  Therefore, instead of the standard test macros we
// define a limited number of type-specific macros in their place.

#define PRINTF_LOOP_ASSERT(I,IF,X) {                                           \
    if (!(X)) { fprintf(stderr, "%s: " IF "\n", #I, I);                      \
        realaSsErT(1, #X, __LINE__);}}

#define PRINTF_LOOP2_ASSERT(I,IF,J,JF,X) {                                     \
    if (!(X)) { fprintf(stderr, "%s: " IF "\t%s: " JF "\n", #I, I, #J, J);   \
        realaSsErT(1, #X, __LINE__); } }

#define PRINTF_LOOP3_ASSERT(I,IF,J,JF,K,KF,X) {                                     \
    if (!(X)) { fprintf(stderr, "%s: " IF "\t%s: " JF "\t%s: " KF "\n", #I, I, #J, J, #K, K);   \
        realaSsErT(1, #X, __LINE__); } }

#define PRINTF_LOOP4_ASSERT(I,IF,J,JF,K,KF,L,LF,X) {                                     \
    if (!(X)) { fprintf(stderr, "%s: " IF "\t%s: " JF "\t%s: " KF "\t%s: " LF "\n", #I, I, #J, J, #K, K, #L, L);   \
        realaSsErT(1, #X, __LINE__); } }

#define PRINTF_LOOP5_ASSERT(I,IF,J,JF,K,KF,L,LF,M,MF,X) {                                     \
    if (!(X)) { fprintf(stderr, "%s: " IF "\t%s: " JF "\t%s: " KF "\t%s: " LF "\t%s: " MF "\n", #I, I, #J, J, #K, K, #L, L, #M, M);   \
        realaSsErT(1, #X, __LINE__); } }

#define OUTPUT_BUFFER_SIZE 4096

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------
//
// Usage example code assumes that BSLS_BSLTESTUTIL_* macros have been renamed
// to replace the standard test macros.  In order to simplify the rest of the
// test driver, the standard macros are redefined after 'main', and the usage
// example case just calls a funtion, 'executeUsageExample', which is defined
// at the very end of the file.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First, we write a component to test, which provides a utility class:
//..
    struct xyza_BslExampleUtil {
        // This utility class provides sample functionality to demonstrate how
        // a test driver might be written validating its only method.

        static int fortyTwo();
            // Return the integer value '42'.
    };

    inline
    int xyza_BslExampleUtil::fortyTwo()
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
// Next, we define the standard print and LOOP_ASSERT macros, as aliases to the
// macros defined by this component:
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
    #define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
    #define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
    #define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Finally, we write the test case for the 'static' 'fortyTwo' method, using
// the (standard) abbreviated macro names we have just defined.
//..
    void executeUsageExample(bool verbose, bool veryVerbose, bool veryVeryVerbose)
    {
        (void) veryVerbose;
        (void) veryVeryVerbose;

        const int value = xyza_BslExampleUtil::fortyTwo();
        if (verbose) P(value);
        LOOP_ASSERT(value, 42 == value);
    }

//=============================================================================
//                    CLEANUP STANDARD TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#undef ASSERT
#define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

#undef L_
#define L_ __LINE__                           // current Line number

//=============================================================================
//                             HELPER FUNCTIONS
//-----------------------------------------------------------------------------

enum {
#ifdef BSLS_PLATFORM__OS_WINDOWS
    PATH_BUFFER_SIZE = MAX_PATH
#elif defined(BSLS_PLATFORM__OS_HPUX)
    PATH_BUFFER_SIZE = L_tmpnam
#else
    //TODO: replace PATH_MAX with something portable
    PATH_BUFFER_SIZE = PATH_MAX
#endif
};

bool tempFileName(char *result, bool verboseFlag)
{
    ASSERT(result);

#ifdef BSLS_PLATFORM__OS_WINDOWS
    char tmpPathBuf[MAX_PATH];
    if (! GetTempPath(MAX_PATH, tmpPathBuf) || ! GetTempFileName(tmpPathBuf, "bsls", 0, result)) {
        return false;
    }
#elif defined(BSLS_PLATFORM__OS_HPUX)
    if(! tempnam(result, "bsls")) {
        return false;
    }
#else
    char *fn = tempnam(0, "bsls");
    if (fn) {
        strncpy(result, fn, PATH_BUFFER_SIZE);
        free(fn);
    } else {
        return false;
    }
#endif

    if (verboseFlag) printf("\tUse '%s' as a base filename.\n", result);

    // Test Invariant:
    ASSERT('\0' != result[0]); // result not empty

    return true;
}

class OutputRedirector {
  private:
    char d_fileName[PATH_BUFFER_SIZE];
    char d_outputBuffer[OUTPUT_BUFFER_SIZE];

    bool d_isRedirectedFlag;
    bool d_isFileCreatedFlag;
    long d_outputSize;
    bool d_isOutputReadyFlag;
    bool d_isVerboseFlag;

  public:
    // CREATORS
    explicit OutputRedirector(bool isVerboseFlag);
    ~OutputRedirector();

    // MANIPULATORS
    bool redirect();
    void reset();
    bool load();

    // ACCESSORS
    bool isRedirected();
    bool isOutputReady();
    char *getOutput();
    int strcmp(const char *expected);
    int strcmp(const char *expected, size_t expectedLength);
};

OutputRedirector::OutputRedirector(bool isVerboseFlag)
    : d_isRedirectedFlag(false)
    , d_isFileCreatedFlag(false)
    , d_outputSize(0L)
    , d_isOutputReadyFlag(false)
    , d_isVerboseFlag(isVerboseFlag)
{
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
        if (d_isVerboseFlag) {
            fprintf(stderr, "Warning " __FILE__ "(%d): Output already redirected\n", __LINE__);
        }
        success = true;
    } else if (! freopen("/dev/stdout", "w+", stderr)) {
            // Redirect stderr
        if (d_isVerboseFlag) {
            fprintf(stdout, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Failed to redirect stderr");
        }
    } else if (! tempFileName(d_fileName, d_isVerboseFlag)) {
            // Get temp file name
        if (d_isVerboseFlag) {
            fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Failed to get temp file name for stdout capture");
        }
    } else if (! freopen(d_fileName, "w+", stdout)) {
            // Redirect stdout
        if (d_isVerboseFlag) {
            fprintf(stderr, "d_fileName: %s\n", d_fileName);
            fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Failed to redirect stdout");
        }
    } else {
        d_isFileCreatedFlag = true;
        d_isRedirectedFlag = true;

        if (EOF == fflush(stdout)) {
            if (d_isVerboseFlag) {
                fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Error flushing stdout");
            }
        }

        success = true;
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
    d_outputSize = ftell(stdout);                  

    if (d_outputSize + 1 > OUTPUT_BUFFER_SIZE) {
        if (d_isVerboseFlag) {
            fprintf(stderr, "d_outputSize: %ld\n", d_outputSize);
            fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Captured output exceeds read buffer size");
        }
        d_outputSize = 0L;
        return false;
    }

    rewind(stdout);                                   

    long charsRead = fread(d_outputBuffer, sizeof(char), d_outputSize, stdout);
    if (d_outputSize != charsRead) {
        if (d_isVerboseFlag) {
            fprintf(stderr, "d_outputSize: %ld\tcharsRead: %ld\n", d_outputSize, charsRead);
            fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", __LINE__, "Could not read all captured output");
        }
        return false;
    } else {
        d_outputBuffer[d_outputSize] = '\0'; // To ensure that direct inspection of buffer does not overflow
    }

    d_isOutputReadyFlag = true;

    return true;
}

bool OutputRedirector::isRedirected()
{
    return d_isRedirectedFlag;
}

bool OutputRedirector::isOutputReady() {
    return d_isOutputReadyFlag;
}

char *OutputRedirector::getOutput() {
    return d_outputBuffer;
}

int OutputRedirector::strcmp(const char *expected)
{
    return strcmp(expected, strlen(expected));
}

int OutputRedirector::strcmp(const char *expected, size_t expectedLength)
{
    if (!d_isOutputReadyFlag) {
        if (d_isVerboseFlag) {
            fprintf(stderr, "Error " __FILE__ "(%d): No captured output available for comparison with '%s'    (failed)\n", __LINE__, expected);
        }
        return -1;
    }

    size_t len = strlen(expected);
    size_t limit = expectedLength < d_outputSize ? expectedLength : d_outputSize;

    int result = strncmp(d_outputBuffer, expected, limit);
    
    return result ? result : d_outputSize - expectedLength;
}

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

template <typename TEST_TYPE>
struct DataRow {
    int d_line;
    TEST_TYPE d_input;
    const char *d_expectedOutput_p;
    const char *d_description_p;
};

#define PREFIX "<"
#define SUFFIX ">"
static const char prefixString[] = PREFIX;
static const char suffixString[] = SUFFIX;

template <typename TEST_TYPE>
struct TestDriver {
    static void testCase3(OutputRedirector *output, const DataRow<TEST_TYPE> *DATA, const int NUM_DATA, const char *formatString);
};

#define EXPECTED_OUTPUT_BUFFER_SIZE 1024
#define FORMAT_STRING_SIZE 256

template <typename TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase3(OutputRedirector *output, const DataRow<TEST_TYPE> *DATA, const int NUM_DATA, const char *formatString)
{
    static char expectedOutputBuffer[EXPECTED_OUTPUT_BUFFER_SIZE];
    static char tempFormatString[FORMAT_STRING_SIZE];

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE =       DATA[ti].d_line;
        TEST_TYPE INPUT =      DATA[ti].d_input;
        const char *EXPECTED = DATA[ti].d_expectedOutput_p;

        int expectedSize = 0;
    
        if (!EXPECTED) {
            int charsWritten = snprintf(tempFormatString, FORMAT_STRING_SIZE, "%%s%s%%s", formatString);
            ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
            charsWritten = snprintf(expectedOutputBuffer, EXPECTED_OUTPUT_BUFFER_SIZE, tempFormatString, PREFIX, INPUT, SUFFIX);
            ASSERT(charsWritten >= 0 && charsWritten < EXPECTED_OUTPUT_BUFFER_SIZE); 
            EXPECTED = expectedOutputBuffer;
            expectedSize = charsWritten;
        } else {
            expectedSize = strlen(EXPECTED);
        }
        
        output->reset();
        bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);

        if (!(output->load())) {
            int charsWritten = snprintf(tempFormatString, FORMAT_STRING_SIZE,  "%%s: " "%%d" "\t%%s: " "%s" "\n", formatString);
            ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
            fprintf(stderr, tempFormatString, "LINE", LINE, "INPUT", INPUT);
            realaSsErT(1, "output->load()", __LINE__);
        }

        if(!(0 == output->strcmp(EXPECTED, expectedSize))) {
            int charsWritten = snprintf(tempFormatString, FORMAT_STRING_SIZE,  "%%s: " "%%d" "\t%%s: " "%s" "\t%%s: " "%%s" "\t%%s: " "%%s" "\n", formatString);
            ASSERT(charsWritten >= 0 && charsWritten < FORMAT_STRING_SIZE);
            fprintf(stderr, tempFormatString, "LINE", LINE, "INPUT", INPUT, "EXPECTED", EXPECTED, "output->getOutput()", output->getOutput());
            realaSsErT(1, "0 == output->strcmp(EXPECTED, expectedSize)", __LINE__);
        }
    } 
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    //setbuf(stdout, 0);    // Use unbuffered output on stdout


    fprintf(stderr, "TEST " __FILE__ " CASE %d\n", test);

    // Capture stdout, and send stderr to stdout
    OutputRedirector output(veryVerbose);
    if (!output.redirect()) {
        return 1;
    }

    switch (test) { case 0:
      case 8: {
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
        
        executeUsageExample(verbose, veryVerbose, veryVeryVerbose);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // LOOP ASSERT MACRO TEST

        // [ 7] BSLS_BSLTESTUTIL_LOOP_ASSERT(I,X)
        // [ 7] BSLS_BSLTESTUTIL_LOOP2_ASSERT(I,J,X)
        // [ 7] BSLS_BSLTESTUTIL_LOOP3_ASSERT(I,J,K,X)
        // [ 7] BSLS_BSLTESTUTIL_LOOP4_ASSERT(I,J,K,L,X)
        // [ 7] BSLS_BSLTESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)
        // [ 7] BSLS_BSLTESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // VALUE OUTPUT MACRO TEST


        // [ 6] BSLS_BSLTESTUTIL_Q(X)
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
                ASSERT(0 == output.strcmp("<| sample |>\n"));

                output.reset();
                BSLS_BSLTESTUTIL_Q(42);
                ASSERT(output.load());
                ASSERT(0 == output.strcmp("<| 42 |>\n"));
            }
        }

        // [ 6] BSLS_BSLTESTUTIL_P(X)
        {
            static const DataRow<int> DATA[] =
            {
                //LINE INPUT OUTPUT         DESC
                //---- ----- ------         ----
            
                { L_,    42, "input = 42\n",  "Positive Integer" },
                { L_,     0, "input = 0\n",   "Zero"             },
                { L_,   -42, "input = -42\n", "Negative Integer" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                int input =            DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                BSLS_BSLTESTUTIL_P(input);
                PRINTF_LOOP2_ASSERT(LINE, "%d", input, "%d", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", input, "%d", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
        }

        // [ 6] BSLS_BSLTESTUTIL_P_(X)
        {
            static const DataRow<int> DATA[] =
            {
                //LINE INPUT OUTPUT         DESC
                //---- ----- ------         ----
            
                { L_,    42, "input = 42, ",  "Positive Integer" },
                { L_,     0, "input = 0, ",   "Zero"             },
                { L_,   -42, "input = -42, ", "Negative Integer" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                int input =            DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                BSLS_BSLTESTUTIL_P_(input);
                PRINTF_LOOP2_ASSERT(LINE, "%d", input, "%d", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", input, "%d", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
        }

     } break;
      case 5: {
        // --------------------------------------------------------------------
        // STATIC OUTPUT MACRO TEST

        // [ 5] BSLS_BSLTESTUTIL_L_
        {
            ASSERT(__LINE__ == BSLS_BSLTESTUTIL_L_);
        }

        // [ 5] BSLS_BSLTESTUTIL_T_
        {
            output.reset();
            BSLS_BSLTESTUTIL_T_
            ASSERT(output.load());
            ASSERT(0 == output.strcmp("\t"));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STATIC OUTPUT TEST

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING PRINT OPERATIONS"
                    "\n------------------------\n");
        }

        // [ 4] static void printStringNoFlush(const char *s);
        {
            static const DataRow<char *> DATA[] =
            {
                //LINE INPUT OUTPUT         DESC
                //---- ----- ------         ----
            
                { L_,    "", "",  "Empty cstring" },
                { L_,    "a", "a",   "Non-empty cstring"             },
                { L_,    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         ,
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#$"
                         , "Long cstring"             },
                { L_,    "first line\nsecond line", "first line\nsecond line",  "CString containing newline" },
                { L_,    "first\nsecond\nthird", "first\nsecond\nthird",  "CString containing multiple newlines" },
                { L_,    "first line\n", "first line\n",  "CString containing final newline" },
                { L_,    "first\nsecond\nthird\n", "first\nsecond\nthird\n",  "CString containing multiple newlines including final" },
                { L_,    "\nsecond line", "\nsecond line",  "CString containing initial newline" },
                { L_,    "\nfirst\nsecond\nthird", "\nfirst\nsecond\nthird",  "CString containing multiple newlines including initial" },
                { L_,    "first\tsecond", "first\tsecond",  "CString containing tab" },
                { L_,    "first\t", "first\t",  "CString containing final tab" },
                { L_,    "\tfirst", "\tfirst",  "CString containing initial tab" },
                { L_,    "first\rsecond", "first\rsecond",  "CString containing carriage return" },
                { L_,    "\rfirst", "\rfirst",  "CString containing initial carriage return" },
                { L_,    "first\r", "first\r",  "CString containing final carriage return" },
                { L_,    "first\r\nsecond", "first\r\nsecond",  "CString containing carriage return/newline" },
                { L_,    "\r\nfirst", "\r\nfirst",  "CString containing initial carriage return/newline" },
                { L_,    "first\r\n", "first\r\n",  "CString containing final carriage return/newline" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const char *INPUT =    DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::printStringNoFlush(INPUT);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%s", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%s", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
        }

        // [ 4] static void printTab();
        {
            output.reset();
            bsls::BslTestUtil::printTab();
            ASSERT(output.load());
            ASSERT(0 == output.strcmp("\t"));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FORMATTED OUTPUT TEST
	//   Ensure that the 'debugPrint' formatted output methods write values
	//   to stdout in the expected form
	//
	// Concerns:
	//: 1 The 'debugPrint' method writes the value to stdout.
	//:
	//: 2 The 'debugPrint' method writes the value in the intended format.
	//
	// Plan:
	//: 1 Using the table-driven technique:  (C-1, 2)
	//:
	//:   For each overload of debugPrint, choose a number of significant
	//:   values for the second parameter, and check that each is written
	//:   to stdout in the intended format as follows:
	//:
	//:   Reset the output redirector, invoke the method under test with
	//:   the appropriate value for the second parameter, then compare the
	//:   contents of the redirector with the expected form.
	//
	// Testing:
        //   static void debugPrint(const char *s, bool b, const char *t);
        //   static void debugPrint(const char *s, char c, const char *t);
        //   static void debugPrint(const char *s, signed char v, const char *t);
        //   static void debugPrint(const char *s, unsigned char v, const char *t);
        //   static void debugPrint(const char *s, short v, const char *t);
        //   static void debugPrint(const char *s, unsigned short v, const char *t);
        //   static void debugPrint(const char *s, int v, const char *t);
        //   static void debugPrint(const char *s, unsigned int v, const char *t);
        //   static void debugPrint(const char *s, long v, const char *t);
        //   static void debugPrint(const char *s, unsigned long v, const char *t);
        //   static void debugPrint(const char *s, long long v, const char *t);
        //   static void debugPrint(const char *s, unsigned long long v, const char *t);
        //   static void debugPrint(const char *s, float v, const char *t);
        //   static void debugPrint(const char *s, double v, const char *t);
        //   static void debugPrint(const char *s, long double v, const char *t);
        //   static void debugPrint(const char *s, char *str, const char *t);
        //   static void debugPrint(const char *s, const char *str, const char *t);
        //   static void debugPrint(const char *s, void *p, const char *t);
        //   static void debugPrint(const char *s, const void *p, const char *t);
	// --------------------------------------------------------------------

        // [ 3] static void debugPrint(const char *s, bool b, const char *t);
        {
            output.reset();
            bsls::BslTestUtil::debugPrint(PREFIX, true, SUFFIX);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(PREFIX "true" SUFFIX));

            output.reset();
            bsls::BslTestUtil::debugPrint(PREFIX, false, SUFFIX);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(PREFIX "false" SUFFIX));

            static const DataRow<bool> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, true, "<true>",      "true" },
		{ L_, false, "<false>",    "false" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            TestDriver<bool>::testCase3(&output, DATA, NUM_DATA, "%d");
        }

        // [ 3] static void debugPrint(const char *s, char c, const char *t);
        {
	    const size_t EXPECTED_SIZE = 3 + strlen(PREFIX) + strlen(SUFFIX);
	    static char maxCharString[3 * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxCharString, sizeof maxCharString, "%s'%c'%s", PREFIX, CHAR_MAX, SUFFIX);
	    static char minCharString[3 * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minCharString, sizeof minCharString, "%s'%c'%s", PREFIX, CHAR_MIN, SUFFIX);

            static const DataRow<char> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, '\0', (const char *) 0,      "\\0" },
		{ L_, CHAR_MIN, minCharString, "CHAR_MIN" },
		{ L_, CHAR_MAX, maxCharString, "CHAR_MAX" },
                { L_, '\x01', PREFIX "'\x01'" SUFFIX,      "\\x01" },
                { L_, '\t',   PREFIX "'\t'" SUFFIX,        "tab character" },
                { L_, '\n',   PREFIX "'\n'" SUFFIX,        "newline character" },
                { L_, '\r',   PREFIX "'\r'" SUFFIX,        "carriage return character" },
                { L_, 'A',    PREFIX "'A'" SUFFIX,         "positive signed character" },
                { L_, '\xcc',    PREFIX "'\xcc'" SUFFIX,   "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) { break;
                const int LINE =       DATA[ti].d_line;
                const char INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%c", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%c", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED, EXPECTED_SIZE));
            }
            TestDriver<char>::testCase3(&output, DATA, NUM_DATA, "'%c'");
        }

        // [ 3] static void debugPrint(const char *s, signed char v, const char *t);
        {
	    static char maxCharString[((CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxCharString, sizeof maxCharString, "%s%hhd%s", PREFIX, SCHAR_MAX, SUFFIX);
	    static char minCharString[((CHAR_BIT - 1) / 3 + 2) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minCharString, sizeof minCharString, "%s%hhd%s", PREFIX, SCHAR_MIN, SUFFIX);

            static const DataRow<signed char> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, '\0',   PREFIX "0" SUFFIX,        "null character" },
                { L_, '\x01', PREFIX "1" SUFFIX,      "\\x01" },
		{ L_, SCHAR_MAX, maxCharString, "SCHAR_MAX" },
		{ L_, SCHAR_MIN, minCharString, "SCHAR_MIN" },
                { L_, '\x50', PREFIX "80" SUFFIX,       "positive signed character" },
                { L_, '\xcc', PREFIX "-52" SUFFIX,      "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const signed char INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%hhd", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%hhd", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<signed char>::testCase3(&output, DATA, NUM_DATA, "%hhd");
        }

        // [ 3] static void debugPrint(const char *s, unsigned char v, const char *t);
        {
	    static char maxCharString[(CHAR_BIT / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxCharString, sizeof maxCharString, "%s%hhu%s", PREFIX, UCHAR_MAX, SUFFIX);

            static const DataRow<unsigned char> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, '\0',   PREFIX "0" SUFFIX,        "null character" },
                { L_, '\x01', PREFIX "1" SUFFIX,      "\\x01" },
		{ L_, UCHAR_MAX, maxCharString, "UCHAR_MAX" },
                { L_, '\x50', PREFIX "80" SUFFIX,       "positive signed character" },
                { L_, '\xcc', PREFIX "204" SUFFIX,      "negative signed character" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const unsigned char INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%hhu", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%hhu", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<unsigned char>::testCase3(&output, DATA, NUM_DATA, "%hhu");
        }

        // [ 3] static void debugPrint(const char *s, short v, const char *t);
        {
	    static char maxShortString[((sizeof(short) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxShortString, sizeof maxShortString, "%s%hd%s", PREFIX, SHRT_MAX, SUFFIX);
	    static char minShortString[((sizeof(short) * CHAR_BIT - 1) / 3 + 2) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minShortString, sizeof minShortString, "%s%hd%s", PREFIX, SHRT_MIN, SUFFIX);

            static const DataRow<short> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0,   PREFIX "0" SUFFIX,        "0" },
                { L_, 1, PREFIX "1" SUFFIX,      "1" },
                { L_, -1, PREFIX "-1" SUFFIX,      "-1" },
		{ L_, SHRT_MAX, maxShortString, "SHRT_MAX" },
		{ L_, SHRT_MIN, minShortString, "SHRT_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const short INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%hd", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%hd", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<short>::testCase3(&output, DATA, NUM_DATA, "%hd");
        }

        // [ 3] static void debugPrint(const char *s, unsigned short v, const char *t);
        {
	    static char maxShortString[((sizeof(unsigned short) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxShortString, sizeof maxShortString, "%s%hu%s", PREFIX, USHRT_MAX, SUFFIX);

            static const DataRow<unsigned short> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0,   PREFIX "0" SUFFIX,        "0" },
                { L_, 1, PREFIX "1" SUFFIX,      "1" },
		{ L_, USHRT_MAX, maxShortString, "USHRT_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const unsigned short INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%hu", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%hu", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<unsigned short>::testCase3(&output, DATA, NUM_DATA, "%hu");
        }

        // [ 3] static void debugPrint(const char *s, int v, const char *t);
        {
	    static char maxIntString[((sizeof(int) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxIntString, sizeof maxIntString, "%s%d%s", PREFIX, INT_MAX, SUFFIX);
	    static char minIntString[((sizeof(int) * CHAR_BIT - 1) / 3 + 2) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minIntString, sizeof minIntString, "%s%d%s", PREFIX, INT_MIN, SUFFIX);

            static const DataRow<int> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0,   PREFIX "0" SUFFIX,        "0" },
                { L_, 1, PREFIX "1" SUFFIX,      "1" },
                { L_, -1, PREFIX "-1" SUFFIX,      "-1" },
		{ L_, INT_MAX, maxIntString, "INT_MAX" },
		{ L_, INT_MIN, minIntString, "INT_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const int INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%d", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%d", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<int>::testCase3(&output, DATA, NUM_DATA, "%d");
        }

        // [ 3] static void debugPrint(const char *s, unsigned int v, const char *t);
        {
	    static char maxIntString[((sizeof(unsigned int) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxIntString, sizeof maxIntString, "%s%u%s", PREFIX, UINT_MAX, SUFFIX);

            static const DataRow<unsigned int> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0,   PREFIX "0" SUFFIX,        "0" },
                { L_, 1, PREFIX "1" SUFFIX,      "1" },
		{ L_, UINT_MAX, maxIntString, "UINT_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const unsigned int INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%u", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%u", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<unsigned int>::testCase3(&output, DATA, NUM_DATA, "%u");
        }

        // [ 3] static void debugPrint(const char *s, long v, const char *t);
        {
	    static char maxLongString[((sizeof(long) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxLongString, sizeof maxLongString, "%s%ld%s", PREFIX, LONG_MAX, SUFFIX);
	    static char minLongString[((sizeof(long) * CHAR_BIT - 1) / 3 + 2) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minLongString, sizeof minLongString, "%s%ld%s", PREFIX, LONG_MIN, SUFFIX);

            static const DataRow<long> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0L,   PREFIX "0" SUFFIX,        "0L" },
                { L_, 1L, PREFIX "1" SUFFIX,      "1L" },
                { L_, -1L, PREFIX "-1" SUFFIX,      "-1L" },
		{ L_, LONG_MAX, maxLongString, "LONG_MAX" },
		{ L_, LONG_MIN, minLongString, "LONG_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const long INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%ld", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%ld", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<long>::testCase3(&output, DATA, NUM_DATA, "%ld");
        }

        // [ 3] static void debugPrint(const char *s, unsigned long v, const char *t);
        {
	    static char maxLongString[((sizeof(unsigned long) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxLongString, sizeof maxLongString, "%s%lu%s", PREFIX, ULONG_MAX, SUFFIX);

            static const DataRow<unsigned long> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0UL,   PREFIX "0" SUFFIX,        "0UL" },
                { L_, 1UL, PREFIX "1" SUFFIX,      "1UL" },
		{ L_, ULONG_MAX, maxLongString, "ULONG_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const unsigned long INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%lu", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%lu", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<unsigned long>::testCase3(&output, DATA, NUM_DATA, "%lu");
        }

        // [ 3] static void debugPrint(const char *s, long long v, const char *t);
        {
	    static char maxLongLongString[((sizeof(long long) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxLongLongString, sizeof maxLongLongString, "%s%lld%s", PREFIX, LLONG_MAX, SUFFIX);
	    static char minLongLongString[((sizeof(long long) * CHAR_BIT - 1) / 3 + 2) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(minLongLongString, sizeof minLongLongString, "%s%lld%s", PREFIX, LLONG_MIN, SUFFIX);

            static const DataRow<long long> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0LL,   PREFIX "0" SUFFIX,        "0LL" },
                { L_, 1LL, PREFIX "1" SUFFIX,      "1LL" },
                { L_, -1LL, PREFIX "-1" SUFFIX,      "-1LL" },
		{ L_, LLONG_MAX, maxLongLongString, "LLONG_MAX" },
		{ L_, LLONG_MIN, minLongLongString, "LLONG_MIN" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const long long INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%lld", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%lld", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<long long>::testCase3(&output, DATA, NUM_DATA, "%lld");
        }

        // [ 3] static void debugPrint(const char *s, unsigned long long v, const char *t);
        {
	    static char maxLongLongString[((sizeof(unsigned long long) * CHAR_BIT - 1) / 3 + 1) * sizeof(char) + sizeof prefixString + sizeof suffixString];
	    snprintf(maxLongLongString, sizeof maxLongLongString, "%s%llu%s", PREFIX, ULLONG_MAX, SUFFIX);

            static const DataRow<unsigned long long> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, 0ULL,   PREFIX "0" SUFFIX,        "0ULL" },
                { L_, 1ULL, PREFIX "1" SUFFIX,      "1ULL" },
		{ L_, ULLONG_MAX, maxLongLongString, "ULLONG_MAX" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE =       DATA[ti].d_line;
                const unsigned long long INPUT =      DATA[ti].d_input;
                const char *EXPECTED = DATA[ti].d_expectedOutput_p;

                output.reset();
                bsls::BslTestUtil::debugPrint(PREFIX, INPUT, SUFFIX);
                PRINTF_LOOP2_ASSERT(LINE, "%d", INPUT, "%llu", output.load());
                PRINTF_LOOP4_ASSERT(LINE, "%d", INPUT, "%llu", EXPECTED, "%s", output.getOutput(), "%s", 0 == output.strcmp(EXPECTED));
            }
            TestDriver<unsigned long long>::testCase3(&output, DATA, NUM_DATA, "%llu");
        }

        // [ 3] static void debugPrint(const char *s, float v, const char *t);
        // [ 3] static void debugPrint(const char *s, double v, const char *t);
        // [ 3] static void debugPrint(const char *s, long double v, const char *t);
        // [ 3] static void debugPrint(const char *s, char *str, const char *t);
        // [ 3] static void debugPrint(const char *s, const char *str, const char *t);
        // [ 3] static void debugPrint(const char *s, void *p, const char *t);
	{
            static const DataRow<void *> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, (void *) 0,              (const char *) 0,      "NULL pointer" },
                { L_, (void *) &output, (const char *) 0,      "valid char *" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver<void *>::testCase3(&output, DATA, NUM_DATA, "%p");
	}
        // [ 3] static void debugPrint(const char *s, const void *p, const char *t);
	{
            static const DataRow<const void *> DATA[] =
            {
                //LINE INPUT  OUTPUT         DESC
                //---- -----  ------         ----
            
                { L_, (const void *) 0,              (const char *) 0,      "NULL pointer" },
                { L_, (const void *) &output, (const char *) 0,      "valid char *" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            TestDriver<const void *>::testCase3(&output, DATA, NUM_DATA, "%p");
	}

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS TEST
        //
        // Concerns:
        //: 1 Output is captured
        //: 2 Captured output is accurate
        //: 3 Non-captured output is on stdout
        //: 4 No-newline works
        //: 5 Empty output works
        //: 6 Rewind works
        //: 7 Can distinguish between open and closed redirect targets

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING TEST APPARATUS"
                    "\n----------------------\n");
        }

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING OUTPUT CAPTURE"
                    "\n----------------------\n");
        }

        {
            // 1 Output is captured
            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stdout);
            ASSERT(0 == ftell(stdout));
            printf("%s", testString);
            ASSERT(testStringLength == ftell(stdout));
        }

        {
            // 2 Captured output is accurate
            enum { TEST_STRING_SIZE = 15 };
            const char *testString = "This is output";
            char buffer[TEST_STRING_SIZE];

            ASSERT(TEST_STRING_SIZE == strlen(testString) + 1);

            rewind(stdout);
            ASSERT(0 == ftell(stdout));
            printf("%s", testString);
            long outputSize = ftell(stdout);
            ASSERT(outputSize + 1 == TEST_STRING_SIZE);
            rewind(stdout);
            ASSERT(outputSize == fread(buffer, sizeof(char), outputSize, stdout));
            buffer[TEST_STRING_SIZE - 1] = '\0';
            ASSERT(0 == strcmp(testString, buffer));
        }

        {
            // 3 reset works
            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            rewind(stdout);
            printf("%s", testString);
            ASSERT(testStringLength == ftell(stdout));
            output.reset();
            ASSERT(0 == ftell(stdout));
        }

        {
            // 4 load works
            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            output.reset();
            ASSERT(0 == ftell(stdout));
            ASSERT(false == output.isOutputReady());
            ASSERT(0 == strlen(output.getOutput()));
            printf("%s", testString);
            ASSERT(testStringLength == ftell(stdout));
            ASSERT(false == output.isOutputReady());
            ASSERT(0 == strlen(output.getOutput()));
            ASSERT(output.load());
            ASSERT(testStringLength == ftell(stdout));
            ASSERT(true == output.isOutputReady());
            ASSERT(testStringLength == strlen(output.getOutput()));
        }

        {
            // 5 strcmp works
            const char *testString = "This is output";
            const char *longString = "This is outputA";
            const char *shortString = "This is outpu";
            const char *highValueString = "This is outpuu";
            const char *lowValueString = "This is outpus";
            size_t testStringLength = strlen(testString);

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(strcmp(testString, testString) == output.strcmp(testString));
            ASSERT((strcmp(testString, shortString) > 0) == (output.strcmp(shortString) > 0));
            ASSERT((strcmp(testString, shortString) < 0) == (output.strcmp(shortString) < 0));
            ASSERT((strcmp(testString, longString) > 0) == (output.strcmp(longString) > 0));
            ASSERT((strcmp(testString, longString) < 0) == (output.strcmp(longString) < 0));
            ASSERT(strcmp(testString, highValueString) == output.strcmp(highValueString));
            ASSERT(strcmp(testString, lowValueString) == output.strcmp(lowValueString));
        }

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING ERROR CONDITIONS"
                    "\n------------------------\n");
        }

        {
            // 6 Incorrect output is correctly diagnosed

            const char *testString =         "This is good output";
            const char *wrongLengthString =  "This is too short";
            const char *wrongContentString = "This is not correct";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 != output.strcmp(wrongLengthString));

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 != output.strcmp(wrongContentString));
 
            output.reset();
            int stringLength = strlen(testString);
            for (int idx = 0; idx * stringLength < OUTPUT_BUFFER_SIZE; ++idx) {
                printf("%s", testString);
            }
            printf("%s", testString);
            ASSERT(!output.load());
            ASSERT(-1 == output.strcmp(testString));
        }

        {
            // 7 strcmp before load produces error
            const char *testString = "This is output";

            output.reset();
            printf("%s", testString);
            ASSERT(0 != output.strcmp(testString));
        }

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING EMBEDDED NEWLINES"
                    "\n-------------------------\n");
        }

        {
            // 8 Embedded newlines work
            const char *testString = "This has an\nembedded newline";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(testString));
            
            const char *twoNewlineTestString = "This has two\nembedded newlines\n";

            output.reset();
            printf("%s", twoNewlineTestString);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(twoNewlineTestString));
        }

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING EMPTY OUTPUT"
                    "\n--------------------\n");
        }

        {
            // 9 Empty output works
            const char *testString = "";

            output.reset();
            printf("%s", testString);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(testString));
        }

        {
            // 10 Embedded nulls work
            const char *testString1 = "abc\0def";
            const char *testString2 = "\0def";
            const char *testString3 = "abc\0";

            output.reset();
            printf("%s", testString1);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(testString1));

            output.reset();
            printf("%s", testString2);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(testString2));

            output.reset();
            printf("%s", testString3);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(testString3));
        }

        {
            // 11 Filesystem-dependent control sequences work
            const char *crnlTestString = "ab\r\ncd";
            const char *ctrlDTestString = "ab" "\x04" "cd";

            output.reset();
            printf("%s", crnlTestString);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(crnlTestString));

            output.reset();
            printf("%s", ctrlDTestString);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp(ctrlDTestString));
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

        const bool b = true;
        const char c = 'c';
        const signed char sc = -42;
        const unsigned char uc = 142;
        const short ss = -1234;
        const unsigned short us = 1234;
        const int si = -123456;
        const unsigned int ui = 123456;
        const long sl = -7654321l;
        const unsigned long ul = 7654321ul;
        const long long sll = -5123467890ll;
        const unsigned long long ull = 9876543210ull;
        const float f = 2.78f;
        const double d = 3.14159268;
        const long double ld = 1.608e300L;
        char hello[] = { 'h', 'e', 'l', 'l', 'o', '\0' };
        char *s = hello;
        const char world[] = { 'w', 'o', 'r', 'l', 'd', '\0' };
        const char *cs = world;
        void *pv = hello;
        const void *pcv = &si;

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
            BSLS_BSLTESTUTIL_LOOP6_ASSERT(verbose, ss, us, sll, ull, si, !verbose);

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
