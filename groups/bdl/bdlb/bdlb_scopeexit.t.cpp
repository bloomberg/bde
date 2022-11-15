// bdlb_scopeexit.t.cpp                                               -*-C++-*-
#include <bdlb_scopeexit.h>

#include <bdlb_tokenizer.h>

#include <bdlf_bind.h>

#include <bsla_unreachable.h>
#include <bsla_fallthrough.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_managedptr.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_movableref.h>

#include <bsl_cstring.h>
#include <bsl_sstream.h>
#include <bsl_string_view.h>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bsl_utility.h>  // for bsl::declval
#endif
#include <bsl_vector.h>

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FE01
// BDE_VERIFY pragma: -TW01


//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 3] ~ScopeExit()
// [ 3] release()
// [ 4] template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
// [ 4] explicit ScopeExit(void (*function)())
// [ 5] ScopeExit(bslmf::MovableRef<ScopeExit> original)
// [ 6] ScopeExitUtil::makeScopeExit()
// [ 7] ScopeExitAny
// [ 8] BDLB_SCOPEEXIT_PROCTOR
// [ 9] BDLB_SCOPEEXIT_GUARD
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST MACHINERY
// [10] CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)
// [11] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    BSLMF_ASSERT(BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L);
    // 'BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L' is implied here

    #define BDLB_SCOPEEXIT_USES_MODERN_CPP
        // See header for meaning, replicated here for white box tests.
#endif

#if defined(BDE_BUILD_TARGET_EXC) && defined(BDLB_SCOPEEXIT_USES_MODERN_CPP)
    #define BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE
        // 'BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE' is defined if all compilers
        // features (exceptions, C++11 'auto' and lambdas) are present that are
        // needed to compile the C++11 (and later) usage example.
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    #define U_PARAM_TO_EXIT_FUNC_CONDITIONAL_EXPLICIT explicit
#else
    #define U_PARAM_TO_EXIT_FUNC_CONDITIONAL_EXPLICIT
#endif
    // This macro is defined to 'explicit' when explicit conversions from
    // 'bdlb::ScopeExit<EXIT_FUNC>' constructor 'EXIT_FUNC_PARAM' parameters
    // are supported by our implementation.  The 'bdlb::ScopeExit<EXIT_FUNC>'
    // converting constructor itself is 'explicit' so this is fine.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #define U_DECLVAL(type) bsl::declval<type>()
#else
    #define U_DECLVAL(type) (*(type*)(0))
#endif
    // "Portable 'std::declval<>()'".

// ============================================================================
//                           HELPER TYPE ALIASES
// ----------------------------------------------------------------------------

typedef bslmf::MovableRefUtil MoveUtil;

// ============================================================================
//                             TEST MACHINERY
// ----------------------------------------------------------------------------

static const char k_IDENT_CHARS[] =
             "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // Characters that may be present in C/C++ identifiers.  Used from more
    // than one function.  We avoid depending on 'bdlb::CharType' just for
    // testing as it would raise the level of this component.

bool testMacroText(const bsl::string_view& expectedTokens,
                   const bsl::string_view& macroText)
    // Return 'true' if the specified 'macroText' contains the 'expectedTokens'
    // in the same order as in the argument, and 'false' otherwise.
    // Preprocessed tokens in C++ are separated by space characters that may be
    // optional between certain tokens.  'expectedTokens' is a descriptor that
    // contains literal characters (tokens), special token delimiter
    // characters, and special unique identifier sequence indicator character:
    //: ' ': mandatory (at least one space) token separator
    //: '$': optional (space) token separator
    //: '#': indicates a non-empty sequence of identifier characters
    //
    // The behavior is undefined if 'expectedTokens' contains more than one
    // special character without an intervening literal (token) character.  The
    // behavior is also undefined if either or the arguments is empty or
    // contains spaces only.
{
    static const bsl::string_view::size_type npos = bsl::string_view::npos;

    const bsl::size_t eSize = expectedTokens.size();

    bsl::size_t mPos = 0;

    for (bsl::size_t ePos = 0; ePos < expectedTokens.size(); ++ePos) {
        switch (expectedTokens[ePos]) {
          case ' ': {                            // At least one space required
            if (' ' != macroText[mPos]) {
                // Missing space(s)
                return false;                                         // RETURN
            }
            BSLA_FALLTHROUGH;
          }                                                     // FALL-THROUGH
          case '$': {                                      // Optional space(s)
            mPos = macroText.find_first_not_of(" ", mPos);
            if (npos == mPos && ePos + 1 != eSize) {
                // Missing tokens
                return false;                                         // RETURN
            }
          } break;
          case '#': {                      // At least one identifier character
            if (0 == bsl::strchr(k_IDENT_CHARS, macroText[mPos])) {
                // Not an identifier character
                return false;                                         // RETURN
            }
            mPos = macroText.find_first_not_of(k_IDENT_CHARS, mPos);
            if (npos == mPos && ePos + 1 != eSize) {
                // Missing tokens
                return false;                                         // RETURN
            }
          } break;
          default: {  // Literal token
            const bsl::size_t tEnd = expectedTokens.find_first_of(" $#", ePos);
            const bsl::size_t tLen = (npos == tEnd ? eSize : tEnd) - ePos;
            if (macroText.substr(mPos, tLen) !=
                                           expectedTokens.substr(ePos, tLen)) {
                // Token does not match
                return false;                                         // RETURN
            }
            ePos += tLen - 1; // We have a '++ePos' in the 'for' above
            mPos += tLen;
          }
        }
    }

    return true;
}

                         // ======================
                         // struct IncrementValues
                         // ======================

struct IncrementValues {
    // This 'struct' provides unique counter-increment values that can be used
    // to differentiate between different kinds of exit functions executed.

    enum Enum {
        k_NONE,                          //  0 is unused/skipped
        k_EXIT_FUNCTION_PARAM          , //  1 'ExitFunctionParam'
        k_EXIT_FUNCTION_PARAM_THROW    , //  2 'ExitFunctionParamThrows'
        k_EXIT_FUNCTION                , //  3 'ExitFunction'
        k_MOVE_THROWS_EXIT_FUNCTION    , //  4 'MoveOnlyExitFunction'
        k_COPY_THROWS_EXIT_FUNCTION    , //  5 'CopyOnlyExitFunction'
        k_BOTH_THROW_EXIT_FUNCTION     , //  6 'CopyOnlyExitFunction'
        k_MOVE_ONLY_EXIT_FUNCTION_PARAM, //  7 'MoveOnlyExitFunctionParam'
        k_MOVE_ONLY_EXIT_FUNCTION      , //  8 'MoveOnlyExitFunction'
        k_COPY_ONLY_EXIT_FUNCTION      , //  9 'CopyOnlyExitFunction'
        k_MOVE_COPY_EXIT_FUNCTION      , // 10 'MoveCopyExitFunction'
        k_FUNCTION_POINTER             , // 11 'void freeExitFunction()'
        k_STATELESS_LAMBDA             , // 12 any non-capturing lambda
        k_STATEFUL_LAMBDA                // 13 lambdas that capture
    };
};

                         // ======================
                         // struct ExceptionValues
                         // ======================

#ifdef BDE_BUILD_TARGET_EXC
struct ExceptionValues {
    // This 'struct' provides a unique exception type 'Enum' and a bound set
    // of unique exception values (the enumerators) that can be used to
    // identify the exceptions thrown by copy or move of different type of
    // functors.

    enum Enum {
        k_EXIT_FUNCTION_PARAM_THROW,     //  0 ExitFunctionParamThrows
        k_MOVE_THROWS_EXIT_FUNCTION,     //  1 MoveThrowsExitFunction
        k_COPY_THROWS_EXIT_FUNCTION,     //  2 CopyThrowsExitFunction
        k_MOVE_BOTH_THROW_EXIT_FUNCTION, //  3 move 'BothThrowsExitFunction'
        k_COPY_BOTH_THROW_EXIT_FUNCTION, //  4 copy 'BothThrowsExitFunction'
        k_COPY_THROWS_LAMBDA,            //  5 lambdas that throws on copy
        k_MOVE_THROWS_LAMBDA             //  6 lambdas that throws on move
    };
};
#endif

                          // =================
                          // ExitFunctionParam
                          // =================

class ExitFunctionParam {
    // A functor class that and increments the counter supplied on construction
    // when called, and 'ExitFunction' below can be constructed from it.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_EXIT_FUNCTION_PARAM'.

    // CREATORS
    explicit ExitFunctionParam(int *counter_p);
        // Create an 'ExitFunctionParam' object that will increment the integer
        // pointed by the specified 'counter_p' when it is called.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by 'IncrementValues::k_EXIT_FUNCTION_PARAM'.

    int *counterPtr() const;
        // Return the counter pointer of this object.
};

                          // -----------------
                          // ExitFunctionParam
                          // -----------------
// CLASS METHODS
int ExitFunctionParam::theIncrement()
{
    return IncrementValues::k_EXIT_FUNCTION_PARAM;
}

// CREATORS
ExitFunctionParam::ExitFunctionParam(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

// ACCESSORS
void ExitFunctionParam::operator()() const
{
    *d_counter_p += theIncrement();
}

int *ExitFunctionParam::counterPtr() const
{
    return d_counter_p;
}

                        // =======================
                        // ExitFunctionParamThrows
                        // =======================

#ifdef BDE_BUILD_TARGET_EXC
class ExitFunctionParamThrows {
    // A functor class that increments the counter supplied on construction
    // when called, and 'ExitFunction' has a constructor for that throws.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_EXIT_FUNCTION_PARAM_THROW'.

    // CREATORS
    explicit ExitFunctionParamThrows(int *counter_p);
        // Create an 'ExitFunctionParamThrows' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_EXIT_FUNCTION_PARAM_THROW'.

    int *counterPtr() const;
        // Return the counter pointer of this object.
};

                        // -----------------------
                        // ExitFunctionParamThrows
                        // -----------------------
// CLASS METHODS
int ExitFunctionParamThrows::theIncrement()
{
    return IncrementValues::k_EXIT_FUNCTION_PARAM_THROW;
}

// CREATORS
ExitFunctionParamThrows::ExitFunctionParamThrows(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

// ACCESSORS
void ExitFunctionParamThrows::operator()() const
{
    *d_counter_p += theIncrement();
}

int *ExitFunctionParamThrows::counterPtr() const
{
    return d_counter_p;
}
#endif // BDE_BUILD_TARGET_EXC

                             // ============
                             // ExitFunction
                             // ============

class ExitFunction {
    // A functor that increments the counter supplied on construction when
    // called.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_EXIT_FUNCTION'.

    // CREATORS
    explicit ExitFunction(int *counter_p);
        // Create an 'ExitFunction' object that will increment the specified
        // 'counter_p' upon its invocation.

    U_PARAM_TO_EXIT_FUNC_CONDITIONAL_EXPLICIT
    ExitFunction(const ExitFunctionParam& param);
        // Create an 'ExitFunction' object that will increment the same counter
        // that the specified 'param' does.

#ifdef BDE_BUILD_TARGET_EXC
    U_PARAM_TO_EXIT_FUNC_CONDITIONAL_EXPLICIT
    ExitFunction(const ExitFunctionParamThrows&);
        // Unconditionally throw an integer of value
        // 'ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW'.
#endif

    // ACCESSORS
    void operator()() const;
        // Increment the counter by 'IncrementValues::k_EXIT_FUNCTION'.
};

                             // ------------
                             // ExitFunction
                             // ------------
// CLASS METHODS
int ExitFunction::theIncrement()
{
    return IncrementValues::k_EXIT_FUNCTION;
}

// CREATORS
ExitFunction::ExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

ExitFunction::ExitFunction(const ExitFunctionParam& param)
: d_counter_p(param.counterPtr())
{
    BSLS_ASSERT(param.counterPtr());
}

#ifdef BDE_BUILD_TARGET_EXC
ExitFunction::ExitFunction(const ExitFunctionParamThrows&)
{
    throw ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW;
}
#endif

// ACCESSORS
void ExitFunction::operator()() const
{
    *d_counter_p += theIncrement();
}

                        // ======================
                        // MoveThrowsExitFunction
                        // ======================

#ifdef BDE_BUILD_TARGET_EXC
class MoveThrowsExitFunction {
    // A functor that increments the counter supplied on construction when
    // called and throws when moved.  The copy constructor does not exist so
    // we cannot use it by mistake.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    MoveThrowsExitFunction(const MoveThrowsExitFunction&) BSLS_KEYWORD_DELETED;
    MoveThrowsExitFunction& operator=(const MoveThrowsExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MoveThrowsExitFunction& operator=(MoveThrowsExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_MOVE_THROWS_EXIT_FUNCTION'.

    // CREATORS
    explicit MoveThrowsExitFunction(int *counter_p);
        // Create an 'MoveThrowsExitFunction' object that will increment the
        // specified 'counter_p' upon its invocation.

    MoveThrowsExitFunction(bslmf::MovableRef<MoveThrowsExitFunction>);
        // Throw 'ExceptionValues::k_MOVE_THROWS_EXIT_FUNCTION'.

        // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_MOVE_THROWS_EXIT_FUNCTION'.
};

                        // ----------------------
                        // MoveThrowsExitFunction
                        // ----------------------
// CLASS METHODS
int MoveThrowsExitFunction::theIncrement()
{
    return IncrementValues::k_MOVE_THROWS_EXIT_FUNCTION;
}

// CREATORS
MoveThrowsExitFunction::MoveThrowsExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

MoveThrowsExitFunction::MoveThrowsExitFunction(
                                     bslmf::MovableRef<MoveThrowsExitFunction>)
{
    throw ExceptionValues::k_MOVE_THROWS_EXIT_FUNCTION;
}

// ACCESSORS
void MoveThrowsExitFunction::operator()() const
{
    *d_counter_p += theIncrement();
}
#endif // BDE_BUILD_TARGET_EXC

                        // ======================
                        // CopyThrowsExitFunction
                        // ======================

#ifdef BDE_BUILD_TARGET_EXC
class CopyThrowsExitFunction {
    // A functor that increments the counter supplied on construction when
    // called and throws when copied.  The move constructor does not exist so
    // we cannot use it by mistake.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    CopyThrowsExitFunction& operator=(const CopyThrowsExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    CopyThrowsExitFunction(CopyThrowsExitFunction&&);
    CopyThrowsExitFunction& operator=(CopyThrowsExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_MOVE_THROWS_EXIT_FUNCTION'.

    // CREATORS
    explicit CopyThrowsExitFunction(int *counter_p);
        // Create an 'CopyThrowsExitFunction' object that will increment the
        // specified 'counter_p' upon its invocation.

    CopyThrowsExitFunction(const CopyThrowsExitFunction&);
        // Throw 'ExceptionValues::k_COPY_THROWS_EXIT_FUNCTION'.

        // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_COPY_THROWS_EXIT_FUNCTION'.
};

                        // ----------------------
                        // CopyThrowsExitFunction
                        // ----------------------

// CLASS METHODS
int CopyThrowsExitFunction::theIncrement()
{
    return IncrementValues::k_COPY_THROWS_EXIT_FUNCTION;
}

// CREATORS
CopyThrowsExitFunction::CopyThrowsExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

CopyThrowsExitFunction::CopyThrowsExitFunction(const CopyThrowsExitFunction&)
{
    throw ExceptionValues::k_COPY_THROWS_EXIT_FUNCTION;
}

// ACCESSORS
void CopyThrowsExitFunction::operator()() const
{
    *d_counter_p += theIncrement();
}
#endif // BDE_BUILD_TARGET_EXC

                         // =====================
                         // BothThrowExitFunction
                         // =====================

#ifdef BDE_BUILD_TARGET_EXC
class BothThrowExitFunction {
    // A functor that increments the counter supplied on construction when
    // called and throws when copied or moved.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    BothThrowExitFunction& operator=(const BothThrowExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    BothThrowExitFunction& operator=(BothThrowExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_BOTH_THROW_EXIT_FUNCTION'.

    // CREATORS
    explicit BothThrowExitFunction(int *counter_p);
        // Create an 'BothThrowExitFunction' object that will increment the
        // specified 'counter_p' upon its invocation.

    BothThrowExitFunction(const BothThrowExitFunction&);
        // Throw 'ExceptionValues::k_COPY_BOTH_THROW_EXIT_FUNCTION'.

    BothThrowExitFunction(bslmf::MovableRef<BothThrowExitFunction>);
        // Throw 'ExceptionValues::k_MOVE_BOTH_THROW_EXIT_FUNCTION'.

        // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_BOTH_THROW_EXIT_FUNCTION'.
};

                        // ---------------------
                        // BothThrowExitFunction
                        // ---------------------
// CLASS METHODS
int BothThrowExitFunction::theIncrement()
{
    return IncrementValues::k_BOTH_THROW_EXIT_FUNCTION;
}

// CREATORS
BothThrowExitFunction::BothThrowExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

BothThrowExitFunction::BothThrowExitFunction(const BothThrowExitFunction&)
{
    throw ExceptionValues::k_COPY_BOTH_THROW_EXIT_FUNCTION;
}

BothThrowExitFunction::BothThrowExitFunction(
                                      bslmf::MovableRef<BothThrowExitFunction>)
{
    throw ExceptionValues::k_MOVE_BOTH_THROW_EXIT_FUNCTION;
}

// ACCESSORS
void BothThrowExitFunction::operator()() const
{
    *d_counter_p += theIncrement();
}
#endif // BDE_BUILD_TARGET_EXC

                      // =========================
                      // MoveOnlyExitFunctionParam
                      // =========================

class MoveOnlyExitFunctionParam {
    // A move-only functor class that is convertible to 'MoveOnlyExitFunction'
    // (see below) and increments the counter supplied on construction when
    // called.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    MoveOnlyExitFunctionParam(const MoveOnlyExitFunctionParam&)
                                                          BSLS_KEYWORD_DELETED;
    MoveOnlyExitFunctionParam& operator=(const MoveOnlyExitFunctionParam&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MoveOnlyExitFunctionParam& operator=(MoveOnlyExitFunctionParam&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION_PARAM'.

    // CREATORS
    explicit MoveOnlyExitFunctionParam(int *counter_p);
        // Create an 'MoveOnlyExitFunctionParam' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called.

    U_PARAM_TO_EXIT_FUNC_CONDITIONAL_EXPLICIT
    MoveOnlyExitFunctionParam(
                        bslmf::MovableRef<MoveOnlyExitFunctionParam> original);
        // Create a 'MoveOnlyExitFunctionParam' object having the same value as
        // the specified 'original' object by initializing this object's
        // counter pointer to that of the 'original' and setting the 'original'
        // to the moved-from state by setting its pointer to null.  The
        // behavior is undefined if 'original' is in a moved-from state.

    // MANIPULATORS
    void reset();
        // Set this object to moved-from state.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION_PARAM'.  The behavior is
        // undefined if this object is in a moved-from state.

    int *counterPtr() const;
        // Return the counter pointer stored in this object.
};

                      // -------------------------
                      // MoveOnlyExitFunctionParam
                      // -------------------------
// CLASS METHODS
int MoveOnlyExitFunctionParam::theIncrement()
{
    return IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION_PARAM;
}

// CREATORS
MoveOnlyExitFunctionParam::MoveOnlyExitFunctionParam(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

MoveOnlyExitFunctionParam::MoveOnlyExitFunctionParam(
                         bslmf::MovableRef<MoveOnlyExitFunctionParam> original)
: d_counter_p(bslmf::MovableRefUtil::access(original).d_counter_p)
{
    BSLS_ASSERT(bslmf::MovableRefUtil::access(original).d_counter_p != 0);

    bslmf::MovableRefUtil::access(original).d_counter_p = 0;
}

// MANIPULATORS
void MoveOnlyExitFunctionParam::reset()
{
    d_counter_p = 0;
}

// ACCESSORS
void MoveOnlyExitFunctionParam::operator()() const
{
    BSLS_ASSERT(d_counter_p);

    *d_counter_p += theIncrement();
}

int *MoveOnlyExitFunctionParam::counterPtr() const
{
    return d_counter_p;
}

                //  ==========================================
                // < TYPE TRAITS -- MoveOnlyExitFunctionParam >
                //  ==========================================

#ifndef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
// In case there is no reliable '<type_traits>' header we use 'bsl' a trait to
// mark our class no-throw-move-constructible.
namespace bsl {
template <>
struct is_nothrow_move_constructible<MoveOnlyExitFunctionParam>
: bsl::true_type
{};
}  // close 'bsl' namespace
#endif

                          // ====================
                          // MoveOnlyExitFunction
                          // ====================

class MoveOnlyExitFunction {
    // Move-only functor that increments the counter supplied on construction
    // when called.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    MoveOnlyExitFunction(const MoveOnlyExitFunction&)     BSLS_KEYWORD_DELETED;
    MoveOnlyExitFunction& operator=(const MoveOnlyExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MoveOnlyExitFunction& operator=(MoveOnlyExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION'.

    // CREATORS
    explicit MoveOnlyExitFunction(int *counter_p);
        // Create an 'MoveOnlyExitFunction' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called,
        // unless this object is in a moved-from state.

    MoveOnlyExitFunction(bslmf::MovableRef<MoveOnlyExitFunction> original);
        // Create a 'MoveOnlyExitFunction' object having the same value as the
        // specified 'original' object by initializing this object's counter
        // pointer to that of the 'original', and setting the 'original' object
        // to the moved-from state by setting its pointer to null.  The
        // behavior is undefined if 'original' is in a moved-from state.

    MoveOnlyExitFunction(
              bslmf::MovableRef<MoveOnlyExitFunctionParam> param);  // IMPLICIT
        // Create a 'MoveOnlyExitFunction' object that increments the same
        // counter as the specified 'param' and set 'param' to a moved-from
        // state.  The behavior is undefined if 'param' is already in a
        // moved-from state.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION'.  The behavior is
        // undefined if this object is in a moved-from state.
};

                       // --------------------
                       // MoveOnlyExitFunction
                       // --------------------
// CLASS METHODS
int MoveOnlyExitFunction::theIncrement()
{
    return IncrementValues::k_MOVE_ONLY_EXIT_FUNCTION;
}

// CREATORS
MoveOnlyExitFunction::MoveOnlyExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

MoveOnlyExitFunction::MoveOnlyExitFunction(
                              bslmf::MovableRef<MoveOnlyExitFunction> original)
: d_counter_p(bslmf::MovableRefUtil::access(original).d_counter_p)
{
    BSLS_ASSERT(bslmf::MovableRefUtil::access(original).d_counter_p != 0);

    bslmf::MovableRefUtil::access(original).d_counter_p = 0;
}

MoveOnlyExitFunction::MoveOnlyExitFunction(
                            bslmf::MovableRef<MoveOnlyExitFunctionParam> param)
: d_counter_p(bslmf::MovableRefUtil::access(param).counterPtr())
{
    BSLS_ASSERT(0 != bslmf::MovableRefUtil::access(param).counterPtr());

    bslmf::MovableRefUtil::access(param).reset();
}

// ACCESSORS
void MoveOnlyExitFunction::operator()() const
{
    BSLS_ASSERT(d_counter_p);

    *d_counter_p += theIncrement();
}

                //  =====================================
                // < TYPE TRAITS -- MoveOnlyExitFunction >
                //  =====================================

#ifndef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
// In case there is no reliable '<type_traits>' header we use 'bsl' a trait to
// mark our class no-throw-move-constructible.
namespace bsl {
template <>
struct is_nothrow_move_constructible<MoveOnlyExitFunction>
: bsl::true_type
{};
}  // close 'bsl' namespace
#endif

                          // ====================
                          // CopyOnlyExitFunction
                          // ====================

class CopyOnlyExitFunction {
    // Copy-only functor (move is disabled) that increments the counter
    // supplied on construction when called.

  private:
    // DATA
    int *d_counter_p;  // held, not owned

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  private:
    // NOT IMPLEMENTED
    CopyOnlyExitFunction& operator=(CopyOnlyExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_COPY_ONLY_EXIT_FUNCTION'.

    // CREATORS
    explicit CopyOnlyExitFunction(int *counter_p);
        // Create an 'CopyOnlyExitFunction' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // We need to define the copy constructor explicitly because we have
    // deleted move-assignment.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    CopyOnlyExitFunction(const CopyOnlyExitFunction&) = default;
#else
    CopyOnlyExitFunction(const CopyOnlyExitFunction& original);
        // Create a 'CopyOnlyExitFunction' instance that increments the same
        // pointer as the 'original' object.
#endif
#endif
//
    // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_COPY_ONLY_EXIT_FUNCTION'.
};

                       // --------------------
                       // CopyOnlyExitFunction
                       // --------------------
// CLASS METHODS
int CopyOnlyExitFunction::theIncrement()
{
    return IncrementValues::k_COPY_ONLY_EXIT_FUNCTION;
}

// CREATORS
CopyOnlyExitFunction::CopyOnlyExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    !defined( BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
CopyOnlyExitFunction::CopyOnlyExitFunction(
                                          const CopyOnlyExitFunction &original)
: d_counter_p(original.d_counter_p)
{
}
#endif

// ACCESSORS
void CopyOnlyExitFunction::operator()() const
{
    BSLS_ASSERT(d_counter_p);

    *d_counter_p += theIncrement();
}

                          // =====================
                          // struct MoveCopyCounts
                          // =====================

struct MoveCopyCounts {
    // Provides public, resettable static counters for move and copy counts.
    // Shared by all counting code for brevity.

    // PUBLIC CLASS DATA
    static unsigned s_copyCount;
    static unsigned s_moveCount;

    // CLASS METHODS
    static void resetCopyMoveCounts();
        // Sets the copy and move counters to zero.
};

                          // ---------------------
                          // struct MoveCopyCounts
                          // ---------------------
// PUBLIC CLASS DATA
unsigned MoveCopyCounts::s_copyCount = 0;
unsigned MoveCopyCounts::s_moveCount = 0;

// CLASS METHODS
void MoveCopyCounts::resetCopyMoveCounts()
{
    s_copyCount = 0;
    s_moveCount = 0;
}

                      // =============================
                      // template MoveCopyExitFunction
                      // =============================

template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
class MoveCopyExitFunction : public MoveCopyCounts {
    // A functor that increments a counter supplied on construction when
    // invoked, and also counts how many times its instances have been copy-
    // or move-constructed.  The template parameters 'MOVE_IS_NOEXCEPT' and
    // 'COPY_IS_NOEXCEPT' determine if the move and copy constructors are
    // 'noexcept', respectively.  This feature is necessary in testing
    // move/copy selection as well as the exception specification of the
    // 'ScopeExit' move constructor.  Note that to simplify the code the
    // counters are shared between *all* instances of the template (we never
    // use more than one instance in a test).

  private:
    // DATA
    int *d_counter_p;  // held, not owned

  private:
    // NOT IMPLEMENTED
    MoveCopyExitFunction& operator=(const MoveCopyExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MoveCopyExitFunction& operator=(MoveCopyExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CLASS METHODS
    static int theIncrement();
        // Return 'IncrementValues::k_MOVE_COPY_EXIT_FUNCTION'.

    // CREATORS
    explicit MoveCopyExitFunction(int *counter_p);
        // Create an 'MoveCopyExitFunction' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called,
        // unless this object is in a moved-from state.

    MoveCopyExitFunction(const MoveCopyExitFunction& original)
                          BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(COPY_IS_NOEXCEPT)
        // Create a 'MoveCopyExitFunction' object having the same value as the
        // specified 'original' object by initializing this object's counter
        // pointer to that of the 'original'.  Also increase 's_copyCount' by
        // one.  The behavior is undefined if 'original' is in a moved-from
        // state.
    : d_counter_p(original.d_counter_p)
    {
        // This function needs to be defined inline in the class body due to a
        // Microsoft Visual C++ bug that prevents the compiler from matching
        // the definition with the declaration with a "C2382: redefinition;
        // different exception specification" error.
        BSLS_ASSERT(bslmf::MovableRefUtil::access(original).d_counter_p != 0);

        ++s_copyCount;
    }

    MoveCopyExitFunction(bslmf::MovableRef<MoveCopyExitFunction> original)
            BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(MOVE_IS_NOEXCEPT);  // IMPLICIT
        // Create a 'MoveCopyExitFunction' object having the same value as the
        // specified 'original' object by initializing this object's counter
        // pointer to that of the 'original', and setting the 'original' object
        // to the moved-from state by setting its pointer to null.  Also
        // increase 's_moveCount' by one.  The behavior is undefined if
        // 'original' is in a moved-from state.  Notice that this move
        // constructor must be 'noexcept', otherwise it will not be selected by
        // 'ScopeExit' to use since we have a copy constructor present.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by
        // 'IncrementValues::k_MOVE_COPY_EXIT_FUNCTION'. The behavior is
        // undefined if this object is in a moved-from state.
};

// TRAITS
#ifndef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
namespace bsl {
template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
struct is_nothrow_move_constructible<MoveCopyExitFunction<MOVE_IS_NOEXCEPT,
                                                          COPY_IS_NOEXCEPT> >
: bsl::integral_constant<bool, MOVE_IS_NOEXCEPT>
{};
}  // close 'bsl' namespace
#endif

                          // --------------------
                          // MoveCopyExitFunction
                          // --------------------
// CLASS METHODS
template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
int MoveCopyExitFunction<MOVE_IS_NOEXCEPT, COPY_IS_NOEXCEPT>::theIncrement()
{
    return IncrementValues::k_MOVE_COPY_EXIT_FUNCTION;
}

// CREATORS
template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
MoveCopyExitFunction<MOVE_IS_NOEXCEPT, COPY_IS_NOEXCEPT>::
MoveCopyExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
MoveCopyExitFunction<MOVE_IS_NOEXCEPT, COPY_IS_NOEXCEPT>::
MoveCopyExitFunction(bslmf::MovableRef<MoveCopyExitFunction> original)
                          BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(MOVE_IS_NOEXCEPT)
: d_counter_p(bslmf::MovableRefUtil::access(original).d_counter_p)
{
    BSLS_ASSERT(bslmf::MovableRefUtil::access(original).d_counter_p != 0);

    bslmf::MovableRefUtil::access(original).d_counter_p = 0;

    ++s_moveCount;
}

// ACCESSORS
template <bool MOVE_IS_NOEXCEPT, bool COPY_IS_NOEXCEPT>
void
MoveCopyExitFunction<MOVE_IS_NOEXCEPT, COPY_IS_NOEXCEPT>::
operator()() const
{
    BSLS_ASSERT(d_counter_p);

    *d_counter_p += theIncrement();
}

typedef MoveCopyExitFunction<false, false> MoveCopyBothNotNoexceptFunctor;
typedef MoveCopyExitFunction<true,  false> MoveNoexceptCopyNotFunctor;
typedef MoveCopyExitFunction<false, true>  CopyNoexceptMoveNotFunctor;
typedef MoveCopyExitFunction<true,  true>  MoveCopyBothNoexceptFunctor;

// ============================================================================
//                Free Function and Stateless Lambda Support
// ============================================================================

int g_counter = 0;
    // Global counter for stateless lambdas and 'void(*)()' functions.

void freeExitFunction()
    // Increment the global counter 'g_counter' by
    // 'IncrementValues::k_FUNCTION_POINTER'.
{
    g_counter += IncrementValues::k_FUNCTION_POINTER;
}

                       // =============================
                       // VerifyExplicitConstructorUtil
                       // =============================

template <class TESTED_TYPE, class PARAM_TYPE>
struct VerifyExplicitConstructorUtil {
    struct LargerThanChar { char d_a[42]; };

    static LargerThanChar testcall(const TESTED_TYPE&);
    static char testcall(...);
        // An overload set that is used to determine if 'EXIT_FUNC_PARAM'
        // converts explicitly to 'TESTED_TYPE'.  See the compile time 'bool'
        // definition of 'k_PASSED' below.
#ifdef BSLS_PLATFORM_CMP_AIX
#pragma  report(disable, "1540-2924")
    // 1540-2924 (W) Cannot pass an argument of non - POD class type "<type>"
    // through ellipsis.  Obviously no argument is passed through ellipsis,
    // because 'testcall' is within 'sizeof', a *non-evaluated* context.
#endif
    static const bool k_PASSED =
        sizeof(testcall(            U_DECLVAL(PARAM_TYPE))) == sizeof(char) &&
        sizeof(testcall(TESTED_TYPE(U_DECLVAL(PARAM_TYPE)))) > sizeof(char);
#ifdef BSLS_PLATFORM_CMP_AIX
#pragma  report(pop)
#endif
#undef U_PARAM
};

             // ================================================
             // VerifyExplicitConstructorUtilTesterImplicitParam
             // ================================================

struct VerifyExplicitConstructorUtilTesterImplicitParam {};

             // ================================================
             // VerifyExplicitConstructorUtilTesterExplicitParam
             // ================================================

struct VerifyExplicitConstructorUtilTesterExplicitParam {};

                   // ===================================
                   // VerifyExplicitConstructorUtilTester
                   // ===================================

struct VerifyExplicitConstructorUtilTester {
    // Implicitly constructible from one type, explicitly from another.  Only
    // for testing, no implementation for member functions on purpose.

    VerifyExplicitConstructorUtilTester(
         const VerifyExplicitConstructorUtilTesterImplicitParam&);  // IMPLICIT

    explicit VerifyExplicitConstructorUtilTester(
                      const VerifyExplicitConstructorUtilTesterExplicitParam&);
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
MoveCopyBothNoexceptFunctor
makeMoveCopyBothNoexceptFunctor(int *counter_p)
    // Create a temporary 'MoveCopyBothNoexceptFunctor' object that increments
    // the specified 'counter_p' for testing move from a temporary on C++11 and
    // later.
{
    return MoveCopyBothNoexceptFunctor(counter_p);
}
#endif

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01

#ifdef BDE_BUILD_TARGET_EXC

///Usage Examples
///--------------
// This section illustrates intended use of this component.
//
///Example 1: Using a Scope Exit Proctor in C++11 or later
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we assume a C++ compiler supporting sufficiently functional
// C++11 or later.  Suppose we are creating a simple database that stores names
// and their associated addresses and we store the names and addresses in two
// separate tables.  While adding data, these tables may fail the insertion,
// in which case we need to roll back the already inserted data, such as if we
// inserted the address first, we need to remove it if insertion of the
// associated name fails.
//
// First, we emulate our database access with the following simple functions:
//..
int removedAddressId = 0;
    int insertAddress(const char *address)
    {
        (void)address;
        return (0 == removedAddressId ? 2 : 3);
    }

    int insertCustomer(const char *name, int addressId)
    {
        (void)name;
        (void)addressId;
        if (0 == removedAddressId) throw 5; // Simulate failure once
        return 7;
    }

    void removeAddress(int id)
    {
        removedAddressId = id;
    }
//..
// Next, we draw up our complex, customer-creating function signature:
//..
#ifdef BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE
    int addCustomer11(const char *name, const char *address)
    {
//..
// Then we implement it, starting by inserting the address:
//..
        const int addressId = insertAddress(address);
//..
// Our dummy function returns 42, indicating successful address insertion.
//
// Next, we create a proctor to remove the already inserted address if the name
// insertion fails:
//..
        auto addressProctor = bdlb::ScopeExitUtil::makeScopeExit(
                                           [=](){ removeAddress(addressId); });
//..
// Then, we attempt to insert the name:
//..
        const int custId = insertCustomer(name, addressId);
//..
// As our dummy 'insertCustomer' function will fail first time by throwing an
// exception (when'removedAddressId' is zero) we exit this function to the
// caller's error handling 'catch' clause.  While exiting the function due to
// the exception, the local stack is unwound.  The non-trivial destructors of
// local variables are invoked (in the opposite order of their creation).  In
// this case, the destructor of 'addressProctor' invokes its exit function,
// saving our non-zero 'addressId' value into the global 'removedAddressId'
// variable.
//
// On the second call to this function, because 'removedAddressId' is now
// non-zero, 'insertCustomer' will not fail, and we continue execution here.
//
// Next, if the insertion succeeded we are done, so we need to release the
// proctor to make the address permanent, after which we can return the ID:
//..
        addressProctor.release();

        return custId;                                                // RETURN
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.
#endif // BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE

///Example 2: Using a Scope Exit Guard in C++03
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are in the same situation as in the C++11 or later example, but
// we have to create a solution that supports C++03 as well.
//
// First, we have to hand-craft a functor that calls 'removeAddress' with a
// given ID because C++03 does not support lambdas:
//..
    class RemoveAddress {
        int d_id;  // the identifier of the address (row) to remove

      public:
        explicit RemoveAddress(int id)
        : d_id(id)
        {
        }

        void operator()() const
        {
            removeAddress(d_id);
        }
    };
//..
// Then, we implement the add customer function for C++03:
//..
    int addCustomer03(const char *name, const char *address)
    {
        const int addressId = insertAddress(address);
//..
// The code is almost the same code as was in 'addCustomer11' (the
// implementation that requires sufficiently functional C++11 or later
// platform), except for the proctor variable definition.
//
// Next, we define the proctor variable with an explicitly spelled out type
// (that uses the functor type template argument), and a functor object
// initialized with the identifier of the address to remove:
//..
        bdlb::ScopeExit<RemoveAddress> addrProctor((RemoveAddress(addressId)));
//..
// Notice the extra parentheses we had to use to avoid "the most vexing parse"
// (https://en.wikipedia.org/wiki/Most_vexing_parse) issue.  Since we are in
// C++03, we cannot use (curly) brace initialization to avoid that issue.
//
// Now, we can complete the rest of the 'addCustomer03', which is exactly the
// same as the corresponding part of the 'addCustomer11' variant:
//..
        const int custId = insertCustomer(name, addressId);
        addrProctor.release();

        return custId;                                                // RETURN
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.

/// Example 3: Unknown Exit Function Type In C++03
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we decide not to write a functor class for removing an address
// but use the function itself directly with 'bdlf::BindUtil::bind' and that
// way keep the roll-back-code near the point of use like lambdas allow us in
// C++11 and later, albeit with a less elegant syntax.

// See introduction with direct call to the bind expression in the USAGE
// EXAMPLE test case, in 'main', then return reading here.

// Next, we create yet another customer adding function that differs only in
// its proctor definition from the 'addCustomer11' variant:
//..
    int addCustomerAny(const char *name, const char *address)
    {
        const int addressId = insertAddress(address);
//..
// Because we do not know the type of our exit function (it is "some functor
// object of some type", created by 'bind') we have to use the 'bsl::function'
// based 'bdlb::ScopeExitAny':
//..
        bdlb::ScopeExitAny addressProctor(bdlf::BindUtil::bind(&removeAddress,
                                                               addressId));
//..
// Consult {C++03 Restrictions When Exit Function Type Is Unknown} to be aware
// what additional runtime costs this more compact code has compared to a
// "hand made" functor with a known type.
//
// Note that since we have to take the address of a function to create the
// 'bind'-expression-functor we cannot use this format with standard library
// functions (unless taking their address is explicitly allowed by the C++
// standard), and if 'removeAddress' were an overloaded function the code would
// not compile as the compiler would not know which address we want.
//
// The rest of the function is the same as before:
//..
        const int custId = insertCustomer(name, addressId);
        addressProctor.release();

        return custId;                                                // RETURN
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.

///Example 4: Using the Scope Exit Proctor Macro
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have to create portable code that will compile with C++03 as well
// as C++11 and later compilers.  We also want our code to use the more
// efficient type-deducing 'auto' with factory-method variant when compiled
// with a sufficiently functional C++11 or later compiler, and only fall back
// to the slower 'bdlb::ScopeExitAny' solution on C++03 compilers.
//
// We still need to use either functor ('RemoveAddress' in our examples) or a
// bind expression for the exit function because C++03 has no lambdas,
// therefore our portable code cannot use lambdas.  But we *can* choose the
// easy-to-use 'BDLB_SCOPEEXIT_PROCTOR' macro and not sprinkle the add customer
// function with '#ifdef' to see which proctor definition to use.
//
// To keep things simple this component provides a single proctor macro
// instead of two macro names to remember (one for the case case when the type
// of the exit function is known and one when it isn't).  In case the exit
// function name is known we can just directly use
// 'bdlb::ScopeExit< --ExitFunctionType-- >' on any compiler.
//
// First, we start the add customer function as usual:
//..
    int addCustomerMacro(const char *name, const char *address)
    {
        const int addressId = insertAddress(address);
//..
// Then, we define the proctor using a bind expression and the macro:
//..
        BDLB_SCOPEEXIT_PROCTOR(proctor, bdlf::BindUtil::bind(&removeAddress,
                                                             addressId));
//..
// Alternatively, we could have also written a functor and write the shorter
// 'BDLB_SCOPEEXIT_PROCTOR(proctor, RemoveAddress(addressId))' for the proctor.
//
// The rest of the function is the same as before:
//..
        const int custId = insertCustomer(name, addressId);
        proctor.release();

        return custId;                                                // RETURN
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.

#endif // BDE_BUILD_TARGET_EXC

bsl::ostringstream outStream(&bslma::NewDeleteAllocator::singleton());

static bsl::string outStreamContent()
{
    bsl::string rv(outStream.str());
    outStream.str("");
    outStream.clear();

    return rv;
}

///Example 5: Using the Scope Exit Guard Macro
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are writing a printing system that is capable of printing
// out dynamic data structures that can contain numbers, strings, arrays, maps,
// etc.  When printing out data we often have to print delimiters and find
// that it is really easy to forget to print the closing the delimiter.  So we
// look for a simple way to automate them.  We decide we don't want to change
// the printing of the opening delimiters, just have a way to automate the
// printing of the close delimiters without worrying about early returns or
// 'break', 'continue', or other control flow changes.
//
// First, we create a functor type that prints closing delimiters:
//..
    class CloseDelimPrinter {
        const char *d_closingChars_p;  // held, not owned

      public:
        explicit CloseDelimPrinter(const char *s)
        : d_closingChars_p(s)
        {
        }

        void operator()() const
        {
            outStream << d_closingChars_p; // To a fixed stream for brevity
        }
    };
//..
// Then, we can use the above functor and a scope exit guard to automate
// closing of delimiters in the printing functions:
//..
    void printTemplateWithArgs(
                             const bsl::string_view&              templateName,
                             const bsl::vector<bsl::string_view>& args)
    {
//..
// Next, we can move the printing of the opening delimiter and the closing one
// near each other in code, so it is clearly visible if an opened delimiter is
// closed:
//..
        outStream << templateName << '<';
        BDLB_SCOPEEXIT_GUARD(CloseDelimPrinter(">"));
//..
// The macro works in C++03 and C++11 and later, gives the guard variable a
// unique (but unspecified) name, adds an extra set of parentheses to take care
// of "the most vexing parse" and suppresses unused variable compiler warnings.
// The name for the guard variable created is unspecified.  Because this is a
// guard meaning we do not need to call 'release()' (unlike a proctor),
// therefore the name is unimportant.
//
// Now, we can just print what goes inside the delimiters, and we are done:
//..
        if (args.empty()) {
            // Safe to just return, the guard takes care of closing the '<'
            return;                                                   // RETURN
        }

        typedef bsl::vector<bsl::string_view>::const_iterator Cit;

        Cit cit = args.begin();
        outStream << *cit;  // Print first argument
        ++cit;

        for (;cit != args.end(); ++cit) {
            outStream << ", " << *cit;  // Print subsequent argument
        }

        const bsl::string_view last = *(args.end() - 1);
        if (last.back() == '>') {
            outStream << ' ';
        }
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.

// BDE_VERIFY pragma: pop
// BDE_VERIFY pragma: pop
//

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: No memory is leaked.

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;
#ifdef BDE_BUILD_TARGET_EXC
///Example 1: Using a Scope Exit Proctor in C++11 or later
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we assume a C++ compiler supporting sufficiently functional
// C++11 or later.  Suppose we are creating a simple database that stores names
// and their associated addresses and we store the names and addresses in two
// separate tables.  While adding data, these tables may fail the insertion,
// in which case we need to roll back the already inserted data, such as if we
// inserted the address first, we need to remove it if insertion of the
// associated name fails.

// See the USAGE EXAMPLE section just before 'main' for type and function
// definitions, then return here for the assertions.

#ifdef BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE
//..
// Now we can verify that a first attempt to add a customer fails with the
// "right" exception and that 'removedAddressId' is the expected value:
//..
    bool seenException = false;
    try {
        addCustomer11("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
    }
    catch (int exceptionValue) {
        ASSERT(5 == exceptionValue);
        seenException = true;
    }
    ASSERT(seenException);
    ASSERT(2 == removedAddressId);
//..
// Finally we verify that calling 'addCustomer11' again succeeds with the right
// identifier returned, and that 'removedAddressId' does not change:
//..
    ASSERT(7 == addCustomer11("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
    ASSERT(2 == removedAddressId);
//..
#else
        bool seenException = false;
#endif // BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE

///Example 2: Using a Scope Exit Guard in C++03
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are in the same situation as in the C++11 or later example, but
// we have to create a solution that supports C++03 as well.

// See the USAGE EXAMPLE section just before 'main' for type and function
// definitions, then return here for the assertions.

// Finally, we can verify that both during the failing first attempt to add a
// customer to our imaginary database and the successful second attempt the
// 'RemoveAddress' functor based proctor works just as well as the lambda based
// 'addCustomer11' variant did:
//..
    removedAddressId = 0;
    seenException = false;
    try {
        addCustomer03("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
    }
    catch (int exceptionValue) {
        ASSERT(5 == exceptionValue);
        seenException = true;
    }
    ASSERT(seenException);
    ASSERT(2 == removedAddressId);

    ASSERT(7 == addCustomer03("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
    ASSERT(2 == removedAddressId);
//..


/// Example 3: Unknown Exit Function Type In C++03
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we decide not to write a functor class for removing an address
// but use the function itself directly with 'bdlf::BindUtil::bind' and that
// way keep the roll-back-code near the point of use like lambdas allow us in
// C++11 and later, albeit with a less elegant syntax.
//
// First, we design our bind expression as
// 'bdlf::BindUtil::bind(&removeAddress, addressId)'.
//
// Then, we can even try it to see if it works as intended by calling the
// result of a bind expression using a constant for the address ID:
//..
    removedAddressId = 0;
    bdlf::BindUtil::bind(&removeAddress, 11)();
    ASSERT(11 == removedAddressId);
//..
// Notice the subtle '()' after the bind expression.  We immediately call it
// after creating it (then destroy it).  We have to do it this way.  We have no
// idea what its type is so we cannot make a variable for it.

// See the USAGE EXAMPLE section just before 'main' the function definition,
// then return here for the assertions.

// Finally, we can verify that 'bind' and 'bdlb::ScopeExitAny' based proctor
// works just as well:
//..
    removedAddressId = 0;
    seenException = false;
    try {
        addCustomerAny("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
    }
    catch (int exceptionValue) {
        ASSERT(5 == exceptionValue);
        seenException = true;
    }
    ASSERT(seenException);
    ASSERT(2 == removedAddressId);

    ASSERT(7 == addCustomerAny("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
    ASSERT(2 == removedAddressId);
//..

///Example 4: Using the Scope Exit Proctor Macro
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have to create portable code that will compile with C++03 as well
// as C++11 and later compilers.  We also want our code to use the more
// efficient type-deducing 'auto' with factory-method variant when compiled
// with a sufficiently functional C++11 or later compiler, and only fall back
// to the slower 'bdlb::ScopeExitAny' solution on C++03 compilers.

// See the USAGE EXAMPLE section just before 'main' the function definition,
// then return here for the assertions.

// Finally, we can verify the easy proctor with the now customary code:
//..
    removedAddressId = 0;
    seenException = false;
    try {
        addCustomerMacro("Quibi", "6555 Barton Ave, Los Angeles, CA, 90038");
    }
    catch (int exceptionValue) {
        ASSERT(5 == exceptionValue);
        seenException = true;
    }
    ASSERT(seenException);
    ASSERT(2 == removedAddressId);

    ASSERT(7 == addCustomerMacro("Plum Inc.", "1i Imagine Sq, Coppertin, CA"));
    ASSERT(2 == removedAddressId);
//..
#endif // BDE_BUILD_TARGET_EXC

///Example 5: Using the Scope Exit Guard Macro
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we are writing a printing system that is capable of printing
// out dynamic data structures that can contain numbers, strings, arrays, maps,
// etc.  When printing out data we often have to print delimiters and find
// that it is really easy to forget to print the closing the delimiter.  So we
// look for a simple way to automate them.  We decide we don't want to change
// the printing of the opening delimiters, just have a way to automate the
// printing of the close delimiters without worrying about early returns or
// 'break', 'continue', or other control flow changes.

// See the USAGE EXAMPLE section just before 'main' for type and function
// definitions, then return here for the assertions.

// Finally, we can print some templates and verify that the argument
// delimiters are closed:
//..
    bsl::vector<bsl::string_view> targs;
    printTemplateWithArgs("TypeList", targs);
    ASSERT(outStreamContent() == "TypeList<>");

    targs.push_back("bsl::string_view");
    printTemplateWithArgs("bsl::vector", targs);
    ASSERT(outStreamContent() == "bsl::vector<bsl::string_view>");

    targs.push_back("bsl::vector<bsl::string_view>");
    printTemplateWithArgs("bsl::unordered_map", targs);
    ASSERTV(outStreamContent() ==
       "bsl::unordered_map<bsl::string_view, bsl::vector<bsl::string_view> >");
//..
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // CLASS TEMPLATE ARGUMENT DEDUCTION
        //
        // Concerns:
        //: 1 When CTAD is supported:
        //:   1 Code using no template arguments for 'bdlb::ScopeExit' compiles
        //:   2 The expected type is created.
        //:   3 The created object works as expected.
        //
        // Plan:
        //: 1 Create scope exit objects from all supported types.
        //: 2 Verify with 'bsl::is_same' they are the expected type.
        //: 3 Verify they execute the given exit function by using counters.
        //
        // Testing:
        //   CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCLASS TEMPLATE ARGUMENT DEDUCTION"
                          << "\n=================================" << endl;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD

#define U_ASSERT_SCOPE_EXIT_TYPE(Type) \
    ASSERT((bsl::is_same_v<decltype(X), const bdlb::ScopeExit<Type>>))

        if (veryVerbose) cout << "Function pointer\n";
        g_counter = 0;
        {
            const bdlb::ScopeExit X(freeExitFunction);
            U_ASSERT_SCOPE_EXIT_TYPE(void (*)());
        }
        ASSERTV(g_counter, IncrementValues::k_FUNCTION_POINTER,
                IncrementValues::k_FUNCTION_POINTER == g_counter);

        if (veryVerbose) cout << "Non-throwing functors\n";

#define U_CTAD_NORMAL_VERIFY(ExitFuncType) do {             \
    counter = 0;                                            \
    {                                                       \
        const bdlb::ScopeExit X((ExitFuncType(&counter)));  \
        U_ASSERT_SCOPE_EXIT_TYPE(ExitFuncType);             \
    }                                                       \
    ASSERTV(counter, ExitFuncType::theIncrement(),          \
            ExitFuncType::theIncrement() == counter);       \
    } while(false)

        int counter = 0;
        U_CTAD_NORMAL_VERIFY(ExitFunction);
        U_CTAD_NORMAL_VERIFY(MoveOnlyExitFunction);
        U_CTAD_NORMAL_VERIFY(CopyOnlyExitFunction);
        U_CTAD_NORMAL_VERIFY(MoveCopyBothNotNoexceptFunctor);
        U_CTAD_NORMAL_VERIFY(MoveNoexceptCopyNotFunctor);
        U_CTAD_NORMAL_VERIFY(CopyNoexceptMoveNotFunctor);
        U_CTAD_NORMAL_VERIFY(MoveCopyBothNoexceptFunctor);
#undef U_CTAD_NORMAL_VERIFY

#ifdef BDE_BUILD_TARGET_EXC
        if (veryVerbose) cout << "Throwing functors\n";
        bool exceptionSeen = false;
#define U_CTAD_THROWS_VERIFY(ExitFuncType, excVal) do {            \
    counter = 0;                                                   \
    try {                                                          \
        const bdlb::ScopeExit X((ExitFuncType(&counter)));         \
        static_assert(                                             \
            bsl::is_same_v<decltype(X),                            \
                           const bdlb::ScopeExit<ExitFuncType>>);  \
    }                                                              \
    catch (ExceptionValues::Enum value) {                          \
        exceptionSeen = true;                                      \
        ASSERTV(value, ExceptionValues::excVal);                   \
    }                                                              \
    ASSERTV(counter, ExitFuncType::theIncrement(),                 \
            ExitFuncType::theIncrement() == counter);              \
    ASSERT(exceptionSeen);                                         \
    } while(false)

        U_CTAD_THROWS_VERIFY(CopyThrowsExitFunction,
                                k_COPY_THROWS_EXIT_FUNCTION);
        U_CTAD_THROWS_VERIFY(BothThrowExitFunction,
                                k_COPY_THROWS_EXIT_FUNCTION);
        U_CTAD_THROWS_VERIFY(MoveThrowsExitFunction,
                                k_MOVE_THROWS_EXIT_FUNCTION);
#undef U_CTAD_THROWS_VERIFY
#endif // BDE_BUILD_TARGET_EXC

        if (veryVerbose) cout << "Stateless lambda\n";
        g_counter = 0;
        {
            const bdlb::ScopeExit X{
                [] { g_counter += IncrementValues::k_STATELESS_LAMBDA; } };
        }
        ASSERTV(g_counter, IncrementValues::k_STATELESS_LAMBDA,
                IncrementValues::k_STATELESS_LAMBDA == g_counter);
        {
            auto l = [] { g_counter += IncrementValues::k_STATELESS_LAMBDA; };
            const bdlb::ScopeExit X{ l };
            ASSERT((bsl::is_same_v<decltype(X),
                                   const bdlb::ScopeExit<decltype(l)>>));
        }
        ASSERTV(g_counter, IncrementValues::k_STATELESS_LAMBDA,
                IncrementValues::k_STATELESS_LAMBDA * 2 == g_counter);

        if (veryVerbose) cout << "Stateful lambda\n";
        counter = 0;
        {
            const bdlb::ScopeExit X{
                [&counter] { counter += IncrementValues::k_STATEFUL_LAMBDA; }
            };
        }
        ASSERTV(counter, IncrementValues::k_STATEFUL_LAMBDA,
                IncrementValues::k_STATEFUL_LAMBDA == counter);
        {
            auto l = [&counter] {
                counter += IncrementValues::k_STATEFUL_LAMBDA;
            };
            const bdlb::ScopeExit X{ l };
            ASSERT((bsl::is_same_v<decltype(X),
                                   const bdlb::ScopeExit<decltype(l)>>));
        }
        ASSERTV(counter, IncrementValues::k_STATEFUL_LAMBDA,
                IncrementValues::k_STATEFUL_LAMBDA * 2 == counter);

#undef U_ASSERT_SCOPE_EXIT_TYPE
#else // BSLS_COMPILERFEATURES_SUPPORT_CTAD
        cout << "Class template argument deduction is not supported" << endl;
#endif
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // GUARD MACRO
        //
        // Concerns:
        //: 1 'BDLB_SCOPEEXIT_GUARD' creates a guard variable with a unique
        //:   name (using the proctor macro).
        //:
        //: 2 The guard is created using the specified exit function.
        //
        // Plan:
        //: 1 We turn the result of 'BDLB_SCOPEEXIT_GUARD' invocations into
        //:   string literals and verify that:
        //:   1 The "invariable" parts are as expected (beginning and end).
        //:   2 The unique part of the variable name is unique.
        //:   2 The unique part of the variable name is a valid identifier.
        //
        // Testing:
        //   BDLB_SCOPEEXIT_GUARD
        // --------------------------------------------------------------------

        if (verbose) cout << "\nGUARD MACRO"
                          << "\n===========" << endl;

        int counter = 0;
        {
            BDLB_SCOPEEXIT_GUARD(ExitFunction(&counter));
        }
        ASSERTV(counter, ExitFunction::theIncrement(),
                ExitFunction::theIncrement() == counter);

#define U_STRINGIFY2(totext) #totext
#define U_STRINGIFY(totext) U_STRINGIFY2(totext)
        const bsl::string_view guardMacroText(
                                  U_STRINGIFY(BDLB_SCOPEEXIT_GUARD(exitFunc)));

#if BSLA_MAYBE_UNUSED_IS_ACTIVE
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
#define U_MAYBE_UNUSED  "$[[$maybe_unused$]]"
#else
#define U_MAYBE_UNUSED  "$__attribute__$($($__unused__$)$)"
#endif
#else
#define U_MAYBE_UNUSED
#endif

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
        const bsl::string_view EXPECTED(
            U_MAYBE_UNUSED
            "$const auto bdlbScopeExitGuard_#${$"
                         "BloombergLP$::$bdlb$::$ScopeExitUtil$::$"
                                          "makeScopeExit$($exitFunc$)$}$");
#else
        const bsl::string_view EXPECTED(
            U_MAYBE_UNUSED
            "$const BloombergLP$::$bdlb$::$ScopeExitAny "
                                     "bdlbScopeExitGuard_#$($($exitFunc$)$)$");
#endif
#undef U_MAYBE_UNUSED

        ASSERTV(guardMacroText, EXPECTED,
                testMacroText(EXPECTED, guardMacroText));

        // Each invocation (as long as on separate line) should create a
        // different variable name:

        const bsl::string_view secondGuardText(
                                  U_STRINGIFY(BDLB_SCOPEEXIT_GUARD(exitFunc)));

        ASSERTV(secondGuardText, EXPECTED,
                testMacroText(EXPECTED, secondGuardText));

        ASSERTV(guardMacroText, secondGuardText,
                guardMacroText != secondGuardText);

#undef U_STRINGIFY
#undef U_STRINGIFY2
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PROCTOR MACRO
        //
        // Concerns:
        //: 1 'BDLB_SCOPEEXIT_PROCTOR' creates the proctor variable using the
        //:    type appropriate for the build mode (C++03 or sufficiently
        //:    functional C++11 or later).
        //:
        //: 2 The proctor is created using the specified variable name.
        //:
        //: 3 The proctor is created using the specified exit function.
        //
        // Plan:
        //: 1 Create a proctor using the 'BDLB_SCOPEEXIT_PROCTOR' macro and
        //:   make sure the type of the created object is either
        //:   'bdlb::ScopeExitAny' (in C++03), or a type deduced from the
        //:   return type of the factory function (in C++11 or later).
        //
        // Testing:
        //   BDLB_SCOPEEXIT_PROCTOR
        // --------------------------------------------------------------------

        if (verbose) cout << "\nPROCTOR MACRO"
                          << "\n=============" << endl;

        int counter = 0;
        {
            BDLB_SCOPEEXIT_PROCTOR(proctor, ExitFunction(&counter));

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
            bdlb::ScopeExit<ExitFunction> *proctorPtr = &proctor;
            (void)proctorPtr; // if the code compiles - the type is correct
#else
            bdlb::ScopeExitAny *proctorPtr = &proctor;
            (void)proctorPtr; // if the code compiles - the type is correct
#endif
        }
        ASSERTV(counter, ExitFunction::theIncrement(),
                ExitFunction::theIncrement() == counter);

#define U_STRINGIFY2(totext) #totext
#define U_STRINGIFY(totext) U_STRINGIFY2(totext)
        const bsl::string_view proctorMacroText(
                       U_STRINGIFY(BDLB_SCOPEEXIT_PROCTOR(varName, exitFunc)));
#undef U_STRINGIFY
#undef U_STRINGIFY2

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
        const bsl::string_view EXPECTED(
            "$auto varName${$BloombergLP$::$bdlb$::$ScopeExitUtil$::$"
                                          "makeScopeExit$($exitFunc$)$}$");
#else
        const bsl::string_view EXPECTED(
            "$BloombergLP$::$bdlb$::$ScopeExitAny varName$(($exitFunc$))$");
#endif

        ASSERTV(proctorMacroText, EXPECTED,
                testMacroText(EXPECTED, proctorMacroText));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // SCOPEEXITANY
        //
        // Concerns:
        //: 1 'ScopeExitAny' can store and properly execute an exit function
        //:   that is not move-only (due to 'function' limitations).
        //
        // Plan:
        //: 1 Create 'bdlb::ScopeExitAny' objects using a different argument
        //:   types for the constructor ('ExitFunction'/'ExitFunctionParam').
        //:
        //: 2 Verify that the object invokes the exit function upon its
        //:   destruction *once*.
        //
        // Testing:
        //   ScopeExitAny
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSCOPEEXITANY"
                             "\n============" << endl;

        int counter = 0;
        {
            bdlb::ScopeExitAny mObj((ExitFunction(&counter)));
        }
        ASSERTV(counter, ExitFunction::theIncrement(),
                ExitFunction::theIncrement() == counter);

        counter = 0;
        {
            bdlb::ScopeExitAny mObj((ExitFunctionParam(&counter)));
        }
        ASSERTV(counter, ExitFunctionParam::theIncrement(),
                ExitFunctionParam::theIncrement() == counter);

        counter = 0;
        {
            bdlb::ScopeExitAny mObj((CopyOnlyExitFunction(&counter)));
        }
        ASSERTV(counter, CopyOnlyExitFunction::theIncrement(),
                CopyOnlyExitFunction::theIncrement() == counter);

        using bdlb::ScopeExitAny;

        counter = 0;
        {
            ScopeExitAny mObj((MoveCopyBothNotNoexceptFunctor(&counter)));
        }
        ASSERTV(counter, CopyNoexceptMoveNotFunctor::theIncrement(),
                CopyNoexceptMoveNotFunctor::theIncrement() == counter);

        counter = 0;
        {
            ScopeExitAny mObj((MoveNoexceptCopyNotFunctor(&counter)));
        }
        ASSERTV(counter, MoveNoexceptCopyNotFunctor::theIncrement(),
                MoveNoexceptCopyNotFunctor::theIncrement() == counter);

        counter = 0;
        {
            ScopeExitAny mObj((CopyNoexceptMoveNotFunctor(&counter)));
        }
        ASSERTV(counter, CopyNoexceptMoveNotFunctor::theIncrement(),
                CopyNoexceptMoveNotFunctor::theIncrement() == counter);

        counter = 0;
        {
            ScopeExitAny mObj((MoveCopyBothNoexceptFunctor(&counter)));
        }
        ASSERTV(counter, MoveCopyBothNoexceptFunctor::theIncrement(),
                MoveCopyBothNoexceptFunctor::theIncrement() == counter);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // FACTORY FUNCTION
        //
        // Concerns:
        //: 1 The factory function allocates no memory.
        //:
        //: 2 The factory function creates a 'ScopeExit' object with the
        //:   intended exit function.
        //:
        //: 3 Move only parameter and exit types work.
        //:
        //: 4 The factory function perfect forwards the argument and it returns
        //    a temporary that will be moved by the compiler.
        //
        // Plan:
        //: 1 Use an allocator monitor to verify that no memory is allocated
        //:   during this test.
        //:
        //: 2 Create a 'bdlb::ScopeExit' object using 'bdlb::ScopeExitUtil',
        //:   with a lambda for an exit function.  The lambda increments a
        //:   counter used for later verification.
        //:
        //: 3 Verify that the object returned calls the intended exit function
        //:   upon its destruction.  Verify the exact expected counter value to
        //:   ensure that the lambda was called only once.
        //:
        //: 4 Using 'MoveOnlyExitFunction' verify that such types work.
        //:
        //: 5 Using 'MoveNoexceptCopyNotFunctor' verify that the factory
        //:   function does perfect forwarding of its argument to the
        //:   'bdlb::ScopeExit' constructor.
        //
        // Testing:
        //   ScopeExitUtil::makeScopeExit()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nFACTORY FUNCTION"
                             "\n================" << endl;

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
        int counter = 0;
        {
            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            auto mObj = bdlb::ScopeExitUtil::makeScopeExit(
                                                          [&](){ ++counter; });
            ASSERT(dam.isTotalSame());
        }
        ASSERTV(counter, 1 == counter);

        // Verifying that move-only arguments and functor works
        counter = 0;
        {
            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            auto mObj = bdlb::ScopeExitUtil::makeScopeExit(
                                               MoveOnlyExitFunction(&counter));
            ASSERT(dam.isTotalSame());
        }
        ASSERTV(counter, MoveOnlyExitFunction::theIncrement(),
                MoveOnlyExitFunction::theIncrement() == counter);

        // Verifying that we perfect forward to the converting constructor
        // which then chooses a 'noexcept' move over copy.  (The second move of
        // the return value is eliminated by copy elision.)
        counter = 0;
        {
            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            const unsigned& moveCount = MoveCopyCounts::s_moveCount;
            const unsigned& copyCount = MoveCopyCounts::s_copyCount;

            MoveCopyCounts::resetCopyMoveCounts();
            auto mObj = bdlb::ScopeExitUtil::makeScopeExit(
                                         MoveNoexceptCopyNotFunctor(&counter));
            ASSERTV(moveCount, copyCount, moveCount == 1 && copyCount == 0);
            ASSERT(dam.isTotalSame());
        }
        ASSERTV(counter, MoveNoexceptCopyNotFunctor::theIncrement(),
                MoveNoexceptCopyNotFunctor::theIncrement() == counter);
#else
        if (verbose) cout << "Test is skipped on C++03 platforms." << endl;
#endif // BDLB_SCOPEEXIT_USES_MODERN_CPP
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //
        // Concerns:
        //: 1 Move constructing a 'ScopeExit' object from a source 'ScopeExit'
        //:   object ensures that the exit function will only be called by the
        //:   target object, and not the source.
        //:
        //: 2 If the 'EXIT_FUNC' type is movable it is moved, otherwise copied.
        //:
        //: 3 Move-only 'EXIT_FUNC' types are supported by the move
        //:   constructor.
        //:
        //: 4 Concern 1 is fulfilled even if 'EXIT_FUNC' has no move support,
        //:   only copy, by 'release()'-ing the source of the move.
        //:
        //: 5 The move operation is 'noexcept' in sufficiently function C++11
        //:   or later  when either of the move- or copy-constructor is
        //:   'noexcept'.  In C++03 only the pseudo move constructor can be
        //:   tested for no exceptions as we have an explicit trait for that.
        //
        // Plan:
        //: 1 Use 3 kind of verifiable exit functions that support: move-only,
        //:   copy-only, and both move and copy.  Repeat the following for each
        //:   exit function type:
        //:   1 Create a source 'ScopeExit' object for the move.
        //:
        //:   2 Move-construct a second 'ScopeExit' object from the source.
        //:
        //:   3 Verify that the exit function was not called during the move.
        //:
        //:   4 Destroy the move target and verify that the exit function was
        //:     called once during its destruction.
        //:
        //:   5 Destroy the source object next and verify that the exit
        //:     function is not called during its destruction.
        //:
        //:   6 Repeat with the destruction order reversed.  Use 'ManagedPtr'
        //:     to control the destruction order (as it works in C++03 as well,
        //:     unlike 'unique_ptr').
        //:
        //: 2 For the 'EXIT_FUNC' type that supports both move and copy verify
        //:   that a move takes place and no copy during the explicit move.
        //:   (We also verify the implicit move from temporary in C++11 or
        //:   later if sufficiently functional r-value references are present.)
        //:
        //: 3 The 'noexcept' operator is used, if present, to verify the
        //:   exception specification.
        //
        // Testing:
        //   ScopeExit(bslmf::MovableRef<ScopeExit> original)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE CONSTRUCTOR"
                             "\n================" << endl;

#define U_MAKE_MANAGED bslma::ManagedPtrUtil::makeManaged<TestedScopeExitType>
    // Shorter, more readable lines

        int counter = 0;
        {
            typedef MoveOnlyExitFunction                TestedExitFuncType;
            typedef bdlb::ScopeExit<TestedExitFuncType> TestedScopeExitType;

            {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TestedScopeExitType source((TestedExitFuncType(&counter)));
#else
                // C++03 cannot move from an unnamed temporary
                TestedExitFuncType  exitFuncParam(&counter);
                TestedScopeExitType source(MoveUtil::move(exitFuncParam));
#endif
                {
                    const TestedScopeExitType moveTarget(
                                                       MoveUtil::move(source));
                    ASSERTV(counter, 0 == counter);
                }
                ASSERTV(counter, TestedExitFuncType::theIncrement(),
                       TestedExitFuncType::theIncrement() == counter);
            }
                ASSERTV(counter, TestedExitFuncType::theIncrement(),
                       TestedExitFuncType::theIncrement() == counter);

            counter = 0;
            {
                typedef bslma::ManagedPtr<TestedScopeExitType>
                                                            TestedScopeExitPtr;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TestedScopeExitPtr source(U_MAKE_MANAGED(
                                                TestedExitFuncType(&counter)));
#else
                // C++03 cannot move from an unnamed temporary
                TestedExitFuncType exitFuncParam(&counter);
                TestedScopeExitPtr source(
                                U_MAKE_MANAGED(MoveUtil::move(exitFuncParam)));
#endif
                TestedScopeExitPtr moveTarget(
                                      U_MAKE_MANAGED(MoveUtil::move(*source)));
                ASSERTV(counter, 0 == counter);

                source.reset();
                ASSERTV(counter, 0 == counter);

                moveTarget.reset();
                ASSERTV(counter, TestedExitFuncType::theIncrement(),
                       TestedExitFuncType::theIncrement() == counter);
            }
            ASSERTV(counter, TestedExitFuncType::theIncrement(),
                    TestedExitFuncType::theIncrement() == counter);
        }

        counter = 0;
        {
            typedef MoveNoexceptCopyNotFunctor          TestedExitFuncType;
            typedef bdlb::ScopeExit<TestedExitFuncType> TestedScopeExitType;

            const unsigned& moveCount = MoveCopyCounts::s_moveCount;
            const unsigned& copyCount = MoveCopyCounts::s_copyCount;

            {
                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitType source((TestedExitFuncType(&counter)));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // C++11 or later moves the unnamed temporary value, no copy.
                // It may even eliminate the move and copy altogether.
#else
                ASSERTV(moveCount, copyCount,
                        moveCount == 0 && copyCount == 1);
                // C++03 will copy here because it has no real r-value
                // references to detect that our nameless temporary is an
                // r-value.
#endif
                {
                    MoveCopyCounts::resetCopyMoveCounts();
                    TestedScopeExitType moveTarget(MoveUtil::move(source));
                    ASSERTV(moveCount, copyCount,
                            moveCount == 1 && copyCount == 0);
                    // Here a move should happen regardless of C++ version
                    // because we explicitly change the type of the constructor
                    // argument to an r-value type (simulated or real).
                    ASSERTV(counter, 0 == counter);
                }
                ASSERTV(counter, TestedExitFuncType::theIncrement(),
                       TestedExitFuncType::theIncrement() == counter);
            }
            ASSERTV(counter, TestedExitFuncType::theIncrement(),
                    TestedExitFuncType::theIncrement() == counter);

            counter = 0;
            {
                typedef bslma::ManagedPtr<TestedScopeExitType>
                                                            TestedScopeExitPtr;

                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitPtr source(U_MAKE_MANAGED(
                                                TestedExitFuncType(&counter)));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // C++11 or later moves the unnamed temporary value, no copy.
                // It may even eliminate the move and copy altogether.
#else
                ASSERTV(moveCount, copyCount,
                        moveCount == 0 && copyCount == 1);
                // C++03 will copy here because it has no real r-value
                // references to detect that our nameless temporary is an
                // r-value.
#endif
                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitPtr moveTarget(
                                      U_MAKE_MANAGED(MoveUtil::move(*source)));
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // Here a move should happen regardless of C++ version because
                // we explicitly change the type of the constructor argument
                // to an r-value type (simulated or real).
                ASSERTV(counter, 0 == counter);

                source.reset();
                ASSERTV(counter, 0 == counter);

                moveTarget.reset();
                ASSERTV(counter, TestedExitFuncType::theIncrement(),
                       TestedExitFuncType::theIncrement() == counter);
            }
            ASSERTV(counter, TestedExitFuncType::theIncrement(),
                    TestedExitFuncType::theIncrement() == counter);
        }
#ifdef BDE_BUILD_TARGET_EXC
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    // We can only test 'noexcept' specification if it is supported.

        ASSERT(bsl::is_nothrow_move_constructible<
                                    bdlb::ScopeExit<MoveNoexceptCopyNotFunctor>
                                                         >::value);
        // We can detect 'noexcept' move via explicit traits specialization in
        // C++03, and type traits in C++11 and later with sufficiently
        // functional type traits support.

        ASSERT(bsl::is_nothrow_move_constructible<
                                   bdlb::ScopeExit<MoveCopyBothNoexceptFunctor>
                                                         >::value);

        ASSERT(!bsl::is_nothrow_move_constructible<
                                bdlb::ScopeExit<MoveCopyBothNotNoexceptFunctor>
                                                          >::value);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    // With C++11 or later with sufficiently functional type traits support we
    // use a smarter specification that can also detect 'noexcept' copy
    // constructors.
        ASSERT(bsl::is_nothrow_move_constructible<
                                    bdlb::ScopeExit<CopyNoexceptMoveNotFunctor>
                                                         >::value);
#endif
#endif // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif // BDE_BUILD_TARGET_EXC
#undef U_MAKE_MANAGED
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONVERTING CONSTRUCTOR
        //
        // Concerns:
        //: 1 The converting constructor:
        //:   1 is not in the overload set if the constructor parameter is not
        //:     convertible to the 'EXIT_FUNC' template argument.
        //:
        //:   2 is not in the overload set if the constructor parameter has the
        //:     same type as t.
        //:
        //: 2 The constructor runs the exit function if the copy construction
        //:   of the exit function throws.
        //:
        //: 3 Move-only parameter types are supported.
        //:
        //: 4 The converting constructor is explicit.
        //
        // Plan:
        //: 1 The converting constructor's conditional presence in the overload
        //:   set dependent on the argument type (C-1) cannot be fully verified
        //:   in C++03, so we do what can be done by knowing that test code
        //:   will not compile in case the 'enable_if' is not done right.
        //:
        //: 2 Create a 'bdlb::ScopeExit<ExitFunction>' guard using a different
        //:   argument type for the constructor ('ExitFunctionParam'). C-1
        //:
        //: 2 Verify that the guard is created and invokes the exit function
        //:   upon its destruction. C-1
        //:
        //: 3 Create a 'bdlb::ScopeExit<ExitFunction>' guard using an argument
        //:   type for the constructor ('ExitFunctionParamThrows') that causes
        //:   the constructor to throw. C-2, C-4 ('ExitFunctionParamThrows' is
        //:   not copyable or movable.)
        //:
        //: 4 Verify that the guard invokes 'operator()' of the
        //:   'ExitFunctionParamThrows' argument. C-2
        //:
        //: 5 Use 'MoveOnlyExitFunc' as a parameter to verify that the code
        //:   compiles and works as intended by updating the counter.
        //:
        //: 6 Use the internal overload set of 'VerifyExplicitConstructorUtil'
        //:   to verify that no implicit conversion takes place.
        //
        // Testing:
        //   template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
        //   explicit ScopeExit(void (*function)())
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCONVERTING CONSTRUCTOR"
                          << "\n======================" << endl;

        int counter = 0;

        if (veryVerbose) cout << "Converting Constructor\n";
        {
            counter = 0;
            {
                bdlb::ScopeExit<ExitFunction>
                    mObj((ExitFunctionParam(&counter)));
            }
            ASSERTV(counter, ExitFunction::theIncrement(),
                    ExitFunction::theIncrement() == counter);

            counter = 0;
            {
                const ExitFunctionParam       p(&counter);
                bdlb::ScopeExit<ExitFunction> mObj(p);
            }
            ASSERTV(counter, ExitFunction::theIncrement(),
                    ExitFunction::theIncrement() == counter);

#ifdef BDE_BUILD_TARGET_EXC
            bool seenException = false;
            counter = 0;
            try {
                bdlb::ScopeExit<ExitFunction>
                    mObj((ExitFunctionParamThrows(&counter)));
            }
            catch (ExceptionValues::Enum value) {
                seenException = true;
                ASSERTV(value, ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW,
                        ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW == value);
            }

            // Sanity check
            ASSERT(seenException);

            ASSERTV(counter, ExitFunctionParamThrows::theIncrement(),
                    ExitFunctionParamThrows::theIncrement() == counter);
#endif // BDE_BUILD_TARGET_EXC

            counter = 0;
            MoveOnlyExitFunctionParam exitFunctionParam(&counter);
            {
                bdlb::ScopeExit<MoveOnlyExitFunction>
                                       mObj(MoveUtil::move(exitFunctionParam));
            }
            ASSERTV(counter, MoveOnlyExitFunction::theIncrement(),
                    MoveOnlyExitFunction::theIncrement() == counter);

            g_counter = 0;
            {
                const bdlb::ScopeExit<void(*)()> X(freeExitFunction);
            }
            ASSERTV(g_counter, IncrementValues::k_FUNCTION_POINTER,
                    IncrementValues::k_FUNCTION_POINTER == g_counter);
        }

        if (veryVerbose) cout << "Converting Constructor Explicitness\n";
        {
            ASSERT((VerifyExplicitConstructorUtil<
                        bdlb::ScopeExit<ExitFunction>,
                        ExitFunctionParam>::k_PASSED));

#ifdef BDE_BUILD_TARGET_EXC
            ASSERT((VerifyExplicitConstructorUtil<
                        bdlb::ScopeExit<ExitFunction>,
                        ExitFunctionParamThrows>::k_PASSED));
#endif
        }

        if (veryVerbose) cout << "Possibly Throwing Value Constructor\n";
        {
            counter = 0;
            MoveOnlyExitFunction exitFunction(&counter);
            {
                bdlb::ScopeExit<MoveOnlyExitFunction>
                                            mObj(MoveUtil::move(exitFunction));
            }
            ASSERTV(counter, MoveOnlyExitFunction::theIncrement(),
                    MoveOnlyExitFunction::theIncrement() == counter);
        }

        if (veryVerbose) cout << "'noexcept' Value Constructor\n";
        {
            counter = 0;
            MoveNoexceptCopyNotFunctor exitFunction(&counter);
            {
                bdlb::ScopeExit<MoveNoexceptCopyNotFunctor>
                                            mObj(MoveUtil::move(exitFunction));
            }
            ASSERTV(counter, MoveNoexceptCopyNotFunctor::theIncrement(),
                    MoveNoexceptCopyNotFunctor::theIncrement() == counter);
        }

        if (veryVerbose) cout << "Value Constructor Explicitness\n";
        {
            ASSERT((VerifyExplicitConstructorUtil<
                                         bdlb::ScopeExit<MoveOnlyExitFunction>,
                                         MoveOnlyExitFunction
                                                 >::k_PASSED));

            ASSERT(
                (VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<MoveCopyBothNoexceptFunctor>,
                    MoveCopyBothNoexceptFunctor
                                              >::k_PASSED));

            ASSERT(
                (VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<CopyNoexceptMoveNotFunctor>,
                    CopyNoexceptMoveNotFunctor>::k_PASSED));

            ASSERT(
                (VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<MoveNoexceptCopyNotFunctor>,
                    MoveNoexceptCopyNotFunctor>::k_PASSED));

            ASSERT(
                (VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<MoveCopyBothNotNoexceptFunctor>,
                    MoveCopyBothNotNoexceptFunctor
                                              >::k_PASSED));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DESTRUCTION
        //
        // Concerns:
        //: 1 The destructor executes the scope exit function *once* if no
        //:   'release()' has been called.
        //:
        //: 2 The destructor does not execute the scope exit function if
        //:   'release()' has been called.
        //
        // Plan:
        //: 1 Create a 'bdlb::ScopeExit<ExitFunction>' object with a counter.
        //:   Verify that the counter has been increased by 1 after the object
        //:   is destroyed. C-1
        //:
        //: 2 Create a 'bdlb::ScopeExit<ExitFunction>' object with a counter,
        //:   then call 'release()' on it.  Verify that the counter has *not*
        //:   been increased after the object is destroyed. C-2
        //
        // Testing:
        //   ~ScopeExit()
        //   release()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDESTRUCTION"
                          << "\n===========" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> proctor((ExitFunction(&counter)));
            proctor.release();

            ASSERTV(counter, 0 == counter);
        }

        // The guard does nothing on destruction.
        ASSERTV(counter, 0 == counter);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //   This case verifies that the test machinery works as expected.
        //
        // Concerns:
        //: 1 All exit function test and exit function parameter test types and
        //:   the free function, when called, increment their counter by their
        //:   corresponding 'IncrementValues' enumerator value on every call
        //:   but do not change change the counter on any other operation
        //:   (construction, destruction, conversion).
        //:
        //: 2 Non-throwing exit function parameter const and r-value references
        //:   convert to their counterpart exit function types and the
        //:   resulting function object will increment the same counter as the
        //:   parameter object they were constructed from.
        //:
        //: 3 Throwing exit function parameter const and r-value references
        //:   convert to their counterpart exit function types and during the
        //:   conversion throw an exception of type 'ExceptionValues::Enum'
        //:   with their corresponding enumerator value.
        //:
        //: 4 'MoveOnlyExitFunction' can be moved but not copied.
        //:
        //: 5 'CopyOnlyExitFunction' cannot be moved, only copied.
        //:
        //: 6 'CopyMoveExitFunction' can be both moved and copied, it
        //:   increments the class-level copy and move counters properly.
        //:   'resetCopyMoveCounts()' sets the static counters to zero.
        //:
        //: 7 'VerifyExplicitConstructorUtil' detects 'explicit' construction
        //:   as intended.
        //:
        //: 8 'testMacroText' ignores extra spaces between identifier tokens,
        //:    but does not allows spaces into an identifier token.
        //
        // Plan:
        //: 1 C-3 is tested by creating a 'const' object with a counter,
        //:   doing the verifying that the counter is unchanged with "other"
        //:   operations, calling the functor in a short loop and verify that
        //:   the counter changes as expected, and finally verifying the
        //:   counter value has not been changed by destruction.
        //:
        //: 2 Non-throwing parameters (C-4) are verified by
        //:
        //: 3 The throwing
        //:   parameter type conversion is tested by the usual 'try'-'catch'
        //:   plus a 'bool' test.
        //:
        //: 2 In C++03 it is not possible to verify that a type is non-copyable
        //:   without running into a compiler error, so we only verify that
        //:   'MoveOnlyExitFunction' is move-only when reliable C++11
        //:   '<type_traits>' are present.
        //:
        //: 3 'CopyOnlyExitFunction' not declared move constructor cannot be
        //:   verified (even in C++11 or later), because the copy constructor
        //:   "wins" the overload resolution. Copy-constructibility is verified
        //:   using type traits under C++11 or later.  Notice that because we
        //:   explicitly require the template arguments to be
        //:   *MoveConstructible* 'CopyOnlyExitFunction' does not (and cannot)
        //:   '= delete' its move constructor, so 'is_move_constructible' will
        //:   say yes, because the type is constructible from an r-value
        //:   reference using the copy constructor.
        //:
        //: 4 The 'static' counters of 'CopyMoveExitFunction' are tested by
        //:   creating a copy from a 'const' source first and verifying the
        //:   counters, the move construct a copy and verify the counters.
        //:   Under C++11 or later with sufficiently functional r-value
        //:   references we also verify move from a temporary object.
        //:
        //: 5 'VerifyExplicitConstructorUtil' is tested using 3 helper types:
        //:   two parameter types, and one that has an explicit, and an
        //:   implicit constructor using either of the two parameter types.
        //:   This works both in C++03 and C++11 or later.
        //
        // Testing:
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTEST MACHINERY"
                             "\n==============" << endl;

        if (veryVerbose) cout << "Verifying function object\n";

        const int k_LOOPCOUNT = 10;

        int counter = 0;                                      // 'ExitFunction'
        {
            const ExitFunction ef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                ef();
                ASSERTV(i, counter, ExitFunction::theIncrement(),
                        ExitFunction::theIncrement() * i == counter);
            }
        }
        ASSERTV(counter, ExitFunction::theIncrement(),
                ExitFunction::theIncrement() * k_LOOPCOUNT == counter);

#ifdef BDE_BUILD_TARGET_EXC
        counter = 0;                                // 'MoveThrowsExitFunction'
        {
            MoveThrowsExitFunction        efm(&counter);
            const MoveThrowsExitFunction& ef(efm);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                ef();
                ASSERTV(i, counter, MoveThrowsExitFunction::theIncrement(),
                        MoveThrowsExitFunction::theIncrement() * i == counter);
            }

            bool seenException = false;
            try {
                const MoveThrowsExitFunction ef2(MoveUtil::move(efm));
            }
            catch (ExceptionValues::Enum value) {
                seenException = true;
                ASSERTV(value, ExceptionValues::k_MOVE_THROWS_EXIT_FUNCTION,
                        ExceptionValues::k_MOVE_THROWS_EXIT_FUNCTION == value);
            }
            ASSERT(seenException);
        }
        ASSERTV(counter, MoveThrowsExitFunction::theIncrement(),
            MoveThrowsExitFunction::theIncrement() * k_LOOPCOUNT == counter);

        counter = 0;                                // 'CopyThrowsExitFunction'
        {
            const CopyThrowsExitFunction ef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                ef();
                ASSERTV(i, counter, CopyThrowsExitFunction::theIncrement(),
                    CopyThrowsExitFunction::theIncrement() * i == counter);
            }

            bool seenException = false;
            try {
                const CopyThrowsExitFunction ef2(ef);
            }
            catch (ExceptionValues::Enum value) {
                seenException = true;
                ASSERTV(value, ExceptionValues::k_COPY_THROWS_EXIT_FUNCTION,
                        ExceptionValues::k_COPY_THROWS_EXIT_FUNCTION == value);
            }
            ASSERT(seenException);
        }
        ASSERTV(counter, CopyThrowsExitFunction::theIncrement(),
            CopyThrowsExitFunction::theIncrement() * k_LOOPCOUNT == counter);

        counter = 0;                                 // 'BothThrowExitFunction'
        {
            BothThrowExitFunction        efm(&counter);
            const BothThrowExitFunction& ef(efm);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                ef();
                ASSERTV(i, counter, BothThrowExitFunction::theIncrement(),
                        BothThrowExitFunction::theIncrement() * i == counter);
            }

            bool seenException = false;
            try {
                const BothThrowExitFunction ef2(ef);
            }
            catch (ExceptionValues::Enum value) {
                seenException = true;
                ASSERTV(
                    value,
                    ExceptionValues::k_COPY_BOTH_THROW_EXIT_FUNCTION,
                    ExceptionValues::k_COPY_BOTH_THROW_EXIT_FUNCTION == value);
            }
            ASSERT(seenException);

            seenException = false;
            try {
                const BothThrowExitFunction ef2(MoveUtil::move(efm));
            }
            catch (ExceptionValues::Enum value) {
                seenException = true;
                ASSERTV(
                    value,
                    ExceptionValues::k_MOVE_BOTH_THROW_EXIT_FUNCTION,
                    ExceptionValues::k_MOVE_BOTH_THROW_EXIT_FUNCTION == value);
            }
            ASSERT(seenException);
        }
        ASSERTV(counter, BothThrowExitFunction::theIncrement(),
            BothThrowExitFunction::theIncrement() * k_LOOPCOUNT == counter);
#endif // BDE_BUILD_TARGET_EXC

        counter = 0;                                  // 'MoveOnlyExitFunction'
        {
            const MoveOnlyExitFunction moef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                moef();
                ASSERTV(i, counter, MoveOnlyExitFunction::theIncrement(),
                        MoveOnlyExitFunction::theIncrement() * i == counter);
            }
        }
        ASSERTV(counter, MoveOnlyExitFunction::theIncrement(),
                MoveOnlyExitFunction::theIncrement() * k_LOOPCOUNT == counter);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT(!bsl::is_copy_constructible<MoveOnlyExitFunction>::value);
        ASSERT(!bsl::is_copy_assignable<   MoveOnlyExitFunction>::value);
        ASSERT( bsl::is_move_constructible<MoveOnlyExitFunction>::value);
#endif

        counter = 0;                                  // 'CopyOnlyExitFunction'
        {
            const CopyOnlyExitFunction coef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                coef();
                ASSERTV(i, counter, CopyOnlyExitFunction::theIncrement(),
                        CopyOnlyExitFunction::theIncrement() * i == counter);
            }
        }
        ASSERTV(counter, CopyOnlyExitFunction::theIncrement(),
                CopyOnlyExitFunction::theIncrement() * k_LOOPCOUNT == counter);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT( bsl::is_copy_constructible<CopyOnlyExitFunction>::value);
        ASSERT(!bsl::is_copy_assignable<   CopyOnlyExitFunction>::value);
        ASSERT( bsl::is_move_constructible<CopyOnlyExitFunction>::value);
            // 'is_move_constructible' tells if the type can be constructed
            // from an r-value reference of itself.  This type can, and it uses
            // its copy constructor to do that.
#endif

        counter = 0;                           // 'MoveCopyBothNoexceptFunctor'
        {
            const MoveCopyBothNoexceptFunctor mcef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                mcef();
                ASSERTV(i,
                        counter,
                        MoveCopyBothNoexceptFunctor::theIncrement(),
                        MoveCopyBothNoexceptFunctor::theIncrement() * i
                                                                   == counter);
            }
        }
        ASSERTV(
            counter,
            MoveCopyBothNoexceptFunctor::theIncrement(),
            MoveCopyBothNoexceptFunctor::theIncrement() * k_LOOPCOUNT
                                                                   == counter);

        const unsigned& moveCount = MoveCopyCounts::s_moveCount;
        const unsigned& copyCount = MoveCopyCounts::s_copyCount;

        ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);
        {
            const MoveCopyBothNoexceptFunctor source(&counter);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

            const MoveCopyBothNoexceptFunctor copied(source);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 1);

            counter = 0;
            copied();
            ASSERTV(counter,
                    MoveCopyBothNoexceptFunctor::theIncrement(),
                    MoveCopyBothNoexceptFunctor::theIncrement() == counter);
        }

        MoveCopyCounts::resetCopyMoveCounts();
        ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

        {
            MoveCopyBothNoexceptFunctor source(&counter);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

            const MoveCopyBothNoexceptFunctor moveTarget(
                                                       MoveUtil::move(source));
            ASSERTV(moveCount, copyCount, moveCount == 1 && copyCount == 0);

            counter = 0;
            moveTarget();
            ASSERTV(counter,
                    MoveCopyBothNoexceptFunctor::theIncrement(),
                    MoveCopyBothNoexceptFunctor::theIncrement() == counter);
        }

        if (veryVerbose) cout << "Verifying parameter functors\n";

        counter = 0;                                     // 'ExitFunctionParam'
        {
            const ExitFunctionParam efp(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                efp();
                ASSERTV(i, counter, ExitFunctionParam::theIncrement(),
                        ExitFunctionParam::theIncrement() * i == counter);
            }

            const ExitFunction ef(efp);
            ASSERTV(
                counter, ExitFunctionParam::theIncrement(),
                ExitFunctionParam::theIncrement() * k_LOOPCOUNT == counter);

            ef();
            ASSERTV(counter,
                    ExitFunctionParam::theIncrement(),
                    ExitFunction::theIncrement(),
                    ExitFunctionParam::theIncrement() * k_LOOPCOUNT
                                    + ExitFunction::theIncrement() == counter);
        }
        ASSERTV(counter,
                ExitFunctionParam::theIncrement(),
                ExitFunction::theIncrement(),
                ExitFunctionParam::theIncrement() * k_LOOPCOUNT
                                    + ExitFunction::theIncrement() == counter);

#ifdef BDE_BUILD_TARGET_EXC
        counter = 0;                               // 'ExitFunctionParamThrows'
        {
            const ExitFunctionParamThrows efpt(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                efpt();
                ASSERTV(
                    i, counter, ExitFunctionParamThrows::theIncrement(),
                    ExitFunctionParamThrows::theIncrement() * i == counter);
            }

            bool exceptionSeen = false;
            try {
                const ExitFunction ef(efpt);
                (void)ef;
            }
            catch (ExceptionValues::Enum value) {
                exceptionSeen = true;
                ASSERTV(value, ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW,
                        ExceptionValues::k_EXIT_FUNCTION_PARAM_THROW == value);
            }
            ASSERTV(counter, ExitFunctionParamThrows::theIncrement(),
                    ExitFunctionParamThrows::theIncrement() * k_LOOPCOUNT
                                                                   == counter);
            ASSERT(exceptionSeen);
        }
        ASSERTV(counter, ExitFunctionParamThrows::theIncrement(),
                ExitFunctionParamThrows::theIncrement() * k_LOOPCOUNT
                                                                   == counter);
#endif

        counter = 0;                             // 'MoveOnlyExitFunctionParam'
        {
            MoveOnlyExitFunctionParam        moefpm(&counter);
            const MoveOnlyExitFunctionParam& moefp(moefpm);
            ASSERTV(counter, 0 == counter);

            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                moefp();
                ASSERTV(i, counter, MoveOnlyExitFunctionParam::theIncrement(),
                    MoveOnlyExitFunctionParam::theIncrement() * i == counter);
            }

            const MoveOnlyExitFunction moef(MoveUtil::move(moefpm));
            ASSERT(0 == moefp.counterPtr());
            ASSERTV(counter, MoveOnlyExitFunctionParam::theIncrement(),
                    MoveOnlyExitFunctionParam::theIncrement() * k_LOOPCOUNT
                                                                   == counter);

            moef();
            ASSERTV(counter,
                    MoveOnlyExitFunctionParam::theIncrement(),
                    MoveOnlyExitFunction::theIncrement(),
                    MoveOnlyExitFunctionParam::theIncrement() * k_LOOPCOUNT
                            + MoveOnlyExitFunction::theIncrement() == counter);
        }
        ASSERTV(counter,
                MoveOnlyExitFunctionParam::theIncrement(),
                MoveOnlyExitFunction::theIncrement(),
                MoveOnlyExitFunctionParam::theIncrement() * k_LOOPCOUNT
                            + MoveOnlyExitFunction::theIncrement() == counter);

        if (veryVerbose) cout << "Verifying move/copy counting\n";

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        {
            MoveCopyCounts::resetCopyMoveCounts();
            const MoveCopyBothNoexceptFunctor fromTemporary(
                                    makeMoveCopyBothNoexceptFunctor(&counter));
            ASSERTV(moveCount, copyCount, moveCount <= 2 && copyCount == 0);
            // Compilers are allowed to eliminate this move.

            counter = 0;
            fromTemporary();
            ASSERTV(counter,
                    MoveCopyBothNoexceptFunctor::theIncrement(),
                    MoveCopyBothNoexceptFunctor::theIncrement() == counter);
        }
#endif

        if (veryVerbose) cout << "Verifying free function exit function\n";
        {
            g_counter = 0;
            for (int i = 1; i <= k_LOOPCOUNT; ++i) {
                freeExitFunction();
                ASSERTV(i, g_counter, IncrementValues::k_FUNCTION_POINTER,
                        IncrementValues::k_FUNCTION_POINTER * i == g_counter);
            }
        }

        if (veryVerbose) cout << "Verifying explicit constructor tester\n";
        ASSERT(
            !(VerifyExplicitConstructorUtil<
                 VerifyExplicitConstructorUtilTester,
                 VerifyExplicitConstructorUtilTesterImplicitParam>::k_PASSED));
        ASSERT(
            (VerifyExplicitConstructorUtil<
                 VerifyExplicitConstructorUtilTester,
                 VerifyExplicitConstructorUtilTesterExplicitParam>::k_PASSED));

        if (veryVerbose) cout << "Verifying macro expansion text tester\n";
        ASSERT( testMacroText("$a$",  "a" ));
        ASSERT( testMacroText("$ab$", "ab"));

        ASSERT( testMacroText("$a$",  " a " ));
        ASSERT( testMacroText("$ab$", " ab "));

        ASSERT( testMacroText("$a$",  "a  " ));
        ASSERT( testMacroText("$ab$", "ab  "));

        ASSERT( testMacroText("$a$::$b$", "a:: b"));
        ASSERT( testMacroText("$a$::$b$", "a ::b"));

        ASSERT(!testMacroText("$ab$", "a b"));

        ASSERT( testMacroText(
                       "$::$bdlb$::$ScopeExitAny varName$($($exitFunc$)$)$",
                       "::bdlb ::ScopeExitAny  varName( (exitFunc ))   "));
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
        //: 1 Create a scope exit guard that updates a counter on exit.
        //: 2 Destroy the guard.
        //: 3 Verify that the counter has been updated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard((ExitFunction(&counter)));

            // The action is not run yet.
            ASSERTV(counter, 0 == counter);
        }

        // The guard has invoked the action ('ExitFunction::operator()').
        ASSERTV(counter, ExitFunction::theIncrement() == counter);

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
