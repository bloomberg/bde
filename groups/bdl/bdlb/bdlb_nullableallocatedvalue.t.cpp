// bdlb_nullableallocatedvalue.t.cpp                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullableallocatedvalue.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_review.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bsl_algorithm.h>  // 'swap'
#include <bsl_climits.h>
#include <bsl_cstdlib.h>    // 'atoi', 'abs'
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// TBD
//
// Primary Manipulators and Basic Accessors
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Primary Manipulators:
//
// Basic Accessors:
//
//-----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
// [ 05] bool operator==(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator==(const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator==(const b_NV<TYPE>&, const TYPE&);
// [ 05] bool operator!=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator!=(const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator!=(const b_NV<TYPE>&, const TYPE&);
// [ 05] bool operator<( const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator<( const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator<( const b_NV<TYPE>&, const TYPE&);
// [ 05] bool operator<=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator<=(const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator<=(const b_NV<TYPE>&, const TYPE&);
// [ 05] bool operator>( const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator>( const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator>( const b_NV<TYPE>&, const TYPE&);
// [ 05] bool operator>=(const b_NV<TYPE>&, const b_NV<TYPE>&);
// [ 05] bool operator>=(const TYPE&,       const b_NV<TYPE>&);
// [ 05] bool operator>=(const b_NV<TYPE>&, const TYPE&);
//
// TRAITS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

typedef bdlb::NullableAllocatedValue<int> NullableInt;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableInt>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableInt>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableInt>::value);

typedef bdlb::NullableAllocatedValue<bsl::string> NullableString;

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<NullableString>::value);
BSLMF_ASSERT(true == bslmf::IsBitwiseMoveable<NullableString>::value);
BSLMF_ASSERT(true == bdlb::HasPrintMethod<NullableString>::value);

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate use of this component.
//
// Suppose we want to create a linked list of nodes that contain integers:
//..
    struct LinkedListNode {
        int                                          d_value;
        bdlb::NullableAllocatedValue<LinkedListNode> d_next;
    };
//..
// Note that 'bdlb::NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdlb::NullableValue' requires that the template parameter 'TYPE' be
// a complete type when the class is instantiated.
//
// We can now traverse a linked list and add a new value at the end using the
// following code:
//..
    void addValueAtEnd(LinkedListNode *node, int value)
    {
        while (!node->d_next.isNull()) {
            node = &node->d_next.value();
        }

        node->d_next.makeValue();
        node = &node->d_next.value();
        node->d_value = value;
    }
//..

//=============================================================================
//              GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class Swappable {
    // 'Swappable', used for testing 'swap', does not take an allocator.

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'Swappable' objects created
                                      // since last reset

    // DATA
    int d_value;

    // FRIENDS
    friend void swap(Swappable&, Swappable&);

  private:
    // NOT IMPLEMENTED
    void swap(Swappable&);

  public:
    // CLASS METHODS
    static int numObjectsCreated()
    {
        return s_numObjectsCreated;
    }

    static void reset()
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit Swappable(int v)
    : d_value(v)
    {
        ++Swappable::s_numObjectsCreated;
    }

    Swappable(const Swappable& original)
    : d_value(original.d_value)
    {
        ++Swappable::s_numObjectsCreated;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // MANIPULATORS
    Swappable& operator=(const Swappable& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif

    // ACCESSORS
    int value() const
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.value() == rhs.value();
}

bool operator!=(const Swappable& lhs, const Swappable& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool Swappable::s_swapCalledFlag    = false;
int  Swappable::s_numObjectsCreated = 0;

void swap(Swappable& a, Swappable& b)
{
    Swappable::s_swapCalledFlag = true;

    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

class SwappableWithAllocator {
    // 'SwappableWithAllocator', used for testing 'swap', takes an allocator.

    // CLASS DATA
    static bool s_swapCalledFlag;     // 'true' if 'swap' free function called
                                      // since last reset; 'false' otherwise

    static int  s_numObjectsCreated;  // number of 'SwappableWithAllocator'
                                      // objects created since last reset

    // DATA
    int               d_value;
    bsl::string       d_string;
    bslma::Allocator *d_allocator_p;  // held, not owned

    // FRIENDS
    friend void swap(SwappableWithAllocator&, SwappableWithAllocator&);

  private:
    // NOT IMPLEMENTED
    void swap(SwappableWithAllocator&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableWithAllocator,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int numObjectsCreated()
    {
        return s_numObjectsCreated;
    }

    static void reset()
    {
        s_swapCalledFlag    = false;
        s_numObjectsCreated = 0;
    }

    static bool swapCalled()
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit
    SwappableWithAllocator(int v, bslma::Allocator *basicAllocator = 0)
    : d_value(v)
    , d_string(bsl::abs(v), 'x', basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        ++SwappableWithAllocator::s_numObjectsCreated;
    }

    SwappableWithAllocator(const SwappableWithAllocator&  original,
                           bslma::Allocator              *basicAllocator = 0)
    : d_value(original.d_value)
    , d_string(original.d_string, basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        ++SwappableWithAllocator::s_numObjectsCreated;
    }

    ~SwappableWithAllocator()
    {
        BSLS_ASSERT_OPT(allocator() == d_string.get_allocator().mechanism());
    }

    // MANIPULATORS
    SwappableWithAllocator& operator=(const SwappableWithAllocator& rhs)
    {
        d_value  = rhs.d_value;
        d_string = rhs.d_string;

        return *this;
    }

    // ACCESSORS
    bslma::Allocator *allocator() const
    {
        return d_allocator_p;
    }

    const bsl::string& string() const
    {
        return d_string;
    }

    int value() const
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return lhs.value() == rhs.value() && lhs.string() == rhs.string();
}

bool operator!=(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool SwappableWithAllocator::s_swapCalledFlag    = false;
int  SwappableWithAllocator::s_numObjectsCreated = 0;

void swap(SwappableWithAllocator& a, SwappableWithAllocator& b)
{
    SwappableWithAllocator::s_swapCalledFlag = true;

    if (a.allocator() == b.allocator()) {
        bslalg::SwapUtil::swap(&a.d_value, &b.d_value);

        a.d_string.swap(b.d_string);

        return;                                                       // RETURN
    }

    SwappableWithAllocator futureA(b, a.allocator());
    SwappableWithAllocator futureB(a, b.allocator());

    bslalg::SwapUtil::swap(&a.d_value, &futureA.d_value);
    bslalg::SwapUtil::swap(&b.d_value, &futureB.d_value);

    a.d_string.swap(futureA.d_string);
    b.d_string.swap(futureB.d_string);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    bslma::TestAllocator *ALLOC = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
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

        LinkedListNode node;
        node.d_value = 3;
        ASSERT(node.d_next.isNull());

        addValueAtEnd(&node, 5);
        ASSERT(!node.d_next.isNull());
        ASSERT(node.d_next.value().d_next.isNull());
        ASSERT(3 == node.d_value);
        ASSERT(5 == node.d_next.value().d_value);

        addValueAtEnd(&node, 53);
        ASSERT(!node.d_next.isNull());
        ASSERT(!node.d_next.value().d_next.isNull());
        ASSERT(node.d_next.value().d_next.value().d_next.isNull());
        ASSERT( 3 == node.d_value);
        ASSERT( 5 == node.d_next.value().d_value);
        ASSERT(53 == node.d_next.value().d_next.value().d_value);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //: 1 Swap of two null objects leaves both objects null.
        //:
        //: 2 Swap of a null object and a non-null object, or of two non-null
        //:   objects, swaps the underlying pointers to the out-of-place
        //:   objects (i.e., no temporary objects are incurred to effect the
        //:   swap).
        //
        // Plan:
        //: 1 Create a class, 'Swappable', with a 'swap' free function
        //:   instrumented to track swap calls and creators instrumented to
        //:   track the number of 'Swappable' objects created.  Instantiate
        //:   'bdlb::NullableAllocatedValue' with 'Swappable' and execute
        //:   operations needed to verify the concerns.  (C-1..2)
        //
        // Testing:
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& other);
        //   void swap(bdlb::NullableAllocatedValue<TYPE>& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout << "Testing w/type not taking an allocator." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            if (veryVerbose) cout << "\tSanity test 'Swappable' type." << endl;
            {
                ASSERT(0 == Swappable::numObjectsCreated());
                ASSERT(!Swappable::swapCalled());

                Swappable obj1(1);
                Swappable obj2(2);
                ASSERT(2 == Swappable::numObjectsCreated());

                const Swappable Zobj1(obj1);
                const Swappable Zobj2(obj2);
                ASSERT(4 == Swappable::numObjectsCreated());

                ASSERT(obj1 == Zobj1);
                ASSERT(obj2 == Zobj2);

                ASSERT(!Swappable::swapCalled());
                swap(obj1, obj2);
                ASSERT( Swappable::swapCalled());
                ASSERT(4 == Swappable::numObjectsCreated());

                ASSERT(obj2 == Zobj1);
                ASSERT(obj1 == Zobj2);

                Swappable::reset();
                ASSERT(!Swappable::swapCalled());
                ASSERT(0 == Swappable::numObjectsCreated());
            }

            typedef bdlb::NullableAllocatedValue<Swappable> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // member 'swap'

                Swappable::reset();

                mX.swap(mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(0 == Swappable::numObjectsCreated());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                Swappable::reset();

                swap(mX, mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(0 == Swappable::numObjectsCreated());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const Swappable VV(10);

                // 'swap' member called on non-null object.
                {
                    Obj mX(VV);  const Obj& X = mX;
                    Obj mY;      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // member 'swap'

                    Swappable::reset();

                    mX.swap(mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(0 == Swappable::numObjectsCreated());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    Swappable::reset();

                    swap(mX, mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(0 == Swappable::numObjectsCreated());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }

                // 'swap' member called on null object.
                {
                    Obj mX;      const Obj& X = mX;
                    Obj mY(VV);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // member 'swap'

                    Swappable::reset();

                    mX.swap(mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(0 == Swappable::numObjectsCreated());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    Swappable::reset();

                    swap(mX, mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(0 == Swappable::numObjectsCreated());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const Swappable UU(10);
                const Swappable VV(20);

                Obj mX(UU);  const Obj& X = mX;
                Obj mY(VV);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // member 'swap'

                Swappable::reset();

                mX.swap(mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(0 == Swappable::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());

                // free 'swap'

                Swappable::reset();

                swap(mX, mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(0 == Swappable::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());
            }
        }

        if (verbose) cout << "Testing w/type taking an allocator." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            if (veryVerbose) cout
                << "\tSanity test 'SwappableWithAllocator' type." << endl;
            {
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!SwappableWithAllocator::swapCalled());

                SwappableWithAllocator obj1(1, &scratch);
                SwappableWithAllocator obj2(2, &scratch);
                ASSERT(2 == SwappableWithAllocator::numObjectsCreated());

                const SwappableWithAllocator Zobj1(obj1, &scratch);
                const SwappableWithAllocator Zobj2(obj2, &scratch);
                ASSERT(4 == SwappableWithAllocator::numObjectsCreated());

                ASSERT(obj1 == Zobj1);
                ASSERT(obj2 == Zobj2);

                ASSERT(!SwappableWithAllocator::swapCalled());
                swap(obj1, obj2);
                ASSERT( SwappableWithAllocator::swapCalled());
                ASSERT(4 == SwappableWithAllocator::numObjectsCreated());

                ASSERT(obj2 == Zobj1);
                ASSERT(obj1 == Zobj2);

                SwappableWithAllocator::reset();
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
            }

            typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX(&oa);  const Obj& X = mX;
                Obj mY(&oa);  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const SwappableWithAllocator VV(88, &scratch);

                // 'swap' member called on non-null object.
                {
                    Obj mX(VV, &oa);  const Obj& X = mX;
                    Obj mY(&oa);      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // member 'swap'

                    SwappableWithAllocator::reset();

                    mX.swap(mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }

                // 'swap' member called on null object.
                {
                    Obj mX(&oa);      const Obj& X = mX;
                    Obj mY(VV, &oa);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // member 'swap'

                    SwappableWithAllocator::reset();

                    mX.swap(mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const SwappableWithAllocator UU(88, &scratch);
                const SwappableWithAllocator VV(99, &scratch);

                Obj mX(UU, &oa);  const Obj& X = mX;
                Obj mY(VV, &oa);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());
            }
        }

        if (verbose) cout << "Testing free 'swap' w/different allocators."
                          << endl;
        {
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX(&oa);  const Obj& X = mX;
                Obj mY(&za);  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(0 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const SwappableWithAllocator VV(88, &scratch);

                // non-null object as first argument
                {
                    Obj mX(VV, &oa);  const Obj& X = mX;
                    Obj mY(&za);      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(1 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }

                // null object as first argument
                {
                    Obj mX(&oa);      const Obj& X = mX;
                    Obj mY(VV, &za);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(1 == SwappableWithAllocator::numObjectsCreated());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const SwappableWithAllocator UU(88, &scratch);
                const SwappableWithAllocator VV(99, &scratch);

                Obj mX(UU, &oa);  const Obj& X = mX;
                Obj mY(VV, &za);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(2 == SwappableWithAllocator::numObjectsCreated());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdlb::NullableAllocatedValue<SwappableWithAllocator> Obj;

            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }

      } break;
      case 8: {
        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20140601;

        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Use 'bsl::string' for 'TYPE'.
        //
        //   Specify a set of unique values.  Construct and initialize all
        //   combinations (u, v) in the cross product.  Copy construct a
        //   control w from v, assign v to u, and assert that w == u and
        //   w == v.  Then test aliasing by copy constructing a control w from
        //   each u, assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue(const bdlb::NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nTesting assignment u = V." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], ALLOC);  const Obj& V = mV;

                    Obj mW(V, ALLOC);  const Obj& W = mW;

                    mU = V;

                    ASSERTV(U, W, U == W);
                    ASSERTV(V, W, V == W);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], ALLOC);  const Obj& U = mU;
                Obj mW(U,     ALLOC);  const Obj& W = mW;

                mU = U;

                ASSERTV(U, W, U == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //   Any value must be copy constructible without affecting the
        //   argument.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   Specify a set whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identical objects W and X using tested methods.  Then
        //   copy construct Y from X and use the equality operator to assert
        //   that both X and Y have the same value as W.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue(const bdlb::NullableAllocatedValue&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

        if (verbose) cout << "\nUsing bdlb::NullableAllocatedValue<"
                          << "bsl::string>." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = "456";

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X, ALLOC);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 That all 6 comparison operators work properly.  A null values is
        //:   always not equal to, and less than, any non-null value.  Two null
        //:   values are equal.
        //
        // Plan:
        //: 1 Use 'int' for 'TYPE'.
        //:
        //: 2 Create an array 'values' of 7 non-null nullable objects, none
        //:   equal to each other, sorted in value from lowest to highest.
        //:
        //: 3 Try comparisons of two non-null nullable objects.
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create 'const int UR = values[ii]' and non-null
        //:     'const Obj U(UV)'.
        //:
        //:   o Within that loop, iterate 'jj' through the indexes of 'values',
        //:     and in that loop create 'const int VR = values[jj]' and
        //:     non-null 'const Obj V(VR)'.
        //:
        //:   o Try every possible comparison of 'UR' and 'V', with 'UR' on the
        //:     lhs and 'V' on the rhs, whose results should match the boolean
        //:     results of comparing 'ii' and 'jj'.
        //:
        //:   o Try every possible comparison of 'U' and 'VR', with 'U' on the
        //:     lhs and 'VR' on the rhs, whose results should match the boolean
        //:     results of comparing 'ii' and 'jj'.
        //:
        //:   o Try every possible comparison of 'U' and 'V', whose results
        //:     should match the boolean results of comparing 'ii' and 'jj'.
        //:
        //: 4 Try comparisons between a null nullable value and non-nullable
        //:   'int'.
        //:   o Create a null nullable value 'N'.
        //:
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create 'const int RV(values[ii])'.
        //:
        //:   o In the loop, do all possible comparisons of 'N' and 'RV', and
        //:     'N' should always be less than 'RV', regardless of the value of
        //:     'RV'.
        //:
        //: 5 Try comparisons of one null object and one non-null object.
        //:   o Create a null nullable value 'N'.
        //:
        //:   o Iterate 'ii' through the indexes of 'values', and in that loop
        //:     create non-null 'const Obj NV(values[ii])'.
        //:
        //:   o In the loop, do all possible comparisons of 'N' and 'NV', and
        //:     'N' should always be less than 'NV', regardless of the value of
        //:     'NV'.
        //:
        //: 6 Try comparisons of two null nullable objects.
        //:   o Create two null nullable objects 'LN' and 'RN'
        //:
        //:   o Try every possible comparison between them, they are always
        //:     equal, not greater, not less.
        //
        // TESTING:
        //   bool operator==(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator==(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator==(const b_NV<TYPE>&, const TYPE&);
        //   bool operator!=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator!=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator!=(const b_NV<TYPE>&, const TYPE&);
        //   bool operator<( const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator<( const TYPE&,       const b_NV<TYPE>&);
        //   bool operator<( const b_NV<TYPE>&, const TYPE&);
        //   bool operator<=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator<=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator<=(const b_NV<TYPE>&, const TYPE&);
        //   bool operator>( const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator>( const TYPE&,       const b_NV<TYPE>&);
        //   bool operator>( const b_NV<TYPE>&, const TYPE&);
        //   bool operator>=(const b_NV<TYPE>&, const b_NV<TYPE>&);
        //   bool operator>=(const TYPE&,       const b_NV<TYPE>&);
        //   bool operator>=(const b_NV<TYPE>&, const TYPE&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING COMPARISON OPERATORS\n"
                             "============================\n";

        typedef bdlb::NullableAllocatedValue<int> Obj;

        const int values[] = { INT_MIN, -1000, -123, 0, +123, +1000, INT_MAX };
        enum { k_NUM_VALUES = sizeof values / sizeof *values };

        // Comparison between two non-null values.

        for (int ii = 0; ii < k_NUM_VALUES; ++ii) {
            const int RU = values[ii];
            const Obj U(RU);
            ASSERT(!U.isNull());

            if (veryVerbose) { T_ P_(ii) P(U) }

            for (int jj = 0; jj < k_NUM_VALUES; ++jj) {
                const int RV = values[jj];
                const Obj V(RV);
                ASSERT(!V.isNull());

                if (veryVeryVerbose) { T_ T_ P_(jj) P(V) }

                // lhs is nullable, but non-null, rhs is raw value

                ASSERTV(U, RV, (ii == jj) == (U == RV));
                ASSERTV(U, RV, (ii != jj) == (U != RV));

                ASSERTV(U, RV, (ii <  jj) == (U <  RV));
                ASSERTV(U, RV, (ii <= jj) == (U <= RV));
                ASSERTV(U, RV, (ii >  jj) == (U >  RV));
                ASSERTV(U, RV, (ii >= jj) == (U >= RV));

                // lhs is raw value, rhs is nullable, but non-null

                ASSERTV(RU, V, (ii == jj) == (RU == V));
                ASSERTV(RU, V, (ii != jj) == (RU != V));

                ASSERTV(RU, V, (ii <  jj) == (RU <  V));
                ASSERTV(RU, V, (ii <= jj) == (RU <= V));
                ASSERTV(RU, V, (ii >  jj) == (RU >  V));
                ASSERTV(RU, V, (ii >= jj) == (RU >= V));

                // both sides are nullable, but non-null

                ASSERTV(U, V, (ii == jj) == (U == V));
                ASSERTV(U, V, (ii != jj) == (U != V));

                ASSERTV(U, V, (ii <  jj) == (U <  V));
                ASSERTV(U, V, (ii <= jj) == (U <= V));
                ASSERTV(U, V, (ii >  jj) == (U >  V));
                ASSERTV(U, V, (ii >= jj) == (U >= V));
            }
        }

        // Comparison between null nullable values and non-nullable raw value.

        {
            const Obj N;
            ASSERT(N.isNull());

            for (int ii = 0; ii < k_NUM_VALUES; ++ii) {
                const int RV = values[ii];    // Raw Value

                ASSERT(!(N  == RV));
                ASSERT(!(RV == N ));

                ASSERT(  N  != RV );
                ASSERT(  RV != N  );

                ASSERT(  N  <  RV );
                ASSERT(!(RV <  N ));

                ASSERT(  N  <= RV );
                ASSERT(!(RV <= N ));

                ASSERT(!(N  >  RV));
                ASSERT(  RV >  N  );

                ASSERT(!(N  >= RV));
                ASSERT(  RV >= N  );
            }
        }

        // Comparison between null nullable values and non-null nullable value.

        {
            const Obj N;
            ASSERT(N.isNull());

            for (int ii = 0; ii < k_NUM_VALUES; ++ii) {
                const Obj NV(values[ii]);
                ASSERT(!NV.isNull());

                ASSERT(!(N  == NV));
                ASSERT(!(NV == N ));

                ASSERT(  N  != NV );
                ASSERT(  NV != N  );

                ASSERT(  N  <  NV );
                ASSERT(!(NV <  N ));

                ASSERT(  N  <= NV );
                ASSERT(!(NV <= N ));

                ASSERT(!(N  >  NV));
                ASSERT(  NV >  N  );

                ASSERT(!(N  >= NV));
                ASSERT(  NV >= N  );
            }
        }

        // Comparison between two null values.

        {
            const Obj LN, RN;
            ASSERT(LN.isNull());
            ASSERT(RN.isNull());

            ASSERT(  LN == RN );
            ASSERT(!(LN != RN));

            ASSERT(!(LN <  RN));
            ASSERT(  LN <= RN );

            ASSERT(!(LN >  RN));
            ASSERT(  LN >= RN);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   The print method and output (<<) operator work as expected.
        //
        // Plan:
        //   Conduct the test using 'int' for 'TYPE'.
        //
        //   For a set of values, check that the 'print' function and the
        //   output (<<) operator work as expected.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //   bsl::ostream& operator<<bsl::ostream&, const b_NV<T>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Print Method & Output (<<) Operator"
                          << "\n==========================================="
                          << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        const ValueType VALUE1          = 123;
        const char      NULL_RESULT[]   = "NULL";
        const char      VALUE1_RESULT[] = "123";

        if (verbose) cout << "\nTesting 'print' Method." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting Output (<<) Operator." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;
                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   Verify that the primary manipulators and basic accessors work as
        //   expected.
        //
        // Concerns:
        //   * The default constructor creates a null object.
        //   * 'makeValue()' sets the value to the default value for 'TYPE'.
        //   * 'makeValue(const TYPE&)' sets the value appropriately.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   First, verify the default constructor by testing that the
        //   resulting object is null.
        //
        //   Next, verify that the 'makeValue' function works by making a value
        //   equal to the value passed into 'makeValue'.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdlb::NullableAllocatedValue();
        //   ~bdlb::NullableAllocatedValue();
        //   TYPE& makeValue();
        //   BOOTSTRAP: TYPE& makeValue(const TYPE&);
        //   bool isNull() const;
        //   const TYPE& value() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators & Basic Accessors"
                          << "\n=============================================="
                          << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<int>'."
                          << endl;
        {
            typedef int                                     ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX;  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(3);  // set some random value
                mX.makeValue();   // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;
                const ValueType VALUE2 = 456;

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE2 == X.value());
            }
        }

        if (verbose) cout << "\nUsing 'bdlb::NullableAllocatedValue<"
                          << "bsl::string>'." << endl;
        {
            typedef bsl::string                             ValueType;
            typedef bdlb::NullableAllocatedValue<ValueType> Obj;

            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            {
                Obj mX(ALLOC);  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue("3");  // set some random value
                mX.makeValue();     // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";

                mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX(ALLOC);  const Obj& X = mX;

                const ValueType VALUE1 = "123";
                const ValueType VALUE2 = "456";

                mX.makeValue(VALUE1);
                mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERTV(X.value(), VALUE2 == X.value());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST USING 'int'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isNull'.
        //
        // Plan:
        //   Create four test objects using the default, value, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        typedef int                                     ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        // possible values
        const ValueType VA(123);
        const ValueType VB(234);
        const ValueType VC(345);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST USING 'bsl::string'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isSet'.
        //
        // Plan:
        //   Create four test objects using the default, value, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   This test case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        typedef bsl::string                             ValueType;
        typedef bdlb::NullableAllocatedValue<ValueType> Obj;

        // possible values
        const ValueType VA("The");
        const ValueType VB("Breathing");
        const ValueType VC("Test");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
