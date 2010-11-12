// bdeat_arrayfunctions.t.cpp                  -*-C++-*-

#include <bdeat_arrayfunctions.h>

#include <bdeat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bslmf_issame.h>             // for testing only

#include <bsls_platformutil.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
// [ 2] struct IsArray<TYPE>
// [ 2] struct ElementType<TYPE>
// [ 1] METHOD FORWARDING TEST
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace Obj = bdeat_ArrayFunctions;

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <typename LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    int operator()(const LVALUE_TYPE& object) const;
        // Assign 'object' to '*d_destination_p'.

    template <typename RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Do nothing.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <typename RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    const RVALUE_TYPE& d_value;  // held, not owned

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    int operator()(RVALUE_TYPE *object) const;
        // Assign 'd_value' to '*object'.

    template <typename LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Do nothing.
};

                       // ================
                       // class FixedArray
                       // ================

namespace Test {

template <int SIZE, typename TYPE>
class FixedArray
{
    // Fixed-sized array that conforms to the 'bdeat_ArrayFunctions'
    // interface.

    TYPE d_values[SIZE];
    int  d_length;

  public:
    FixedArray();

    // Compiler-generated functions:
    // FixedArray(const FixedArray&);
    // FixedArray& operator=(const FixedArray&);
    // ~FixedArray();

    // MANIPULATORS
    void append(const TYPE& v);

    void resize(int newSize);

    template <typename MANIPULATOR>
    int manipulateElement(MANIPULATOR& manip, int index);

    // ACCESSORS
    int length() const;

    template <typename ACCESSOR>
    int accessElement(ACCESSOR& acc, int index) const;
};

// FREE MANIPULATORS
template <int SIZE, typename TYPE, typename MANIPULATOR>
int bdeat_arrayManipulateElement(FixedArray<SIZE, TYPE> *array,
                                 MANIPULATOR&            manipulator,
                                 int                     index);

template <int SIZE, typename TYPE>
void bdeat_arrayResize(FixedArray<SIZE, TYPE> *array, int newSize);

// FREE ACCESSORS
template <int SIZE, typename TYPE, typename ACCESSOR>
int bdeat_arrayAccessElement(const FixedArray<SIZE, TYPE>& array,
                             ACCESSOR&                     accessor,
                             int                           index);

template <int SIZE, typename TYPE>
bsl::size_t bdeat_arraySize(const FixedArray<SIZE, TYPE>& array);
    // Return the number of elements in the specified 'array'.

} // Close namespace Test

namespace BloombergLP {
namespace bdeat_ArrayFunctions {
    // META FUNCTIONS
    template <int SIZE, typename TYPE>
    struct ElementType<Test::FixedArray<SIZE, TYPE> > {
        typedef TYPE Type;
    };

    template <int SIZE, typename TYPE>
    struct IsArray<Test::FixedArray<SIZE, TYPE> > : public bslmf_MetaInt<1> {
    };
}
}



                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <typename LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <typename LVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object) const
{
    *d_lValue_p = object;
    return 0;
}

template <typename LVALUE_TYPE>
template <typename RVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <typename RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <typename RVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object) const
{
    *object = d_value;
    return 0;
}

template <typename RVALUE_TYPE>
template <typename LVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object) const
{
    return -1;
}

                       // ----------------------
                       // class Test::FixedArray
                       // ----------------------

template <int SIZE, typename TYPE>
inline
Test::FixedArray<SIZE, TYPE>::FixedArray()
: d_length(0)
{
}

template <int SIZE, typename TYPE>
inline
void Test::FixedArray<SIZE, TYPE>::append(const TYPE& v)
{
    d_values[d_length++] = v;
}

template <int SIZE, typename TYPE>
void Test::FixedArray<SIZE, TYPE>::resize(int newSize)
{
    // If growing, then null out new elements
    for (int i = d_length; i < newSize; ++i) {
        d_values[i] = TYPE();
    }

    d_length = newSize;
}

template <int SIZE, typename TYPE>
template <typename MANIPULATOR>
inline
int Test::FixedArray<SIZE, TYPE>::manipulateElement(MANIPULATOR& manip,
                                                    int          index)
{
    return manip(&d_values[index]);
}

template <int SIZE, typename TYPE>
inline
int Test::FixedArray<SIZE, TYPE>::length() const
{
    return d_length;
}

template <int SIZE, typename TYPE>
template <typename ACCESSOR>
inline
int Test::FixedArray<SIZE, TYPE>::accessElement(ACCESSOR& acc, int index) const
{
    return acc(d_values[index]);
}

// FREE MANIPULATORS
template <int SIZE, typename TYPE, typename MANIPULATOR>
int Test::bdeat_arrayManipulateElement(Test::FixedArray<SIZE, TYPE> *array,
                                       MANIPULATOR&                  manip,
                                       int                           index)
{
    return array->manipulateElement(manip, index);
}

template <int SIZE, typename TYPE>
void Test::bdeat_arrayResize(Test::FixedArray<SIZE, TYPE> *array, int newSize)
{
    array->resize(newSize);
}

// FREE ACCESSORS
template <int SIZE, typename TYPE, typename ACCESSOR>
int Test::bdeat_arrayAccessElement(const Test::FixedArray<SIZE, TYPE>& array,
                                   ACCESSOR&                           acc,
                                   int                                 index)
{
    return array.accessElement(acc, index);
}

template <int SIZE, typename TYPE>
bsl::size_t Test::bdeat_arraySize(const Test::FixedArray<SIZE, TYPE>& array)
{
    return array.length();
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The following snippets of code illustrate the usage of this component.
// Suppose you wanted to create a function that prints values to a specified
// output stream.  The function should also be able to expand arrays and print
// each element in the array.  We will use a stateful function object for this
// example.  First, define a 'PrintValue' function class:
//..
#include <bdeat_arrayfunctions.h>
#include <bslmf_if.h>

#include <bsl_iostream.h>
#include <bsl_vector.h>

namespace BDEAT_ARRAYFUNCTIONS_USAGE_EXAMPLE {
//..
// The entire 'PrintValue' function class is provided below, uninterrupted, for
// clarity:
//..
class PrintValue {
    // This function will print values to the specified output stream.

    // PRIVATE DATA MEMBERS
    bsl::ostream *d_stream_p;
    int           d_verbose;

    // PRIVATE TYPES
    struct IsNotArrayType { };
    struct IsArrayType    { };

    // PRIVATE OPERATIONS
    template <typename TYPE>
    int execute(const TYPE& value, IsNotArrayType)
    {
        enum { SUCCESS = 0 };

        if (d_verbose)
            (*d_stream_p) << value << ' ';

        return SUCCESS;
    }

    template <typename TYPE>
    int execute(const TYPE& value, IsArrayType)
    {
        enum { SUCCESS = 0, FAILURE = -1 };

        int numElements = bdeat_ArrayFunctions::size(value);

        for (int index = 0; index < numElements; ++index) {
            if (0 != bdeat_ArrayFunctions::accessElement(value,
                                                         *this,
                                                         index)) {
                return FAILURE;
            }
        }

        return SUCCESS;
    }

  public:
    // CREATORS
    PrintValue(bsl::ostream *stream, int verbose)
    : d_stream_p(stream), d_verbose(verbose)
    {
    }

    // OPERATIONS
    template <typename TYPE>
    int operator()(const TYPE& value)
    {
        typedef typename
        bslmf_If<bdeat_ArrayFunctions::IsArray<TYPE>::VALUE,
                 IsArrayType,
                 IsNotArrayType>::Type Toggle;

        return execute(value, Toggle());
    }
};  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "array"
// behavior through the 'bdeat_ArrayFunctions' 'namespace' (e.g.,
// 'bsl::vector') and any other type that has 'operator<<' defined for it.  For
// example:
//..
void usageExample(int verbose)
{
    PrintValue printValue(&bsl::cout, verbose);

    int intScalar = 123;

    printValue(intScalar);  // expected output: '123 '

    bsl::vector<int> intArray;
    intArray.push_back(345);
    intArray.push_back(456);
    intArray.push_back(567);

    printValue(intArray);  // expected output: '345 456 567 '
}
//..
} // namespace BDEAT_ARRAYFUNCTIONS_USAGE_EXAMPLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
          using namespace BDEAT_ARRAYFUNCTIONS_USAGE_EXAMPLE;
          usageExample(verbose);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsArray
        //   struct ElementType
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == bdeat_ArrayFunctions::IsArray<int>::VALUE);

        typedef
            Obj::ElementType<Test::FixedArray<9, short> >::Type FAElementType;
        ASSERT(1 ==
           (bdeat_ArrayFunctions::IsArray<Test::FixedArray<3, char> >::VALUE));
        ASSERT(1 == (bslmf_IsSame<FAElementType, short>::VALUE));

        typedef Obj::ElementType<bsl::vector<int> >::Type VecElementType;
        ASSERT(1 == bdeat_ArrayFunctions::IsArray<bsl::vector<int> >::VALUE);
        ASSERT(1 == (bslmf_IsSame<VecElementType, int>::VALUE));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        {
            if (verbose) cout << "Testing forwarding with Test::FixedArray"
                              << endl;
            Test::FixedArray<10, int> mV; Test::FixedArray<10, int>& V = mV;
            mV.append(66);
            mV.append(77);

            ASSERT(2 == Obj::size(V));

            int              value;
            GetValue<int>    getter(&value);
            AssignValue<int> setter1(33);
            AssignValue<int> setter2(44);

            Obj::accessElement(V, getter, 0); ASSERT(66 == value);
            Obj::accessElement(V, getter, 1); ASSERT(77 == value);

            Obj::manipulateElement(&mV, setter1, 0);
            Obj::manipulateElement(&mV, setter2, 1);

            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);

            Obj::resize(&mV, 5);
            ASSERT(5 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 3); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 4); ASSERT( 0 == value);

            Obj::resize(&mV, 3);
            ASSERT(3 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);

            Obj::resize(&mV, 0);
            ASSERT(0 == Obj::size(V));
        }

        {
            if (verbose) cout << "Testing vector specialization" << endl;
            bsl::vector<int> mV;  const bsl::vector<int>& V = mV;
            mV.push_back(66);
            mV.push_back(77);

            ASSERT(2 == Obj::size(V));

            int              value;
            GetValue<int>    getter(&value);
            AssignValue<int> setter1(33);
            AssignValue<int> setter2(44);

            Obj::accessElement(V, getter, 0); ASSERT(66 == value);
            Obj::accessElement(V, getter, 1); ASSERT(77 == value);

            Obj::manipulateElement(&mV, setter1, 0);
            Obj::manipulateElement(&mV, setter2, 1);

            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);

            Obj::resize(&mV, 5);
            ASSERT(5 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 3); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 4); ASSERT( 0 == value);

            Obj::resize(&mV, 3);
            ASSERT(3 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);

            Obj::resize(&mV, 0);
            ASSERT(0 == Obj::size(V));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
