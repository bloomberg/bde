// bslstl_iterator.t.cpp                                              -*-C++-*-

#include <bslstl_iterator.h>
#include <bslstl_allocator.h>
#include <bslma_default.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace BloombergLP;
using namespace std;

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
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING (PRIMITIVE) GENERATORS
// [ 4] TESTING BASIC ACCESSORS:     Not Applicable
// [ 5] TESTING OUTPUT:              Not Applicable
// [ 8] TESTING SWAP:                Not Applicable
// [ 9] TESTING ASSIGNMENT OPERATOR: Not Applicable
// [10] STREAMING FUNCTIONALITY:     Not Applicable
// [14] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

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
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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

    (void) veryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
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

        if (verbose) cout << endl << "TESTING FREE FUNCTIONS" << endl
                                  << "======================" << endl;

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, 39, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        reverse_iterator it1(testData + numElements);

        if (verbose) cout << "\nTest 'operator-', 'operator>', "
                          << "'operator<=', and 'operator>='"   << endl;

        ASSERT(it1 <= it1);
        ASSERT(it1 >= it1);
        for (int i = 0;i < numElements; ++i) {
            reverse_iterator it2(testData + i);
            LOOP3_ASSERT(i, *it1, *it2, it2 > it1);
            LOOP3_ASSERT(i, *it1, *it2, it1 < it2);
            LOOP3_ASSERT(i, *it1, *it2, it1 <= it2);
            LOOP3_ASSERT(i, *it1, *it2, !(it2 <= it1));
            LOOP3_ASSERT(i, *it1, *it2, it2 >= it1);
            LOOP3_ASSERT(i, *it1, *it2, !(it1 >= it2));
        }
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

        if (verbose) cout << endl << "TESTING OTHER ACCESSORS" << endl
                                  << "=======================" << endl;

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, 39, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        if (verbose) cout << "\nTest 'operator+'" << endl;

        reverse_iterator it(testData + numElements);
        for (int i = 1;i < numElements; ++i) {
            LOOP2_ASSERT(i, *(it + i), it + i != it);
            LOOP2_ASSERT(i, *(it + i),
                         testData[numElements - i - 1] == *(it + i));
        }

        if (verbose) cout << "\nTest 'operator-'" << endl;

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

        if (verbose) cout << endl << "TESTING OTHER MANIPULATORS" << endl
                                  << "=========================" << endl;

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        reverse_iterator it1(testData + numElements);
        reverse_iterator it2 = it1;

        if (verbose) cout << "\nTest post-increment" << endl;
        it2++;
        it2++;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[numElements - 3]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) cout << "\nTest pre-decrement" << endl;
        --it2;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[numElements - 2]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) cout << "\nTest post-decrement" << endl;
        it2--;
        ASSERT(it2 == it1);
        ASSERT(*it2 == testData[numElements - 1]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) cout << "\nTest 'operator+='" << endl;
        it2 += numElements - 1;
        ASSERT(it2 != it1);
        ASSERT(*it2 == testData[0]);
        ASSERT(*it1 == testData[numElements - 1]);

        if (verbose) cout << "\nTest 'operator-='" << endl;
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

        if (verbose) cout << endl << "TESTING COPY CONSTRUCTOR" << endl
                                  << "========================" << endl;

        //  Declare test data and types.

        int testData[] = { 42, 13, 56, 72, };
        int numElements = sizeof(testData) / sizeof(int);
        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        bsl::allocator<reverse_iterator> ta;  // Need a temp allocator to
                                              // persist iterator.

        if (verbose)
            cout << "\nValidate copy-constructed object preserves value."
                 << endl;
        reverse_iterator it1(testData + numElements);
        reverse_iterator it3(it1);
        ASSERT(it1 == it3);
        ASSERT(*it1 == testData[numElements - 1]);
        ASSERT(*it1 == testData[numElements - 1]);

        reverse_iterator *pit4;
        {
            reverse_iterator it2(testData + numElements);
            ASSERT(it2 == it1);
            pit4 = ta.allocate(1);
            ta.construct(pit4, it2);  // Copy construct '*pit4' from 'it2'.

            ASSERT( *pit4 == it2);
            ASSERT(  *it2 == testData[numElements - 1]);
            ASSERT(**pit4 == testData[numElements - 1]);


            if (verbose)
                cout << "\nValidate changing original does not affect copy."
                     << endl;

            // After this line, 'it2' will go out of scope.

            ++it2;
            ASSERT( *pit4 != it2);
            ASSERT(  *it2 == testData[numElements - 2]);
            ASSERT(**pit4 == testData[numElements - 1]);
        }

        ASSERT( *pit4 == it1);
        ASSERT(**pit4 == testData[numElements - 1]);

        if (verbose)
            cout << "\nValidate changing copy does not affect original."
                 << endl;

        ++it3;
        ASSERT( it3 != it1);
        ASSERT(*it3 == testData[numElements - 2]);
        ASSERT(*it1 == testData[numElements - 1]);

        // Destroy the allocated '*pit4'.

        ta.destroy(pit4);
        ta.deallocate(pit4, 1);
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
        if (verbose) cout << endl << "TESTING EQUALITY OPERATOR" << endl
                                  << "=========================" << endl;

        using namespace testcontainer;

        typedef MyFixedSizeArray<int, 5>              TestContainer;
        typedef TestContainer::iterator               iterator;
        typedef TestContainer::const_iterator         const_iterator;
        typedef TestContainer::reverse_iterator       reverse_iterator;
        typedef TestContainer::const_reverse_iterator const_reverse_iterator;

        TestContainer tc;
        for (int i = 0; i < tc.size(); ++i) {
            tc[i] = i * i + i * 13 + 1;
        }

        if (verbose) cout << "\nValidate self-equality" << endl;
        const reverse_iterator ritBegin = tc.rbegin();
        ASSERT(   ritBegin == ritBegin );
        ASSERT( !(ritBegin != ritBegin));
        ASSERT(tc.rbegin() == ritBegin );

        if (verbose) cout << "\nValidate inequality" << endl;
        const reverse_iterator ritEnd = tc.rend();
        ASSERT(     ritBegin != ritEnd );
        ASSERT(   !(ritBegin == ritEnd));
        ASSERT(       ritEnd == ritEnd );
        ASSERT(     !(ritEnd != ritEnd));
        ASSERT(  tc.rbegin() != ritEnd );
        ASSERT(!(tc.rbegin() == ritEnd));
        ASSERT(    tc.rend() == ritEnd );
        ASSERT(  !(tc.rend() != ritEnd));

        if (verbose) cout << "\nValidate transition to expected value" << endl;
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

        if (verbose) cout << endl << "TESTING (PRIMITIVE) GENERATORS" << endl
                                  << "==============================" << endl;

        if (verbose) cout << "\nValidating primitive test machinery" << endl;

        if (verbose) cout << "\nTesting class MyFixedSizeArray<int>" << endl;

        using namespace testcontainer;

        const int arrayLength = 5;
        typedef MyFixedSizeArray<int, arrayLength>    TestContainer;
        typedef TestContainer::iterator               iterator;
        typedef TestContainer::const_iterator         const_iterator;
        typedef TestContainer::reverse_iterator       reverse_iterator;
        typedef TestContainer::const_reverse_iterator const_reverse_iterator;

        TestContainer tc;
        ASSERT(arrayLength == tc.size());

        if (verbose) cout << "\nCheck 'operator[]' and 'operator[] const'"
                          << endl;

        for (int i = 0; i < tc.size(); ++i) {
            tc[i] = i + 1;
        }
        const TestContainer &tc2 = tc;
        for (int i = 0; i < tc2.size(); ++i) {
            ASSERT(i + 1 == tc2[i]);
        }

        if (verbose) cout << "\nCheck iterator has right range" << endl;
        int length = 0;
        iterator itBegin = tc.begin();
        iterator itEnd   = tc.end();
        while(itBegin != itEnd) {
            ++length;
            ++itBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        if (verbose) cout << "\nCheck reverse iterator has right range"
                          << endl;
        length = 0;
        reverse_iterator ritBegin = tc.rbegin();
        reverse_iterator ritEnd   = tc.rend();
        while(ritBegin != ritEnd) {
            ++length;
            ++ritBegin;
        }
        LOOP_ASSERT(length, arrayLength == length);

        if (verbose) cout << "\nCheck iterators refer to right values" << endl;
        itBegin = tc.begin();
        for (int i = 0;i < tc.size(); ++i) {
            LOOP_ASSERT(*itBegin, i + 1 == *(itBegin++));
        }

        if (verbose) cout << "\nCheck reverse iterators refer to right values"
                          << endl;
        ritBegin = tc.rbegin();
        for (int i = tc.size() ;i > 0; --i) {
            LOOP_ASSERT(*ritBegin, i == *(ritBegin++));
        }

        if (verbose) cout << "\nRepeat the tests for const_iterators" << endl;

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

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATORS" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor and 'operator++'."
                          << endl;

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

        reverse_iterator itData2(testData + numElements);
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

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        int testData[] = { 0, 1, 2, 3 };

        typedef int                                   *iterator;
        typedef int const                             *const_iterator;
        typedef bsl::reverse_iterator<iterator>        reverse_iterator;
        typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

        if (verbose) cout << "\nConstruct a basic reverse iterator value"
                          << endl;
        reverse_iterator it1(testData + 3);
        LOOP_ASSERT( *it1, 2 == *it1);

        if (verbose) cout << "\nMake a copy of that iterator" << endl;
        reverse_iterator it2(it1);
        LOOP_ASSERT(  *it2,            2 ==  *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nIncrement an iterator" << endl;
        ++it2;
        LOOP_ASSERT(  *it2,             1 == *it2);
        LOOP2_ASSERT(&*it1, &*it2,   it1 !=   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 != &*it2);

        //  Increment the other iterator iterator, verify both iterators have
        //  the same value again.

        if (verbose) cout << "\nVerify multipass property of reverse iterator"
                          << endl;
        ++it1;
        LOOP_ASSERT(  *it1,            1 ==  *it1);
        LOOP2_ASSERT(&*it1, &*it2,   it1 ==   it2);
        LOOP2_ASSERT(&*it1, &*it2, &*it1 == &*it2);

        if (verbose) cout << "\nConstruct a 'const_reverse_iterator'" << endl;
        const_reverse_iterator itEnd(testData + 4);

        if (verbose) cout << "\nMake a copy of 'const_reverse_iterator'"
                          << endl;

        const_reverse_iterator itBegin(it1);
        bsl::iterator_traits<
                            const_reverse_iterator>::difference_type distance =
                                                 bsl::distance(itEnd, itBegin);
        LOOP_ASSERT(distance, 2 == distance);

        if (verbose) cout << "\nVerify writing through a dereferenced iterator"
                          << endl;
        *it1 = 42;
        LOOP_ASSERT( *it1, 42 == *it1);

        if (verbose) cout << "\nVerify that writes through one iterator are"
                          << "  visible through another" << endl;
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
