// bsltf_movestate.t.cpp                                              -*-C++-*-
#include <bsltf_movestate.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>   // printf
#include <stdlib.h>  // atoi
#include <string.h>  // strlen, strcmp

using namespace BloombergLP;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single enumeration having sequential
// enumerator values.
//
// Given the dependency restrictions of this component, we will therefore
// follow only the first step of our standard 3-step approach to testing
// enumeration types.
// ----------------------------------------------------------------------------
// TYPES
// [ 1] enum Enum { ... };
//
// CLASS METHODS
// [ 1] const char *toAscii(SystemClockType::Enum val);
//
// ADL CUSTOMIZATION POINTS
// [ 2] MoveState::Enum bsltf::getMovedFrom(const TYPE& object);
// [ 2] MoveState::Enum bsltf::getMovedInto(const TYPE& object);
// [ 2] void bsltf::setMovedInto(TYPE *object, MoveState::Enum value);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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


// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsltf::MoveState::Enum Enum;
typedef bsltf::MoveState       Obj;
typedef bsltf::CopyMoveState   Cms;

/// Class that supplies ADL-findable `getMovedFrom`, `getMovedInto`, and
/// `setMovedInto` functions.
struct AdlClass1 {
    Enum d_movedFrom;
    Enum d_movedInto;

    AdlClass1() : d_movedFrom(Obj::e_NOT_MOVED), d_movedInto(Obj::e_NOT_MOVED)
        { }

    friend Enum getMovedFrom(const AdlClass1& obj) { return obj.d_movedFrom; }
    friend Enum getMovedInto(const AdlClass1& obj) { return obj.d_movedInto; }
    friend void setMovedInto(AdlClass1* obj_p, Enum value)
        { obj_p->d_movedInto = value; }
};

/// Class that supplies ADL-findable `getMovedFrom`, `getMovedInto`, and
/// `setMovedInto` functions.
struct AdlClass2 {

    Cms::Enum d_copyMoveState;

    AdlClass2() : d_copyMoveState(Cms::e_ORIGINAL) { }

    friend Cms::Enum copyMoveState(const AdlClass2& obj)
        { return obj.d_copyMoveState; }

    friend void setCopyMoveState(AdlClass2 *obj, Cms::Enum value)
        { obj->d_copyMoveState = value; }
};

/// Class with no customization points.
struct nonAdlClass {
};

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const int NUM_ENUMERATORS = 3;

#define UNKNOWN_FORMAT "(* UNKNOWN *)"

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// `bsltf::MoveState`.
//
// First, we create a variable `value` of type `bsltf::MoveState::Enum`
// and initialize it with the enumerator value
// `bsltf::MoveState::e_MOVED`:
// ```
    bsltf::MoveState::Enum value = bsltf::MoveState::e_MOVED;
// ```
// Now, we store the address of its ASCII representation in a pointer variable,
// `asciiValue`, of type `const char *`:
// ```
    const char *asciiValue = bsltf::MoveState::toAscii(value);
// ```
// Finally, we verify the value of `asciiValue`:
// ```
    ASSERT(0 == strcmp(asciiValue, "MOVED"));
// ```

      } break;
      case 2: {
        // -------------------------------------------------------------------
        // TESTING ADL CUSTOMIZATION POINTS
        //
        // Concerns:
        // 1. If a type supplies a customization of the namespace functions
        //    `getMovedFrom`, `getMovedInto`, and/or `setMovedInto`, then an
        //    unqualified call to one of those functions on an object of that
        //    type calls the appropriate customization function.
        //
        // 2. If a type does *not* supply a customization of the namespace
        //    functions `getMovedFrom`, `getMovedInto`, and/or `setMovedInto`
        //    but *does* supply an customizatoin function for `copyMoveState`
        //    and/or `setCopyMoveState` then an unqualified call to one of the
        //    first three functions on an object of that type yields an adapted
        //    call to one of the latter two functions.
        //
        // 3. Otherwise `getMovedFrom` and `getMovedInto` return
        //    `MoveState::e_UNKNOWN` and `setMovedInto` does nothing.
        //
        // Plan:
        // 1. Create a class in its own namespace having customized versions of
        //    namespace-scope `getMovedFrom`, `getMovedInto`, and
        //    `setMovedInto`.  Verify that these customizations are called when
        //    the corresponding function is called without qualification.
        //    (C-1)
        //
        // 2. Create a class in its own namespace having customized versions of
        //    `copyMoveState` and `setCopyMoveState`.  Verify that an
        //    unqualified call to `getMovedFrom`, `getMovedInto`, or
        //    `setMovedInto` calls the appropriate customization point and
        //    adapts it correctly.  (C-2)
        //
        // 3. Verify that an unqualified call to `getMovedFrom`, `getMovedInto`
        //    on an `int` or a class not providing the needed customization
        //    points returns `e_UNKNOWN`.  Verify that an unqualified call to
        //    `setMovedInto` succeeds with no detectable effect.
        //
        // Testing
        //    MoveState::Enum bsltf::getMovedFrom(const TYPE& object);
        //    MoveState::Enum bsltf::getMovedInto(const TYPE& object);
        //    void bsltf::setMovedInto(TYPE *object, MoveState::Enum value);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING ADL CUSTOMIZATION POINTS"
                            "\n================================\n");

        using bsltf::getMovedFrom;
        using bsltf::getMovedInto;
        using bsltf::setMovedInto;

        AdlClass1 adl1; const AdlClass1& ADL1 = adl1;
        ASSERT(Obj::e_NOT_MOVED == getMovedFrom(ADL1));
        ASSERT(Obj::e_NOT_MOVED == getMovedInto(ADL1));

        setMovedInto(&adl1, Obj::e_MOVED);
        ASSERT(Obj::e_NOT_MOVED == getMovedFrom(ADL1));
        ASSERT(Obj::e_MOVED     == getMovedInto(ADL1));

        adl1.d_movedFrom = Obj::e_MOVED;
        setMovedInto(&adl1, Obj::e_UNKNOWN);
        ASSERT(Obj::e_MOVED   == getMovedFrom(ADL1));
        ASSERT(Obj::e_UNKNOWN == getMovedInto(ADL1));

        AdlClass2 adl2; const AdlClass2& ADL2 = adl2;
        ASSERT(Obj::e_NOT_MOVED == getMovedFrom(ADL2));
        ASSERT(Obj::e_NOT_MOVED == getMovedInto(ADL2));

        Cms::set(&adl2, Cms::e_MOVED_FROM);
        ASSERT(Obj::e_MOVED     == getMovedFrom(ADL2));
        ASSERT(Obj::e_NOT_MOVED == getMovedInto(ADL2));

        setMovedInto(&adl2, Obj::e_MOVED);
        ASSERT(Cms::e_MOVED_INTO == copyMoveState(ADL2));
        ASSERT(Obj::e_NOT_MOVED  == getMovedFrom(ADL2));
        ASSERT(Obj::e_MOVED      == getMovedInto(ADL2));

        setMovedInto(&adl2, Obj::e_UNKNOWN);
        ASSERT(Cms::e_UNKNOWN == copyMoveState(ADL2));
        ASSERT(Obj::e_UNKNOWN == getMovedFrom(ADL2));
        ASSERT(Obj::e_UNKNOWN == getMovedInto(ADL2));

      } break;
      case 1: {
        // -------------------------------------------------------------------
        // TESTING `enum` AND `toAscii`
        //
        // Concerns:
        // 1. The enumerator values are sequential, starting from 0.
        //
        // 2. The `toAscii` method returns the expected string representation
        //    for each enumerator.
        //
        // 3. The `toAscii` method returns a distinguished string when passed
        //    an out-of-band value.
        //
        // 4. The string returned by `toAscii` is non-modifiable.
        //
        // 5. The `toAscii` method has the expected signature.
        //
        // Plan:
        // 1. Verify that the enumerator values are sequential, starting from
        //    0.  (C-1)
        //
        // 2. Verify that the `toAscii` method returns the expected string
        //    representation for each enumerator.  (C-2)
        //
        // 3. Verify that the `toAscii` method returns a distinguished string
        //    when passed an out-of-band value.  (C-3)
        //
        // 4. Take the address of the `toAscii` (class) method and use the
        //    result to initialize a variable of the appropriate type.
        //    (C-4, C-5)
        //
        // Testing:
        //   enum Enum { ... };
        //   const char *toAscii(SystemClockType::Enum val);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `enum` AND `toAscii`"
                            "\n============================\n");

#if BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wconversion"
#endif

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line         enumerator value        expected result
            // ----    -----------------------      -----------------
            {  L_,     Obj::e_NOT_MOVED,             "NOT_MOVED"      },
            {  L_,     Obj::e_MOVED,                 "MOVED"          },
            {  L_,     Obj::e_UNKNOWN,               "UNKNOWN"        },

#ifndef BDE_BUILD_TARGET_UBSAN
            {  L_,     (Enum)NUM_ENUMERATORS,       UNKNOWN_FORMAT    },
            {  L_,     (Enum)99,                    UNKNOWN_FORMAT    }
#endif
        };

#if BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#       pragma GCC diagnostic pop
#endif

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("\nVerify enumerator values are sequential.\n");

        for (int ti = 0; ti < NUM_ENUMERATORS; ++ti) {
            const Enum VALUE = DATA[ti].d_value;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            ASSERTV(ti, VALUE, ti == VALUE);
        }

        if (verbose) printf("\nTesting `toAscii`.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;
            const Enum  VALUE = DATA[ti].d_value;
            const char *EXP   = DATA[ti].d_exp;

            const char *result = Obj::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            ASSERTV(LINE, ti, EXP, result, strlen(EXP) == strlen(result));
            ASSERTV(LINE, ti, EXP, result,      0 == strcmp(EXP, result));
        }

        if (verbose) printf("\nVerify `toAscii` signature.\n");

        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Obj::toAscii;  (void)FP;
        }

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
// Copyright 2014 Bloomberg Finance L.P.
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
