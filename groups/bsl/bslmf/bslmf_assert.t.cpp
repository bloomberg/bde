// bslmf_assert.t.cpp                                                 -*-C++-*-
#include <bslmf_assert.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // printf
#include <stdlib.h>  // atoi

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 1] BSLMF_ASSERT(expr)i
// ----------------------------------------------------------------------------
// [  ] USAGE EXAMPLE
// [ 2] CONCERN: BSLMF_ASSERT in non-instantiated template classes

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

namespace BloombergLP {
namespace bslmftest {

                           // ======================
                           // class SomeTemplateType
                           // ======================

template <class SOME_TYPE>
class SomeTemplateType {
    // This specialization of 'SomeTemplateType' defines an alias 'Type' for a
    // functor that delegates to a function pointer matching the parameterized
    // 'SOME_TYPE' type.

    BSLMF_ASSERT(4 == sizeof(SOME_TYPE));
        // This 'BSLMF_ASSERT' statement ensures that the parameter 'SOME_TYPE'
        // must be a function pointer.
};

}  // close package namespace
}  // close enterprise namespace


// namespace scope

BSLMF_ASSERT(sizeof(int) >= sizeof(char));

// un-named namespace
namespace {
    BSLMF_ASSERT(1);
#if defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(1);
#else
    BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
#endif
}  // close unnamed namespace

namespace {
    BSLMF_ASSERT(1);
}  // close unnamed namespace

namespace Bar {

    BSLMF_ASSERT(1);
#if defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT(1);
#else
    BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
#endif
}  // close namespace Bar

class MyType {
    // class scope

    BSLMF_ASSERT(sizeof(int) >= sizeof(char));
    BSLMF_ASSERT(sizeof(int) >= sizeof(char));

  public:
    int d_data;
    void foo();
};

void MyType::foo()
{
    // function scope

    BSLMF_ASSERT(sizeof(int) >= sizeof(char));
    BSLMF_ASSERT(1);

    using namespace BloombergLP;
    BSLMF_ASSERT(1);
}

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

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // BSLMF_ASSERT MACRO WITHIN TEMPLATE CLASS
        //   Sun Studio has historically required a special implementation of
        //   'BSMF_ASSERT'.  According to DRQS 79918675, starting with Sun
        //   Studio 12.4 the special implementation causes build failures by
        //   its mere presence in a templatized class, even if that class is
        //   never instantiated.  On the other hand, Sun Studio 12.4 has
        //   improved template instantiation, and therefore does not need the
        //   special implementation.  This test case checks that the case
        //   reported in DRQS 79918675 is fixed by switching to the standard
        //   implementation on versions 12.4 and better.
        //
        // Concerns:
        //  1 Uses of BSLMF_ASSERT in non-instantiated templates does not cause
        //    a build failure when the assertion depends on a template
        //    parameter.
        //
        // Plan:
        //  1 Create a template class having an assertion dependent on the
        //    template parameter, but do not instantiate the class.  If the code
        //    builds on Sun Studio 12.4, the test passes.  (C-1)
        //
        // Testing:
        //   CONCERN: BSLMF_ASSERT in non-instantiated template classes
        // --------------------------------------------------------------------

        if (verbose) printf("\nBSLMF_ASSERT MACRO WITHIN TEMPLATE CLASS"
                            "\n========================================\n");

        // Do nothing.  The test is entirely encapsulated in the definition of
        // 'SomeTemplateType' above.

        ASSERT(true);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING BSLMF_ASSERT MACRO
        // We have the following concerns:
        //   1. Works for all non-zero integral values.
        //   2. Works in and out of class scope.
        //
        // Plan:
        //   Invoke the macro at namespace, class, and function scope (above)
        //   and verify that it does not cause a compiler error.  Please see
        //   the 'ttt' test package group for test cases where the macro
        //   should cause a compile-time error.
        //
        // Testing:
        //   BSLMF_ASSERT(expr)
        // --------------------------------------------------------------------

        if (verbose) printf("\nBSLMF_ASSERT Macro"
                            "\n==================\n");

        BSLMF_ASSERT(sizeof(int) >= sizeof(char));
        BSLMF_ASSERT(sizeof(int) >= sizeof(char));
        BSLMF_ASSERT(1);  ASSERT(219 == __LINE__);
        BSLMF_ASSERT(1);  ASSERT(220 == __LINE__);
        BSLMF_ASSERT(1 > 0 && 1);

// MSVC: __LINE__ macro breaks when /ZI is used (see Q199057 or KB199057)
// SUN:  BSLMF_ASSERT is defined the way that breaks this test
// GCC:  Declares a function, rather than a typedef, from v4.8.1
#if !defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT) && \
    !defined(BSLS_PLATFORM_CMP_MSVC) &&                      \
    !defined(BSLS_PLATFORM_CMP_SUN)  &&                      \
    !(defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR > 40800)
        bslmf_Assert_219 t1; // test typedef name creation; matches above line
        bslmf_Assert_220 t2; // test typedef name creation; matches above line
        ASSERT(sizeof t1 == sizeof t2);  // use t1 and t2
#endif

        BSLMF_ASSERT(2);
        BSLMF_ASSERT(-1);

#if defined(BSLS_PLATFORM_CMP_SUN)
        BSLMF_ASSERT(1);
#else
        BSLMF_ASSERT(1); BSLMF_ASSERT(1); // not class scope
#endif

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
