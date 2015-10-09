// bdlb_nulloutputiterator.t.cpp                                      -*-C++-*-
#include <bdlb_nulloutputiterator.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

#include <bslmf_issame.h>       // for testing only

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// bdlb::NullOutputIterator has no state and thus very little to test.  One
// test case is sufficient to test that every method can be instantiated.  The
// usage example completes the test by proving that it works in idiomatic use.
// ----------------------------------------------------------------------------
// [ 2] NullOutputIterator();
// [ 2] NullOutputIterator(const NullOutputIterator& rhs);
// [ 2] ~NullOutputIterator();
// [ 2] NullOutputIterator& operator=(const NullOutputIterator& rhs);
// [ 2] AssignmentProxy operator*();
// [ 2] NullOutputIterator& operator++();
// [ 2] NullOutputIterator& operator++(int);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class MyClass {
    // A simple test class that can instantiated and copied to help test the
    // instantiation of 'bdlb::NullOutputIterator' on a user-defined type.

  public:
    // CREATORS
    MyClass(int v = 0) : d_member(v) { }
    MyClass(const MyClass& rhs) : d_member(rhs.d_member) { }
    ~MyClass() { }
    MyClass& operator=(const MyClass& rhs) {
        d_member = rhs.d_member;
        return *this;
    }

  private:
    int d_member;
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
//
// In the following example we use a 'bdlb::NullOutputIterator' to enable us to
// call a function to capture its return code, while ignoring the output
// provided through an iterator.
//
// First, we define a function 'runningSum' that returns output both through an
// output iterator and through a return status code:
//..
    template <class IN_ITER, class OUT_ITER>
    typename bsl::iterator_traits<OUT_ITER>::value_type
    runningSum(IN_ITER first, IN_ITER last, OUT_ITER output)
    {
        typename bsl::iterator_traits<OUT_ITER>::value_type total = 0;
        while (first != last) {
            total += *first++;
            *output++ = total;
        }
        return total;
    }
//..
// Now, we define a function 'average' that captures the total sum returned by
// 'runningSum' and uses a 'bdlb::NullOutputIterator' to facilitate calling the
// function, and ignoring the output it provides through its output iterator
// parameter:
//..
    int average(const int values[], int numValues)
    {
        return runningSum(values, values + numValues,
                                  bdlb::NullOutputIterator<int>()) / numValues;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;


        {
            const int myArray[5] = { 3, 4, 5, 7, 11 };

            int averageValue = average(myArray, 5);
            ASSERT( averageValue == 6 );
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   This case exercises all methods.
        //
        // Concerns:
        //: 1 The default constructor can create an object.
        //:
        //: 2 The copy constructor can create a copy of the object.
        //:
        //: 4 Assignment assigns the value of the original object.
        //:
        //: 4 The template can be instantiated for both basic types and
        //:   user-defined classes.
        //:
        //: 5 The template correctly defines the iterator traits.
        //
        // Plan:
        //: 1 Manually call different methods of the object. (C-1..5)
        //
        // Testing:
        //   NullOutputIterator();
        //   NullOutputIterator(const NullOutputIterator& rhs);
        //   ~NullOutputIterator();
        //   NullOutputIterator& operator=(const NullOutputIterator& rhs);
        //   AssignmentProxy operator*();
        //   NullOutputIterator& operator++();
        //   NullOutputIterator& operator++(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC TEST" << endl
                          << "==========" << endl;

        if (veryVerbose) cout << "\tTesting basic type" << endl;
        {
            typedef bdlb::NullOutputIterator<int> Obj;

            const Obj NO1;
            Obj       nO1(NO1);
            nO1 = NO1;
            const int I = 5;
            *nO1 = I;
            ++nO1;
            nO1++;

            typedef bsl::iterator_traits<Obj> ObjTraits;
            BSLMF_ASSERT((bsl::is_same<ObjTraits::iterator_category,
                                       bsl::output_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<ObjTraits::value_type, int>::value));
        }

        if (veryVerbose) cout << "\tTesting user-defined type" << endl;
        {
            typedef bdlb::NullOutputIterator<MyClass> Obj;

            const Obj NO1;
            Obj       nO1(NO1);
            nO1 = NO1;
            const MyClass C(7);
            *nO1 = C;
            ++nO1;
            nO1++;

            typedef bsl::iterator_traits<Obj> ObjTraits;
            BSLMF_ASSERT((bsl::is_same<ObjTraits::iterator_category,
                                       bsl::output_iterator_tag>::value));
            BSLMF_ASSERT((bsl::is_same<ObjTraits::value_type,
                                       MyClass>::value));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This component does not use breathing test case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
      }  break;

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
