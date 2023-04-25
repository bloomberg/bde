// bslma_allocatorutil.t.cpp                                          -*-C++-*-

#include <bslma_allocatorutil.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_keyword.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#endif

using std::printf;
using std::fprintf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// [ 2] bslma::Allocator *adapt(const bsl::allocator<TYPE>&);
// [ 2] ALLOCATOR         adapt(const ALLOCATOR&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLES
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

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslma::AllocatorUtil Util;

template <class TYPE>
class DerivedAllocator : public bsl::allocator<TYPE>
{
    typedef bsl::allocator<TYPE> Base;

  public:
    DerivedAllocator(bslma::Allocator *a = 0) : Base(a) { }

    template <class T2>
    DerivedAllocator(const DerivedAllocator<T2>& other)
        : Base(other.mechanism()) { }
};

template <class TYPE>
class StlAllocator
{
    // A class that meets the minimum requirements for a C++17 allocator.

    bslma::Allocator *d_mechanism;

  public:
    typedef TYPE value_type;

    StlAllocator(bslma::Allocator *a = 0) : d_mechanism(a) { }

    template <class T2>
    StlAllocator(const StlAllocator<T2>& other)
        : d_mechanism(other.mechanism()) { }

    TYPE *allocate(std::size_t);
    void deallocate(TYPE *, std::size_t);
        // No implementations neeeded for this test

    bslma::Allocator *mechanism() const { return d_mechanism; }
};

template <class T1, class T2>
bool operator==(const StlAllocator<T1>& lhs, const StlAllocator<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
bool operator!=(const StlAllocator<T1>& lhs, const StlAllocator<T2>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

template <class EXP_TYPE, class ARG_TYPE>
bool matchType(const ARG_TYPE&)
    // Return 'true' if 'ARG_TYPE', deduced from the argument, has exactly the
    // same type as the 'EXP_TYPE' exlicitly-specified template parameter,
    // otherwise 'false'.
{
    return bsl::is_same<EXP_TYPE, ARG_TYPE>::value;
}

class LegacyAAClass {
    // Legacy-AA class.

    bslma::Allocator *d_allocator_p;
    // ...

  public:
    explicit LegacyAAClass(bslma::Allocator *basicAllocator = 0)
        : d_allocator_p(basicAllocator) { /* ... */ }

    bslma::Allocator *allocator() const { return d_allocator_p; }
};

class BslAAClass {
    // Bsl-AA class.

    bsl::allocator<int> d_allocator;
    // ...

  public:
    typedef bsl::allocator<int> allocator_type;

    explicit BslAAClass(const allocator_type& allocator = allocator_type())
        : d_allocator(allocator) { /* ... */ }

    allocator_type get_allocator() const { return d_allocator; }
};

class StlAAClass {
    // Stl-AA class.

    StlAllocator<int> d_allocator;
    // ...

  public:
    typedef StlAllocator<int> allocator_type;

    explicit StlAAClass(const allocator_type& allocator = allocator_type())
        : d_allocator(allocator) { /* ... */ }

    allocator_type get_allocator() const { return d_allocator; }
};



// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: Future-proofing Member Construction
///- - - - - - - - - - - - - - - - - - - - - - -
// This example shows how we construct an AA member variable, using
// 'bslma::AllocatorUtil::adapt' so that it is both self-documenting and robust
// in case the member type is modernized from *legacy-AA* (using
// 'bslma::Allocator *' directly in its interface) to *bsl-AA* (using
// 'bsl::allocator' in its interface).
//
// First, we define a class, 'Data1', that has a legacy-AA interface:
//..
    class Data1 {
        // Legacy-AA data class.

        bslma::Allocator *d_allocator_p;
        // ...

      public:
        explicit Data1(bslma::Allocator *basicAllocator = 0)
            : d_allocator_p(basicAllocator) { /* ... */ }

        bslma::Allocator *allocator() const { return d_allocator_p; }
    };
//..
// Next, we define a class, 'MyClass1', that has a member of type 'Data1'.
// 'MyClass' uses a modern, bsl-AA interface:
//..
    class MyClass1 {
        bsl::allocator<char> d_allocator;
        Data1                d_data;

      public:
        typedef bsl::allocator<char> allocator_type;

        explicit MyClass1(const allocator_type& allocator = allocator_type());

        const Data1& data() const { return d_data; }
        allocator_type get_allocator() const { return d_allocator; }
    };
//..
// Next, we define the constructor for 'MyClass1'.  Since 'MyClass1' uses
// 'bsl::allocator' and the 'Data1' uses 'bslma::Allocator *', we employ
// 'bslma::AllocatorUtil::adapt' to obtain an allocator suitable for passing to
// the constructor for 'd_data':
//..
    MyClass1::MyClass1(const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data(bslma::AllocatorUtil::adapt(allocator))
    {
    }
//..
// Next, assume that we update our 'Data' class from legacy-AA to bsl-AA
// (renamed from 'Data1' to 'Data2' for illustrative purposes):
//..
    class Data2 {
        // Bsl-AA data class.

        bsl::allocator<int> d_allocator;
        // ...

      public:
        typedef bsl::allocator<int> allocator_type;

        explicit Data2(const allocator_type& allocator = allocator_type())
            : d_allocator(allocator) { /* ... */ }

        allocator_type get_allocator() const { return d_allocator; }
    };
//..
// Now, we notice that **nothing** about 'MyClass' needs to change, not even
// the way its constructed passes an allocator to 'd_data':
//..
    class MyClass2 {
        bsl::allocator<char> d_allocator;
        Data2                d_data;

      public:
        typedef bsl::allocator<char> allocator_type;

        explicit MyClass2(const allocator_type& allocator = allocator_type());

        const Data2& data() const { return d_data; }
        allocator_type get_allocator() const { return d_allocator; }
    };

    MyClass2::MyClass2(const allocator_type& allocator)
        : d_allocator(allocator)
        , d_data(bslma::AllocatorUtil::adapt(allocator))
    {
    }
//..
// Finally, we test both versions of 'MyClass' and show that the allocator that
// is passed to the 'MyClass' constructor gets forwarded to its data member:
//..
    void usageExample1()
    {
        bslma::TestAllocator ta;
        bsl::allocator<char> alloc(&ta);

        MyClass1 obj1(alloc);
        ASSERT(&ta == obj1.data().allocator());

        MyClass2 obj2(alloc);
        ASSERT(alloc == obj2.data().get_allocator());
    }
//..

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'adapt'
        //
        // Concerns:
        //: 1 For allocators of type 'bslma::Allocator *' or specializations of
        //:   'bsl::allocator', the return value of 'adapt' is convertible to
        //:   both 'bslma::Allocator *' and 'bsl::allocator<T>' (for arbitrary
        //:   'T').
        //: 2 For allocators of type 'bslma::Allocator *' or specializations of
        //:   'bsl::allocator', the return value of 'adapt' can be passed as
        //:   the allocator argument for direct initializing an object of
        //:   bsl-AA or legacy-AA type.
        //: 3 Concerns 1 and 2 apply equally to allocators of type derived from
        //:   'bsl::allocator' or pointer to a type derived from
        //:   'bslma::Allocator'.
        //: 4 For all other allocator types, the return value of 'adapt' can be
        //:   used to initialize a an object in the same allocator family and
        //:   used to initialize an AA object having a compatible
        //:   'allocator_type'.
        //
        // Plan:
        //: 1 Using 'alloc' arguments of type 'bslma::Allocator *' and
        //:   'bsl::allocator<short>', call 'adapt(alloc)' and use the return
        //:   value to copy-initialize an object of type 'bslma::Allocator *'
        //:   and an object of type 'bsl:allocator<int>'.  Verify that the
        //:   constructed object represents the same allocator as 'alloc'.
        //:   (C-1)
        //: 2 With the same arguments as step 1, use the return value of
        //:   'adapt' to direct-initialize an object of legacy-AA type and an
        //:   object of bsl-AA type.  Verify that the allocator stored by the
        //:   constructed object represents the same allocator as was passed to
        //:   'adapt'.  (C-2)
        //: 3 Repeat steps 1 and 2 with arguments of type 'bslma::TestAllocator
        //:   *' and 'DerivedAllocator<float>', where 'DerivedAllocator<float>'
        //:   is derived from 'bsl::allocator<float>'.  (C-3)
        //: 4 Pass an object of a non-bsl allocator type to 'adapt' and use the
        //:   return value to initialize an allocator object belonging to the
        //:   allocator same family.  Verify that the input and output
        //:   represent the same result.  Initialize an object of stl-AA type
        //:   with the return value of 'adapt'.  Verify that the expected value
        //:   was used as its allocator.  (C-4)
        //
        // TESTING
        //      bslma::Allocator *adapt(const bsl::allocator<TYPE>&);
        //      ALLOCATOR         adapt(const ALLOCATOR&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'adapt'"
                            "\n===============\n");

        bslma::TestAllocator    ta1, ta2, ta3, ta4;
        bslma::Allocator       *a1 = &ta1;
        bsl::allocator<short>   a2(&ta2);
        DerivedAllocator<float> a3(&ta3);
        StlAllocator<bool>      a4(&ta4);

        // Step 1
        ASSERT(matchType<bslma::Allocator *>(Util::adapt(a1)));
        ASSERT(a1 == Util::adapt(a1));
        ASSERT(matchType<bslma::Allocator *>(Util::adapt(a2)));
        ASSERT(a2.mechanism() == Util::adapt(a2));

        bsl::allocator<int> baa = Util::adapt(a1);
        ASSERT(a1 == baa.mechanism());
        bsl::allocator<int> bab = Util::adapt(a2);
        ASSERT(a2 == bab);

        // Step 2
        LegacyAAClass lca(Util::adapt(a1));
        ASSERT(a1 == lca.allocator());
        LegacyAAClass lcb(Util::adapt(a2));
        ASSERT(a2.mechanism() == lcb.allocator());

        BslAAClass bca(Util::adapt(a1));
        ASSERT(a1 == bca.get_allocator().mechanism());
        BslAAClass bcb(Util::adapt(a2));
        ASSERT(a2 == bcb.get_allocator());

        // Step 3
        ASSERT(matchType<bslma::TestAllocator *>(Util::adapt(&ta1)));
        bslma::Allocator *apc = Util::adapt(&ta1);
        ASSERT(&ta1 == apc);
        ASSERT(matchType<bslma::Allocator *>(Util::adapt(a3)));
        bslma::Allocator *apd = Util::adapt(a3);
        ASSERT(a3.mechanism() == apd);
        bsl::allocator<int> bac = Util::adapt(&ta1);
        ASSERT(&ta1 == bac.mechanism());
        bsl::allocator<int> bad = Util::adapt(a3);
        ASSERT(a3 == bad);

        LegacyAAClass lcc(Util::adapt(&ta1));
        ASSERT(&ta1 == lcc.allocator());
        LegacyAAClass lcd(Util::adapt(a3));
        ASSERT(a3.mechanism() == lcd.allocator());
        BslAAClass bcc(Util::adapt(&ta1));
        ASSERT(&ta1 == bcc.get_allocator().mechanism());
        BslAAClass bcd(Util::adapt(a3));
        ASSERT(a3 == bcd.get_allocator());

        // Step 4
        ASSERT(matchType<StlAllocator<bool> >(Util::adapt(a4)));
        StlAllocator<char> saa = Util::adapt(a4);
        ASSERT(a4 == saa);

        StlAAClass sab(Util::adapt(a4));
        ASSERT(a4 == sab.get_allocator());

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Test 'adapt'
        {
            bslma::TestAllocator ta;
            bsl::allocator<int>  alloc(&ta);

            bsl::allocator<char> a1 = Util::adapt(&ta);
            ASSERT(&ta == a1.mechanism());

            bslma::Allocator *a2p = Util::adapt(alloc);
            ASSERT(alloc.mechanism() == a2p);
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
