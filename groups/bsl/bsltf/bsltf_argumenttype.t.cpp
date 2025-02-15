// bsltf_argumenttype.t.cpp                                           -*-C++-*-

#include <bsltf_argumenttype.h>

#include <bsls_platform.h>

// the following suppresses warnings from `#include` inlined functions
#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include <bslma_usesbslmaallocator.h>

#include <bslmf_util.h>             // for usage example only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a single unconstrained (value-semantic)
// attribute class.
//
// This particular attribute class also provides a `set` method capable of
// putting an object into any state relevant for thorough testing, obviating
// the primitive generator function, `gg`, normally used for this purpose.  We
// will therefore follow our standard 10-case approach to testing
// value-semantic types except that we will test the `set` method in case 2 (in
// lieu of the generator function), along with the default constructor and main
// accessors.  Note that copying an object of this class records whether the
// original object is const; thus, we test copy operations with both const and
// non-const arguments.
//
// Objects created with default constructor (having attribute value `-1`) have
// special meaning in the context of the test framework.  Such objects denote
// objects having `valid, but unspecified` state in the context of move
// operations.
//
// Global Concerns:
//  - No memory is every allocated from this component.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] ArgumentType(ArgumentTypeDefault);
// [ 3] ArgumentType(int value);
// [ 7] ArgumentType(ArgumentType      & original);
// [ 7] ArgumentType(ArgumentType const& original);
// [10] ArgumentType(bslmf::MovableRef<ArgumentType> original);
// [14] ~ArgumentType();
// [15] ArgumentTypeByValue(ArgumentTypeDefault);
// [15] ArgumentTypeByValue(ArgumentType<N> const& original);
// [15] ArgumentTypeByValue(ArgumentType<N>      & original);
// [15] ArgumentTypeByValue(bslmf::MovableRef<ArgumentType<N> > original);
//
// MANIPULATORS
// [ 9] ArgumentType& operator=(ArgumentType      & rhs);
// [ 9] ArgumentType& operator=(ArgumentType const& rhs);
// [11] ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs);
// [12] ArgumentType& operator=(int rhs);
// [ 2] void reset();
// [ 2] void set(int value, CopyMoveState::Enum state);
//
// ACCESSORS
// [ 2] operator int() const;
// [ 2] CopyMoveState::Enum copyMoveState() const;
// [ 4] MoveState::Enum movedFrom() const;  // deprecated
// [ 4] MoveState::Enum movedInto() const;  // deprecated
//
// PSEUDO ACCESSORS
// [ 4] CopyMoveState::Enum CopyMoveState::get(obj);   // psuedo accessor
// [ 4] bool CopyMoveState::isCopiedConstInto(obj);    // psuedo accessor
// [ 4] bool CopyMoveState::isCopiedInto(obj);         // psuedo accessor
// [ 4] bool CopyMoveState::isCopiedNonconstInto(obj); // psuedo accessor
// [ 4] bool CopyMoveState::isMovedFrom(obj);          // psuedo accessor
// [ 4] bool CopyMoveState::isMovedInto(obj);          // psuedo accessor
// [ 4] bool CopyMoveState::isOriginal(obj);           // psuedo accessor
//
// FREE FUNCTIONS
// [ 6] bool operator==   // implicitly available via `int` conversion
// [ 6] bool operator!=   // implicitly available via `int` conversion
//
// TYPE TRAITS
// [13] bslma::UsesBslmaAllocator<ArgumentType<N>>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] USAGE EXAMPLE

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bsltf::ArgumentType<1> Obj;

typedef bslmf::MovableRefUtil  MoveUtil;
typedef bsltf::CopyMoveState   CMS;
typedef bsltf::MoveState       MS;         // DEPRECATED

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

int VALUE_DATA[] = { -1, 0, 1, 512, 999, INT_MAX };
const size_t NUM_VALUES = sizeof(VALUE_DATA) / sizeof(VALUE_DATA[0]);

#define MVFROM(state) (CMS::Enum(state | CMS::e_MOVED_FROM))

struct DataRow {
    int       d_line;
    int       d_value;
    CMS::Enum d_cmState;
    MS::Enum  d_expMovedInto;
    MS::Enum  d_expMovedFrom;
};

#define MS_NMV MS::e_NOT_MOVED
#define MS_MV  MS::e_MOVED
#define MS_UKN MS::e_UNKNOWN

const DataRow DATA[] = {
//  Line  Value    Copy/move state                      Mv into Mv from
//  ----  -------  -----------------------------------  ------- -------
    { L_,      -1, CMS::e_ORIGINAL                    , MS_NMV, MS_NMV },
    { L_,       0, CMS::e_ORIGINAL                    , MS_NMV, MS_NMV },
    { L_,       1, CMS::e_ORIGINAL                    , MS_NMV, MS_NMV },
    { L_,     999, CMS::e_ORIGINAL                    , MS_NMV, MS_NMV },
    { L_, INT_MAX, CMS::e_ORIGINAL                    , MS_NMV, MS_NMV },

    { L_,      -1, CMS::e_COPIED_INTO                 , MS_NMV, MS_NMV },
    { L_,       0, CMS::e_COPIED_INTO                 , MS_NMV, MS_NMV },
    { L_,       1, CMS::e_COPIED_INTO                 , MS_NMV, MS_NMV },
    { L_,     999, CMS::e_COPIED_INTO                 , MS_NMV, MS_NMV },
    { L_, INT_MAX, CMS::e_COPIED_INTO                 , MS_NMV, MS_NMV },

    { L_,      -1, CMS::e_COPIED_CONST_INTO           , MS_NMV, MS_NMV },
    { L_,       0, CMS::e_COPIED_CONST_INTO           , MS_NMV, MS_NMV },
    { L_,       1, CMS::e_COPIED_CONST_INTO           , MS_NMV, MS_NMV },
    { L_,     999, CMS::e_COPIED_CONST_INTO           , MS_NMV, MS_NMV },
    { L_, INT_MAX, CMS::e_COPIED_CONST_INTO           , MS_NMV, MS_NMV },

    { L_,      -1, CMS::e_COPIED_NONCONST_INTO        , MS_NMV, MS_NMV },
    { L_,       0, CMS::e_COPIED_NONCONST_INTO        , MS_NMV, MS_NMV },
    { L_,       1, CMS::e_COPIED_NONCONST_INTO        , MS_NMV, MS_NMV },
    { L_,     999, CMS::e_COPIED_NONCONST_INTO        , MS_NMV, MS_NMV },
    { L_, INT_MAX, CMS::e_COPIED_NONCONST_INTO        , MS_NMV, MS_NMV },

    { L_,      -1, CMS::e_MOVED_INTO                  , MS_MV , MS_NMV },
    { L_,       0, CMS::e_MOVED_INTO                  , MS_MV , MS_NMV },
    { L_,       1, CMS::e_MOVED_INTO                  , MS_MV , MS_NMV },
    { L_,     999, CMS::e_MOVED_INTO                  , MS_MV , MS_NMV },
    { L_, INT_MAX, CMS::e_MOVED_INTO                  , MS_MV , MS_NMV },

    { L_,      -1, CMS::e_MOVED_FROM                  , MS_NMV, MS_MV  },

    { L_,      -1, MVFROM(CMS::e_COPIED_INTO         ), MS_NMV, MS_MV  },

    { L_,      -1, MVFROM(CMS::e_COPIED_CONST_INTO   ), MS_NMV, MS_MV  },

    { L_,      -1, MVFROM(CMS::e_COPIED_NONCONST_INTO), MS_NMV, MS_MV  },

    { L_,      -1, MVFROM(CMS::e_MOVED_INTO          ), MS_MV , MS_MV  },

    { L_,      -1, CMS::e_UNKNOWN                     , MS_UKN, MS_UKN },
    { L_,       0, CMS::e_UNKNOWN                     , MS_UKN, MS_UKN },
    { L_,       1, CMS::e_UNKNOWN                     , MS_UKN, MS_UKN },
    { L_,     999, CMS::e_UNKNOWN                     , MS_UKN, MS_UKN },
    { L_, INT_MAX, CMS::e_UNKNOWN                     , MS_UKN, MS_UKN }
};

const std::size_t DATA_SIZE = sizeof(DATA) / sizeof(DATA[0]);

#undef MS_NMV
#undef MS_MV
#undef MS_UKN

//=============================================================================
//                            FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Compare the specified memory segments `segmentA` and `segmentB`, both of
/// the specified `size` bytes, and return the number of bits that differ
/// between them.
unsigned numBitsChanged(const void *segmentA,
                        const void *segmentB,
                        size_t      size)
{
    const unsigned char *a = static_cast<const unsigned char *>(segmentA);
    const unsigned char *b = static_cast<const unsigned char *>(segmentB);

    unsigned ret = 0;
    for (const unsigned char *end = a + size; a < end; ++a, ++b) {
        for (unsigned diff = *a ^ *b; diff; diff >>= 1) {
            ret += diff & 1;
        }
    }

    return ret;
}

                                // =====
                                // Thing
                                // =====

struct Thing {
    char d_byte;

    // CLASS METHOD

    /// Return `true` if the destructor of a `Thing` is observed to execute
    /// on object destruction, and `false` otherwise.
    ///
    /// On some compilers in optimized mode, destructors that change only
    /// the footprint of the object are optimized away and are not executed.
    static
    bool isDtorExecuted();

  private:
    // PRIVATE CREATORS
    Thing() : d_byte(0) {}
    ~Thing() { d_byte = 127; }
};

                                // -----
                                // Thing
                                // -----

// CLASS METHOD
bool Thing::isDtorExecuted()
{
    bsls::ObjectBuffer<Thing> buffer;
    Thing *p = new (buffer.address()) Thing();
    BSLS_ASSERT(0 == *reinterpret_cast<char *>(p));
    p->~Thing();

    return 0 != *reinterpret_cast<char *>(p);
}

void testByVal(int                           LINE,
               bsltf::ArgumentTypeByValue<1> arg,
               int                           expVal,
               CMS::Enum                     expState)
{
    ASSERTV(LINE, expVal, int(arg), expVal == arg);
    ASSERTV(LINE, expState, arg.copyMoveState(),
            expState == arg.copyMoveState());
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Passing Arguments of the Correct Type and Order
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to test a function, `forwardData`, that takes a variable
// number of arguments and forwards them to another function (called
// `delegateFunction`, in this example).  Note, that the example below provides
// separate implementations for compilers that support C++11 and those that do
// not.  For clarity, we define `forwardData` inline and limit our C++03
// expansion of the variadic template to 2 arguments.
//
// First, we define a set of `delegateFunction` overloads.  The first
// argument, if any, is expected to have value `1` and the second argument is
// expected to have value `2`.  The return value of `delegateFunction` is `1`
// if the first argument is passed as an rvalue, `2` if the second argument is
// passed as an rvalue, `3` if they are both passed as `rvalues`, and `0`
// otherwise.  Using parameters of type `ArgumentTypeByValue` allows us to
// verify the type, value, and value-category (rvalue vs lvalue) of each
// argument in a manner that is compatible with C++03:
// ```
    int delegateFunction()
    {
        return 0;
    }

    int delegateFunction(bsltf::ArgumentTypeByValue<1> arg01)
    {
        ASSERT(1 == arg01);
        return bsltf::CopyMoveState::isMovedInto(arg01) ? 1 : 0;
    }

    int delegateFunction(bsltf::ArgumentTypeByValue<1> arg01,
                         bsltf::ArgumentTypeByValue<2> arg02)
    {
        ASSERT(1 == arg01);
        ASSERT(2 == arg02);

        int ret = 0;
        if (bsltf::CopyMoveState::isMovedInto(arg01)) ret += 1;
        if (bsltf::CopyMoveState::isMovedInto(arg02)) ret += 2;
        return ret;
    }
// ```
// Now, we define the forwarding function we intend to test, providing both
// C++03 and C++11 interfaces:
// ```
    #if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) && \
        defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

    template <class... Args>
    inline
    int forwardData(Args&&... args)
    {
        return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args, args)...);
    }

    #else // If variadic templates or rvalue references are not supported

    inline
    int forwardData()
    {
        return delegateFunction();
    }

    template <class Args1>
    inline
    int forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args1)  args_01)
    {
        return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args1, args_01));
    }

    template <class Args1, class Args2>
    inline
    int forwardData(BSLS_COMPILERFEATURES_FORWARD_REF(Args1)  args_01,
                    BSLS_COMPILERFEATURES_FORWARD_REF(Args2)  args_02)
    {
        return delegateFunction(BSLS_COMPILERFEATURES_FORWARD(Args1, args_01),
                                BSLS_COMPILERFEATURES_FORWARD(Args2, args_02));
    }

    #endif
// ```
// Finally, we define a test case for `forwardData` passing `ArgumentType` as
// arguments to the `forwardData` function and verifying that they are
// perfectly forwarded to `delegateFunction`.
// ```
    void usageExample()
    {
        int ret = forwardData();
        ASSERT(0 == ret);

        bsltf::ArgumentType<1> A01(1);
        ret = forwardData(A01);
        ASSERT(0 == ret);

        bsltf::ArgumentType<1> A11(1);
        bsltf::ArgumentType<2> A12(2);
        ret = forwardData(A11, bslmf::MovableRefUtil::move(A12));
        ASSERT(2 == ret);
        ASSERT(!bsltf::CopyMoveState::isMovedFrom(A11));
        ASSERT( bsltf::CopyMoveState::isMovedFrom(A12));

        // Note that passing arguments in a wrong order will fail to compile:
        // ret = forwardData(A12, A11);  // ERROR
    }
// ```

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;

      case 15: {
        // --------------------------------------------------------------------
        // TESTING `ArgumentTypeByValue`
        //
        // Concerns:
        // 1. Passing an `ArgumentType` object to a function taking an
        //    `ArgumentTypeByValue` parameter will faithfully pass the
        //    argument's value.
        // 2. The copy/move state of the resulting `ArgumentTypeByValue`
        //    argument will be `e_COPIED_CONST_INTO`,
        //    `e_COPIED_NONCONST_INTO`, or `e_MOVED_INTO`, depending on the
        //    value category of the actual argument.
        // 3. The copy/move state of the object passed to the function will be
        //    set to moved-from if the object is passed as an rvalue reference.
        //
        // Plan:
        // 1. Create a function taking an `ArgumentTypeByValue<N>` parameter by
        //    value.  Call the function with a const-lvalue, nonconst-lvalue,
        //    and rvalue of type `ArgumentType<N>` and verify that the value
        //    and copy/move states are transfered or set correctly.
        //
        // Testing
        //   ArgumentTypeByValue(ArgumentTypeDefault);
        //   ArgumentTypeByValue(ArgumentType<N> const& original);
        //   ArgumentTypeByValue(ArgumentType<N>      & original);
        //   ArgumentTypeByValue(bslmf::MovableRef<ArgumentType<N> > original);
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING `ArgumentTypeByValue`\n"
                            "=============================\n");

        const Obj a1(11);
        Obj a2(22);
        Obj a3(33);

        testByVal(L_, a1, 11, CMS::e_COPIED_CONST_INTO);
        testByVal(L_, a2, 22, CMS::e_COPIED_NONCONST_INTO);
        testByVal(L_, MoveUtil::move(a3), 33, CMS::e_MOVED_INTO);
        ASSERT(CMS::isMovedFrom(a3));
        testByVal(L_, Obj(44), 44,
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                  CMS::e_MOVED_INTO          // prvalue is moved from
#else
                  CMS::e_COPIED_CONST_INTO   // C++03 can't track prvalues
#endif
        );
        testByVal(L_, bsltf::ArgumentTypeDefault(), -1, CMS::e_ORIGINAL);
      } break;

      case 14: {
        // --------------------------------------------------------------------
        // TESTING DESTRUCTOR SABOTAGE
        //
        // Concern:
        // 1. That the destructor sets state of the object to one that cannot
        //    be mistaken for a valid value.
        //
        // Plan:
        // 1. Create and destroy an object in a `bsls::ObjectBuffer`, and
        //    observe that at least a quarter of the bits are changed by the
        //    destructor.
        //
        // Testing:
        //   ~ArgumentType();
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING DESTRUCTOR SABOTAGE\n"
                            "===========================\n");

        bsls::ObjectBuffer<Obj> xBuffer;
        char                    yBuffer[sizeof(Obj)];

        Obj* pX = new (xBuffer.buffer()) Obj(3);
        Obj& mX = *pX; const Obj& X = mX;
        ASSERT(3               == X);
        ASSERT(CMS::e_ORIGINAL == X.copyMoveState());

        std::memcpy(yBuffer, xBuffer.buffer(), sizeof(Obj));

        // `isDtorExecuted` is `false` if destructors like ours are optimized
        // away.
        bool isDtorExecuted = Thing::isDtorExecuted();
        if (veryVerbose) P(isDtorExecuted);

        unsigned changed = numBitsChanged(xBuffer.buffer(),
                                          yBuffer,
                                          sizeof(Obj));
        ASSERT(0 == changed);

        mX.~Obj();

        changed = numBitsChanged(xBuffer.buffer(),
                                 yBuffer,
                                 sizeof(Obj));
        ASSERT(changed >= (sizeof(Obj) * 8) / 4 || !isDtorExecuted);

      } break;

      case 13: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   Ensure that `ArgumentType` has the necessary trait values to
        //   guarantee its expected behavior.
        //
        // Concerns:
        // 1. The object does no have the `bslma::UsesBslmaAllocator` trait.
        //
        // Plan:
        // 1. Verify that `bslma::UsesBslmaAllocator<Obj>::value` is `false`.
        //    (C-1)
        //
        // Testing:
        //   bslma::UsesBslmaAllocator<ArgumentType<N>>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // VALUE-ASSIGNMENT OPERATOR
        //   Ensure that we can assign an integer value to any object of the
        //   class the value.
        //
        // Concerns:
        // 1. value assignment operator sets the value of any modifiable
        //    target object to that of the integer rhs.
        //
        // 2. The value assigment operator returns a non-const reference to the
        //    object being assigned to.
        //
        // 3. The copy/move state of the lhs is set to the default state,
        //    regardless of its initial state.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move states:
        //
        // 2. For each row in the table from P-1:
        //
        //   1. Create a modifiable `Obj`, `mX`, and `const` reference to it,
        //      `X` having the value and copy/move state from the table row.
        //
        //   2. Assign to `mX` a value `V` from a list of valid values and
        //      capture the return reference in `mR`.  Verify that the address
        //      of `mR` is equal to the address of `mX`.  (C-1)
        //
        //   3. Verify that `X` compares equal to `V`.
        //
        //   4. Verify that the copy/move attributes of `X` reflect the default
        //      copy/move state.
        //
        // Testing:
        //   ArgumentType& operator=(int rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE-ASSIGNMENT OPERATOR"
                            "\n=========================\n");

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE           = DATA[i].d_line;
            const int INIT_VALUE     = DATA[i].d_value;
            const CMS::Enum INIT_CMS = DATA[i].d_cmState;

            if (veryVerbose) { T_ P_(LINE) P(INIT_VALUE) P(INIT_CMS) }

            Obj mX; const Obj& X = mX;
            mX.set(INIT_VALUE, INIT_CMS);

            for (std::size_t j = 0; j < NUM_VALUES; ++j) {
                const int VALUE = VALUE_DATA[j];

                if (-1 == VALUE) continue;  // Skip invalid value

                if (veryVerbose) { T_ T_ P(VALUE) }

                Obj& mR = (mX = VALUE);  // value assignment

                ASSERTV(LINE, VALUE, X, &mR, &mX, &mR   == &mX);
                ASSERTV(LINE, VALUE, X, VALUE           == int(X));
                ASSERTV(LINE, VALUE, X, CMS::e_ORIGINAL == X.copyMoveState());
            }
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            ASSERT_PASS(mX = 0);
            ASSERT_PASS(mX = 1);
            ASSERT_PASS(mX = INT_MAX);
            ASSERT_FAIL(mX = -1);
        }
      } break;

      case 11: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, leaving the `rhs` in with valid but
        //   unspecified value.
        //
        // Concerns:
        // 1. The move assignment operator sets the value of any modifiable
        //    target object to that of the rhs.
        //
        // 2. The move assigment operator returns a non-const reference to the
        //    object being assigned to.
        //
        // 3. Move-assigning an object to itself has no effect (alias-safety).
        //
        // 4. Except in the case of self-assignment, the the copy/move
        //    attributes of the lhs reflect the moved-into state.  The initial
        //    copy/move attributes of the rhs are irrelevant.
        //
        // 5. The value of the lhs object is set to -1 and its copy/move
        //    attributes are updated to reflect the moved-from state.
        //
        // 6. The lhs and rhs are independent after the assignment; changing
        //    one does not change the other.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move attributes (one value and one
        //    combination of three copy/move attributes per row):
        //
        // 2. Using nested loops over the table from P-1, create an object `mX`
        //    in the outer loop and an object `mZ` in the inner loop, taking
        //    the values (V1 and V2, respectively) and copy/move states from
        //    the corresponding table rows.  Let `X` be a const reference to
        //    `mX`; similarly for `Z`/`mZ`.
        //
        //   1. Assign to `mX` from `bslmf::MovableRefUtil::move(mX)`.  Verify
        //      that neither the value nor the copy/move state of `mX` has
        //      changed.  (C-3)
        //
        //   2. Assign to `mX` from `bslmf::MovableRefUtil::move(mZ)` and
        //      capture the returned reference in `mR`.  Verify that the
        //      address of `mR` is the same as the address of `mX`.  (C-2)
        //
        //   3. Use the equality-comparison operator to verify that the value
        //      of `X` is the same as the value from the table row.  (C-1)
        //
        //   4. Verify that the copy/move attributes of `X` reflect the
        //      moved-into state.  (C-4)
        //
        //   5. Verify that `Z` is in the moved-from state.  (C-5)
        //
        //   6. Modify `mZ` (and therefore `Z`).  Verify that `X` does
        //      not change its value when `mZ` is modified.  (C-6)
        //
        // Testing:
        //   ArgumentType& operator=(bslmf::MovableRef<ArgumentType> rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE1          = DATA[i].d_line;
            const int VALUE1         = DATA[i].d_value;
            const CMS::Enum CMSTATE1 = DATA[i].d_cmState;

            if (veryVerbose) { T_ P_(LINE1) P(VALUE1) P(CMSTATE1) }

            Obj mX; const Obj& X = mX;
            mX.set(VALUE1, CMSTATE1);

            // Check self assignment
            mX = MoveUtil::move(mX);
            ASSERTV(LINE1, X, VALUE1   == X);
            ASSERTV(LINE1, X, CMSTATE1 == X.copyMoveState());

            for (std::size_t j = 0; j < DATA_SIZE; ++j) {
                const int LINE2          = DATA[j].d_line;
                const int VALUE2         = DATA[j].d_value;
                const CMS::Enum CMSTATE2 = DATA[j].d_cmState;

                if (veryVerbose) { T_ T_ P_(LINE2) P(VALUE2) P(CMSTATE2) }

                Obj mZ; const Obj& Z = mZ;
                mZ.set(VALUE2, CMSTATE2);

                Obj& mR = (mX = MoveUtil::move(mZ));
                ASSERTV(LINE1, LINE2, &mR == &mX);
                ASSERTV(LINE1, LINE2, X, VALUE2 == X);
                ASSERTV(LINE1, LINE2, X,
                        CMS::e_MOVED_INTO       == X.copyMoveState());
                ASSERTV(LINE1, LINE2, Z, -1     == Z);
                ASSERTV(LINE1, LINE2, Z,
                        CMSTATE2 == CMS::e_UNKNOWN || CMS::isMovedFrom(Z));

                // Modify Z
                mZ.set(-1, CMS::e_ORIGINAL);

                // Verify that X is unchanged
                ASSERTV(LINE1, LINE2, X, X == VALUE2);
            }
        }
      } break;

      case 10: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can move the value from one object to the other,
        //   leaving the original with a valid but unspecified value.
        //
        // Concerns:
        // 1. The move constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The the copy/move state of the new object is set to the moved-to
        //    state.  The copy/move attributes of the original are irrelevant.
        //
        // 3. The value of the original object is set to -1 and its copy/move
        //    attributes are updated to reflect the moved-from state.
        //
        // 4. The original and copy are independent; changing one does not
        //    change the other.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move attributes (one value and one
        //    combination of three copy/move attributes per row):
        //
        // 2. For each row in the table from P-1:
        //
        //   1. Create a modifiable `Obj`, `mZ`, and `const` reference to it,
        //      `Z` having the value and copy/move state from the table row.
        //
        //   2. Use the move constructor to create an object `X`, supplying it
        //      a movable reference to `mZ` (see P-2.1).
        //
        //   3. Use the equality-comparison operator to verify that The newly
        //      constructed object, `X`, has the value specified in the table
        //      row.  (C-1)
        //
        //   4. Verify that `X` is in a moved-to state.  (C-2)
        //
        //   5. Verify that `Z` is in a moved-from state with value -1.  (C-3)
        //
        //   6. Modify `mZ` (and therefore `Z`).  Verify that `X` does
        //      not change its value when `mZ` is modified.  (C-4)
        //
        // Testing:
        //   ArgumentType(bslmf::MovableRef<ArgumentType> original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTOR"
                            "\n================\n");

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE          = DATA[i].d_line;
            const int VALUE         = DATA[i].d_value;
            const CMS::Enum CMSTATE = DATA[i].d_cmState;

            if (veryVerbose) { T_ P_(LINE) P(VALUE) P(CMSTATE) }

            Obj mZ; const Obj& Z = mZ;
            mZ.set(VALUE, CMSTATE);

            const Obj X(MoveUtil::move(mZ));
            ASSERTV(LINE, X, VALUE             == X);
            ASSERTV(LINE, X, CMS::e_MOVED_INTO == X.copyMoveState());
            ASSERTV(LINE, Z, -1                == Z);
            ASSERTV(LINE, Z, CMSTATE == CMS::e_UNKNOWN || CMS::isMovedFrom(Z));

            // Modify the source object.
            mZ.set(55, CMS::e_ORIGINAL);

            ASSERTV(LINE, X, X == VALUE);
        }
      } break;

      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        // 1. The copy assignment operator sets the value of any modifiable
        //    target object to that of the rhs.
        //
        // 2. The copy assigment can have a const or non-const argument and
        //    returns a non-const reference to the object being assigned to.
        //
        // 3. Assigning an object to itself has no effect (alias-safety).
        //
        // 4. Except in the case of self-assignment, the the copy attribute of
        //    the lhs is set to `e_COPIED_CONST_INTO` or
        //    `e_COPIED_NONCONST_INTO`, depending on the constness of the
        //    constructor argument.  The copy/move attributes of the rhs are
        //    irrelevant.
        //
        // 5. The value and copy/move state of the source object is not
        //    modified.
        //
        // 6. The lhs and rhs are independent after the assignment; changing
        //    one does not change the other.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move attributes (one value and one
        //    combination of three copy/move attributes per row):
        //
        // 2. Using nested loops over the table from P-1, create two identical
        //    objects `mX` and `mY` in the outer loop and an object `mZ` in the
        //    inner loop, taking the values (V1 and V2, respectively) and
        //    copy/move states from the corresponding table rows.  Let `X` be a
        //    const reference to `mX`; similarly for `Y`/`mY` and `Z`/`mZ`.
        //
        //   1. Assign to `mX` from `X`.  Verify that neither the value nor the
        //      copy/move state of `mX` has changed.  (C-3)
        //
        //   2. Assign `mX` the value of `Z` and capture the returned reference
        //      in `mR`.  Verify that the address of `mR` is the same as the
        //      address of `mX`.  (C-2)
        //
        //   3. Use the equality-comparison operator to verify that the value
        //      of `X` is the same as the value of `Z`.  (C-1)
        //
        //   4. Verify that the copy/move state of `X` is
        //      `e_COPIED_CONST_INTO`.  (C-4)
        //
        //   5. Verify that `Z` is unchanged (still having the value and
        //      copy/move state specified in the table row).  (C-5)
        //
        //   6. Assign to `mY` from `mY`.  Verify that neither the value nor
        //      the copy/move state of `mX` has changed.  (C-3)
        //
        //   7. Assign to `mY` the value of `mZ` and capture the returned
        //      reference in `mR2`.  Verify that the address of `mR2` is the
        //      same as the address of `mY`.  (C-2)
        //
        //   8. Use the equality-comparison operator to verify that the value
        //      of `Y` is the same as the value of `Z`.  (C-1)
        //
        //   9. Verify that the copy/move state of `Y` is
        //      `e_COPIED_NONCONST_INTO`.  (C-4)
        //
        //   10. Verify that `Z` is unchanged (still having the value and
        //       copy/move state specified in the table row).  (C-5)
        //
        //   11. Modify `mZ` (and therefore `Z`).  Verify that `X` and `Y` do
        //       not change their values when `mZ` is modified.  (C-6)
        //
        // Testing:
        //   ArgumentType& operator=(ArgumentType      & rhs);
        //   ArgumentType& operator=(ArgumentType const& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE1          = DATA[i].d_line;
            const int VALUE1         = DATA[i].d_value;
            const CMS::Enum CMSTATE1 = DATA[i].d_cmState;

            if (veryVerbose) { T_ P_(LINE1) P(VALUE1) P(CMSTATE1) }

            Obj mX; const Obj& X = mX;
            Obj mY; const Obj& Y = mY; Obj& mY2 = mY;
            mX.set(VALUE1, CMSTATE1);
            mY.set(VALUE1, CMSTATE1);

            // Check self assignment from const
            mX = X;
            ASSERTV(LINE1, X, VALUE1   == X);
            ASSERTV(LINE1, X, CMSTATE1 == X.copyMoveState());

            // Check self assignment from non-const
            mY = mY2;
            ASSERTV(LINE1, Y, VALUE1   == Y);
            ASSERTV(LINE1, Y, CMSTATE1 == Y.copyMoveState());

            for (std::size_t j = 0; j < DATA_SIZE; ++j) {
                const int LINE2          = DATA[j].d_line;
                const int VALUE2         = DATA[j].d_value;
                const CMS::Enum CMSTATE2 = DATA[j].d_cmState;

                if (veryVerbose) { T_ T_ P_(LINE2) P(VALUE2) P(CMSTATE2) }

                Obj mZ; const Obj& Z = mZ;
                mZ.set(VALUE2, CMSTATE2);

                Obj& mR1 = (mX = Z);  // Assign from const
                ASSERTV(LINE1, LINE2, X, &mR1     == &mX);
                ASSERTV(LINE1, LINE2, X, VALUE2   == X);
                ASSERTV(LINE1, LINE2, X,
                        CMS::e_COPIED_CONST_INTO  == X.copyMoveState());
                ASSERTV(LINE1, LINE2, Z, VALUE2   == Z);
                ASSERTV(LINE1, LINE2, Z, CMSTATE2 == Z.copyMoveState());

                Obj& mR2 = (mY = mZ);  // Assign from non-const
                ASSERTV(LINE1, LINE2, Y, &mR2       == &mY);
                ASSERTV(LINE1, LINE2, Y, VALUE2     == Y);
                ASSERTV(LINE1, LINE2, Y,
                        CMS::e_COPIED_NONCONST_INTO == Y.copyMoveState());
                ASSERTV(LINE1, LINE2, Z, VALUE2     == Z);
                ASSERTV(LINE1, LINE2, Z, CMSTATE2   == Z.copyMoveState());

                // Modify Z
                mZ.set(-1, CMS::e_ORIGINAL);

                // Verify that X and Y are unchanged
                ASSERTV(LINE1, LINE2, X, X == VALUE2);
                ASSERTV(LINE1, LINE2, Y, Y == VALUE2);
            }
        }

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        // 1. The copy constructor creates an object having the same value as
        //    that of the supplied original object.
        //
        // 2. The the copy attribute of the new object is set to
        //    `e_COPIED_CONST_INTO` or `e_COPIED_NONCONST_INTO`, depending on
        //    the constness of the constructor argument.  The copy/move
        //    attributes of the original are irrelevant.
        //
        // 3. The value and copy/move attributes of the original object is
        //    unchanged, whether or not the original is `const`.
        //
        // 4. The original and copy are independent; changing one does not
        //    change the other.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move attributes.
        //
        // 2. For each entry in the table from P-1:
        //
        //   1. Create a modifiable `Obj`, `mZ`, and `const` reference to it,
        //      `Z` having the value and copy/move state from the table row.
        //
        //   2. Use the copy constructor to create an object `X`, supplying it
        //      the `const` reference `Z` (see P-2.1).
        //
        //   3. Use the equality-comparison operator to verify that The newly
        //      constructed object, `X`, has the value specified in the table
        //      row.  (C-1)
        //
        //   4. Verify that `Z` is unchanged (still having the value and
        //      copy/move state specified in the table row).  (C-3)
        //
        //   5. Use the copy constructor to create an object `Y`, supplying it
        //      the modifiable object `mZ` (see P-2.1).
        //
        //   6. Use the equality-comparison operator to verify that The newly
        //      constructed object, `Y`, has the value specified in the table
        //      row.  (C-1)
        //
        //   7. Verify that `mZ` is unchanged (still having the value and
        //      copy/move state specified in the table row).  (C-3)
        //
        //   8. Modify `mZ` (and therefore `Z`).  Verify that `X` and `Y` do
        //      not change their values when `mZ` is modified.  (C-4)
        //
        // Testing:
        //   ArgumentType(ArgumentType      & original);
        //   ArgumentType(ArgumentType const& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE          = DATA[i].d_line;
            const int VALUE         = DATA[i].d_value;
            const CMS::Enum CMSTATE = DATA[i].d_cmState;

            if (veryVerbose) { T_ P(VALUE) P(CMSTATE) }

            Obj mZ; const Obj& Z = mZ;
            mZ.set(VALUE, CMSTATE);

            const Obj X(Z);  // Copy from const original
            ASSERTV(LINE, X, VALUE                       == X);
            ASSERTV(LINE, X, CMS::e_COPIED_CONST_INTO    == X.copyMoveState());
            ASSERTV(LINE, Z, VALUE                       == Z);
            ASSERTV(LINE, Z, CMSTATE                     == Z.copyMoveState());

            const Obj Y(mZ);  // Copy from non-const original
            ASSERTV(LINE, Y, VALUE                       == Y);
            ASSERTV(LINE, Y, CMS::e_COPIED_NONCONST_INTO == Y.copyMoveState());
            ASSERTV(LINE, Z, VALUE                       == Z);
            ASSERTV(LINE, Z, CMSTATE                     == Z.copyMoveState());

            // Modify the source object.
            mZ.set(55, CMS::e_MOVED_INTO);

            ASSERTV(LINE, Z, 55                          == Z);
            ASSERTV(LINE, Z, CMS::e_MOVED_INTO           == Z.copyMoveState());
            ASSERTV(LINE, X, VALUE                       == X);
            ASSERTV(LINE, X, CMS::e_COPIED_CONST_INTO    == X.copyMoveState());
            ASSERTV(LINE, Y, VALUE                       == Y);
            ASSERTV(LINE, Y, CMS::e_COPIED_NONCONST_INTO == Y.copyMoveState());
        }

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Objects of the test type are compared by their values [implicitly]
        //   converted to `int`.
        //
        // Concerns:
        // 1. Two `ArgumentType` objects can be compared for equality, even if
        //    they have different template arguments.
        //
        // 2. An `ArgumentType` object can be compared for equality with an
        //    `int`, object, with the `int` being on either side of the
        //    comparison operator.
        //
        // 3. A object compares equal to itself.
        //
        // 4. The result of operators `==` or `!=` is the same as converting
        //    both sized to `int`; the non-saliant copy/move attributes are
        //    ignored.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move attributes (one value and one
        //    combination of three copy/move attributes per row):
        //
        // 2. Using nested loops over the table from P-1, create an object `mX`
        //    in the outer loop and an object `mY` in the inner loop, taking
        //    the value (V1 and V2, respectively) and copy/move states from the
        //    corresponding table rows.  Then:
        //
        //   1. Verify that `X == X` and `not X != X`.  (C-3)
        //
        //   2. If V1 == V2:
        //
        //     1. Verify that `X == Y` and `not X != Y`.  (C-1, C-4)
        //
        //     2. Verify that `X == V2`, `not (X != V2)`, `V1 == Y`, and 'not
        //        (V1 != Y)' (C-2, C-4)
        //
        //   3. Otherwise, the results of P-2.2.1 and P-2.2.2 should be
        //      inverted. (C-1, C-2, C-4)
        //
        // Testing:
        //     bool operator==   // implicitly available via `int` conversion
        //     bool operator!=   // implicitly available via `int` conversion
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS\n"
                            "\n=============================\n");

        typedef bsltf::ArgumentType<1> Obj1;
        typedef bsltf::ArgumentType<2> Obj2;

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE1          = DATA[i].d_line;
            const int VALUE1         = DATA[i].d_value;
            const CMS::Enum CMSTATE1 = DATA[i].d_cmState;

            if (veryVerbose) { T_ P(LINE1) P(VALUE1) P(CMSTATE1) }

            Obj1 mX; const Obj1& X = mX;
            mX.set(VALUE1, CMSTATE1);

            // Check self equality
            ASSERTV(LINE1, X,    X == X);
            ASSERTV(LINE1, X, ! (X != X));

            for (std::size_t j = 0; j < DATA_SIZE; ++j) {
                const int LINE2          = DATA[j].d_line;
                const int VALUE2         = DATA[j].d_value;
                const CMS::Enum CMSTATE2 = DATA[j].d_cmState;

                if (veryVerbose) { T_ T_ P(LINE2) P(VALUE2) P(CMSTATE2) }

                Obj2 mY; const Obj2& Y = mY;
                mY.set(VALUE2, CMSTATE2);

                // Object comparisons
                ASSERTV(LINE1, LINE2, (X == Y) == (VALUE1 == VALUE2));
                ASSERTV(LINE1, LINE2, (X != Y) == (VALUE1 != VALUE2));

                // Compare object to int
                ASSERTV(LINE1, LINE2, (X == VALUE2) == (VALUE1 == VALUE2));
                ASSERTV(LINE1, LINE2, (X != VALUE2) == (VALUE1 != VALUE2));
                ASSERTV(LINE1, LINE2, (VALUE1 == Y) == (VALUE1 == VALUE2));
                ASSERTV(LINE1, LINE2, (VALUE1 != Y) == (VALUE1 != VALUE2));
            }
        }

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        // 1. Each of the (deprecated) `movedInto` and `movedFrom` accessors
        //    return the expected result for each valid copy/move state.
        //
        // 2. A call to `obj.copyMoveState()` returns the same value as
        //    `CopyMoveState::get(obj)`, where `obj` is an object of type
        //    `CopyMoveState`.
        //
        // 3. Each `static` *psuedo* *accessor* defined in `CopyMoveState`
        //    (`isOriginal(obj)`, `isCopiedInto(obj)`, etc.) returns the
        //    correct value when called on an `ArgumentType` object.
        //
        // 4. Each accessor and psuedo accessor operates on a `const` object.
        //
        // Plan:
        // 1. Using the table-driven technique, create on object in each
        //    copy/move state and create a `const` reference to that object.
        //    For each such object:
        //
        //   1. Using expected values from the table to verify that the
        //      `movedInto` and `movedFrom` accessors return the correct
        //      values.  (C-1)
        //
        //   2. Verify that `obj.copyMoveState()` returns the same value as
        //      `CopyMoveState::get(obj)`.  (C-2)
        //
        //   3. Verify that each psuedo-accessor in `CopyMoveState` returns the
        //      correct result for the object.  (C-3)
        //
        //   4. In the steps above, verify that the functions can be applied to
        //      the `const` reference.  (C-4)
        //
        // Testing:
        //   MoveState::Enum movedFrom() const;             // deprecated
        //   MoveState::Enum movedInto() const;             // deprecated
        //   CopyMoveState::Enum CopyMoveState::get(obj);   // psuedo accessor
        //   bool CopyMoveState::isCopiedConstInto(obj);    // psuedo accessor
        //   bool CopyMoveState::isCopiedInto(obj);         // psuedo accessor
        //   bool CopyMoveState::isCopiedNonconstInto(obj); // psuedo accessor
        //   bool CopyMoveState::isMovedFrom(obj);          // psuedo accessor
        //   bool CopyMoveState::isMovedInto(obj);          // psuedo accessor
        //   bool CopyMoveState::isOriginal(obj);           // psuedo accessor
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        Obj mX; const Obj& X = mX;

        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE                 = DATA[i].d_line;
            const int VALUE                = DATA[i].d_value;
            const CMS::Enum CMSTATE        = DATA[i].d_cmState;
            const MS::Enum  EXP_MOVED_INTO = DATA[i].d_expMovedInto;
            const MS::Enum  EXP_MOVED_FROM = DATA[i].d_expMovedFrom;

            if (veryVerbose) { T_ P(VALUE) P(CMSTATE) }

            mX.set(VALUE, CMSTATE);

            ASSERTV(LINE, X, VALUE   == int(X));
            ASSERTV(LINE, X, CMSTATE == X.copyMoveState());
            ASSERTV(LINE, X, X.copyMoveState() == CMS::get(X));

#define EXP(BITS) ((CMSTATE & CMS::e_ ## BITS) == CMS::e_ ## BITS)

            ASSERTV(LINE, ((CMSTATE & ~CMS::e_MOVED_FROM) == CMS::e_ORIGINAL)
                    == CMS::isOriginal(X));
            ASSERTV(LINE, EXP(COPIED_INTO)       == CMS::isCopiedInto(X));
            ASSERTV(LINE, EXP(COPIED_CONST_INTO) == CMS::isCopiedConstInto(X));
            ASSERTV(LINE,
                    EXP(COPIED_NONCONST_INTO) == CMS::isCopiedNonconstInto(X));
            ASSERTV(LINE, EXP(MOVED_INTO)        == CMS::isMovedInto(X));
            ASSERTV(LINE, EXP(MOVED_FROM)        == CMS::isMovedFrom(X));
            ASSERTV(LINE, EXP(UNKNOWN)           == CMS::isUnknown(X));

#undef EXP

#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
            ASSERTV(LINE, X, X.movedInto() == EXP_MOVED_INTO);
            ASSERTV(LINE, X, X.movedFrom() == EXP_MOVED_FROM);
            ASSERTV(LINE, X, X.movedInto() == getMovedInto(X));
            ASSERTV(LINE, X, X.movedFrom() == getMovedFrom(X));
#ifdef BSLS_PLATFORM_PRAGMA_GCC_DIAGNOSTIC_GCC
#pragma GCC diagnostic pop
#endif
        }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        // 1. The value constructor can create an object having any value that
        //    does not violate the documented precondition.
        //
        // 2. The resulting object is in the default copy/move state.
        //
        // 3. (Negative testing) In the appropriate build mode, supplying an
        //    out-of-bounds value results in an assertion failure.
        //
        // Plan:
        // 1. Using the table-driven technique, specify a set of distinct
        //    integer values (one per row).
        //
        // 2. For each row (representing a distinct object value, `V`) in the
        //    table of P-1:
        //
        //     1. Use the value constructor to create an object having the
        //        value `V` and convert the object to `int` verify that it has
        //        value `V`.  (C-1)
        //
        //     2. Use the `copyMoveState` accessor to verify that the copy/move
        //        state is the default state.  (C-2)
        //
        // 3. Using a representative sample of valid and invalid values,
        //    construct an object using the value constructor and verify that
        //    an assert failure occurs iff the value is not valid.
        //
        // Testing:
        //   ArgumentType(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CONSTRUCTOR"
                            "\n=================\n");

        for (std::size_t vi = 0; vi < NUM_VALUES; ++vi) {
            const int VALUE = VALUE_DATA[vi];

            if (VALUE < 0) continue;  // Skip invalid values

            if (veryVerbose) { T_ P(VALUE) }

            Obj mX(VALUE);  const Obj& X = mX;

            ASSERTV(VALUE, X, VALUE == int(X));
            ASSERTV(VALUE, X, CMS::e_ORIGINAL == X.copyMoveState());
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_PASS(Obj(0));
            ASSERT_PASS(Obj(1));
            ASSERT_PASS(Obj(INT_MAX));
            ASSERT_FAIL(Obj(-1));
            ASSERT_FAIL(Obj(-2));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR AND PRIMARY MANIPULATORS
        //   Ensure that we can construct an object with an arbitary value and
        //   any valid copy/move state.  This case fully tests the default
        //   constructor, the `set` method, and main accessors.
        //
        // Concerns:
        // 1. An object created with the default constructor has the
        //    contractually specified default value and default copy/move
        //    state.
        //
        // 2. The `set` method can set the object to any valid state.
        //
        // 3. The `reset` method will set the object to the default-constructed
        //    state.
        // 4. The value and copy/move state can be read through a const
        //    reference using the appropriate accessors.
        //
        // 5. (Negative testing) In the appropriate build mode, setting an
        //    invalid combination of value and copy/move state results in an
        //    assertion failure.
        //
        // Plan:
        // 1. Default-construct an object and use the individual (as yet
        //    unproven) `int` conversion operator and `copyMoveState`
        //    accessor to verify the default-constructed state.  (C-1)
        //
        // 2. Using the table-driven technique, specify a set of distinct
        //    object states comprising a representative set of values and every
        //    valid combination of copy/move states.
        //
        // 3. For each object state in the table from P-2:
        //
        //   1. Use the `set` method to put an object, `mX`, into the state
        //    specified in the table rows.  (C-2)
        //
        //   2. Use the `int` conversion operator to Verify that `mX` has the
        //      intended integer value.  (C-4)
        //
        //   3. Use the `copyMoveState` accessor to verify that `mX` has the
        //      intended copy/move state.  (C-4)
        //
        //   4. Use the `reset` method on `mX` and use the `int` conversion
        //      operator and `copyMoveState` accessor to verify that it is back
        //      to its default-constructed state.  (C-3)
        //
        // 4. Using the `AssertTest` mechanism and a representative sample of
        //    valid and invalid arguments to `set`, verify that the invalid
        //    combinations trigger assert failures whereas the valid ones do
        //    not.  (C-5)
        //
        // Testing:
        //   ArgumentType(ArgumentTypeDefault);
        //   void set(int value, CopyMoveState::Enum state);
        //   operator int() const;
        //   CopyMoveState::Enum copyMoveState() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR AND PRIMARY MANIPULATORS"
                            "\n=====================================\n");

        const int D = -1;  // default value attribute

        // P1: Test default constructor
        Obj mX; const Obj& X = mX;
        ASSERTV(X, D                == int(X));
        ASSERTV(X, CMS::e_ORIGINAL  == X.copyMoveState());

        const Obj Y = bsltf::ArgumentTypeDefault();
        ASSERTV(Y, D                == int(Y));
        ASSERTV(Y, CMS::e_ORIGINAL  == Y.copyMoveState());

        // Steps P-2..3: Loop through tables of values
        for (std::size_t i = 0; i < DATA_SIZE; ++i) {
            const int LINE          = DATA[i].d_line;
            const int VALUE         = DATA[i].d_value;
            const CMS::Enum CMSTATE = DATA[i].d_cmState;

            if (veryVerbose) { T_ P(VALUE) P(CMSTATE) }

            mX.set(VALUE, CMSTATE);
            ASSERTV(LINE, X, VALUE == int(X));
            ASSERTV(LINE, X, CMSTATE == X.copyMoveState());

            mX.reset();
            ASSERTV(LINE, X, D                == int(X));
            ASSERTV(LINE, X, CMS::e_ORIGINAL  == X.copyMoveState());
        }

        if (verbose) printf("\tNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;
            ASSERT_PASS(mX.set( 4, CMS::e_ORIGINAL));
            ASSERT_PASS(mX.set( 1, CMS::e_MOVED_INTO));
            ASSERT_PASS(mX.set(-1, CMS::e_MOVED_FROM));
            ASSERT_PASS(mX.set(-1, CMS::Enum(CMS::e_MOVED_INTO |
                                             CMS::e_MOVED_FROM)));

            ASSERT_FAIL(mX.set(-2, CMS::e_ORIGINAL));
            ASSERT_FAIL(mX.set( 0, CMS::e_MOVED_FROM));
            ASSERT_FAIL(mX.set( 1, CMS::Enum(CMS::e_COPIED_INTO |
                                             CMS::e_MOVED_FROM)));
            ASSERT_FAIL(mX.set( 1, CMS::Enum(CMS::e_COPIED_NONCONST_INTO |
                                             CMS::e_MOVED_INTO)));
            ASSERT_FAIL(mX.set(-1, CMS::Enum(CMS::e_COPIED_NONCONST_INTO |
                                             CMS::e_MOVED_INTO)));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            Obj X;
            ASSERT(-1 == X);
            ASSERT(CMS::e_ORIGINAL == X.copyMoveState());
            ASSERT(  CMS::isOriginal(X));
            ASSERT(! CMS::isCopiedInto(X));
            ASSERT(! CMS::isMovedInto(X));
            ASSERT(! CMS::isMovedFrom(X));

            Obj Y(1);
            ASSERT(1 == Y);
            ASSERT(CMS::e_ORIGINAL == Y.copyMoveState());
            ASSERT(  CMS::isOriginal(Y));
            ASSERT(! CMS::isCopiedInto(Y));
            ASSERT(! CMS::isMovedInto(Y));
            ASSERT(! CMS::isMovedFrom(Y));

            Obj Z(Y);
            ASSERT(1 == Z);
            ASSERT(CMS::e_COPIED_NONCONST_INTO == Z.copyMoveState());
            ASSERT(! CMS::isOriginal(Z));
            ASSERT(  CMS::isCopiedNonconstInto(Z));
            ASSERT(! CMS::isMovedInto(Z));
            ASSERT(! CMS::isMovedFrom(Z));
            ASSERT(  CMS::isOriginal(Y));
            ASSERT(! CMS::isMovedFrom(Y));

            Obj M(MoveUtil::move(Z));
            ASSERT(1 == M);
            ASSERT(CMS::e_MOVED_INTO == M.copyMoveState());
            ASSERT(! CMS::isOriginal(M));
            ASSERT(! CMS::isCopiedInto(M));
            ASSERT(  CMS::isMovedInto(M));
            ASSERT(! CMS::isMovedFrom(M));
            ASSERT((CMS::e_COPIED_NONCONST_INTO | CMS::e_MOVED_FROM) ==
                   Z.copyMoveState());
            ASSERT(  CMS::isCopiedNonconstInto(Z));
            ASSERT(  CMS::isMovedFrom(Z));
        }
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
// Copyright 2016 Bloomberg Finance L.P.
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
