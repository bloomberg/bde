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
using namespace bdlb;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is implemented using contained constructor proxy
// objects for a generic functor type and a generic iterator type.  (Use of the
// proxy objects allows the functor and iterator to be initialized with an
// allocator even if the allocator is not used.)
//
// Because of the possibility that neither contained object uses allocators,
// this component uses a form of implementation inheritance that supplies an
// 'allocator()' method only when at least one of the contained objects does
// use an allocator.  Testing is required to demonstrate that the 'allocator()'
// method is or is not present as appropriate, and that it returns the
// allocator supplied to the constructor of the object.  Additionally, this
// componet sets its 'UsesBslmaAllocator' allocator trait to true if either of
// its contained objects uses allocators, and to false if neither does.
// Testing is required to demonstrate that this is done correctly.
//
// This component is an iterator, and tags itself as such by inheritance from
// 'bsl::iterator', supplying appropriate template arguments.  For iterator
// categories other than input iterator, dereferencing an iterator must provide
// a reference type.  Accordingly, this component sets its iterator category to
// input iterator if the functor does not return a reference type, otherwise
// passing through the iterator category of the underlying iterator.  Testing
// is required to demonstrate that the category and other iterator parameters
// are correctly determined.
//
// As per the above, this component must be able to determine the return type
// of the functor.  In C++11 and onward, language features allow this to be
// determined, but in C++03 the return type must be supplied as a 'result_type'
// member of the functor.  This component is specialized for pointers to
// functions, from which it can determine the result type, and otherwise uses
// 'bslmf::ResultType' in C++03.  Testing is required to demonstrate that this
// component picks up the correct result type in both C++03 and C++11.
//
// The remit of this component is to pass on iterator operations to the
// contained iterator, and to apply the functor to the dereferenced contained
// iterator when this component is itself dereferenced.  Testing is required to
// demonstrate that this occurs correctly, including for indexed dereference
// ('operator[](difference_type)') when the underlying iterator support it, and
// pointer dereference (operator->()) when the functor returns a reference.
//
// Functors used by this component can maintain state.  Testing is required to
// demonstrate that such stateful functors maintain their state throughout the
// iteration process.
//
// This component supports pairwise comparison operations on objects of this
// type, comparing only the contained iterators.  Testing is required to
// demonstrate that contained functors do not participate.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TransformIterator();
// [ 2] TransformIterator(Allocator *);
// [ 2] TransformIterator(const ITERATOR&, FUNCTOR, Allocator * = 0);
// [ 2] TransformIterator(const TransformIterator&, Allocator * = 0);
//
// MANIPULATORS
// [ 3] TransformIterator& operator=(const TransformIterator&);
// [ 3] TransformIterator& operator++();
// [ 3] TransformIterator& operator++(int);
// [ 3] TransformIterator& operator--();
// [ 3] TransformIterator& operator--(int);
// [ 3] TransformIterator& operator+=(difference_type);
// [ 3] TransformIterator& operator-=(difference_type);
// [ 3] Traits::reference operator*();
// [ 3] pointer operator->();
// [ 3] reference operator[](difference_type);
// [ 3] FUNCTOR& functor();
// [ 3] ITERATOR& iterator();
// [ 3] void swap(TransformIterator&);
//
// ACCESSORS
// [ 4] reference operator*() const;
// [ 4] pointer operator->() const;
// [ 4] reference operator[](difference_type n) const;
// [ 4] TransformIterator operator+(difference_type) const;
// [ 4] TransformIterator operator-(difference_type) const;
// [ 4] const FUNCTOR& functor() const;
// [ 4] const ITERATOR& iterator() const;
//
// FREE FUNCTIONS
// [ 5] bool operator==(const TI&, const TI&);
// [ 5] bool operator!=(const TI&, const TI&);
// [ 5] bool operator<(const TI&, const TI&);
// [ 5] bool operator<=(const TI&, const TI&);
// [ 5] bool operator>(const TI&, const TI&);
// [ 5] bool operator>=(const TI&, const TI&);
//
// ALLOCATOR TRAITS
// [ 6] bslma::UsesBslmaAllocator
// [ 6] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE

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

namespace {

                            // ===================
                            // class Parenthesizer
                            // ===================

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
    explicit Parenthesizer(bslma::Allocator *basicAllocator);
        // Create a Parenthesizer object using the specified 'basicAllocator'
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

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Parenthesizer, bslma::UsesBslmaAllocator);
};

                            // -------------------
                            // class Parenthesizer
                            // -------------------

// PUBLIC CREATORS
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

// PUBLIC MANIPULATORS
bsl::string& Parenthesizer::operator()(bsl::string& s)
{
    return s = (d_before += "(") + s + (d_after += ")");
}

                       // =============================
                       // class FunctorWithoutAllocator
                       // =============================

class FunctorWithoutAllocator {
    // A functor that does not use allocators.
  public:
    // PUBLIC TYPES
    typedef int result_type;

    // PUBLIC ACCESSORS
    int operator()(int n) const;
        // Return the specified 'n'.
};

                       // -----------------------------
                       // class FunctorWithoutAllocator
                       // -----------------------------

// PUBLIC ACCESSORS
int FunctorWithoutAllocator::operator()(int n) const
{
    return n;
}

                         // ==========================
                         // class FunctorWithAllocator
                         // ==========================

class FunctorWithAllocator {
    // A functor that does not use allocators.

  private:
    bslma::Allocator *d_allocator_p;  // the allocator for this object

  public:
    // PUBLIC TYPES
    typedef int result_type;

    // PUBLIC CREATORS
    FunctorWithAllocator();
    explicit FunctorWithAllocator(bslma::Allocator *basicAllocator);
        // Create an object of this type.  Optionally specify 'basicAllocator'
        // to set as the allocator of the new object.

    FunctorWithAllocator(const FunctorWithAllocator&  ,
                         bslma::Allocator            *basicAllocator = 0);
        // Create an object of this type.  Optionally specify 'basicAllocator'
        // to set as the allocator of the new object.

    // PUBLIC ACCESSORS
    int operator()(int n) const;
        // Return the specified 'n'.

    bslma::Allocator *allocator() const;
        // Return the allocator of this object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FunctorWithAllocator,
                                   bslma::UsesBslmaAllocator);
};

                       // --------------------------
                       // class FunctorWithAllocator
                       // --------------------------

// PUBLIC CREATORS
FunctorWithAllocator::FunctorWithAllocator()
: d_allocator_p(0)
{
}

FunctorWithAllocator::FunctorWithAllocator(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

FunctorWithAllocator::FunctorWithAllocator(
                                   const FunctorWithAllocator&  ,
                                   bslma::Allocator            *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

// PUBLIC ACCESSORS
int FunctorWithAllocator::operator()(int n) const
{
    return n;
}

bslma::Allocator *FunctorWithAllocator::allocator() const
{
    return d_allocator_p;
}

                       // ==============================
                       // class IteratorWithoutAllocator
                       // ==============================

class IteratorWithoutAllocator : public bsl::reverse_iterator<int *> {
    // An iterator that does not use allocators.

  public:
    IteratorWithoutAllocator();
        // Create an object of this type.

    explicit IteratorWithoutAllocator(int *iterator);
        // Create an object of this type using the specified 'iterator'.
};

                       // ------------------------------
                       // class IteratorWithoutAllocator
                       // ------------------------------

IteratorWithoutAllocator::IteratorWithoutAllocator()
{
}

IteratorWithoutAllocator::IteratorWithoutAllocator(int *iterator)
: bsl::reverse_iterator<int *>(iterator)
{
}

                        // ===========================
                        // class IteratorWithAllocator
                        // ===========================

class IteratorWithAllocator : public bsl::reverse_iterator<int *> {
    // An iterator that uses allocators.

  private:
    bslma::Allocator *d_allocator_p;  // the allocator for this object

  public:
    // PUBLIC CREATORS
    IteratorWithAllocator();
    explicit IteratorWithAllocator(bslma::Allocator *basicAllocator);
        // Create an object of this type.  Optionally specify 'basicAllocator'
        // to set as the allocator of the new object.

    explicit IteratorWithAllocator(int *iterator);
    IteratorWithAllocator(int *iterator, bslma::Allocator *basicAllocator);
        // Create an object of this type using the specified 'iterator'.
        // Optionally specify 'basicAllocator' to set as the allocator of the
        // new object.

    IteratorWithAllocator(const IteratorWithAllocator&  other,
                          bslma::Allocator             *basicAllocator = 0);
        // Create a copy of the specified 'other' object. Optionally specify
        // 'basicAllocator' to set as the allocator of the new object.

    // PUBLIC ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator of this object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(IteratorWithAllocator,
                                   bslma::UsesBslmaAllocator);
};

                        // ---------------------------
                        // class IteratorWithAllocator
                        // ---------------------------

// PUBLIC CREATORS
IteratorWithAllocator::IteratorWithAllocator()
: d_allocator_p(0)
{
}

IteratorWithAllocator::IteratorWithAllocator(bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

IteratorWithAllocator::IteratorWithAllocator(int *iterator)
: bsl::reverse_iterator<int *>(iterator)
, d_allocator_p(0)
{
}

IteratorWithAllocator::IteratorWithAllocator(int              *iterator,
                                             bslma::Allocator *basicAllocator)
: bsl::reverse_iterator<int *>(iterator)
, d_allocator_p(basicAllocator)
{
}

IteratorWithAllocator::IteratorWithAllocator(
                                  const IteratorWithAllocator&  other,
                                  bslma::Allocator             *basicAllocator)
: bsl::reverse_iterator<int *>(other)
, d_allocator_p(basicAllocator)
{
}

// PUBLIC ACCESSORS
bslma::Allocator *IteratorWithAllocator::allocator() const
{
    return d_allocator_p;
}

                                   // =============
                                   // USAGE EXAMPLE
                                   // =============

// Next, we create a functor that will return a price given a product.  The
// following prolix functor at namespace scope is necessary for C++03:
//..
#if __cplusplus < 201103L
class Pricer {
  private:
    // PRIVATE DATA
    bsl::map<bsl::string, double> *d_prices_p;  // the price list;

  public:
    // PUBLIC CREATORS
    explicit Pricer(bsl::map<bsl::string, double>& prices);
        // Create an object of this type using the specified 'prices'.

    // PUBLIC TYPES
    typedef double result_type;

    // PUBLIC ACCESSORS
    double operator()(const bsl::string& product) const;
        // Return the price of the specified 'product'.
};

// PUBLIC CREATORS
Pricer::Pricer(bsl::map<bsl::string, double>& prices)
: d_prices_p(&prices)
{
}

double Pricer::operator()(const bsl::string& product) const
{
    return (*d_prices_p)[product];
}
#endif
//..

}  // close unnamed namespace

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
      case 7: {
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
    typedef bdlb::TransformIterator<bsl::function<double(const bsl::string&)>,
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
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TRAITS AND 'allocator()'
        //
        // Concerns:
        //: 1 The transform iterator should have an affirmative allocator trait
        //:   if either its functor or underlying iterator uses allocators, and
        //:   a negative one if neither do.
        //
        //: 2 The transform iterator should have an 'allocator()' method if
        //:   if either its functor or underlying iterator uses allocators, and
        //:   not have such a method if neither use allocators.
        //:
        //: 3 If the transform iterator has an 'allocator()' method, it should
        //:   return the allocator with which the object was initialized, if
        //:   one was provided.
        //:
        //: 4 The allocator of the subobjects should be the same as the
        //:   allocator with which the object was initialized, if one was
        //:   provided.
        //
        // Plan:
        //: 1 Create all four possible versions of the transform iterator with
        //:   its members using or not using allocators, and verify that the
        //:   trait is correct.  (C-1)
        //
        // Testing:
        //   bslma::UsesBslmaAllocator
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING TRAITS AND 'allocator()'"
                             "\n================================\n";

        if (veryVerbose) {
            cout << "functor with allocator, iterator with allocator\n";
        }
        {
            typedef bdlb::TransformIterator<FunctorWithAllocator,
                                            IteratorWithAllocator>
                Obj;

            ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

            bslma::TestAllocator ta(veryVeryVeryVerbose);

            Obj        mX(&ta);
            const Obj& X = mX;

            ASSERT(&ta == X.allocator());
            ASSERT(&ta == X.functor().allocator());
            ASSERT(&ta == X.iterator().allocator());
        }

        if (veryVerbose) {
            cout << "functor without allocator, iterator with allocator\n";
        }
        {
            typedef bdlb::TransformIterator<FunctorWithoutAllocator,
                                            IteratorWithAllocator>
                Obj;

            ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

            bslma::TestAllocator ta(veryVeryVeryVerbose);

            Obj        mX(&ta);
            const Obj& X = mX;

            ASSERT(&ta == X.allocator());
            ASSERT(&ta == X.iterator().allocator());
        }

        if (veryVerbose) {
            cout << "functor with allocator, iterator without allocator\n";
        }
        {
            typedef bdlb::TransformIterator<FunctorWithAllocator,
                                            IteratorWithoutAllocator>
                Obj;

            ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

            bslma::TestAllocator ta(veryVeryVeryVerbose);

            Obj        mX(&ta);
            const Obj& X = mX;

            ASSERT(&ta == X.allocator());
            ASSERT(&ta == X.functor().allocator());
        }

        if (veryVerbose) {
            cout << "functor without allocator, iterator without allocator\n";
        }
        {
            typedef bdlb::TransformIterator<FunctorWithoutAllocator,
                                            IteratorWithoutAllocator>
                Obj;

            ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);

            struct FakeAllocatorMethod {
                enum { allocator = -1 };
            };

            struct DerivedTransformIterator : FakeAllocatorMethod, Obj {
            };

            ASSERT(-1 == DerivedTransformIterator::allocator);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FREE FUNCTIONS
        //
        // Concerns:
        //: 1 'TransformIterator' objects can be compared when the underlying
        //:   iterators can be compared.
        //:
        //: 2 Such comparisons disregard the functors of the objects.
        //
        // Plan:
        //: 1 Create 'TransformIterator' objects holding reverse iterators into
        //:   an array of integers and verify that comparisons are correct.
        //:   (C-1)
        //:
        //: 2 When comparing these objects, use different functors and verify
        //:   that comparisons are not affected.
        //:   (C-2)
        //
        // Testing:
        //   bool operator==(const TI&, const TI&);
        //   bool operator!=(const TI&, const TI&);
        //   bool operator<(const TI&, const TI&);
        //   bool operator<=(const TI&, const TI&);
        //   bool operator>(const TI&, const TI&);
        //   bool operator>=(const TI&, const TI&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING FREE FUNCTIONS"
                             "\n======================\n";

        double a[1] = {.785};

        typedef bsl::reverse_iterator<double *> ri;
        typedef TransformIterator<double (*)(double), ri> Obj;

        Obj iterators[4] = {
            Obj(ri(a + 0), bsl::sin),
            Obj(ri(a + 0), bsl::cos),
            Obj(ri(a + 1), bsl::sin),
            Obj(ri(a + 1), bsl::cos),
        };

        bool expected_eq[4][4] = {
            true,  true,  false, false,
            true,  true,  false, false,
            false, false, true,  true,
            false, false, true,  true,
        };

        bool expected_lt[4][4] = {
            false, false, false, false,
            false, false, false, false,
            true,  true,  false, false,
            true,  true,  false, false,
        };

        if (veryVerbose) {
            cout << "\toperator==\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = expected_eq[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] == iterators[j]));
            }
        }

        if (veryVerbose) {
            cout << "\toperator!=\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = !expected_eq[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] != iterators[j]));
            }
        }

        if (veryVerbose) {
            cout << "\toperator<\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = expected_lt[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] < iterators[j]));
            }
        }

        if (veryVerbose) {
            cout << "\toperator<=\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = expected_lt[i][j] || expected_eq[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] <= iterators[j]));
            }
        }

        if (veryVerbose) {
            cout << "\toperator>\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = !expected_lt[i][j] && !expected_eq[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] > iterators[j]));
            }
        }

        if (veryVerbose) {
            cout << "\toperator>=\n";
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                bool expected = !expected_lt[i][j];
                if (veryVeryVerbose) {
                    P_(i) P_(j) P(expected)
                }
                ASSERT(expected == (iterators[i] >= iterators[j]));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS
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
        //   reference operator*() const;
        //   pointer operator->() const;
        //   reference operator[](difference_type n) const;
        //   TransformIterator operator+(difference_type) const;
        //   TransformIterator operator-(difference_type) const;
        //   const FUNCTOR& functor() const;
        //   const ITERATOR& iterator() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING ACCESSORS"
                             "\n=================\n";
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
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
        //   TransformIterator& operator=(const TransformIterator&);
        //   TransformIterator& operator++();
        //   TransformIterator& operator++(int);
        //   TransformIterator& operator--();
        //   TransformIterator& operator--(int);
        //   TransformIterator& operator+=(difference_type);
        //   TransformIterator& operator-=(difference_type);
        //   Traits::reference operator*();
        //   pointer operator->();
        //   reference operator[](difference_type);
        //   FUNCTOR& functor();
        //   ITERATOR& iterator();
        //   void swap(TransformIterator&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING MANIPULATORS"
                             "\n====================\n";
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
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
        //   TransformIterator();
        //   TransformIterator(Allocator *);
        //   TransformIterator(const ITERATOR&, FUNCTOR, Allocator * = 0);
        //   TransformIterator(const TransformIterator&, Allocator * = 0);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING CREATORS"
                             "\n================\n";
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 Basic functionality of this component works as expected.
        //
        // Plan:
        //: 1 Exercise some instances and verify their operation.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        if (veryVerbose) {
            cout << "Simple Transformation\n";
        }
        {
            int    DATA[]   = {-1, 1, -2, 2, 3};
            size_t NUM_DATA = sizeof DATA / sizeof *DATA;
            if (veryVerbose) {
                cout << "\tusing function pointer\n";
            }
            {
                typedef TransformIterator<int (*)(int), int *> Obj;

                Obj begin(DATA + 0, bsl::abs);
                Obj end(DATA + NUM_DATA, bsl::abs);

                ASSERT(9 == std::accumulate(begin, end, 0));
            }
            if (veryVerbose) {
                cout << "\tusing bsl::function\n";
            }
            {
                typedef TransformIterator<bsl::function<int(int)>, int *> Obj;

                int (*abs)(int) = &bsl::abs;  // Pick correct overload of abs.

                Obj begin(DATA + 0, abs);
                Obj end(DATA + NUM_DATA, abs);

                ASSERT(9 == std::accumulate(begin, end, 0));
            }
        }

        if (veryVerbose) {
            cout << "Stateful Functor\n";
        }
        {
            bslma::TestAllocator ta(veryVeryVeryVerbose);
            if (veryVerbose) {
                cout << "\tusing function object and allocators\n";
            }
            {
                typedef TransformIterator<Parenthesizer, bsl::string *> Obj;

                bsl::string DATA[]   = {"1", "2", "3"};
                size_t      NUM_DATA = sizeof DATA / sizeof *DATA;

                Parenthesizer parenthesizer(&ta);

                Obj begin(DATA + 0, parenthesizer, &ta);
                Obj end(DATA + NUM_DATA, parenthesizer, &ta);

                ASSERT("(1)((2))(((3)))" ==
                       bsl::accumulate(begin, end, bsl::string()));
                ASSERT("(1)" == DATA[0]);
                ASSERT("((2))" == DATA[1]);
                ASSERT("(((3)))" == DATA[2]);
            }
            if (veryVerbose) {
                cout << "\tusing bsl::function and allocators\n";
            }
            {
                typedef TransformIterator<
                    bsl::function<bsl::string&(bsl::string&)>,
                    bsl::string *>
                    Obj;

                bsl::string DATA[]   = {"1", "2", "3"};
                size_t      NUM_DATA = sizeof DATA / sizeof *DATA;

                Parenthesizer parenthesizer(&ta);

                Obj begin(DATA + 0, parenthesizer, &ta);
                Obj end(DATA + NUM_DATA, parenthesizer, &ta);

                ASSERT("(1)((2))(((3)))" ==
                       bsl::accumulate(begin, end, bsl::string()));
                ASSERT("(1)" == DATA[0]);
                ASSERT("((2))" == DATA[1]);
                ASSERT("(((3)))" == DATA[2]);
            }
        }
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
