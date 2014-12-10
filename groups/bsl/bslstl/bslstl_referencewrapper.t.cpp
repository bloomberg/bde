// bslstl_referencewrapper.t.cpp                                      -*-C++-*-
#include <bslstl_referencewrapper.h>

#include <bsls_bsltestutil.h>

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
// class reference_wrapper<T> (reference_wrapper)
// ============================================================================
//
// ----------------------------------------------------------------------------
// TESTING INSTANTIATIONS AND BASIC FUNCTIONALITY
// [ 1] reference_wrapper(T&);
// [ 1] reference_wrapper(consts reference_wrapper<T>&)
// [ 1] reference_wrapper<T>& operator=(reference_wrapper<T>&) const;
// [ 1] operator T&() const;
// [ 1] T& get() const;
// [ 1] reference_wrapper<T> cref(T&)
// [ 1] reference_wrapper<T> ref(T&)
// ----------------------------------------------------------------------------
// [ 1] BASIC TESTS
// [ 2] USAGE EXAMPLE

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               TEST FACILITIES
// ----------------------------------------------------------------------------

struct dummy {};
void use(dummy&) {}
void const_use(const dummy&) {}
    // these functions are used solely to verify that a reference_wrapper can
    // be passed to a function expecting an actual reference.

// ============================================================================
//              USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------

namespace TEST_CASE_USAGE {

// Let us suppose that we wish to handle objects that will be passed to a
// comparison function expecting references to the objects.  Let us suppose
// further that these objects are large enough that we would not wish to move
// them around bodily as they are sorted. (Please note that plausible examples
// of uses for this component are limited in C++98, particularly so when given
// that the example may not depend on other library components.)
//
/// First, let us define the large object:

struct Canary {
    int d_values[1000];

    explicit Canary(int values);
};

Canary::Canary(int values)
{
     for (int i = 0; i < 1000; ++i) {
         d_values[i] = values;
     }
}

// Next, the comparison function:

bool operator<(Canary const& a, Canary const& b)
{
   return a.d_values[0] < b.d_values[0];
}

// Finally, a generic function to sort two items:

template <typename T>
void sortTwoItems(T& a, T& b)
{
    if (b < a) {
        T tmp(a);
        a = b;
        b = tmp;
    }
}

}  // close namespace TEST_CASE_USAGE

// ============================================================================
//                                 MAIN PROGRAM
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
        // USAGE EXAMPLE
        //   Extracted from bslstl_referencewrapper.h
        //
        // Concerns:
        //  1 The example in the header builds and runs as claimed.
        //
        // Plan:
        //  1 Reformat the comment to runnable code, as prescribed.
        //
        // Testing:
        //   USAGE EXAMPLE
        //
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nUsage example"
                   "\n=============\n");
        }

        using namespace TEST_CASE_USAGE;

            // We can call 'sortTwoItems()' on wrappers representing Canary
            // objects without need to move actual, large 'Canary' objects
            // about. In the call to 'sortTwoItems()', below, the 'operator='
            // used in it is that of 'bsl::reference_wrapper<Canary>', but the
            // 'operator<' used is the one declared for 'Canary&' objects.  All
            // of the conversions needed are applied implicitly.

        Canary two(2);
        Canary one(1);
        bsl::reference_wrapper<Canary> canaryA = bsl::ref(two);
        bsl::reference_wrapper<Canary> canaryB = bsl::ref(one);
        sortTwoItems(canaryA, canaryB);

        ASSERT(&canaryA.get() == &one);
        ASSERT(&canaryB.get() == &two);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TESTS
        //   Create objects each possible way, verify they have the right
        //   contents and can be used in the standard ways.
        //
        // Concerns:
        //  1 Template functions defined are only checked by the compiler when
        //    called, so all templates must be instantiated here.
        //
        //  2 The various functions' argument must be copied into the object
        //    correctly.
        //
        //  3 The accessors must reproduce the various functions' argument
        //    values.
        //
        // Plan:
        //  1 Define a dummy type
        //  2 Wrap the dummy type using each available method
        //  3 Use the wrappers' explicit and implicit accessors
        //
        // Testing:
        //   reference_wrapper(T&);
        //   reference_wrapper(consts reference_wrapper<T>&)
        //   reference_wrapper<T>& operator=(reference_wrapper<T>&) const;
        //   operator T&() const;
        //   T& get() const;
        //   reference_wrapper<T> cref(T&)
        //   reference_wrapper<T> ref(T&)
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBasic tests"
                   "\n===========\n");
        }
        dummy a;
        const dummy b = {};

        bsl::reference_wrapper<dummy> rwa(a);
        bsl::reference_wrapper<const dummy> rwca(a);
        bsl::reference_wrapper<const dummy> rwcb(b);

        bsl::reference_wrapper<dummy> copyrwa(rwa);
        bsl::reference_wrapper<const dummy> copyrwca(rwca);
        bsl::reference_wrapper<const dummy> copyrwcb(rwcb);

        copyrwa = a;
        copyrwca = a;
        copyrwcb = b;

        copyrwa = rwa;
        copyrwca = rwca;
        copyrwcb = rwcb;

        dummy& rax = rwa;
        const dummy& rcax = rwca;
        const dummy& rcbx = rwcb;

        dummy& ray = bsl::ref(a);
        const dummy& rcay = bsl::cref(a);
        const dummy& rcby = bsl::cref(b);

        bsl::reference_wrapper<dummy> copyrwaz(bsl::ref(rwa));
        bsl::reference_wrapper<const dummy> copyrwcaz(bsl::ref(rwca));
        bsl::reference_wrapper<const dummy> copyrwcbz(bsl::ref(rwcb));

        use(rwa);
        const_use(rwca);
        const_use(rwcb);

        ASSERT(&copyrwa.get() == &a);
        ASSERT(&copyrwca.get() == &a);
        ASSERT(&copyrwcb.get() == &b);
        ASSERT(&copyrwaz.get() == &a);
        ASSERT(&copyrwcaz.get() == &a);
        ASSERT(&copyrwcbz.get() == &b);

        (void)rax; (void)rcax; (void)rcbx; (void)ray; (void)rcay; (void)rcby;

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
