// bdlat_arrayfunctions.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlat_arrayfunctions.h>

#include <bslim_testutil.h>

#include <bdlat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bslmf_if.h>
#include <bslmf_issame.h>             // for testing only

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
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 2] struct IsArray<TYPE>
// [ 2] struct ElementType<TYPE>
// [ 1] METHOD FORWARDING TEST
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace Obj = bdlat_ArrayFunctions;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <class LVALUE_TYPE>
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
        // Assign the specified 'object' to '*d_destination_p'.

    template <class RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Do nothing with the specified 'object'.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <class RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    const RVALUE_TYPE& d_value;  // held, not owned

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    int operator()(RVALUE_TYPE *object) const;
        // Assign 'd_value' to the specified '*object'.

    template <class LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Do nothing with the specified 'object'.
};

                              // ================
                              // class FixedArray
                              // ================

namespace Test {

template <int SIZE, class TYPE>
class FixedArray
{
    // Fixed-sized array that conforms to the 'bdlat_ArrayFunctions'
    // interface.

    TYPE d_values[SIZE];
    int  d_length;

  public:
    FixedArray();

    // Compiler-generated functions:
    //  FixedArray(const FixedArray&);
    //  FixedArray& operator=(const FixedArray&);
    //  ~FixedArray();

    // MANIPULATORS
    void append(const TYPE& v);

    void resize(int newSize);

    template <class MANIPULATOR>
    int manipulateElement(MANIPULATOR& manip, int index);

    // ACCESSORS
    int length() const;

    template <class ACCESSOR>
    int accessElement(ACCESSOR& acc, int index) const;
};

// FREE MANIPULATORS
template <int SIZE, class TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(FixedArray<SIZE, TYPE> *array,
                                 MANIPULATOR&            manipulator,
                                 int                     index);

template <int SIZE, class TYPE>
void bdlat_arrayResize(FixedArray<SIZE, TYPE> *array, int newSize);

// FREE ACCESSORS
template <int SIZE, class TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(const FixedArray<SIZE, TYPE>& array,
                             ACCESSOR&                     accessor,
                             int                           index);

template <int SIZE, class TYPE>
bsl::size_t bdlat_arraySize(const FixedArray<SIZE, TYPE>& array);
    // Return the number of elements in the specified 'array'.

}  // close namespace Test

namespace BloombergLP {
namespace bdlat_ArrayFunctions {
    // META FUNCTIONS
    template <int SIZE, class TYPE>
    struct ElementType<Test::FixedArray<SIZE, TYPE> > {
        typedef TYPE Type;
    };

    template <int SIZE, class TYPE>
    struct IsArray<Test::FixedArray<SIZE, TYPE> > : public bslmf::MetaInt<1> {
    };
}  // close namespace bdlat_ArrayFunctions
}  // close enterprise namespace



                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <class LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object) const
{
    *d_lValue_p = object;
    return 0;
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <class RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object) const
{
    *object = d_value;
    return 0;
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object) const
{
    return -1;
}

                           // ----------------------
                           // class Test::FixedArray
                           // ----------------------

template <int SIZE, class TYPE>
inline
Test::FixedArray<SIZE, TYPE>::FixedArray()
: d_length(0)
{
}

template <int SIZE, class TYPE>
inline
void Test::FixedArray<SIZE, TYPE>::append(const TYPE& v)
{
    d_values[d_length++] = v;
}

template <int SIZE, class TYPE>
void Test::FixedArray<SIZE, TYPE>::resize(int newSize)
{
    // If growing, then null out new elements
    for (int i = d_length; i < newSize; ++i) {
        d_values[i] = TYPE();
    }

    d_length = newSize;
}

template <int SIZE, class TYPE>
template <class MANIPULATOR>
inline
int Test::FixedArray<SIZE, TYPE>::manipulateElement(MANIPULATOR& manip,
                                                    int          index)
{
    return manip(&d_values[index]);
}

template <int SIZE, class TYPE>
inline
int Test::FixedArray<SIZE, TYPE>::length() const
{
    return d_length;
}

template <int SIZE, class TYPE>
template <class ACCESSOR>
inline
int Test::FixedArray<SIZE, TYPE>::accessElement(ACCESSOR& acc, int index) const
{
    return acc(d_values[index]);
}

// FREE MANIPULATORS
template <int SIZE, class TYPE, class MANIPULATOR>
int Test::bdlat_arrayManipulateElement(Test::FixedArray<SIZE, TYPE> *array,
                                       MANIPULATOR&                  manip,
                                       int                           index)
{
    return array->manipulateElement(manip, index);
}

template <int SIZE, class TYPE>
void Test::bdlat_arrayResize(Test::FixedArray<SIZE, TYPE> *array, int newSize)
{
    array->resize(newSize);
}

// FREE ACCESSORS
template <int SIZE, class TYPE, class ACCESSOR>
int Test::bdlat_arrayAccessElement(const Test::FixedArray<SIZE, TYPE>& array,
                                   ACCESSOR&                           acc,
                                   int                                 index)
{
    return array.accessElement(acc, index);
}

template <int SIZE, class TYPE>
bsl::size_t Test::bdlat_arraySize(const Test::FixedArray<SIZE, TYPE>& array)
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

namespace BDLAT_ARRAYFUNCTIONS_USAGE_EXAMPLE {
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
    template <class TYPE>
    int execute(const TYPE& value, IsNotArrayType)
    {
        enum { SUCCESS = 0 };

        if (d_verbose)
            (*d_stream_p) << value << ' ';

        return SUCCESS;
    }

    template <class TYPE>
    int execute(const TYPE& value, IsArrayType)
    {
        enum { SUCCESS = 0, FAILURE = -1 };

        int numElements = bdlat_ArrayFunctions::size(value);

        for (int index = 0; index < numElements; ++index) {
            if (0 != bdlat_ArrayFunctions::accessElement(value,
                                                         *this,
                                                         index)) {
                return FAILURE;                                       // RETURN
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
    template <class TYPE>
    int operator()(const TYPE& value)
    {
        typedef typename
        bslmf::If<bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
                 IsArrayType,
                 IsNotArrayType>::Type Toggle;

        return execute(value, Toggle());
    }
};  // end 'class PrintValue'
//..
// The 'PrintValue' function class can be used for types that expose "array"
// behavior through the 'bdlat_ArrayFunctions' 'namespace' (e.g.,
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
}  // close namespace BDLAT_ARRAYFUNCTIONS_USAGE_EXAMPLE

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

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
          using namespace BDLAT_ARRAYFUNCTIONS_USAGE_EXAMPLE;
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

        ASSERT(0 == bdlat_ArrayFunctions::IsArray<int>::VALUE);

        typedef
            Obj::ElementType<Test::FixedArray<9, short> >::Type FAElementType;
        ASSERT(1 ==
           (bdlat_ArrayFunctions::IsArray<Test::FixedArray<3, char> >::VALUE));
        ASSERT(1 == (bslmf::IsSame<FAElementType, short>::VALUE));

        typedef Obj::ElementType<bsl::vector<int> >::Type VecElementType;
        ASSERT(1 == bdlat_ArrayFunctions::IsArray<bsl::vector<int> >::VALUE);
        ASSERT(1 == (bslmf::IsSame<VecElementType, int>::VALUE));

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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
