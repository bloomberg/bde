// bsltf_copymovetracker.t.cpp                                        -*-C++-*-

#include <bsltf_copymovetracker.h>

#include <bslmf_util.h>

#include <bsls_annotation.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <cstdio>   // 'printf'
#include <cstdlib>  // 'atoi'

#ifdef BDE_VERIFY
// Suppress some pedantic bde_verify checks in this test driver
#pragma bde_verify -FD01   // Function declaration requires contract
#pragma bde_verify -FD03   // Parameter not documented in function contract
#pragma bde_verify -FABC01 // Function not in alphabetical order
#pragma bde_verify -TP19   // Missing or malformed standard test driver section
#pragma bde_verify -TY02   // Template parameter uses single-letter name
#endif

using std::printf;
using std::fprintf;
using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// 'CopyMoveTracker' retains a limited history of move and copy operations.
// Testing consists of ensuring that each constructor and method maintains and
// reports this history correctly.  The 'setCopyMoveState' manipulator can put
// an object into any valid state, and thus can be used to establish a starting
// condition for each test.  Because 'CopyMoveState' is designed to be
// inherited from, limited tests are performed using derived classes;
// specifically, constructing from a derived-class object should track the
// copy/move history appropriately.
// ----------------------------------------------------------------------------
// CREATORS
// [ 4] CopyMoveTracker();
// [ 5] CopyMoveTracker(const CopyMoveTracker& original);
// [ 5] CopyMoveTracker(CopyMoveTracker& original);
// [ 6] CopyMoveTracker(bslmf::MovableRef<CopyMoveTracker> original);
// [ 6] CopyMoveTracker(bslmf::MovableRef<DERIVED> original);
//
// MANIPULATORS
// [ 7] CopyMoveTracker& operator=(const CopyMoveTracker& original);
// [ 7] CopyMoveTracker& operator=(CopyMoveTracker& original);
// [ 8] CopyMoveTracker& operator=(CopyMoveTracker&& original);
// [ 8] CopyMoveTracker& operator=(DERIVED&& original);
// [ 9] void resetCopyMoveState();
// [ 2] void setCopyMoveState(CopyMoveState::Enum state);
// [12] void swapCopyMoveStates(CopyMoveTracker& b);
//
// ACCESSORS
// [ 2] CopyMoveState::Enum copyMoveState() const;
// [ 3] bool isCopiedConstInto() const;
// [ 3] bool isCopiedInto() const;
// [ 3] bool isCopiedNonconstInto() const;
// [ 3] bool isMovedFrom() const;
// [ 3] bool isMovedInto() const;
// [ 3] bool isOriginal() const;
//
// HIDDEN FRIENDS (FREE FUNCTIONS)
// [10] bool operator==(const CopyMoveTracker&, const CopyMoveTracker&);
// [10] bool operator!=(const CopyMoveTracker&, const CopyMoveTracker&);
// [11] CopyMoveState::Enum copyMoveState(const CopyMoveTracker&);
// [11] void setCopyMoveState(CopyMoveTracker *, CopyMoveState::Enum);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [13] USAGE EXAMPLES
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                                VERBOSITY
// ----------------------------------------------------------------------------

int verbose             = 0;
int veryVerbose         = 0;
int veryVeryVerbose     = 0;
int veryVeryVeryVerbose = 0; // For test allocators

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsltf::CopyMoveTracker Obj;
typedef bsltf::CopyMoveState   CMS;
typedef CMS::Enum              Enum;

class Client {
    // Class that uses a 'CopyMoveTracker'.

    typedef bslmf::MovableRefUtil MoveUtil;

    Obj d_tracker;
    int d_val;

  public:
    explicit Client(int i = 42) : d_val(i) { }
    Client(const Client& original)
        : d_tracker(original.d_tracker), d_val(original.d_val) { }

    Client(bslmf::MovableRef<Client> original)
        : d_tracker(MoveUtil::move(MoveUtil::access(original).d_tracker))
        , d_val(MoveUtil::access(original).d_val)
        { }

    void setCopyMoveState(CMS::Enum state)
        { d_tracker.setCopyMoveState(state); }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend bool operator==(const Client&, const Client&) = default;
#endif
};

class DerivedClient : public Obj {
    // Class inherited from 'CopyMoveTracker'.  The psuedo-accessor
    // 'CopyMoveState::get' and psuedo-manipulator 'CopyMoveState::set' work
    // automatically as a consequence of this inheritance and do not need to be
    // customized for this class.

    typedef bslmf::MovableRefUtil MoveUtil;

    int d_val;

  public:
    explicit DerivedClient(int i = 42) : Obj(), d_val(i) { }
    DerivedClient(const DerivedClient& original)
        : Obj(original), d_val(original.d_val) { }

    DerivedClient(bslmf::MovableRef<DerivedClient> original)
        : Obj(MoveUtil::move(original))
        , d_val(MoveUtil::access(original).d_val)
        { }
};

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const Enum VALID_STATE_LIST[] = {
    CMS::e_ORIGINAL            ,
    CMS::e_COPIED_INTO         ,
    CMS::e_COPIED_CONST_INTO   ,
    CMS::e_COPIED_NONCONST_INTO,
    CMS::e_MOVED_INTO          ,
    CMS::e_MOVED_FROM          ,

    Enum(CMS::e_COPIED_INTO          | CMS::e_MOVED_FROM),
    Enum(CMS::e_COPIED_CONST_INTO    | CMS::e_MOVED_FROM),
    Enum(CMS::e_COPIED_NONCONST_INTO | CMS::e_MOVED_FROM),
    Enum(CMS::e_MOVED_INTO           | CMS::e_MOVED_FROM),

    CMS::e_UNKNOWN
};

const std::size_t NUM_VALID_STATES = sizeof(VALID_STATE_LIST) / sizeof(Enum);

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A tracked value class
/// - - - - - - - - - - - - - - - -
// In this example, we create a class that holds an integer value and tracks
// moves and copies.
//
// First, we define the class and it's data members, including a
// 'CopyMoveTracker' to keep track of the moves and copies:
//..
    class TrackedValue {
        bsltf::CopyMoveTracker d_tracker;
        int                    d_value;
//..
// Next, we define the constructors for 'TrackedValue' such that they forward
// appropriately the 'CopyMoveTracker' member appropriately:
//..
      public:
        explicit TrackedValue(int v = 0) : d_tracker(), d_value(v) { }
        TrackedValue(const TrackedValue& original)
            : d_tracker(original.d_tracker), d_value(original.d_value) { }
        TrackedValue(TrackedValue& original)
            : d_tracker(original.d_tracker), d_value(original.d_value) { }
        TrackedValue(bslmf::MovableRef<TrackedValue> original)
            : d_tracker(bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).d_tracker))
        {
            TrackedValue& originalLvalue = original;
            d_value = originalLvalue.d_value;
            originalLvalue.d_value = -1;
        }
//..
// For this example, we don't need to define the assignment operators, but
// their implemenation would be similar to the corresponding constructors.
//
// Next, we define an accessor to return the value of our tracked value.
//..
        int value() const { return d_value; }
//..
// Then, we define a hidden friend function, 'copyMoveState', that returns the
// copy/move state.  This friend function is an ADL customization point that
// allows 'CopyMoveState::get(obj)' to return the copy/move state when 'obj' is
// a tracked value and allows boolean psuedo-accessors such as
// 'CopyMoveState::isMovedFrom(obj)' to query the copy/move state:
//..
        friend bsltf::CopyMoveState::Enum copyMoveState(const TrackedValue& v)
            { return v.d_tracker.copyMoveState(); }
    };
//..
// Next, we define equality-comparison operators for 'TrackedValue'.  Note that
// only the value attribute is compared; the copy/move state is not a salient
// attribute of the class and is thus not part of its value:
//..
    bool operator==(const TrackedValue &a, const TrackedValue &b)
    {
        return a.value() == b.value();
    }

    BSLS_ANNOTATION_UNUSED
    bool operator!=(const TrackedValue &a, const TrackedValue &b)
    {
        return a.value() != b.value();
    }
//..
// Now we use 'TrackedValue' in a program, beginning by constructing a
// variable.  The variable is in the not-copied-or-moved state:
//..
    void usageExample1()
    {
        TrackedValue tv1(99);
        ASSERT(99 == tv1.value());
        ASSERT(bsltf::CopyMoveState::isOriginal(tv1));
//..
// Finally, we make a copy of our 'TrackedValue' variable.  The copy is in a
// copied-into state, but it still has the same *value* as 'tv1':
//..
        TrackedValue tv2(tv1);
        ASSERT(99 == tv2.value());
        ASSERT(bsltf::CopyMoveState::isCopiedInto(tv2));
        ASSERT(bsltf::CopyMoveState::isCopiedNonconstInto(tv2));
        ASSERT(tv2 == tv1);
    }
//..
//
///Example 2: Testing a wrapper template
/// - - - - - - - - - - - - - - - - - -
// In this example, we test a simple wrapper template, 'CountedWrapper<T>',
// that holds an object of type 'T' and counts the number of such wrapper
// object currently live in the program.  We begin by sketching the wrapper
// template being tested (with unnecessary details left out):
//..
//  #include <bslmf_util.h>

    template <class TYPE>
    class CountedWrapper {
        // Hold an object of 'TYPE' and count the number of objects.

        // CLASS DATA
        static int s_count;

        // DATA
        TYPE d_object;

      public:
        // CLASS METHODS
        static int count() { return s_count; }

        // CREATORS
        CountedWrapper() { ++s_count; }

        template <class ARG>
        explicit
        CountedWrapper(BSLS_COMPILERFEATURES_FORWARD_REF(ARG) ctorArg)
            // Construct the wrapped object by forwarding the specified
            // 'ctorArg' to the constructor for 'TYPE'.
            : d_object(BSLS_COMPILERFEATURES_FORWARD(ARG, ctorArg))
            { ++s_count; }

        ~CountedWrapper() { --s_count; }

        // ...

        // ACCESSORS
        const TYPE& object() const { return d_object; }
    };

    template <class TYPE>
    int CountedWrapper<TYPE>::s_count = 0;
//..
// Next, we instantiat our wrapper with the 'TrackedValue' class from Example 1
// so that we can track whether the argument passed to the wrapper constructor
// is correctly passed to the wrapped object, including preserving its value
// category:
//..
    typedef CountedWrapper<TrackedValue> WrappedValue;
//..
// Next, we check that a value-constructed wrapper results in a tracked value
// that has not be copied or moved; i.e., no temporaries were created and then
// copied.  Checking the copy/move state requires calling static methods of
// 'bsltf::CopyMoveState'; we make such calls terser by defining 'Cms' as an
// abbreviation for 'bsltf::CopyMoveState':
//..
    void usageExample2()
    {
        typedef bsltf::CopyMoveState Cms;

        WrappedValue wv1(99);  // Default constructor
        ASSERT(1 == WrappedValue::count());
        ASSERT(99 == wv1.object().value());
        ASSERT(Cms::isOriginal(wv1.object()));
//..
// Next, we check that a wrapper constructed from a 'TrackedValue' variable
// forwards the variable as an lvalue, resulting in a call to the copy
// constructor.  We also check that, in C++11, the lvalue is perfectly
// forwarded as a non-const lvalue:
//..
        TrackedValue t2(44);
        ASSERT(Cms::isOriginal(t2));

        WrappedValue wv2(t2);
        ASSERT(44 == t2.value());                 // Unchanged
        ASSERT(Cms::isOriginal(t2));              // Unchanged
        ASSERT(2 == WrappedValue::count());
        ASSERT(44 == wv2.object().value());
        ASSERT(Cms::isCopiedInto(wv2.object()));  // Copy constructed
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Copy constructed from non-const original
        ASSERT(Cms::isCopiedNonconstInto(wv2.object()));
#endif
//..
// Finally, we check that a wrapper constructed from a moved 'TrackedValue'
// forwards the variable as an rvalue, resulting in a call to the move
// constructor.  Note that original variable is also modified by this
// operation:
//..
        TrackedValue t3(t2);
        ASSERT(44 == t3.value());
        ASSERT(Cms::isCopiedNonconstInto(t3));

        WrappedValue wv3(bslmf::MovableRefUtil::move(t3));
        ASSERT(-1 == t3.value());
        ASSERT(Cms::isCopiedNonconstInto(t3));
        ASSERT(Cms::isMovedFrom(t3));
        ASSERT(3 == WrappedValue::count());
        ASSERT(44 == wv3.object().value());
        ASSERT(Cms::isMovedInto(wv3.object()));
    }
//..

}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 13: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        //: 1 That the usage examples shown in the component-level
        //:   documentation compile and run as described.
        //
        // Plan:
        //: 1 Copy the usage examples from the component header, changing
        //    'assert' to 'ASSERT' and execute them.
        //
        // Testing:
        //     USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();
        usageExample2();

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'swapCopyMoveStates'
        //
        // Concerns:
        //: 1 'a.swapCopyMoveStates(b)' sets copy/move state of both 'a' and
        //:   'b' to the bitwise OR of 'e_MOVED_INTO' and 'e_MOVED_FROM',
        //:   regardless of the initial states of the a and b 'CopyMoveTracker'
        //:   objects.
        //
        // Plan:
        //: 1 In a pair of nested loops setting two 'CopyMoveTracker' objects
        //:   to each valid copy/move state.  Call 'a.swapCopyMoveStates(b)'
        //:   and verify that both objects are set to the 'e_MOVED_INTO |
        //:   e_MOVED_FROM' state.  (C-1)
        //
        // Testing:
        //     void swapCopyMoveStates(CopyMoveTracker& b);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swapCopyMoveStates'"
                            "\n===========================\n");

        Obj mA; const Obj& a = mA;
        Obj mB; const Obj& b = mB;

        const Enum exp = Enum(CMS::e_MOVED_INTO | CMS::e_MOVED_FROM);

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum iE = VALID_STATE_LIST[i];
            mA.setCopyMoveState(iE);

            for (std::size_t j = 0; j < NUM_VALID_STATES; ++j) {
                const Enum jE = VALID_STATE_LIST[j];
                mB.setCopyMoveState(jE);

                mA.swapCopyMoveStates(mB);

                ASSERTV(iE, jE, exp == a.copyMoveState());
                ASSERTV(iE, jE, a.isMovedInto());
                ASSERTV(iE, jE, a.isMovedFrom());
                ASSERTV(iE, jE, exp == b.copyMoveState());
                ASSERTV(iE, jE, b.isMovedInto());
                ASSERTV(iE, jE, b.isMovedFrom());
            }
        }

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // HIDDEN FRIENDS
        //
        // Concerns:
        //: 1 For any 'CopyMoveTracker' object, 'x', an unqualified call to
        //:   'setCopyMoveState(x, s)' has the same effect as
        //:   'x.setCopyMoveState(s)'.
        //: 2 For any 'CopyMoveTracker' object, 'x', 'CopyMoveState::set(x, s)'
        //:   invokes 'setCopyMoveState(x, s)'.
        //: 3 For any 'CopyMoveTracker' object, 'x', an unqualified call to
        //:   'copyMoveState(x)' returns the same value as 'x.copyMoveState()'.
        //: 4 For any 'CopyMoveTracker' object, 'x', 'CopyMoveState::get(x)'
        //:   returns 'copyMoveState(x)'.
        //: 5 The above concerns apply to classes derived from
        //:   'CopyMoveTracker', even if they do not customize
        //:   'setCopyMoveState' and/or 'copyMoveState'.
        //
        // Plan:
        //: 1 Loop over every valid state, 'S'.  Within the loop, create a
        //:   'CopyMoveTracker' object 'mX' and const reference 'X'.  Then:
        //:
        //:   1 Use an unqualified call to 'setCopyMoveState(&mX, S)' to put a
        //:     'mX' into state 'S' and verify the expected effect.  (C-1)
        //:   2 Reset 'mX' and use 'CopyMoveState::set(&mX, S)' to put a 'mX'
        //:     into state 'S' and verify the expected effect.  (C-2)
        //:   3 Verify that an unqualified call to 'copyMoveState(X)' returns
        //:     'S'.  (C-3)
        //:   4 Verify that 'CopyMoveState::get(X)' returns 'S' (C-4)
        //:
        //: 2 Repeat the entire test using a derived class, 'DerivedClient'
        //:   insetead of 'CopyMoveTracker directly.  (C-5)
        //
        // Testing:
        //      CopyMoveState::Enum copyMoveState(const CopyMoveTracker&);
        //      void setCopyMoveState(CopyMoveTracker *, CopyMoveState::Enum);
        // --------------------------------------------------------------------

        if (verbose) printf("\nHIDDEN FRIENDS"
                            "\n==============\n");

        // Make sure default implementations are in scope
        using namespace bsltf;

        // Step 1
        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum E = VALID_STATE_LIST[i];

            Obj mX; const Obj& X = mX;

            // Step 1.1
            setCopyMoveState(&mX, E);
            ASSERTV(E, E == X.copyMoveState());

            // Step 1.2
            mX.resetCopyMoveState();
            CMS::set(&mX, E);
            ASSERTV(E, E == X.copyMoveState());

            // Step 1.3
            ASSERTV(E, copyMoveState(X),  E == copyMoveState(X));
            ASSERTV(E, copyMoveState(X) == X.copyMoveState());

            // Step 1.4
            ASSERTV(E, CMS::get(X), E == CMS::get(X));
        }

        // Step 2
        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum E = VALID_STATE_LIST[i];

            DerivedClient mX; const Obj& X = mX;

            // Step 2.1
            setCopyMoveState(&mX, E);
            ASSERTV(E, E == X.copyMoveState());

            // Step 2.2
            mX.resetCopyMoveState();
            CMS::set(&mX, E);
            ASSERTV(E, E == X.copyMoveState());

            // Step 2.3
            ASSERTV(E, copyMoveState(X),  E == copyMoveState(X));
            ASSERTV(E, copyMoveState(X) == X.copyMoveState());

            // Step 2.4
            ASSERTV(E, CMS::get(X), E == CMS::get(X));
        }

      } break;

      case 10: {
        // --------------------------------------------------------------------
        // EQUALITY COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 'operator==' always returns 'true' regardless of the copy/move
        //:   states of the lhs and rhs 'CopyMoveTracker' objects.
        //: 2 'operator!=' always returns 'false'.
        //: 3 In C++20, a derived class declaring a defaulted 'operator==' will
        //:   correctly determine that it's base class always returns equal.
        //
        // Plan:
        //: 1 In a pair of nested loops setting two 'CopyMoveTracker' objects
        //:   to each valid copy/move state, verify that comparing the two with
        //:   'operator==' always returns 'true' and comparing them with
        //:   'operator!=' always returns 'false'.  (C-1, C-2)
        //: 2 In C++20, create a derived class having a defaulted
        //:   'operator=='.  Within the loop from step 1, create derived-class
        //:   objects having the same and different values and the same or
        //:   different copy/move states.  Verify that the compiler-generated
        //:   equality operators correctly compare the objects' values and
        //:   ignores their copy/move states.  (C-3)
        //
        // Testing:
        //     bool operator==(const CopyMoveTracker&, const CopyMoveTracker&);
        //     bool operator!=(const CopyMoveTracker&, const CopyMoveTracker&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY COMPARISON OPERATORS"
                            "\n=============================\n");

        Obj mLhs; const Obj& lhs = mLhs;
        Obj mRhs; const Obj& rhs = mRhs;

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum iE = VALID_STATE_LIST[i];
            mLhs.setCopyMoveState(iE);

            for (std::size_t j = 0; j < NUM_VALID_STATES; ++j) {
                const Enum jE = VALID_STATE_LIST[j];
                mRhs.setCopyMoveState(jE);

                ASSERTV(iE, jE, true  == (lhs == rhs));
                ASSERTV(iE, jE, false == (lhs != rhs));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
                const int V1 = static_cast<int>(i);
                const int V2 = V1 + 1;  // Any value different from 'V1'

                Client mD1(V1); const Client& D1 = mD1;
                mD1.setCopyMoveState(iE);

                // Same value, possibly different state than D1
                Client mD2(V1); const Client& D2 = mD2;
                mD2.setCopyMoveState(jE);

                // Different value, possibly different state than D1
                Client mD3(V2); const Client& D3 = mD3;
                mD3.setCopyMoveState(jE);

                ASSERTV(iE, jE,   (D1 == D2));
                ASSERTV(iE, jE,   (D2 == D1));
                ASSERTV(iE, jE, ! (D1 != D2));
                ASSERTV(iE, jE, ! (D2 != D1));
                ASSERTV(iE, jE, ! (D1 == D3));
                ASSERTV(iE, jE, ! (D3 == D1));
                ASSERTV(iE, jE,   (D1 != D3));
                ASSERTV(iE, jE,   (D3 != D1));
#endif
            }
        }

      } break;

      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'resetCopyMoveState'
        //
        // Concerns:
        //: 1 Regardless of the starting state, 'resetCopyMoveState()' will
        //:   reset the state of a 'CopyMoveTracker' to 'e_ORIGINAL'.
        //
        // Plan:
        //: 1 In a loop, put a 'CopyMoveTracker' into each valid state, then
        //:   call 'resetCopyMoveState'.  Verify that the 'copyMoveState' then
        //:   returns 'e_ORIGINAL'.
        //
        // Testing:
        //      void resetCopyMoveState();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'resetCopyMoveState'"
                            "\n============================\n");

        Obj x;

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum E = VALID_STATE_LIST[i];

            x.setCopyMoveState(E);
            ASSERT(E == x.copyMoveState());

            x.resetCopyMoveState();
            ASSERT(CMS::e_ORIGINAL == x.copyMoveState());
            ASSERT(x.isOriginal());
            ASSERT(! x.isMovedFrom());
        }

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // MOVE ASSIGNMENT OPERATORS
        //
        // Concerns:
        //: 1 After a move assigment, the lhs 'CopyMoveTracker' has a copy/move
        //:   state of 'e_MOVED_INTO'.
        //: 2 Assignming from an rvalue of class derived from
        //:   'CopyMoveTracker' has the same effect as move assignment.
        //: 3 The copy/move state of the moved-from (rhs) object is modified by
        //:   setting the 'e_MOVED_FROM' bit; the remaining bits of the
        //:   copy/move state are unchanged.
        //
        // Plan:
        //: 1 In a pair of nested loops, create lhs and rhs 'CopyMoveTracker'
        //:   objects having with a different copy/move states in each
        //:   iteration.  Move-construct from the rhs to the lhs object and
        //:   verify that the lhs object is set to the moved-into state.  (C-1)
        //: 2 In the same loops as step 1, create an object of class derived
        //:   from 'CopyMoveTracker', with the same copy/move state as the
        //:   previous rhs object.  Move assign from the derived-class object
        //:   to lhs and verify that the lhs object is set to the moved-into
        //:   state.  (C-2)
        //: 3 For both steps above, verify that the rhs object's copy/move
        //:   state is unhchanged except that the 'e_MOVED_FROM' bit is set.
        //:   (C-3)
        //
        // Testing:
        //      CopyMoveTracker& operator=(CopyMoveTracker&& original);
        //      CopyMoveTracker& operator=(DERIVED&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE ASSIGNMENT OPERATORS"
                            "\n=========================\n");

        Obj lhs;

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum e     = VALID_STATE_LIST[i];
            const bool valid = (e != CMS::e_UNKNOWN);

            for (std::size_t j = 0; j < NUM_VALID_STATES; ++j) {
                {
                    Obj rhs;
                    rhs.setCopyMoveState(e);
                    lhs.setCopyMoveState(VALID_STATE_LIST[j]);
                    lhs = bslmf::MovableRefUtil::move(rhs);
                    ASSERTV(e, lhs.isMovedInto());
                    ASSERTV(e, CMS::e_MOVED_INTO == lhs.copyMoveState());
                    ASSERTV(e, valid == rhs.isMovedFrom());
                    ASSERTV(e, (e|CMS::e_MOVED_FROM) == rhs.copyMoveState());
                }
#if 0
                {
                    Derived rhs;
                    rhs.setCopyMoveState(e);
                    lhs.setCopyMoveState(VALID_STATE_LIST[j]);
                    lhs = bslmf::MovableRefUtil::move(rhs);
                    ASSERTV(e, lhs.isMovedInto());
                    ASSERTV(e, CMS::e_MOVED_INTO == lhs.copyMoveState());
                    ASSERTV(e, valid == rhs.isMovedFrom());
                    ASSERTV(e, (e|CMS::e_MOVED_FROM) == rhs.copyMoveState());
                }
#endif // 0
            }
        }

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATORS
        //
        // Concerns:
        //: 1 After a copy assignment, calling 'isCopied' on the lhs
        //:   'CopyMoveTracker' returns 'true'.
        //: 2 If the rhs object is a 'const' lvalue, then the lhs object
        //:   gets a copy/move state of 'e_COPIED_CONST_INTO'.
        //: 3 If the rhs object is a non-'const' lvalue, then the lhs
        //:   object gets a copy/move state of 'e_COPIED_NONCONST_INTO'.
        //: 4 The rhs object's state is unchanged by the assignment.
        //: 5 The above concerns apply regardless of the either object's
        //:   initial copy/move state.
        //
        // Plan:
        //: 1 Construct the rhs 'CopyMoveTracker' object and create a 'const'
        //:   reference to it.
        //: 2 Construct the lhs 'CopyMoveTracker' object and assign to it from
        //:   the rhs object and the 'const' reference to the rhs object.
        //:   Verify that each lhs has the expected copy/move state.  (C-1,
        //:   C-2, C-3)
        //: 3 Verify that the rhs object's copy/move state is unchanged.  (C-4)
        //: 4 Repeat the previous steps in a pair of nested loops, setting the
        //:   copy/move state of the lhs and rhs objects before preforming the
        //:   assignments.  Verify that the original states have no effect on
        //:   the resulting state of the lhs and that the rhs remains unchanged
        //:   by the copies.  (C-5)
        //
        // Testing:
        //      CopyMoveTracker& operator=(const CopyMoveTracker& original);
        //      CopyMoveTracker& operator=(CopyMoveTracker& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY ASSIGNMENT OPERATORS"
                            "\n=========================\n");

        Obj rhs; const Obj& constRhs = rhs;
        Obj lhs;

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum e = VALID_STATE_LIST[i];
            rhs.setCopyMoveState(e);

            for (std::size_t j = 0; j < NUM_VALID_STATES; ++j) {
                lhs.setCopyMoveState(VALID_STATE_LIST[j]);

                lhs = constRhs;
                ASSERTV(e, lhs.isCopiedInto());
                ASSERTV(e, CMS::e_COPIED_CONST_INTO == lhs.copyMoveState());
                ASSERTV(e, e == constRhs.copyMoveState());  // Unchanged

                lhs.setCopyMoveState(VALID_STATE_LIST[j]);
                lhs = rhs;
                ASSERTV(e, lhs.isCopiedInto());
                ASSERTV(e, CMS::e_COPIED_NONCONST_INTO == lhs.copyMoveState());
                ASSERTV(e, e == constRhs.copyMoveState());  // Unchanged
            }
        }

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        //
        // Concerns:
        //: 1 A move constructed 'CopyMoveTracker' object has a copy/move state
        //:   of 'e_MOVED_INTO'.
        //: 2 Constructing from an rvalue of class derived from
        //:   'CopyMoveTracker' has the same effect as move construction.
        //: 3 The copy/move state of the moved-from object is modified by
        //:   setting the 'e_MOVED_FROM' bit; the remaining bits of the
        //:   copy/move state are unchanged.
        //
        // Plan:
        //: 1 In a loop, create an original 'CopyMoveTracker' with a different
        //:   copy/move state in each iteration.  Move-construct an object from
        //:   the original and verify that the new object is in the moved-into
        //:   state.  (C-1)
        //: 2 In the same loop as step 1, create an object of class derived
        //:   from 'CopyMoveTracker', with a different copy/move state in each
        //:   iteration.  Construct a 'CopyMoveTracker' object from an rvalue
        //:   reference to the derived-class object and verify that the new
        //:   object is in the moved-into state.  (C-2)
        //: 3 For both steps above, verify that the original object's copy/move
        //:   state is unhchanged except that the 'e_MOVED_FROM' bit is set.
        //:   (C-3)
        //
        // Testing:
        //      CopyMoveTracker(bslmf::MovableRef<CopyMoveTracker> original);
        //      CopyMoveTracker(bslmf::MovableRef<DERIVED> original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTORS"
                            "\n=================\n");

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum e     = VALID_STATE_LIST[i];
            const bool valid = ! (e & CMS::e_UNKNOWN);

            {
                Obj original;
                original.setCopyMoveState(e);
                Obj newObj(bslmf::MovableRefUtil::move(original));
                ASSERTV(e, newObj.isMovedInto());
                ASSERTV(e, CMS::e_MOVED_INTO == newObj.copyMoveState());
                ASSERTV(e, valid == original.isMovedFrom());
                ASSERTV(e, (e|CMS::e_MOVED_FROM) == original.copyMoveState());
            }
#if 0
            {
                Derived original;
                original.setCopyMoveState(e);
                Obj newObj(bslmf::MovableRefUtil::move(original));
                ASSERTV(e, newObj.isMovedInto());
                ASSERTV(e, CMS::e_MOVED_INTO == newObj.copyMoveState());
                ASSERTV(e, valid == original.isMovedFrom());
                ASSERTV(e, (e|CMS::e_MOVED_FROM) == original.copyMoveState());
            }
#endif // 0
        }

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 A copy constructed 'CopyMoveTracker' returns 'true' from the
        //:   'isCopied()' accessor.
        //: 2 If the original object is a 'const' lvalue, then the new object
        //:   has a copy/move state of 'e_COPIED_CONST_INTO'.
        //: 3 If the original object is a non-'const' lvalue, then the new
        //:   object has a copy/move state of 'e_COPIED_NONCONST_INTO'.
        //: 4 The original object's state is unchanged.
        //: 5 The above concerns apply regardless of the original object's
        //:   copy/move state.
        //
        // Plan:
        //: 1 Construct an original 'CopyMoveTracker' object and create a
        //:   'const' reference to it.
        //: 2 Copy construct one 'CopyMoveTracker' object from the original
        //:   object and one from the 'const' reference to the original object.
        //:   Verify that each copy has the expected copy/move state.  (C-1,
        //:   C-2, C-3)
        //: 3 Verify that the original object's copy/move state is unchanged.
        //:   (C-4)
        //: 4 Repeat the previous steps in a loop, changing the copy/move state
        //:   of the original object before making the copies.  Verify that the
        //:   original state has no effect on the state of the copies and that
        //:   the original remains unchanged by the copies.  (C-5)
        //
        // Testing:
        //      CopyMoveTracker(const CopyMoveTracker& original);
        //      CopyMoveTracker(CopyMoveTracker& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        Obj original; const Obj& constOriginal = original;

        for (std::size_t i = 0; i < NUM_VALID_STATES; ++i) {
            const Enum e = VALID_STATE_LIST[i];
            original.setCopyMoveState(e);

            Obj constCopy(constOriginal);
            ASSERTV(e, constCopy.isCopiedInto());
            ASSERTV(e, CMS::e_COPIED_CONST_INTO == constCopy.copyMoveState());
            ASSERTV(e, e == constOriginal.copyMoveState());  // Unchanged

            Obj nonconstCopy(original);
            ASSERTV(e, nonconstCopy.isCopiedInto());
            ASSERTV(e, CMS::e_COPIED_NONCONST_INTO ==
                       nonconstCopy.copyMoveState());
            ASSERTV(e, e == constOriginal.copyMoveState());  // Unchanged
        }

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        //
        // Concerns:
        //: 1 The default constructor creates a 'CopyMoveTracker' in the
        //:   'e_ORIGINAL' state.
        //
        // Plan:
        //: 1 Construct a 'CopyMoveTracker' object using the default
        //:   constructor.  Verify that 'copyMoveState' returns 'e_ORIGINAL'.
        //:   (C-1)
        //
        // Testing:
        //       CopyMoveTracker();
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CONSTRUCTOR"
                            "\n===================\n");

        Obj x;
        ASSERT(CMS::e_ORIGINAL == x.copyMoveState());
        ASSERT(x.isOriginal());
        ASSERT(! x.isMovedFrom());

      } break;

      case 3: {
        // -------------------------------------------------------------------
        // TESTING ACCESSORS
        //   Verify operation of the 'isCopiedInto', 'isMovedInto' and similar
        //   accessors that check specific bits in the copy/move state.
        //
        // Concerns:
        //: 1 When called on an object returning a copy/move state that
        //:   includes neither 'e_COPIED_INTO' nor the 'e_MOVED_INTO' bit,
        //:   'isOriginal' returns 'true' and all of the 'is*Into' queries
        //:   return 'false'.
        //: 2 Each query other than 'isOriginal' returns 'true' if the
        //:   copy/move state contains the bits from the corresponding
        //:   enumerator.
        //: 3 Setting the 'e_MOVED_FROM' bit in the copy/move state causes
        //:   'isMovedFrom' to return 'true' and does not change the result
        //:   from any of the other queries.
        //: 4 For a 'CopyMoveTracker', 't', 'CopyMoveState::isCopied(t)'
        //:   returns the same result as 't.isCopied()'.  The same is true for
        //:   the remaining accessors.
        //
        // Plan:
        //: 1 Use a table-driven approach to verify that each query produces
        //:   the expected value when called on an object returning each of the
        //:   copy/move states in the range 'e_ORIGINAL' to 'e_MOVED_INTO',
        //:   inclusive.  (C-1, C-2)
        //: 2 Verify that 'isMovedFrom' returns 'false' when called on each
        //:   object from step 1.  Then set the 'e_MOVED_FROM' bit in the
        //:   object's state, without modifying any other bits, and verify that
        //:   'isMovedFrom' now returns 'true' unless the 'e_UNKNOWN' bit is
        //:   set.  (C-3)
        //: 3 For each query in the preceeding steps, verify that the same
        //:   query made via the corresponding 'CopyMoveState' static member
        //:   function returns the same result.  (C-4)
        //
        // Testing:
        //     bool isCopiedConstInto() const;
        //     bool isCopiedInto() const;
        //     bool isCopiedNonconstInto() const;
        //     bool isMovedFrom() const;
        //     bool isMovedInto() const;
        //     bool isOriginal() const;
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        static const bool F = false;
        static const bool T = true;

        static const struct {
            Enum d_state;    // enumerator value
            bool d_expOriginal;
            bool d_expCopiedInto;
            bool d_expCopiedConstInto;
            bool d_expCopiedNonconstInto;
            bool d_expMovedInto;
            bool d_expMovedFrom;
            bool d_expUnknown;
        } DATA[] = {
            //            :  hasUnknownCopyMoveState ---------.
            //            :  isMovedFrom ------------------.  |
            //            :  isMovedInto ---------------.  |  |
            //            :  isNonconstCopiedInto ---.  |  |  |
            //  Exp return:  isConstCopiedInto ---.  |  |  |  |
            //            :  isCopiedInto -----.  |  |  |  |  |
            //            :  isOriginal ----.  |  |  |  |  |  |
            //                              |  |  |  |  |  |  |
            // enumerator value             V  V  V  V  V  V  V
            // ---------------------------  -  -  -  -  -  -  -
            {  CMS::e_ORIGINAL            , T, F, F, F, F, F, F },
            {  CMS::e_COPIED_INTO         , F, T, F, F, F, F, F },
            {  CMS::e_COPIED_CONST_INTO   , F, T, T, F, F, F, F },
            {  CMS::e_COPIED_NONCONST_INTO, F, T, F, T, F, F, F },
            {  CMS::e_MOVED_INTO          , F, F, F, F, T, F, F },
            {  CMS::e_MOVED_FROM          , T, F, F, F, F, T, F },
            {  CMS::e_UNKNOWN             , F, F, F, F, F, F, T }
        };

        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

#define TEST(E, EXP, QUERY) do {                                \
            ASSERTV(E, EXP, (EXP) == Cmt.QUERY());              \
            ASSERTV(E, EXP, Cmt.QUERY() == CMS::QUERY(Cmt));    \
        } while (false);

        for (std::size_t i = 0; i < NUM_DATA; ++i) {
            const Enum E                 = DATA[i].d_state;
            const bool expOriginal       = DATA[i].d_expOriginal;
            const bool expCopied         = DATA[i].d_expCopiedInto;
            const bool expCopiedConst    = DATA[i].d_expCopiedConstInto;
            const bool expCopiedNonconst = DATA[i].d_expCopiedNonconstInto;
            const bool expMovedInto      = DATA[i].d_expMovedInto;
            const bool expMovedFrom      = DATA[i].d_expMovedFrom;
            const bool expUnknown        = DATA[i].d_expUnknown;

            Obj mCmt; const Obj& Cmt = mCmt;

            // Step 1 & 3
            mCmt.setCopyMoveState(E);
            TEST(E, expOriginal      , isOriginal);
            TEST(E, expCopied        , isCopiedInto);
            TEST(E, expCopiedConst   , isCopiedConstInto);
            TEST(E, expCopiedNonconst, isCopiedNonconstInto);
            TEST(E, expMovedInto     , isMovedInto);
            TEST(E, expMovedFrom     , isMovedFrom);
            ASSERTV(E, expUnknown == Cmt.hasUnknownCopyMoveState());
            ASSERTV(E, Cmt.hasUnknownCopyMoveState() == CMS::isUnknown(Cmt));

            if (E == CMS::e_UNKNOWN) {
                continue;
            }

            // Step 2 & 3
            const Enum mfE = Enum(E | CMS::e_MOVED_FROM);
            mCmt.setCopyMoveState(mfE);
            TEST(mfE, expOriginal      , isOriginal);
            TEST(mfE, expCopied        , isCopiedInto);
            TEST(mfE, expCopiedConst   , isCopiedConstInto);
            TEST(mfE, expCopiedNonconst, isCopiedNonconstInto);
            TEST(mfE, expMovedInto     , isMovedInto);
            TEST(mfE, ! expUnknown     , isMovedFrom);
            ASSERTV(mfE, expUnknown == Cmt.hasUnknownCopyMoveState());
            ASSERTV(mfE, Cmt.hasUnknownCopyMoveState() == CMS::isUnknown(Cmt));

#undef TEST

        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC MANIPULATORS AND ACCESSORS
        //    A 'CopyMoveTracker' can be put into any valid state using the
        //    single manipulator, 'setCopyMoveState'.  The entire state can be
        //    read out using the single accessor, 'copyMoveState'.
        //
        // Concerns:
        //: 1 A call to 'setCopyMoveState(x)' sets the state of a
        //:   'CopyMoveTracker' object to 'x', for valid values of 'x' in the
        //:   range 'e_ORIGINAL' to 'e_MAX_ENUM', inclusive.
        //: 2 A call to 'setCopyMoveState(x)' with an invalid value for 'x'
        //:   triggers a defensive check error (in non-optimized builds).
        //
        // Plan:
        //: 1 Create a variable of type 'CopyMoveTracker' (the constructor is
        //:   not being tested here, other than that it is invocable).
        //: 2 For each valid bit pattern, 'e' in the range of
        //:   'CopyMoveState::Enum', call 'setCopyMoveState(e)' on the object
        //:   from step 1.  Using the 'copyMoveState' accessor, verify that the
        //:   object state was set as specified.  (C-1)
        //: 3 For each invalid bit pattern, 'e', in the range of
        //:   'CopyMoveState::Enum', call 'setCopyMoveState(e)' on the object.
        //:   Using the 'ASSERT_FAIL' macro, verify that the call would trigger
        //:   an assert failure in the appropriate build mode.  (C-2)
        //
        // Testing:
        //      void setCopyMoveState(CopyMoveState::Enum state);
        //      CopyMoveState::Enum copyMoveState() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC MANIPULATORS AND ACCESSORS"
                            "\n================================\n");

        Obj cmt;

        for (std::size_t i = 0; i < CMS::e_MAX_ENUM; ++i) {
            const Enum E = Enum(i);

            bsls::AssertTestHandlerGuard g;

            if (CMS::isValid(E)) {
                ASSERT_PASS(cmt.setCopyMoveState(E));
                ASSERTV(E, E == cmt.copyMoveState());
            }
            else {
                ASSERT_FAIL(cmt.setCopyMoveState(E));
            }
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj v1; const Obj& V1 = v1;
        ASSERT(v1.isOriginal());

        Obj v2(V1);                       // copy construct
        ASSERT(v1.isOriginal());          // Unchanged
        ASSERT(v2.isCopiedInto());
        ASSERT(v2.isCopiedConstInto());
        ASSERT(! v2.isCopiedNonconstInto());

        Obj v3(v1);                       // non-const copy construct
        ASSERT(v1.isOriginal());          // Unchanged
        ASSERT(v3.isCopiedInto());
        ASSERT(! v3.isCopiedConstInto());
        ASSERT(v3.isCopiedNonconstInto());

        v3 = bslmf::MovableRefUtil::move(v2);  // Move assignment
        ASSERT(v2.isCopiedInto());             // unchanged
        ASSERT(v2.isMovedFrom());              // moved-from after copied into
        ASSERT(v3.isMovedInto());

        ASSERT(v1 == v2);  // always compare equal

        // Test ADL-findable 'copyMoveState'.
        ASSERTV(CMS::get(v2),
                CMS::get(v2) == (CMS::e_COPIED_CONST_INTO|CMS::e_MOVED_FROM));
        ASSERT(CMS::isMovedInto(v3));

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
