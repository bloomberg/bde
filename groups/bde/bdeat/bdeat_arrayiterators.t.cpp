// bdeat_arrayiterators.t.cpp                  -*-C++-*-

#include <bdeat_arrayiterators.h>
#include <bdeat_arrayfunctions.h>
#include <bdeat_valuetypefunctions.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_vector.h>
#include <bsl_algorithm.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// The iterators defined in this component have very few operations.  Testing
// consists of instantiating the iterators with different sequence types and
// testing the '*i++ = v' expression on the instantiated iterator.  This test
// driver defines an opaque sequence type that provides only indirect access
// to its elements.  (The sequence can only be manipulated or accessed only
// though 'bdeat_SequenceFunctions' methods and the individual items can be
// manipulated or accessed only though 'bdeat_ValueFunctions' methods.)  This
// opaque sequence type is used to instantiate and test the class and function
// templates in this component.  A second set of tests is also performed with
// a simple 'bsl::vector' sequence.
//-----------------------------------------------------------------------------
// [ 1] class BackInsertIterator<TYPE>
// [ 1] backInserter(const TYPE* array)
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush; // P(X) no nl
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

namespace Obj = bdeat_ArrayIterators;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace Test {

                       // ================
                       // class FixedArray
                       // ================

template <int SIZE, typename TYPE> class FixedArray;
    // Fixed-sized array that conforms to the 'bdeat_arrayfunctions'
    // interface and can only be manipulated and accessed through that
    // interface.

// FREE MANIPULATORS (bdeat_arrayfunctions manipulators for FixedArray)
template <int SIZE, typename TYPE, typename MANIPULATOR>
int bdeat_arrayManipulateElement(FixedArray<SIZE, TYPE> *array,
                                 MANIPULATOR&            manipulator,
                                 int                     index);

template <int SIZE, typename TYPE>
void bdeat_arrayResize(FixedArray<SIZE, TYPE> *array, int newSize);

// FREE ACCESSORS (bdeat_arrayfunctions accessors for FixedArray)
template <int SIZE, typename TYPE, typename ACCESSOR>
int bdeat_arrayAccessElement(const FixedArray<SIZE, TYPE>& array,
                             ACCESSOR&                     accessor,
                             int                           index);

template <int SIZE, typename TYPE>
bsl::size_t bdeat_arraySize(const FixedArray<SIZE, TYPE>& array);
    // Return the number of elements in the specified 'array'.

template <int SIZE, typename TYPE>
class FixedArray
{
    // Fixed-sized array that conforms to the 'bdeat_arrayfunctions'
    // interface and can only be manipulated and accessed through that
    // interface.

#ifndef BSLS_PLATFORM__CMP_MSVC // MSVC has problems with friend templates.

    // FRIEND MANIPULATORS (only way to change an object of this class)
    template <int SIZE2, typename TYPE2, typename MANIPULATOR>
    friend int
    bdeat_arrayManipulateElement(FixedArray<SIZE2, TYPE2> *array,
                                 MANIPULATOR&             manipulator,
                                 int                      index);

    template <int SIZE2, typename TYPE2>
    friend void bdeat_arrayResize(FixedArray<SIZE2, TYPE2> *array,
                                  int                       newSize);

    // FRIEND ACCESSORS (only way to access attributes of the array)
    template <int SIZE2, typename TYPE2, typename ACCESSOR>
    friend int
    bdeat_arrayAccessElement(const FixedArray<SIZE2, TYPE2>& array,
                             ACCESSOR&                       accessor,
                             int                             index);

    template <int SIZE2, typename TYPE2>
    friend bsl::size_t bdeat_arraySize(const FixedArray<SIZE2, TYPE2>& array);

#else
    // MSVC has problems with friend templates, so make everything public.
  public:
#endif
    // PRIVATE DATA MEMBERS
    TYPE d_values[SIZE];
    int  d_length;

  public:
    // CREATORS
    FixedArray();

    // Compiler-generated functions:
    // FixedArray(const FixedArray&);
    // FixedArray& operator=(const FixedArray&);
    // ~FixedArray();
};

                       // ============================
                       // class FixedArrayElement
                       // ============================

template <typename TYPE> class FixedArrayElement;
    // Proxy object to access an element of a FixedArray.  The 'accessElement'
    // and 'manipulateElement' methods in 'FixedArray' used this proxy to
    // simulate a complex array that does not provide direct references to the
    // underlying array items.  This class meets the requirements of
    // 'bdeat_valuefunction'.

// FREE MANIPULATORS (bdeat_valuefunctions manipulators for FixedArrayElement)
template <typename TYPE>
void bdeat_valueTypeReset(FixedArrayElement<TYPE> *object);

template <typename TYPE, typename RHS_TYPE>
int bdeat_valueTypeAssign(FixedArrayElement<TYPE> *lhs, const RHS_TYPE& rhs);

// FREE ACCESSORS (bdeat_valuefunctions accessors for FixedArrayElement)
template <typename LHS_TYPE, typename TYPE>
int bdeat_valueTypeAssign(LHS_TYPE                       *lhs,
                          const FixedArrayElement<TYPE>&  rhs);

template <typename TYPE>
class FixedArrayElement {
    // Proxy object to access an element of a FixedArray.  The 'accessElement'
    // and 'manipulateElement' methods in 'FixedArray' use this proxy to
    // simulate a complex array that does not provide direct references to the
    // underlying array items.  This class meets the requirements of
    // 'bdeat_valuefunction'.

#ifndef BSLS_PLATFORM__CMP_MSVC // MSVC has problems with friend templates.

    template <int SIZE, typename TYPE2>
    friend class FixedArray;
        // The 'FixedArray' class template is a friend of this class template.

    template <typename TYPE2>
    friend void bdeat_valueTypeReset(FixedArrayElement<TYPE2> *object);

    template <typename TYPE2, typename RHS_TYPE>
    friend int bdeat_valueTypeAssign(FixedArrayElement<TYPE2> *lhs,
                                     const RHS_TYPE&           rhs);

    template <typename LHS_TYPE, typename TYPE2>
    friend int bdeat_valueTypeAssign(LHS_TYPE                        *lhs,
                                     const FixedArrayElement<TYPE2>&  rhs);

#else
    // MSVC has problems with friend templates, so make everything public.
  public:
#endif
    TYPE* d_element;

  private:
    // NOT IMPLEMENTED
    FixedArrayElement(const FixedArrayElement&);
    FixedArrayElement& operator=(const FixedArrayElement&);

  public:
    // CREATORS
    FixedArrayElement(TYPE* element) : d_element(element) { }
        // Construct a proxy to the specified 'element'.

    // Compiler-generated destructor:
    // ~FixedArrayElement();
};


} // Close namespace Test

namespace BloombergLP {
namespace bdeat_ArrayFunctions {
    // META FUNCTIONS
    template <int SIZE, typename TYPE>
    struct ElementType<Test::FixedArray<SIZE, TYPE> > {
        typedef Test::FixedArrayElement<TYPE> Type;
    };

    template <int SIZE, typename TYPE>
    struct IsArray<Test::FixedArray<SIZE, TYPE> > : public bslmf_MetaInt<1> {
    };
}
}

                       // ----------------------
                       // class Test::FixedArray
                       // ----------------------

// CREATOR
template <int SIZE, typename TYPE>
inline
Test::FixedArray<SIZE, TYPE>::FixedArray()
: d_length(0)
{
}

// FREE MANIPULATORS
template <int SIZE, typename TYPE, typename MANIPULATOR>
int Test::bdeat_arrayManipulateElement(Test::FixedArray<SIZE, TYPE> *array,
                                       MANIPULATOR&                  manip,
                                       int                           index)
{
    // Use a proxy to simulate an array that does not provide direct
    // references to its elements, e.g., a database wrapper might provide a
    // cursor-like proxy for table elements.
    FixedArrayElement<TYPE> proxy(&array->d_values[index]);
    return manip(&proxy);
}

template <int SIZE, typename TYPE>
void Test::bdeat_arrayResize(Test::FixedArray<SIZE, TYPE> *array, int newSize)
{
    // If growing, then null out new elements
    for (int i = array->d_length; i < newSize; ++i) {
        array->d_values[i] = TYPE();
    }

    array->d_length = newSize;
}

// FREE ACCESSORS
template <int SIZE, typename TYPE, typename ACCESSOR>
int Test::bdeat_arrayAccessElement(const Test::FixedArray<SIZE, TYPE>& array,
                                   ACCESSOR&                           acc,
                                   int                                 index)
{
    // Use a proxy to simulate an array that does not provide direct
    // references to its elements, e.g., a database wrapper might provide a
    // cursor-like proxy for table elements.
    FixedArrayElement<TYPE> proxy(const_cast<int*>(&array.d_values[index]));
    return acc(proxy);
}

template <int SIZE, typename TYPE>
bsl::size_t Test::bdeat_arraySize(const Test::FixedArray<SIZE, TYPE>& array)
{
    return array.d_length;
}

                       // -----------------------------
                       // class Test::FixedArrayElement
                       // -----------------------------

// FREE MANIPULATORS
template <typename TYPE>
inline
void Test::bdeat_valueTypeReset(FixedArrayElement<TYPE> *object)
{
    *object->d_element = TYPE();
}

template <typename TYPE, typename RHS_TYPE>
inline
int Test::bdeat_valueTypeAssign(FixedArrayElement<TYPE> *lhs,
                                const RHS_TYPE&          rhs)
{
    *lhs->d_element = rhs;
    return 0;
}

// FREE ACCESSORS
template <typename LHS_TYPE, typename TYPE>
inline
int Test::bdeat_valueTypeAssign(LHS_TYPE                       *lhs,
                                const FixedArrayElement<TYPE>&  rhs)
{
    *lhs = *rhs.d_element;
    return 0;
}

                        // ===============
                        // class TestValue
                        // ===============

template <typename TYPE>
struct TestValue {
    // Visitor to test that a value is as expected.
    TYPE d_expected;
    bool d_result;
  public:
    TestValue(const TYPE& expected) : d_expected(expected), d_result(false) { }

    bool result() const { return d_result; }

    template <typename T2>
    int operator()(const T2& value) {
        int item;
        int rc = bdeat_ValueTypeFunctions::assign(&item, value);
        if (veryVeryVerbose) { P_(item); P(d_expected) }
        if (0 == rc) {
            d_result = (item == d_expected);
        }
        return rc;
    }
};

template <typename ARRAY_TYPE, typename ITEM_TYPE>
bool testArrayItem(const ARRAY_TYPE& array, int index, const ITEM_TYPE& exp)
    // Return true if the item at the specified 'index' in the specified
    // 'array' has the value specified in 'exp'.
{
    TestValue<ITEM_TYPE> probe(exp);
    bdeat_ArrayFunctions::accessElement(array, probe, index);
    return probe.result();
}

//=============================================================================
//                  CLASSES AND FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// To use the facilities in this component, you must of course include the
// header file:
//..
//  #include <bdeat_arrayiterators.h>
//..
// The main use of the facilities in this component is for creating generic
// algorithms.  The following generic function appends a few integers to the
// end of an object of type 'ARRAY' that adheres to the 'bdeat_ArrayFunctions'
// interface.  It starts by creating a 'BackInsertIterator':
//..
    template <typename ARRAY>
    void appendSome(ARRAY *arrayObj)
    {
        bdeat_ArrayIterators::BackInsertIterator<ARRAY> it(arrayObj);
//..
// Now, using the "*i++ = v" idiom, append the numbers 5 and 4 to the array
// object:
//..
        *it++ = 5;
        *it++ = 4;
//..
// Alternatively, one can use the iterator in a standard algorithm.  For
// example, the following code appends the numbers 3, 2, and 1 to the array
// object:
//..
        const int VALUES[] = { 3, 2, 1 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(VALUES[0]);
        bsl::copy(VALUES, VALUES + NUM_VALUES, it);
    }
//..
// An alternative implementation of 'appendSome' would use 'backInserter' to
// create an iterator without declaring its exact type.  Note that in this
// case we do not create a variable 'it', but simply pass the iterator to a
// standard algorithm:
//..
    template <typename ARRAY>
    void appendSome2(ARRAY *arrayObj)
    {
        const int VALUES[] = { 5, 4, 3, 2, 1 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(VALUES[0]);
        bsl::copy(VALUES, VALUES + NUM_VALUES,
                  bdeat_ArrayIterators::backInserter(arrayObj));
    }
//..
// In our main program, we need to construct an array that adheres to the
// 'bdeat_arrayfunctions' interface:
//..
//  #include <vector>

    int usageExample()
    {
        typedef bsl::vector<int> my_IntArrayType;
//..
// The result of calling 'appendSome' is that the elements 5, 4, 3, 2 and 1
// are appended to the array:
//..
        my_IntArrayType array1;
        appendSome(&array1);
        ASSERT(5 == array1[0]);
        ASSERT(4 == array1[1]);
        ASSERT(3 == array1[2]);
        ASSERT(2 == array1[3]);
        ASSERT(1 == array1[4]);
//..
// The result of calling 'appendSome2' is the same:
//..
        my_IntArrayType array2;
        appendSome2(&array2);
        ASSERT(array2 == array1);

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Example"
                               << "\n=====================" << bsl::endl;

        usageExample();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING BACK INSERTER
        //
        // Testing:
        //    class BackInsertIterator<TYPE>;
        //    BackInsertIterator<TYPE> backInserter(const TYPE* array);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Back Inserter"
                               << "\n=====================" << bsl::endl;

        static const int INPUT[5] = { 1, 2, 3, 5, 7 };

        if (verbose) bsl::cout << "Testing back-inserter with Test::FixedArray"
                               << bsl::endl;
        {
            Test::FixedArray<10, int> mV;
            const Test::FixedArray<10, int>& V = mV;

            // Append the numbers 1 to 5 to 'mV'
            bsl::copy(INPUT, INPUT + 5, Obj::backInserter(&mV));

            ASSERT(5 == bdeat_ArrayFunctions::size(V));
            ASSERT(testArrayItem(V,  0,  1));
            ASSERT(testArrayItem(V,  1,  2));
            ASSERT(testArrayItem(V,  2,  3));
            ASSERT(testArrayItem(V,  3,  5));
            ASSERT(testArrayItem(V,  4,  7));
        }

        if (verbose) bsl::cout << "Testing back-inserter with vector"
                               << bsl::endl;
        {
            bsl::vector<int> mV; bsl::vector<int>& V = mV;
            mV.push_back(66);
            mV.push_back(77);

            // Append the numbers 1 to 5 to 'mV'
            bsl::copy(INPUT, INPUT + 5, Obj::backInserter(&mV));

            ASSERT(7 == V.size());
            ASSERT(66 == V[0]);
            ASSERT(77 == V[1]);
            ASSERT( 1 == V[2]);
            ASSERT( 2 == V[3]);
            ASSERT( 3 == V[4]);
            ASSERT( 5 == V[5]);
            ASSERT( 7 == V[6]);

            ASSERT(testArrayItem(V,  0, 66));
            ASSERT(testArrayItem(V,  1, 77));
            ASSERT(testArrayItem(V,  2,  1));
            ASSERT(testArrayItem(V,  3,  2));
            ASSERT(testArrayItem(V,  4,  3));
            ASSERT(testArrayItem(V,  5,  5));
            ASSERT(testArrayItem(V,  6,  7));
        }

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
