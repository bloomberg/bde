// bdlb_nulloutputiterator.t.cpp                                      -*-C++-*-

#include <bdlb_nulloutputiterator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//===============================TEST PLAN=====================================
// bdlb::NullOutputIterator has no state and thus very little to test.  One
// test case is sufficient to test that every method can be instantiated.  The
// usage example completes the test by proving that it works in idiomatic use.
// ----------------------------------------------------------------------------
// [1] bdlb::NullOutputIterator();
// [1] bdlb::NullOutputIterator(const bdlb::NullOutputIterator& rhs);
// [1] ~bdlb::NullOutputIterator();
// [1] bdlb::NullOutputIterator& operator=(const bdlb::NullOutputIterator& rhs)
// [1] AssignmentProxy operator*();
// [1] bdlb::NullOutputIterator& operator++();
// [1] bdlb::NullOutputIterator& operator++(int);
// ----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class my_Class
{
    // Dummy class to test instantiation of 'bdlb::NullOutputIterator'.

  public:
    my_Class(int v = 0) : d_member(v) { }
    my_Class(const my_Class& rhs) : d_member(rhs.d_member) { }
    ~my_Class() { }
    my_Class& operator=(const my_Class& rhs)
        { d_member = rhs.d_member; return *this;}

  private:
    int d_member;
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following test function is designed to traverse an input iterator range
// and sum the elements.  A running total is written to the output iterator
// and the final sum is returned.
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
// The following program uses 'bdlb::NullOutputIterator' to compute the
// sum of a elements in an array while discarding the running total.
//..
    int usageExample1()
    {
        static const int myArray[5] = { 2, 3, 5, 7, 11 };
        int sum = runningSum(&myArray[0], &myArray[5],
                             bdlb::NullOutputIterator<int>());
        ASSERT(28 == sum);
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  The usage
        //   example exercises almost all of the class under test within the
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT' and replace
        //   'main' with 'usageExample1'.  Call 'usageExample1' within
        //   this test case.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageExample1();

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING ALL METHODS
        //   bdlb::NullOutputIterator has no state and thus very little to
        //   test.  This one test case is sufficient to test that every method
        //   can be instantiated.
        //
        // PLAN
        //   Instantiate 'bdlb::NullOutputIterator<int>' and
        //   'bdlb::NullOutputIterator<my_Class>'.  Invoke every method of
        //   each instantiation.  Instantiate bsl::iterator_traits for each
        //   instantiation.  A successful compile is sufficient to verify
        //   correctness.
        //
        // TESTING:
        //   bdlb::NullOutputIterator();
        //   bdlb::NullOutputIterator(const bdlb::NullOutputIterator& rhs);
        //   ~bdlb::NullOutputIterator();
        //   bdlb::NullOutputIterator& operator=(
        //                                const bdlb::NullOutputIterator& rhs);
        //   AssignmentProxy operator*();
        //   bdlb::NullOutputIterator& operator++();
        //   bdlb::NullOutputIterator& operator++(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting All Methods"
                          << "\n===================" << endl;

        const bdlb::NullOutputIterator<int> NI1;
        bdlb::NullOutputIterator<int> ni2(NI1);
        ni2 = NI1;
        const int I = 5;
        *ni2 = I;
        ++ni2;
        ni2++;
        bsl::iterator_traits<bdlb::NullOutputIterator<int> > TI1;
        ASSERT(sizeof TI1 >= 0);  // use TI1

        const bdlb::NullOutputIterator<my_Class> NC1;
        bdlb::NullOutputIterator<my_Class> nc2(NC1);
        nc2 = NC1;
        const my_Class C(7);
        *nc2 = C;
        ++nc2;
        nc2++;
        bsl::iterator_traits<bdlb::NullOutputIterator<my_Class> > TC1;
        ASSERT(sizeof TC1 >= 0);  // use TC1

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
