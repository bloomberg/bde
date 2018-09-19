// bslstl_iterator.t.cpp                                              -*-C++-*-

#include <bslstl_iterator.h>

#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>

#include <bslstl_set.h>

#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bsl::reverse_iterator' is an iterator adaptor (in-core value-semantic type)
// that adapts a more limited type, which offers a basic set of operations, so
// that the resulting 'bsl::reverse_iterator' object meets all the requirements
// of a standard reverse iterator.  These requirements are spelled out in
// [reverse.iter.requirements] (24.5.1.2), reverse_iterator requirements.  The
// primary manipulator of a reverse iterator is the pre-increment operator
// which, together with a function that returns an iterator to the start of a
// sequence, and a second function to return an iterator to the end of the same
// sequence, is sufficient to attain any achievable state.  The primary
// accessor is the dereference method from which the state can be reasonably
// inferred by inspecting the result.
//
// In order to test this iterator adaptor, a simple container supporting
// reverse iterators will be implemented, to provide the basic type to be
// adapted.  This container will use the 'bsl::reverse_iterator' template to
// declare its iterators, as suggested in the usage example.
//-----------------------------------------------------------------------------
// CLASS 'bsl::reverse_iterator'
//
// CREATORS
// [ 2] bsl::reverse_iterator();
// [ 2] bsl::reverse_iterator(ITER x);
// [ 7] bsl::reverse_iterator(const bsl::reverse_iterator&);
//
// MANIPULATORS
// [ 2] bsl::reverse_iterator& operator++();
// [11] bsl::reverse_iterator  operator++(int);
// [11] bsl::reverse_iterator& operator+=(typename difference_type n);
// [11] bsl::reverse_iterator& operator--();
// [11] bsl::reverse_iterator  operator--(int);
// [11] bsl::reverse_iterator& operator-=(typename difference_type n);
//
// ACCESSORS
// [12] operator+(typename difference_type n) const;
// [12] operator-(typename difference_type n) const;
//
// FREE FUNCTIONS
// [ 6] bool operator==(const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [ 6] bool operator!=(const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator< (const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator> (const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator<=(const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator>=(const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator- (const bsl::reverse_iterator&, bsl::reverse_iterator&);
// [13] bool operator+ (typename difference_type, bsl::reverse_iterator&);
//
// other functions
// [13] distance(ITER, ITER);
// [14] T::iterator begin(T& container);
// [14] T::const_iterator begin(const T& container);
// [14] T *begin(T (&array)[N]);
// [14] T::iterator end(T& container);
// [14] T::const_iterator end(const T& container);
// [14] T *end(T (&array)[N]);
// [14] T::const_iterator cbegin(const T& container);
// [14] T::reverse_iterator rbegin(T& container);
// [14] T::const_reverse_iterator rbegin(const T& container);
// [14] reverse_iterator<T *> rbegin(T (&array)[N]);
// [14] reverse_iterator<const T *> rbegin(std::initializer_list<T> il);
// [14] T::const_reverse_iterator crbegin(const T& container);
// [14] T::const_iterator cend(const T& container);
// [14] T::reverse_iterator rend(T& container);
// [14] T::const_reverse_iterator rend(const T& container);
// [14] reverse_iterator<T *> rend(T (&array)[N]);
// [14] reverse_iterator<const T *> rend(std::initializer_list<T> il);
// [14] T::const_reverse_iterator crend(const T& container);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING (PRIMITIVE) GENERATORS
// [ 4] TESTING BASIC ACCESSORS:     Not Applicable
// [ 5] TESTING OUTPUT:              Not Applicable
// [ 8] TESTING SWAP:                Not Applicable
// [ 9] TESTING ASSIGNMENT OPERATOR: Not Applicable
// [10] STREAMING FUNCTIONALITY:     Not Applicable
// [15] CONCERN: Range functions are not ambiguous with 'std' under ADL
// [16] USAGE EXAMPLE
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03 1
#endif
//=============================================================================
//                  TESTING APPARATUS/CLASS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace testcontainer {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Using Iterators to Traverse a Container
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use the 'bsl::iterator' and 'bsl::reverse_iterator'
// to traverse an iterable container type.
//
// Suppose that we have an iterable container template type 'MyFixedSizeArray'.
// An instantiation of 'MyFixedSizeArray' represents an array having fixed
// number of elements, which is a parameter passed to the class constructor
// during construction.  A traversal of 'MyFixedSizeArray' can be accomplished
// using basic iterators (pointers) as well as reverse iterators.
//
// First, we create a elided definition of the template container class,
// 'MyFixedSizeArray', which provides mutable and constant iterators of
// template type 'bsl::iterator' and 'reverse_iterator':
//..
template <class VALUE, int SIZE>
class MyFixedSizeArray
    // This is a container that contains a fixed number of elements.  The
    // number of elements is specified upon construction and can not be
    // changed afterwards.
{
    // DATA
    VALUE  d_array[SIZE];  // storage of the container

  public:
    // PUBLIC TYPES
    typedef VALUE value_type;
//..
// Here, we define mutable and constant iterators and reverse iterators:
//..
    typedef VALUE                                  *iterator;
    typedef VALUE const                            *const_iterator;
    typedef bsl::reverse_iterator<iterator>         reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>   const_reverse_iterator;

    // CREATORS
    //! MyFixedSizeArray() = default;
        // Create a 'MyFixedSizeArray' object having the parameterized
        // 'SIZE' elements of the parameterized type 'VALUE'.

    //! MyFixedSizeArray(const MyFixedSizeArray& original) = default;
        // Create a 'MyFixedSizeArray' object having same number of
        // elements as that of the specified 'rhs', and the same value of
        // each element as that of corresponding element in 'rhs'.

    //! ~MyFixedSizeArray() = default;
        // Destroy this object.
//..
// Now, we define the 'begin' and 'end' methods to return basic iterators
// ('VALUE*' and 'const VALUE*'), and the 'rbegin' and 'rend' methods to return
// reverse iterators ('bsl::reverse_iterator<VALUE*>' and
// 'bsl::reverse_iterator<const VALUE*>) type:
//..
    // MANIPULATORS
    iterator begin();
        // Return the basic iterator providing modifiable access to the
        // first valid element of this object.

    iterator end();
        // Return the basic iterator providing modifiable access to the
        // position one after the last valid element of this object.

    reverse_iterator rbegin();
        // Return the reverse iterator providing modifiable access to the
        // last valid element of this object.

    reverse_iterator rend();
        // Return the reverse iterator providing modifiable access to the
        // position one before the first valid element of this object.

    VALUE& operator[](int i);
        // Return the reference providing modifiable access of the
        // specified 'i'th element of this object.

    // ACCESSORS
    const_iterator begin() const;
        // Return the basic iterator providing non-modifiable access to the
        // first valid element of this object.

    const_iterator end() const;
        // Return the basic iterator providing non-modifiable access to the
        // position one after the last valid element of this object.

    const_reverse_iterator rbegin() const;
        // Return the reverse iterator providing non-modifiable access to
        // the last valid element of this object.

    const_reverse_iterator rend() const;
        // Return the reverse iterator providing non-modifiable access to
        // the position one before the first valid element of this object.

    int size() const;
        // Return the number of elements contained in this object.

    const VALUE& operator[](int i) const;
        // Return the reference providing non-modifiable access of the
        // specified 'i'th element of this object.
};

// ...
//..

                            // ----------------
                            // MyFixedSizeArray
                            // ----------------

// MANIPULATORS
template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::iterator
MyFixedSizeArray<VALUE,SIZE>::begin()
{
    return d_array;
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::iterator
MyFixedSizeArray<VALUE,SIZE>::end()
{
    return d_array + SIZE;
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::reverse_iterator
MyFixedSizeArray<VALUE,SIZE>::rbegin()
{
    return reverse_iterator(end());
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::reverse_iterator
MyFixedSizeArray<VALUE,SIZE>::rend()
{
    return reverse_iterator(begin());
}

template<class VALUE, int SIZE>
inline VALUE& MyFixedSizeArray<VALUE,SIZE>::operator[](int i)
{
    return d_array[i];
}

// ACCESSORS
template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::const_iterator
MyFixedSizeArray<VALUE,SIZE>::begin() const
{
    return d_array;
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::const_iterator
MyFixedSizeArray<VALUE,SIZE>::end() const
{
    return d_array + SIZE;
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::const_reverse_iterator
MyFixedSizeArray<VALUE,SIZE>::rbegin() const
{
    return const_reverse_iterator(end());
}

template<class VALUE, int SIZE>
typename MyFixedSizeArray<VALUE,SIZE>::const_reverse_iterator
MyFixedSizeArray<VALUE,SIZE>::rend() const
{
    return const_reverse_iterator(begin());
}

template<class VALUE, int SIZE>
inline int MyFixedSizeArray<VALUE,SIZE>::size() const
{
    return SIZE;
}

template<class VALUE, int SIZE>
inline
const VALUE& MyFixedSizeArray<VALUE,SIZE>::operator[](int i) const
{
    return d_array[i];
}

// FREE FUNCTIONS
template<class VALUE, int SIZE>
bool operator==(const MyFixedSizeArray<VALUE,SIZE>& lhs,
                const MyFixedSizeArray<VALUE,SIZE>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }
    for (int i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;                                             // RETURN
        }
    }
    return true;
}

class AccessTestContainer
    // This is a container that contains a fixed number of elements.  The
    // number of elements is specified upon construction and can not be
    // changed afterwards.
{
    // TYPES
    enum {
        e_BEGIN               = 1,
        e_CONST_BEGIN         = 2,
        e_REVERSE_BEGIN       = 4,
        e_CONST_REVERSE_BEGIN = 8,
        e_END                 = 16,
        e_CONST_END           = 32,
        e_REVERSE_END         = 64,
        e_CONST_REVERSE_END   = 128
    };

    // DATA
    int         d_array[2];        // storage of the container
    mutable int d_functionCalled;  //

  public:
    // PUBLIC TYPES
    typedef int                                   *iterator;
    typedef int const                             *const_iterator;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

    // CREATORS
    AccessTestContainer();
        // Create a 'AccessTestContainer' object.

    // MANIPULATORS
    iterator begin();
        // Return the basic iterator providing modifiable access to the
        // first valid element of this object.

    iterator end();
        // Return the basic iterator providing modifiable access to the
        // position one after the last valid element of this object.

    reverse_iterator rbegin();
        // Return the reverse iterator providing modifiable access to the
        // last valid element of this object.

    reverse_iterator rend();
        // Return the reverse iterator providing modifiable access to the
        // position one before the first valid element of this object.


    // ACCESSORS
    const_iterator begin() const;
        // Return the basic iterator providing non-modifiable access to the
        // first valid element of this object.

    const_iterator end() const;
        // Return the basic iterator providing non-modifiable access to the
        // position one after the last valid element of this object.

    const_reverse_iterator rbegin() const;
        // Return the reverse iterator providing non-modifiable access to
        // the last valid element of this object.

    const_reverse_iterator rend() const;
        // Return the reverse iterator providing non-modifiable access to
        // the position one before the first valid element of this object.

    int functionCalled() const;
    bool beginCalled() const;
    bool constBeginCalled() const;
    bool reverseBeginCalled() const;
    bool constReverseBeginCalled() const;
    bool endCalled() const;
    bool constEndCalled() const;
    bool reverseEndCalled() const;
    bool constReverseEndCalled() const;
};

                            // -------------------
                            // AccessTestContainer
                            // -------------------
// CREATORS
AccessTestContainer::AccessTestContainer()
: d_functionCalled(0)
{
    d_array[0] = 0;
    d_array[1] = 1;
}

// MANIPULATORS
AccessTestContainer::iterator
AccessTestContainer::begin()
{
    d_functionCalled |= e_BEGIN;
    return d_array;
}

AccessTestContainer::iterator
AccessTestContainer::end()
{
    d_functionCalled |= e_END;
    return d_array + 1;
}

AccessTestContainer::reverse_iterator
AccessTestContainer::rbegin()
{
    d_functionCalled |= e_REVERSE_BEGIN;
    return reverse_iterator(end());
}

AccessTestContainer::reverse_iterator
AccessTestContainer::rend()
{
    d_functionCalled |= e_REVERSE_END;
    return reverse_iterator(begin());
}

// ACCESSORS
AccessTestContainer::const_iterator
AccessTestContainer::begin() const
{
    d_functionCalled |= e_CONST_BEGIN;
    return d_array;
}

AccessTestContainer::const_iterator
AccessTestContainer::end() const
{
    d_functionCalled |= e_CONST_END;
    return d_array + 1;
}

AccessTestContainer::const_reverse_iterator
AccessTestContainer::rbegin() const
{
    d_functionCalled |= e_CONST_REVERSE_BEGIN;
    return const_reverse_iterator(end());
}

AccessTestContainer::const_reverse_iterator
AccessTestContainer::rend() const
{
    d_functionCalled |= e_CONST_REVERSE_END;
    return const_reverse_iterator(begin());
}

int AccessTestContainer::functionCalled() const
{
    return d_functionCalled;
}

bool AccessTestContainer::beginCalled() const
{
    return (e_BEGIN == d_functionCalled);
}

bool AccessTestContainer::constBeginCalled() const
{
    return (e_CONST_BEGIN == d_functionCalled);
}

bool AccessTestContainer::reverseBeginCalled() const
{
    return ((e_REVERSE_BEGIN | e_END) ==
                               (d_functionCalled & (e_REVERSE_BEGIN | e_END)));
}

bool AccessTestContainer::constReverseBeginCalled() const
{
    return ((e_CONST_REVERSE_BEGIN | e_CONST_END) ==
                   (d_functionCalled & (e_CONST_REVERSE_BEGIN | e_CONST_END)));
}

bool AccessTestContainer::endCalled() const
{
    return (e_END == d_functionCalled);
}

bool AccessTestContainer::constEndCalled() const
{
    return (e_CONST_END == d_functionCalled);
}

bool AccessTestContainer::reverseEndCalled() const
{
    return ((e_REVERSE_END | e_BEGIN) ==
                               (d_functionCalled & (e_REVERSE_END | e_BEGIN)));
}

bool AccessTestContainer::constReverseEndCalled() const
{
    return ((e_CONST_REVERSE_END | e_CONST_BEGIN) ==
                   (d_functionCalled & (e_CONST_REVERSE_END | e_CONST_BEGIN)));
}

}  // close namespace testcontainer

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
namespace {

// The following functions are used solely to verify that the value returned
// from the range functions has expected type.

template<class T>
bool isConstIterator(typename T::iterator)
{
    return false;
}

template<class T>
bool isConstIterator(typename T::const_iterator)
{
    return true;
}

template<class T>
bool isConstReverseIterator(typename T::reverse_iterator)
{
    return false;
}

template<class T>
bool isConstReverseIterator(typename T::const_reverse_iterator)
{
    return true;
}

}  // close unnamed namespace

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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============");

        using namespace testcontainer;

// Then, we create a 'MyFixedSizeArray' and initialize its elements:
//..
    // Create a fixed array having five elements.

    MyFixedSizeArray<int, 5> fixedArray;

    // Initialize the values of each element in the fixed array.

    for (int i = 0; i < fixedArray.size(); ++i) {
        fixedArray[i] = i + 1;
    }
//..
// Now, we generate reverse iterators using the 'rbegin' and 'rend' methods of
// the fixed array object:
//..
    MyFixedSizeArray<int, 5>::reverse_iterator rstart  = fixedArray.rbegin();
    MyFixedSizeArray<int, 5>::reverse_iterator rfinish = fixedArray.rend();
//..
// Finally, we traverse the fixed array again in reverse order using the two
// generated reverse iterators:
//..
    if (veryVerbose) {
        printf("Traverse array using reverse iterator:\n");
        while (rstart != rfinish) {
            printf("\tElement: %d\n", *rstart);
            ++rstart;
        }
    }
//..
// The preceding loop produces the following output on 'stdout':
//..
//  Traverse array using reverse iterator:
//       Element: 5
//       Element: 4
//       Element: 3
//       Element: 2
//       Element: 1
//..
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ADL CONCERN
        //
        // Concerns:
        //: 1 Range functions can be used with 'std' containers under ADL.
        //
        // Plan:
        //: 1 Call all 8 range functions (unqualified) for a 'bsl::set<int>',
        //:   as this will be associated with both namespace 'bsl' and native
        //:   'std' (for 'std::less' as a template parameter).  Note that this
        //:   test scenario is implemented in the test driver of the
        //:   'bslim_bslstandardheadertest' component to avoid include loop.
        //:
        //: 2 Explicitly introduce both namespaces ('bsl' and 'native_std') and
        //:   call all 8 range functions for an array of integers.  (C-1)
        //
        // Testing
        //   CONCERN: Range functions are not ambiguous with 'std' under ADL
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ADL CONCERN"
                            "\n===================\n");

        using namespace bsl;
        using namespace native_std;

        int mX[] = {1, 2, 3, 4, 5};

        (void)begin(mX);
        (void)end(mX);
        (void)rbegin(mX);
        (void)rend(mX);
        (void)cbegin(mX);
        (void)cend(mX);
        (void)crbegin(mX);
        (void)crend(mX);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ACCESS FUNCTIONS
        //   Most of the functions (except accepting arrays ones) just transfer
        //   requests to the corresponding methods of the container class. So
        //   we are going to check not only the return values, but also that
        //   expected methods have been called.
        //
        // Concerns:
        //: 1 The access functions call appropriate methods of the passed
        //:   object.
        //:
        //: 2 The access functions correctly transfer return values obtained
        //:   from the methods of the passed object.
        //
        // Plan:
        //: 1 Using object of the class 'AccessTestContainer' call an access
        //:   function and check that appropriate methods of the special class
        //:   have been called.  (C-1)
        //:
        //: 2 Compare return value of an access function with the value
        //:   obtained from direct call of the appropriate container method.
        //:
        //: 3 Using arrays and initializer lists check that access functions
        //:   return expected results.  (C-2)
        //
        // Testing
        //   T::iterator begin(T& container);
        //   T::const_iterator begin(const T& container);
        //   T *begin(T (&array)[N]);
        //   T::iterator end(T& container);
        //   T::const_iterator end(const T& container);
        //   T *end(T (&array)[N]);
        //   T::const_iterator cbegin(const T& container);
        //   T::reverse_iterator rbegin(T& container);
        //   T::const_reverse_iterator rbegin(const T& container);
        //   reverse_iterator<T *> rbegin(T (&array)[N]);
        //   reverse_iterator<const T *> rbegin(std::initializer_list<T> il);
        //   T::const_reverse_iterator crbegin(const T& container);
        //   T::const_iterator cend(const T& container);
        //   T::reverse_iterator rend(T& container);
        //   T::const_reverse_iterator rend(const T& container);
        //   reverse_iterator<T *> rend(T (&array)[N]);
        //   reverse_iterator<const T *> rend(std::initializer_list<T> il);
        //   T::const_reverse_iterator crend(const T& container);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESS FUNCTIONS"
                            "\n========================\n");

        if (verbose) printf("\tTest container class.\n");
        {
            typedef testcontainer::AccessTestContainer Container;

            typedef Container::iterator                iterator;
            typedef Container::const_iterator          const_iterator;
            typedef Container::const_iterator          const_iterator;
            typedef Container::reverse_iterator        reverse_iterator;
            typedef Container::const_reverse_iterator  const_reverse_iterator;

                  Container mXB;
            const Container  XB;
            const Container  XCB;
                  Container mXRB;
            const Container  XRB;
            const Container  XCRB;

                  Container mXE;
            const Container  XE;
            const Container  XCE;
                  Container mXRE;
            const Container  XRE;
            const Container  XCRE;

#ifndef BSLS_PLATFORM_CMP_SUN
            // Sun compiler is not able to distinguish
            // isConstIterator<Container>(const int*) from
            // isConstIterator<Container>(int*) or
            // isConstReverseIterator<Cont>(bsl::reverse_iterator<const int*>)
            // from isConstReverseIterator<Cont>(bsl::reverse_iterator<int*>),
            // so the following test scenarios are excluded for it.

            ASSERT(!isConstIterator       <Container>((bsl::begin  (mXB  ))));
            ASSERT( isConstIterator       <Container>((bsl::begin  ( XB  ))));
            ASSERT( isConstIterator       <Container>((bsl::cbegin ( XCB ))));
            ASSERT(!isConstReverseIterator<Container>((bsl::rbegin (mXRB ))));
            ASSERT( isConstReverseIterator<Container>((bsl::rbegin ( XRB ))));
            ASSERT( isConstReverseIterator<Container>((bsl::crbegin( XCRB))));

            ASSERT(!isConstIterator       <Container>((bsl::end  (mXE  ))));
            ASSERT( isConstIterator       <Container>((bsl::end  ( XE  ))));
            ASSERT( isConstIterator       <Container>((bsl::cend ( XCE ))));
            ASSERT(!isConstReverseIterator<Container>((bsl::rend (mXRE ))));
            ASSERT( isConstReverseIterator<Container>((bsl::rend ( XRE ))));
            ASSERT( isConstReverseIterator<Container>((bsl::crend( XCRE))));
#endif

            iterator               mXBIt   = bsl::begin  (mXB  );
            const_iterator          XBIt   = bsl::begin  ( XB  );
            const_iterator          XCBIt  = bsl::cbegin ( XCB );
            reverse_iterator       mXRBIt  = bsl::rbegin (mXRB );
            const_reverse_iterator  XRBIt  = bsl::rbegin ( XRB );
            const_reverse_iterator  XCRBIt = bsl::crbegin( XCRB);

            iterator               mXEIt   = bsl::end  (mXE  );
            const_iterator          XEIt   = bsl::end  ( XE  );
            const_iterator          XCEIt  = bsl::cend ( XCE );
            reverse_iterator       mXREIt  = bsl::rend (mXRE );
            const_reverse_iterator  XREIt  = bsl::rend ( XRE );
            const_reverse_iterator  XCREIt = bsl::crend( XCRE);

            ASSERTV(mXB.functionCalled(),   mXB.beginCalled()              );
            ASSERTV( XB.functionCalled(),    XB.constBeginCalled()         );
            ASSERTV( XCB.functionCalled(),   XCB.constBeginCalled()        );
            ASSERTV(mXRB.functionCalled(),  mXRB.reverseBeginCalled()      );
            ASSERTV( XRB.functionCalled(),   XRB.constReverseBeginCalled() );
            ASSERTV( XCRB.functionCalled(),  XCRB.constReverseBeginCalled());

            ASSERTV(mXE.functionCalled(),   mXE.endCalled()              );
            ASSERTV( XE.functionCalled(),    XE.constEndCalled()         );
            ASSERTV( XCE.functionCalled(),   XCE.constEndCalled()        );
            ASSERTV(mXRE.functionCalled(),  mXRE.reverseEndCalled()      );
            ASSERTV( XRE.functionCalled(),   XRE.constReverseEndCalled() );
            ASSERTV( XCRE.functionCalled(),  XCRE.constReverseEndCalled());

            ASSERT(mXB.begin()    == mXBIt  );
            ASSERT( XB.begin()    ==  XBIt  );
            ASSERT( XCB.begin()   ==  XCBIt );
            ASSERT(mXRB.rbegin()  == mXRBIt );
            ASSERT( XRB.rbegin()  ==  XRBIt );
            ASSERT( XCRB.rbegin() ==  XCRBIt);

            ASSERT(mXE.end()    == mXEIt);
            ASSERT( XE.end()    ==  XEIt);
            ASSERT( XCE.end()   ==  XCEIt );
            ASSERT(mXRE.rend()  == mXREIt );
            ASSERT( XRE.rend()  ==  XREIt );
            ASSERT( XCRE.rend() ==  XCREIt);
        }

        if (verbose) printf("\tTest arrays.\n");
        {
                  int mX1[] = {1};
            const int  X1[] = {1};
                  int mX2[] = {1, 2};
            const int  X2[] = {1, 2};
                  int mX3[] = {1, 2, 3};
            const int  X3[] = {1, 2, 3};
                  int mX4[] = {1, 2, 3, 4};
            const int  X4[] = {1, 2, 3, 4};
                  int mX5[] = {1, 2, 3, 4, 5};
            const int  X5[] = {1, 2, 3, 4, 5};

            // Testing 'begin' and 'end'.

                  int *mXBIt1 = bsl::begin(mX1);
            const int  *XBIt1 = bsl::begin( X1);
                  int *mXEIt1 = bsl::end  (mX1);
            const int  *XEIt1 = bsl::end  ( X1);
                  int *mXBIt2 = bsl::begin(mX2);
            const int  *XBIt2 = bsl::begin( X2);
                  int *mXEIt2 = bsl::end  (mX2);
            const int  *XEIt2 = bsl::end  ( X2);
                  int *mXBIt3 = bsl::begin(mX3);
            const int  *XBIt3 = bsl::begin( X3);
                  int *mXEIt3 = bsl::end  (mX3);
            const int  *XEIt3 = bsl::end  ( X3);
                  int *mXBIt4 = bsl::begin(mX4);
            const int  *XBIt4 = bsl::begin( X4);
                  int *mXEIt4 = bsl::end  (mX4);
            const int  *XEIt4 = bsl::end  ( X4);
                  int *mXBIt5 = bsl::begin(mX5);
            const int  *XBIt5 = bsl::begin( X5);
                  int *mXEIt5 = bsl::end  (mX5);
            const int  *XEIt5 = bsl::end  ( X5);

            // Moving pointers to the last element.

            --mXEIt1;
            --XEIt1;
            --mXEIt2;
            --XEIt2;
            --mXEIt3;
            --XEIt3;
            --mXEIt4;
            --XEIt4;
            --mXEIt5;
            --XEIt5;

            ASSERTV(mX1[0], *mXBIt1, mX1 + 0 == mXBIt1);
            ASSERTV( X1[0],  *XBIt1,  X1 + 0 ==  XBIt1);
            ASSERTV(mX1[0], *mXEIt1, mX1 + 0 == mXEIt1);
            ASSERTV( X1[0],  *XEIt1,  X1 + 0 ==  XEIt1);
            ASSERTV(mX2[0], *mXBIt2, mX2 + 0 == mXBIt2);
            ASSERTV( X2[0],  *XBIt2,  X2 + 0 ==  XBIt2);
            ASSERTV(mX2[1], *mXEIt2, mX2 + 1 == mXEIt2);
            ASSERTV( X2[1],  *XEIt2,  X2 + 1 ==  XEIt2);
            ASSERTV(mX3[0], *mXBIt3, mX3 + 0 == mXBIt3);
            ASSERTV( X3[0],  *XBIt3,  X3 + 0 ==  XBIt3);
            ASSERTV(mX3[2], *mXEIt3, mX3 + 2 == mXEIt3);
            ASSERTV( X3[2],  *XEIt3,  X3 + 2 ==  XEIt3);
            ASSERTV(mX4[0], *mXBIt4, mX4 + 0 == mXBIt4);
            ASSERTV( X4[0],  *XBIt4,  X4 + 0 ==  XBIt4);
            ASSERTV(mX4[3], *mXEIt4, mX4 + 3 == mXEIt4);
            ASSERTV( X4[3],  *XEIt4,  X4 + 3 ==  XEIt4);
            ASSERTV(mX5[0], *mXBIt5, mX5 + 0 == mXBIt5);
            ASSERTV( X5[0],  *XBIt5,  X5 + 0 ==  XBIt5);
            ASSERTV(mX5[4], *mXEIt5, mX5 + 4 == mXEIt5);
            ASSERTV( X5[4],  *XEIt5,  X5 + 4 ==  XEIt5);

            // Testing 'cbegin' and 'cend'.
            const int *mXCBIt1 = bsl::cbegin(mX1);
            const int  *XCBIt1 = bsl::cbegin( X1);
            const int *mXCEIt1 = bsl::cend  (mX1);
            const int  *XCEIt1 = bsl::cend  ( X1);
            const int *mXCBIt2 = bsl::cbegin(mX2);
            const int  *XCBIt2 = bsl::cbegin( X2);
            const int *mXCEIt2 = bsl::cend  (mX2);
            const int  *XCEIt2 = bsl::cend  ( X2);
            const int *mXCBIt3 = bsl::cbegin(mX3);
            const int  *XCBIt3 = bsl::cbegin( X3);
            const int *mXCEIt3 = bsl::cend  (mX3);
            const int  *XCEIt3 = bsl::cend  ( X3);
            const int *mXCBIt4 = bsl::cbegin(mX4);
            const int  *XCBIt4 = bsl::cbegin( X4);
            const int *mXCEIt4 = bsl::cend  (mX4);
            const int  *XCEIt4 = bsl::cend  ( X4);
            const int *mXCBIt5 = bsl::cbegin(mX5);
            const int  *XCBIt5 = bsl::cbegin( X5);
            const int *mXCEIt5 = bsl::cend  (mX5);
            const int  *XCEIt5 = bsl::cend  ( X5);

            // Moving pointers to the last element.

            --mXCEIt1;
            --XCEIt1;
            --mXCEIt2;
            --XCEIt2;
            --mXCEIt3;
            --XCEIt3;
            --mXCEIt4;
            --XCEIt4;
            --mXCEIt5;
            --XCEIt5;

            ASSERTV(mX1[0], *mXCBIt1, mX1 + 0 == mXCBIt1);
            ASSERTV( X1[0],  *XCBIt1,  X1 + 0 ==  XCBIt1);
            ASSERTV(mX1[0], *mXCEIt1, mX1 + 0 == mXCEIt1);
            ASSERTV( X1[0],  *XCEIt1,  X1 + 0 ==  XCEIt1);
            ASSERTV(mX2[0], *mXCBIt2, mX2 + 0 == mXCBIt2);
            ASSERTV( X2[0],  *XCBIt2,  X2 + 0 ==  XCBIt2);
            ASSERTV(mX2[1], *mXCEIt2, mX2 + 1 == mXCEIt2);
            ASSERTV( X2[1],  *XCEIt2,  X2 + 1 ==  XCEIt2);
            ASSERTV(mX3[0], *mXCBIt3, mX3 + 0 == mXCBIt3);
            ASSERTV( X3[0],  *XCBIt3,  X3 + 0 ==  XCBIt3);
            ASSERTV(mX3[2], *mXCEIt3, mX3 + 2 == mXCEIt3);
            ASSERTV( X3[2],  *XCEIt3,  X3 + 2 ==  XCEIt3);
            ASSERTV(mX4[0], *mXCBIt4, mX4 + 0 == mXCBIt4);
            ASSERTV( X4[0],  *XCBIt4,  X4 + 0 ==  XCBIt4);
            ASSERTV(mX4[3], *mXCEIt4, mX4 + 3 == mXCEIt4);
            ASSERTV( X4[3],  *XCEIt4,  X4 + 3 ==  XCEIt4);
            ASSERTV(mX5[0], *mXCBIt5, mX5 + 0 == mXCBIt5);
            ASSERTV( X5[0],  *XCBIt5,  X5 + 0 ==  XCBIt5);
            ASSERTV(mX5[4], *mXCEIt5, mX5 + 4 == mXCEIt5);
            ASSERTV( X5[4],  *XCEIt5,  X5 + 4 ==  XCEIt5);

            // Testing 'rbegin' and 'rend'.

            bsl::reverse_iterator<      int *> mXRBIt1 = bsl::rbegin(mX1);
            bsl::reverse_iterator<const int *>  XRBIt1 = bsl::rbegin( X1);
            bsl::reverse_iterator<      int *> mXREIt1 = bsl::rend  (mX1);
            bsl::reverse_iterator<const int *>  XREIt1 = bsl::rend  ( X1);
            bsl::reverse_iterator<      int *> mXRBIt2 = bsl::rbegin(mX2);
            bsl::reverse_iterator<const int *>  XRBIt2 = bsl::rbegin( X2);
            bsl::reverse_iterator<      int *> mXREIt2 = bsl::rend  (mX2);
            bsl::reverse_iterator<const int *>  XREIt2 = bsl::rend  ( X2);
            bsl::reverse_iterator<      int *> mXRBIt3 = bsl::rbegin(mX3);
            bsl::reverse_iterator<const int *>  XRBIt3 = bsl::rbegin( X3);
            bsl::reverse_iterator<      int *> mXREIt3 = bsl::rend  (mX3);
            bsl::reverse_iterator<const int *>  XREIt3 = bsl::rend  ( X3);
            bsl::reverse_iterator<      int *> mXRBIt4 = bsl::rbegin(mX4);
            bsl::reverse_iterator<const int *>  XRBIt4 = bsl::rbegin( X4);
            bsl::reverse_iterator<      int *> mXREIt4 = bsl::rend  (mX4);
            bsl::reverse_iterator<const int *>  XREIt4 = bsl::rend  ( X4);
            bsl::reverse_iterator<      int *> mXRBIt5 = bsl::rbegin(mX5);
            bsl::reverse_iterator<const int *>  XRBIt5 = bsl::rbegin( X5);
            bsl::reverse_iterator<      int *> mXREIt5 = bsl::rend  (mX5);
            bsl::reverse_iterator<const int *>  XREIt5 = bsl::rend  ( X5);

            // Moving iterators to the first element.

            --mXREIt1;
            --XREIt1;
            --mXREIt2;
            --XREIt2;
            --mXREIt3;
            --XREIt3;
            --mXREIt4;
            --XREIt4;
            --mXREIt5;
            --XREIt5;

            ASSERTV(mX1[0], *mXRBIt1, mX1[0] == *mXRBIt1);
            ASSERTV( X1[0],  *XRBIt1,  X1[0] ==  *XRBIt1);
            ASSERTV(mX1[0], *mXREIt1, mX1[0] == *mXREIt1);
            ASSERTV( X1[0],  *XREIt1,  X1[0] ==  *XREIt1);
            ASSERTV(mX2[1], *mXRBIt2, mX2[1] == *mXRBIt2);
            ASSERTV( X2[1],  *XRBIt2,  X2[1] ==  *XRBIt2);
            ASSERTV(mX2[0], *mXREIt2, mX2[0] == *mXREIt2);
            ASSERTV( X2[0],  *XREIt2,  X2[0] ==  *XREIt2);
            ASSERTV(mX3[2], *mXRBIt3, mX3[2] == *mXRBIt3);
            ASSERTV( X3[2],  *XRBIt3,  X3[2] ==  *XRBIt3);
            ASSERTV(mX3[0], *mXREIt3, mX3[0] == *mXREIt3);
            ASSERTV( X3[0],  *XREIt3,  X3[0] ==  *XREIt3);
            ASSERTV(mX4[3], *mXRBIt4, mX4[3] == *mXRBIt4);
            ASSERTV( X4[3],  *XRBIt4,  X4[3] ==  *XRBIt4);
            ASSERTV(mX4[0], *mXREIt4, mX4[0] == *mXREIt4);
            ASSERTV( X4[0],  *XREIt4,  X4[0] ==  *XREIt4);
            ASSERTV(mX5[4], *mXRBIt5, mX5[4] == *mXRBIt5);
            ASSERTV( X5[4],  *XRBIt5,  X5[4] ==  *XRBIt5);
            ASSERTV(mX5[0], *mXREIt5, mX5[0] == *mXREIt5);
            ASSERTV( X5[0],  *XREIt5,  X5[0] ==  *XREIt5);

            // Testing 'crbegin' and 'crend'.

            bsl::reverse_iterator<const int *> mXCRBIt1 = bsl::crbegin(mX1);
            bsl::reverse_iterator<const int *>  XCRBIt1 = bsl::crbegin( X1);
            bsl::reverse_iterator<const int *> mXCREIt1 = bsl::crend  (mX1);
            bsl::reverse_iterator<const int *>  XCREIt1 = bsl::crend  ( X1);
            bsl::reverse_iterator<const int *> mXCRBIt2 = bsl::crbegin(mX2);
            bsl::reverse_iterator<const int *>  XCRBIt2 = bsl::crbegin( X2);
            bsl::reverse_iterator<const int *> mXCREIt2 = bsl::crend  (mX2);
            bsl::reverse_iterator<const int *>  XCREIt2 = bsl::crend  ( X2);
            bsl::reverse_iterator<const int *> mXCRBIt3 = bsl::crbegin(mX3);
            bsl::reverse_iterator<const int *>  XCRBIt3 = bsl::crbegin( X3);
            bsl::reverse_iterator<const int *> mXCREIt3 = bsl::crend  (mX3);
            bsl::reverse_iterator<const int *>  XCREIt3 = bsl::crend  ( X3);
            bsl::reverse_iterator<const int *> mXCRBIt4 = bsl::crbegin(mX4);
            bsl::reverse_iterator<const int *>  XCRBIt4 = bsl::crbegin( X4);
            bsl::reverse_iterator<const int *> mXCREIt4 = bsl::crend  (mX4);
            bsl::reverse_iterator<const int *>  XCREIt4 = bsl::crend  ( X4);
            bsl::reverse_iterator<const int *> mXCRBIt5 = bsl::crbegin(mX5);
            bsl::reverse_iterator<const int *>  XCRBIt5 = bsl::crbegin( X5);
            bsl::reverse_iterator<const int *> mXCREIt5 = bsl::crend  (mX5);
            bsl::reverse_iterator<const int *>  XCREIt5 = bsl::crend  ( X5);

            // Moving iterators to the first element.

            --mXCREIt1;
            --XCREIt1;
            --mXCREIt2;
            --XCREIt2;
            --mXCREIt3;
            --XCREIt3;
            --mXCREIt4;
            --XCREIt4;
            --mXCREIt5;
            --XCREIt5;

            ASSERTV(mX1[0], *mXCRBIt1, mX1[0] == *mXCRBIt1);
            ASSERTV( X1[0],  *XCRBIt1,  X1[0] ==  *XCRBIt1);
            ASSERTV(mX1[0], *mXCREIt1, mX1[0] == *mXCREIt1);
            ASSERTV( X1[0],  *XCREIt1,  X1[0] ==  *XCREIt1);
            ASSERTV(mX2[1], *mXCRBIt2, mX2[1] == *mXCRBIt2);
            ASSERTV( X2[1],  *XCRBIt2,  X2[1] ==  *XCRBIt2);
            ASSERTV(mX2[0], *mXCREIt2, mX2[0] == *mXCREIt2);
            ASSERTV( X2[0],  *XCREIt2,  X2[0] ==  *XCREIt2);
            ASSERTV(mX3[2], *mXCRBIt3, mX3[2] == *mXCRBIt3);
            ASSERTV( X3[2],  *XCRBIt3,  X3[2] ==  *XCRBIt3);
            ASSERTV(mX3[0], *mXCREIt3, mX3[0] == *mXCREIt3);
            ASSERTV( X3[0],  *XCREIt3,  X3[0] ==  *XCREIt3);
            ASSERTV(mX4[3], *mXCRBIt4, mX4[3] == *mXCRBIt4);
            ASSERTV( X4[3],  *XCRBIt4,  X4[3] ==  *XCRBIt4);
            ASSERTV(mX4[0], *mXCREIt4, mX4[0] == *mXCREIt4);
            ASSERTV( X4[0],  *XCREIt4,  X4[0] ==  *XCREIt4);
            ASSERTV(mX5[4], *mXCRBIt5, mX5[4] == *mXCRBIt5);
            ASSERTV( X5[4],  *XCRBIt5,  X5[4] ==  *XCRBIt5);
            ASSERTV(mX5[0], *mXCREIt5, mX5[0] == *mXCREIt5);
            ASSERTV( X5[0],  *XCREIt5,  X5[0] ==  *XCREIt5);
        }

        #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        if (verbose) printf("\tTest initializer lists.\n");
        {
            typedef bsl::reverse_iterator<const int *> reverse_iterator;

            std::initializer_list<int> mX1 = {1};
            std::initializer_list<int> mX2 = {1, 2};
            std::initializer_list<int> mX3 = {1, 2, 3};
            std::initializer_list<int> mX4 = {1, 2, 3, 4};
            std::initializer_list<int> mX5 = {1, 2, 3, 4, 5};

            // Testing 'rbegin' and 'rend'.

            reverse_iterator mXRBIt1 = bsl::rbegin(mX1);
            reverse_iterator mXREIt1 = bsl::rend  (mX1);
            reverse_iterator mXRBIt2 = bsl::rbegin(mX2);
            reverse_iterator mXREIt2 = bsl::rend  (mX2);
            reverse_iterator mXRBIt3 = bsl::rbegin(mX3);
            reverse_iterator mXREIt3 = bsl::rend  (mX3);
            reverse_iterator mXRBIt4 = bsl::rbegin(mX4);
            reverse_iterator mXREIt4 = bsl::rend  (mX4);
            reverse_iterator mXRBIt5 = bsl::rbegin(mX5);
            reverse_iterator mXREIt5 = bsl::rend  (mX5);

            ASSERTV(reverse_iterator(mX1.end()  ) == mXRBIt1);
            ASSERTV(reverse_iterator(mX1.begin()) == mXREIt1);
            ASSERTV(reverse_iterator(mX2.end()  ) == mXRBIt2);
            ASSERTV(reverse_iterator(mX2.begin()) == mXREIt2);
            ASSERTV(reverse_iterator(mX3.end()  ) == mXRBIt3);
            ASSERTV(reverse_iterator(mX3.begin()) == mXREIt3);
            ASSERTV(reverse_iterator(mX4.end()  ) == mXRBIt4);
            ASSERTV(reverse_iterator(mX4.begin()) == mXREIt4);
            ASSERTV(reverse_iterator(mX5.end()  ) == mXRBIt5);
            ASSERTV(reverse_iterator(mX5.begin()) == mXREIt5);
        }
#else
        if (verbose) printf("\tInitializer lists are not supported.\n");
#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS
        //
        // Concerns:
        //: 1 'operator<' returns the lexicographic comparison on two objects.
        //:
        //: 2 'operator>', 'operator<=', and 'operator>=' are correctly tied to
        //:   'operator<'.  i.e., For two objects, 'a' and 'b':
        //:
        //:   1 '(a > b) == (b < a)'
        //:
        //:   2 '(a <= b) == !(b < a)'
        //:
        //:   3 '(a >= b) == !(a < b)'
        //:
        //: 3 'operator-' and 'distance' return same results if parameters are
        //:   passed correctly.
        //:
        //: 4 'operator+' and 'bsl::reverse_iterator::operator+' return same
        //:   results if parameters are passed correctly.
        //
        // Plan:
        //: 1 Create reverse iterators 'it1' and 'it2'.  Verify 'operator<'
        //:   returns the correct lexicographic comparison results.  (C-1)
        //:
        //: 2 Verify 'operator>', 'operator<=', and 'operator>=' return correct
        //:   results using 'it1' and 'it2'.  Verify their results are
        //:   tied to results of 'operator<'.  (C-2)
        //:
        //: 3 Compare results of 'operator-' and 'distance' using 'it1' and
        //:   'it2', verify they return same results.  (C-3)
        //:
        //: 4 Compare results of 'operator+' and 'reverse_iterator::operator+'
        //:   using 'it1' and a distance, verify they return same results.
        //:   (C-4)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTIONS"
                            "\n======================\n");

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, 39, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        reverse_iterator it1(testData + numElements);

        if (verbose) printf("\nTest 'operator-', 'operator>', "
                            "'operator<=', and 'operator>='\n");

        for (int i = 0; i < numElements; ++i) {
            reverse_iterator it2(testData + i);
            LOOP3_ASSERT(i, *it1, *it2, it2 > it1);
            LOOP3_ASSERT(i, *it1, *it2, !(it1 > it2));
            LOOP3_ASSERT(i, *it1, *it2, it1 < it2);
            LOOP3_ASSERT(i, *it1, *it2, !(it2 < it1));
            LOOP3_ASSERT(i, *it1, *it2, it1 <= it2);
            LOOP3_ASSERT(i, *it1, *it2, !(it2 <= it1));
            LOOP3_ASSERT(i, *it1, *it2, it2 >= it1);
            LOOP3_ASSERT(i, *it1, *it2, !(it1 >= it2));
        }

#if !defined(BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03)
        for (int i = 0; i < numElements; ++i) {
            const_reverse_iterator it2(testData + i);
            LOOP3_ASSERT(i, *it1, *it2, it2 > it1);
            LOOP3_ASSERT(i, *it1, *it2, !(it1 > it2));
            LOOP3_ASSERT(i, *it1, *it2, it1 < it2);
            LOOP3_ASSERT(i, *it1, *it2, !(it2 < it1));
            LOOP3_ASSERT(i, *it1, *it2, it1 <= it2);
            LOOP3_ASSERT(i, *it1, *it2, !(it2 <= it1));
            LOOP3_ASSERT(i, *it1, *it2, it2 >= it1);
            LOOP3_ASSERT(i, *it1, *it2, !(it1 >= it2));
        }
#endif
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING OTHER ACCESSORS
        //
        // Concerns:
        //: 1 All other accessors of 'bsl::reverse_iterator' work correctly.
        //
        // Plan:
        //: 1 Create reverse iterators 'it'.  Call different accessors of 'it'.
        //:   Verify each accessor returns expected values.  (C-1)
        //
        // Testing:
        //   operator+(typename difference_type n) const;
        //   operator-(typename difference_type n) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OTHER ACCESSORS"
                            "\n=======================\n");

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, 39, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                             *iterator;
        typedef bsl::reverse_iterator<iterator>  reverse_iterator;

        if (verbose) printf("\nTest 'operator+'\n");

        reverse_iterator it(testData + numElements);
        for (int i = 1;i < numElements; ++i) {
            LOOP2_ASSERT(i, *(it + i), it + i != it);
            LOOP2_ASSERT(i, *(it + i),
                         testData[numElements - i - 1] == *(it + i));
        }

        if (verbose) printf("\nTest 'operator-'\n");

        it += numElements - 1;
        for (int i = 1;i < numElements; ++i) {
            LOOP2_ASSERT(i, *(it - 1), it - i != it);
            LOOP2_ASSERT(i, *(it - 1), testData[i] == *(it - i));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OTHER MANIPULATORS
        //
        // Concerns:
        //: 1 All other manipulators of 'bsl::reverse_iterator' work correctly.
        //
        // Plan:
        //: 1 Create reverse iterators 'it1' and 'it2'.  Use 'it1' as a
        //:   reference and call different manipulators of 'it2'.  Verify 'it2'
        //:   has expected values after each manipulation.  (C-1)
        //
        // Testing:
        //   bsl::reverse_iterator  operator++(int);
        //   bsl::reverse_iterator& operator+=(typename difference_type n);
        //   bsl::reverse_iterator& operator--();
        //   bsl::reverse_iterator  operator--(int);
        //   bsl::reverse_iterator& operator-=(typename difference_type n);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OTHER MANIPULATORS"
                            "\n==========================\n");

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        reverse_iterator it1(testData + numElements);
#if !defined(BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03)
        const_reverse_iterator it2 = it1;
#else
        reverse_iterator it2 = it1;
#endif

        if (verbose) printf("\nTest post-increment\n");
        it2++;
        it2++;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[numElements - 3]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) printf("\nTest pre-decrement\n");
        --it2;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[numElements - 2]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) printf("\nTest post-decrement\n");
        it2--;
        ASSERT(it2 == it1);
        ASSERT(*it2 == testData[numElements - 1]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) printf("\nTest 'operator+='\n");
        it2 += numElements - 1;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[0]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) printf("\nTest 'operator-='\n");
        it2 -= numElements - 1;
        ASSERT(it2 == it1);
        ASSERT(*it2 == testData[numElements - 1]);
        ASSERT(*it1 == testData[numElements - 1]);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY: Not Applicable
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR: Not Applicable
        //   The 'operator=' method of 'bsl::reverse_iterator' is directly
        //   inherited from 'std::reverse_iterator'.
        // --------------------------------------------------------------------
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SWAP: Not Applicable
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 A copy-constructed reverse iterator has the same value as the
        //:   original reverse iterator.
        //:
        //: 2 The value of the original reverse iterator is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the original reverse
        //:   iterator have no effect on the copy-constructed reverse iterator.
        //:
        //: 4 Subsequent changes on the copy-constructed reverse iterator have
        //:   no effect on the original.
        //
        // Plan:
        //: 1 Create reverse iterators 'it1' and 'it2',  copy-construct 'it3'
        //:   and 'it4' using 'it1' and 'it2' respectively.  Verify 'it3' has
        //:   the same value as 'it1' and 'it4' has the same value
        //:   as 'it2'.  Also verify 'it1' and 'it2' are unchanged by
        //:   checking the element values they refer to.  (C-1,2)
        //:
        //: 2 Alter value of 'it2', verify 'it4' is unchanged.  (C-3)
        //:
        //: 3 Let 'it2' go out of scope, verify 'it4' is unchanged.  (C-3)
        //:
        //: 4 Alter value of 'it3', verify 'it1' is unchanged.  (C-4)
        //
        // Testing:
        //   bsl::reverse_iterator(const bsl::reverse_iterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        if (verbose) printf(
                      "\nValidate copy-constructed object preserves value.\n");
        reverse_iterator it1(testData + numElements);
#if !defined(BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03)
        const_reverse_iterator it3(it1);
#else
        reverse_iterator it3(it1);
#endif
        ASSERT(it1 == it3);
        ASSERT(*it1 == testData[numElements - 1]);
        ASSERT(*it1 == testData[numElements - 1]);

        bsls::ObjectBuffer<reverse_iterator> buffer;  // Extend lifetime beyond
        reverse_iterator *pit4 = buffer.address();    // the following block.
        {
            reverse_iterator it2(testData + numElements);
            ASSERT(it2 == it1);

            new(pit4) reverse_iterator(it2); // construct '*pit4' from 'it2'.

            ASSERT( *pit4 == it2);
            ASSERT(  *it2 == testData[numElements - 1]);
            ASSERT(**pit4 == testData[numElements - 1]);


            if (verbose) printf(
                       "\nValidate changing original does not affect copy.\n");

            // After this line, 'it2' will go out of scope.

            ++it2;
            ASSERT( *pit4 != it2);
            ASSERT(  *it2 == testData[numElements - 2]);
            ASSERT(**pit4 == testData[numElements - 1]);
        }

        ASSERT( *pit4 == it1);
        ASSERT(**pit4 == testData[numElements - 1]);

        if (verbose) printf(
                       "\nValidate changing copy does not affect original.\n");

        ++it3;
        ASSERT( it3 != it1);
        ASSERT(*it3 == testData[numElements - 2]);
        ASSERT(*it1 == testData[numElements - 1]);

        // Destroy '*pit4'.

        buffer.object().~reverse_iterator();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //
        // Concerns:
        //: 1 The reverse iterators must compare equal to themselves.
        //:
        //: 2 Constant reverse iterators can be compared with mutable reverse
        //:   iterators.
        //:
        //: 3 Constant and mutable reverse iterators referring to the same
        //:   element shall compare equal.
        //:
        //: 4 Reverse iterators (either constant or mutable) that do not refer
        //:   to the same element shall not compare equal.
        //
        // Plan:
        //: 1 Create a fixed size array 'A', assign values to each of its
        //:   elements.
        //:
        //: 2 Get a constant reverse iterator 'itc1' generated by 'rbegin' of
        //:   'A'.  Use 'itc1' to verify self-equality and equality between
        //:   constant and mutable reverse iterators.  (C-1..3)
        //:
        //: 3 Get another constant reverse iterator 'itc2' generated by 'rend'
        //:   of 'A'.  Use 'itc1' and 'itc2' to verify inequality.  (C-4)
        //:
        //: 4 Get a third mutable reverse iterator 'itc2', change its value
        //:   using primary manipulators.  Verify various equality and
        //:   inequality against 'itc1' and 'itc2'.  (C-4)
        //
        // Testing:
        //   bool operator==(const reverse_iterator&, const reverse_iterator&);
        //   bool operator!=(const reverse_iterator&, const reverse_iterator&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATOR"
                            "\n=========================\n");

        using namespace testcontainer;

        typedef MyFixedSizeArray<int, 5>              TestContainer;
        typedef TestContainer::reverse_iterator       reverse_iterator;
        typedef TestContainer::const_reverse_iterator const_reverse_iterator;

        TestContainer tc;
        for (int i = 0; i < tc.size(); ++i) {
            tc[i] = i * i + i * 13 + 1;
        }

        if (verbose) printf("\nValidate self-equality\n");
        const reverse_iterator ritBegin = tc.rbegin();
        ASSERT(   ritBegin == ritBegin );
        ASSERT( !(ritBegin != ritBegin));
        ASSERT(tc.rbegin() == ritBegin );

        if (verbose) printf("\nValidate inequality\n");
#if !defined(BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03)
        const const_reverse_iterator ritEnd = tc.rend();
#else
        const reverse_iterator ritEnd = tc.rend();
#endif
        ASSERT(     ritBegin != ritEnd );
        ASSERT(   !(ritBegin == ritEnd));
        ASSERT(       ritEnd == ritEnd );
        ASSERT(     !(ritEnd != ritEnd));
        ASSERT(  tc.rbegin() != ritEnd );
        ASSERT(!(tc.rbegin() == ritEnd));
        ASSERT(    tc.rend() == ritEnd );
        ASSERT(  !(tc.rend() != ritEnd));

        if (verbose) printf("\nValidate transition to expected value\n");
        reverse_iterator ritCursor = tc.rbegin();
        ASSERT(ritBegin == ritCursor);
        ASSERT(ritEnd   != ritCursor);

        for (int i = 0;i < tc.size() - 1; ++i) {
            ++ritCursor;
            ASSERT(ritBegin != ritCursor);
            ASSERT(ritEnd   != ritCursor);
        }

        ++ritCursor;
        ASSERT(ritBegin != ritCursor);
        ASSERT(ritEnd   == ritCursor);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT: Not Applicable
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS: Not Applicable
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   The primitive generators for our reverse iterator, the primary
        //   piece of test apparatus, are the 'rbegin' and 'rend' member
        //   functions of the sample container type 'MyFixedSizeArray'.
        //
        // Concerns:
        //: 1 All public methods of the test container work correctly.
        //:
        //: 2 The iterators generated by 'begin' and 'end' shall delimit the
        //:   valid memory range allocated in test container.
        //:
        //: 3 The reverse iterators generated by 'rbegin' and 'rend' shall
        //:   delimit the valid memory range allocated in test container.
        //:
        //: 4 The iterators and reverse iterators refer to correct values.
        //:
        //: 5 The above concerns are also valid on constant iterators and
        //:   constant reverse iterators.
        //
        // Plan:
        //: 1 Create a fixed size array 'A', assign values to each of its
        //:   elements.  (C-1)
        //:
        //: 2 Verify iterators and reverse iterators generated by 'A' delimit
        //:   correct memory range.  (C-1..3)
        //:
        //: 3 Verify the values in the memory range delimited by iterators and
        //:   reverse iterators generated by 'A' refer to correct values in
        //:   'A'.  (C-4)
        //:
        //: 4 Repeat the above three steps to verify constant iterators and
        //:   constant reverse iterators.  (C-1..4)
        //
        // Testing:
        //   class MyFixedSizeArray<VALUE>;
        //   MyFixedSizeArray<VALUE, SIZE>::MyFixedSizeArray();
        //   MyFixedSizeArray<VALUE, SIZE>::begin();
        //   MyFixedSizeArray<VALUE, SIZE>::end();
        //   MyFixedSizeArray<VALUE, SIZE>::rbegin();
        //   MyFixedSizeArray<VALUE, SIZE>::rend();
        //   MyFixedSizeArray<VALUE, SIZE>::operator[](int);
        //   MyFixedSizeArray<VALUE, SIZE>::operator[](int) const;
        //   MyFixedSizeArray<VALUE, SIZE>::size() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING (PRIMITIVE) GENERATORS"
                            "\n==============================\n");

        if (verbose) printf("\nValidating primitive test machinery\n");

        if (verbose) printf("\nTesting class MyFixedSizeArray<int>\n");

        using namespace testcontainer;

        const int arrayLength = 5;
        typedef MyFixedSizeArray<int, arrayLength>    TestContainer;
        typedef TestContainer::iterator               iterator;
        typedef TestContainer::const_iterator         const_iterator;
        typedef TestContainer::reverse_iterator       reverse_iterator;
        typedef TestContainer::const_reverse_iterator const_reverse_iterator;

        TestContainer tc;
        ASSERT(arrayLength == tc.size());

        if (verbose) printf("\nCheck 'operator[]' and 'operator[] const'\n");

        for (int i = 0; i < tc.size(); ++i) {
            tc[i] = i + 1;
        }
        const TestContainer &tc2 = tc;
        for (int i = 0; i < tc2.size(); ++i) {
            ASSERT(i + 1 == tc2[i]);
        }

        if (verbose) printf("\nCheck iterator has right range\n");
        int length = 0;
        iterator itBegin = tc.begin();
        iterator itEnd   = tc.end();
        while(itBegin != itEnd) {
            ++length;
            ++itBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        if (verbose) printf("\nCheck reverse iterator has right range\n");
        length = 0;
        reverse_iterator ritBegin = tc.rbegin();
        reverse_iterator ritEnd   = tc.rend();
        while(ritBegin != ritEnd) {
            ++length;
            ++ritBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        if (verbose) printf("\nCheck iterators refer to right values\n");
        itBegin = tc.begin();
        for (int i = 0;i < tc.size(); ++i) {
            LOOP_ASSERT(*itBegin, i + 1 == *(itBegin++));
        }

        if (verbose) printf(
                          "\nCheck reverse iterators refer to right values\n");
        ritBegin = tc.rbegin();
        for (int i = tc.size() ;i > 0; --i) {
            LOOP_ASSERT(*ritBegin, i == *(ritBegin++));
        }

        if (verbose) printf("\nRepeat the tests for const_iterators\n");

        length = 0;
        const_iterator itcBegin = tc2.begin();
        const_iterator itcEnd   = tc2.end();
        while(itcBegin != itcEnd) {
            ++length;
            ++itcBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        length = 0;
        const_reverse_iterator ritcBegin = tc2.rbegin();
        const_reverse_iterator ritcEnd   = tc2.rend();
        while(ritcBegin != ritcEnd) {
            ++length;
            ++ritcBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        itcBegin = tc2.begin();
        for (int i = 0;i < tc2.size(); ++i) {
            LOOP_ASSERT(*itcBegin, i + 1 == *itcBegin);
            ++itcBegin;
        }

        ritcBegin = tc2.rbegin();
        for (int i = tc2.size() ;i > 0; --i) {
            LOOP_ASSERT(*ritcBegin, i == *ritcBegin);
            ++ritcBegin;
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 A reverse iterator created with value constructor has the
        //:   specified value.
        //:
        //: 2 The pre-increment operator changes the iterator to refer to the
        //:   element one before the previously referred element.
        //:
        //: 3 The above concerns are also valid on a constant reverse iterator.
        //
        // Plan:
        //: 1 Create a reverse iterator 'A' and a constant reverse iterator
        //:   'cA' using default constructor.
        //:
        //: 2 Create another reverse iterator 'B' and another constant reverse
        //:   iterator 'cB' using value constructor, verify they both have the
        //:   specified value passed in constructor, and they have different
        //:   values from 'A' and 'cA' (using yet unproven 'operator!=').
        //:   (C-1,3)
        //:
        //: 3 Pre-increment 'B', verify its value has changed by comparing 'B'
        //:   with 'cB'.  Also verify the increment was performed correctly by
        //:   checking the value 'cB' refers to.  (C-2)
        //:
        //: 4 Create another constant reverse iterator 'cC' using value
        //:   constructor, pre-increment it.  Verify 'cC' has changed by
        //:   comparing 'cC' with 'cB'.  Also verify the pre-increment was
        //:   performed correctly by checking the value it refers to.  (C-2,3)
        //
        // Testing:
        //   bsl::reverse_iterator();
        //   bsl::reverse_iterator(ITER);
        //   bsl::reverse_iterator& operator++();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS"
                            "\n============================\n");

        if (verbose) printf(
                          "\nTesting default constructor and 'operator++'.\n");

        //  Declare test data and types.

        int testData[4] = { 0, 1, 2, 3 };
        int numElements = sizeof(testData) / sizeof(int);

        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        const reverse_iterator itcDefault;
              reverse_iterator itDefault;

        // Confirm that an iterator over our reference array does not have the
        // same value as a default constructed iterator.  (Actually, this
        // comparison is undefined in general.)

        const reverse_iterator itcOrigin(testData + numElements);
              reverse_iterator itData   (testData + numElements);
        ASSERT(         3 == *itcOrigin);
        ASSERT(         3 == *itData   );
        ASSERT(itcDefault != itcOrigin );
        ASSERT(itDefault  != itData    );

        //  Confirm that an incremented iterator does not have the same value
        //  as the initial iterator.

        ++itData;
        ASSERT(itcOrigin != itData);
        ASSERT(2 == *itData);

        //  Confirm that incrementing a second copy of the initial iterator
        //  has the same value as the first incremented iterator.

#if !defined(BSLS_ITERATOR_NO_MIXED_OPS_IN_CPP03)
        const_reverse_iterator itData2(testData + numElements);
#else
        reverse_iterator itData2(testData + numElements);
#endif
        ASSERT(itcOrigin == itData2);

        ++itData2;
        ASSERT(itcOrigin != itData2);
        ASSERT(2 == *itData2);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 That basic functionality appears to work as advertised before
        //:   before beginning testing in earnest:
        //:   - default and copy constructors
        //:   - assignment operator
        //:   - primary manipulators, basic accessors
        //:   - 'operator==', 'operator!='
        //
        // Plan:
        //: 1 Test all public methods mentioned in concerns.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        int testData[] = { 0, 1, 2, 3 };

        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        if (verbose) printf("\nConstruct a basic reverse iterator value\n");
        reverse_iterator it1(testData + 3);
        LOOP_ASSERT( *it1, 2 == *it1);

        if (verbose) printf("\nMake a copy of that iterator\n");
        reverse_iterator it2(it1);
        LOOP_ASSERT(  *it2,            2 ==  *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) printf("\nIncrement an iterator\n");
        ++it2;
        LOOP_ASSERT(  *it2,             1 == *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 !=   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 != &*it2);

        //  Increment the other iterator iterator, verify both iterators have
        //  the same value again.

        if (verbose) printf(
                          "\nVerify multipass property of reverse iterator\n");
        ++it1;
        LOOP_ASSERT(  *it1,            1 ==  *it1);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) printf("\nConstruct a 'const_reverse_iterator'\n");
        const_reverse_iterator itEnd(testData + 4);

        if (verbose) printf("\nMake a copy of 'const_reverse_iterator'\n");

        const_reverse_iterator itBegin(it1);
        bsl::iterator_traits<
                            const_reverse_iterator>::difference_type distance =
                                                 bsl::distance(itEnd, itBegin);
        LOOP_ASSERT(distance, 2 == distance);

        if (verbose) printf(
                         "\nVerify writing through a dereferenced iterator\n");
        *it1 = 42;
        LOOP_ASSERT( *it1, 42 == *it1);

        if (verbose) printf(
    "\nVerify that writes through one iterator are visible through another\n");
        LOOP_ASSERT( *it2,         42 == *it2);
        LOOP2_ASSERT(*it1, *it2, *it1 == *it2);
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
