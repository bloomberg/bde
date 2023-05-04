// bdlb_arrayutil.t.cpp                                               -*-C++-*-
#include <bdlb_arrayutil.h>

#include <bslim_testutil.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_string.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] BEHAVIOR TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
//                       GLOBAL TYPEDEFS/CONSTANTS
// ----------------------------------------------------------------------------

struct UserDefined
{
};


// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
namespace BDLB_ARRAYUTIL_USAGE_EXAMPLE {

void usePrimes(bool verbose);
    // Demonstrate accessing a statically defined array using syntax similar to
    // that of standard containers.  If the specified 'verbose' is 'true',
    // write a trace message to standard output.

void query(bsl::string       *result,
           const bsl::string *columns,
           int                numberOfColumns);
    // Load to the specified 'result' the contents of the specified
    // 'numberOfColumns' table 'columns'.

void checkData(const bsl::vector<bsl::string>& data);
    // Assert that the specified 'data' equals the expected values.

void loadData(bsl::vector<bsl::string> *data);
    // Load the specified 'data' the results of querying an unspecified
    // database table for certain columns.

void getAndCheckData(bool verbose);
    // Assert that the values obtained from a certain database equal their
    // expected values.  If the specified 'verbose' is 'true', write a trace
    // message to standard output.

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// When creating a sequence of values it is often easy to write the sequence as
// an initialized array and use this array to initialize a container.  Since
// the array's length may be changed as the program is maintained, the code
// using the array should automatically determine the array's length or
// automatically determine iterators to the beginning and the end of the array.
//
// For example, to initialize a 'bsl::vector<int>' with the first few prime
// numbers stored in an array the following code uses the 'begin' and 'end'
// methods of 'bdlb::ArrayUtil':
//..
    void usePrimes(bool verbose)
    {
        if (verbose) cout << "\n" << "'usePrimes'" << "\n"
                                  << "===========" << "\n";

        const int        primes[] = { 2, 3, 5, 7, 11, 13, 17 };
        bsl::vector<int> values(bdlb::ArrayUtil::begin(primes),
                                bdlb::ArrayUtil::end(primes));

        ASSERT(values.size() == bdlb::ArrayUtil::size(primes));
//..
// Notice that, after constructing 'values' with the content of the array
// 'primes' the assertion verifies that the correct number of values is stored
// in 'values'.
//
// When the length is needed as a constant expression, e.g., to use it for the
// length of another array, the macro 'BDLB_ARRAYUTIL_LENGTH(array)' can be
// used:
//..
        int reversePrimes[BDLB_ARRAYUTIL_SIZE(primes)];

        bsl::copy(values.rbegin(),
                  values.rend(),
                  bdlb::ArrayUtil::begin(reversePrimes));

        ASSERT(bsl::mismatch(bdlb::ArrayUtil::rbegin(primes),
                             bdlb::ArrayUtil::rend(primes),
                             bdlb::ArrayUtil::begin(reversePrimes)).second
               == bdlb::ArrayUtil::end(reversePrimes));
    }
//..
// After defining the array 'reversePrimes' with the same length as 'primes'
// the elements of 'values' are copied in reverse order into this array.  The
// assertion verifies that 'reversePrimes' contains the values from 'primes'
// but in reverse order: 'bsl::mismatch' is used with a reverse sequence of
// 'primes' by using the 'rbegin' and 'rend' methods for 'primes' and normal
// sequence using the 'begin' and 'end' methods for 'reversePrimes'.
//
///Example 2: Use with Database Interfaces
///- - - - - - - - - - - - - - - - - - - -
// The functions 'begin', 'end', and 'size' provided by this component are
// similar to functions provided by containers.  The main difference is that
// they reside in a utility component rather than being member functions.
//
// A typical use case for the 'size' function is a function expecting a pointer
// to a sequence of keys (e.g., columns in a database) and the number of the
// keys in the sequence:
//..
    void query(bsl::string       *result,
               const bsl::string *columns,
               size_t             numberOfColumns)
    {

        // Query the database.

        for (size_t i = 0; i != numberOfColumns; ++i) {
            result[i] = "queried " + columns[i];
        }
    }

    void loadData(bsl::vector<bsl::string> *data)
    {
        const bsl::string columns[] = { "column1", "column2", "column3" };
        bsl::string       result[BDLB_ARRAYUTIL_SIZE(columns)];

        query(result, columns, bdlb::ArrayUtil::size(columns));
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
//: o The length of 'result' should match the length of 'columns'.  When
//:   specifying the length of an array a constant expression is necessary.  In
//:   C++ 2011 the function 'bdlb::ArrayUtil::size' could return a constant
//:   expression but compilers not, yet, implementing the standard a trick must
//:   be used (using 'sizeof' with a reference to suitably sized array of
//:   'char').  This trick is packaged into the macro 'BDLB_ARRAYUTIL_SIZE()'.
//:
//: o When the length is needed in a context where a 'const' expression is not
//:   required, e.g., when calling 'query', the `bdlb::ArrayUtil::size'
//:   function can be used with the array.
//:
//: o The 'bdlb::ArrayUtil::begin' and 'bdlb::ArrayUtil::end' functions are
//:   used to obtain 'begin' and 'end' iterators used with the vector's
//:   'assign' function to put the 'result' obtained from the call to 'query'
//:   into the 'vector' pointed to by 'data'.
//
// Similar needs for an array of a sequence of values frequently arise when
// using one of the database interfaces.
//
// Another common use case are test cases where the content of a computed
// sequence must be compared with an expected result:
//..
    void checkData(const bsl::vector<bsl::string>& data)
    {
        const bsl::string expect[] = { "queried column1",
                                       "queried column2",
                                       "queried column3"
                                     };
        ASSERT(data.size() == bdlb::ArrayUtil::size(expect));
        ASSERT(bdlb::ArrayUtil::end(expect)
            == bsl::mismatch(bdlb::ArrayUtil::begin(expect),
                             bdlb::ArrayUtil::end(expect),
                             data.begin()).first);
    }
//..
// In the code below the actual result in 'data' is compared to the values in
// the array 'expect':
//
//: 1 We make sure that the lengths of 'data' and 'expect' are identical using
//:   'bdlb::ArrayUtil::size'.
//:
//: 2 The sequences are compared using the 'mismatch' algorithm: To get the
//:   begin and of the 'expect' array 'bdlb::ArrayUtil::begin' and
//:   'bdlb::ArrayUtil::end', respectively, are used.
//..
    void getAndCheckData(bool verbose)
    {
        if (verbose) cout << "\n" << "'getAndCheckData'" << "\n"
                                  << "=================" << "\n";
        bsl::vector<bsl::string> data;
        loadData(&data);
        checkData(data);
    }
//..
}  // close namespace BDLB_ARRAYUTIL_USAGE_EXAMPLE

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int     test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) cout << "\n" "USAGE EXAMPLE" "\n"
                                  "=============" "\n";

        BDLB_ARRAYUTIL_USAGE_EXAMPLE::usePrimes(verbose);
        BDLB_ARRAYUTIL_USAGE_EXAMPLE::getAndCheckData(verbose);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BEHAVIOR TEST
        //   Verify all function templates produce the expected result.
        //
        // Concerns:
        //: 1 Verify all function produce the expected results.
        //: 2 Verify different qualifiers work.
        //: 3 Verify different array sizes
        //
        // Plan:
        //: 1 Try all functions.
        //: 2 Try them with built-in and user-defined types.
        //: 3 Try them with both 'const', 'volatile' and non-'const' versions
        //: 4 Use different array sizes for the various types.
        //
        // Testing:
        //   BEHAVIOR TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" << "BEHAVIOR TEST" << "\n"
                                  << "=============" << "\n";

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
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Verify all function templates are callable.
        //
        // Concerns:
        //: 1 Both 'const' and non-'const' arrays should work.
        //: 2 The functions should be callable with arbitrary types.
        //: 3 The functions should be applicable to different array sizes.
        //
        // Plan:
        //: 1 Try all functions.
        //: 2 Try them with built-in and user-defined types.
        //: 3 Try them with 'const', 'volatile' and non-'const' versions.
        //: 4 Use different array sizes for the various types.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\n" << "BREATHING TEST" << "\n"
                                  << "==============" << "\n";

        const int constIntArray[] = { 1 };
        bdlb::ArrayUtil::begin(constIntArray);
        bdlb::ArrayUtil::end(constIntArray);
        bdlb::ArrayUtil::rbegin(constIntArray);
        bdlb::ArrayUtil::rend(constIntArray);
        bdlb::ArrayUtil::size(constIntArray);
        bdlb::ArrayUtil::length(constIntArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(constIntArray));
        BDLB_ARRAYUTIL_SIZE(constIntArray);
        BDLB_ARRAYUTIL_LENGTH(constIntArray);

        volatile int volatileIntArray[] = { 1, 2 };
        bdlb::ArrayUtil::begin(volatileIntArray);
        bdlb::ArrayUtil::end(volatileIntArray);
        bdlb::ArrayUtil::rbegin(volatileIntArray);
        bdlb::ArrayUtil::rend(volatileIntArray);
        bdlb::ArrayUtil::size(volatileIntArray);
        bdlb::ArrayUtil::length(volatileIntArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(volatileIntArray));
        BDLB_ARRAYUTIL_SIZE(volatileIntArray);
        BDLB_ARRAYUTIL_LENGTH(volatileIntArray);

        int intArray[] = { 1, 2, 3 };
        bdlb::ArrayUtil::begin(intArray);
        bdlb::ArrayUtil::end(intArray);
        bdlb::ArrayUtil::rbegin(intArray);
        bdlb::ArrayUtil::rend(intArray);
        bdlb::ArrayUtil::size(intArray);
        bdlb::ArrayUtil::length(intArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(intArray));
        BDLB_ARRAYUTIL_SIZE(intArray);
        BDLB_ARRAYUTIL_LENGTH(intArray);

        const UserDefined constUserDefinedArray[] = { UserDefined() };
        bdlb::ArrayUtil::begin(constUserDefinedArray);
        bdlb::ArrayUtil::end(constUserDefinedArray);
        bdlb::ArrayUtil::rbegin(constUserDefinedArray);
        bdlb::ArrayUtil::rend(constUserDefinedArray);
        bdlb::ArrayUtil::size(constUserDefinedArray);
        bdlb::ArrayUtil::length(constUserDefinedArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(constUserDefinedArray));
        BDLB_ARRAYUTIL_SIZE(constUserDefinedArray);
        BDLB_ARRAYUTIL_LENGTH(constUserDefinedArray);

        volatile UserDefined volatileUserDefinedArray[5];
        bdlb::ArrayUtil::begin(volatileUserDefinedArray);
        bdlb::ArrayUtil::end(volatileUserDefinedArray);
        bdlb::ArrayUtil::rbegin(volatileUserDefinedArray);
        bdlb::ArrayUtil::rend(volatileUserDefinedArray);
        bdlb::ArrayUtil::size(volatileUserDefinedArray);
        bdlb::ArrayUtil::length(volatileUserDefinedArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(volatileUserDefinedArray));
        BDLB_ARRAYUTIL_SIZE(volatileUserDefinedArray);
        BDLB_ARRAYUTIL_LENGTH(volatileUserDefinedArray);

        UserDefined userDefinedArray[1234];
        bdlb::ArrayUtil::begin(userDefinedArray);
        bdlb::ArrayUtil::end(userDefinedArray);
        bdlb::ArrayUtil::rbegin(userDefinedArray);
        bdlb::ArrayUtil::rend(userDefinedArray);
        bdlb::ArrayUtil::size(userDefinedArray);
        bdlb::ArrayUtil::length(userDefinedArray);
        (void) sizeof(bdlb::ArrayUtil::sizer(userDefinedArray));
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
