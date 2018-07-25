// bsls_assert_macroreset.t.cpp                                       -*-C++-*-

#include <bsls_assert_macroreset.h>

#include <bsls_bsltestutil.h>
#include <cstdio>    // 'fprintf'
#include <cstdlib>   // 'atoi'

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component undefines macros from 'bsls_assert.h' and 'bsls_review.h'.
// We will validate that those macros are not defined, then define them, then
// include the header and validate again that they are not defined.
//-----------------------------------------------------------------------------
// [1] bsls_assert_macroreset.h
//-----------------------------------------------------------------------------

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
//                    GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

bool globalVerbose         = false;
bool globalVeryVerbose     = false;
bool globalVeryVeryVerbose = false;

// Forward declaration of function that appears after 'main' to minimize macro
// and header re-inclusion impact on code.
void testMacroHandling();
    // Assert test failures if any component macros are defined or if any do
    // not get undefined by including 'bsls_assert_macroreset.h'.

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

            globalVerbose =         verbose;
        globalVeryVerbose =     veryVerbose;
    globalVeryVeryVerbose = veryVeryVerbose;

    printf( "TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:  // zero is always the leading case
      case 1: {
        // --------------------------------------------------------------------
        // MACRO TEST
        //
        // Concerns:
        //: 1 The macros that 'bsls_assert_macroreset.h' purports to undefine
        //:   are indeed undefined immediately following its inclusion.
        //
        //: 2 These macros should include all macros that are defined and leak
        //:   out of 'bsls_review.h' and 'bsls_assert.h'.
        //
        // Plan:
        //: 1 Use a script to generate the list of macros that either leak out
        //:   of 'bsls_review.h' and 'bsls_assert.h' or that are undefined by
        //:   'bsls_review_macroreset.h' or 'bsls_assert_macroreset.h'.
        //:
        //: 2 Call a function defined at the end of this file that contains the
        //:   generated code to do the remaining steps of this plan.
        //:
        //: 3 Check that all component macros are not defined.
        //:
        //: 4 Define all component macros with a fixed value.
        //:
        //: 5 *Re*-include 'bsls_assert_macroreset.h'.
        //:
        //: 6 Check that all component macros are not defined again.
        //
        // Testing:
        //   bsls_assert_macroreset.h
        // --------------------------------------------------------------------

        if (verbose) printf( "\nMACRO TEST"
                             "\n==========\n" );

        testMacroHandling();

      } break;
      default: {
          fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}


// The following 'bash' script generates all of the checks for the macros that
// are defined in 'bsls_assert.h' and 'bsls_review.h' or undefined in
// 'bsls_assert_macroreset.h' and 'bsls_review_macroreset.h'.
//..
//  #!/bin/bash
//
//  HEADERS=( bsls_assert.h bsls_review.h )
//  RESETHEADERS=( bsls_assert_macroreset.h bsls_review_macroreset.h )
//
//  UNDEFINED=( $(cat ${HEADERS[@]} | grep "#\\s*undef" |
//                    sed -e "s/.*undef //") )
//
//  DEFINED=( $(cat ${HEADERS[@]} ${RESETHEADERS} |
//                  egrep "#\\s*(define|undef)" |
//                  sed -e "s/.*define //" -e "s/.*undef //" -e "s/[( ].*//" |
//                  grep -v "TESTDRIVER_GUARD" |
//                  egrep -v "(^BDE_ASSERT)|(^BSL_ASSERT)|(MACRORESET)" |
//                  sort -u)
//              )
//
//  MACROS=( $(for M in ${DEFINED[@]} ${UNDEFINED[@]} ; do echo "${M}" ; done |
//                 sort | uniq -u ) )
//
//  echo "    // Generated checks."
//
//  for M in "${MACROS[@]}" ; do
//      cat <<EOF
//      #ifdef ${M}
//          ASSERT(!"${M} is already defined!");
//      #endif
//  EOF
//  done
//
//  echo
//  for M in "${MACROS[@]}" ; do
//      echo "    #define ${M} 17"
//  done
//
//  cat <<EOF
//
//      #undef INCLUDED_BSLS_ASSERT_MACRORESET
//      #include <bsls_assert_macroreset.h>
//
//  EOF
//
//  for M in "${MACROS[@]}" ; do
//      cat <<EOF
//      #ifdef ${M}
//          ASSERT(!"${M} is still defined!");
//      #endif
//  EOF
//  done
//
//  echo "    // End of generated checks."
//..

void testMacroHandling()
{
    // All asserts will be compiled out if tests pass - call this here to avoid
    // unused static function warnings.
    ASSERT(true);

    // smoke test
    #ifndef INCLUDED_BSLS_ASSERT_MACRORESET
        ASSERT(!"INCLUDED_BSLS_ASSERT_MACRORESET is NOT defined!");
    #endif

    // Generated checks.
    #ifdef BDE_BUILD_TARGET_DBG
        ASSERT(!"BDE_BUILD_TARGET_DBG is already defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_OPT
        ASSERT(!"BDE_BUILD_TARGET_OPT is already defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_SAFE
        ASSERT(!"BDE_BUILD_TARGET_SAFE is already defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_SAFE_2
        ASSERT(!"BDE_BUILD_TARGET_SAFE_2 is already defined!");
    #endif
    #ifdef BSLS_ASSERT
        ASSERT(!"BSLS_ASSERT is already defined!");
    #endif
    #ifdef BSLS_ASSERT_ASSERT
        ASSERT(!"BSLS_ASSERT_ASSERT is already defined!");
    #endif
    #ifdef BSLS_ASSERT_DISABLED_IMP
        ASSERT(!"BSLS_ASSERT_DISABLED_IMP is already defined!");
    #endif
    #ifdef BSLS_ASSERT_INVOKE
        ASSERT(!"BSLS_ASSERT_INVOKE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_IS_ACTIVE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT is already defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT_OPT
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT_OPT is already defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT_SAFE
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT_SAFE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_NONE
        ASSERT(!"BSLS_ASSERT_LEVEL_NONE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_OPT
        ASSERT(!"BSLS_ASSERT_OPT is already defined!");
    #endif
    #ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_OPT_IS_ACTIVE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_SAFE
        ASSERT(!"BSLS_ASSERT_SAFE is already defined!");
    #endif
    #ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_SAFE_IS_ACTIVE is already defined!");
    #endif
    #ifdef BSLS_REVIEW
        ASSERT(!"BSLS_REVIEW is already defined!");
    #endif
    #ifdef BSLS_REVIEW_DISABLED_IMP
        ASSERT(!"BSLS_REVIEW_DISABLED_IMP is already defined!");
    #endif
    #ifdef BSLS_REVIEW_INVOKE
        ASSERT(!"BSLS_REVIEW_INVOKE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_IS_ACTIVE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_NONE
        ASSERT(!"BSLS_REVIEW_LEVEL_NONE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW is already defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW_OPT
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW_OPT is already defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW_SAFE
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW_SAFE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_OPT
        ASSERT(!"BSLS_REVIEW_OPT is already defined!");
    #endif
    #ifdef BSLS_REVIEW_OPT_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_OPT_IS_ACTIVE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_REVIEW_IMP
        ASSERT(!"BSLS_REVIEW_REVIEW_IMP is already defined!");
    #endif
    #ifdef BSLS_REVIEW_SAFE
        ASSERT(!"BSLS_REVIEW_SAFE is already defined!");
    #endif
    #ifdef BSLS_REVIEW_SAFE_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_SAFE_IS_ACTIVE is already defined!");
    #endif
    #ifdef INCLUDED_BSLS_ASSERT
        ASSERT(!"INCLUDED_BSLS_ASSERT is already defined!");
    #endif
    #ifdef INCLUDED_BSLS_REVIEW
        ASSERT(!"INCLUDED_BSLS_REVIEW is already defined!");
    #endif

    #define BDE_BUILD_TARGET_DBG 17
    #define BDE_BUILD_TARGET_OPT 17
    #define BDE_BUILD_TARGET_SAFE 17
    #define BDE_BUILD_TARGET_SAFE_2 17
    #define BSLS_ASSERT 17
    #define BSLS_ASSERT_ASSERT 17
    #define BSLS_ASSERT_DISABLED_IMP 17
    #define BSLS_ASSERT_INVOKE 17
    #define BSLS_ASSERT_IS_ACTIVE 17
    #define BSLS_ASSERT_LEVEL_ASSERT 17
    #define BSLS_ASSERT_LEVEL_ASSERT_OPT 17
    #define BSLS_ASSERT_LEVEL_ASSERT_SAFE 17
    #define BSLS_ASSERT_LEVEL_NONE 17
    #define BSLS_ASSERT_OPT 17
    #define BSLS_ASSERT_OPT_IS_ACTIVE 17
    #define BSLS_ASSERT_SAFE 17
    #define BSLS_ASSERT_SAFE_IS_ACTIVE 17
    #define BSLS_REVIEW 17
    #define BSLS_REVIEW_DISABLED_IMP 17
    #define BSLS_REVIEW_INVOKE 17
    #define BSLS_REVIEW_IS_ACTIVE 17
    #define BSLS_REVIEW_LEVEL_NONE 17
    #define BSLS_REVIEW_LEVEL_REVIEW 17
    #define BSLS_REVIEW_LEVEL_REVIEW_OPT 17
    #define BSLS_REVIEW_LEVEL_REVIEW_SAFE 17
    #define BSLS_REVIEW_OPT 17
    #define BSLS_REVIEW_OPT_IS_ACTIVE 17
    #define BSLS_REVIEW_REVIEW_IMP 17
    #define BSLS_REVIEW_SAFE 17
    #define BSLS_REVIEW_SAFE_IS_ACTIVE 17
    #define INCLUDED_BSLS_ASSERT 17
    #define INCLUDED_BSLS_REVIEW 17

    #undef INCLUDED_BSLS_ASSERT_MACRORESET
    #include <bsls_assert_macroreset.h>

    #ifdef BDE_BUILD_TARGET_DBG
        ASSERT(!"BDE_BUILD_TARGET_DBG is still defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_OPT
        ASSERT(!"BDE_BUILD_TARGET_OPT is still defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_SAFE
        ASSERT(!"BDE_BUILD_TARGET_SAFE is still defined!");
    #endif
    #ifdef BDE_BUILD_TARGET_SAFE_2
        ASSERT(!"BDE_BUILD_TARGET_SAFE_2 is still defined!");
    #endif
    #ifdef BSLS_ASSERT
        ASSERT(!"BSLS_ASSERT is still defined!");
    #endif
    #ifdef BSLS_ASSERT_ASSERT
        ASSERT(!"BSLS_ASSERT_ASSERT is still defined!");
    #endif
    #ifdef BSLS_ASSERT_DISABLED_IMP
        ASSERT(!"BSLS_ASSERT_DISABLED_IMP is still defined!");
    #endif
    #ifdef BSLS_ASSERT_INVOKE
        ASSERT(!"BSLS_ASSERT_INVOKE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_IS_ACTIVE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT is still defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT_OPT
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT_OPT is still defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_ASSERT_SAFE
        ASSERT(!"BSLS_ASSERT_LEVEL_ASSERT_SAFE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_LEVEL_NONE
        ASSERT(!"BSLS_ASSERT_LEVEL_NONE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_OPT
        ASSERT(!"BSLS_ASSERT_OPT is still defined!");
    #endif
    #ifdef BSLS_ASSERT_OPT_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_OPT_IS_ACTIVE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_SAFE
        ASSERT(!"BSLS_ASSERT_SAFE is still defined!");
    #endif
    #ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
        ASSERT(!"BSLS_ASSERT_SAFE_IS_ACTIVE is still defined!");
    #endif
    #ifdef BSLS_REVIEW
        ASSERT(!"BSLS_REVIEW is still defined!");
    #endif
    #ifdef BSLS_REVIEW_DISABLED_IMP
        ASSERT(!"BSLS_REVIEW_DISABLED_IMP is still defined!");
    #endif
    #ifdef BSLS_REVIEW_INVOKE
        ASSERT(!"BSLS_REVIEW_INVOKE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_IS_ACTIVE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_NONE
        ASSERT(!"BSLS_REVIEW_LEVEL_NONE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW is still defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW_OPT
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW_OPT is still defined!");
    #endif
    #ifdef BSLS_REVIEW_LEVEL_REVIEW_SAFE
        ASSERT(!"BSLS_REVIEW_LEVEL_REVIEW_SAFE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_OPT
        ASSERT(!"BSLS_REVIEW_OPT is still defined!");
    #endif
    #ifdef BSLS_REVIEW_OPT_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_OPT_IS_ACTIVE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_REVIEW_IMP
        ASSERT(!"BSLS_REVIEW_REVIEW_IMP is still defined!");
    #endif
    #ifdef BSLS_REVIEW_SAFE
        ASSERT(!"BSLS_REVIEW_SAFE is still defined!");
    #endif
    #ifdef BSLS_REVIEW_SAFE_IS_ACTIVE
        ASSERT(!"BSLS_REVIEW_SAFE_IS_ACTIVE is still defined!");
    #endif
    #ifdef INCLUDED_BSLS_ASSERT
        ASSERT(!"INCLUDED_BSLS_ASSERT is still defined!");
    #endif
    #ifdef INCLUDED_BSLS_REVIEW
        ASSERT(!"INCLUDED_BSLS_REVIEW is still defined!");
    #endif
    // End of generated checks.
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
