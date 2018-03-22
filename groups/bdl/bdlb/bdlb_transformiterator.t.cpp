// bdlb_transformiterator.t.cpp                                       -*-C++-*-
#include <bdlb_transformiterator.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_cmath.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_numeric.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// ----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

class Parenthesizer {
    // Wrap strings in increasingly nested levels of parentheses.

  private:
    // PRIVATE DATA
    bsl::string d_before;
    bsl::string d_after;

  public:
    // PUBLIC TYPES
    #if __cplusplus < 201103L
    typedef bsl::string& result_type;
    #endif

    // PUBLIC CREATORS
    Parenthesizer(bslma::Allocator *basicAllocator);
        // Create an object of this type using the specified 'basicAllocator'
        // to supply memory.

    Parenthesizer(const Parenthesizer&  other,
                  bslma::Allocator     *basicAllocator);
        // Create a copy of the specified 'other' object using the specified
        // 'basicAllocator' to supply memory.

    // PUBLIC MANIPULATORS
    bsl::string& operator()(bsl::string& s);
        // Increment the parentheses nesting level, modify the specified string
        // 's' to be wrapped in those parentheses, and return a reference to
        // 's'.
};

Parenthesizer::Parenthesizer(bslma::Allocator *basicAllocator)
: d_before(basicAllocator)
, d_after(basicAllocator)
{
}

Parenthesizer::Parenthesizer(const Parenthesizer&  other,
                             bslma::Allocator     *basicAllocator)
: d_before(other.d_before, basicAllocator)
, d_after(other.d_after, basicAllocator)
{
}

bsl::string& Parenthesizer::operator()(bsl::string& s)
{
    return s = (d_before += "(") + s + (d_after += ")");
}

// USAGE EXAMPLE

// Next, we create a functor that will return a price given a product.  The
// following rather prolix functor at namespace scope is necessary for C++03:
//..
#if __cplusplus < 201103L
class Pricer {
  private:
    // PRIVATE DATA
    bsl::map<bsl::string, double> *d_prices;  // the price list;

  public:
    // PUBLIC CREATORS
    Pricer(bsl::map<bsl::string, double>& prices);
        // Create an object of this type using the specified 'prices'.

    // PUBLIC TYPES
    typedef double result_type;

    // PUBLIC ACCESSORS
    double operator()(const bsl::string& product) const;
        // Return the price of the specified 'product'.
};

// PUBLIC CREATORS
Pricer::Pricer(bsl::map<bsl::string, double>& prices)
: d_prices(&prices)
{
}

double Pricer::operator()(const bsl::string& product) const
{
    return (*d_prices)[product];
}
#endif
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test                = argc > 1 ? atoi(argv[1]) : 0;
    int verbose             = argc > 2; (void)verbose;
    int veryVerbose         = argc > 3; (void)veryVerbose;
    int veryVeryVerbose     = argc > 4; (void)veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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
        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Summing Absolute Values
/// - - - - - - - - - - - - - - - - -
// Suppose we have a sequence of numbers and we would like to sum their
// absolute values.  We can use 'bdlb::TransformIterator' for this purpose.
//
// First, we set up the numbers:
//..
    int data[5] = { 1, -1, 2, -2, 3 };
//..
// Then we create the transform iterators that will convert a number to its
// absolute value.  We need ones for the beginning and end of the sequence:
//..
    bdlb::TransformIterator<int(*)(int), int*> dataBegin(data + 0, bsl::abs);
    bdlb::TransformIterator<int(*)(int), int*> dataEnd  (data + 5, bsl::abs);
//..
// Now, we compute the sum of the absolute values of the numbers:
//..
    int sum = bsl::accumulate(dataBegin, dataEnd, 0);
//..
// Finally, we verify that we have computed the sum correctly:
//..
    ASSERT(9 == sum);
//..
//
///Example 2: Totalling a Grocery List
///- - - - - - - - - - - - - - - - - -
// Suppose we have a shopping list of products and we want to compute how much
// it will cost to buy the items.  We can use 'bdlb::TransformIterator' to do
// the computation, looking up the price of each item.
//
// First, we set up the price list:
//..
    bsl::map<bsl::string, double> prices;
    prices["pudding"] = 1.25;
    prices["apple"] = 0.33;
    prices["milk"] = 2.50;
//..
// Then, we set up our shopping list:
//..
   bsl::list<bsl::string> list;
   list.push_back("milk");
   list.push_back("milk");
   list.push_back("pudding");
//..
// Then we create the functor object.  In C++11 or later, the explicit functor
// class above is unnecessary since we can use a lambda:
//..
    #if __cplusplus < 201103L
    Pricer pricer(prices);
    #else
    auto pricer = [&](const bsl::string &product) { return prices[product]; };
    #endif
//..
// Next, we create a pair of transform iterators to process our grocery list:
//..
    typedef bdlb::TransformIterator<Pricer,
                                    bsl::list<bsl::string>::iterator> ti;
    ti groceryBegin(list.begin(), pricer);
    ti groceryEnd(list.end(), pricer);
//..
// Now, we add up the prices of our groceries:
//..
    double total = std::accumulate(groceryBegin, groceryEnd, 0.0);
//..
// Finally, we verify that we have the correct total:
//..
    ASSERT(6.25 == total);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is functional enough to enable comprehensive
        //:   testing in subsequent cases
        //
        // Plan:
        //: 1 Create an object, using the ctor with functional object.
        //:
        //: 2 Assign value to dereferenced iterator.
        //:
        //: 3 Check that functional object was invoked with correct value.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        double d[] = { 1, 4, 9, 16, 25 };
        typedef bdlb::TransformIterator<double(*)(double), double *> Ti;

        ASSERT(15 == bsl::accumulate(Ti(d + 0, sqrt), Ti(d + 5, sqrt), 0.0));

        typedef bdlb::TransformIterator<Parenthesizer, bsl::string *> Ts;
        bslma::TestAllocator allocator("bdlb", veryVeryVeryVerbose);
        bsl::string s[5] = { "1", "2", "3", "4", "5" };
        Parenthesizer parenthesizer(&allocator);
        ASSERT("(1)((2))(((3)))((((4))))(((((5)))))" ==
               bsl::accumulate(Ts(s + 0, parenthesizer, &allocator),
                               Ts(s + 5, parenthesizer, &allocator),
                               bsl::string(&allocator)));
      } break;
      default: {
         cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
         testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}
