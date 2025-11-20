// bslh_hashthreadid.t.cpp                                            -*-C++-*-
#include <bslh_hashthreadid.h>

#include <bslh_defaulthashalgorithm.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // `memcmp`, `memcpy`
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# include <thread>
#endif

using namespace BloombergLP;
using namespace bslh;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an overload of the `hashAppend` free
// function.  We must verify that it properly passes data into the hashing
// algorithm it is given, such that two equal arguments result in equal values
// being passed into the hash algorithm.
// ----------------------------------------------------------------------------
// FREE FUNCTIONS
// [ 2] void hashAppend(t_HASH_ALG&, const std::thread::id&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
namespace usage_example {
// ============================================================================
//                             USAGE EXAMPLE                               
// ----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Hashing a Thread ID
/// - - - - - - - - - - - - - - -
// Suppose we have a struct with an `int` member and a `std::thread::id`
// member:
// ```
struct S {
    int             d_x;
    std::thread::id d_tid;
};
// ```
// To implement `hashAppend` for this struct, we would feed its first member,
// then its second, to `hashAppend`.  This is made possible by the fact that
// this component provides an overload that accepts `std::thread::id`.
// ```
template <class t_HASH_ALG>
void hashAppend(t_HASH_ALG& algorithm, const S& s)
{
    using bslh::hashAppend;
    hashAppend(algorithm, s.d_x);
    hashAppend(algorithm, s.d_tid);
}
// ```
// We can now use our new `hashAppend` overload:
// ```
void usageExample()
{
    using bslh::hashAppend;

    bslh::DefaultHashAlgorithm h1;
    bslh::DefaultHashAlgorithm h2;

    S s1;
    s1.d_x   = 1;
    s1.d_tid = std::this_thread::get_id();
    hashAppend(h1, s1);

    S s2 = s1;
    hashAppend(h2, s2);

    ASSERT(h1.computeHash() == h2.computeHash());
}
// ```
}  // close namespace usage_example
#endif  // C++11

// ============================================================================
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
// ----------------------------------------------------------------------------

/// This class implements a mock hashing algorithm that provides a way to
/// accumulate and then examine data that is being passed into hashing
/// algorithms by `hashAppend`.
class MockAccumulatingHashingAlgorithm {

    void   *d_data_p;  // Data we were asked to hash
    size_t  d_length;  // Length of the data we were asked to hash

  public:
    /// Create an object of this type.
    MockAccumulatingHashingAlgorithm()
    : d_data_p(0)
    , d_length(0)
    {
    }

    /// Destroy this object
    ~MockAccumulatingHashingAlgorithm()
    {
        free(d_data_p);
    }

    /// Append the data of the specified `length` at `voidPtr` for later
    /// inspection.
    void operator()(const void *voidPtr, size_t length)
    {
        d_data_p = realloc(d_data_p, d_length += length);
        memcpy(getData() + d_length - length, voidPtr, length);
    }

    /// Return a pointer to the stored data.
    char *getData()
    {
        return static_cast<char *>(d_data_p);
    }

    /// Return the length of the stored data.
    size_t getLength() const
    {
        return d_length;
    }
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value and
/// `false` otherwise.  Two objects have the same value if they both have
/// the same length and if they both point to data that compares equal.
/// Note that for simplicity of implementation the arguments are
/// (unconventionally) non-`const`.
bool operator==(MockAccumulatingHashingAlgorithm& lhs,
                MockAccumulatingHashingAlgorithm& rhs)
{
    if (lhs.getLength() != rhs.getLength()) {
        return false;                                                 // RETURN
    }

    return 0 == memcmp(lhs.getData(), rhs.getData(), rhs.getLength());
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2; (void)verbose;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4; (void)veryVeryVerbose;
    bool veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        usage_example::usageExample();
#else
        puts("Skipping usage example in C++03");
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `hashAppend`
        //
        // Concerns:
        // 1. The `hashAppend` function ("the function") uses the specified
        //    algorithm.
        //
        // 2. For a given value of `std::thread::id`, the function always
        //    passes the same value into the algorithm.
        //
        // 3. For different values of `std::thread::id`, the function does not
        //    always pass the same value into the algorithm.
        //
        // Plan:
        // 1. Use a locally defined algorithm class,
        //    `MockAccumulatingHashingAlgorithm`, to show that `hashAppend`
        //    uses the supplied algorithm object.  The
        //    `MockAccumulatingHashingAlgorithm` functor stores a
        //    concatenation of all the inputs supplied.  This allows one to
        //    compare the results of separate invocations of the algorithm.
        //
        // 2. Create 3 new threads, resulting in a total of 5 different values
        //    of `std::thread::id` when including the main thread and the null
        //    (default-constructed value).  Create two instances of
        //    `MockAccumulatingHashingAlgorithm` and into each one, pass all 5
        //    thread IDs in the same order.  Verify that the two instances are
        //    equal after this has been done, and are unequal to an instance of
        //    the algorithm on which `hashAppend` has not been called.  (C-1,2)
        //
        // 3. Create 5 instances of `MockAccumulatingHashingAlgorithm` and pass
        //    each of the thread ID values described in P-2 into a different
        //    instance.  Verify that the 5 algorithms are not all equal.
        //    (Because `hashAppend` is implemented using `std::hash` for
        //    `std::thread::id`, some hash collisions are possible; the test
        //    passes as long as all 5 thread IDs don't hash to the same value.)
        //    (C-3)
        //
        // Testing:
        //   void hashAppend(t_HASH_ALG&, const std::thread::id&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `hashAppend`"
                            "\n====================\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        using bslh::hashAppend;

        const int k_EXTRA_THREADS = 3;

        std::thread extraThreads[k_EXTRA_THREADS];

        for (int i = 0; i < k_EXTRA_THREADS; ++i) {
            // The extra threads terminate immediately, but their thread IDs
            // aren't recycled until they're joined.
            extraThreads[i] = std::thread([]{});
        }

        const int k_NUM_THREAD_IDS = 2 + k_EXTRA_THREADS;

        std::thread::id threadIds[k_NUM_THREAD_IDS];

        threadIds[1] = std::this_thread::get_id();
        for (int i = 0; i < k_EXTRA_THREADS; ++i) {
            threadIds[i + 2] = extraThreads[i].get_id();
        }

        {
            MockAccumulatingHashingAlgorithm unused;
            MockAccumulatingHashingAlgorithm hasher1;
            MockAccumulatingHashingAlgorithm hasher2;

            for (int i = 0; i < k_NUM_THREAD_IDS; ++i) {
                hashAppend(hasher1, threadIds[i]);
                hashAppend(hasher2, threadIds[i]);
            }
            
            ASSERT(hasher1 == hasher2);
            ASSERT(!(unused == hasher1));
        }

        {
            MockAccumulatingHashingAlgorithm hashers[k_NUM_THREAD_IDS];

            for (int i = 0; i < k_NUM_THREAD_IDS; ++i) {
                hashAppend(hashers[i], threadIds[i]);
            }

            bool allSame = true;
            for (int i = 1; i < k_NUM_THREAD_IDS; ++i) {
                allSame &= (hashers[0] == hashers[i]);
            }
            ASSERT(!allSame);
        }

        for (int i = 0; i < k_EXTRA_THREADS; ++i) {
            extraThreads[i].join();
        }
#else
        puts("Skipping test case 2 in C++03");
#endif
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
        // 1. Create an instance of `bslh::Hash<>`.  (C-1)
        //
        // 2. Verify that the same hash is produced when a thread ID is hashed
        //    twice.  (Note that we do not test that different hashes are
        //    produced when different thread IDs are hashed, because we have no
        //    way to set a non-null thread ID to a "fixed" value; therefore,
        //    whether such a test would pass depends on whether the thread IDs
        //    assigned by the system happen to avoid a collision.)  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        bslh::Hash<> hasher;

        std::thread::id myID = std::this_thread::get_id();

        ASSERT(hasher(myID) == hasher(myID));
#else
        puts("Skipping breathing test in C++03");
        ASSERT(true);
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
// Copyright 2025 Bloomberg Finance L.P.
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
