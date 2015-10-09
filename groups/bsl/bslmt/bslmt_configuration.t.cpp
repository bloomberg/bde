// bslmt_configuration.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_configuration.h>

#include <bslmt_threadattributes.h>

#include <bslim_testutil.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>

#include <bsl_c_limits.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// This program tests the functionality of the 'bslmt::Configuration' class.
//
// Note that since this component is below 'bslmt::ThreadUtil', we cannot
// actually create any threads and verify stack sizes, so some testing of this
// component is done in 'bslmt_threadutil.t.cpp'.
//
//-----------------------------------------------------------------------------
// [1] Breathing Test
// [2] recommendedDefaultThreadStackSize
// [3] nativeDefaultThreadStackSize
// [3] setDefaultThreadStackSize
// [3] defaultThreadStackSize
// [3] Usage
// [4] results constant across multiple calls

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::Configuration Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Demonstrate Accessing & Modifying the Default Thread Stack Size
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to access both the platform's native and
// BCE configured default stack sizes, and then to set the default stack size
// used by BCE.  Note that the value returned by 'defaultThreadStackSize' may
// be adjusted from that provided by the underlying operating system to reflect
// the actual amount of stack memory available to a created thread.  For
// example, on Itanium platforms (HPUX) the value will be scaled down from the
// operating system supplied value to account for the extra stack space devoted
// to storing registers.  Note that operations creating a thread should perform
// a similar inverse adjustment when configuring the new thread's stack size
// (see 'bslmt_threadutil').
//
// First, we examine the platform's native thread stack size:
//..
    const int nativeDefault =
                          bslmt::Configuration::nativeDefaultThreadStackSize();

    ASSERT(nativeDefault > 0);
//..
// Then, we verify that 'defaultThreadStackSize' is unset.
//..
    ASSERT(bslmt::ThreadAttributes::e_UNSET_STACK_SIZE ==
                               bslmt::Configuration::defaultThreadStackSize());
//..
// Next, we define 'newDefaultStackSize' to some size other than the platform's
// native default stack size:
//..
    const int newDefaultStackSize = nativeDefault * 2;
//..
// Now, we set the default size for BCE to the new size:
//..
    bslmt::Configuration::setDefaultThreadStackSize(newDefaultStackSize);
//..
// Finally, we verify that BCE's default thread stack size has been set to the
// value we specified:
//..
    ASSERT(bslmt::Configuration::defaultThreadStackSize() ==
                                                          newDefaultStackSize);
    ASSERT(bslmt::Configuration::defaultThreadStackSize() != nativeDefault);
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTIPLE CALLS YIELD SAME RESULT
        //
        // Concern:
        //   Some of these calls may cache their results.  Make sure they are
        //   returning the same value when called multiple times.
        //
        // Plan:
        //   Call the routines multiple times and compare values.
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTIPLE CALLS YIELD SAME RESULT\n"
                             "================================\n";

        const int native = Obj::nativeDefaultThreadStackSize();
        ASSERT(native > 0);

        for (int i = 0; i < 10; ++i) {
            ASSERT(native == Obj::nativeDefaultThreadStackSize());
            ASSERT(bslmt::ThreadAttributes::e_UNSET_STACK_SIZE ==
                                                Obj::defaultThreadStackSize());
        }

        const int setSize = native * 2;

        Obj::setDefaultThreadStackSize(setSize);

        for (int i = 0; i < 10; ++i) {
            ASSERT(setSize == Obj::defaultThreadStackSize());
        }

        Obj::setDefaultThreadStackSize(native);

        for (int i = 0; i < 10; ++i) {
            ASSERT(native == Obj::defaultThreadStackSize());
        }
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // SETTING THE THREAD STACK SIZE / USAGE
        //
        // Concern:
        //   That 'setDefaultThreadStackSize' is able to set the default thread
        //   stack size.
        //
        // Plan:
        //   Call 'setDefaultThreadStackSize' with a value that is not equal to
        //   'nativeDefaultThreadStackSize' and then verify that the result of
        //   'defaultThreadStackSize' has been set to the new value.
        // --------------------------------------------------------------------

        if (verbose) cout << "SETTING THE THREAD STACK SIZE\n"
                             "=============================\n";

        // First, we examine the native thread stack size:

        const int nativeDefault =
                          bslmt::Configuration::nativeDefaultThreadStackSize();
        ASSERT(nativeDefault > 0);

        // Then, we verify that 'defaultThreadStackSize' is unset.

        ASSERT(bslmt::ThreadAttributes::e_UNSET_STACK_SIZE ==
                               bslmt::Configuration::defaultThreadStackSize());

        // Next, we define 'newDefaultStackSize' to some size other than the
        // native default size:

        const int newDefaultStackSize = nativeDefault * 2;

        // Now, we set the default size to the new size:

        bslmt::Configuration::setDefaultThreadStackSize(newDefaultStackSize);

        // Finally, we verify that the default thread stack size has been set
        // to the value we specified:

        ASSERT(bslmt::Configuration::defaultThreadStackSize() ==
                                                          newDefaultStackSize);
        ASSERT(bslmt::Configuration::defaultThreadStackSize() !=
                                                                nativeDefault);
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // RECOMMENDED THREAD STACK SIZE
        //
        // Concern:
        //   That 'recommendedThreadStackSize' returns a reasonable value and
        //   that the stack size can be set to that value on all platforms.
        //
        // Plan:
        //   Call 'recommendedDefaultThreadStackSize' and verify that the value
        //   is non-negative.
        // --------------------------------------------------------------------

        if (verbose) cout << "RECOMMENDED THREAD STACK SIZE\n"
                             "=============================\n";

        const int recommended = Obj::recommendedDefaultThreadStackSize();
        ASSERT(recommended > 0);
        ASSERT(recommended < INT_MAX);

        Obj::setDefaultThreadStackSize(recommended);

        ASSERT(Obj::defaultThreadStackSize() == recommended);
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //   That, if 'setDefaultThreadStackSize' hasn't been called, that
        //   'defaultThreadStackSize' equals 'nativeThreadStackSize'.o
        //
        // Plan:
        //   Call 'defaultThreadStackSize' and 'nativeThreadStackSize' without
        //   having called  'setDefaultThreadStackSize', and verify that they
        //   return the same value.
        //
        // Observed Results:
        //   So: Solaris
        //   AI: AIX
        //   HP: HPUX
        //   Li: Linux
        //   Wi: Windows
        //
        //   So 32: defaultStackSize = 1048576, guardSize = 8192
        //   So 64: defaultStackSize = 2097152, guardSize = 8192
        //
        //   AI 32: defaultStackSize = 98304, guardSize = 4096
        //   AI 64: defaultStackSize = 196608, guardSize = 4096
        //
        //   HP 32: defaultStackSize = 131072, guardSize = 4096
        //   HP 64: defaultStackSize = 131072, guardSize = 4096
        //
        //   Li 32: defaultStackSize = 67108864, guardSize = 4096
        //   Li 64: defaultStackSize = 67108864, guardSize = 4096
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        bsl::size_t defaultStackSize = Obj::defaultThreadStackSize();
        ASSERT(bslmt::ThreadAttributes::e_UNSET_STACK_SIZE ==
                                                       (int) defaultStackSize);

        defaultStackSize = Obj::nativeDefaultThreadStackSize();
        ASSERT(defaultStackSize > 0);
        int maxint = bsl::numeric_limits<int>::max();
        ASSERT(defaultStackSize <= static_cast<bsl::size_t>(maxint));

        bsl::size_t guardSize = Obj::nativeDefaultThreadGuardSize();

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT(0 == guardSize);
#else
        ASSERT(guardSize > 0);
        ASSERT(guardSize <= static_cast<bsl::size_t>(maxint));
#endif
        ASSERT(guardSize < defaultStackSize);

        if (verbose) {
            P_(defaultStackSize);    P(guardSize);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return( testStatus );
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
