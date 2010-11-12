// bdeut_nullinputiterator.t.cpp       -*-C++-*-

#include <bdeut_nullinputiterator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//===============================TEST PLAN=====================================
// bdeut_NullInputIterator has no state and thus very little to test.  One
// test case is sufficient to test that every method can be instantiated.  The
// usage example completes the test by proving that it works in idiomatic use.
// ----------------------------------------------------------------------------
// [1] bdeut_NullInputIterator();
// [1] bdeut_NullInputIterator(const bdeut_NullInputIterator& rhs);
// [1] ~bdeut_NullInputIterator();
// [1] bdeut_NullInputIterator& operator=(const bdeut_NullInputIterator& rhs)
// [1] TYPE operator*();
// [1] TYPE* operator->();
// [1] bdeut_NullInputIterator& operator++();
// [1] bdeut_NullInputIterator& operator++(int);
// [1] bool operator==(bdeut_NullInputIterator, bdeut_NullInputIterator);
// [1] bool operator!=(bdeut_NullInputIterator, bdeut_NullInputIterator);
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
//                  SEMI-STANDARD TEST INPUT MACROS
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

class my_Class {
    // Dummy class to test instantiation of 'bdeut_NullInputIterator'.

  public:
    my_Class(int v = 0) : d_value(v) { }
    my_Class(const my_Class& rhs) : d_value(rhs.d_value) { }
    ~my_Class() { }

    int value() const { return d_value; }

  private:
    // The type used to instantiate 'bdeut_NullInputIterator' does not need to
    // be assignable, although it must be copy-constructible.  We disable the
    // assignment operator to ensure that we don't depend on it.
    my_Class& operator=(const my_Class& rhs);
    int d_value;
};

//=============================================================================
//                  USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following test function is designed to traverse an input iterator range
// and sum the elements.
//..
    template <class IN_ITER>
#if defined(BSLS_PLATFORM__CMP_SUN) && (__SUNPRO_CC < 0x550)
    int
#else
    typename bsl::iterator_traits<IN_ITER>::value_type
#endif
    sum(IN_ITER first, IN_ITER last)
    {
        typename bsl::iterator_traits<IN_ITER>::value_type total = 0;
        while (first != last) {
            total += *first++;
        }
        return total;
    }
//..
// The following program uses 'sum' to compute the sum of elements in an
// array.  Then it uses 'sum' again, this time instantiated with
// 'bdeut_NullInputIterator'.  The result is zero because
// 'bdeut_NullInputIterator' is always at the end.  The point is to prove that
// 'sum' compiles when instantiated with pure input iterators.
//..
    int usageExample1()
    {
        static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };

        // Compute the sum using random-access iterators (pointers).
        int r1 = sum(&myArray[0], &myArray[5]);
        ASSERT(28 == r1);

        // Now test that it compiles using pure input iterators:
        typedef bdeut_NullInputIterator<unsigned> iterType;
        unsigned r2 = sum(iterType(), iterType());
        ASSERT(0 == r2);

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
        //   bdeut_NullInputIterator has no state and thus very little to
        //   test.  This one test case is sufficient to test that every method
        //   can be instantiated.
        //
        // PLAN
        //   Instantiate 'bdeut_NullInputIterator<int>' and
        //   'bdeut_NullInputIterator<my_Class>'.  Invoke every method of
        //   each instantiation.  Instantiate bsl::iterator_traits for each
        //   instantiation.  A successful compile is sufficient to verify
        //   correctness.
        //
        // TESTING:
        //   bdeut_NullInputIterator();
        //   bdeut_NullInputIterator(const bdeut_NullInputIterator& rhs);
        //   ~bdeut_NullInputIterator();
        //   bdeut_NullInputIterator& operator=(
        //                                const bdeut_NullInputIterator& rhs);
        //   AssignmentProxy operator*();
        //   bdeut_NullInputIterator& operator++();
        //   bdeut_NullInputIterator& operator++(int);
        //   bool operator==(bdeut_NullInputIterator, bdeut_NullInputIterator);
        //   bool operator!=(bdeut_NullInputIterator, bdeut_NullInputIterator);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting All Methods"
                          << "\n===================" << endl;

        const bdeut_NullInputIterator<int> NI1;
        bdeut_NullInputIterator<int> ni2(NI1);
        ni2 = NI1;
        ASSERT(NI1 == ni2);
        ASSERT(! (NI1 != ni2));
        ASSERT(NI1 == bdeut_NullInputIterator<int>())
        bsl::iterator_traits<bdeut_NullInputIterator<int> > TI1;
        ASSERT(sizeof TI1 >= 0);  // use TI1
        if (0) {
            // The following must compile, but must never be called at run-time
            int i = *ni2;  ASSERT(i);
            ++ni2;
            ni2++;
        }

        const bdeut_NullInputIterator<my_Class> NC1;
        bdeut_NullInputIterator<my_Class> nc2(NC1);
        nc2 = NC1;
        ASSERT(NC1 == nc2);
        ASSERT(! (NC1 != nc2));
        ASSERT(NC1 == bdeut_NullInputIterator<my_Class>())
        bsl::iterator_traits<bdeut_NullInputIterator<my_Class> > TC1;
        ASSERT(sizeof TC1 >= 0);  // use TC1
        if (0) {
            // The following must compile, but must never be called at run-time
            my_Class c = *nc2;
            int v = nc2->value();  ASSERT(v);
            ++nc2;
            nc2++;
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
