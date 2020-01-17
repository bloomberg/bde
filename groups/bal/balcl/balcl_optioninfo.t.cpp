// balcl_optioninfo.t.cpp                                             -*-C++-*-

#include <balcl_optioninfo.h>

#include <balcl_typeinfo.h>
#include <balcl_occurrenceinfo.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_string.h>

#include <bsl_cstdlib.h>  // 'bsl::atoi'

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// balcl::OptionInfo
// ----------------------------------------------------------------------------
// FREE OPERATORS
// [ 1] bool operator==(const OptionInfo& lhs, rhs);
// [ 1] bool operator!=(const OptionInfo& lhs, rhs);
// [ 1] ostream& operator<<(ostream& stream, const OptionInfo& rhs);
// ----------------------------------------------------------------------------
// [xx] BREATHING TEST
// [xx] USAGE EXAMPLE

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
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OccurrenceInfo OccurrenceInfo;
typedef balcl::OptionInfo     OptionInfo;
typedef balcl::TypeInfo       TypeInfo;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

// ============================================================================
//          GLOBAL CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                  USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])  {
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'balcl::OptionInfo'
        //
        // Concerns:
        //: 1 'balcl::OptionInfo' is merely a 'struct' with no
        //:   user-defined member functions, and five members.  We need only
        //:   verify that it can be initialized on the stack.
        //
        // Plan:
        //: 1 Use stack-created instances with a variable number of
        //:   initializers.  Run stack-initialization in a loop to make sure
        //:   all memory gets de-allocated properly.
        //
        // Testing:
        //    bool operator==(const OptionInfo& lhs, rhs);
        //    bool operator!=(const OptionInfo& lhs, rhs);
        //    ostream& operator<<(ostream& stream, const OptionInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'balcl::OptionInfo'" << endl
                          << "===========================" << endl;

        bslma::TestAllocator defaultAllocator(veryVeryVeryVerbose);

        bool        linkedFlag;
        bsl::string defaultValue("default");
        bsl::string tagString("genericTag");

        bslma::DefaultAllocatorGuard guard(&defaultAllocator);

        for (int i = 0; i < 10; ++i) {

            if (veryVerbose) { T_ P(i) }

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
      //v---^
        balcl::OptionInfo specTable[] = {
          {
          },
          {
            tagString                                        // tag
          },
          {
            "s|longTag"                                      // tag
          },
          {
            "s|longTag",                                     // tag
            "option name"                                    // name
          },
          {
            "s|longTag",                                     // tag
            "option name",                                   // name
            "option description"                             // description
          },
          {
            "s|longTag",                                     // tag
            "option name",                                   // name
            "option description",                            // description
            balcl::TypeInfo(&linkedFlag)           // linked variable
          },
          {
            "s|longTag",                                     // tag
            "option name",                                   // name
            "option description",                            // description
            balcl::TypeInfo(&linkedFlag),          // linked variable
            balcl::OccurrenceInfo::e_REQUIRED // occurrence info
          },
          {
            "s|longTag",                                     // tag
            "option name",                                   // name
            "option description",                            // description
            balcl::TypeInfo(&linkedFlag),          // linked variable
            balcl::OccurrenceInfo(defaultValue)    // occurrence info
          }
        };
      //^---v
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

            (void)specTable;

        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numMismatches());
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
// Copyright 2020 Bloomberg Finance L.P.
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
