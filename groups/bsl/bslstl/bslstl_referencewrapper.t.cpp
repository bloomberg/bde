// bslstl_referencewrapper.t.cpp                                      -*-C++-*-
#include <bslstl_referencewrapper.h>

#include <bslmf_isreferencewrapper.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <functional>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is fully specified by the standard, verifiable by
// inspection.  Therefore, testing only exercises basic uses to ensure template
// specializations may be instantiated successfully.
//
// ----------------------------------------------------------------------------
// TESTING INSTANTIATIONS AND BASIC FUNCTIONALITY
// [ 1] reference_wrapper(T&);
// [ 1] reference_wrapper(const reference_wrapper<T>&);
// [ 1] reference_wrapper<T>& operator=(reference_wrapper<T>&);
// [ 1] operator T&() const;
// [ 1] T& get() const;
// [ 3] operator()(...) const;
// [ 1] reference_wrapper<T> cref(const T&);
// [ 1] reference_wrapper<T> cref(reference_wrapper<T>);
// [ 1] reference_wrapper<T> ref(T&);
// [ 1] reference_wrapper<T> ref(reference_wrapper<T>);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] TYPE TRAITS

// ============================================================================
//                  STANDARD BSL ASSERT TEST FUNCTION
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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct Callable {
    void operator()() { }
    int  operator()(int) { return 42; }
    int  operator()(int) const { return -42; }
};

struct NonBitwiseDummy {
    // Test class that is not bitwise moveable
    int d_data;

    explicit NonBitwiseDummy(int v = 0) : d_data(v) { }
    NonBitwiseDummy(const NonBitwiseDummy& rhs) : d_data(rhs.d_data) {}
};

// ============================================================================
//                               TEST FACILITIES
// ----------------------------------------------------------------------------

struct Dummy {
    void operator&();
    const Dummy *localAddressOf() const { return this; }
};
bool isConst(Dummy&) { return false; }
bool isConst(const Dummy&) { return true; }
    // These functions are used solely to verify that a 'reference_wrapper' can
    // be passed to a function expecting an actual reference.

// ============================================================================
//              USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

// Disable specific bde_verify warnings where practice of usage example may
// differ.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01  // Function needs contract, we probably should fix
// BDE_VERIFY pragma: -IND01 // Code is aligned as-if following a '//' comment


namespace TEST_CASE_USAGE {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Sorted References
/// - - - - - - - - - - - - - -
// Let us suppose that we wish to handle objects that will be passed to a
// comparison function expecting references to the objects.  Let us suppose
// further that these objects are large enough that we would not wish to move
// them around bodily as they are sorted.  Note that plausible examples of uses
// for this component are limited in freestanding C++98.
//
// First, let us define the large-object type:
//..
    struct Canary {
        static const int s_size = 1000;
        Canary *d_values[s_size];
        Canary();
    };
//..
    Canary::Canary()
    {
         for (int i = 0; i < s_size; ++i) {
             d_values[i] = this;
         }
    }
//..
// Next, we define the comparison function:
//..
    bool operator<(Canary const& a, Canary const& b)
    {
        return a.d_values[0] < b.d_values[0];
    }
//..
// Finally, we define a generic function to sort two items:
//..
    template <class T>
    void sortTwoItems(T& a, T& b)
    {
        if (b < a) {
            T tmp(a);
            a = b;
            b = tmp;
        }
    }
//..
}  // close namespace TEST_CASE_USAGE

// BDE_VERIFY pragma: pop

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning
    (void)veryVeryVeryVerbose;  // unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example in the component header file compiles, links,
        //:   and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nUSAGE EXAMPLE"
                   "\n=============\n");
        }

        using namespace TEST_CASE_USAGE;

// Disable specific bde_verify warnings where practice of usage example may
// differ.

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01  // Function needs contract, we probably should fix
// BDE_VERIFY pragma: -IND01 // Code is aligned as-if following a '//' comment

//..
// We can call 'sortTwoItems' on wrappers representing 'Canary' objects
// without need to move actual, large 'Canary' objects about.  In the call to
// 'sortTwoItems', below, the 'operator=' used in it is that of
// 'bsl::reference_wrapper<Canary>', but the 'operator<' used is the one
// declared for 'Canary&' arguments.  All of the conversions needed are
// applied implicitly:
//..
    Canary canaries[2];

    bsl::reference_wrapper<Canary> canaryA = bsl::ref(canaries[1]);
    bsl::reference_wrapper<Canary> canaryB = bsl::ref(canaries[0]);
    sortTwoItems(canaryA, canaryB);
//
    ASSERT(&canaryA.get() == canaries);
    ASSERT(&canaryB.get() == canaries + 1);
//..

// BDE_VERIFY pragma: pop

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CALLABLE
        //   Testing that bsl::reference_wrapper is callable (in C++11)
        //
        // Concerns:
        //: 1 That in appropriate build modes, bsl::reference_wrapper is
        //:   callable
        //
        // Plan:
        //: 1 Verify 'BSLSTL_REFRENCEWRAPPER_IS_ALIASED' is 'true' on
        //:   on C++14 platforms.  Note that this test is a sanity check
        //:   and deliberately does not reproduce the logic in the component.
        //:
        //: 2 Verify if 'BSLSTL_REFRENCEWRAPPER_IS_ALIASED' is 'true'
        //:   'bsl::reference_wrapper' is an alias to the platform standard
        //:   library.
        //:
        //: 3 Verify if 'BSLSTL_REFRENCEWRAPPER_IS_ALIASED' we can
        //:   assign a function reference to a 'reference_wrapper' and
        //:   invoke it.
        //
        // Testing:
        //   operator()(...) const;
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING CALLABLE\n"
                   "\n================\n");
        }

#if  BSLS_COMPILERFEATURES_CPLUSPLUS >= 201402L // sanity check
#ifndef BSLSTL_REFRENCEWRAPPER_IS_ALIASED
        BSLMF_ASSERT(false && "reference_wrapper not aliased in C++14");
#endif
#endif

#ifdef BSLSTL_REFRENCEWRAPPER_IS_ALIASED
        if (verbose) {
            printf("\tverify 'reference_wrapper' is an alias to std\n");
        }

        ASSERT(true ==
               (bsl::is_same<std::reference_wrapper<Callable>,
                             bsl::reference_wrapper<Callable> >::value));

        if (verbose) {
            printf("\tverify 'reference_wrapper' is callbable\n");
        }

        {
            Callable c;
            bsl::reference_wrapper<Callable> ref = bsl::ref(c);
            ref();
            ASSERT(42 == ref(0));
        }
        {
            Callable c;
            bsl::reference_wrapper<const Callable> ref = bsl::cref(c);
            ASSERT(-42 == ref(0));
        }
#endif // BSLSTL_REFRENCEWRAPPER_IS_ALIASED

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        //
        // Concerns:
        //: 1 'bslmf::IsBitwiseMoveable<reference_wrapper<T>>::value' is
        //:   true regardless of the type of 'T'.
        //:
        //: 2 'bslmf::IsReferenceWrapper<reference_wrapper<T>>::value' is
        //:   true regardless of the type of 'T'.
        //
        // Plan:
        //: 1 For concern 1, instantiate 'reference_wrapper' with both bitwise
        //:   moveable and non-bitwise moveable types and verify that the
        //:   'bslmf::isBitwiseMovable' trait is true for both cases.
        //:
        //: 2 For concern 2, instantiate 'reference_wrapper' with 2
        //:   unrelated types and verify that the 'bslmf::IsReferenceWrapper'
        //:   trait is true for both cases.
        //
        // Testing:
        //    TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTYPE TRAITS"
                            "\n===========\n");

        using BloombergLP::bslmf::IsBitwiseMoveable;
        using BloombergLP::bslmf::IsReferenceWrapper;
        using bsl::reference_wrapper;

        // First, verify that we have one bitwise movable type and one not
        // bitwise moveable type.
        ASSERT(  IsBitwiseMoveable<int>::value);
        ASSERT(! IsBitwiseMoveable<NonBitwiseDummy>::value);

        // Now test that 'reference_wrapper' is bitwise moveable in either
        // case.
        ASSERT(IsBitwiseMoveable<reference_wrapper<int> >::value);
        ASSERT(IsBitwiseMoveable<reference_wrapper<NonBitwiseDummy> >::value);

        // Test that 'reference_wrapper' is indeed a reference wrapper.
        ASSERT(  IsReferenceWrapper<reference_wrapper<int> >::value);
        ASSERT(  IsReferenceWrapper<reference_wrapper<Dummy> >::value);

        // Test that the trait specialization provided by this component does
        // not incidentally cause 'IsReferenceWrapper' to be true for actual
        // reference types.
        ASSERT(! IsReferenceWrapper<int&>::value);
        ASSERT(! IsReferenceWrapper<Dummy&>::value);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TESTS
        //   Create objects each possible way, verify they have the right
        //   contents and can be used in the standard ways.
        //
        // Concerns:
        //: 1 Template functions defined are only checked by the compiler when
        //:   called, so all templates must be instantiated here.
        //:
        //: 2 The various functions' argument must be copied into the object
        //:   correctly.
        //:
        //: 3 The accessors must reproduce the various functions' argument
        //:   values.
        //
        // Plan:
        //: 1 Define a dummy type.
        //:
        //: 2 Wrap the dummy type using each available method.
        //:
        //: 3 Use the wrappers' explicit and implicit accessors.  (C-1..3)
        //
        // Testing:
        //   reference_wrapper(T&);
        //   reference_wrapper(const reference_wrapper<T>&);
        //   reference_wrapper<T>& operator=(reference_wrapper<T>&);
        //   operator T&() const;
        //   T& get() const;
        //   reference_wrapper<T> cref(const T&);
        //   reference_wrapper<T> cref(reference_wrapper<T>);
        //   reference_wrapper<T> ref(T&);
        //   reference_wrapper<T> ref(reference_wrapper<T>);
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBASIC TESTS"
                   "\n===========\n");
        }

        Dummy       a;
        const Dummy b = {};

        ASSERT(!isConst(a) && isConst(b));  // Test 'isConst'.

        bsl::reference_wrapper<      Dummy> rwa (a);
        bsl::reference_wrapper<const Dummy> rwca(a);
        bsl::reference_wrapper<const Dummy> rwcb(b);

        bsl::reference_wrapper<      Dummy> copyrwa (rwa );
        bsl::reference_wrapper<const Dummy> copyrwca(rwca);
        bsl::reference_wrapper<const Dummy> copyrwcb(rwcb);

        ASSERT(!isConst(rwa));  //  Check conversion to ref type, constness.
        ASSERT( isConst(rwca)); //  Likewise.
        ASSERT( isConst(rwcb));

        copyrwa  = a;   // Assign from raw reference.
        copyrwca = a;   // Likewise.
        copyrwcb = b;

        copyrwa  = rwa;     // Assign from other 'reference_wrapper'.
        copyrwca = rwca;    // Likewise.
        copyrwcb = rwcb;

        Dummy&       rax  = rwa;    // Initialize from 'reference_wrapper'.
        const Dummy& rcax = rwca;   // Likewise.
        const Dummy& rcbx = rwcb;

        Dummy&       ray  = bsl::ref(a);    // Initialize from ref result.
        const Dummy& rcay = bsl::cref(a);   // Likewise.
        const Dummy& rcby = bsl::cref(b);

        ASSERT(!isConst(bsl::ref (a)));
        ASSERT( isConst(bsl::cref(a)));
        ASSERT( isConst(bsl::cref(b)));

        ASSERT(!isConst(bsl::ref (rwa )));
        ASSERT( isConst(bsl::cref(rwca)));
        ASSERT( isConst(bsl::cref(rwcb)));

        bsl::reference_wrapper<      Dummy> copyrwaz (bsl::ref(rwa ));
        bsl::reference_wrapper<const Dummy> copyrwcaz(bsl::ref(rwca));
        bsl::reference_wrapper<const Dummy> copyrwcbz(bsl::ref(rwcb));

        bsl::reference_wrapper<const Dummy> copyrwcazb(bsl::cref(rwca));
        bsl::reference_wrapper<const Dummy> copyrwcbzc(bsl::cref(rwcb));

        Dummy c;

        bsl::reference_wrapper<Dummy> assrwaz(bsl::ref(rwa));
        assrwaz = a;
        assrwaz = c;
        assrwaz = rwa;

        bsl::reference_wrapper<const Dummy> assrwcaz(bsl::ref(rwca));
        assrwcaz = b;
        assrwcaz = c;
        assrwcaz = rwca;
        assrwcaz = rwcb;

        ASSERT(copyrwa.get().localAddressOf() == a.localAddressOf());
        ASSERT(copyrwca.get().localAddressOf() == a.localAddressOf());
        ASSERT(copyrwcb.get().localAddressOf() == b.localAddressOf());
        ASSERT(copyrwaz.get().localAddressOf() == a.localAddressOf());
        ASSERT(copyrwcaz.get().localAddressOf() == a.localAddressOf());
        ASSERT(copyrwcbz.get().localAddressOf() == b.localAddressOf());

        // These do not test much, but they seem preferable to "void(e)":

        ASSERT(rax.localAddressOf() == a.localAddressOf());
        ASSERT(rcax.localAddressOf() == a.localAddressOf());
        ASSERT(rcbx.localAddressOf() == b.localAddressOf());
        ASSERT(ray.localAddressOf() == a.localAddressOf());
        ASSERT(rcay.localAddressOf() == a.localAddressOf());
        ASSERT(rcby.localAddressOf() == b.localAddressOf());
        ASSERT(copyrwcazb.get().localAddressOf() == a.localAddressOf());
        ASSERT(copyrwcbzc.get().localAddressOf() == b.localAddressOf());
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
