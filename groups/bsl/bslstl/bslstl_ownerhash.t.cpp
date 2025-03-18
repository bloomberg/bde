// bslstl_ownerhash.t.cpp                                             -*-C++-*-
#include <bslstl_ownerhash.h>

#include <bslstl_array.h>
#include <bslstl_sharedptr.h>
#include <bslstl_unorderedmap.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>

#include <stdio.h>       // printf
#include <stdlib.h>      // atoi

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <type_traits>   // std::is_trivially_copyable
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under test is an empty struct with a single typedef and templated
// member functions.  We confirm that it is an empty struct, that the typedef
// exists, and then test the member functions.
// ----------------------------------------------------------------------------
// bsl::owner_hash
//----------------
// [ 1] size_t owner_hash::operator()(const shared_ptr<TYPE>&) const
// [ 1] size_t owner_hash::operator()(const weak_ptr<TYPE>&) const
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 2] TESTING TYPEDEF
// [ 3] QoI: Support for empty base optimization

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

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                              // ===============
                              // class SimpleRep
                              // ===============

class SimpleRep : public bslma::SharedPtrRep {
    // DATA
    void *d_ptr_p;

  public:
    // CREATORS

    /// Construct a `SimpleRep` from the specified `ptr`
    explicit SimpleRep(void *ptr);

    // MANIPULATORS

    /// Does nothing
    void disposeObject() BSLS_KEYWORD_OVERRIDE;

    /// Does nothing
    void disposeRep() BSLS_KEYWORD_OVERRIDE;

    /// Return NULL
    void *getDeleter(const std::type_info&) BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return a copy of the pointer passed to the constructor.
    void *originalPtr() const BSLS_KEYWORD_OVERRIDE;
};

                                 // ==========
                                 // class Base
                                 // ==========

class Base {
  public:
    // CREATORS
    virtual ~Base();// = default;

    // MANIPULATORS

    /// A pure virtual function to be overridden
    virtual int *get() = 0;
};

                               // =============
                               // class Derived
                               // =============

class Derived : public Base {
    int d_i;

  public:
    // CREATORS

    /// Construct a `Derived` from the specified `i`
    explicit Derived(int i);

    // MANIPULATORS

    /// return a pointer to the member variable `d_i`
    int *get() BSLS_KEYWORD_OVERRIDE;
};

                              // ---------------
                              // class SimpleRep
                              // ---------------

// CREATORS
SimpleRep::SimpleRep(void *ptr)
: d_ptr_p(ptr)
{
}

// MANIPULATORS
void SimpleRep::disposeObject()
{
}

void SimpleRep::disposeRep()
{
}

void *SimpleRep::getDeleter(const std::type_info&)
{
    return 0;
}

void *SimpleRep::originalPtr() const
{
    return d_ptr_p;
}

                                 // ----------
                                 // class Base
                                 // ----------

// CREATORS
Base::~Base()
{
}

                               // -------------
                               // class Derived
                               // -------------

// CREATORS
Derived::Derived(int i)
: d_i(i)
{
}

// MANIPULATORS
int *Derived::get()
{
    return &d_i;
}

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1: Basic Use of `owner_hash`
/// - - - - - - - - - - - - - - - - - -
// Suppose we need an unordered map accepting shared pointers as keys.  We also
// expect that this container will be accessible from multiple threads and some
// of them will store weak versions of smart pointers to break reference
// cycles.
//
// First, we define an owner-based equality predicate, that is required by the
// `bsl::unordered_map` along with this owner-based hash.
// ```
    struct TestOwnerEqual {
        // TYPES
        typedef void is_transparent;

        /// For the specified `lhs` and `rhs`, return the result of
        /// `lhs.owner_equal(rhs)`
        template <class T1, class T2>
        bool operator()(const bsl::shared_ptr<T1>& lhs,
                        const bsl::shared_ptr<T2>& rhs) const
        {
            return lhs.owner_equal(rhs);
        }

        /// For the specified `lhs` and `rhs`, return the result of
        /// `lhs.owner_equal(rhs)`
        template <class T1, class T2>
        bool operator()(const bsl::shared_ptr<T1>& lhs,
                        const bsl::weak_ptr<T2>&   rhs) const
        {
            return lhs.owner_equal(rhs);
        }

        /// For the specified `lhs` and `rhs`, return the result of
        /// `lhs.owner_equal(rhs)`
        template <class T1, class T2>
        bool operator()(const bsl::weak_ptr<T1>&   lhs,
                        const bsl::shared_ptr<T2>& rhs) const
        {
            return lhs.owner_equal(rhs);
        }

        /// For the specified `lhs` and `rhs`, return the result of
        /// `lhs.owner_equal(rhs)`
        template <class T1, class T2>
        bool operator()(const bsl::weak_ptr<T1>& lhs,
                        const bsl::weak_ptr<T2>& rhs) const
        {
            return lhs.owner_equal(rhs);
        }
    };
// ```
// Note that this struct is defined only to avoid cycle dependencies between
// BDE components.  In real code for these purposes it is recommended to use
// `bsl::owner_equal`.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

BSLMF_ASSERT(bsl::is_trivially_default_constructible<bsl::owner_hash>::value);
BSLMF_ASSERT(bsl::is_trivially_copyable<bsl::owner_hash>::value);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
BSLMF_ASSERT(std::is_trivially_default_constructible<bsl::owner_hash>::value);
BSLMF_ASSERT(std::is_trivially_copyable<bsl::owner_hash>::value);
#endif

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static intialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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
// Then, we create a container and populate it:
// ```
        typedef bsl::unordered_map<
            bsl::shared_ptr<int>,
            int,
            bsl::owner_hash,
            TestOwnerEqual> Map;

        Map                  container;

        bsl::shared_ptr<int> sharedPtr1 = bsl::make_shared<int>(1);
        bsl::shared_ptr<int> sharedPtr2 = bsl::make_shared<int>(2);
        bsl::weak_ptr<int>   weakPtr1(sharedPtr1);

        container[sharedPtr1] = 1;
        container[sharedPtr2] = 2;
// ```
// Now, we make sure, that shared pointers can be used to perform lookup:
// ```
        Map::const_iterator iter = container.find(sharedPtr1);
        ASSERT(container.end() != iter        );
        ASSERT(1               == iter->second);

        iter = container.find(sharedPtr2);
        ASSERT(container.end() != iter);
        ASSERT(2               == iter->second);
// ```
// Finally, we simulate the situation of accessing the container from another
// thread and perform lookup using weak pointers:
// ```
        iter = container.find(weakPtr1);
        ASSERT(container.end() != iter        );
        ASSERT(1               == iter->second);

        bsl::weak_ptr<int> weakPtr3(bsl::make_shared<int>(3));
        iter = container.find(weakPtr3);
        ASSERT(container.end() == iter);
// ```
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING QOI: `owner_hash` IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        // 1. Class `bsl::owner_hash` does not increase the size of an object
        //    when used as a base class.
        //
        // 2. Object of `bsl::owner_hash` class increases size of an object
        //    when used as a class member.
        //
        // Plan:
        // 1. Define two identical non-empty classes with no padding, but
        //    derive one of them from `bsl::owner_hash`, then assert that both
        //    classes have the same size. (C-1)
        //
        // 2. Create a non-empty class with an `bsl::owner_hash` additional
        //    member, assert that class size is larger than sum of other data
        //    member's sizes. (C-2)
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QOI: `owner_hash` IS AN EMPTY TYPE"
                            "\n==========================================\n");

        struct TwoInts {
            int d_a;
            int d_b;
        };

        struct DerivedInts : bsl::owner_hash {
            int d_a;
            int d_b;
        };

        struct IntWithMember {
            bsl::owner_hash d_dummy;
            int             d_a;
        };

        ASSERT(sizeof(TwoInts) == sizeof(DerivedInts));

        ASSERT(sizeof(int)     <  sizeof(IntWithMember));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEF
        //   Comparator's transparency is determined by the presence of the
        //   `is_transparent` type.  We need to verify that the class offers
        //   the required typedef.
        //
        // Concerns:
        // 1. The type `is_transparent` is defined in `bsl::owner_hash`,
        //    publicly accessible and an alias for `void`.
        //
        // Plan:
        // 1. ASSERT the typedef aliases the correct type using
        //    `bsl::is_same`. (C-1)
        //
        // Testing:
        //  TESTING TYPEDEF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPEDEF"
                            "\n===============\n");

        ASSERT((bsl::is_same<void,
                             bsl::owner_hash::is_transparent>::value));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING `owner_hash` FUNCTOR
        //   `owner_hash` is an empty POD type with implicitly defined special
        //   member functions and a `const`-qualified function call operator.
        //
        // Concerns:
        //  1. `owner_hash` can be value-initialized.
        //  2. `owner_hash` can be copy-initialized.
        //  3. `owner_hash` can be copy-assigned.
        //  4. `owner_hash` has overloads for the function call operator that
        //     can take a `shared_ptr<T>` object by reference, or a
        //     `weak_ptr<T>` by reference.
        //  5. The overloaded function call operator for all `owner_hash`
        //     templates returns the hash of the `rep` held by the argument.
        //  6. QoI: No operations on `owner_hash` objects allocate any memory.
        //
        // Plan:
        //  1. Create two shared pointer representation objects, with a known
        //   relationship between their addresses.  Then create shared and weak
        //   ptr objects from these representations, and confirm the correct
        //   runtime behavior when invoking the function call operator of the
        //   `owner_hash` functor.
        //
        // Testing:
        //  size_t owner_hash::operator()(const shared_ptr<TYPE>&) const
        //  size_t owner_hash::operator()(const weak_ptr<TYPE>&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `owner_hash` FUNCTOR"
                            "\n============================\n");

        {
            Derived   d1(1),     d2(1);
            SimpleRep rep1(&d1), rep2(&d2);

            // Shared Derived
            bsl::array<bsl::shared_ptr<Derived>, 3> SD = {
                bsl::shared_ptr<Derived>(),
                bsl::shared_ptr<Derived>(&d1, &rep1),
                bsl::shared_ptr<Derived>(&d2, &rep2)
            };

            // Shared Base
            bsl::array<bsl::shared_ptr<Base>, 3> SB = {
                bsl::shared_ptr<Base>(),
                SD[1],
                SD[2]
            };

            // Shared Alias
            bsl::array<bsl::shared_ptr<int>, 3> SA = {
                bsl::shared_ptr<int>(),
                bsl::shared_ptr<int>(SD[1], SD[1]->get()),
                bsl::shared_ptr<int>(SD[2], SD[2]->get())
            };

            // Weak Derived
            bsl::array<bsl::weak_ptr<Derived>, 3> WD = {
                bsl::weak_ptr<Derived>(),
                SD[1],
                SD[2]
            };

            // Weak Base
            bsl::array<bsl::weak_ptr<Base>, 3> WB = {
                bsl::weak_ptr<Base>(),
                SD[1],
                SD[2]
            };

            // Weak Alias
            bsl::array<bsl::weak_ptr<int>, 3> WA = {
                bsl::weak_ptr<int>(),
                SA[1],
                SA[2]
            };

            bsl::owner_hash OH;

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    // Derived vs Derived
                    ASSERTV(i, j, (OH(SD[i]) == OH(SD[j])) == (i == j));
                    ASSERTV(i, j, (OH(SD[i]) == OH(WD[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(SD[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(WD[j])) == (i == j));

                    // Derived vs Base
                    ASSERTV(i, j, (OH(SD[i]) == OH(SB[j])) == (i == j));
                    ASSERTV(i, j, (OH(SD[i]) == OH(WB[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(SB[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(WB[j])) == (i == j));

                    // Derived vs Alias
                    ASSERTV(i, j, (OH(SD[i]) == OH(SA[j])) == (i == j));
                    ASSERTV(i, j, (OH(SD[i]) == OH(WA[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(SA[j])) == (i == j));
                    ASSERTV(i, j, (OH(WD[i]) == OH(WA[j])) == (i == j));
                }
            }

            // CONCERN: No operations in this test case should allocate memory.

            ASSERTV(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
