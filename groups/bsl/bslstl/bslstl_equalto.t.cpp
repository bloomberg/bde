// bslstl_equalto.t.cpp                                               -*-C++-*-
#include <bslstl_equalto.h>

#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 3] operator()(const VALUE_TYPE&, const VALUE_TYPE&) const
// [ 2] equal_to()
// [ 2] equal_to(const equal_to)
// [ 2] ~equal_to()
// [ 2] equal_to& operator=(const equal_to&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
// [ 4] Standard typedefs
// [ 5] Bitwise-movable trait
// [ 5] IsPod trait
// [ 6] QoI: Is an empty type
// [ 7] equal_to<void>::operator()

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

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a List Set
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to keep a set of a small number of elements, and the only
// comparison operation we have on the type of the elements is an equality
// operator.  We can keep a singly-linked list of the elements, and
// exhausitively use the comparison operator to see if a given value exists in
// the list, forming a primitive set.
//
// First, we define our 'ListSet' template class:

template <class TYPE, class EQUALS = bsl::equal_to<TYPE> >
class ListSet {
    // This class implements a crude implementation of a set, that will keep a
    // set of values and be able to determine if an element is a member of the
    // set.  Unlike a 'bsl::set' or 'bsl::unordered_set', no hash function or
    // transitive 'operator<' is required -- only a transitive 'EQUALS'
    // operator.
    //
    // The 'TYPE' template parameter must have a public copy constructor and
    // destructor available.
    //
    // The 'EQUALS' template parameter must a function with a function whose
    // signature is
    //..
    //  bool operator()(const TYPE& lhs, const TYPE& rhs) const;
    //..
    // and that returns 'true' if 'lhs' and 'rhs' are equivalent and 'false'
    // otherwise.  This equivalence relation must be transitive and symmetric.
    // The comparator must have a publicly accessible default constructor and a
    // publicly accessible destructor.

    // PRIVATE TYPES
    struct Node {
        TYPE  d_value;
        Node *d_next;
    };

    // DATA
    EQUALS            d_comparator;
    Node             *d_nodeList;
    bslma::Allocator *d_allocator_p;

  private:
    // NOT IMPLEMENTED
    ListSet(const ListSet&);
    ListSet& operator=(const ListSet&);

  public:
    // CREATORS
    explicit
    ListSet(bslma::Allocator *allocator = 0)
    : d_comparator()
    , d_nodeList(0)
    , d_allocator_p(bslma::Default::allocator(allocator))
        // Create an empty "ListSet' using the specified 'allocator', or the
        // default allocator if none is specified.
    {}

    ~ListSet()
        // Release all memory used by this 'ListSet'
    {
        for (Node *node = d_nodeList; node; ) {
            Node *toDelete = node;
            node = node->d_next;

            d_allocator_p->deleteObject(toDelete);
        }
    }

    // MANIPULATOR
    bool insert(const TYPE& value)
        // If the specified 'value' isn't contained in this 'ListSet', add it
        // and return 'true', otherwise, return 'false' with no change to the
        // 'ListSet'.
    {
        if (count(value)) {
            return false;                                             // RETURN
        }

        Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
        bslma::ConstructionUtil::construct(&node->d_value,
                                           d_allocator_p,
                                           value);
        node->d_next = d_nodeList;
        d_nodeList = node;

        return true;
    }

    int count(const TYPE& value) const
        // Return the number of nodes whose 'd_value' field is equivalent to
        // the specified 'value', which will always be 0 or 1.
    {
        for (Node *node = d_nodeList; node; node = node->d_next) {
            if (d_comparator(node->d_value, value)) {
                return 1;                                             // RETURN
            }
        }

        return 0;
    }
};

///Example 2: Using Our List Set For a Custom Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to have a list set containing objects of a custom type.  We
// can declare an 'operator==' for our custom type, and 'equal_to' will use
// that.  We will re-use the 'ListSet' template class from example 1, and
// create a new custom type.
//
// First, we define a type 'StringThing', which will contain a 'const char *'
// pointer, it will be a very simple type, that is implicitly castable to or
// from a 'const char *'.  It has no 'operator==' defined, so 'equal_to' will
// need to be explicitly specialized for it:

class StringThing {
    // This class holds a pointer to zero-terminated string.  It is implicitly
    // convertible to and from a 'const char *'.  The difference between this
    // type and a 'const char *' is that 'operator==' will properly compare two
    // objects of this type for equality of strings rather than equality of
    // pointers.

    // DATA
    const char *d_string;    // held, not owned

  public:
    // CREATOR
    StringThing(const char *string)                                 // IMPLICIT
    : d_string(string)
        // Create a 'StringThing' object out of the specified 'string'.
    {}

    // ACCESSOR
    operator const char *() const
        // Implicitly cast this 'StringThing' object to a 'const char *' that
        // refers to the same buffer.
    {
        return d_string;
    }
};

// Then, we create an 'operator==' for StringThings
bool operator==(const StringThing& lhs, const StringThing& rhs)
    // Return 'true' if the specified 'rhs' contains a string that compares
    // equal to the string contained by the specified 'lhs', and 'false'
    // otherwise.
    {
        return !strcmp(lhs, rhs);
    }

bool operator==(int lhs, const StringThing& rhs)
    // Return 'true' if the specified 'rhs', when converted to an int, is equal
    // to the specified 'lhs', and 'false' otherwise.
{
    return lhs == atoi(rhs);
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
//  bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   Extracted from component header file.
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
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 2\n"
                            "===============\n");

// Next, in 'main', we declare a 'ListSet' containing 'StringThing's:

        ListSet<StringThing> lsst;

// Then, we insert a number of values, and observe that redundant inserts
// return 'false' with no effect:

        ASSERT(true  == lsst.insert("woof"));
        ASSERT(true  == lsst.insert("meow"));
        ASSERT(true  == lsst.insert("arf"));
        ASSERT(false == lsst.insert("woof"));
        ASSERT(true  == lsst.insert("bark"));
        ASSERT(false == lsst.insert("meow"));
        ASSERT(false == lsst.insert("woof"));

// Now, we observe that our 'count' method successfully distinguishes between
// values that have been stored in 'lsst' and those that haven't:

        ASSERT(1 == lsst.count("meow"));
        ASSERT(0 == lsst.count("woo"));
        ASSERT(1 == lsst.count("woof"));
        ASSERT(1 == lsst.count("arf"));
        ASSERT(0 == lsst.count("chomp"));

// Finally, we copy values into a buffer and observe that this makes no
// difference to 'count's results:

        char buffer[10];
        strcpy(buffer, "meow");
        ASSERT(1 == lsst.count(buffer));
        strcpy(buffer, "bite");
        ASSERT(0 == lsst.count(buffer));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //   Extracted from component header file.
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
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 1\n"
                            "===============\n");

// Then, in 'main', we declare an instance of 'ListSet' storing 'int's.  The
// default definition of 'bsl::equal_to' will work nicely:

        ListSet<int> lsi;

// Now, we insert several values into our 'ListSet'.  Note that successful
// insertions return 'true' while redundant ones return 'false' with no effect:

        ASSERT(true  == lsi.insert( 5));
        ASSERT(false == lsi.insert( 5));
        ASSERT(false == lsi.insert( 5));
        ASSERT(true  == lsi.insert(11));
        ASSERT(true  == lsi.insert(17));
        ASSERT(true  == lsi.insert(81));
        ASSERT(true  == lsi.insert(32));
        ASSERT(false == lsi.insert(17));

// Finally, we observe that our 'count' method successfully distinguishes
// between values that have been stored in our 'ListSet' and those that
// haven't:

        ASSERT(0 == lsi.count( 7));
        ASSERT(1 == lsi.count( 5));
        ASSERT(0 == lsi.count(13));
        ASSERT(1 == lsi.count(11));
        ASSERT(0 == lsi.count(33));
        ASSERT(1 == lsi.count(32));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // EQUAL_TO<VOID>
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 class equal_to<void> provides a templated operator() that returns
        //:   'true' when the two operands compare equal and 'false' otherwise.
        //
        // Plan:
        //: 1 Call the 'operator()' with a variety of types and values. (C-1)
        //
        // Testing:
        //   equal_to<void>::operator()
        // --------------------------------------------------------------------

        if (verbose) printf("EQUAL_TO<VOID>\n"
                            "==============\n");

        bsl::equal_to<void> eq0;
        ASSERT( eq0(1, 1));  // same type
        ASSERT(!eq0(1, 2));  // same type
        ASSERT( eq0(1, 1L)); // different type, convertible
        ASSERT(!eq0(1, 2L)); // different type, convertible

        ASSERT( (eq0(1, StringThing("1")))); // different type, not convertible
        ASSERT(!(eq0(1, StringThing("2")))); // different type, not convertible

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
        ASSERT( noexcept(eq0(1, 1)));
        ASSERT(!noexcept(eq0(1, StringThing("2"))));
#endif
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // QoI: Is an empty type
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 class 'equal_to' does not increase the size of an
        //:   object when used as a base class.
        //
        // Plan:
        //: 1 Define a non-empty class with no padding, 'TwoInts'.
        //:
        //: 2 Assert that 'TwoInts has the expected size of 8 bytes.
        //:
        //: 3 Create a class, 'DerivedInts', with identical structure to
        //:   'TwoInts' but derived from 'equal_to'.
        //:
        //: 4 Assert that both classes have the same size.
        //:
        //: 5 Create a class, 'IntsWithMember', with identical structure to
        //:   'TwoInts' and an 'equal_to' additional data member.
        //:
        //: 6 Assert that 'IntsWithMember' is larger than the other two
        //:   classes.
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QoI: Is an empty type"
                            "\n=============================\n");

        typedef int TYPE;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedInts : equal_to<TYPE> {
            int a;
            int b;
        };

        struct IntsWithMember {
            equal_to<TYPE> dummy;
            int              a;
            int              b;
        };

        ASSERT(8 == sizeof(TwoInts));
        ASSERT(8 == sizeof(DerivedInts));
        ASSERT(8 < sizeof(IntsWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BDE TYPE TRAITS
        //   The functor is an empty POD, and should have the appropriate BDE
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is bitwise copyable.
        //: 2 The class is bitwise moveable.
        //: 3 The class has the trivial default constructor trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1..3)
        //
        // Testing:
        //   BDE Traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TRAITS"
                            "\n==================\n");

        typedef int TYPE;

        ASSERT(bslmf::IsBitwiseMoveable<equal_to<TYPE> >::value);
        ASSERT(bsl::is_trivially_copyable<equal_to<TYPE> >::value);
        ASSERT(bsl::is_trivially_default_constructible<equal_to<TYPE>
                                                                     >::value);

        ASSERT(bslmf::IsBitwiseMoveable<equal_to<void> >::value);
        ASSERT(bsl::is_trivially_copyable<equal_to<void> >::value);
        ASSERT(bsl::is_trivially_default_constructible<equal_to<void>
                                                                     >::value);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STANDARD TYPEDEFS
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function.
        //
        // Concerns:
        //: 1 The typedef 'first_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 2 The typedef 'second_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 3 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bool'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bslmf::IsSame'. (C-1..3)
        //
        // Testing:
        //   typedef first_argument_type
        //   typedef second_argument_type
        //   typedef result_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEFS"
                            "\n=========================\n");

        typedef int TYPE;

        ASSERT((bslmf::IsSame<bool, equal_to<TYPE>::result_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE,
                              equal_to<TYPE>::first_argument_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE,
                              equal_to<TYPE>::second_argument_type>::VALUE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function, ().
        //
        // Concerns:
        //: 1 Objects of type 'equal_to' can be invokes as a binary
        //:   predicate returning 'bool' and taking two 'const char *'
        //:   arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments have the same string value.
        //:
        //: 4 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1
        //: 2
        //: 3
        //
        // Testing:
        //   operator()(const char*, const char *) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION CALL OPERATOR"
                            "\n======================\n");

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef const char *TYPE;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct {
            int         d_line;
            const char *d_value;
        } DATA[] = {
            // LINE    VALUE
            {  L_,     ""   },
            {  L_,     "a"  },
            {  L_,     "A"  },
            {  L_,     "1"  },
            {  L_,     "AZ" },
            {  L_,     "Aa" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const equal_to<TYPE> compare1 = equal_to<TYPE>();
        const equal_to<void> compare2 = equal_to<void>();

        for (int i = 0; i != NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const char *LHS      = DATA[i].d_value;
            for (int j = 0; j != NUM_DATA; ++j) {
                const char *RHS      = DATA[j].d_value;
                const bool  EXPECTED = i == j;

                LOOP_ASSERT(LINE, compare1(LHS, RHS) == EXPECTED);
                LOOP_ASSERT(LINE, compare2(LHS, RHS) == EXPECTED);
            }
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR
        static_assert(!equal_to<int>() (1,  2),  "");
        static_assert( equal_to<int>() (1,  1),  "");
        static_assert(!equal_to<void>()(1,  2L), "");
        static_assert( equal_to<void>()(1L, 1),  "");
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // C'TORS, D'TOR AND ASSIGNMENT OPERATOR (IMPLICITLY DEFINED)
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile, and
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //: 2 Objects can be created using the copy constructor.
        //: 3 The copy constructor is not declared as explicit.
        //: 4 Objects can be assigned to from constant objects.
        //: 5 Assignments operations can be chained.
        //: 6 Objects can be destroyed.
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-7)
        //:
        //: 2 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const equal_to'
        //:   object. (C-1)
        //:
        //: 3 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'equal_to' from the first. (C-2,3)
        //:
        //: 4 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 5 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 6 Verify the destructor is publicly accessible by allowing the two
        //:   'equal_to' object to leave scope and be
        //:    destroyed. (C-6)
        //
        // Testing:
        //   equal_to()
        //   equal_to(const equal_to)
        //   ~equal_to()
        //   equal_to& operator=(const equal_to&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nIMPLICITLY DEFINED OPERATIONS"
                            "\n=============================\n");

        typedef int TYPE;

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Value initialization\n");
        const equal_to<TYPE> obj1 = equal_to<TYPE>();
        const equal_to<void> obj3 = equal_to<void>();

        if (verbose) printf("Copy initialization\n");
        equal_to<TYPE> obj2 = obj1;
        equal_to<void> obj4 = obj3;

        if (verbose) printf("Copy assignment\n");
        obj2 = obj1;
        obj2 = obj2 = obj1;
        obj4 = obj3;
        obj4 = obj4 = obj3;


        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 1 Create an object 'compare' using the default ctor.
        //:
        //: 2 Call the 'compare' functor with two 'char' literals in lexical
        //:   order.
        //:
        //: 3 Call the 'compare' functor with two 'char' literals in reverse
        //:   lexical order.
        //:
        //: 4 Call the 'compare' functor with two identical 'char' literals.
        //:
        //: 5 Repeat steps 1-4 for 'equal_to<const int>' using 'int' literals.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            equal_to<char> compare;
            ASSERT( compare('A', 'A'));
            ASSERT(!compare('A', 'Z'));
            ASSERT( compare('Z', 'Z'));
        }

        {
            equal_to<const int> compare;
            ASSERT( compare(0, 0));
            ASSERT(!compare(1, 0));
            ASSERT( compare(1, 1));
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
