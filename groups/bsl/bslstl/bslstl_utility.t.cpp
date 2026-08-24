// bslstl_utility.t.cpp                                               -*-C++-*-
#include <bslstl_utility.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <bsltf_copymovestate.h>
#include <bsltf_copymovetracker.h>
#include <bsltf_movablealloctesttype.h>

#include <stdlib.h>    // `atoi`
#include <stdio.h>     // `sprintf`, `snprintf` [NOT `<cstdio>`, which does not
                       // include `snprintf`]

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// The component under test provides implementations for utilities not
// provided by the underlying standard library implementation.
// ----------------------------------------------------------------------------
//
// [ 1] const T& as_const (T &t);
// [ 2] T exchange(T& obj, U&& newValue);
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        fflush(stdout);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                    COMPILER FEATURE ABBREVIATIONS
// ----------------------------------------------------------------------------

/// This macro is defined if `bsl::exchange` has the signature and the
/// semantics of `std::exchange`, rather than those of the C++03 emulation
/// thereof.  Due to imperfect emulation of move semantics, when this macro is
/// not defined, a copy assignment (rather than move assignment) will occur
/// when the second argument to `bsl::exchange` is an rvalue.
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY) ||               \
    defined(BSLS_COMPILERFEATURES_FULL_CPP11)
#define u_EXCHANGE_IS_STD_CONFORMING 1
#endif

// ============================================================================
//                       GLOBAL TEST ALIASES
// ----------------------------------------------------------------------------

using namespace BloombergLP;

typedef bsltf::CopyMoveState CMS;

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                              TEST FUNCTIONS
// ----------------------------------------------------------------------------

/// An aggregate for holding two values. Used to test `bsl_as_const`.
template <class T1, class T2>
struct MyPair
{
    T1 d_first;
    T2 d_second;
};

                             // ==================
                             // class TrackedValue
                             // ==================

/// This class holds an integer value and, using a `bsltf::CopyMoveTracker`
/// subobject, tracks whether it has been copied into, moved into, or moved
/// from, so that the value category of the arguments supplied to
/// `bsl::exchange` can be verified.  Used to test `bsl_exchange`.
class TrackedValue {

    // DATA
    bsltf::CopyMoveTracker d_tracker;
    int                    d_value;

  public:
    // CREATORS

    /// Create an object having the specified `value`.
    explicit TrackedValue(int value)
    : d_tracker()
    , d_value(value)
    {
    }

    /// Create an object having the value of the specified `original` object.
    TrackedValue(const TrackedValue& original)
    : d_tracker(original.d_tracker)
    , d_value(original.d_value)
    {
    }

    /// Create an object having the value of the specified `original` object.
    /// Note that this overload, rather than the one taking a `const`
    /// reference, is selected when `original` is a non-`const` lvalue.
    TrackedValue(TrackedValue& original)
    : d_tracker(original.d_tracker)
    , d_value(original.d_value)
    {
    }

    /// Create an object having the value of the specified `original` object,
    /// which is left in the moved-from state.
    TrackedValue(bslmf::MovableRef<TrackedValue> original)
    : d_tracker(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_tracker))
    , d_value(bslmf::MovableRefUtil::access(original).d_value)
    {
    }

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    TrackedValue& operator=(const TrackedValue& rhs)
    {
        d_tracker = rhs.d_tracker;
        d_value   = rhs.d_value;
        return *this;
    }

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that this overload, rather than the one taking a `const` reference, is
    /// selected when `rhs` is a non-`const` lvalue.
    TrackedValue& operator=(TrackedValue& rhs)
    {
        d_tracker = rhs.d_tracker;
        d_value   = rhs.d_value;
        return *this;
    }

    /// Assign to this object the value of the specified `rhs` object, which is
    /// left in the moved-from state, and return a reference providing
    /// modifiable access to this object.
    TrackedValue& operator=(bslmf::MovableRef<TrackedValue> rhs)
    {
        TrackedValue& lvalue = bslmf::MovableRefUtil::access(rhs);

        d_tracker = bslmf::MovableRefUtil::move(lvalue.d_tracker);
        d_value   = lvalue.d_value;
        return *this;
    }

    // ACCESSORS

    /// Return the value of this object.
    int value() const
    {
        return d_value;
    }

    // HIDDEN FRIENDS

    /// Return the copy/move state of the specified `object`.  This function is
    /// an ADL customization point used by the pseudo-accessors of
    /// `bsltf::CopyMoveState`.
    friend bsltf::CopyMoveState::Enum copyMoveState(
                                                 const TrackedValue& object)
    {
        return object.d_tracker.copyMoveState();
    }
};

namespace test_case_2 {
struct C {
    C& operator=(int) { return *this; }
};

/// Return `true` if the argument has type `C` and is not an lvalue.  This is
/// used to verify the return type of `bsl::exchange` in C++03.
template <class t_TYPE>
bool isCRvalue(t_TYPE)
{
    return bsl::is_same<t_TYPE, C>::value;
}

/// Note that this overload is ambiguous with the one above if the argument is
/// an lvalue or a const rvalue.
template <class t_TYPE>
bool isCRvalue(t_TYPE&);

/// This class is used to test the behavior of `bsl::exchange` when the
/// assignment step fails by throwing an exception.
struct ThrowingAssignment {
    ThrowingAssignment() {}
    ThrowingAssignment(const ThrowingAssignment&) {}
    ThrowingAssignment& operator=(const ThrowingAssignment&)
    {
        throw 2;
    }
};
}  // close namespace test_case_2

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `exchange`
        //
        // Concerns:
        // 1. `bsl::exchange` returns the value held by `obj` prior to the
        //    call, and assigns the value of `newValue` to `obj`.
        // 2. The return type of `bsl::exchange` is the (non-reference) type of
        //    `obj`.
        // 3. `obj` and `newValue` may have different types.
        // 4. The value returned is moved out of, rather than copied out of,
        //    `obj`.
        // 5. A `const` lvalue `newValue` is copied into `obj` as a `const`
        //    lvalue, and is left unchanged.
        // 6. A non-`const` lvalue `newValue` is copied into `obj` as a
        //    non-`const` lvalue, and is left unchanged.
        // 7. A `newValue` that refers to a `bslmf::MovableRef` object is moved
        //    into `obj`, and is left in the moved-from state.
        // 8. In C++11 and later, where `bsl::exchange` has the exact
        //    signature of `std::exchange`, an rvalue `newValue` is moved into
        //    `obj`, and a braced initializer list may be supplied as
        //    `newValue`.
        // 9. If `obj` is of an allocator-aware type having a move
        //    constructor, then the value returned uses the allocator of
        //    `obj`, rather than the default allocator, because that value is
        //    move constructed from `obj`.  Note that this holds in every
        //    language mode.
        //10. Any exception thrown during assignment to `obj` is propagated.
        //
        // Plan:
        // 1. Call `bsl::exchange` on objects of some simple types, supplying a
        //    `newValue` of the same type as `obj` and of a different (but
        //    convertible) type, and verify the value returned and the
        //    resulting value of `obj`.  In C++11 and later, verify the return
        //    type using `decltype`; otherwise, verify it using an ad hoc
        //    overload resolution technique.  (C-1..3)
        // 2. Call `bsl::exchange` on an object of a type having a
        //    `bsltf::CopyMoveTracker` subobject, supplying, in turn, a `const`
        //    lvalue, a non-`const` lvalue, and a movable reference as
        //    `newValue`.  In each case, verify the value returned, the values
        //    of `obj` and of `newValue`, and the copy/move state of each.
        //    (C-1, C-4..7)
        // 3. In C++11 and later, call `bsl::exchange` supplying an rvalue as
        //    `newValue`, and supplying a braced initializer list as
        //    `newValue`.  (C-8)
        // 4. Install a test allocator as the default allocator and call
        //    `bsl::exchange` on an object of an allocator-aware type, having
        //    a move constructor, that was created with a different test
        //    allocator.  Verify that the value returned uses the allocator of
        //    `obj`, and that the default allocator is not used at all.  (C-9)
        // 5. Define a class type with a throwing assignment operator and pass
        //    it to `bsl::exchange`; verify that the expected exception can be
        //    caught by a handler surrounding the call.  (C-10)
        //
        // Testing:
        //   T exchange(T& obj, U&& newValue);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `exchange`"
                            "\n==================\n");

        using namespace test_case_2;
        {
            int mX = 1;

            ASSERT(1 == bsl::exchange(mX, 2));
            ASSERT(2 == mX);

            double mY = 1.5;

            ASSERT(1.5 == bsl::exchange(mY, 3));  // `newValue` is an `int`
            ASSERT(3.0 == mY);

            int  mZ = 4;
            int *mP = &mX;

            ASSERT(&mX == bsl::exchange(mP, &mZ));
            ASSERT(&mZ == mP);

            C mC;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
            ASSERT((bsl::is_same<decltype(bsl::exchange(mX, 2)), int>::value));
            ASSERT((bsl::is_same<decltype(bsl::exchange(mC, 3)), C>::value));
#else
            ASSERT(isCRvalue(bsl::exchange(mC, 1)));
#endif
        }

        if (veryVerbose) printf("\t`const` lvalue `newValue`\n");
        {
            TrackedValue       mX(1);
            const TrackedValue Z(2);

            const TrackedValue& old = bsl::exchange(mX, Z);

            ASSERT(1 == old.value());
            ASSERT(2 == mX.value());

            ASSERT(CMS::isMovedInto(old));
            ASSERT(CMS::isCopiedConstInto(mX));
        }

        if (veryVerbose) printf("\tNon-`const` lvalue `newValue`\n");
        {
            TrackedValue mX(1);
            TrackedValue mZ(2);

            const TrackedValue& old = bsl::exchange(mX, mZ);

            ASSERT(1 == old.value());
            ASSERT(2 == mX.value());
            ASSERT(2 == mZ.value());

            ASSERT(CMS::isMovedInto(old));
            ASSERT(CMS::isCopiedNonconstInto(mX));
            ASSERT(CMS::isOriginal(mZ));
        }

        if (veryVerbose) printf("\tMovable reference `newValue`\n");
        {
            TrackedValue mX(1);
            TrackedValue mZ(2);

            const TrackedValue& old =
                            bsl::exchange(mX, bslmf::MovableRefUtil::move(mZ));

            ASSERT(1 == old.value());
            ASSERT(2 == mX.value());

            ASSERT(CMS::isMovedInto(old));
            ASSERT(CMS::isMovedInto(mX));
            ASSERT(CMS::isMovedFrom(mZ));
        }

        if (veryVerbose) printf("\tAllocator-aware `obj`\n");
        {
            typedef bsltf::MovableAllocTestType Obj;

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj       mX(1, &oa);
            const Obj Z(2, &oa);

            const Obj& old = bsl::exchange(mX, Z);

            ASSERT(1 == old.data());
            ASSERT(2 == mX.data());

            // The value returned is move constructed from `mX`, and so uses
            // the allocator of `mX`; had it been copy constructed instead, it
            // would use the default allocator.

            ASSERT(CMS::isMovedInto(old));
            ASSERT(&oa == old.allocator());
            ASSERT(&oa == mX.allocator());
            ASSERT(0   == da.numBlocksTotal());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) printf("\tthrowing assignment\n");
        {
            int caughtValue = -1;
            try {
                ThrowingAssignment t1, t2;
                bsl::exchange(t1, t2);
            } catch (int val) {
                caughtValue = val;
            }
            ASSERT(2 == caughtValue);
        }
#else
        if (veryVerbose) printf("\texceptions disabled\n");
#endif

#ifdef u_EXCHANGE_IS_STD_CONFORMING
        if (veryVerbose) printf("\tRvalue `newValue`\n");
        {
            TrackedValue mX(1);

            const TrackedValue& old = bsl::exchange(mX, TrackedValue(2));

            ASSERT(1 == old.value());
            ASSERT(2 == mX.value());

            ASSERT(CMS::isMovedInto(old));
            ASSERT(CMS::isMovedInto(mX));
        }

        if (veryVerbose) printf("\tBraced initializer list `newValue`\n");
        {
            // The type of `newValue` cannot be deduced from a braced
            // initializer list, so it defaults to the type of `obj`.

            int mX = 1;

            ASSERT(1 == bsl::exchange(mX, {}));
            ASSERT(0 == mX);
        }
#endif  // u_EXCHANGE_IS_STD_CONFORMING

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //
        // Concerns:
        // 1. The return type of `bsl::as_const` is a reference offering
        //    non-modifiable access to the type of the parameter passed to the
        //    function.
        // 2. The return value of `bsl::as_const` is the same object that is
        //    passed to the function.
        //
        // Plan:
        // 1. Call `as_const` with different types and values.
        // 2. Ensure that the type and value of the return is correct.
        //
        // Testing:
        //   const T& as_const (T &t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTIONALITY TEST"
                            "\n==================\n");

        typedef MyPair<int, double> Obj;

        Obj X;
        X.d_first = 23;
        X.d_second = 42.0;
        const Obj &cX = bsl::as_const(X);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        ASSERT((bsl::is_same<decltype(bsl::as_const(X)), const Obj &>::value));
#endif
        ASSERT(&cX.d_first == &X.d_first);  // same object

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
// Copyright 2022 Bloomberg Finance L.P.
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
