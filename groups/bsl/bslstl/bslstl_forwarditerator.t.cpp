// bslstl_forwarditerator.t.cpp                                       -*-C++-*-

#include <bslstl_forwarditerator.h>

#include <bslstl_iterator.h>

#include <bslmf_issame.h>
#include <bsls_bsltestutil.h>
#include <bsls_unspecifiedbool.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bslstl::ForwardIterator' is an in-core value-semantic type that adapts a
// more limited type, which offers a basic set of operations, so that the
// resulting 'bslstl::ForwardIterator' object meets all the requirements of a
// standard Forward Iterator.  These requirements are spelled out in
// [forward.iterators], Table 103 - Forward iterator requirements.  The primary
// manipulator of an iterator is the pre-increment operator that, together
// with a function that returns an iterator to the start of a sequence, and a
// second function to return an iterator to the end of the same sequence, is
// sufficient to attain any achievable state.  While an iterator offers no
// direct accessors, its state can be reasonably inferred by inspecting the
// result of the dereference operator.
//
// In order to test this iterator adaptor, a simple container supporting
// forward iterators will be implemented, to provide the basic type to be
// adapted.  This container will use the 'bslstl::ForwardIterator' template
// to declare its iterators, as suggested in the usage example.
//
//  SKETCH NOTES FOR A PLAN THAT NEEDS UPDATING
//
// The following is the set of direct accessors (i.e., accessors that have
// direct contact with the physical state of the object):
// - int value() const;
// - ...;
//-----------------------------------------------------------------------------
// [ 2] ForwardIterator();
// [ 3] ForwardIterator(IMPL);
// [ 7] ForwardIterator(const ForwardIterator& original);
// [ 2] ~ForwardIterator();
// [ 9] ForwardIterator& operator=(const ForwardIterator& rhs);
// [ 2] ForwardIterator& operator++();
// [11] ForwardIterator operator++(ForwardIterator&, int);
// [ 4] T& operator*() const;
// [10] T *operator->() const;
// [  ] ITER_IMP& imp();
// [  ] const ITER_IMP& imp() const;
// [ 6] bool operator==(const ForwardIterator&, const ForwardIterator&);
// [ 6] bool operator!=(const ForwardIterator&, const ForwardIterator&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE is informative only, and does not compile independently
//
// Further, there are a number of behaviors that explicitly should not compile
// by accident that we will provide tests for.  These tests should fail to
// compile if the appropriate macro is defined.  Each such test will use a
// unique macro for its feature test, and provide a commented-out definition of
// that macro immediately above the test, to easily enable compiling that test
// while in development.  Below is the list of all macros that control the
// availability of these tests:
//  #define BSLSTL_FORWARDITERATOR_COMPILE_FAIL_NON_REFERENCE_DEREFERENCE
//-----------------------------------------------------------------------------

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GLOBAL HELPER FUNCTIONS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                ALGORITHMS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace
{

template<class Iter>
int testDistance( Iter first, Iter last ) {
    //  A basic algorithm to verify the iterator can type walk the range
    //  specified by the pair of iterators 'first' and 'last', and return at
    //  end of the range.  We choose to calculate distance as this might prove
    //  useful in verifying the number of iterations and operations in further
    //  tests.
    int result = 0;
    while( first != last ) {
        ++result;
        ++first;
    }
    return result;
}


// Create an overload set that can determine, using tag dispatch techniques,
// exactly which iterator tag is declared for this iterator adaptor.
enum iter_tag_type {
    output_iterator,
    input_iterator,
    forward_iterator,
    bidirectional_iterator,
    random_access_iterator
};

template<class Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::output_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return output_iterator;
}

template<class Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::input_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return input_iterator;
}

template<class Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::forward_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return forward_iterator;
}

template<class Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::bidirectional_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return bidirectional_iterator;
}

template<class Iter>
iter_tag_type
testTagDispatch( Iter, Iter, std::random_access_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return random_access_iterator;
}


//  Create a set of overloads to verify that appropriately tagged algorithms
//  can be called with this iterator adaptor, but not other categories.
//  To an extend this is duplicating work that should be part of the
//  bslstl_iterator test driver, consider moving as appropriate.

template<class Iter>
bool testOutputTag( Iter, Iter ) { return true; }

template<class Iter>
bool testOutputTag( Iter, Iter, std::output_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

template<class Iter>
bool testInputTag( Iter, Iter ) { return false; }

template<class Iter>
bool testInputTag( Iter, Iter, std::input_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return true;
}

template<class Iter>
bool testForwardTag( Iter, Iter ) { return false; }

template<class Iter>
bool testForwardTag( Iter, Iter, std::forward_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return true;
}

template<class Iter>
bool testBidirectionalTag( Iter, Iter ) { return true; }

template<class Iter>
bool testBidirectionalTag( Iter, Iter, std::bidirectional_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

template<class Iter>
bool testRandomAccessTag( Iter, Iter ) { return true; }

template<class Iter>
bool testRandomAccessTag( Iter, Iter, std::random_access_iterator_tag* =
                                         (typename Iter::iterator_category*)0 )
{
    return false;
}

struct Wrap { int data; };

//  Class for testing compilation error, see
//  BSLSTL_FORWARDITERATOR_COMPILE_FAIL_NON_REFERENCE_DEREFERENCE
template<class TYPE>
class IncorrectDereferenceIter
{
public:
    // Compiler-generated default, copy and destruction
    // IncorrectDereferenceIter();
    // IncorrectDereferenceIter(const IncorrectDereferenceIter&);
    // IncorrectDereferenceIter& operator=(const IncorrectDereferenceIter&);
    // ~IncorrectDereferenceIter();

    TYPE operator*() const;
        // This type of signature should fail to compile

    bool operator==(const IncorrectDereferenceIter& rhs) const;
    void operator++();
};


}  // close unnamed namespace

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//  Minimal implementation of a singly linked list to validate basic iterator
//  operations with a suitably restricted container.  The important feature for
//  validating the iterator adaptor is that the iterator-like class being
//  adapted for the container's iterators support only the minimum set of
//  operations required for the adaptor.  If not for this requirement, we would
//  use 'slist' as an already well-tested component.  Note that this simple
//  list container supports appending items to the front of the list, but does
//  not support insertion at any other location, nor erasure.  This provides
//  the minimal set of facilities needed to validate the iterator under test.
template <class T>
class my_List
{
    // PRIVATE TYPES
    struct Node
    {
        Node *d_next;
        T     d_val;

        Node(const T& val, Node* next = 0)
            : d_next(next), d_val(val) { }
    };

    class IteratorImp
    {
      private:
        // PRIVATE TYPES
        typedef bsls::UnspecifiedBool<IteratorImp> BoolHost;
        typedef typename BoolHost::BoolType        BoolType;

        // DATA
        Node*   d_node;

      public:
        IteratorImp(Node* node = 0) : d_node(node) { }

        // Compiler-generated copy and destruction
        // IteratorImp(const IteratorImp&);
        // IteratorImp& operator=(const IteratorImp&);
        // ~IteratorImp();

        BoolType operator==(const IteratorImp& rhs) const
            { return d_node == rhs.d_node
                   ? BoolHost::trueValue()
                   : BoolHost::falseValue(); }

        T& operator*() const { return d_node->d_val; }

        void operator++() { d_node = d_node->d_next; }
    };

    friend class bslstl::ForwardIterator<T,IteratorImp>;

    friend class bslstl::ForwardIterator<const T,IteratorImp>;

    // Private copy constructor and copy assignment operator.
    // This class does not have value semantics.
    my_List(const my_List&);
    my_List& operator=(const my_List&);

    // PRIVATE DATA
    Node * d_head;

  public:
    // Declare iterator types:
    typedef bslstl::ForwardIterator<T, IteratorImp>       iterator;
    typedef bslstl::ForwardIterator<const T, IteratorImp> const_iterator;

    my_List() : d_head() {}

    ~my_List() {
        while (d_head) {
            Node *x = d_head;
            d_head = d_head->d_next;
            delete x;
        }
    }

    void push(const T& v) {
        d_head = new Node(v, d_head);
    }

    iterator begin() { return IteratorImp(d_head); }
    iterator end()   { return IteratorImp(0); }
    const_iterator begin() const { return IteratorImp(d_head); }
    const_iterator end() const { return IteratorImp(0); }
};


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING POST-INCREMENT OPERATOR
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   ForwardIterator operator++(ForwardIterator&, int);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POST-INCREMENT OPERATOR"
                            "\n===============================\n");

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::ForwardIterator<int, int*> iterator;

        if (verbose) printf("\nConstruct a basic iterator value\n");
        iterator it1 = testData;
        iterator it2 = it1;
        iterator it3 = it1;

        ++it2;
        ASSERT( it3++ == it1);
        ASSERT( it3 == it2);


      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR->
        //
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //    T *operator->() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OPERATOR->"
                            "\n==================\n");

        if (verbose) printf("\nVerify iterator properties with a"
                             " directly examinable container\n");
        {

        //  Declare test data and types
        Wrap testData[2] = { {13}, {99} };
        typedef bslstl::ForwardIterator<Wrap, Wrap*> iterator;
        typedef bslstl::ForwardIterator<const Wrap, Wrap*> const_iterator;

        const iterator itWritable = testData;
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testData;

        ASSERT(13 == itWritable->data);
        ASSERT(13 == itReadable->data);
        ASSERT(&itWritable->data == &testData[0].data);
        ASSERT(&itReadable->data == &testData[0].data);

        if (verbose) printf("\nVerify can observe new value written through"
                            " the iterator\n");
        itWritable->data = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
        ASSERT(42 == itReadable->data);
        ASSERT(42 == itWritable->data);
        ASSERT(42 == testData[0].data);

        testData[0].data = 13;

        ASSERT(13 == itWritable->data);
        ASSERT(13 == itReadable->data);
        ASSERT(&itWritable->data == &testData[0].data);
        ASSERT(&itReadable->data == &testData[0].data);

        iterator itNext = itWritable;
        ++itNext;
        ASSERT(99 == itNext->data);
        ASSERT(13 == itWritable->data);
        ASSERT(&itNext->data == &testData[1].data);

        itNext->data = 42;
        ASSERT(42 == itNext->data);
        ASSERT(13 == itWritable->data);
        ASSERT(42 == testData[1].data);
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   ForwardIterator& operator=(const ForwardIterator& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATOR"
                            "\n===========================\n");

        if (verbose) printf("\nTesting assignment u = v\n");

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        testContainer.push(42);
        testContainer.push(13);

        const iterator itBase = testContainer.begin();
        const iterator itSecond = ++testContainer.begin();

        iterator itTest = itBase;
        ASSERT(itBase == itTest);
        ASSERT(itSecond != itTest);

        itTest = itSecond;
        ASSERT(itBase != itTest);
        ASSERT(itSecond == itTest);

        itTest = itBase;
        ASSERT(itBase == itTest);
        ASSERT(itSecond != itTest);

        // Test const_iterator assignment
        const_iterator citTest = itBase;
        ASSERT(itBase == citTest);
        ASSERT(itSecond != citTest);

        citTest = itSecond;
        ASSERT(itBase != citTest);
        ASSERT(itSecond == citTest);

        citTest = itBase;
        ASSERT(itBase == citTest);
        ASSERT(itSecond != citTest);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTOR
        //  This iterator has two value constructors,
        //  (1) construct from an iterator-like value pointing to host sequence
        //  (2) const a constant iterator from a mutable iterator
        //  Note that (1) has been thoroughly tested by the generator functions
        //  in test [3], and (2) has been widely used in earlier tests to test
        //  interoperability.
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTOR"
                            "\n=========================\n");

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl::ForwardIterator<int, int*> iterator;
        typedef bslstl::ForwardIterator<const int, int*> const_iterator;

        const iterator itSource = testData;
        const_iterator itCopy = itSource;
        ASSERT(itSource == itCopy);

      } break;
       case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   ForwardIterator(const ForwardIterator& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl::ForwardIterator<int, int*> iterator;
        typedef bslstl::ForwardIterator<const int, int*> const_iterator;

        const iterator itSource = testData;
        iterator itCopy = itSource;
        ASSERT(itSource == itCopy);

        const const_iterator citSource = testData;
        const_iterator citCopy = itSource;
        ASSERT(citSource == citCopy);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //   The equality operator defines the notion of value for a type.  It
        //   shall return 'true' when two iterators have the same value, and
        //   'false' otherwise.  Likewise, operator!= shall return
        //
        // Concerns:
        //    Iterators must compare equal to themselves.
        //    constant iterators can be compared with mutable iterators
        //    constant and mutable iterators referring to the same element
        //      shall compare equal
        //    iterators that do not refer to the same element shall not compare
        //    equal
        //    tests based on the identity of the referenced element must be
        //    valid, even when the referenced element overloads operator&
        //
        // Plan:
        //   Create a list with a single element, so that the sequence is not
        //   empty, but two increments will take an iterator from 'begin' to
        //   'end'.  Validate that both the begin and end iterators compare
        //   equal to themselves, and do not compare equal to each other.  Then
        //   verify that an iterator copied from a 'begin' iterator compares
        //   equal to 'begin' and not 'end', after a single increment compares
        //   equal to neither, and after a third increment compares equal to
        //   the 'end' iterator.  Validating two iterators compare equal means
        //   asserting both the '==' and '!=' operators yield the correct
        //   values.
        //
        // Testing:
        //   bool operator==(const ForwardIterator&, const ForwardIterator&);
        //   bool operator!=(const ForwardIterator&, const ForwardIterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATOR"
                            "\n=========================\n");

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        testContainer.push(42);
        testContainer.push(13);

        if (verbose) printf("\nvalidate self-equality\n");
        const iterator itBegin = testContainer.begin();
        ASSERT(itBegin == itBegin);
        ASSERT(!(itBegin != itBegin));
        ASSERT(testContainer.begin() == itBegin);

        if (verbose) printf("\nvalidate inequality\n");
        const iterator itEnd = testContainer.end();
        ASSERT(itBegin != itEnd);
        ASSERT(!(itBegin == itEnd));
        ASSERT(itEnd == itEnd);
        ASSERT(!(itEnd != itEnd));

        if (verbose) printf("\nvalidate interoperability of types\n");
        const const_iterator citBegin = testContainer.begin();
        const_iterator citEnd   = testContainer.end();
        ASSERT(citBegin == itBegin);
        ASSERT(citEnd == itEnd);

        if (verbose) printf("\nvalidate transition to expected value\n");
        iterator itCursor = testContainer.begin();
        ASSERT(itBegin == itCursor);
        ASSERT(citEnd != itCursor);

        ++itCursor;
        ASSERT(itBegin != itCursor);
        ASSERT(citEnd != itCursor);

        ++itCursor;
        ASSERT(itBegin != itCursor);
        ASSERT(citEnd == itCursor);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'print' AND 'operator<<'
        //   N/A for this component, although a debug printer might be
        //   considered in the future.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'print' AND 'operator<<'"
                            "\n================================\n");

        if (verbose) printf("\nIterators do not support printing or streaming."
                            "\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   While an iterator has no named attributes, and so no basic
        //   accessors, the -> operator acts as a basic accessor, returning the
        //
        //
        // Concerns:
        //   Dereferencing an iterator should refer to the expected element
        //   Must be able to check dereferenced object's identity, even if it
        //   overloads operator&
        //   Should be able to write through a mutable iterator, and observe
        //   the effect through a constant iterator referring to the same
        //   element.
        //
        // Plan:
        //   Perform initial validation against an array of ints, that can be
        //   directly manipulated to confirm iterators are operating correctly.
        //   Repeat tests using the sample list container to verify that the
        //   same operations work as advertised when the adapted iterator
        //   offers only the minimal set of operations.
        //
        // Testing:
        //   T& operator*() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf("\nVerify iterator properties with a"
                             " directly examinable container\n");
        {
            //  Declare test data and types
            int testData[1] = { 13 };
            typedef bslstl::ForwardIterator<int, int*> iterator;
            typedef bslstl::ForwardIterator<const int, int*> const_iterator;

            const iterator itWritable = testData;
            // Obtain a fresh copy of an equivalent constant iterator
            const_iterator itReadable = testData;

            ASSERT(13 == *itWritable);
            ASSERT(13 == *itReadable);
            ASSERT(&*itWritable == testData);
            ASSERT(&*itReadable == testData);

            if (verbose) printf("\nVerify can observe new value written"
                                 " through the iterator\n");
            *itWritable = 42;

            // Test itReadable before itWritable to be sure the update is not a
            // side effect of the reading!
            ASSERT(42 == *itReadable);
            ASSERT(42 == *itWritable);
            ASSERT(42 == testData[0]);

            testData[0] = 13;

            ASSERT(13 == *itWritable);
            ASSERT(13 == *itReadable);
            ASSERT(&*itWritable == testData);
            ASSERT(&*itReadable == testData);
        }

        if (verbose) printf(
                           "\nRepeat tests with a minimal adapted iterator\n");
        {
            typedef my_List<int> test_container_type;
            typedef test_container_type::iterator iterator;
            typedef test_container_type::const_iterator const_iterator;

            test_container_type testContainer;
            testContainer.push(13);

            const iterator itWritable = testContainer.begin();
            // Obtain a fresh copy of an equivalent constant iterator
            const_iterator itReadable = testContainer.begin();

            ASSERT(13 == *itWritable);
            ASSERT(13 == *itReadable);
            ASSERT(&*itWritable == &*itReadable);

            if (verbose) printf("\nVerify can observe new value written"
                                " through the iterator\n");
            *itWritable = 42;

            //  Test itReadable before itWritable to be sure the update is not
            //  a side effect of the reading!
            ASSERT(&*itWritable == &*itReadable);
            ASSERT(42 == *itReadable);
            ASSERT(42 == *itWritable);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The primitive generators for our iterator, the primary piece of
        //   test apparatus, are the 'begin' and 'end' member functions of the
        //   sample container type.  It is difficult to test the value of these
        //   iterators without any basic accessors, but we can assert several
        //   basic properties:
        //   ..
        //    1/ No generated iterator shall have the value of a default-
        //       constructed iterator.
        //    2/ The generated iterators shall compare equal to themselves.
        //    3/ The iterators generated by 'begin' and 'end' shall compare
        //       equal if, and only if, the underlying container is empty.
        //   ..
        //   Note that while it is not possible to compare an iterator with
        //   the default-constructed iterator value in the general case, the
        //   specific cases we are testing ensure this is well defined.  This
        //   is another facility of the iterator adaptors.
        //
        // Concerns:
        //   1. The test container may have some latent bug that yields bad
        //      iterators.
        //
        // Testing:
        //   class my_List<T>
        //   my_List<T>::begin
        //   my_List<T>::end
        //   ForwardIterator(IMPL);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose) printf("\nValidating primitive test machinery\n");

        if (verbose) printf("\nTesting class my_List<int>\n");

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        iterator itBegin = testContainer.begin();
        iterator itEnd   = testContainer.end();
        ASSERT(itBegin == itEnd);

        testContainer.push(3);
        itBegin = testContainer.begin();
        itEnd = testContainer.end();
        ASSERT(itBegin != itEnd);

        ++itBegin;
        ASSERT(itBegin == itEnd);

        testContainer.push(2);
        testContainer.push(1);

        if (verbose) printf("\nCheck iterator range has right length\n");
        int length = 0;
        itBegin = testContainer.begin();
        while(itBegin != itEnd) {
            ++length;
            ++itBegin;
        }
        LOOP_ASSERT(length, 3 == length);

        if (verbose) printf("\nCheck iterators refer to right values\n");
        itBegin = testContainer.begin();
        ASSERT(1 == *(itBegin++));
        ASSERT(2 == *(itBegin++));
        ASSERT(3 == *(itBegin++));

        if (verbose) printf("\nRepeat the tests for const_iterators\n");
        const TestContainer& constContainer = testContainer;
        const_iterator itcBegin = constContainer.begin();
        const const_iterator itcEnd = constContainer.end();
        length = 0;
        while( itcBegin != itcEnd) {
            ++length;
            ++itcBegin;
        }
        ASSERT(3 == length);

        itcBegin = constContainer.begin();
        ASSERT(1 == *itcBegin++);
        ASSERT(2 == *itcBegin++);
        ASSERT(3 == *itcBegin++);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   ForwardIterator();
        //   ~ForwardIterator();
        //   ForwardIterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS"
                            "\n============================\n");

        if (verbose) printf("\nTesting default constructor, destructor, "
                             "and 'operator++'.\n");

        //  Declare test data and types
        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::ForwardIterator<int, int*> iterator;
        typedef bslstl::ForwardIterator<const int, int*> const_iterator;

        //  Confirm that we can default-initialize const iterators.
        //  Confirm that iterator and const_iterator can be compared.
        const iterator itDefault;
        const const_iterator itcDefault;
        ASSERT( itDefault == itcDefault);

        //  Confirm that an iterator over our reference array does not have the
        //  same value as a default constructed iterator.  (Actually, this
        //  comparison is undefined in general.)
        const const_iterator itcOrigin = testData;
        iterator itData = testData;
        ASSERT( itDefault != itData);
        ASSERT( itcOrigin == itData);

        //  Confirm that an incremented iterator does not have the same value
        //  as the initial iterator.
        ++itData;
        ASSERT( itcOrigin != itData);

        //  Confirm that incrementing a second copy of the initial iterator
        //  has the same value as the first incremented iterator.
        const_iterator itcCopy = itcOrigin;
        ASSERT( itcOrigin == itcCopy);

        ++itcCopy;
        ASSERT( itcOrigin != itcCopy);

        const_iterator itReader = testData;
        const int* itValidator = testData;
        for(size_t i = 0; i < sizeof(testData)/sizeof(int); ++itValidator,
                                                            ++itReader, ++i) {
            LOOP2_ASSERT(*itValidator, *itReader, *itValidator == *itReader);
        }
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
        //: 1 Invoke all methods and verify their behavior.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::ForwardIterator<int, int*> iterator;
        typedef bslstl::ForwardIterator<const int, int*> const_iterator;

        if (verbose) printf("\nConstruct a basic iterator value\n");
        iterator it1 = testData;
        LOOP_ASSERT( *it1, 0 == *it1);

        if (verbose) printf("\nMake a copy of that iterator\n");
        iterator it2 = it1;
        LOOP_ASSERT(  *it2,            0 ==  *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) printf("\nIncrement an iterator\n");
        ++it2;
        LOOP_ASSERT(  *it2,             1 == *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 !=   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 != &*it2);

        //  Increment the other iterator iterator,
        //  verify both iterators have the same value again
        if (verbose) printf(
                          "\nVerify multipass property of forward iterator\n");
        ++it1;
        LOOP_ASSERT(  *it1,            1 ==  *it1);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) printf("\nConstruct a const_iterator\n");
        const_iterator itEnd = testData + 4;

        //  Verify const_iterator and iterator interoperate
        //  Verify the expected number of iterations are required to traverse
        //  the array.
        if (verbose) printf(
                          "\nVerify multipass property of forward iterator\n");

        const_iterator itBegin = it1;
        int distance = testDistance( itBegin, itEnd );
        LOOP_ASSERT(distance, 3 == distance);

        if (verbose) printf(
                         "\nVerify writing through a dereferenced iterator\n");
        *it1 = 42;
        LOOP_ASSERT( *it1, 42 == *it1);

        if (verbose) printf("\nVerify that writes through one iterator are"
                             " visible through another\n");
        LOOP_ASSERT( *it2,         42 == *it2);
        LOOP2_ASSERT(*it1, *it2, *it1 == *it2);

        //  To test operator->, create an array of struct values
        Wrap testWrap[4] = { {0}, {1}, {2}, {3} };
        typedef bslstl::ForwardIterator<Wrap, Wrap*> wrap_iterator;
        typedef bslstl::ForwardIterator<const Wrap, Wrap*> const_wrap_iterator;

        wrap_iterator wit1 = testWrap;
        LOOP_ASSERT(wit1->data, 0 == wit1->data );

        const_wrap_iterator wit2 = wit1;
        LOOP_ASSERT(wit2->data,           0 == wit2->data);
        LOOP2_ASSERT(&*wit1, &*wit2,   wit1 == wit2);
        LOOP2_ASSERT(&*wit1, &*wit2, &*wit1 == &*wit2);

        wit1->data = 13;
        LOOP_ASSERT(wit1->data,          13 == wit1->data );
        LOOP_ASSERT(wit2->data,          13 == wit2->data);
        LOOP2_ASSERT(&*wit1, &*wit2, &*wit1 == &*wit2);

        //  Verify 'const_iterator' iterates, just like non-'const' 'iterator'
        ++wit1;
        ++wit2;
        LOOP2_ASSERT(&*wit1, &*wit2, wit1 == wit2);

        // Tests for post-increment
        ASSERT(wit2++ == wit1);
        ASSERT(1 == (*(wit1++)).data);


        //  Test for default constructor, which constructs a singular iterator
        //      Can assign to a singular value, and destroy it
        //      may not make a copy
        const_wrap_iterator x;

        //  Test assignment safely overwrites the singular value
        x = wit1;
        LOOP2_ASSERT(&*wit1, &*x, wit1 == x);

        // Not yet validated constness of any APIs, e.g., operator* and
        // operator-> (are constness concerns appropriate for a breathing test,
        // we just want to confirm that each individual API can be called.)

// #define BSLSTL_FORWARDITERATOR_COMPILE_FAIL_NON_REFERENCE_DEREFERENCE
#if defined(BSLSTL_FORWARDITERATOR_COMPILE_FAIL_NON_REFERENCE_DEREFERENCE)
        {
            // confirm that non-reference dereference return type fails to
            // compile - see DRQS 105559701.
            bslstl::ForwardIterator<const int, IncorrectDereferenceIter<int>>
                                                                            it;
        }
#endif
        // --------------------------------------------------------------------
        // (ORIGINAL) BREATHING/USAGE TEST
        //   We retain the original breathing test supplied by Pablo when the
        //   facility under test was a support class, rather than than a fully
        //   detailed component.  This section of the breathing test can be
        //   retired once the new testing facility has passed review.
        //
        // Concerns:
        //
        // Plan:
        //    Bring an iterator to life
        //    Make a copy, and assert is has the same value
        //    Increment the first iterator, assert no longer has the same value
        //    Increment the second iterator and assert that it again has the
        //    same value as the first
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\n(ORIGINAL BREATHING TEST)"
                            "\n=========================\n");

        static const int DATA[] = { 1, 2, 3, 4, 5 };
        static const int DATA_LEN = sizeof(DATA) / sizeof(DATA[0]);

        my_List<int> a; const my_List<int> &A = a;
        ASSERT(a.begin() == a.end());
        ASSERT(A.begin() == A.end());

        typedef my_List<int>::iterator iter_type;
        typedef my_List<int>::const_iterator const_iter_type;

        //  Assert iterator_traits instantiates for these iterators
        //  Assert iterator_traits finds the expected typedefs
        typedef bsl::iterator_traits<iter_type>  IterTraits;
        typedef bsl::iterator_traits<const_iter_type>  ConstIterTraits;
        ASSERT((bsl::is_same<IterTraits::difference_type,
                std::ptrdiff_t>::value));
        ASSERT((bsl::is_same<IterTraits::value_type, int>::value));
        ASSERT((bsl::is_same<IterTraits::pointer, int *>::value));
        ASSERT((bsl::is_same<IterTraits::reference, int &>::value));
        ASSERT((bsl::is_same<IterTraits::iterator_category,
                std::forward_iterator_tag>::value));

        ASSERT((bsl::is_same<ConstIterTraits::difference_type,
                std::ptrdiff_t>::value));
        ASSERT((bsl::is_same<ConstIterTraits::value_type, int>::value));
        ASSERT((bsl::is_same<ConstIterTraits::pointer, const int *>::value));
        ASSERT((bsl::is_same<ConstIterTraits::reference, const int&>::value));
        ASSERT((bsl::is_same<ConstIterTraits::iterator_category,
                std::forward_iterator_tag>::value));

        if (verbose) printf("\nPopulate the test list.\n");
        int i;
        for (i = 0; i < DATA_LEN; ++i) {
            if(veryVerbose) { T_ P_(i) P(DATA[i]) }
            a.push(DATA[i]);
        }

        ASSERT(A.begin() == a.begin());
        ASSERT(A.end()   == a.end());

        if (verbose) printf("\nCompare iterated values to original source.\n");
        i = DATA_LEN;
        for (my_List<int>::const_iterator it = A.begin(); it != A.end(); ++it){
            // Note that we assume we can dereference without changing the
            // value of the iterator.  This is a fair assumption for a
            // breathing test but should be checked strictly in the main
            // driver.
            if(veryVerbose) { T_ P_(i) P_(DATA[i]) P(*it) }
            ASSERT(DATA[--i] == *it);
        }

        for (my_List<int>::iterator it = a.begin(); it != a.end(); ) {
            //  Each iteration of the loop shall advance 'it' exactly once.

            // Test 'const_iterator' can be initialized from non-'const'.
            my_List<int>::const_iterator itc = it;
            ASSERT(itc == it);

            // Test assignment from non-'const' to 'const_iterator'.
            ++itc;
            ASSERT(itc != it);
            itc = it;
            ASSERT(itc == it);

            // Test post-increment, including return value
            int val = *it;
            *it++ -= 2;
            ASSERT(*itc++ == (val-2));

            //  Test both post-increments were equivalent
            ASSERT(it == itc);
            if (it != a.end()) {
                ASSERT(*itc == *it);
            }

            // This shouldn't compile:
            // it = itc;
            // my_List<int>::iterator it2 = itc;
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
