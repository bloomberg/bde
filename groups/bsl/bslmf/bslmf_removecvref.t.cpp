// bslmf_removecvref.t.cpp                                            -*-C++-*-
#include <bslmf_removecvref.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
#include <type_traits> // 'std::remove_cvref', 'std::remove_cvref_t'
#endif

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions, 'bsl::remove_cvref' and
// 'bsl::remove_cvref_t', that strip reference-ness and remove any top-level
// cv-qualifiers from a template parameter type.  Thus, we need to ensure that
// the values returned by the meta-function are correct for each possible
// category of types.  When 'std::remove_cvref' is available in the native
// standard library, 'bsl::remove_cvref' and 'bsl::remove_cvref_t' must be
// aliases to the native meta functions.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_cvref::type
// [ 1] bsl::remove_cvref_t
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] CONCERN: Aliased to standard types when available.

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
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consequently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#elif defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::remove_cvref'.
};

typedef int TestType::* Pm;
typedef int (TestType::*Pmf)();
typedef int (TestType::*Pmq)() const;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#define ASSERT_SAME(X, Y)                                                     \
    ASSERTV(bsls::NameOf<bsl::remove_cvref<  X>::type>(),                     \
            bsls::NameOf<Y>(),                                                \
            (bsl::is_same<bsl::remove_cvref<  X>::type, Y>::value));          \
    ASSERTV(bsls::NameOf<bsl::remove_cvref_t<  X> >(),                        \
            bsls::NameOf<Y>(),                                                \
            (bsl::is_same<bsl::remove_cvref_t<X>,       Y>::value));
#else
#define ASSERT_SAME(X, Y)                                                     \
    ASSERTV(bsls::NameOf<bsl::remove_cvref<  X>::type>(),                     \
            bsls::NameOf<Y>(),                                                \
            (bsl::is_same<bsl::remove_cvref<  X>::type, Y>::value));
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define ASSERT_REMOVE_CVREF(X)                                                \
    ASSERT_SAME(X,                  X)                                        \
    ASSERT_SAME(X&,                 X)                                        \
    ASSERT_SAME(const X,            X)                                        \
    ASSERT_SAME(volatile X,         X)                                        \
    ASSERT_SAME(const X&,           X)                                        \
    ASSERT_SAME(volatile X&,        X)                                        \
    ASSERT_SAME(const volatile X,   X)                                        \
    ASSERT_SAME(const volatile X&,  X)                                        \
    ASSERT_SAME(X&&,                X)                                        \
    ASSERT_SAME(const X&&,          X)                                        \
    ASSERT_SAME(volatile X&&,       X)                                        \
    ASSERT_SAME(const volatile X&&, X)
#else
#define ASSERT_REMOVE_CVREF(X)                                                \
    ASSERT_SAME(X,                 X)                                         \
    ASSERT_SAME(X&,                X)                                         \
    ASSERT_SAME(const X,           X)                                         \
    ASSERT_SAME(volatile X,        X)                                         \
    ASSERT_SAME(const X&,          X)                                         \
    ASSERT_SAME(volatile X&,       X)                                         \
    ASSERT_SAME(const volatile X,  X)                                         \
    ASSERT_SAME(const volatile X&, X)
#endif

}  // close unnamed namespace

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers and Reference-ness of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified reference type ('MyCvRefType') and the same type
// without the cv-qualifiers and reference-ness ('MyType'):
//..
        typedef const volatile int& MyCvRefType;
        typedef                int  MyType;
//..
// Now, we remove the cv-qualifiers from 'MyCvRefType' and its reference-ness
// using 'bsl::remove_cvref' and verify that the resulting type is the same as
// 'MyType':
//..
        ASSERT(true == (bsl::is_same<bsl::remove_cvref<MyCvRefType>::type,
                                     MyType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a 'const'-qualifier, 'volatile'-qualifier and its reference-ness from
// 'MyCvRefType' using 'bsl::remove_cvref_t' and verify that the resulting type
// is the same as 'MyType':
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
        ASSERT(
            true ==
            (bsl::is_same<bsl::remove_cvref_t<MyCvRefType>, MyType>::value));
#endif
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALIASED TO STANDARD META FUNCTIONS
        //
        // Concerns:
        //: 1 The meta functions 'bsl::remove_cvref' and 'bsl::remove_cvref_v'
        //:   should be aliased to their standard library analogs when the
        //:   latter is available from the native library.
        //
        // Plan:
        //: 1 When 'BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY' is
        //:   defined, use 'bsl::is_same' to compare the two meta functions
        //:   using a representative type.
        //
        // Testing:
        //   CONCERN: Aliased to standard types when available.
        // --------------------------------------------------------------------

        if (verbose) printf("\nALIASED TO STANDARD META FUNCTIONS"
                            "\n==================================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
        typedef int T;

        if (veryVerbose) printf(
                              "\nTesting 'remove_cvref' alias using 'int'.\n");

        ASSERT((bsl::is_same<bsl::remove_cvref<T>,
                             std::remove_cvref<T> >::value));

        if (veryVerbose) printf(
                            "\nTesting 'remove_cvref_t' alias using 'int'.\n");

        ASSERT((bsl::is_same<bsl::remove_cvref_t<T>,
                             std::remove_cvref_t<T> >::value));
#else
        if (veryVerbose) printf(
                            "\nSkipped: neither standard type is available\n");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_cvref::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_cvref'
        //   instantiations has the same type as the template parameter type
        //   except that it has any top-level cv-qualifiers and its
        //   reference-ness removed.
        //
        // Concerns:
        //: 1 'bsl::remove_cvref' leaves types that are not 'const'-qualified
        //:    nor 'volatile'-qualified as-is.
        //:
        //: 2 'bsl::remove_cvref' leaves types that are not references as-is.
        //:
        //: 3 'bsl::remove_cvref' removes any top-level cv-qualifiers.
        //:
        //: 4 'bsl::remove_cvref' removes any reference-ness.
        // :
        //: 5 'bsl::remove_cvref' removes top-level cv-qualifiers from a
        //:   pointer-to-member object type, and not from the qualifier in the
        //:   pointed-to member.
        //:
        //: 5 'bsl::remove_cvref_t' represents the return type of
        //:   'bsl::remove_cvref' meta-function for a variety of template
        //:   parameter types.
        //
        // Plan:
        //  1 Verify that 'bsl::remove_cvref::type' has the correct type for
        //    each concern. (C 1-5)
        //
        //  2 Verify that 'bsl::remove_cvref_t' has the same type as the return
        //    type of 'bsl::remove_cvref' for a variety of template parameter
        //    types. (C-6)
        //
        // Testing:
        //   bsl::remove_cvref::type
        //   bsl::remove_cvref_t
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_cvref::type'\n"
                            "\n=========================\n");

        // Basic types

        ASSERT_REMOVE_CVREF(int   );

        ASSERT_REMOVE_CVREF(Enum  );
        ASSERT_REMOVE_CVREF(Struct);
        ASSERT_REMOVE_CVREF(Union );
        ASSERT_REMOVE_CVREF(Class );

        // 'void'

        ASSERT_SAME(void,                                void                );
        ASSERT_SAME(const          void,                 void                );
        ASSERT_SAME(      volatile void,                 void                );
        ASSERT_SAME(const volatile void,                 void                );

        // Pointers

        ASSERT_SAME(int,                                 int                 );
        ASSERT_SAME(int *,                               int *               );
        ASSERT_SAME(int const *,                         int const *         );
        ASSERT_SAME(int volatile *,                      int volatile *      );
        ASSERT_SAME(int const volatile *,                int const volatile *);

        ASSERT_SAME(int * const,                         int *               );
        ASSERT_SAME(int * volatile,                      int *               );
        ASSERT_SAME(int * const volatile,                int *               );

        ASSERT_SAME(const int * const,                   const int *         );
        ASSERT_SAME(const int * volatile,                const int *         );
        ASSERT_SAME(const int * const volatile,          const int *         );

        ASSERT_SAME(volatile int * const,                volatile int *      );
        ASSERT_SAME(volatile int * volatile,             volatile int *      );
        ASSERT_SAME(volatile int * const volatile,       volatile int *      );

        ASSERT_SAME(const volatile int * const,          const volatile int *);
        ASSERT_SAME(const volatile int * volatile,       const volatile int *);
        ASSERT_SAME(const volatile int * const volatile, const volatile int *);

        ASSERT_SAME(int&,                                int                 );
        ASSERT_SAME(int *&,                              int *               );
        ASSERT_SAME(int const *&,                        int const *         );
        ASSERT_SAME(int volatile *&,                     int volatile *      );
        ASSERT_SAME(int const volatile *&,               int const volatile *);

        ASSERT_SAME(int * const&,                        int *               );
        ASSERT_SAME(int * volatile&,                     int *               );
        ASSERT_SAME(int * const volatile&,               int *               );

        ASSERT_SAME(const int * const&,                  const int *         );
        ASSERT_SAME(const int * volatile&,               const int *         );
        ASSERT_SAME(const int * const volatile&,         const int *         );

        ASSERT_SAME(volatile int * const&,               volatile int *      );
        ASSERT_SAME(volatile int * volatile&,            volatile int *      );
        ASSERT_SAME(volatile int * const volatile&,      volatile int *      );

        ASSERT_SAME(const volatile int * const&,         const volatile int *);
        ASSERT_SAME(const volatile int * volatile&,      const volatile int *);
        ASSERT_SAME(const volatile int * const volatile&,const volatile int *);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_SAME(int&&,                               int                 );
        ASSERT_SAME(int *&&,                             int *               );
        ASSERT_SAME(int const *&&,                       int const *         );
        ASSERT_SAME(int volatile *&&,                    int volatile *      );
        ASSERT_SAME(int const volatile *&&,              int const volatile *);

        ASSERT_SAME(int * const&&,                       int *               );
        ASSERT_SAME(int * volatile&&,                    int *               );
        ASSERT_SAME(int * const volatile&&,              int *               );

        ASSERT_SAME(const int * const&&,                 const int *         );
        ASSERT_SAME(const int * volatile&&,              const int *         );
        ASSERT_SAME(const int * const volatile&&,        const int *         );

        ASSERT_SAME(volatile int * const&&,              volatile int *      );
        ASSERT_SAME(volatile int * volatile&&,           volatile int *      );
        ASSERT_SAME(volatile int * const volatile&&,     volatile int *      );

        ASSERT_SAME(const volatile int * const&&,        const volatile int *);
        ASSERT_SAME(const volatile int * volatile&&,     const volatile int *);
        ASSERT_SAME(const volatile int * const volatile&&,
                    const volatile int *);
#endif

        // Arrays

        ASSERT_SAME(int[5],                              int[5]              );
        ASSERT_SAME(int[5][2],                           int[5][2]           );
        ASSERT_SAME(int[5][2][3],                        int[5][2][3]        );
        ASSERT_SAME(int[],                               int[]               );
        ASSERT_SAME(int[][2],                            int[][2]            );
        ASSERT_SAME(int[][2][3],                         int[][2][3]         );

        ASSERT_SAME(const int[5],                        int[5]              );
        ASSERT_SAME(const int[5][2],                     int[5][2]           );
        ASSERT_SAME(const int[5][2][3],                  int[5][2][3]        );

        ASSERT_SAME(const int[],                         int[]               );
        ASSERT_SAME(const int[][2],                      int[][2]            );
        ASSERT_SAME(const int[][2][3],                   int[][2][3]         );

        ASSERT_SAME(volatile int[5],                     int[5]              );
        ASSERT_SAME(volatile int[5][2],                  int[5][2]           );
        ASSERT_SAME(volatile int[5][2][3],               int[5][2][3]        );

        ASSERT_SAME(volatile int[],                      int[]               );
        ASSERT_SAME(volatile int[][2],                   int[][2]            );
        ASSERT_SAME(volatile int[][2][3],                int[][2][3]         );

        ASSERT_SAME(const volatile int[5],               int[5]              );
        ASSERT_SAME(const volatile int[5][2],            int[5][2]           );
        ASSERT_SAME(const volatile int[5][2][3],         int[5][2][3]        );

        ASSERT_SAME(const volatile int[],                int[]               );
        ASSERT_SAME(const volatile int[][2],             int[][2]            );
        ASSERT_SAME(const volatile int[][2][3],          int[][2][3]         );

        // Class members

        ASSERT_SAME(const int TestType::*,              const int TestType::*);
        ASSERT_SAME(const int (TestType::*)() const,
                    const int (TestType::*)() const);

        ASSERT_SAME(               Pm,                   Pm                  );
        ASSERT_SAME(const          Pm,                   Pm                  );
        ASSERT_SAME(      volatile Pm,                   Pm                  );
        ASSERT_SAME(const volatile Pm,                   Pm                  );

        ASSERT_SAME(               Pmf,                  Pmf                 );
        ASSERT_SAME(const          Pmf,                  Pmf                 );
        ASSERT_SAME(      volatile Pmf,                  Pmf                 );
        ASSERT_SAME(const volatile Pmf,                  Pmf                 );

        ASSERT_SAME(               Pmq,                  Pmq                 );
        ASSERT_SAME(const          Pmq,                  Pmq                 );
        ASSERT_SAME(      volatile Pmq,                  Pmq                 );
        ASSERT_SAME(const volatile Pmq,                  Pmq                 );

        // Pointers to functions

        ASSERT_SAME(void (),                             void ()             );
        ASSERT_SAME(void (*)(),                          void (*)()          );
        ASSERT_SAME(void (*&)(),                         void (*)()          );

        ASSERT_SAME(void (int),                          void (int)          );
        ASSERT_SAME(void (int&),                         void (int&)         );

        ASSERT_SAME(void (&)(),                          void ()             );
        ASSERT_SAME(void (&)(int),                       void (int)          );
        ASSERT_SAME(void (&)(int&),                      void (int&)         );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_SAME(void ( &&) (),                       void ()             );
        ASSERT_SAME(void (*&&) (),                       void (*)()          );
        ASSERT_SAME(void ( &&)(int&),                    void (int&)         );
#endif

        ASSERT_SAME(const int(*                )(),      const int(* )()     );
        ASSERT_SAME(const int(* const          )(),      const int(* )()     );
        ASSERT_SAME(const int(*       volatile )(),      const int(* )()     );
        ASSERT_SAME(const int(* const volatile )(),      const int(* )()     );

        ASSERT_SAME(const int(*               &)(),      const int(* )()     );
        ASSERT_SAME(const int(* const         &)(),      const int(* )()     );
        ASSERT_SAME(const int(*       volatile&)(),      const int(* )()     );
        ASSERT_SAME(const int(* const volatile&)(),      const int(* )()     );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT_SAME(const int(*               &&)(),     const int(* )()     );
        ASSERT_SAME(const int(* const         &&)(),     const int(* )()     );
        ASSERT_SAME(const int(*       volatile&&)(),     const int(* )()     );
        ASSERT_SAME(const int(* const volatile&&)(),     const int(* )()     );
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
// Copyright 2021 Bloomberg Finance L.P.
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
