// bslstl_referencewrapper.t.cpp                                      -*-C++-*-
#include <bslstl_referencewrapper.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides no functionality per se.  Its sole purpose
// is capture a type in an assignable, copyable object. Therefore, it suffices
// to verify, via appropriate declarations, that the names required are
// defined, and usable as intended
//
// ----------------------------------------------------------------------------
// class reference_wrapper<TYPENAME> (reference_wrapper)
// ============================================================================
//
// ----------------------------------------------------------------------------
// [ 1] reference_wrapper(TYPENAME&);
// [ 1] reference_wrapper(consts reference_wrapper<TYPENAME>&)
// [ 1] reference_wrapper<TYPENAME>& operator=(
// [ 1]                                    reference_wrapper<TYPENAME>&) const;
// [ 1] operator TYPENAME&() const;
// [ 1] TYPENAME& get() const;
// ----------------------------------------------------------------------------

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
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Create objects each possible way, verify they have the right
        //   contents and can be used in the standard ways.
        //
        // Plan:
        //   Define a dummy type, wrap it, and use the wrapper.
        //
        // Testing:
        //   reference_wrapper(TYPENAME&);
        //   reference_wrapper(consts reference_wrapper<TYPENAME>&)
        //   reference_wrapper<TYPENAME>& operator=(
        //                                 reference_wrapper<TYPENAME>&) const;
        //   operator TYPENAME&() const;
        //   TYPENAME& get() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

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
