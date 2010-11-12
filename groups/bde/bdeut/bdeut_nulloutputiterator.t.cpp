// bdeut_nulloutputiterator.t.cpp       -*-C++-*-

#include <bdeut_nulloutputiterator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//===============================TEST PLAN=====================================
// bdeut_NullOutputIterator has no state and thus very little to test.  One
// test case is sufficient to test that every method can be instantiated.  The
// usage example completes the test by proving that it works in idiomatic use.
// ----------------------------------------------------------------------------
// [1] bdeut_NullOutputIterator();
// [1] bdeut_NullOutputIterator(const bdeut_NullOutputIterator& rhs);
// [1] ~bdeut_NullOutputIterator();
// [1] bdeut_NullOutputIterator& operator=(const bdeut_NullOutputIterator& rhs)
// [1] AssignmentProxy operator*();
// [1] bdeut_NullOutputIterator& operator++();
// [1] bdeut_NullOutputIterator& operator++(int);
// ----------------------------------------------------------------------------
// [2] USAGE EXAMPLE

//==========================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
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
    // Dummy class to test instantiation of 'bdeut_NullOutputIterator'.

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
//                  USAGE EXAMPLE
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
// The following program uses 'bdeut_NullOutputIterator' to compute the
// sum of a elements in an array while discarding the running total.
//..
    int usageExample1()
    {
        static const int myArray[5] = { 2, 3, 5, 7, 11 };
        int sum = runningSum(&myArray[0], &myArray[5],
                             bdeut_NullOutputIterator<int>());
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
        //   bdeut_NullOutputIterator has no state and thus very little to
        //   test.  This one test case is sufficient to test that every method
        //   can be instantiated.
        //
        // PLAN
        //   Instantiate 'bdeut_NullOutputIterator<int>' and
        //   'bdeut_NullOutputIterator<my_Class>'.  Invoke every method of
        //   each instantiation.  Instantiate bsl::iterator_traits for each
        //   instantiation.  A successful compile is sufficient to verify
        //   correctness.
        //
        // TESTING:
        //   bdeut_NullOutputIterator();
        //   bdeut_NullOutputIterator(const bdeut_NullOutputIterator& rhs);
        //   ~bdeut_NullOutputIterator();
        //   bdeut_NullOutputIterator& operator=(
        //                                const bdeut_NullOutputIterator& rhs);
        //   AssignmentProxy operator*();
        //   bdeut_NullOutputIterator& operator++();
        //   bdeut_NullOutputIterator& operator++(int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting All Methods"
                          << "\n===================" << endl;

        const bdeut_NullOutputIterator<int> NI1;
        bdeut_NullOutputIterator<int> ni2(NI1);
        ni2 = NI1;
        const int I = 5;
        *ni2 = I;
        ++ni2;
        ni2++;
        bsl::iterator_traits<bdeut_NullOutputIterator<int> > TI1;
        ASSERT(sizeof TI1 >= 0);  // use TI1

        const bdeut_NullOutputIterator<my_Class> NC1;
        bdeut_NullOutputIterator<my_Class> nc2(NC1);
        nc2 = NC1;
        const my_Class C(7);
        *nc2 = C;
        ++nc2;
        nc2++;
        bsl::iterator_traits<bdeut_NullOutputIterator<my_Class> > TC1;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
