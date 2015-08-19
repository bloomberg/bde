// bdlb_arrayutil.t.cpp                                               -*-C++-*-

#include <bdlb_arrayutil.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_string.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST - covering the small functionality there is
// [ 2] BEHAVIOR TEST - verify the methods yield the expected results
// [ 3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                       GLOBAL TYPEDEFS/CONSTANTS
//-----------------------------------------------------------------------------

struct UserDefined
{
};


//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BDEU_ARRAYUTIL_USAGE_EXAMPLE {
    void usePrimes()
    {
///Usage Example 1
///---------------
// When creating a sequence of values it is often easy to write the sequence as
// an initialized array and use this array to initialize a container.  Since
// the array's length may get adjusted in during when the program is
// maintained, the code using the array should automatically determine the
// array's length or automatically determine iterators to the beginning and the
// end of the array.  For example, to initialize a 'bsl::vector<int>' with the
// first few prime numbers stored in an array the following code uses the
// 'begin' and 'end' methods of 'bdlb::ArrayUtil':
//..
    const int        primes[] = { 2, 3, 5, 7, 11, 13, 17 };
    bsl::vector<int> values(bdlb::ArrayUtil::begin(primes),
                            bdlb::ArrayUtil::end(primes));

    ASSERT(values.size() == bdlb::ArrayUtil::size(primes));
//..
// After constructing 'values' with the content of the array 'primes' the
// assertion verifies that the correct number of values is stored in 'values'.
// When the length is needed as a constant expression, e.g., to use it for the
// length of another array, the macro 'BDLB_ARRAYUTIL_LENGTH(array)' can be
// used:
//..
    int reversePrimes[BDLB_ARRAYUTIL_SIZE(primes)];
    bsl::copy(values.rbegin(), values.rend(),
              bdlb::ArrayUtil::begin(reversePrimes));

    ASSERT(bsl::mismatch(bdlb::ArrayUtil::rbegin(primes),
                         bdlb::ArrayUtil::rend(primes),
                         bdlb::ArrayUtil::begin(reversePrimes)).second
           == bdlb::ArrayUtil::end(reversePrimes));
//..
// After defining the array 'reversePrimes' with the same length as 'primes'
// the elements of 'values' are copied in reverse order into this array.  The
// assertion verifies that 'reversePrimes' contains the values from 'primes'
// but in reverse order: 'bsl::mismatch' is used with a reverse sequence of
// 'primes' by using the 'rbegin' and 'rend' methods for 'primes' and normal
// sequence using the 'begin' and 'end' methods for 'reversePrimes'.
//..
    }
//..
//
///Usage Example 2
///---------------
// The functions 'begin', 'end', and 'size' provided by this component are
// similar to functions provided by containers.  The main difference is that
// they reside in a utility component rather than being member functions.
//
// A typical use case for the 'size' function is a function expecting a pointer
// to a sequence of keys (e.g., columns in a database) and the number of the
// keys in the sequence:
//..
    void query(const bsl::string  *columns,
               int                 numberOfColumns,
               bsl::string        *result)
    {
        for (int i = 0; i != numberOfColumns; ++i) {
            result[i] = "queried " + columns[i];
        }
    }

    void loadData(bsl::vector<bsl::string> *data)
    {
        const bsl::string columns[] = { "column1", "column2", "column3" };
        bsl::string       result[BDLB_ARRAYUTIL_SIZE(columns)];
        query(columns, bdlb::ArrayUtil::size(columns), result);
        data->assign(bdlb::ArrayUtil::begin(result),
                     bdlb::ArrayUtil::end(result));
    }
//..
// The 'loadData' function shows how to use the different function templates.
// The array 'columns' doesn't have a length specified.  It is determined from
// the number of elements it is initialized with.  In this case it is easy to
// see that there are three elements but in real situations the number of
// elements can be non-trivial to get right.  Also, changing the number of
// elements would make it necessary to apply the corresponding change in
// multiple places.  Thus, the length is determined using 'bdlb::ArrayUtil':
//
// * The length of 'result' should match the length of 'columns'.  When
//   specifying the length of an array a constant expression is necessary.  In
//   C++ 2011 the function 'bdlb::ArrayUtil::size' could return a constant
//   expression but compilers not, yet, implementing the standard a trick needs
//   to be used (using 'sizeof' with a reference to suitably sized array of
//   'char').  This trick is packaged into the macro 'BDLB_ARRAYUTIL_SIZE()'.
// * When the length is needed in a context where a const expression isn't
//   required, e.g., when calling 'query', the `bdlb::ArrayUtil::size' function
//   can be used with the array.
// * The 'bdlb::ArrayUtil::begin' and 'bdlb::ArrayUtil::end' functions are used
//   to obtain begin and end iterators used with the vector's 'assign' function
//   to put the 'result' obtained from the call to 'query' into the 'vector'
//   pointed to by 'data'.
//
// Similar needs for an array of a sequence of values frequently arise when
// using one of the database interfaces.  Another common use case are test
// cases where the content of a somehow computed sequence needs to be compared
// with an expected result:
//..
    void checkData(const bsl::vector<bsl::string>& data)
    {
        const bsl::string expect[] = {
            "queried column1", "queried column2", "queried column3"
        };
        ASSERT(data.size() == bdlb::ArrayUtil::size(expect));
        ASSERT(bdlb::ArrayUtil::end(expect)
               == bsl::mismatch(bdlb::ArrayUtil::begin(expect),
                                bdlb::ArrayUtil::end(expect),
                                data.begin()).first);
    }
//..
// In this code the actual result in 'data' is compared to the values in the
// array 'expect':
//
// * First it is made sure that the lengths of 'data' and 'expect' are
//   identical using 'bdlb::ArrayUtil::size'.
// * Next, the sequences are compared using the 'mismatch' algorithm: To get
//   the begin and of the 'expect' array 'bdlb::ArrayUtil::begin' and
//   'bdlb::ArrayUtil::end', respectively, are used.
//..
    void usageExample(bool verbose)
    {
        if (verbose) cout << "\nUsing Basic Functions"
                          << "\n--------------" << endl;
        bsl::vector<bsl::string> data;
        loadData(&data);
        checkData(data);
    }
//..
}  // close namespace BDEU_ARRAYUTIL_USAGE_EXAMPLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int     test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
        BDEU_ARRAYUTIL_USAGE_EXAMPLE::usageExample(verbose);
        BDEU_ARRAYUTIL_USAGE_EXAMPLE::usePrimes();
      } break;
      case 2: {
        // BEHAVIOR TEST
        //   Verify all function templates produce the expected result.
        //
        // Concerns:
        //   - Verify all function produce the expected results.
        //   - Verify different qualifiers work.
        //   - Verify different array sizes
        //
        // Plan:
        //   - Try all functions.
        //   - Try them with built-in and user-defined types.
        //   - Try them with both const, volatile and non-const versions
        //   - Use different array sizes for the various types.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BEHAVIOR TEST" << endl
                          << "==============" << endl;

        const int constIntArray[] = { 1 };
        ASSERT(constIntArray     == bdlb::ArrayUtil::begin(constIntArray));
        ASSERT(constIntArray + 1 == bdlb::ArrayUtil::end(constIntArray));
        ASSERT(bsl::reverse_iterator<const int*>(constIntArray + 1)
               == bdlb::ArrayUtil::rbegin(constIntArray));
        ASSERT(bsl::reverse_iterator<const int*>(constIntArray)
               == bdlb::ArrayUtil::rend(constIntArray));
        ASSERT(1 == bdlb::ArrayUtil::size(constIntArray));
        ASSERT(1 == bdlb::ArrayUtil::length(constIntArray));
        ASSERT(1 == sizeof(bdlb::ArrayUtil::sizer(constIntArray)));
        ASSERT(1 == BDLB_ARRAYUTIL_SIZE(constIntArray));
        ASSERT(1 == BDLB_ARRAYUTIL_LENGTH(constIntArray));

        volatile int volatileIntArray[] = { 1, 2 };
        ASSERT(volatileIntArray == bdlb::ArrayUtil::begin(volatileIntArray));
        ASSERT(volatileIntArray + 2 == bdlb::ArrayUtil::end(volatileIntArray));
        ASSERT(bsl::reverse_iterator<volatile int*>(volatileIntArray + 2)
               == bdlb::ArrayUtil::rbegin(volatileIntArray));
        ASSERT(bsl::reverse_iterator<volatile int*>(volatileIntArray)
               == bdlb::ArrayUtil::rend(volatileIntArray));
        ASSERT(2 == bdlb::ArrayUtil::size(volatileIntArray));
        ASSERT(2 == bdlb::ArrayUtil::length(volatileIntArray));
        ASSERT(2 == sizeof(bdlb::ArrayUtil::sizer(volatileIntArray)));
        ASSERT(2 == BDLB_ARRAYUTIL_SIZE(volatileIntArray));
        ASSERT(2 == BDLB_ARRAYUTIL_LENGTH(volatileIntArray));

        int intArray[] = { 1, 2, 3 };
        ASSERT(intArray     == bdlb::ArrayUtil::begin(intArray));
        ASSERT(intArray + 3 == bdlb::ArrayUtil::end(intArray));
        ASSERT(bsl::reverse_iterator<int*>(intArray + 3)
               == bdlb::ArrayUtil::rbegin(intArray));
        ASSERT(bsl::reverse_iterator<int*>(intArray)
               == bdlb::ArrayUtil::rend(intArray));
        ASSERT(3 == bdlb::ArrayUtil::size(intArray));
        ASSERT(3 == bdlb::ArrayUtil::length(intArray));
        ASSERT(3 == sizeof(bdlb::ArrayUtil::sizer(intArray)));
        ASSERT(3 == BDLB_ARRAYUTIL_SIZE(intArray));
        ASSERT(3 == BDLB_ARRAYUTIL_LENGTH(intArray));

        const UserDefined constUserDefinedArray[] = { UserDefined() };
        ASSERT(constUserDefinedArray
               == bdlb::ArrayUtil::begin(constUserDefinedArray));
        ASSERT(constUserDefinedArray + 1
               == bdlb::ArrayUtil::end(constUserDefinedArray));
        ASSERT(bsl::reverse_iterator<const UserDefined*>(constUserDefinedArray
                                                         + 1)
               == bdlb::ArrayUtil::rbegin(constUserDefinedArray));
        ASSERT(bsl::reverse_iterator<const UserDefined*>(constUserDefinedArray)
               == bdlb::ArrayUtil::rend(constUserDefinedArray));
        ASSERT(1 == bdlb::ArrayUtil::size(constUserDefinedArray));
        ASSERT(1 == bdlb::ArrayUtil::length(constUserDefinedArray));
        ASSERT(1 == sizeof(bdlb::ArrayUtil::sizer(constUserDefinedArray)));
        ASSERT(1 == BDLB_ARRAYUTIL_SIZE(constUserDefinedArray));
        ASSERT(1 == BDLB_ARRAYUTIL_LENGTH(constUserDefinedArray));

        volatile UserDefined volatileUserDefinedArray[5];
        ASSERT(volatileUserDefinedArray
               == bdlb::ArrayUtil::begin(volatileUserDefinedArray));
        ASSERT(volatileUserDefinedArray + 5
               == bdlb::ArrayUtil::end(volatileUserDefinedArray));
        ASSERT(bsl::reverse_iterator<volatile UserDefined*>(
                                                  volatileUserDefinedArray + 5)
               == bdlb::ArrayUtil::rbegin(volatileUserDefinedArray));
        ASSERT(bsl::reverse_iterator<volatile UserDefined*>(
                                                      volatileUserDefinedArray)
               == bdlb::ArrayUtil::rend(volatileUserDefinedArray));
        ASSERT(5 == bdlb::ArrayUtil::size(volatileUserDefinedArray));
        ASSERT(5 == bdlb::ArrayUtil::length(volatileUserDefinedArray));
        ASSERT(5 == sizeof(bdlb::ArrayUtil::sizer(volatileUserDefinedArray)));
        ASSERT(5 == BDLB_ARRAYUTIL_SIZE(volatileUserDefinedArray));
        ASSERT(5 == BDLB_ARRAYUTIL_LENGTH(volatileUserDefinedArray));

        UserDefined userDefinedArray[1234];
        ASSERT(userDefinedArray == bdlb::ArrayUtil::begin(userDefinedArray));
        ASSERT(userDefinedArray + 1234
               == bdlb::ArrayUtil::end(userDefinedArray));
        ASSERT(bsl::reverse_iterator<UserDefined*>(userDefinedArray + 1234)
               == bdlb::ArrayUtil::rbegin(userDefinedArray));
        ASSERT(bsl::reverse_iterator<UserDefined*>(userDefinedArray)
               == bdlb::ArrayUtil::rend(userDefinedArray));
        ASSERT(1234 == bdlb::ArrayUtil::size(userDefinedArray));
        ASSERT(1234 == bdlb::ArrayUtil::length(userDefinedArray));
        ASSERT(1234 == sizeof(bdlb::ArrayUtil::sizer(userDefinedArray)));
        ASSERT(1234 == BDLB_ARRAYUTIL_SIZE(userDefinedArray));
        ASSERT(1234 == BDLB_ARRAYUTIL_LENGTH(userDefinedArray));

      } break;
      case 1: {
        // BREATHING TEST
        //   Verify all function templates are callable.
        //
        // Concerns:
        //   - Both const and non-const arrays should work.
        //   - The functions should be callable with arbitrary types.
        //   - The functions should be applicable to different array sizes.
        //
        // Plan:
        //   - Try all functions.
        //   - Try them with built-in and user-defined types.
        //   - Try them with both const, volatile and non-const versions
        //   - Use different array sizes for the various types.
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int constIntArray[] = { 1 };
        bdlb::ArrayUtil::begin(constIntArray);
        bdlb::ArrayUtil::end(constIntArray);
        bdlb::ArrayUtil::rbegin(constIntArray);
        bdlb::ArrayUtil::rend(constIntArray);
        bdlb::ArrayUtil::size(constIntArray);
        bdlb::ArrayUtil::length(constIntArray);
        sizeof(bdlb::ArrayUtil::sizer(constIntArray));
        BDLB_ARRAYUTIL_SIZE(constIntArray);
        BDLB_ARRAYUTIL_LENGTH(constIntArray);

        volatile int volatileIntArray[] = { 1, 2 };
        bdlb::ArrayUtil::begin(volatileIntArray);
        bdlb::ArrayUtil::end(volatileIntArray);
        bdlb::ArrayUtil::rbegin(volatileIntArray);
        bdlb::ArrayUtil::rend(volatileIntArray);
        bdlb::ArrayUtil::size(volatileIntArray);
        bdlb::ArrayUtil::length(volatileIntArray);
        sizeof(bdlb::ArrayUtil::sizer(volatileIntArray));
        BDLB_ARRAYUTIL_SIZE(volatileIntArray);
        BDLB_ARRAYUTIL_LENGTH(volatileIntArray);

        int intArray[] = { 1, 2, 3 };
        bdlb::ArrayUtil::begin(intArray);
        bdlb::ArrayUtil::end(intArray);
        bdlb::ArrayUtil::rbegin(intArray);
        bdlb::ArrayUtil::rend(intArray);
        bdlb::ArrayUtil::size(intArray);
        bdlb::ArrayUtil::length(intArray);
        sizeof(bdlb::ArrayUtil::sizer(intArray));
        BDLB_ARRAYUTIL_SIZE(intArray);
        BDLB_ARRAYUTIL_LENGTH(intArray);

        const UserDefined constUserDefinedArray[] = { UserDefined() };
        bdlb::ArrayUtil::begin(constUserDefinedArray);
        bdlb::ArrayUtil::end(constUserDefinedArray);
        bdlb::ArrayUtil::rbegin(constUserDefinedArray);
        bdlb::ArrayUtil::rend(constUserDefinedArray);
        bdlb::ArrayUtil::size(constUserDefinedArray);
        bdlb::ArrayUtil::length(constUserDefinedArray);
        sizeof(bdlb::ArrayUtil::sizer(constUserDefinedArray));
        BDLB_ARRAYUTIL_SIZE(constUserDefinedArray);
        BDLB_ARRAYUTIL_LENGTH(constUserDefinedArray);

        volatile UserDefined volatileUserDefinedArray[5];
        bdlb::ArrayUtil::begin(volatileUserDefinedArray);
        bdlb::ArrayUtil::end(volatileUserDefinedArray);
        bdlb::ArrayUtil::rbegin(volatileUserDefinedArray);
        bdlb::ArrayUtil::rend(volatileUserDefinedArray);
        bdlb::ArrayUtil::size(volatileUserDefinedArray);
        bdlb::ArrayUtil::length(volatileUserDefinedArray);
        sizeof(bdlb::ArrayUtil::sizer(volatileUserDefinedArray));
        BDLB_ARRAYUTIL_SIZE(volatileUserDefinedArray);
        BDLB_ARRAYUTIL_LENGTH(volatileUserDefinedArray);

        UserDefined userDefinedArray[1234];
        bdlb::ArrayUtil::begin(userDefinedArray);
        bdlb::ArrayUtil::end(userDefinedArray);
        bdlb::ArrayUtil::rbegin(userDefinedArray);
        bdlb::ArrayUtil::rend(userDefinedArray);
        bdlb::ArrayUtil::size(userDefinedArray);
        bdlb::ArrayUtil::length(userDefinedArray);
        sizeof(bdlb::ArrayUtil::sizer(userDefinedArray));
        BDLB_ARRAYUTIL_SIZE(userDefinedArray);
        BDLB_ARRAYUTIL_LENGTH(userDefinedArray);

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
