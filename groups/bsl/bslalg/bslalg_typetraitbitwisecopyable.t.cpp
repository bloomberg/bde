// bslalg_typetraitbitwisecopyable.t.cpp                              -*-C++-*-

#include <bslalg_typetraitbitwisecopyable.h>

#include <bslalg_hastrait.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// Verify that the trait under test can be detected using 'bslalg::HasTrait'
// whether the trait is ascribed using 'BSLMF_NESTED_TRAIT_DECLARATION' or
// using the (preferred) C++11 idiom for defining traits.
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::TypeTraitBitwiseCopyable  Obj;

struct AlmostTrivial {
    AlmostTrivial() {}
    AlmostTrivial(const AlmostTrivial&) {}
};

class NotTrivial {
  private:
    void *d_this_p;

  public:
    NotTrivial() : d_this_p(this) {}
    NotTrivial(const NotTrivial&) : d_this_p(this) {}
};

class AnotherNotTrivial {
  private:
    void *d_this_p;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(AnotherNotTrivial,
                                   bsl::is_trivially_copyable);

    AnotherNotTrivial() : d_this_p(this) {}
    AnotherNotTrivial(const AnotherNotTrivial&) : d_this_p(this) {}
};

namespace bsl {
template <>
struct is_trivially_copyable<AlmostTrivial> : true_type {};
}  // close namespace bsl

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TRAIT CLASS
        //
        // Concerns:
        //: 1 The name of the trait class does not change over time.
        //
        // Plan:
        //: 1 Create an instance of the trait class.
        //
        // Testing:
        //   class bslalg::TypeTraitBitwiseCopyable;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAIT CLASS"
                            "\n===================\n");

        Obj mX;

        (void) mX;

        ASSERT(( bslalg::HasTrait<AlmostTrivial,     Obj>::VALUE));
        ASSERT((!bslalg::HasTrait<NotTrivial,        Obj>::VALUE));
        ASSERT(( bslalg::HasTrait<AnotherNotTrivial, Obj>::VALUE));

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
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
