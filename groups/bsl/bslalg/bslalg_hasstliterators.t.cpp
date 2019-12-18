// bslalg_hasstliterators.t.cpp                                       -*-C++-*-

#include <bslalg_hasstliterators.h>

#include <bslmf_integralconstant.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The type under testing is a primitive class, which is used as a tag type and
// therefore is empty.  There is nothing to test except that the name of the
// class is as expected, and the breathing test.
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

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
//                  COMPILER DEFECT MACROS TO GUIDE TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
# define BSLALG_HASSTLITERATORS_ABOMINABLE_FUNCTION_MATCH_CONST 1
// The Solaris CC compiler matches 'const' qualified abominable functions as
// 'const'-qualified template parameters, but does not strip the 'const'
// qualifier when passing that template parameter onto the next instantiation.
// Therefore, 'HasStlIteratorTraits<void() const>' requests infinite template
// recursion.  We opt to not try a workaround in the header for this platform,
// where we delegate to the same implementation as the primary template, as
// that would leave an awkward difference in behavior between for 'const'
// qualified class types between using a nested trait and directly specializing
// the trait.
#endif

#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 0x1900)   \
 || defined(BSLMF_ISTRIVIALLYCOPYABLE_ABOMINABLE_FUNCTION_MATCH_CONST)
# define BSLALG_HASSTLITERATORS_NO_ABOMINABLE_FUNCTIONS  1
// Older MSVC compilers do not parse abominable function types, so it does not
// matter whether trait would support them or not, we can simply disable such
// tests on this platform.
#endif

//=============================================================================
//                  MACROS TO CONFIGURE TESTING
//-----------------------------------------------------------------------------

//#define BSLALG_HASSTLITERATORS_TEST_INCOMPLETE_TYPES 1
// Define this macro to enable test coverage of incomplete class types, which
// should produce a compile-error.  Testing with this macro must be enabled
// outside the regular nightly regression cycle, as by its existence, it would
// cause the test driver to fail.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
struct HasStlIteratorsTrait : bslalg::HasStlIterators<TYPE>::type {
    // This class template has the 'HasStlIterators' trait, which is registered
    // using the deprecated 'bslalg' scheme for associating nested traits.
};

class Incomplete;
    // This class supports testing of incomplete types.

struct RequiresStlIteratorsType {
    // This class template has the 'HasStlIterators' trait, which is registered
    // using the preferred 'bslmf' scheme for associating nested traits.  This
    // class tests interoperability of the old scheme with the new.

    BSLMF_NESTED_TRAIT_DECLARATION(RequiresStlIteratorsType,
                                   bslalg::HasStlIterators);
};

struct DoesNotRequireStlIteratorsType {
    // This class explicitly does NOT have the 'HasStlIterators' trait.
};

struct SpecializedForStlIteratorsType {
    // This class template has the 'HasStlIterators' trait, which is registered
    // by explicit template of the trait for this type.
    // class tests interoperability of the old scheme with the new.
};

namespace BloombergLP {
namespace bslalg {

template <>
struct HasStlIterators<SpecializedForStlIteratorsType> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

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
        // BREATHING TEST
        //
        // Concerns:
        //: 1 'HasStlIterators<TYPE> compiles and gives the expected result for
        //:   every category of type.
        //
        // Plan:
        //: 1 Implement a new class 'HasStlIteratorTrait' derived from
        //:   'HasStlIterators<TYPE>::type' and attempt to detect this trait
        //:   on a class that declares "nested traits" using
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //
        //: 2 Make sure that this trait is not detected on a class that does
        //:   not declare "nested traits".
        //
        // Testing: struct bslalg::HasStlIterators;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT( HasStlIteratorsTrait<RequiresStlIteratorsType>::value);
        ASSERT( HasStlIteratorsTrait<const RequiresStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                                    volatile RequiresStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait
                             <const volatile RequiresStlIteratorsType>::value);

        ASSERT(!HasStlIteratorsTrait<RequiresStlIteratorsType&>::value);
        ASSERT(!HasStlIteratorsTrait<const RequiresStlIteratorsType&>::value);

        ASSERT(!HasStlIteratorsTrait<RequiresStlIteratorsType *>::value);
        ASSERT(!HasStlIteratorsTrait<const RequiresStlIteratorsType *>::value);

        ASSERT(!HasStlIteratorsTrait<RequiresStlIteratorsType[]>::value);
        ASSERT(!HasStlIteratorsTrait<RequiresStlIteratorsType()>::value);
#if !defined(BSLALG_HASSTLITERATORS_NO_ABOMINABLE_FUNCTIONS)
        ASSERT(!HasStlIteratorsTrait<RequiresStlIteratorsType() const>::value);
#endif

        ASSERT( HasStlIteratorsTrait<SpecializedForStlIteratorsType>::value);
        ASSERT( HasStlIteratorsTrait<
                                 const SpecializedForStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                              volatile SpecializedForStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                        const volatile SpecializedForStlIteratorsType>::value);

        ASSERT(!HasStlIteratorsTrait<SpecializedForStlIteratorsType&>::value);
        ASSERT(!HasStlIteratorsTrait<
                                const SpecializedForStlIteratorsType&>::value);

        ASSERT(!HasStlIteratorsTrait<SpecializedForStlIteratorsType *>::value);
        ASSERT(!HasStlIteratorsTrait<
                               const SpecializedForStlIteratorsType *>::value);


        ASSERT(!HasStlIteratorsTrait<DoesNotRequireStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                                 const DoesNotRequireStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                              volatile DoesNotRequireStlIteratorsType>::value);
        ASSERT(!HasStlIteratorsTrait<
                        const volatile DoesNotRequireStlIteratorsType>::value);

#if defined(BSLALG_HASSTLITERATORS_TEST_INCOMPLETE_TYPES)
        ASSERT(!HasStlIteratorsTrait<Incomplete>::value);
        ASSERT(!HasStlIteratorsTrait<const Incomplete>::value);
        ASSERT(!HasStlIteratorsTrait<volatile Incomplete>::value);
        ASSERT(!HasStlIteratorsTrait<const volatile Incomplete>::value);
#endif

        ASSERT(!HasStlIteratorsTrait<Incomplete *>::value);
        ASSERT(!HasStlIteratorsTrait<const Incomplete *>::value);
        ASSERT(!HasStlIteratorsTrait<volatile Incomplete *>::value);
        ASSERT(!HasStlIteratorsTrait<const volatile Incomplete *>::value);

        ASSERT(!HasStlIteratorsTrait<Incomplete &>::value);
        ASSERT(!HasStlIteratorsTrait<const Incomplete &>::value);
        ASSERT(!HasStlIteratorsTrait<volatile Incomplete &>::value);
        ASSERT(!HasStlIteratorsTrait<const volatile Incomplete &>::value);
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
// Copyright 2019 Bloomberg Finance L.P.
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
