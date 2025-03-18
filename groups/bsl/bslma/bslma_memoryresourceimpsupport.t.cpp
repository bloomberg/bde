// bslma_memoryresourceimpsupport.t.cpp                               -*-C++-*-

#include <bslma_memoryresource.h>
#include <bslma_memoryresourceimpsupport.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstdio>   // `printf`
#include <cstdlib>  // `atoi`
#include <new>      // `align_val_t` and aligned `new`

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using std::printf;
using std::fprintf;
using std::fflush;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// This test driver simply has one test case for each independent utility
// provided by the component.
// ----------------------------------------------------------------------------
// [ 2] void *singularPointer();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLES
// [-1] CONCERN: `singularPointer()` points to garbage
// [-1] CONCERN: `singularPointer()` points to non-writable memory
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

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose             = 0;
int veryVerbose         = 0;
int veryVeryVerbose     = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma::MemoryResourceImpSupport Util;

const std::ptrdiff_t k_MAX_ALIGNMENT = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Example 1: Write a `memory_resource` using `singularPointer`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we derive a `NewDeleteResource` class, derived from
// `bsl::memory_resource`, that allocates and deallocate using `operator new`
// and `operator delete`, respectively, and uses `singularPointer()` as a
// special value when allocating and allocating zero bytes.
//
// First, we define the class interface, which implements the protected virtual
// functions `do_allocate`, `do_deallocate`, and `do_is_equal`:
// ```
//  #include <bslma_memoryresource.h>
//  #include <bslma_memoryresourceimpsupport.h>
//  #include <bsls_alignmentutil.h>
//  #include <bsls_assert.h>

    /// Memory resource that allocates using `operator new`.
    class NewDeleteResource : public bsl::memory_resource {
      protected:
        // PROTECTED MANIPULATORS

        /// Return the specified `size` bytes with the specified `alignment`
        /// allocated from the global heap using `operator new`.  The behavior
        /// is undefined unless the specified `alignment` is less than or equal
        /// to the maximum platform alignment.
        void *do_allocate(std::size_t size, std::size_t alignment)
                                                         BSLS_KEYWORD_OVERRIDE;

        /// Deallocate the memory block specified by `p` having the specified
        /// `size` and `alignment` from the global heap using `operator
        /// delete`.  The behavior is undefined unless `p` was returned from a
        /// previous call to `allocate`, using the same `size` and `alignment`.
        void do_deallocate(void *p, std::size_t size, std::size_t alignment)
                                                         BSLS_KEYWORD_OVERRIDE;

        /// Return `true` if `x` is a `NewDeleteResource` and `false`
        /// otherwise.
        bool do_is_equal(const bsl::memory_resource& x) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
    };
// ```
// Next, we implement the `do_allocate` method, which forwards most requests to
// `operator new`. Section [basic.stc.dynamic.allocation] of the C++ standard,
// however states that the return value of an allocation function when the
// requested size is zero is a *non-null* pointer to a suitably aligned block
// of storage, so we use `singularPointer` to provide the address in such
// circumstances:
// ```
//  #include <new>      // `align_val_t` and aligned `new`

    void *NewDeleteResource::do_allocate(std::size_t size,
                                         std::size_t alignment)
    {
        if (0 == size) {
            return bslma::MemoryResourceImpSupport::singularPointer();
        }

    #ifdef __cpp_aligned_new
        return ::operator new(size, std::align_val_t(alignment));
    #else
        (void) alignment;
        BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        return ::operator new(size);
    #endif
    }
// ```
// Next, we implement the `do_deallocate` method, which forwards most requests
// to `operator delete`, but does nothing if the incoming pointer was the
// result of a zero-sized allocation:
// ```
    void NewDeleteResource::do_deallocate(void        *p,
                                          std::size_t  size,
                                          std::size_t  alignment)
    {
        (void) size;  // Not used in OPT build

        if (bslma::MemoryResourceImpSupport::singularPointer() == p) {
            BSLS_ASSERT(0 == size);
            return;
        }

        BSLS_ASSERT(0 < size);
    #ifdef __cpp_aligned_new
        ::operator delete(p, std::align_val_t(alignment));
    #else
        (void) (size, alignment);
        BSLS_ASSERT(alignment <= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        ::operator delete(p);
    #endif
    }
// ```
// Next, we complete the implementation with `do_is_equal`.  All instances of
// `NewDeleteResource` compare equal, so we need only check that the argument
// is a `NewDeleteResource`.  As a short-cut, we check if the address of `b` is
// the same as `this`, to quickly catch the common case that they are both
// pointers to the singleton object:
// ```
    bool NewDeleteResource::do_is_equal(const bsl::memory_resource& b) const
                                                          BSLS_KEYWORD_NOEXCEPT
    {
        return this == &b || 0 != dynamic_cast<const NewDeleteResource *>(&b);
    }
// ```
// Now, when we call `allocate`, we observe that each non-zero allocation
// yields a different pointer, whereas all of the zero allocations yield the
// same address.
// ```
    void usageExample1()
    {
        NewDeleteResource r;
        void *p1 = r.allocate(1, 1);
        void *p2 = r.allocate(8);
        ASSERT(p1 != p2);

        void *p3 = r.allocate(0, 1);
        void *p4 = r.allocate(0, 4);
        ASSERT(p3 != p1);
        ASSERT(p3 != p2);
        ASSERT(p3 == p4);
// ```
// Finally, when we deallocate memory, we would expect nothing to happen when
// deallocating the zero-sized blocks `p3` and `p4`:
// ```
        r.deallocate(p1, 1, 1);
        r.deallocate(p2, 8);
        r.deallocate(p3, 0, 1);
        r.deallocate(p4, 0, 4);
    }
// ```

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. That the usage examples shown in the component-level
        //    documentation compile and run as described.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, changing
        //    `assert` to `ASSERT` and execute them.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING `singularPointer`
        //
        // Concerns:
        // 1. A call to `Util::singularPointer` returns a non-const, non-null
        //    pointer.
        //
        // 2. The same pointer is returned every time.
        //
        // 3. The address stored in the pointer has (at least) the maximum
        //    platform alignment.
        //
        // 4. In C++11 and later, `Util::singularPointer()` is a compile-time
        //    constant.
        //
        // Plan:
        // 1. Call `Util::singularPointer` and store the result in `void *`
        //    variable.  Verify that the resulting pointer is not null.  (C-1)
        //
        // 2. Call `Util::singularPointer` a severl more times and verify that
        //    the returned value is always the same as the first one.  (C-2)
        //
        // 3. Verify that the pointer returned in step 1 is aligned
        //    appropriately.  (C-3)
        //
        // 4. Initialize a `constexpr` pointer to the return value of calling
        //    `singlePointer` and verify that it compiles succesfully.  (C-4)
        //
        // Testing:
        //   void *singularPointer();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `singularPointer`"
                            "\n=========================\n");

        void *p = Util::singularPointer();
        ASSERT(0 != p);

        for (int i = 0; i < 5; ++i) {
            ASSERT(Util::singularPointer() == p);
        }

        // Test alignment
        ASSERT(bsls::AlignmentUtil::isAligned(p, k_MAX_ALIGNMENT));

        // Test that it is evaluated at compile time
        BSLS_KEYWORD_CONSTEXPR static void *const sp = Util::singularPointer();
        ASSERT(sp == p);

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Test that `singularPointer` returns a non-null pointer
        void *p1 = Util::singularPointer();
        ASSERT(0 != p1);

        // Test that `singularPointer` returns the same value each time.
        void *p2 = Util::singularPointer();
        ASSERT(0 != p2);
        ASSERT(p1 == p2);

      } break;

      case -1: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTS FOR `singularPointer`
        //
        // Concerns:
        // 1. Dereferencing the pointer returned by `Util::singlularPointer`
        //    yields garbage on read.
        //
        // 2. Dereferencing the pointer returned by `Util::singlularPointer`
        //    results in a segmentation violation on write.
        //
        // Plan
        // 1. Read an integral value through the (suitably cast) result of
        //    `Util::singularPointer`.  Verify manually that there is no useful
        //    data there (ideally not zeros).  2 Cast
        //
        // 2. Write a byte through the (suitably cast) result of
        //    `Util::singularPointer`.  Verify that a segmentation violation
        //    results.
        //
        // Testing:
        //   CONCERN: `singularPointer()` points to garbage
        //   CONCERN: `singularPointer()` points to non-writable memory
        // --------------------------------------------------------------------

        if (verbose) printf("\nNEGATIVE TESTS FOR `singularPointer`"
                            "\n====================================\n");

        // Print the first two integers at the return address
        const unsigned int* p =
            static_cast<unsigned int*>(Util::singularPointer());
        printf("Data at `singularPointer()`: 0x%x 0x%x\n", p[0], p[1]);

        if (verbose) printf("Write to memory at `singularPointer()`\n");
        fflush(stdout);
        *static_cast<char *>(Util::singularPointer()) = 1;
        if (verbose) printf("Write succeeded\n");
        ASSERT(false && "Expected SEGV");

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
// Copyright 2024 Bloomberg Finance L.P.
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
