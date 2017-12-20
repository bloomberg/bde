// ball_defaultobserver.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_defaultobserver.h>

#include <ball_context.h>                      // for testing only
#include <ball_record.h>                       // for testing only
#include <ball_recordattributes.h>             // for testing only
#include <ball_userfields.h>

#include <bdlt_datetime.h>                     // for testing only
#include <bdlt_epochutil.h>                    // for testing only

#include <bslim_testutil.h>

#include <bsls_platform.h>                     // for testing only

#include <bslma_testallocator.h>               // for testing only
#include <bslma_testallocatorexception.h>      // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

// Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(PASSTHROUGH)
#undef PASSTHROUGH
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD - fill
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [  ] etc.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

}  // close unnamed namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::DefaultObserver Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;      // Supress compiler warning.
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        {
            ball::DefaultObserver defaultObserver(&bsl::cout);

            // Do *not* do this.  It significantly increases the level # of
            // this component.

            // ball::LoggerManagerConfiguration lmc;
            // ball::LoggerManager loggerManager(&defaultObserver, lmc);
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;



        if (verbose) cout << "Publish a single message." << endl;
        {
            Obj X(&bsl::cout);

            ball::RecordAttributes fixed;
            ball::UserFields       userValues;

            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            fixed.setTimestamp(now);
            fixed.setProcessID(100);
            fixed.setThreadID(0);
            X.publish(ball::Record(fixed, userValues),
                      ball::Context(ball::Transmission::e_PASSTHROUGH, 0, 1));
        }

        if (verbose) cout << "Publish a sequence of three messages." << endl;
        {
            Obj X(&bsl::cout);

            ball::RecordAttributes fixed;
            ball::UserFields  userValues;

            const int NUM_MESSAGES = 3;
            for (int n = 0; n < NUM_MESSAGES; ++n) {
                bdlt::Datetime now =
                                    bdlt::EpochUtil::convertFromTimeT(time(0));
                fixed.setTimestamp(now);
                fixed.setProcessID(201 + n);
                fixed.setThreadID(31 + n);
                X.publish(ball::Record(fixed, userValues),
                          ball::Context(ball::Transmission::e_TRIGGER,
                                        n,
                                        NUM_MESSAGES));
            }
        }
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
// Copyright 2015 Bloomberg Finance L.P.
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
