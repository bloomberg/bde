// bdeu_arrayutil.t.cpp                                               -*-C++-*-

#include <bdeu_arrayutil.h>

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

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
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
// When creating a sequence of values it is often easy to write the sequence
// as an initialized array and use this array to initialize a container. Since
// the array's length may get adjusted in during when the program is
// maintained, the code using the array should automatically determine the
// array's length or automatically determine iterators to the beginning and the
// end of the array. For example, to initialize a 'bsl::vector<int>' with the
// first few prime numbers stored in an array the following code uses the
// 'begin()' and 'end()' methods of 'bdeu_ArrayUtil':
//..
    const int        primes[] = { 2, 3, 5, 7, 11, 13, 17 };
    bsl::vector<int> values(bdeu_ArrayUtil::begin(primes),
                            bdeu_ArrayUtil::end(primes));
  
    ASSERT(values.size() == bdeu_ArrayUtil::size(primes));
//..
// After constructing 'values' with the content of the array 'primes' the
// assertion verifies that the correct number of values is stored in 'values'.
// When the length is needed as a constant expression, e.g., to use it for the
// length of another array, the macro 'BDEU_ARRAYUTIL_LENGTH(array)' can be
// used:
//..
    int reversePrimes[BDEU_ARRAYUTIL_SIZE(primes)];
    bsl::copy(values.rbegin(), values.rend(),
              bdeu_ArrayUtil::begin(reversePrimes));
  
    ASSERT(bsl::mismatch(bdeu_ArrayUtil::rbegin(primes),
                         bdeu_ArrayUtil::rend(primes),
                         bdeu_ArrayUtil::begin(reversePrimes)).second
           == bdeu_ArrayUtil::end(reversePrimes));
//..
// After defining the array 'reversePrimes' with the same length as 'primes'
// the elements of 'values' are copied in reverse order into this array. The
// assertion verifies that 'reversePrimes' contains the values from 'primes'
// but in reverse order: 'bsl::mismatch()' is used with a reverse sequence of
// 'primes' by using the 'rbegin()' and 'rend()' methods for 'primes' and
// normal sequence using the 'begin()' and 'end()' methods for 'reversePrimes'.
    }
//
///Usage Example 2
///---------------
// The functions 'begin()', 'end()', and 'size()' provided by this component
// are similar to functions provided by containers. The main difference is that
// they reside in a utility component rather than being member functions.
//
// A typical use case for the 'size()' function is a function expecting a
// pointer to a sequence of keys (e.g., columns in a database) and the number
// of the keys in the sequence:
//..
    void query(const bsl::string*  columns,
               int                 numberOfColumns,
               bsl::string        *result) {
        for (int i = 0; i != numberOfColumns; ++i) {
            result[i] = "queried " + columns[i];
        }
    }
  
    void loadData(bsl::vector<bsl::string> *data) {
        const bsl::string columns[] = { "column1", "column2", "column3" };
        bsl::string       result[BDEU_ARRAYUTIL_SIZE(columns)];
         query(columns, bdeu_ArrayUtil::size(columns), result);
        data->assign(bdeu_ArrayUtil::begin(result),
                     bdeu_ArrayUtil::end(result));
    }
//..
// The 'loadData()' function shows how to use the different function templates.
// The array 'columns' doesn't have a length specified. It is determined from
// the number of elements it is initialized with. In this case it is easy to
// see that there are three elements but in real situations the number of
// elements can be non-trivial to get right. Also, changing the number of
// elements would make it necessary to apply the corresponding change in
// multiple places. Thus, the length is determined using 'bdeu_ArrayUtil':
//
// * The length of 'result' should match the length of 'columns'. When
//   specifying  the length of an array a constant expression is necessary.
//   In C++ 2011 the function 'bdeu_ArrayUtil::size()' could return a constant
//   expression butcompilers not, yet, implementing the standard a trick needs
//   to be used (using 'sizeof' with a reference to suitably sized array of
//   'char'). This trick is packaged into the macro 'BDEU_ARRAYUTIL_SIZE()'.
// * When the length is needed in a context where a const expression isn't
//   required, e.g., when calling 'query()', the `bdeu_ArrayUtil::size()'
//   function can be used with the array.
// * The 'bdeu_ArrayUtil::begin()' and 'bdeu_ArrayUtil::end()' functions are
//   used to obtain begin and end iterators used with the vector's 'assign()'
//   function to put the 'result' obtained from the call to 'query()' into the
//   vector pointed to by 'data'.
//
// Similar needs for an array of a sequence of values frequently arise when
// using one of the database interfaces. Another common use case are test
// cases where the content of a somehow computed sequence needs to be compared
// with an expected result:
//..
    void checkData(const bsl::vector<bsl::string>& data) {
        const bsl::string expect[] = {
            "queried column1", "queried column2", "queried column3"
        };
        ASSERT(data.size() == bdeu_ArrayUtil::size(expect));
        ASSERT(bdeu_ArrayUtil::end(expect)
               == bsl::mismatch(bdeu_ArrayUtil::begin(expect),
                                bdeu_ArrayUtil::end(expect),
                                data.begin()).first);
    }
//..
// In this code the actual result in 'data' is compared to the values in the
// array 'expect':
//
// * First it is made sure that the lengths of 'data' and 'expect' are
//   identical using 'bdeu_ArrayUtil::size()'.
// * Next, the sequences are compared using the 'mismatch()' algorithm: To get
//   the begin and of the 'expect' array 'bdeu_ArrayUtil::begin()' and
//   'bdeu_ArrayUtil::end()', respectively, are used.
//..
    void usageExample(int verbose) {
        if (verbose) cout << "\nUsing Basic Functions"
                          << "\n--------------" << endl;
         bsl::vector<bsl::string> data;
        loadData(&data);
        checkData(data);
    }
//..
// close namespace BDEU_ARRAYUTIL_USAGE_EXAMPLE
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

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
        ASSERT(constIntArray     == bdeu_ArrayUtil::begin(constIntArray));
        ASSERT(constIntArray + 1 == bdeu_ArrayUtil::end(constIntArray));
        ASSERT(bsl::reverse_iterator<const int*>(constIntArray + 1)
               == bdeu_ArrayUtil::rbegin(constIntArray));
        ASSERT(bsl::reverse_iterator<const int*>(constIntArray)
               == bdeu_ArrayUtil::rend(constIntArray));
        ASSERT(1 == bdeu_ArrayUtil::size(constIntArray));
        ASSERT(1 == bdeu_ArrayUtil::length(constIntArray));
        ASSERT(1 == sizeof(bdeu_ArrayUtil::sizer(constIntArray)));
        ASSERT(1 == BDEU_ARRAYUTIL_SIZE(constIntArray));
        ASSERT(1 == BDEU_ARRAYUTIL_LENGTH(constIntArray));

        volatile int volatileIntArray[] = { 1, 2 };
        ASSERT(volatileIntArray     == bdeu_ArrayUtil::begin(volatileIntArray));
        ASSERT(volatileIntArray + 2 == bdeu_ArrayUtil::end(volatileIntArray));
        ASSERT(bsl::reverse_iterator<volatile int*>(volatileIntArray + 2)
               == bdeu_ArrayUtil::rbegin(volatileIntArray));
        ASSERT(bsl::reverse_iterator<volatile int*>(volatileIntArray)
               == bdeu_ArrayUtil::rend(volatileIntArray));
        ASSERT(2 == bdeu_ArrayUtil::size(volatileIntArray));
        ASSERT(2 == bdeu_ArrayUtil::length(volatileIntArray));
        ASSERT(2 == sizeof(bdeu_ArrayUtil::sizer(volatileIntArray)));
        ASSERT(2 == BDEU_ARRAYUTIL_SIZE(volatileIntArray));
        ASSERT(2 == BDEU_ARRAYUTIL_LENGTH(volatileIntArray));

        int intArray[] = { 1, 2, 3 };
        ASSERT(intArray     == bdeu_ArrayUtil::begin(intArray));
        ASSERT(intArray + 3 == bdeu_ArrayUtil::end(intArray));
        ASSERT(bsl::reverse_iterator<int*>(intArray + 3)
               == bdeu_ArrayUtil::rbegin(intArray));
        ASSERT(bsl::reverse_iterator<int*>(intArray)
               == bdeu_ArrayUtil::rend(intArray));
        ASSERT(3 == bdeu_ArrayUtil::size(intArray));
        ASSERT(3 == bdeu_ArrayUtil::length(intArray));
        ASSERT(3 == sizeof(bdeu_ArrayUtil::sizer(intArray)));
        ASSERT(3 == BDEU_ARRAYUTIL_SIZE(intArray));
        ASSERT(3 == BDEU_ARRAYUTIL_LENGTH(intArray));

        const UserDefined constUserDefinedArray[] = { UserDefined() };
        ASSERT(constUserDefinedArray
               == bdeu_ArrayUtil::begin(constUserDefinedArray));
        ASSERT(constUserDefinedArray + 1
               == bdeu_ArrayUtil::end(constUserDefinedArray));
        ASSERT(bsl::reverse_iterator<const UserDefined*>(constUserDefinedArray
                                                         + 1)
               == bdeu_ArrayUtil::rbegin(constUserDefinedArray));
        ASSERT(bsl::reverse_iterator<const UserDefined*>(constUserDefinedArray)
               == bdeu_ArrayUtil::rend(constUserDefinedArray));
        ASSERT(1 == bdeu_ArrayUtil::size(constUserDefinedArray));
        ASSERT(1 == bdeu_ArrayUtil::length(constUserDefinedArray));
        ASSERT(1 == sizeof(bdeu_ArrayUtil::sizer(constUserDefinedArray)));
        ASSERT(1 == BDEU_ARRAYUTIL_SIZE(constUserDefinedArray));
        ASSERT(1 == BDEU_ARRAYUTIL_LENGTH(constUserDefinedArray));

        volatile UserDefined volatileUserDefinedArray[5];
        ASSERT(volatileUserDefinedArray
               == bdeu_ArrayUtil::begin(volatileUserDefinedArray));
        ASSERT(volatileUserDefinedArray + 5
               == bdeu_ArrayUtil::end(volatileUserDefinedArray));
        ASSERT(bsl::reverse_iterator<volatile UserDefined*>(
                                                  volatileUserDefinedArray + 5)
               == bdeu_ArrayUtil::rbegin(volatileUserDefinedArray));
        ASSERT(bsl::reverse_iterator<volatile UserDefined*>(
                                                      volatileUserDefinedArray)
               == bdeu_ArrayUtil::rend(volatileUserDefinedArray));
        ASSERT(5 == bdeu_ArrayUtil::size(volatileUserDefinedArray));
        ASSERT(5 == bdeu_ArrayUtil::length(volatileUserDefinedArray));
        ASSERT(5 == sizeof(bdeu_ArrayUtil::sizer(volatileUserDefinedArray)));
        ASSERT(5 == BDEU_ARRAYUTIL_SIZE(volatileUserDefinedArray));
        ASSERT(5 == BDEU_ARRAYUTIL_LENGTH(volatileUserDefinedArray));

        UserDefined userDefinedArray[1234];
        ASSERT(userDefinedArray == bdeu_ArrayUtil::begin(userDefinedArray));
        ASSERT(userDefinedArray + 1234
               == bdeu_ArrayUtil::end(userDefinedArray));
        ASSERT(bsl::reverse_iterator<UserDefined*>(userDefinedArray + 1234)
               == bdeu_ArrayUtil::rbegin(userDefinedArray));
        ASSERT(bsl::reverse_iterator<UserDefined*>(userDefinedArray)
               == bdeu_ArrayUtil::rend(userDefinedArray));
        ASSERT(1234 == bdeu_ArrayUtil::size(userDefinedArray));
        ASSERT(1234 == bdeu_ArrayUtil::length(userDefinedArray));
        ASSERT(1234 == sizeof(bdeu_ArrayUtil::sizer(userDefinedArray)));
        ASSERT(1234 == BDEU_ARRAYUTIL_SIZE(userDefinedArray));
               ASSERT(1234 == BDEU_ARRAYUTIL_LENGTH(userDefinedArray));

      } break;
      case 1: {
        // BREATHING TEST
        //   Verify all function templates are callable.
        //
        // Concerns:
        //   - Both const and non-const arrays should work.
        //   - The functions should be callalable with arbitrary types. 
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
        bdeu_ArrayUtil::begin(constIntArray);
        bdeu_ArrayUtil::end(constIntArray);
        bdeu_ArrayUtil::rbegin(constIntArray);
        bdeu_ArrayUtil::rend(constIntArray);
        bdeu_ArrayUtil::size(constIntArray);
        bdeu_ArrayUtil::length(constIntArray);
        sizeof(bdeu_ArrayUtil::sizer(constIntArray));
        BDEU_ARRAYUTIL_SIZE(constIntArray);
        BDEU_ARRAYUTIL_LENGTH(constIntArray);

        volatile int volatileIntArray[] = { 1, 2 };
        bdeu_ArrayUtil::begin(volatileIntArray);
        bdeu_ArrayUtil::end(volatileIntArray);
        bdeu_ArrayUtil::rbegin(volatileIntArray);
        bdeu_ArrayUtil::rend(volatileIntArray);
        bdeu_ArrayUtil::size(volatileIntArray);
        bdeu_ArrayUtil::length(volatileIntArray);
        sizeof(bdeu_ArrayUtil::sizer(volatileIntArray));
        BDEU_ARRAYUTIL_SIZE(volatileIntArray);
        BDEU_ARRAYUTIL_LENGTH(volatileIntArray);

        int intArray[] = { 1, 2, 3 };
        bdeu_ArrayUtil::begin(intArray);
        bdeu_ArrayUtil::end(intArray);
        bdeu_ArrayUtil::rbegin(intArray);
        bdeu_ArrayUtil::rend(intArray);
        bdeu_ArrayUtil::size(intArray);
        bdeu_ArrayUtil::length(intArray);
        sizeof(bdeu_ArrayUtil::sizer(intArray));
        BDEU_ARRAYUTIL_SIZE(intArray);
        BDEU_ARRAYUTIL_LENGTH(intArray);

        const UserDefined constUserDefinedArray[] = { UserDefined() };
        bdeu_ArrayUtil::begin(constUserDefinedArray);
        bdeu_ArrayUtil::end(constUserDefinedArray);
        bdeu_ArrayUtil::rbegin(constUserDefinedArray);
        bdeu_ArrayUtil::rend(constUserDefinedArray);
        bdeu_ArrayUtil::size(constUserDefinedArray);
        bdeu_ArrayUtil::length(constUserDefinedArray);
        sizeof(bdeu_ArrayUtil::sizer(constUserDefinedArray));
        BDEU_ARRAYUTIL_SIZE(constUserDefinedArray);
        BDEU_ARRAYUTIL_LENGTH(constUserDefinedArray);

        volatile UserDefined volatileUserDefinedArray[5];
        bdeu_ArrayUtil::begin(volatileUserDefinedArray);
        bdeu_ArrayUtil::end(volatileUserDefinedArray);
        bdeu_ArrayUtil::rbegin(volatileUserDefinedArray);
        bdeu_ArrayUtil::rend(volatileUserDefinedArray);
        bdeu_ArrayUtil::size(volatileUserDefinedArray);
        bdeu_ArrayUtil::length(volatileUserDefinedArray);
        sizeof(bdeu_ArrayUtil::sizer(volatileUserDefinedArray));
        BDEU_ARRAYUTIL_SIZE(volatileUserDefinedArray);
        BDEU_ARRAYUTIL_LENGTH(volatileUserDefinedArray);

        UserDefined userDefinedArray[1234];
        bdeu_ArrayUtil::begin(userDefinedArray);
        bdeu_ArrayUtil::end(userDefinedArray);
        bdeu_ArrayUtil::rbegin(userDefinedArray);
        bdeu_ArrayUtil::rend(userDefinedArray);
        bdeu_ArrayUtil::size(userDefinedArray);
        bdeu_ArrayUtil::length(userDefinedArray);
        sizeof(bdeu_ArrayUtil::sizer(userDefinedArray));
        BDEU_ARRAYUTIL_SIZE(userDefinedArray);
        BDEU_ARRAYUTIL_LENGTH(userDefinedArray);

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
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
