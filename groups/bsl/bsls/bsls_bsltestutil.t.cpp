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
//                    STANDARD BDE ASSERT TEST MACROS
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
# define ASSERT(X) { realaSsErT(!(X), #X, __LINE__); }

// ============================================================================
//            VARIATIONS ON STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
//
// The standard BDE LOOP-ASSERT test macros rely on overloads of 'operator<<'
// ostream to provide a uniform interface for all types.  The purpose of
// bsls_testutil is precisely to provide a similar overloading mechanism for
// printf-based output.  In order to use the standard macros in this test
// driver, we would have to define a test appartus of equivalent complexity to
// the class we are testing.  Therefore, instead of the standard test macros we
// define a limited number of type-specific macros in their place.

#define LOOP3_ASSERT_STRING_INT_CHAR(I,J,K,X) {                                             \
    if (!(X)) { fprintf(stderr, "%s: %s\t%s: %d\t%s: %c\n", #I, I, #J, J, #K, K);            \
              realaSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT_STRING_INT(I,J,X) {                                             \
    if (!(X)) { fprintf(stderr, "%s: %s\t%s: %d\n", #I, I, #J, J);            \
              realaSsErT(1, #X, __LINE__); } }

#define LOOP_ASSERT_STRING(I,X) {                                                \
    if (!(X)) { fprintf(stderr, "%s: %s\n", #I, I); realaSsErT(1, #X, __LINE__);}}

#define LOOP_ASSERT_LONG(I,X) {                                                \
    if (!(X)) { fprintf(stderr, "%s: %ld\n", #I, I); realaSsErT(1, #X, __LINE__);}}

#define LOOP_ASSERT_INT(I,X) {                                                \
    if (!(X)) { fprintf(stderr, "%s: %d\n", #I, I); realaSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT_INT(I,J,X) {                                             \
    if (!(X)) { fprintf(stderr, "%s: %d\t%s: %d\n", #I, I, #J, J);            \
              realaSsErT(1, #X, __LINE__); } }

#if 0

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; realaSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       realaSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       realaSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

#endif

#define OUTPUT_BUFFER_SIZE 4096

//=============================================================================
//                      HELPER FUNCTION FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
// Usage example code assumes that BSLS_BSLTESTUTIL_* macros have been renamed
// to replace the standard test macros.  In order to simplify the rest of the
// test driver, the standard macros are redefined after 'main', and the usage
// example case just calls a funtion, 'executeUsageExample', which is defined
// at the very end of the file.

static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

void executeUsageExample(bool verbose, bool veryVerbose, bool veryVeryVerbose);

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

    if (d_outputSize > OUTPUT_BUFFER_SIZE) {
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
    if (!d_isOutputReadyFlag) {
        if (d_isVerboseFlag) {
            fprintf(stderr, "Error " __FILE__ "(%d): No captured output available for comparison with '%s'    (failed)\n", __LINE__, expected);
        }
        return -1;
    }

    size_t len = strlen(expected);
    size_t limit = len < d_outputSize ? len : d_outputSize;

    int result = strncmp(d_outputBuffer, expected, limit);
    
    return result ? result : d_outputSize - len;
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
      case 4: {
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
      case 3: {
        // --------------------------------------------------------------------
        // PRINT OPERATIONS TEST

        if (verbose) {
            fprintf(stderr, 
                    "\nTESTING PRINT OPERATIONS"
                    "\n------------------------\n");
        }

        {
            if (verbose) {
                fprintf(stderr, 
                        "\nTESTING BSLS_BSLTESTUTIL_Q"
                        "\n--------------------------\n");
            }

            output.reset();
            BSLS_BSLTESTUTIL_Q(42);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp("<| 42 |>\n"));

            output.reset();
            BSLS_BSLTESTUTIL_Q(0);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp("<| 0 |>\n"));

            output.reset();
            BSLS_BSLTESTUTIL_Q(-42);
            ASSERT(output.load());
            ASSERT(0 == output.strcmp("<| -42 |>\n"));

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
            ASSERT(strcmp(testString, longString) == output.strcmp(longString));
            ASSERT(strcmp(testString, shortString) == output.strcmp(shortString));
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

//=============================================================================
//                             USAGE EXAMPLE CODE
//-----------------------------------------------------------------------------
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
    // static int testStatus = 0;
    // 
    // static void aSsErT(bool b, const char *s, int i)
    // {
    //     if (b) {
    //         printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
    //         if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    //     }
    // }

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


// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
