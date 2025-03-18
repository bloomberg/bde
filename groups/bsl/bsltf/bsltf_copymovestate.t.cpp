// bsltf_copymovestate.t.cpp                                          -*-C++-*-
#include <bsltf_copymovestate.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_bsltestutil.h>

#include <cstdio>   // printf
#include <cstdlib>  // atoi
#include <cstring>  // strlen, strcmp, strncmp

using namespace BloombergLP;
using std::printf;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a single enumeration having sequential
// bit-pattern enumnerators.  It provides static methods for converting to
// string and for calling ADL customization points that simplify its expected
// usage in test drivers.  It also provides equality comparison operators that
// operate on a mix of the enumeration and integers.
// ----------------------------------------------------------------------------
// TYPES
// [ 2] enum Enum { ... };
//
// CLASS METHODS
// [ 5] Enum get(const TYPE& obj);
// [ 7] bool isCopiedConstInto(const TYPE& v);
// [ 7] bool isCopiedInto(const TYPE& v);
// [ 7] bool isCopiedNonconstInto(const TYPE& v);
// [ 7] bool isMovedFrom(const TYPE& v);
// [ 7] bool isMovedInto(const TYPE& v);
// [ 7] bool isOriginal(const TYPE& v);
// [ 7] bool isUnknown(const TYPE& v);
// [ 3] bool isValid(Enum val);
// [ 6] void set(TYPE *obj_p, Enum state);
// [ 4] const char *toAscii(CopyMoveState::Enum val);
//
// FREE FUNCTIONS
// [ 8] bool operator==(CopyMoveState::Enum a, int b);
// [ 8] bool operator==(int a, CopyMoveState::Enum b);
// [ 8] bool operator!=(CopyMoveState::Enum a, int b);
// [ 8] bool operator!=(int a, CopyMoveState::Enum b);
// [ 9] bool operator!(CopyMoveState::Enum value);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
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
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number


// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsltf::CopyMoveState       Util;
typedef bsltf::CopyMoveState::Enum Enum;

// Namespace `xyz` for ADL testing
namespace xyz {

// Address of last argument to `xyz::copyMoveState` found via ADL
const void *lastAdl_p = 0;

/// `struct` having a `copyMoveState` customization point
struct Test1 {
    Enum d_value;

    explicit Test1(Enum e = Util::e_UNKNOWN) : d_value(e) { }
};

inline
Enum copyMoveState(const Test1& x)
{
    lastAdl_p = &x;
    return x.d_value;
}

inline
void setCopyMoveState(Test1 *obj_p, Enum state)
{
    lastAdl_p = obj_p;
    obj_p->d_value = state;
}

/// `struct` inheriting a `copyMoveState` customization point
struct Test2 : Test1 {
    explicit Test2(Enum e = Util::e_UNKNOWN) : Test1(e) { }
};

/// `struct` not having a `copyMoveState` customization point
struct Test3 {
    Enum d_value;
    explicit Test3(Enum e = Util::e_UNKNOWN) : d_value(e) { }
};

/// `struct` template having a `copyMoveState` customization point
template <unsigned ID>
struct TemplateTest1 {
    Enum d_value;

    explicit TemplateTest1(Enum e = Util::e_UNKNOWN) : d_value(e) { }

    friend Enum copyMoveState(const TemplateTest1& x)
    {
        lastAdl_p = &x;
        return x.d_value;
    }

    friend void setCopyMoveState(TemplateTest1 *obj_p, Enum state)
    {
        lastAdl_p = obj_p;
        obj_p->d_value = state;
    }
};

/// `struct` template inheriting a `copyMoveState` customization point
template <unsigned ID>
struct TemplateTest2 : TemplateTest1<ID> {
    explicit TemplateTest2(Enum e = Util::e_UNKNOWN) : TemplateTest1<ID>(e) { }
};

/// `struct` template not having a `copyMoveState` customization point
template <unsigned ID>
struct TemplateTest3 {
    Enum d_value;
    explicit TemplateTest3(Enum e = Util::e_UNKNOWN) : d_value(e) { }
};

}  // close namespace xyz

// ============================================================================
//                       GLOBAL CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const char *const INVALID_FORMAT = "(* INVALID *)";

const Enum ENUM_LIST[] = {
    Util::e_ORIGINAL            ,
    Util::e_COPIED_INTO         ,
    Util::e_COPIED_CONST_INTO   ,
    Util::e_COPIED_NONCONST_INTO,
    Util::e_MOVED_INTO          ,
    Util::e_MOVED_FROM          ,
    Util::e_UNKNOWN
};

const std::size_t NUM_ENUMS = sizeof(ENUM_LIST) / sizeof(Enum);

const Enum VALID_STATE_LIST[] = {
    Util::e_ORIGINAL            ,
    Util::e_COPIED_INTO         ,
    Util::e_COPIED_CONST_INTO   ,
    Util::e_COPIED_NONCONST_INTO,
    Util::e_MOVED_INTO          ,
    Util::e_MOVED_FROM          ,

    Enum(Util::e_COPIED_INTO          | Util::e_MOVED_FROM),
    Enum(Util::e_COPIED_CONST_INTO    | Util::e_MOVED_FROM),
    Enum(Util::e_COPIED_NONCONST_INTO | Util::e_MOVED_FROM),
    Enum(Util::e_MOVED_INTO           | Util::e_MOVED_FROM),

    Util::e_UNKNOWN
};

/// Return `true` if the specified `e` enumerator has a name starting with
/// `e_COPIED`.
bool isCopiedEnum(Enum e)
{
    return (e == Util::e_COPIED_INTO          ||
            e == Util::e_COPIED_CONST_INTO    ||
            e == Util::e_COPIED_NONCONST_INTO);
}

// ============================================================================
//                       USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tracking an object's history
///- - - - - - - - - - - - - - - - - - - -
// In this example, we show how `CopyMoveState` can be used to track the most
// recent copy or move operation applied to an object.
//
// First, we define a `TrackedValue` type that contains an integer value and a
// `CopyMoveState`
// ```
//  #include <bsltf_copymovestate.h>
//  #include <bslmf_movableref.h>
//  #include <cstring>

    struct TrackedValue {
        int                        d_value;
        bsltf::CopyMoveState::Enum d_copyMoveState;
// ```
// Next, we define a value constructor that indicates that the object was
// neither copied into, moved into, nor moved from:
// ```
        TrackedValue(int v = 0)                                     // IMPLICIT
            : d_value(v)
            , d_copyMoveState(bsltf::CopyMoveState::e_ORIGINAL) { }
// ```
// Next, we define a copy constructor that records the fact that the object was
// copied into.  As in the case of most copy constructors, the original is
// accessed through a `const` reference, and the copy/moved state reflects that
// fact.
// ```
        TrackedValue(const TrackedValue& original)
            : d_value(original.d_value)
            , d_copyMoveState(bsltf::CopyMoveState::e_COPIED_CONST_INTO) { }
// ```
// Next, we define a move constructor that records both that the object being
// constructed was moved into, but also that the original object was moved
// from.
// ```
        TrackedValue(bslmf::MovableRef<TrackedValue> original)
            : d_value(bslmf::MovableRefUtil::access(original).d_value)
            , d_copyMoveState(bsltf::CopyMoveState::e_MOVED_INTO)
        {
            TrackedValue& originalRef   = original;
            originalRef.d_value         = -1;
            originalRef.d_copyMoveState = bsltf::CopyMoveState::e_MOVED_FROM;
        }
// ```
// Next, we declare the destructor and assignment operators, but, for the
// purpose of this example, we don't need to see their implementations:
// ```
        //! ~TrackedValue() = default;

        TrackedValue& operator=(const TrackedValue&);
        TrackedValue& operator=(bslmf::MovableRef<TrackedValue>);
// ```
// Then, we define accessors for the value and copy/move state.
// ```
        int value() const { return d_value; }
        bsltf::CopyMoveState::Enum copyMoveState() const
            { return d_copyMoveState; }
// ```
// Now, outside of the class definition or (better yet) as a hidden friend
// function, we define an ADL customization point used to retrieve the
// copy/move state of the tracked value.  This free function will be called in
// generic code that doesn't know whether the type it's working with has a
// `copyMoveState` accessor; a default is provided for types that don't:
// ```
        friend bsltf::CopyMoveState::Enum copyMoveState(const TrackedValue& v)
        {
            return v.copyMoveState();
        }
    };
// ```
// Finally, we test our `TrackedValue` class by creating an object, copying it,
// and moving it.  After each step, we test that each object's copy/move state
// is as expected.  Note that `e_COPIED_INTO` names a bit that is set in both
// of the other `e_COPIED_*` enumerators.  At the end, we verify that the
// copy/move state is correctly converted to a string by the `toAscii` method
// and that the copy/move state of a plain `int` is always "e_UNKNOWN".
// ```
    void usageExample1()
    {
        typedef bsltf::CopyMoveState Cms;

        TrackedValue tv1(42);
        ASSERT(42                         == tv1.value());
        ASSERT(Cms::e_ORIGINAL            == tv1.copyMoveState());
        ASSERT(Cms::get(tv1)              == tv1.copyMoveState());

        TrackedValue tv2(tv1);  // Copy
        ASSERT(42                         == tv2.value());
        ASSERT(Cms::e_COPIED_CONST_INTO   == tv2.copyMoveState());
        ASSERT(Cms::e_COPIED_INTO          & tv2.copyMoveState());

        TrackedValue tv3(bslmf::MovableRefUtil::move(tv1));  // Move
        ASSERT(42                         == tv3.value());
        ASSERT(Cms::e_MOVED_INTO          == tv3.copyMoveState());
        ASSERT(-1                         == tv1.value());
        ASSERT(Cms::e_MOVED_FROM          == tv1.copyMoveState());

        ASSERT(0 == std::strcmp("MOVED_FROM",
                                Cms::toAscii(tv1.copyMoveState())));

        ASSERT(Cms::e_UNKNOWN == Cms::get(5));  // `int` doesn't track state
    }
// ```

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? std::atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, recopy
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");


        usageExample1();

      } break;
      case 9: {
        // -------------------------------------------------------------------
        // TESTING `operator!`
        //
        // Concerns:
        // 1. `operator!` returns `true` for `e_ORIGINAL` and `false` for all
        //    other enumerators in `CopyMoveState`.
        //
        // Plan:
        // 1. For each enumerator `e`, verify that `! e` yields the expected
        //    value.  (C-1)
        //
        // Testing:
        //     bool operator!(CopyMoveState::Enum value);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `operator!`"
                            "\n===================\n");

        ASSERT(true == ! Util::e_ORIGINAL);

        for (std::size_t i = 1; i < NUM_ENUMS; ++i) {
            const Enum e = ENUM_LIST[i];
            ASSERTV(e, false == !e);
        }

      } break;
      case 8: {
        // -------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //     Verify that an `Enum` value can be campared to an `int`.
        //
        // Concerns:
        // 1. An `Enum` value and an `int` can be compared using `operator==`
        //    and `operater!=`; the resulting `bool` value is the same as if
        //    both were converted to `int`.
        // 2. The previous concern applies even for bit patterns that don't
        //    represent a specific enumerator (e.g., if it is the bitwise OR of
        //    two or more enumerators).
        //
        // Plan:
        // 1. Use a pair fo nested loops over every integer value from `0` to
        //    `e_MAX_ENUM` to generate a set of integer pairs.  Cast one of the
        //    integers to `Enum`, then compare to the other using `operator==`
        //    and `operator!=`.  Verify that the comparison produces the
        //    expected result. (C-1, C-2)
        //
        // Testing:
        //     bool operator==(CopyMoveState::Enum a, int b);
        //     bool operator==(int a, CopyMoveState::Enum b);
        //     bool operator!=(CopyMoveState::Enum a, int b);
        //     bool operator!=(int a, CopyMoveState::Enum b);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATORS"
                            "\n==========================\n");

        for (int i = 0; i <= Util::e_MAX_ENUM; ++i) {
            const Enum e1 = Enum(i);

            for (int j = 0; j <= Util::e_MAX_ENUM; ++j) {
                const Enum e2 = Enum(j);

                ASSERTV(i, j, (i == j) == (i  == e2));
                ASSERTV(i, j, (i == j) == (e1 == j ));
                ASSERTV(i, j, (i != j) == (i  != e2));
                ASSERTV(i, j, (i != j) == (e1 != j ));
            }
        }

      } break;
      case 7: {
        // -------------------------------------------------------------------
        // TESTING `is*` CLASS METHODS
        //   Verify operation of the `isCopiedInto`, `isMovedInto` and similar
        //   class methods that check specific bits in an object's copy/move
        //   state.
        //
        // Concerns:
        // 1. When called on an object that does not provide a copy/move state,
        //    all of the `is*` queries return `false` except `isUnknown`.
        // 2. When called on an object returning a copy/move state that
        //    includes neither `e_COPIED_INTO` nor the `e_MOVED_INTO` bit,
        //    `isOriginal` returns `true` and all of the `is*Into` queries
        //    return `false`.
        // 3. Each query other than `isOriginal` returns `true` if the
        //    copy/move state contains the bits from the corresponding
        //    enumerator.
        // 4. Setting the `e_MOVED_FROM` bit in the copy/move state causes
        //    `isMovedFrom` to return `true` (unless the `e_UNKNOWN` bit is
        //    also set) and does not change the result from any of the other
        //    queries.
        // 5. When called on an object returning a copy/move state for which
        //    the `e_UNKNOWN` bit is set, all of the other `is*` queries return
        //    `false`, regardless of the value of the remaining bits.  This
        //    concern overrides concerns 2-4.
        //
        // Plan:
        // 1. Call each query on an object that does not return a copy/move
        //    state and verify that `isUnknown` returns `true` and all the rest
        //    return `false`.  (C-1)
        // 2. Use a table-driven approach to verify that each query produces
        //    the expected value for each enumerator.  (C-2, C-3)
        // 3. Verify that `isMovedFrom` returns `false` when called on each
        //    object from step 3.  Then set the `e_MOVED_FROM` bit in the
        //    object's state, without modifying any other bits, and verify that
        //    `isMovedFrom` now returns `true`.  (C-4)
        // 4. Set the `e_UNKNOWN` bit in the move/copy state for each object in
        //    step 2, without modifying any other bit, and verify that all of
        //    the queries now return `false`. (C-5)
        //
        // Testing:
        //     bool isCopiedConstInto(const TYPE& v);
        //     bool isCopiedInto(const TYPE& v);
        //     bool isCopiedNonconstInto(const TYPE& v);
        //     bool isMovedFrom(const TYPE& v);
        //     bool isMovedInto(const TYPE& v);
        //     bool isOriginal(const TYPE& v);
        //     bool isUnknown(const TYPE& v);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `is*` CLASS METHODS"
                            "\n===========================\n");

        // Step 1
        int untracked = 0;
        ASSERT(! Util::isOriginal(untracked));
        ASSERT(! Util::isCopiedInto(untracked));
        ASSERT(! Util::isCopiedConstInto(untracked));
        ASSERT(! Util::isCopiedNonconstInto(untracked));
        ASSERT(! Util::isMovedInto(untracked));
        ASSERT(! Util::isMovedFrom(untracked));
        ASSERT(  Util::isUnknown(untracked));

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
            //             :  isUnknown -----------------------.
            //             :  isMovedFrom ------------------.  |
            //             :  isMovedInto ---------------.  |  |
            //             :  isNonconstCopiedInto ---.  |  |  |
            //   Exp return:  isConstCopiedInto ---.  |  |  |  |
            //             :  isCopiedInto -----.  |  |  |  |  |
            //             :  isOriginal ----.  |  |  |  |  |  |
            //                               |  |  |  |  |  |  |
            // enumerator value              V  V  V  V  V  V  V
            // ----------------------------  -  -  -  -  -  -  -
            {  Util::e_ORIGINAL            , T, F, F, F, F, F, F },
            {  Util::e_COPIED_INTO         , F, T, F, F, F, F, F },
            {  Util::e_COPIED_CONST_INTO   , F, T, T, F, F, F, F },
            {  Util::e_COPIED_NONCONST_INTO, F, T, F, T, F, F, F },
            {  Util::e_MOVED_INTO          , F, F, F, F, T, F, F },
            {  Util::e_MOVED_FROM          , T, F, F, F, F, T, F },
            {  Util::e_UNKNOWN             , F, F, F, F, F, F, T }
        };

        const std::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (std::size_t i = 0; i < NUM_DATA; ++i) {
            const Enum E                 = DATA[i].d_state;
            const bool expOriginal       = DATA[i].d_expOriginal;
            const bool expCopied         = DATA[i].d_expCopiedInto;
            const bool expCopiedConst    = DATA[i].d_expCopiedConstInto;
            const bool expCopiedNonconst = DATA[i].d_expCopiedNonconstInto;
            const bool expMovedInto      = DATA[i].d_expMovedInto;
            const bool expMovedFrom      = DATA[i].d_expMovedFrom;
            const bool expUnknown        = DATA[i].d_expUnknown;

            // Step 2
            xyz::Test1 obj(E);
            ASSERTV(E, expOriginal       == Util::isOriginal(obj));
            ASSERTV(E, expCopied         == Util::isCopiedInto(obj));
            ASSERTV(E, expCopiedConst    == Util::isCopiedConstInto(obj));
            ASSERTV(E, expCopiedNonconst == Util::isCopiedNonconstInto(obj));
            ASSERTV(E, expMovedInto      == Util::isMovedInto(obj));
            ASSERTV(E, expMovedFrom      == Util::isMovedFrom(obj));
            ASSERTV(E, expUnknown        == Util::isUnknown(obj));

            // Step 3
            const Enum mfE = Enum(E | Util::e_MOVED_FROM);
            obj.d_value = mfE;
            ASSERTV(mfE, expOriginal       == Util::isOriginal(obj));
            ASSERTV(mfE, expCopied         == Util::isCopiedInto(obj));
            ASSERTV(mfE, expCopiedConst    == Util::isCopiedConstInto(obj));
            ASSERTV(mfE, expCopiedNonconst == Util::isCopiedNonconstInto(obj));
            ASSERTV(mfE, expMovedInto      == Util::isMovedInto(obj));
            ASSERTV(mfE, (! expUnknown)    == Util::isMovedFrom(obj));
            ASSERTV(mfE, expUnknown        == Util::isUnknown(obj));

            // Step 4
            const Enum ukE = Enum(E | Util::e_UNKNOWN);
            obj.d_value = ukE;
            ASSERTV(ukE, ! Util::isOriginal(obj));
            ASSERTV(ukE, ! Util::isCopiedInto(obj));
            ASSERTV(ukE, ! Util::isCopiedConstInto(obj));
            ASSERTV(ukE, ! Util::isCopiedNonconstInto(obj));
            ASSERTV(ukE, ! Util::isMovedInto(obj));
            ASSERTV(ukE, ! Util::isMovedFrom(obj));
            ASSERTV(ukE,   Util::isUnknown(obj));
        }

      } break;
      case 6: {
        // -------------------------------------------------------------------
        // TESTING `set`
        //   Verify that the static `CopyMoveState::set` function template
        //   calls the ADL `setCopyMoveState` customization function for its
        //   argument, or does nothing if such a customization function does
        //   not exist.
        //
        // Concerns:
        // 1. A call to `CopyMoveState::set(x, state))` returns
        //    `setCopyMoveState(x, state))` if such a call is found by ADL.
        // 2. Concern 1 also applies to objects of class derived from classes
        //    providing the `setCopyMoveState` ADL customization.
        // 3. A call to `CopyMoveState::set(x, state))` has no effect if
        //    `setCopyMoveState(x, state))` not found by ADL.
        // 4. The above concerns apply whether or not the type of `x` is a
        //    template specialization.
        //
        // Plan:
        // 1. Create a class `xyz::Test1` and a function
        //    `xyz::setCopyMoveState(Test1 *, state))` that tracks its most
        //    recent call.  Create an object `x` of type `xyz::Test1` and
        //    verify that `CopyMoveState::set(&x, state))` calls
        //    `xyz::setCopyMoveState(&x, state))`.  (C-1)
        // 2. Create a class `xyz::Test2` inheriting from `xyz::Test1` but not
        //    providing its own `setCopyMoveState(Test2 *, state))` free
        //    function.  Create an object `y` of type `xyz::Test2` and verify
        //    that the `CopyMoveState::set(&y, state))` has the same effect as
        //    for `Test1`.  (C-2)
        // 3. Create a class `xyz::Test3` having no `setCopyMoveState`
        //    overload.  Create an object `z` of type `xyz::Test3` and verify
        //    that `CopyMoveState::set(&z, state))` has no effect.  Also verify
        //    that, for `int i`, `CopyMoveState::set(&i, state)` has no effect.
        //    (C-3)
        // 4. Repeat the steps above with class templates instead of `Test1`,
        //    `Test2`, and `Test3`.  (C-4)
        //
        // Testing:
        //     void set(TYPE *obj_p, Enum state);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `set`"
                            "\n=============\n");

        xyz::Test1 x;
        bsltf::CopyMoveState::set(&x, Util::e_COPIED_INTO);
        ASSERT(Util::e_COPIED_INTO == x.d_value);
        ASSERT(&x == xyz::lastAdl_p);

        xyz::Test2 y;
        bsltf::CopyMoveState::set(&y, Util::e_COPIED_CONST_INTO);
        ASSERT(Util::e_COPIED_CONST_INTO == y.d_value);
        ASSERT(&y == xyz::lastAdl_p);

        xyz::Test3 z;
        bsltf::CopyMoveState::set(&z, Util::e_MOVED_INTO);
        ASSERT(Util::e_UNKNOWN == z.d_value);
        ASSERT(&y == xyz::lastAdl_p);  // unchanged

        int i = 5;
        bsltf::CopyMoveState::set(&i, Util::e_MOVED_FROM);
        ASSERT(5 == i);
        ASSERT(&y == xyz::lastAdl_p);  // unchanged

        xyz::TemplateTest1<1> tx;
        bsltf::CopyMoveState::set(&tx, Util::e_COPIED_INTO);
        ASSERT(Util::e_COPIED_INTO == tx.d_value);
        ASSERT(&tx == xyz::lastAdl_p);

        xyz::TemplateTest2<1> ty;
        bsltf::CopyMoveState::set(&ty, Util::e_COPIED_NONCONST_INTO);
        ASSERT(Util::e_COPIED_NONCONST_INTO == ty.d_value);
        ASSERT(&ty == xyz::lastAdl_p);

        xyz::TemplateTest3<2> tz;
        bsltf::CopyMoveState::set(&tz, Util::e_MOVED_INTO);
        ASSERT(Util::e_UNKNOWN == tz.d_value);
        ASSERT(&ty == xyz::lastAdl_p);  // unchanged

      } break;
      case 5: {
        // -------------------------------------------------------------------
        // TESTING `get`
        //   Verify that the static `CopyMoveState::get` function template
        //   calls the ADL `copyMoveState` customization function for its
        //   argument, or returns `e_UNKNOWN` if such a customization function
        //   does not exist.
        //
        // Concerns:
        // 1. A call to `CopyMoveState::get(x)` returns `copyMoveState(x)` if
        //    such a call is found by ADL.
        // 2. Concern 1 also applies to objects of class derived from classes
        //    providing the `copyMoveState` ADL customization.
        // 3. A call to `CopyMoveState::get(x)` returns `e_UNKNOWN` if
        //    `copyMoveState(x)` not found by ADL.
        // 4. The above concerns apply whether or not the type of `x` is a
        //    template specialization.
        //
        // Plan:
        // 1. Create a class `xyz::Test1` and a function
        //    `xyz::copyMoveState(Test1)` that tracks its most recent call.
        //    Create an object `x` of type `xyz::Test1` and verify that
        //    `CopyMoveState::get(x)` calls `xyz::copyMoveState(x)`.  (C-1)
        // 2. Create a class `xyz::Test2` inheriting from `xyz::Test1` but not
        //    providing its own `xyz::copyMoveState(Test2)` free function.
        //    Create an object `y` of type `xyz::Test2` and verify that the
        //    `CopyMoveState::get(y)` has the same result as for `Test1`.
        //    (C-2)
        // 3. Create a class `xyz::Test3` having no `copyMoveState` overload.
        //    Create an object `z` of type `xyz::Test3` and verify that
        //    `CopyMoveState::get(z)` returns `e_UNKNOWN`.  Also verify that,
        //    for `int i`, `CopyMoveState::get(i)` returns `e_UNKNOWN`.  (C-3)
        // 4. Repeat the steps above with class templates instead of `Test1`
        //    and `Test3`.  (C-4)
        //
        // Testing:
        //     Enum get(const TYPE& obj);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `get`"
                            "\n=============\n");

        xyz::Test1 x(Util::e_COPIED_INTO);
        Enum e = bsltf::CopyMoveState::get(x);
        ASSERT(Util::e_COPIED_INTO == e);
        ASSERT(&x == xyz::lastAdl_p);

        xyz::Test2 y(Util::e_COPIED_CONST_INTO);
        e = bsltf::CopyMoveState::get(y);
        ASSERT(Util::e_COPIED_CONST_INTO == e);
        ASSERT(&y == xyz::lastAdl_p);

        xyz::Test3 z(Util::e_MOVED_INTO);
        e = bsltf::CopyMoveState::get(z);
        ASSERT(Util::e_UNKNOWN == e);
        ASSERT(&y == xyz::lastAdl_p);  // unchanged

        int i = 5;
        e = bsltf::CopyMoveState::get(i);
        ASSERT(Util::e_UNKNOWN == e);
        ASSERT(&y == xyz::lastAdl_p);  // unchanged

        xyz::TemplateTest1<1> tx(Util::e_COPIED_INTO);
        e = bsltf::CopyMoveState::get(tx);
        ASSERT(Util::e_COPIED_INTO == e);
        ASSERT(&tx == xyz::lastAdl_p);

        xyz::TemplateTest2<1> ty(Util::e_COPIED_INTO);
        e = bsltf::CopyMoveState::get(ty);
        ASSERT(Util::e_COPIED_INTO == e);
        ASSERT(&ty == xyz::lastAdl_p);

        xyz::TemplateTest3<2> tz(Util::e_MOVED_INTO);
        e = bsltf::CopyMoveState::get(tz);
        ASSERT(Util::e_UNKNOWN == e);
        ASSERT(&ty == xyz::lastAdl_p);  // unchanged

      } break;
      case 4: {
        // -------------------------------------------------------------------
        // TESTING `toAscii`
        //   Verify that the static `toAscii` method returns an appropriate
        //   static null-terminated ASCII string for each possible `Enum`
        //   value.
        //
        // Concerns:
        // 1. The `toAscii` method returns a string containing the name of the
        //    enumerator without the leading "e_".
        // 2. For each enumerator that indicates a copied-into or moved-into
        //    state, passing the bitwise OR of that enumerator and
        //    `e_MOVED_FROM` to `toAscii` returns a string containing both the
        //    enumerator name and `MOVED_FROM`, separated by a comma.
        // 3. The `toAscii` method returns a distinguished string when passed
        //    an out-of-band value.
        // 4. The string returned by `toAscii` is not modifiable.
        // 5. The `toAscii` method has the expected signature.
        //
        // Plan:
        // 1. Verify that the `toAscii` method returns the expected string
        //    representation for each individual enumerator.  (C-1)
        // 2. Supply `toAscii` with the logical OR of each appropriate
        //    enumerator and `e_MOVED_FROM`.  Verify that the returned string
        //    has the expected value.  (C-2)
        // 3. Verify that the `toAscii` method returns a distinguished string
        //    when passed an out-of-band value.  (C-3)
        // 4. Take the address of the `toAscii` (class) method and use the
        //    result to initialize a variable of the appropriate type.
        //    (C-4, C-5)
        //
        // Testing:
        //     const char *toAscii(CopyMoveState::Enum val);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `toAscii`"
                            "\n=================\n");

        static const struct {
            int         d_lineNum;  // source line number
            Enum        d_value;    // enumerator value
            const char *d_exp;      // expected result
        } DATA[] = {
            // line  enumerator value              expected result
            // ----  ----------------------------  -----------------
            {  L_,   Util::e_ORIGINAL            , "ORIGINAL"  },
            {  L_,   Util::e_COPIED_INTO         , "COPIED_INTO"          },
            {  L_,   Util::e_COPIED_CONST_INTO   , "COPIED_CONST_INTO"    },
            {  L_,   Util::e_COPIED_NONCONST_INTO, "COPIED_NONCONST_INTO" },
            {  L_,   Util::e_MOVED_INTO          , "MOVED_INTO"           },
            {  L_,   Util::e_MOVED_FROM          , "MOVED_FROM"           },
            {  L_,   Util::e_UNKNOWN             , "UNKNOWN"              },

            {  L_,   Enum(Util::e_UNKNOWN - 1)   , INVALID_FORMAT         },
            {  L_,   Enum(Util::e_UNKNOWN + 1)   , INVALID_FORMAT         }
        };

        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_lineNum;
            const Enum        VALUE   = DATA[ti].d_value;
            const char *const EXP     = DATA[ti].d_exp;
            const std::size_t EXP_LEN = std::strlen(EXP);

            const char *result = Util::toAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            // C-1, C-3
            ASSERTV(LINE, EXP, result, 0 == std::strcmp(EXP, result));

            // C-2
            if (VALUE > Util::e_ORIGINAL && VALUE < Util::e_MOVED_FROM) {
                const Enum MOVED_FROM_VALUE = Enum(VALUE | Util::e_MOVED_FROM);
                result = Util::toAscii(MOVED_FROM_VALUE);
                ASSERTV(LINE, EXP, result,
                        0 == std::strncmp(EXP, result, EXP_LEN));
                ASSERTV(LINE, EXP, result, 0 == std::strcmp(", MOVED_FROM",
                                                            result + EXP_LEN));
            }
        }

        if (verbose) printf("\nVerify `toAscii` signature.\n");
        {
            typedef const char *(*FuncPtr)(Enum);

            const FuncPtr FP = &Util::toAscii;  (void)FP;  // C-4, C-5
        }

      } break;
      case 3: {
        // -------------------------------------------------------------------
        // TESTING `isValid`
        //
        // Concerns:
        // 1. `CopyMoveState::isValid(x)` returns `true` if `x` is a valid
        //    value of `CopyMoveState::Enum` and `false` otherwise.
        //
        // Plan:
        // 1. For every value of `x` between `e_ORIGINAL` and `e_UNKNOWN + 1`,
        //    verify that `isValid(x)` returns the expected value.
        //
        // TESTING
        //    bool isValid(Enum val);
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `isValid`"
                            "\n=================\n");

        std::size_t nextValid = 0;  // Index of next valid state

        for (int i = 0; i <= Util::e_UNKNOWN; ++i) {
            const Enum x = Enum(i);

            const bool EXP = (x == VALID_STATE_LIST[nextValid]);
            ASSERTV(i, x, EXP, Util::isValid(x) == EXP);

            if (EXP) {
                ++nextValid;  // Update to refer to next valid state
            }
        }

        const Enum x = Enum(Util::e_UNKNOWN + 1);
        ASSERTV(x, false == Util::isValid(x));

      } break;
      case 2: {
        // -------------------------------------------------------------------
        // TESTING `Enum`
        //    Verify the expected relationships between enumerators in
        //    `CopyMoveState'.
        //
        // Concerns:
        // 1. When converted to `bool`, `e_ORIGINAL` yields `false` and the
        //    remaining enumerators yield `true`.
        // 2. Every enumerator has a distinct value.
        // 3. The enumerators `e_COPIED_INTO`, `e_COPIED_CONST_INTO`, and
        //    `e_COPIED_NONCONST_INTO`, have the `e_COPIED_INTO` bit in
        //    common; otherwise no two enumerators have any bits in common.
        //
        // Plan:
        // 1. Using nested loops, iterate through every pair of enumerators.
        // 2. In the outer loop, verify that only `e_ORIGINAL` has an value of
        //    `0` when cast to `int`.  (C-1)
        // 3. For each pair of distinct enumerators, verify that they have
        //    different values. (C-2)
        // 4. If both of the enumerators have names starting with `e_COPIED`,
        //    verify that the bitwise AND of the two iterators is
        //    `e_COPIED_INTO`; otherwise verify that the bitwise AND is zero.
        //    (C-3)
        //
        // TESTING
        //    Enum
        // -------------------------------------------------------------------

        if (verbose) printf("\nTESTING `Enum`"
                            "\n==============\n");

        for (std::size_t i = 0; i < NUM_ENUMS; ++i) {
            const Enum e1 = ENUM_LIST[i];

            const bool expBool = (e1 != Util::e_ORIGINAL);
            ASSERTV(i, expBool == bool(e1));  // C-1

            for (std::size_t j = i; j < NUM_ENUMS; ++j) {
                const Enum e2 = ENUM_LIST[j];

                ASSERTV(i, j, (i == j) == (e1 == e2));  // C-2

                int expCommonBits = 0;
                if (i == j) {
                    expCommonBits = int(e1);
                }
                else if (isCopiedEnum(e1) && isCopiedEnum(e2)) {
                    expCommonBits = int(Util::e_COPIED_INTO);
                }
                ASSERTV(i, j, expCommonBits == (e1 & e2));  // C-3
            }  // end for (j...)
        }  // end for (i...)
      } break;
      case 1: {
        // -------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Execute each method to verify functionality for simple cases.
        //
        // Testing:
        //      BREATHING TEST
        // -------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        ASSERT(0 == Util::e_ORIGINAL);
        ASSERT(Util::e_COPIED_INTO & Util::e_COPIED_CONST_INTO);
        ASSERT(Util::e_COPIED_INTO & Util::e_COPIED_NONCONST_INTO);

        ASSERT(0 == strcmp("COPIED_CONST_INTO",
                           Util::toAscii(Util::e_COPIED_CONST_INTO)));
        ASSERT(0 == strcmp("COPIED_NONCONST_INTO, MOVED_FROM",
                           Util::toAscii(Enum(Util::e_COPIED_NONCONST_INTO |
                                              Util::e_MOVED_FROM))));

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
