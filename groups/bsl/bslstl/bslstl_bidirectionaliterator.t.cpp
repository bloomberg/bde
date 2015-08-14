// bslstl_bidirectionaliterator.t.cpp                                 -*-C++-*-

#include <bslstl_bidirectionaliterator.h>

#include <bslstl_iterator.h>   // for testing only
#include <bslmf_issame.h>
#include <bsls_unspecifiedbool.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <algorithm>  // std::reverse

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bslstl::BidirectionalIterator' is an in-core value-semantic type that that
// adapts a more limited type, which offers a basic set of operations, so that
// the resulting 'bslstl::BidirectionalIterator' object meets all the
// requirements of a standard Bidirectional Iterator.  These requirements are
// spelled out in [forward.iterators], Table 104 - Bidirectional iterator
// requirements.  The primary manipulator of an iterator is the pre-increment
// operator which, together with a function that returns an iterator to the
// start of a sequence, and a second function to return an iterator to the end
// of the same sequence, is sufficient to attain any achievable state.  While
// an iterator offers no direct accessors, its state can be reasonably inferred
// by inspecting the result of the dereference operator.
//
// In order to test this iterator adaptor, a simple container supporting
// forward iterators will be implemented, to provide the basic type to be
// adapted.  This container will use the 'bslstl::BidirectionalIterator'
// template to declare its iterators, as suggested in the usage example.
//
//  SKETCH NOTES FOR A PLAN THAT NEEDS UPDATING
//
// The following is the set of direct accessors (i.e., accessors that have
// direct contact with the physical state of the object):
// - int value() const;
// - ...;
//-----------------------------------------------------------------------------
// [ 2] bslstl::BidirectionalIterator();
// [ 3] bslstl::BidirectionalIterator(IMPL);
// [ 7] bslstl::BidirectionalIterator(const bslstl::BidirectionalIterator&);
// [ 2] ~bslstl::BidirectionalIterator();
// [ 9] bslstl::BidirectionalIterator& operator=(
//                                       const bslstl::BidirectionalIterator&);
// [ 2] bslstl::BidirectionalIterator& operator++();
// [12] bslstl::BidirectionalIterator  operator++(
//                                        bslstl::BidirectionalIterator&, int);
// [11] bslstl::BidirectionalIterator& operator--();
// [12] bslstl::BidirectionalIterator  operator--(
//                                        bslstl::BidirectionalIterator&, int);
// [ 4] T& operator*() const;
// [10] T *operator->() const;
// [ 6] bool operator==(const bslstl::BidirectionalIterator&,
//                      const bslstl::BidirectionalIterator&);
// [ 6] bool operator!=(const bslstl::BidirectionalIterator&,
//                      const bslstl::BidirectionalIterator&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE is informative only, and does not compile independently
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                ALGORITHMS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace
{

template<class Iter>
int testDistance( Iter first, Iter last )
{
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


//  Create an overload set that can determine, using tag dispatch techniques,
//  exactly which iterator tag is declared for this iterator adaptor
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
        Node *d_prev;
        T     d_val;

        Node(const T& val, Node* next)
          : d_next(next), d_prev(), d_val(val)
        {
            if(d_next) {
              d_next->d_prev = this;
            }
        }
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
        void operator--() { d_node = d_node->d_prev; }
    };

    friend class bslstl::BidirectionalIterator<T,IteratorImp>;

    friend class bslstl::BidirectionalIterator<const T,IteratorImp>;

    // Private copy constructor and copy assignment operator.
    // This class does not have value semantics.
    my_List(const my_List&);
    my_List& operator=(const my_List&);

    // PRIVATE DATA
    Node * d_head;
    Node * d_tail;

  public:
    // Declare iterator types:
    typedef bslstl::BidirectionalIterator<T, IteratorImp>       iterator;
    typedef bslstl::BidirectionalIterator<const T, IteratorImp> const_iterator;

    my_List() : d_head(new Node(T(),0)), d_tail(d_head) {}

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
    iterator end()   { return IteratorImp(d_tail); }
    const_iterator begin() const { return IteratorImp(d_head); }
    const_iterator end() const { return IteratorImp(d_tail); }
};

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace testcontainer {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Defining a Standard Compliant Bidirectional Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a standard compliant bidirectional access iterator
// for a container.
//
// First, we define an iterator, 'MyArrayIterator', that meets the requirements
// of the 'IMP_ITER' template parameter of 'BidirectionalIterator' class (see
// class level documentation), but does not meet the full set of requirements
// for a bidirectional iterator as defined by the C++ standard.  Note that the
// following shows only the public interface required.  Private members and
// additional methods that may be needed to implement this class are elided in
// this example:
//..
    template <class VALUE>
    class MyArrayIterator {
        // This class implements the minimal requirements to implement a
        // bidirectional iterator using 'bslstl::BidirectionalIterator'.
//
// *** Remove the following from usage example to emphasis on the interface ***
        // DATA
        VALUE *d_value_p;  // address of an element in an array (held, not
                           // owned)
      private:
        // FRIENDS
        template <class OTHER_VALUE>
        friend bool operator==(const MyArrayIterator<OTHER_VALUE>&,
                               const MyArrayIterator<OTHER_VALUE>&);
//
      public:
        explicit MyArrayIterator(VALUE* address);
            // Create a 'MyArrayIterator' object referring to the
            // element of type 'VALUE' at the specified 'address'.
// ****************************************************************************
      public:
        // CREATORS
        MyArrayIterator();
            // Create a 'MyArrayIterator' object that does not refer to any
            // value.
//
        MyArrayIterator(const MyArrayIterator& original);
            // Create a 'MyArrayIterator' object having the same value
            // as the specified 'original' object.
//
        ~MyArrayIterator();
            // Destroy this object;
//
        // MANIPULATORS
        MyArrayIterator& operator=(const MyArrayIterator& rhs);
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.
//
        void operator++();
            // Increment this object to refer to the next element in an array.
//
        void operator--();
            // Decrement this object to refer to the previous element in an
            // array.
//
        // ACCESSORS
        VALUE& operator*() const;
            // Return a reference providing modifiable access to the value (of
            // the parameterized 'VALUE' type) of the element referred to by
            // this object.
    };
//
    template <class VALUE>
    bool operator==(const MyArrayIterator<VALUE>&,
                    const MyArrayIterator<VALUE>&);
//..
// Notice that 'MyArrayIterator' does not implement a complete standard
// compliant bidirectional iterator.  It is missing methods such as 'operator+'
// and 'operator[]'.
//
// Then, we define the interface for our container class template,
// 'MyFixedSizeArray'.  The implementation of the interface is elided for
// brevity:
//..
    template <class VALUE, int SIZE>
    class MyFixedSizeArray {
        // This class implements a container that contains the parameterized
        // 'SIZE' number of elements of the parameterized 'VALUE' type.
//
        // DATA
        VALUE d_array[SIZE];   // storage of the container
//
      public:
        // PUBLIC TYPES
        typedef VALUE value_type;
//..
// Now, we use 'BidirectionalIterator' to create a standard compliant iterator
// for this container:
//..
        typedef bslstl::BidirectionalIterator<VALUE,
                                             MyArrayIterator<VALUE> > iterator;
        typedef bslstl::BidirectionalIterator<const VALUE,
                                             MyArrayIterator<VALUE> >
                                                                const_iterator;
//..
// Notice that the implementation for 'const_iterator' is
// 'MyArrayIterator<VALUE>' and *not* 'MyArrayIterator<const VALUE>'.
//
// Next, we continue defining the rest of the class.
//..
        // CREATORS
        //! MyFixedSizeArray() = default;
            // Create a 'MyFixedSizeArray' object having the parameterized
            // 'SIZE' number of elements of the parameterized type 'VALUE'.
//
        //! MyFixedSizeArray(const MyFixedSizeArray& original) = default;
            // Create a 'MyFixedSizeArray' object having same number of
            // elements as that of the specified 'original', the same value of
            // each element as that of corresponding element in 'original'.
//
        //! ~MyFixedSizeArray() = default;
            // Destroy this object.
//
        // MANIPULATORS
        iterator begin();
            // Return a bidirectional iterator providing modifiable access to
            // the first valid element of this object.
//
        iterator end();
            // Return a bidirectional iterator providing modifiable access to
            // the last valid element of this object.
//
        VALUE& operator[](int position);
            // Return a reference providing modifiable access to the element at
            // the specified 'position'.
//
        // ACCESSORS
        const_iterator begin() const;
            // Return a bidirectional iterator providing non-modifiable access
            // to the first valid element of this object.
//
        const_iterator end() const;
            // Return a bidirectional iterator providing non-modifiable access
            // to the last valid element of this object.
//
        const VALUE& operator[](int position) const;
            // Return a reference providing non-modifiable access to the
            // specified 'i'th element in this object.
    };
//..

                        // ---------------
                        // MyArrayIterator
                        // ---------------

// CREATORS
template <class VALUE>
MyArrayIterator<VALUE>::MyArrayIterator(VALUE *address)
: d_value_p(address)
{
}

template <class VALUE>
MyArrayIterator<VALUE>::MyArrayIterator()
: d_value_p(0)
{
}

template <class VALUE>
MyArrayIterator<VALUE>::MyArrayIterator(const MyArrayIterator& original)
: d_value_p(original.d_value_p)
{
}

template <class VALUE>
MyArrayIterator<VALUE>& MyArrayIterator<VALUE>::operator=(
                                             const MyArrayIterator<VALUE>& rhs)
{
    d_value_p = rhs.d_value_p;
    return *this;
}

template <class VALUE>
MyArrayIterator<VALUE>::~MyArrayIterator()
{
}


// MANIPULATORS
template <class VALUE>
void MyArrayIterator<VALUE>::operator++()
{
    ++d_value_p;
}

template <class VALUE>
void MyArrayIterator<VALUE>::operator--()
{
    --d_value_p;
}

// ACCESSORS
template <class VALUE>
VALUE& MyArrayIterator<VALUE>::operator*() const
{
    return *d_value_p;
}

// FREE FUNCTIONS
template <class VALUE>
bool operator==(const MyArrayIterator<VALUE>& lhs,
                const MyArrayIterator<VALUE>& rhs)
{
    return lhs.d_value_p == rhs.d_value_p;
}

                        // ----------------
                        // MyFixedSizeArray
                        // ----------------

// MANIPULATORS
template <class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE, SIZE>::iterator
MyFixedSizeArray<VALUE, SIZE>::begin()
{
    return MyArrayIterator<VALUE>(d_array);
}

template <class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE, SIZE>::iterator
MyFixedSizeArray<VALUE, SIZE>::end()
{
    return MyArrayIterator<VALUE>(d_array + SIZE);
}

template <class VALUE, int SIZE>
VALUE& MyFixedSizeArray<VALUE, SIZE>::operator[](int position)
{
    return d_array[position];
}

// ACCESSORS
template <class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE, SIZE>::const_iterator
MyFixedSizeArray<VALUE, SIZE>::begin() const
{
    return MyArrayIterator<VALUE>(d_array);
}

template <class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE, SIZE>::const_iterator
MyFixedSizeArray<VALUE, SIZE>::end() const
{
    return MyArrayIterator<VALUE>(d_array + SIZE);
}

template <class VALUE, int SIZE>
const VALUE& MyFixedSizeArray<VALUE, SIZE>::operator[](int i) const
{
    return d_array[i];
}

}  // close namespace testcontainer

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << ("\nUSAGE EXAMPLE"
                              "\n=============");

        using namespace testcontainer;

// Then, we create a 'MyFixedSizeArray' and initialize its elements:
//..
    MyFixedSizeArray<int, 5> fixedArray;
    fixedArray[0] = 1;
    fixedArray[1] = 2;
    fixedArray[2] = 3;
    fixedArray[3] = 4;
    fixedArray[4] = 5;
//..
// Finally, to show that 'MyFixedSizeArray::iterator' can be used as a
// bidirectional iterator, we invoke a function that takes bidirectional
// iterators as parameters, such as 'std::reverse', on the 'begin' and 'end'
// iterators and verify the results:
//..
    std::reverse(fixedArray.begin(), fixedArray.end());
//
    ASSERT(fixedArray[0] == 5);
    ASSERT(fixedArray[1] == 4);
    ASSERT(fixedArray[2] == 3);
    ASSERT(fixedArray[3] == 2);
    ASSERT(fixedArray[4] == 1);
//..
      } break;
      case 12: {
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
        //   bslstl::BidirectionalIterator& operator++(int);
        //   bslstl::BidirectionalIterator& operator--(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING POST-*CREMENT OPERATOR" << endl
                                  << "==============================" << endl;

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

        if (verbose) cout << "\nConstruct basic iterator values" << endl;
        iterator it1 = testData;
        iterator it2 = it1;
        iterator it3 = it1;
        ++it2;

        if (verbose) cout << "\nvalidate post-increment operator" << endl;
        ASSERT( it3++ == it1);
        ASSERT( it3 == it2);

        if (verbose) cout << "\nvalidate post-decrement operator" << endl;
        ASSERT( it3-- == it2);
        ASSERT( it3 == it1);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DECREMENT OPERATOR
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   bslstl::BidirectionalIterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING DECREMENT OPERATOR" << endl
                                  << "==========================" << endl;

        //  Declare test data and types
        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

        //  Confirm that an iterator over our reference array does not have the
        //  same value as a default constructed iterator.  (Actually, this
        //  comparison is undefined in general.)
        const const_iterator itcOrigin = &testData[3];
        iterator itData = &testData[3];
        ASSERT( itcOrigin == itData);

        //  Confirm that an decremented iterator does not have the same value
        //  as the initial iterator
        --itData;
        ASSERT( itcOrigin != itData);

        //  Confirm that decrementing a second copy of the initial iterator
        //  has the same value as the first incremented iterator.
        const_iterator itcCopy = itcOrigin;
        ASSERT( itcOrigin == itcCopy);

        --itcCopy;
        ASSERT( itcOrigin != itcCopy);
        ASSERT( itData == itcCopy);

        const_iterator itReader = &testData[4];
        const int* itValidator = testData + 4;
        for(int i = 4; i; --itValidator,--itReader, --i) {
            if(4 == i) {
                LOOP_ASSERT(i, &*itReader == itValidator);
            }
            else {
                LOOP3_ASSERT(i, *itReader, *itValidator,
                             &*itReader == itValidator);
            }
        }
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
        //    T *operator->() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OPERATOR->" << endl
                                  << "==================" << endl;

        {
        if (verbose) cout << "\nVerify iterator properties with a"
                             " directly examinable container" << endl;

        //  Declare test data and types
        Wrap testData[2] = { {13}, {99} };
        typedef bslstl::BidirectionalIterator<Wrap, Wrap*> iterator;
        typedef bslstl::BidirectionalIterator<const Wrap, Wrap*>
                                                                const_iterator;

        const iterator itWritable = testData;
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testData;

        ASSERT(13 == itWritable->data);
        ASSERT(13 == itReadable->data);
        ASSERT(&itWritable->data == &testData[0].data);
        ASSERT(&itReadable->data == &testData[0].data);

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
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
        //   bslstl::BidirectionalIterator& operator=(
        //                                const bslstl::BidirectionalIterator&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = v" << endl;

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

        if (verbose) cout << endl << "TESTING VALUE CONSTRUCTOR" << endl
                                  << "=========================" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

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
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONSTRUCTOR" << endl
                                  << "========================" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

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
        //    Constant iterators can be compared with mutable iterators.
        //    Constant and mutable iterators referring to the same element.
        //      shall compare equal.
        //    Iterators that do not refer to the same element shall not compare
        //      equal.
        //    Tests based on the identity of the referenced element must be
        //      valid, even when the referenced element overloads operator&.
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
        //   bool operator==(const bslstl::BidirectionalIterator&,
        //                   const bslstl::BidirectionalIterator&);
        //   bool operator!=(const bslstl::BidirectionalIterator&,
        //                   const bslstl::BidirectionalIterator&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TESTING EQUALITY OPERATOR" << endl
                                  << "=========================" << endl;

        typedef my_List<int> TestContainer;
        typedef TestContainer::iterator iterator;
        typedef TestContainer::const_iterator const_iterator;

        TestContainer testContainer;
        testContainer.push(42);
        testContainer.push(13);

        if (verbose) cout << "\nvalidate self-equality" << endl;
        const iterator itBegin = testContainer.begin();
        ASSERT(itBegin == itBegin);
        ASSERT(!(itBegin != itBegin));
        ASSERT(testContainer.begin() == itBegin);

        if (verbose) cout << "\nvalidate inequality" << endl;
        const iterator itEnd = testContainer.end();
        ASSERT(itBegin != itEnd);
        ASSERT(!(itBegin == itEnd));
        ASSERT(itEnd == itEnd);
        ASSERT(!(itEnd != itEnd));

        if (verbose) cout << "\nvalidate interoperability of types" << endl;
        const const_iterator citBegin = testContainer.begin();
        const_iterator citEnd   = testContainer.end();
        ASSERT(citBegin == itBegin);
        ASSERT(citEnd == itEnd);

        if (verbose) cout << "\nvalidate transition to expected value" << endl;
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
        if (verbose) cout <<"\nThis is trivially satisfied for iterator types."
                            "\nIterators do not support printing or streaming."
                          << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   While an iterator has no named attributes, and so no basic
        //   accessors, the -> operator acts as a basic accessor, returning the
        //
        //
        // Concerns:
        //   Dereferencing an iterator should refer to the expected element.
        //   Must be able to check dereferenced object's identity, even if it
        //     overloads operator&.
        //   Should be able to write through a mutable iterator, and observe
        //     the effect through a constant iterator referring to the same
        //     element.
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

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;


        {
        if (verbose) cout << "\nVerify iterator properties with a"
                             " directly examinable container" << endl;

        //  Declare test data and types
        int testData[1] = { 13 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

        const iterator itWritable = testData;
        // Obtain a fresh copy of an equivalent constant iterator
        const_iterator itReadable = testData;

        ASSERT(13 == *itWritable);
        ASSERT(13 == *itReadable);
        ASSERT(&*itWritable == testData);
        ASSERT(&*itReadable == testData);

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
        *itWritable = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
        ASSERT(42 == *itReadable);
        ASSERT(42 == *itWritable);
        ASSERT(42 == testData[0]);

        testData[0] = 13;

        ASSERT(13 == *itWritable);
        ASSERT(13 == *itReadable);
        ASSERT(&*itWritable == testData);
        ASSERT(&*itReadable == testData);
        }

        {
        if (verbose) cout << "\nRepeat tests with a minimal adapted iterator"
                          << endl;

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

        if (verbose) cout << "\nVerify can observe new value written through"
                          << " the iterator" << endl;
        *itWritable = 42;

        //  Test itReadable before itWritable to be sure the update is not a
        //  side effect of the reading!
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
        //   bslstl::BidirectionalIterator(IMPL);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING (PRIMITIVE) GENERATORS" << endl
                                  << "==============================" << endl;

        if (verbose) cout << "\nValidating primitive test machinery" << endl;

        if (verbose) cout << "\nTesting class my_List<int>" << endl;

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

        if (verbose) cout << "\nCheck iterator range has right length" << endl;
        int length = 0;
        itBegin = testContainer.begin();
        while(itBegin != itEnd) {
            ++length;
            ++itBegin;
        }
        LOOP_ASSERT(length, 3 == length);

        if (verbose) cout << "\nCheck iterators refer to right values" << endl;
        itBegin = testContainer.begin();
        ASSERT(1 == *(itBegin++));
        ASSERT(2 == *(itBegin++));
        ASSERT(3 == *(itBegin++));

        if (verbose) cout << "\nRepeat the tests for const_iterators" << endl;
        const TestContainer& constContainer = testContainer;
        const_iterator itcBegin = testContainer.begin();
        const const_iterator itcEnd = testContainer.end();
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
        //   bslstl::BidirectionalIterator();
        //   ~bslstl::BidirectionalIterator();
        //   bslstl::BidirectionalIterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATORS" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor, destructor, "
                             "and 'operator++'." << endl;

        //  Declare test data and types
        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

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
        //  as the initial iterator
        ++itData;
        ASSERT( itcOrigin != itData);

        //  Confirm that incrementing a second copy of the initial iterator
        //  has the same value as the first incremented iterator.
        const_iterator itcCopy = itcOrigin;
        ASSERT( itcOrigin == itcCopy);

        ++itcCopy;
        ASSERT( itcOrigin != itcCopy);
        ASSERT( itData == itcCopy);

        const_iterator itReader = testData;
        const int* itValidator = testData;
        for(size_t i = 0; i < sizeof(testData)/sizeof(int); ++itValidator,
                                                            ++itReader, ++i) {
            LOOP3_ASSERT(i, *itReader, *itValidator,&*itReader == itValidator);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   before beginning testing in earnest:
        //   - default and copy constructors
        //   - assignment operator
        //   - primary manipulators, basic accessors
        //   - 'operator==', 'operator!='
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        int testData[4] = { 0, 1, 2, 3 };
        typedef bslstl::BidirectionalIterator<int, int*> iterator;
        typedef bslstl::BidirectionalIterator<const int, int*> const_iterator;

        if (verbose) cout << "\nConstruct a basic iterator value" << endl;
        iterator it1 = testData;
        LOOP_ASSERT( *it1, 0 == *it1);

        if (verbose) cout << "\nMake a copy of that iterator" << endl;
        iterator it2 = it1;
        LOOP_ASSERT(  *it2,            0 ==  *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nIncrement an iterator" << endl;
        ++it2;
        LOOP_ASSERT(  *it2,             1 == *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 !=   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 != &*it2);

        //  Increment the other iterator iterator,
        //  verify both iterators have the same value again
        if (verbose) cout << "\nVerify multipass property of forward iterator"
                          << endl;
        ++it1;
        LOOP_ASSERT(  *it1,            1 ==  *it1);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nConstruct a const_iterator" << endl;
        const_iterator itEnd = testData + 4;

        //  Verify const_iterator and iterator interoperate
        //  Verify the expected number of iterations are required to traverse
        //  the array.
        if (verbose) cout << "\nVerify multipass property of forward iterator"
                          << endl;

        const_iterator itBegin = it1;
        int distance = testDistance( itBegin, itEnd );
        LOOP_ASSERT(distance, 3 == distance);

        if (verbose) cout << "\nVerify writing through a dereferenced iterator"
                          << endl;
        *it1 = 42;
        LOOP_ASSERT( *it1, 42 == *it1);

        if (verbose) cout << "\nVerify that writes through one iterator are"
                          << "  visible through another" << endl;
        LOOP_ASSERT( *it2,         42 == *it2);
        LOOP2_ASSERT(*it1, *it2, *it1 == *it2);

        //  To test operator->, create an array of struct values
        Wrap testWrap[4] = { {0}, {1}, {2}, {3} };
        typedef bslstl::BidirectionalIterator<Wrap, Wrap*> wrap_iterator;
        typedef bslstl::BidirectionalIterator<const Wrap, Wrap*>
                                                           const_wrap_iterator;

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

        //  Confirm const_iterator can iterate, just like a non-const_iterator
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

        // Not yet validated constness of any APIs, e.g., 'operator*' and
        // 'operator->' (are constness concerns appropriate for a breathing
        // test, we just want to confirm that each individual API can be
        // called.)

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

        if (verbose) cout << endl << "(ORIGINAL BREATHING TEST)" << endl
                                  << "=========================" << endl;

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
                              std::bidirectional_iterator_tag>::value));

        ASSERT((bsl::is_same<ConstIterTraits::difference_type,
                              std::ptrdiff_t>::value));
        ASSERT((bsl::is_same<ConstIterTraits::value_type,
                              const int>::value));
        ASSERT((bsl::is_same<ConstIterTraits::pointer, const int *>::value));
        ASSERT((bsl::is_same<ConstIterTraits::reference,
                              const int &>::value));
        ASSERT((bsl::is_same<ConstIterTraits::iterator_category,
                              std::bidirectional_iterator_tag>::value));

        if (verbose) cout << "\nPopulate the test list." << std::endl;
        int i;
        for (i = 0; i < DATA_LEN; ++i) {
            if(veryVerbose) { T_ P_(i) P(DATA[i]) }
            a.push(DATA[i]);
        }

        ASSERT(A.begin() == a.begin());
        ASSERT(A.end()   == a.end());

        if (verbose) cout << "\nCompare iterated values to original source."
                          << std::endl;
        i = 4;
        for (my_List<int>::const_iterator it = A.begin(); it != A.end(); ++it,
                                                                          --i){
            // Note that we assume we can dereference without changing the
            // value of the iterator.  This is a fair assumption for a
            // breathing test but should be checked strictly in the main
            // driver.
            if(veryVerbose) { T_ P_(i) P_(DATA[i]) P(*it) }
            LOOP3_ASSERT(i, DATA[i], *it, DATA[i] == *it);
        }

        if (verbose) cout << "\nCompare in reversed iteration order."
                          << std::endl;
        for (my_List<int>::const_iterator it = A.end(); it != A.begin();){
            // Note that we will be decremented inside the body of the loop.
            // Note that we assume we can dereference without changing the
            // value of the iterator.  This is a fair assumption for a
            // breathing test but should be checked strictly in the main
            // driver.
            --it;
            ++i;
            if(veryVerbose) { T_ P_(i) P_(DATA[i]) P(*it) }
            LOOP3_ASSERT(i, DATA[i], *it, DATA[i] == *it);
        }

        for (my_List<int>::iterator it = a.begin(); it != a.end(); ) {
            //  Each iteration of the loop shall advance 'it' exactly once.

            // Test that initialization from non-const to const iterator works.
            my_List<int>::const_iterator itc = it;
            ASSERT(itc == it);

            // Test assignment from non-const to const iterators
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
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
