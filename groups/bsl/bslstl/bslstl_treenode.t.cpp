// bslstl_treenode.t.cpp                                              -*-C++-*-
#include <bslstl_treenode.h>

#include <bslstl_allocatortraits.h>
#include <bslstl_allocator.h>
#include <bslstl_string.h>

#include <bslalg_rbtreeanchor.h>
#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeutil.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>
#include <bsls_util.h>

#include <algorithm>
#include <functional>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace std;
using namespace bslstl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// MANIPULATORS
// [ 2] VALUE_TYPE& value();
// [ 2] const VALUE_TYPE& value() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 3] CONCERN: 'value' can be constructed with 'allocator_traits'.

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::RbTreeNode Node;

// Fundamental-type-specific print functions.

namespace bsl {

inline void debugprint(const bsl::string &s) {
    printf("\"%s\"", s.c_str()); fflush(stdout);
}

}  // close namespace bsl

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

class TestType1 {
    static bool s_constructedFlag;

  public:
    TestType1() { s_constructedFlag = true; }
    static bool isConstructed() { return s_constructedFlag; }
    static void reset() { s_constructedFlag = false; }
};

bool TestType1::s_constructedFlag = false;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Allocating and Deallocating 'TreeNode' Objects.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we define a factory class for allocating and
// destroying 'TreeNode' objects.
//
// First, we define the interface for the class 'NodeFactory':
//..
    template <class VALUE, class ALLOCATOR>
    class NodeFactory {
//..
// The parameterized 'ALLOCATOR' is intended to allocate objects of the
// parameterized 'VALUE', so to use it to allocate objects of 'TreeNode<VALUE>'
// we must rebind it to the tree node type.  Note that in general, we use
// 'allocator_traits' to perform actions using an allocator (including the
// rebind below):
//..
        // PRIVATE TYPES
        typedef typename bsl::allocator_traits<ALLOCATOR>::template
                               rebind_traits<TreeNode<VALUE> > AllocatorTraits;
        typedef typename AllocatorTraits::allocator_type       NodeAllocator;

        // DATA
        NodeAllocator d_allocator;  // rebound tree-node allocator

        // NOT IMPLEMENTED
        NodeFactory(const NodeFactory&);
        NodeFactory& operator=(const NodeFactory&);

      public:
        // CREATORS
        NodeFactory(const ALLOCATOR& allocator);
            // Create a tree node-factory that will use the specified
            // 'allocator' to supply memory.

        // MANIPULATORS
        TreeNode<VALUE> *createNode(const VALUE& value);
            // Create a new 'TreeNode' object holding the specified 'value'.

        void deleteNode(bslalg::RbTreeNode *node);
            // Destroy and deallocate the specified 'node'.  The behavior is
            // undefined unless 'node' is the address of a
            // 'TreeNode<VALUE>' object.
    };
//..
// Now, we implement the 'NodeFactory' type:
//..
    template <class VALUE, class ALLOCATOR>
    inline
    NodeFactory<VALUE, ALLOCATOR>::NodeFactory(const ALLOCATOR& allocator)
    : d_allocator(allocator)
    {
    }
//..
// We implement the 'createNode' function by using the rebound
// 'allocator_traits' for our allocator to in-place copy-construct the
// supplied 'value' into the 'value' data member of our 'result' node
// object.  Note that 'TreeNode' is a POD-like type, without a constructor, so
// we do not need to call its constructor here:
//..
    template <class VALUE, class ALLOCATOR>
    inline
    TreeNode<VALUE> *
    NodeFactory<VALUE, ALLOCATOR>::createNode(const VALUE& value)
    {
        TreeNode<VALUE> *result = AllocatorTraits::allocate(d_allocator, 1);
        AllocatorTraits::construct(d_allocator,
                                   bsls::Util::addressOf(result->value()),
                                   value);
        return result;
    }
//..
// Finally, we define the function 'deleteNode', for destroying 'TreeNode'
// objects.  Again, we use the rebound 'allocator_traits' for our tree node
// type, this time to destroy the 'd_value' date member of node, and then to
// deallocate its footprint.  Note that 'TreeNode' is a POD-like type,
//  so we do not need to call its destructor here:
//..
    template <class VALUE, class ALLOCATOR>
    inline
    void NodeFactory<VALUE, ALLOCATOR>::deleteNode(bslalg::RbTreeNode *node)
    {
        TreeNode<VALUE> *treeNode = static_cast<TreeNode<VALUE> *>(node);
        AllocatorTraits::destroy(d_allocator,
                                 bsls::Util::addressOf(treeNode->value()));
        AllocatorTraits::deallocate(d_allocator, treeNode, 1);
    }
//..
//
///Example 2: Creating a Simple Tree of 'TreeNode' Objects.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a container-type 'Set' for
// holding a set of values of a parameterized 'VALUE'.
//
// First, we define a comparator for 'VALUE' of 'TreeNode<VALUE>' objects.
// This type is designed to be supplied to functions in 'bslalg::RbTreeUtil'.
// Note that, for simplicity, this type uses 'operator<' to compare values,
// rather than a client defined comparator type.
//..
    template <class VALUE>
    class Comparator {
      public:
        // CREATORS
        Comparator() {}
            // Create a node-value comparator.

        // ACCESSORS
        bool operator()(const VALUE&              lhs,
                        const bslalg::RbTreeNode& rhs) const;
        bool operator()(const bslalg::RbTreeNode& lhs,
                        const VALUE&              rhs) const;
            // Return 'true' if the specified 'lhs' is less than (ordered
            // before) the specified 'rhs', and 'false' otherwise.  The
            // behavior is undefined unless the supplied 'bslalg::RbTreeNode'
            // object is of the derived 'TreeNode<VALUE>' type.
    };
//..
// Then, we implement the comparison methods of 'Comparator'.  Note that the
// supplied 'RbTreeNode' objects must be 'static_cast' to
// 'TreeNode<VALUE>' to access their value:
//..
    template <class VALUE>
    inline
    bool Comparator<VALUE>::operator()(const VALUE&              lhs,
                                       const bslalg::RbTreeNode& rhs) const
    {
        return lhs < static_cast<const TreeNode<VALUE>& >(rhs).value();
    }

    template <class VALUE>
    inline
    bool Comparator<VALUE>::operator()(const bslalg::RbTreeNode& lhs,
                                       const VALUE&              rhs) const
    {
        return static_cast<const TreeNode<VALUE>& >(lhs).value() < rhs;
    }
//..
// Now, having defined the requisite helper types, we define the public
// interface for 'Set'.  Note that for the purposes of illustrating the use of
// 'TreeNode' a number of simplifications have been made.  For example, this
// implementation provides only 'insert', 'remove', 'isMember', and
// 'numMembers' operations:
//..
    template <class VALUE,
              class ALLOCATOR = bsl::allocator<VALUE> >
    class Set {
        // PRIVATE TYPES
        typedef Comparator<VALUE>             ValueComparator;
        typedef NodeFactory<VALUE, ALLOCATOR> Factory;

        // DATA
        bslalg::RbTreeAnchor d_tree;     // tree of node objects
        Factory              d_factory;  // allocator for node objects

        // NOT IMPLEMENTED
        Set(const Set&);
        Set& operator=(const Set&);

      public:
        // CREATORS
        Set(const ALLOCATOR& allocator = ALLOCATOR());
            // Create an empty set. Optionally specify a 'allocator' used to
            // supply memory.  If 'allocator' is not specified, a default
            // constructed 'ALLOCATOR' object is used.

        ~Set();
            // Destroy this set.

        // MANIPULATORS
        void insert(const VALUE& value);
            // Insert the specified value into this set.

        bool remove(const VALUE& value);
            // If 'value' is a member of this set, then remove it and return
            // 'true', and return 'false' otherwise.

        // ACCESSORS
        bool isElement(const VALUE& value) const;
            // Return 'true' if the specified 'value' is a member of this set,
            // and 'false' otherwise.

        int numElements() const;
            // Return the number of elements in this set.
    };
//..
// Now, we define the implementation of 'Set':
//..
    // CREATORS
    template <class VALUE, class ALLOCATOR>
    inline
    Set<VALUE, ALLOCATOR>::Set(const ALLOCATOR& allocator)
    : d_tree()
    , d_factory(allocator)
    {
    }

    template <class VALUE, class ALLOCATOR>
    inline
    Set<VALUE, ALLOCATOR>::~Set()
    {
        bslalg::RbTreeUtil::deleteTree(&d_tree, &d_factory);
    }

    // MANIPULATORS
    template <class VALUE, class ALLOCATOR>
    void Set<VALUE, ALLOCATOR>::insert(const VALUE& value)
    {
        int comparisonResult;
        ValueComparator comparator;
        bslalg::RbTreeNode *parent =
            bslalg::RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
                                                         &d_tree,
                                                         comparator,
                                                         value);
        if (0 != comparisonResult) {
            bslalg::RbTreeNode *node = d_factory.createNode(value);
            bslalg::RbTreeUtil::insertAt(&d_tree,
                                         parent,
                                         comparisonResult < 0,
                                         node);
        }
    }

    template <class VALUE, class ALLOCATOR>
    bool Set<VALUE, ALLOCATOR>::remove(const VALUE& value)
    {
        bslalg::RbTreeNode *node =
                    bslalg::RbTreeUtil::find(d_tree, ValueComparator(), value);
        if (node) {
            bslalg::RbTreeUtil::remove(&d_tree, node);
            d_factory.deleteNode(node);
        }
        return node;
    }

    // ACCESSORS
    template <class VALUE, class ALLOCATOR>
    inline
    bool Set<VALUE, ALLOCATOR>::isElement(const VALUE& value) const
    {
        ValueComparator comparator;
        return bslalg::RbTreeUtil::find(d_tree, comparator, value);
    }

    template <class VALUE, class ALLOCATOR>
    inline
    int Set<VALUE, ALLOCATOR>::numElements() const
    {
        return d_tree.numNodes();
    }
//..
// Notice that the definition and implementation of 'Set' never directly
// uses the 'TreeNode' type, but instead use it indirectly through
// 'Comparator', and 'NodeFactory', and uses it via its base-class
// 'bslalg::RbTreeNode'.

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

// Finally, we test our 'Set'.
//..
    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    Set<int> set(&objectAllocator);
    ASSERT(0 == defaultAllocator.numBytesInUse());
    ASSERT(0 == objectAllocator.numBytesInUse());
    ASSERT(0 == set.numElements());

    set.insert(1);
    ASSERT(set.isElement(1));
    ASSERT(1 == set.numElements());

    set.insert(1);
    ASSERT(set.isElement(1));
    ASSERT(1 == set.numElements());

    set.insert(2);
    ASSERT(set.isElement(1));
    ASSERT(set.isElement(2));
    ASSERT(2 == set.numElements());

    ASSERT(0 == defaultAllocator.numBytesInUse());
    ASSERT(0 <  objectAllocator.numBytesInUse());
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: 'value' CAN BE CONSTRUCTED WITH 'allocator_traits'.
        //
        // Concerns:
        //: 1 'value' can be constructed with 'allocator_traits::construct'.
        //
        // Plan:
        //: 1 Create a class of which its construction can be verified.  Use
        //:   'allocator_traits::construct' to construct the class and verify
        //:   that the class's constructor is called.
        //
        // Testing:
        //   CONCERN: 'value' can be constructed with 'allocator_traits'.
        // --------------------------------------------------------------------
        bslma::TestAllocator da("default");
        bslma::TestAllocator oa("object");

        if (verbose) printf(
                "\nTesting construction of 'value' using allocator_traits.\n");
        {
            typedef TestType1      Type;
            typedef TreeNode<Type> Obj;

            typedef bsl::allocator<Obj>          Alloc;
            typedef bsl::allocator_traits<Alloc> AllocTraits;

            Alloc allocator(&oa);

            ASSERTV(!Type::isConstructed());

            Obj *xPtr = AllocTraits::allocate(allocator, 1);
            Obj& mX = *xPtr; const Obj& X = mX;
            (void) X;

            AllocTraits::construct(allocator,
                                   bsls::Util::addressOf(mX.value()));
            ASSERTV(Type::isConstructed());
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value().~Type();
            AllocTraits::deallocate(allocator, &mX, 1);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Manipulators can set value.
        //:
        //: 2 Accessor return value set by manipulator.
        //:
        //: 3 Accessor is declared const.
        //
        // Plan:
        //: 1 Create a 'TreeNode' with 'VALUE_TYPE' as 'int' and set 'value'
        //:   distinct numbers.  Verify the values are set with the accessor.
        //:
        //: 2 Create a 'TreeNode' with a type that has a constructor that can
        //:   be verified if it has been invoked.  Verify that the constructor
        //:   is invoked when 'allocator_traits::construct' is used.
        //
        // Testing:
        //   VALUE_TYPE& value();
        //   const VALUE_TYPE& value() const;
        // --------------------------------------------------------------------
        bslma::TestAllocator da("default");
        bslma::TestAllocator oa("object");

        bslma::DefaultAllocatorGuard defaultGuard(&da);

        if (verbose) printf("\nTesting manipulator and accessor for 'int'.\n");
        {
            typedef int            Type;
            typedef TreeNode<Type> Obj;

            typedef bsl::allocator<Obj>          Alloc;
            typedef bsl::allocator_traits<Alloc> AllocTraits;

            Alloc allocator(&oa);
            Obj *xPtr = AllocTraits::allocate(allocator, 1);
            Obj& mX = *xPtr; const Obj& X = mX;

            mX.value() = 0;
            ASSERTV(X.value(), 0 == X.value());

            mX.value() = 1;
            ASSERTV(X.value(), 1 == X.value());

            mX.value() = INT_MAX;
            ASSERTV(X.value(), INT_MAX == X.value());

            ASSERTV(0 == da.numBlocksTotal());
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value().~Type();
            AllocTraits::deallocate(allocator, &mX, 1);
            ASSERTV(0 == oa.numBlocksInUse());
        }

        if (verbose) printf(
                         "\nTesting manipulator and accessor for 'string'.\n");
        {
            typedef bsl::string    Type;
            typedef TreeNode<Type> Obj;

            typedef bsl::allocator<Obj>          Alloc;
            typedef bsl::allocator_traits<Alloc> AllocTraits;

            Alloc allocator(&oa);
            Obj *xPtr = AllocTraits::allocate(allocator, 1);
            Obj& mX = *xPtr; const Obj& X = mX;

            AllocTraits::construct(allocator,
                                   bsls::Util::addressOf(mX.value()));

            const char D[] = "";
            const char A[] = "a_" SUFFICIENTLY_LONG_STRING;

            bslma::TestAllocator scratch("scratch");
            const bsl::string B("ABC", &scratch);

            Type value(&scratch);

            mX.value() = D;
            value = X.value();
            ASSERTV(value, D == value);
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value() = A;
            value = X.value();
            ASSERTV(value, A == value);
            ASSERTV(2 == oa.numBlocksInUse());

            mX.value() = B;
            value = X.value();
            ASSERTV(value, B == value);
            ASSERTV(2 == oa.numBlocksInUse());

            ASSERTV(0 == da.numBlocksTotal());

            mX.value().~Type();
            AllocTraits::deallocate(allocator, &mX, 1);
            ASSERTV(0 == oa.numBlocksInUse());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator da("default");
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        typedef TreeNode<int> Obj;
        Obj *xPtr = static_cast<Obj *>(da.allocate(sizeof(Obj)));
        Obj& mX = *xPtr; const Obj& X = mX;

        mX.value() = 0;
        ASSERTV(X.value(), 0 == X.value());

        mX.value() = 1;
        ASSERTV(X.value(), 1 == X.value());

        da.deallocate(&mX);
        ASSERTV(0 == da.numBytesInUse());

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
