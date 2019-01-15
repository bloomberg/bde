// bsls_outputredirector.t.cpp                                        -*-C++-*-
#include <bsls_outputredirector.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides a global facility for low-level code to
// write log messages.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o By default, the 'platformDefaultMessageHandler' is used.
//: o Exceptions thrown in a log message handler are propagated.
//: o Precondition violations are detected in appropriate build modes.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] OutputRedirector(Stream which, bool verbose, bool veryVerbose);
// [  ] ~OutputRedirector();
// MANIPULATORS
// [  ] void clear();
// [  ] void disable();
// [  ] void enable();
// [  ] bool load();
// ACCESSORS
// [  ] int compare(const char *expected, size_t expectedLength) const;
// [  ] int compare(const char *expected) const;
// [  ] const char *getOutput() const;
// [  ] bool isOutputReady() const;
// [  ] bool isRedirecting() const;
// [  ] FILE *nonRedirectedStream() const;
// [  ] const struct stat& originalStat() const;
// [  ] size_t outputSize() const;
// [  ] FILE *redirectedStream() const;
// [  ] OutputRedirector::Stream redirectedStreamId() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ 2] ~OutputRedirector();  // BOOTSTRAP


// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace


// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;
    (void)    veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    printf("TEST %s CASE %d\n", __FILE__, test);

    switch(test) { case 0: // zero is always the leading case
    case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR
        //   'OutputRedirector' has just one constructor, which does not start
        //   the redirection/capturing behavior, so test thoroughly in just
        //   this test case.  The destructor can test only bootstrap behavior,
        //   as the interesting cases will involve starting (and stopping)
        //   redirection.  Similarly, it is not possible to verify passing the
        //   verbose flags correctly without potentially writing text in such
        //   modes, so defer (bootstrap) testing of verbose flags to manually
        //   running the test driver in those modes.
        //
        // Concerns:
        //: 1 'OutputRedirector' objects construct and destroy properly.
        //: 2 A freshly constructed object is not actively redirecting output.
        //: 3 After construction, the object is set to redirect the stream
        //:   specified at construction.
        //: 4 Attempts to specify an invlalid stream are detected though
        //:   assertions in appropriate build modes.
        //
        // Plan:
        //: 1 In distinct scopes, constructor an 'OutputRedirector' for each of
        //:   the valid streams, and allow that object destructor to run safely
        //:   at the end of that local scope.
        //: 2 Using the basic accessors, confirm that each newly constructed
        //:   object is in an empty disabled state.
        //: 3 In appropriate build modes, repeat the tests for the constructor
        //:   with invalid stream values, and confirm the expected assertions
        //:   are triggered.
        //
        // Testing:
        //   OutputRedirector(Stream which, bool verbose, bool veryVerbose);
        //   ~OutputRedirector();  // BOOTSTRAP
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTOR"
                            "\n===================\n");

        if (verbose) printf("Redirector for 'stdout'\n");
        {
            bsls::OutputRedirector redirector(
                                      bsls::OutputRedirector::e_STDOUT_STREAM);

            ASSERT(bsls::OutputRedirector::e_STDOUT_STREAM ==
                                              redirector.redirectedStreamId());
            ASSERT(stdout == redirector.redirectedStream());
            ASSERT(stderr == redirector.nonRedirectedStream());

            ASSERT(!redirector.isRedirecting());
            ASSERT(!redirector.isOutputReady());
            ASSERTV(redirector.outputSize(), 0 == redirector.outputSize());

            // Confirm output buffer is established and internal to object.
            const char *const BUFFER          = redirector.getOutput();
            const char *const OBJ_START       = (char *)&redirector;
            const long int    REDIRECTOR_SIZE =
                         static_cast<long int>(sizeof(bsls::OutputRedirector));

            ASSERTV(OBJ_START, BUFFER, OBJ_START < BUFFER);
            ASSERTV(BUFFER - OBJ_START, REDIRECTOR_SIZE,
                    BUFFER - OBJ_START <  REDIRECTOR_SIZE);
        }

        if (verbose) printf("Redirector for 'stderr'\n");
        {
            bsls::OutputRedirector redirector(
                                      bsls::OutputRedirector::e_STDERR_STREAM);

            ASSERT(bsls::OutputRedirector::e_STDERR_STREAM ==
                                              redirector.redirectedStreamId());
            ASSERT(stderr == redirector.redirectedStream());
            ASSERT(stdout == redirector.nonRedirectedStream());

            ASSERT(!redirector.isRedirecting());
            ASSERT(!redirector.isOutputReady());
            ASSERTV(redirector.outputSize(), 0 == redirector.outputSize());

            // Confirm output buffer is established and internal to object.
            const char *const BUFFER          = redirector.getOutput();
            const char *const OBJ_START       = (char *)&redirector;
            const long int    REDIRECTOR_SIZE =
                         static_cast<long int>(sizeof(bsls::OutputRedirector));

            ASSERTV(OBJ_START, BUFFER, OBJ_START < BUFFER);
            ASSERTV(BUFFER - OBJ_START, REDIRECTOR_SIZE,
                    BUFFER - OBJ_START <  REDIRECTOR_SIZE);
        }

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
        //: 1 Perform an ad-hoc test of the primary manipulators and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bsls::OutputRedirector redirector(
                                       bsls::OutputRedirector::e_STDOUT_STREAM,
                                       verbose,
                                       veryVerbose);

        if (verbose) printf("Verify by manual inspection: no redirect yet.\n");

        redirector.enable();

        printf("Hello world!");  // note, no '\n'

        // Ensure the redirector is 'disable'd after the call to 'load' so that
        // 'ASSERT' macros display any errors correctly.

        if (!redirector.load()) {
            redirector.disable();
            ASSERT(!"Could not load redirected output into buffer.");
        }
        else {
            redirector.disable();

            if (!redirector.isOutputReady()) {
                ASSERT(!"Redirected output buffer is empty.");
                break;
            }

            if (redirector.outputSize() < 12 ) {
                ASSERT(!"Redirected output buffer is shorted than expected.");
                break;
            }

            const char *printedText = redirector.getOutput();
            ASSERT(0 == strncmp("Hello world!", printedText, 12));
        }

        if (verbose) printf(
                      "Text for manual inspection after redirect disabled.\n");
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
// limitations under the License
