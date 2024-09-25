// bslma_stdallocator.t.cpp                                           -*-C++-*-

#include <bslma_stdallocator.h>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component has been renamed to `bslma_bslallocator`.  The only test is
// to ensure that `#include`ing this component header will make the
// `bsl::allocator` and `bsl::allocator_traits<bsl::allocator<T>>` class
// templates available as though `bslma_bslallocator.h` were `#included`
// directly.
// ----------------------------------------------------------------------------
// [ 1] bsl::allocator<T>
// [ 1] bsl::allocator_traits<bsl::allocator<T>>
// ----------------------------------------------------------------------------

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>  // std::atoi

using std::printf;

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? std::atoi(argv[1]) : 0;
    int  testStatus = 0;
    bool verbose             = argc > 2;
    // bool veryVerbose         = argc > 3;
    // bool veryVeryVerbose     = argc > 4;
    // bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // TEST CLASS TEMPLATES
        //   This case exercises verifies that the class templates defined in
        //   the `bslma_bslallocator` component are available to clients of
        //   this component.  This is a compile-only test;
        //
        // Concerns:
        // 1. The `bsl::allocator` template can be instantiated and an object
        //    created.
        // 2. The `bsl::allocator_traits<bsl::allocator<T>>` template can be
        //    instantiated for an arbitrary `T`.
        //
        // Plan:
        // 1. Define an object of type `bsl::allocator<int>`.  Verify that the
        //    definition compiles.
        // 2. Define an object of type
        //    `bsl::allocator_traits<bsl::allocator<short>>::allocator_type`.
        //    Verify that the definition compiles.
        //
        // Testing:
        //      TEST CLASS TEMPLATES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEST CLASS TEMPLATES"
                            "\n====================\n");

        // If this compiles, test passes.
        bsl::allocator<int>                                           a1;
        bsl::allocator_traits<bsl::allocator<short> >::allocator_type a2;

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
// Copyright 2023 Bloomberg Finance L.P.
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
