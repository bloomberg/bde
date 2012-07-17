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
//-----------------------------------------------------------------------------
// [ ]
// [ ]
// [ ]
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 2] Usage example
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

#define CAPTURE_RESET rewind(stdout)

#define CAPTURE_ASSERT(expected) checkCapture((expected), stdout, __LINE__)

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

// fileName has to be visible to cleanTempFile and to redirectOutputs
#ifdef BSLS_PLATFORM__OS_WINDOWS
static char fileName[MAX_PATH];
#elif defined(BSLS_PLATFORM__OS_HPUX)
static char fileName[L_tmpnam];
#else
//TODO: replace PATH_MAX with something portable
static char fileName[PATH_MAX];
#endif

void cleanTempFile() {
    fclose(stdout);
    unlink(fileName);
}

bool tempFileName(char *result, bool verboseFlag)
{
    ASSERT(result);

#ifdef BSLS_PLATFORM__OS_WINDOWS
    const size_t maxSize = MAX_PATH;
    char tmpPathBuf[MAX_PATH];
    if (! GetTempPath(MAX_PATH, tmpPathBuf) || ! GetTempFileName(tmpPathBuf, "bsls", 0, result)) {
        return false;
    }
#elif defined(BSLS_PLATFORM__OS_HPUX)
    const size_t maxSize = L_tmpnam;
    if(! tempnam(result, "bsls")) {
        return false;
    }
#else
    //TODO: replace PATH_MAX with something portable
    const size_t maxSize = PATH_MAX;
    char *fn = tempnam(0, "bsls");
    if (fn) {
        strncpy(result, fn, maxSize);
        free(fn);
    } else {
        return false;
    }
#endif

    if (verboseFlag) printf("\tUse '%s' as a base filename.\n", result);

    ASSERT(0 == atexit(cleanTempFile));
        // Warn but do not fail, since failing will only leave behind the temp
        // file we cannot clean up

    // Test Invariant:
    ASSERT('\0' != result[0]); // result not empty

    return true;
}

bool redirectOutputs(bool verboseFlag)
{
    bool success = false;

    if (! freopen("/dev/stdout", "w+", stderr)) {
            // Redirect stderr
        // Use aSsErT instead of realaSsErT, because stderr has not been
        // redirected
        aSsErT(1, "Failed to redirect stdout", __LINE__);
        // Fix values of realTestStatus and testStatus to account for using
        // aSsErT above
        realTestStatus = testStatus;
        testStatus = 0;
    } else if (! tempFileName(fileName, verboseFlag)) {
            // Get temp file name
        realaSsErT(1, "Failed to get temp file name for stdout capture", __LINE__);
    } else if (! freopen(fileName, "w+", stdout)) {
            // Redirect stdout
        fprintf(stderr, "fileName: %s\n", fileName);
        realaSsErT(1, "Failed to redirect stdout", __LINE__);
    } else if (EOF == fflush(stdout)) {
        realaSsErT(1, "Error flushing stdout", __LINE__);
    } else {
        success = true;
    }

    return success;
}

bool checkCapture(const char *referenceString, FILE *captureFile, int lineNum) {
    static char outputBuffer[OUTPUT_BUFFER_SIZE];

    size_t stringLength = strlen(referenceString);                     
    long outputSize = ftell(captureFile);                  

    if (outputSize > OUTPUT_BUFFER_SIZE) {
        fprintf(stderr, "ftell(captureFile): %ld\n", outputSize);
        realaSsErT(1, "captured output exceeds read buffer size", lineNum);
        return false;
    } else if (stringLength != outputSize) {
        rewind(captureFile);                                   
        long charsRead = fread(outputBuffer, sizeof(char), outputSize, captureFile);
        fprintf(stderr, "outputSize: %ld\toutputBuffer: %s\treferenceString: %s\n", outputSize, outputBuffer, referenceString);
        realaSsErT(1, "captured output size mismatch", lineNum);
        return false;
    }

    rewind(captureFile);                                   

    long charsRead = fread(outputBuffer, sizeof(char), outputSize, captureFile);
    if (outputSize != charsRead) {
        fprintf(stderr, "outputSize: %ld\tcharsRead: %ld\n", outputSize, charsRead);
        realaSsErT(1, "could not read all captured output", lineNum);
        return false;
    } else if (0 != strncmp(referenceString, outputBuffer, stringLength)) {
        fprintf(stderr, "outputBuffer: %s\treferenceString: %s\n", 
                outputBuffer,
                referenceString);
        realaSsErT(1, "captured output does not match reference string", lineNum);
        return false;
    }

    return true;
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

    if (! redirectOutputs(verbose)) {
            // Capture stdout, and send stderr to stdout
        return realTestStatus;
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
        //    driver, remove leading comment characters, and replace 'assert'
        //    with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) fprintf(stderr, "\nTESTING USAGE EXAMPLE"
                            "\n---------------------\n");
        
        executeUsageExample(verbose, veryVerbose, veryVeryVerbose);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRINT OPERATIONS TEST

        {
            CAPTURE_RESET;
            BSLS_BSLTESTUTIL_Q(42);
            CAPTURE_ASSERT("<| 42 |>\n");

            CAPTURE_RESET;
            BSLS_BSLTESTUTIL_Q(0);
            CAPTURE_ASSERT("<| 0 |>\n");

            CAPTURE_RESET;
            BSLS_BSLTESTUTIL_Q(-42);
            CAPTURE_ASSERT("<| -42 |>\n");

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

        {
            // 1 Output is captured
            const char *testString = "This is output";
            size_t testStringLength = strlen(testString);

            CAPTURE_RESET;
            ASSERT(0 == ftell(stdout));
            printf("%s", testString);
            ASSERT(testStringLength == ftell(stdout));
        }

        {
            // 2 Captured output is accurate
            const char *testString = "This is more output";

            CAPTURE_RESET;
            printf("%s", testString);
            CAPTURE_ASSERT(testString);
            ASSERT(0 == realTestStatus);
        }

        {
            // 3 Incorrect output is correctly diagnosed

            const char *testString =         "This is good output";
            const char *wrongLengthString =  "This is too short";
            const char *wrongContentString = "This is not correct";

            CAPTURE_RESET;
            printf("%s", testString);
            CAPTURE_ASSERT(wrongLengthString);
            ASSERT(1 == realTestStatus);
            realTestStatus -= 1;

            CAPTURE_RESET;
            printf("%s", testString);
            CAPTURE_ASSERT(wrongContentString);
            ASSERT(1 == realTestStatus);
            realTestStatus -= 1;
 
            CAPTURE_RESET;
            int stringLength = strlen(testString);
            for (int idx = 0; idx * stringLength < OUTPUT_BUFFER_SIZE; ++idx) {
                printf("%s", testString);
            }
            printf("%s", testString);
            CAPTURE_ASSERT(testString);
            ASSERT(1 == realTestStatus);
            realTestStatus -= 1;
        }

        {
            // 4 Embedded newlines work
            const char *testString = "This has an\nembedded newline";

            CAPTURE_RESET;
            printf("%s", testString);
            CAPTURE_ASSERT(testString);
            ASSERT(0 == realTestStatus);
            
            const char *twoNewlineTestString = "This has two\nembedded newlines\n";

            CAPTURE_RESET;
            printf("%s", twoNewlineTestString);
            CAPTURE_ASSERT(twoNewlineTestString);
            ASSERT(0 == realTestStatus);
        }

        {
            // 5 Empty output works
            const char *testString = "";

            CAPTURE_RESET;
            printf("%s", testString);
            CAPTURE_ASSERT(testString);
            ASSERT(0 == realTestStatus);
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
