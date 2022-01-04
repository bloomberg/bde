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

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//-----------------------------------------------------------------------------
// [ 3] ~ScopeExit()
// [ 3] release()
// [ 4] template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
// [ 5] ScopeExit(bslmf::MovableRef<ScopeExit> original)
// [ 6] ScopeExitUtil::makeScopeExit()
// [ 7] ScopeExitAny
// [ 8] BDLB_SCOPEEXIT_PROCTOR
// [ 9] BDLB_SCOPEEXIT_GUARD
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST MACHINERY
// [10] USAGE EXAMPLE
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
// 'BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L' is implied
#define BDLB_SCOPEEXIT_USES_MODERN_CPP
    // See header for meaning, replicated here for white box tests.
#endif

#if defined(BDE_BUILD_TARGET_EXC) && defined(BDLB_SCOPEEXIT_USES_MODERN_CPP)
#define BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE
    // 'BDLB_SCOPEEXIT_ENABLE_CPP11_EXAMPLE' is defined if all compilers
    // features (exceptions, C++11 'auto' and lambdas) are present that are
    // needed to compile the C++11 usage example.
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#define u_MOVE bsl::move
    // We want to support the "native" move operation in Modern C++ code.
#else
#define u_MOVE bslmf::MovableRefUtil::move
    // C++03 only has an approximation of Modern C++ move.
#endif

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
    // Tell if the specified 'macroText' contains the 'expectedTokens' in the
    // same order as in the argument.  Preprocessed tokens in C++ are separated
    // by space characters that may be optional between certain tokens.
    // 'expectedTokens' is a descriptor that contains literal characters
    // (tokens), special token delimiter characters, and special unique
    // identifier sequence indicator character:
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
                return false;                                       // RETURN
            }
            ePos += tLen - 1; // We have a '++ePos' in the 'for' above
            mPos += tLen;
          }
        }
    }

    return true;
}
                             // ============
                             // ExitFunction
                             // ============

class ExitFunction {
    // A functor that increments a specified counter by one when called.

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

  public:
    // CREATORS
    explicit ExitFunction(int *counter_p);
        // Create an 'ExitFunction' object that will increment the specified
        // 'counter_p' upon its invocation.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by one.
};

                             // ------------
                             // ExitFunction
                             // ------------

// CREATORS
ExitFunction::ExitFunction(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

// ACCESSORS
void ExitFunction::operator()() const
{
    *d_counter_p += 1;
}

                          // =================
                          // ExitFunctionParam
                          // =================

class ExitFunctionParam {
    // A functor class that is convertible to 'ExitFunction' and increments a
    // user-provided pointer by two when called.

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

  public:
    // CREATORS
    explicit ExitFunctionParam(int *counter_p);
        // Create an 'ExitFunctionParam' object that will increment the integer
        // pointed by the specified 'counter_p' when it is called.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by two.

    operator ExitFunction() const;
        // Create an 'ExitFunction' object that increments the same counter
        // this object increments.
};

                          // -----------------
                          // ExitFunctionParam
                          // -----------------

// CREATORS
ExitFunctionParam::ExitFunctionParam(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

// ACCESSORS
void ExitFunctionParam::operator()() const
{
    *d_counter_p += 2;
}

ExitFunctionParam::operator ExitFunction() const
{
    return ExitFunction(d_counter_p);
}

#ifdef BDE_BUILD_TARGET_EXC

                        // ======================
                        // ExitFunctionParamThrow
                        // ======================

class ExitFunctionParamThrow {
    // A functor class that is convertible to 'ExitFunction' and increments a
    // user-provided pointer by three when called.

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

  public:
    // CREATORS
    explicit ExitFunctionParamThrow(int *counter_p);
        // Create an 'ExitFunctionParamThrow' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by three.

    operator ExitFunction() const;
        // Throw an exception of type 'int', value 42.
};

                        // ----------------------
                        // ExitFunctionParamThrow
                        // ----------------------

// CREATORS
ExitFunctionParamThrow::ExitFunctionParamThrow(int *counter_p)
: d_counter_p(counter_p)
{
    BSLS_ASSERT(counter_p);
}

// ACCESSORS
void ExitFunctionParamThrow::operator()() const
{
    *d_counter_p += 3;
}

ExitFunctionParamThrow::operator ExitFunction() const
{
    throw 42;
}
#endif // BDE_BUILD_TARGET_EXC

                          // ====================
                          // MoveOnlyExitFunction
                          // ====================

class MoveOnlyExitFunction {
    // Move-only functor that increments a specified counter by four when it is
    // called.

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

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
    // CREATORS
    explicit MoveOnlyExitFunction(int *counter_p);
        // Create an 'MoveOnlyExitFunction' object that will increment the
        // integer pointed by the specified 'counter_p' when it is called,
        // unless this object is in a moved-from state.

    MoveOnlyExitFunction(bslmf::MovableRef<MoveOnlyExitFunction> original);
        // Create a 'MoveOnlyExitFunction' object having the same value as the
        // specified 'original' object by initializing this object's counter
        // pointer to that of the 'original', and setting the 'original' object
        // to the moved-from state by setting its pointer to null. The behavior
        // is undefined 'original' is already in a moved-from state.

    // ACCESSORS
    void operator()() const;
        // Increment the counter by four. The behavior is undefined if this
        // object is in a moved-from state.
};

                       // --------------------
                       // MoveOnlyExitFunction
                       // --------------------

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

// ACCESSORS
void MoveOnlyExitFunction::operator()() const
{
    BSLS_ASSERT(d_counter_p);

    *d_counter_p += 4;
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

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
    // Move-only functor that increments a specified counter.

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  private:
    // NOT IMPLEMENTED
    CopyOnlyExitFunction& operator=(CopyOnlyExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
    // Note that move constructor is not 'delete'-ed.  It simply will not get
    // generated.  Deleting a function does not remove it from the overload set
    // but "poisons" it.  It can still be selected by overload resolution and
    // if it is the result is a compilation error.
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

public:
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
        // Increment the counter by five.
};

                       // --------------------
                       // CopyOnlyExitFunction
                       // --------------------

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

    *d_counter_p += 5;
}

                          // =====================
                          // struct MoveCopyCounts
                          // =====================

struct MoveCopyCounts {
    // Provides public, resettable static counters for move and copy counts.
    // Shared by all counting code for brevitiy.

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
    // A functor that increments a specified counter and also counts how many
    // times its instances have been copy- or move-constructed.  The template
    // parameters 'MOVE_IS_NOEXCEPT' and 'COPY_IS_NOEXCEPT' determine if the
    // move and copy constructors are 'noexcept', respectively.  This feature
    // is necessary in testing move/copy selection as well as the exception
    // specification of the 'ScopeExit' move constructor.  Note that to
    // simplify the code the counters are shared between *all* instances of the
    // template (we never use more than one instance in a test).

  private:
    // DATA
    int *d_counter_p;  // Held, not owned

  private:
    // NOT IMPLEMENTED
    MoveCopyExitFunction& operator=(const MoveCopyExitFunction&)
                                                          BSLS_KEYWORD_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    MoveCopyExitFunction& operator=(MoveCopyExitFunction&&)
                                                          BSLS_KEYWORD_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
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
                         BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(MOVE_IS_NOEXCEPT);
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
        // Increment the counter by six. The behavior is undefined if this
        // object is in a moved-from state.
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

    *d_counter_p += 6;
}

typedef MoveCopyExitFunction<false, false> MoveThrowCopyThrowExitFunction;
typedef MoveCopyExitFunction<true,  false> MoveNothrowCopyThrowExitFunction;
typedef MoveCopyExitFunction<false, true>  MoveThrowCopyNothrowExitFunction;
typedef MoveCopyExitFunction<true,  true>  MoveNothrowCopyNothrowExitFunction;

                // =============================
                // VerifyExplicitConstructorUtil
                // =============================

template <class TESTED_TYPE, class EXIT_FUNC_PARAM>
struct VerifyExplicitConstructorUtil {
    struct LargerThanChar { char d_a[42]; };

    static LargerThanChar testcall(const TESTED_TYPE&);

    static char testcall(...);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define u_DECLVAL bsl::declval<EXIT_FUNC_PARAM>()
#else
#define u_DECLVAL static_cast<const EXIT_FUNC_PARAM&>(*(const EXIT_FUNC_PARAM*)(0))
#endif
#ifdef BSLS_PLATFORM_CMP_AIX
#pragma  report(disable, "1540-2924")
    // 1540-2924 (W) Cannot pass an argument of non - POD class type "<type>"
    // through ellipsis.  Obviously no argument is passed through ellipsis,
    // because 'testcall' is within 'sizeof', a *non-evaluated* context.
#endif
    static const bool PASSED = sizeof(testcall(u_DECLVAL)) == sizeof(char)
                    && sizeof(testcall(TESTED_TYPE(u_DECLVAL))) > sizeof(char);
#ifdef BSLS_PLATFORM_CMP_AIX
#pragma  report(pop)
#endif
#undef u_DECLVAL
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
                      const VerifyExplicitConstructorUtilTesterImplicitParam&);

    explicit VerifyExplicitConstructorUtilTester(
                      const VerifyExplicitConstructorUtilTesterExplicitParam&);
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
MoveNothrowCopyNothrowExitFunction
makeMoveNothrowCopyNothrowExitFunction(int* counter_p)
    // Create a temporary 'MoveNothrowCopyNothrowExitFunction' object for
    // testing move from a temporary on Modern C++.
{
    return MoveNothrowCopyNothrowExitFunction(counter_p);
}
#endif

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC

///Usage Examples
///--------------
// This section illustrates intended use of this component.
//
///Example 1: Using a Scope Exit Proctor in Modern C++
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we assume a Modern C++ compiler supporting C++11 or later.
// Suppose we are creating a simple database that stores names and their
// associated addresses and we store the names and addresses in two separate
// tables.  While adding data, these tables may fail the insertion, in which
// case we need to roll back the already inserted data, such as if we inserted
// the address first, we need to remove it if insertion of the associated name
// fails.
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
// As our dummy 'insertCustomer' function will fail first time (when
// 'removedAddressId' is zero) with an exception, and we exist this function to
// the caller's error handling 'catch' clause.  While exiting the function via
// the exception the local stack is unwound.  All local variables having
// non-trivial destructors get destroyed by calling them.  On this stack there
// is only 'addressProctor' with a destructor, and it calls its exist function,
// which will save our non-zero 'addressId' value into the global
// 'removedAddressId' variable.
//
// On a second call to this function, because 'removedAddressId' is now
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
// Suppose we are in the same situation as in the Modern C++ example, but we
// have to create a solution that supports C++03 as well.
//
// First, we have to hand-craft a functor that calls 'removeAddress' with a
// given ID because Because C++03 does not support lambdas:
//..
    class RemoveAddress {
        int d_id;  // The identifier of the address (row) to remove

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
// The code is almost the same code as was in 'addCustomer11' for Modern C++,
// except for the upcoming proctor variable definition.
//
// Next, we define the proctor variable with an explicitly spelled out type
// (that uses the functor type template argument), and a functor object
// initialized with the identifier of the address to remove:
//..
        bdlb::ScopeExit<RemoveAddress> addrProctor((RemoveAddress(addressId)));
//..
// Notice the extra parenthesis we had to use to avoid "the most vexing parse"
// (https://en.wikipedia.org/wiki/Most_vexing_parse) issue.  Since we are in
// C++03, we cannot use (curly) brace initialization to avoid that issue.
//
// Now, we can complete the rest of the 'addCustomer03', which is exactly the
// same as the corresponding part of the modern 'addCustomer11' variation:
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
// way keep the roll-back-code near the rest like lambdas allow us in
// Modern C++, albeit with a less fortunate syntax.

// See introduction with direct call to the the bind expression in the
// USAGE EXAMPLE test case, in 'main', then return here for the function
// definition.

// Next, we create yet another customer adding function that differs only in
// its proctor definition from the Modern C++ variant:
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
// as modern compilers.  We would like to keep things easy to remember, so we
// don't want to remember and write different, long macro names for the case
// when the type of the exit function is known, and when it is not.  But we
// also want our code to use the more efficient 'auto' and factory-method
// variation when compiled with modern C++, and only fall back to the slower
// 'bdlb::ScopeExitAny' solution on C++03 big iron compilers.
//
// We still need to use either functor ('RemoveAddress' in our examples) or a
// bind expression for the exit function because C++03 has no lambdas, so
// portable code cannot use them.  But we *can* chose the easy-to-use
// 'BDLB_SCOPEEXIT_PROCTOR' macro and not sprinkle the customer adder function
// with '#ifdef' to see which proctor definition to use.
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
// Significantly less effort than creating code to chose between the faster
// running Modern C++ variation and the C++03 variation, and then also write
// both implementations at every proctor variable definition.
//
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
// printing of the close delimiters without worry about early returns or
// 'break' statement or other control flow.
//
// First, we create a functor type that prints closing delimiters:
//..
    class CloseDelimPrinter {
        const char *d_closingChars; // Using string literals for brevity

      public:
        explicit CloseDelimPrinter(const char *s)
        : d_closingChars(s)
        {
        }

        void operator()() const
        {
            outStream << d_closingChars; // To a fixed stream for brevity
        }
    };
//..
// Then, we can use the above functor and a scope exit guard to automate
// closing of delimiters in the printing functions:
//..
    void printText(const bsl::string_view& text)
    {
//..
// Although this function is very simplistic it serves only as a teaching tool.
//
// Next, we can move the printing of the opening delimiter and the closing one
// near each other in code, so it is clearly visible if an opened delimiter is
// closed:
//..
        outStream << '"';  BDLB_SCOPEEXIT_GUARD(CloseDelimPrinter("\""));
//..
// 'BDLB_SCOPEEXIT_GUARD' provides a very simple way of doing this.  Notice
// that we did not need to provide a name for the guard variable, the macro
// gave it a unique name.  We did not need to worry about "the most vexing
// parse", the macro takes care of adding the extra pair of parenthesis.  We do
// not need to suppress an unused variable compiler warning, the macro does
// that.  And of course C++03 compatibility is included as well.
//
// What is the name of the guard variable?  It is unspecified.  Since this is a
// guard, meaning we do not want to 'release()' it (like a proctor), we do not
// need to know the name.  If you need to call 'release()' , use the other
// macro: 'BDLB_SCOPEEXIT_PROCTOR'.
//
// Now, we can just print what goes inside the delimiters, and we are done:
//..
        bsl::string_view::size_type pos = 0;
        for(;;) {
            const bsl::string_view::size_type end = text.find('"', pos);
            outStream << text.substr(pos,
                                     end != bsl::string_view::npos ?
                                     end - pos :  end);
            if (end == bsl::string_view::npos) {
                break;                                                 // BREAK
            }
            outStream << "\\\""; // Escaped " is \"
            pos = end + 1;
        }
    }
//..
// See conclusion in USAGE EXAMPLE test case, in 'main'.

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
      case 10: {
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
///Example 1: Using a Scope Exit Proctor in Modern C++
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In Example 1 we assume a Modern C++ compiler supporting C++11 or later.
// Suppose we are creating a simple database that stores names and their
// associated addresses and we store the names and addresses in two separate
// tables.  While adding data, these tables may fail the insertion, in which
// case we need to roll back the already inserted data, such as if we inserted
// the address first, we need to remove it if insertion of the associated name
// fails.

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
// Suppose we are in the same situation as in the Modern C++ example, but we
// have to create a solution that supports C++03 as well.

// See the USAGE EXAMPLE section just before 'main' for type and function
// definitions, then return here for the assertions.

// Finally, we can verify that both during the failing first attempt to add a
// customer to our imaginary database and the successful second attempt the
// 'RemoveAddress' functor based proctor works just as well as the lambda based
// modern variant did:
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
// way keep the roll-back-code near the rest like lambdas allow us in
// Modern C++, albeit with a less fortunate syntax.
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
// as modern compilers.  We would like to keep things easy to remember, so we
// don't want to remember and write different, long macro names for the case
// when the type of the exit function is known, and when it is not.  But we
// also want our code to use the more efficient 'auto' and factory-method
// variation when compiled with modern C++, and only fall back to the slower
// 'bdlb::ScopeExitAny' solution on C++03 big iron compilers.

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
// printing of the close delimiters without worry about early returns or
// 'break' statement or other control flow.

// See the USAGE EXAMPLE section just before 'main' for type and function
// definitions, then return here for the assertions.

// Finally, we can print text and verify that it is indeed delimited:
//..
    printText("simple text");
    ASSERT(outStreamContent() == "\"simple text\"");

    printText("\"simple\" \"text\"");
    ASSERT(outStreamContent() == "\"\\\"simple\\\" \\\"text\\\"\"");
//..
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
        ASSERTV(counter, 1 == counter);

#define u_STRINGIFY2(totext) #totext
#define u_STRINGIFY(totext) u_STRINGIFY2(totext)
        const bsl::string_view guardMacroText(
                                  u_STRINGIFY(BDLB_SCOPEEXIT_GUARD(exitFunc)));

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
        const bsl::string_view EXPECTED(
#if BSLA_MAYBE_UNUSED_IS_ACTIVE
            "$[[$maybe_unused$]]"
#endif
            "$const auto bdlbScopeExitGuard_#${$"
                         "BloombergLP$::$bdlb$::$ScopeExitUtil$::$"
                                          "makeScopeExit$($exitFunc$)$}$");
#else
        const bsl::string_view EXPECTED(
#if BSLA_MAYBE_UNUSED_IS_ACTIVE
            "$[[$maybe_unused$]]"
#endif
            "$const BloombergLP$::$bdlb$::$ScopeExitAny "
                                     "bdlbScopeExitGuard_#$($($exitFunc$)$)$");
#endif

        ASSERTV(guardMacroText, EXPECTED,
                testMacroText(EXPECTED, guardMacroText));

        // Each invocation (as long as on separate line) should create a
        // different variable name:

        const bsl::string_view secondGuardText(
                                  u_STRINGIFY(BDLB_SCOPEEXIT_GUARD(exitFunc)));


        ASSERTV(secondGuardText, EXPECTED,
                testMacroText(EXPECTED, secondGuardText));

        ASSERTV(guardMacroText, secondGuardText,
                guardMacroText != secondGuardText);

#undef u_STRINGIFY
#undef u_STRINGIFY2
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PROCTOR MACRO
        //
        // Concerns:
        //: 1 'BDLB_SCOPEEXIT_PROCTOR' creates the proctor variable using the
        //     type appropriate for the build mode (modern or C++03).
        //:
        //: 2 The proctor is created using the specified variable name.
        //:
        //: 3 The proctor is created using the specified exit function.
        //
        // Plan:
        //: 1 Create a proctor using the 'BDLB_SCOPEEXIT_PROCTOR' macro and
        //:   make sure the type of the created object is either
        //:   'bdlbb::ScopeExitAny' (in C++03), or a type deduced from the
        //:   return type of the factory function (in C++11).
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
        ASSERTV(counter, 1 == counter);

#define u_STRINGIFY2(totext) #totext
#define u_STRINGIFY(totext) u_STRINGIFY2(totext)
        const bsl::string_view proctorMacroText(
                       u_STRINGIFY(BDLB_SCOPEEXIT_PROCTOR(varName, exitFunc)));
#undef u_STRINGIFY
#undef u_STRINGIFY2

#ifdef BDLB_SCOPEEXIT_USES_MODERN_CPP
        const bsl::string_view EXPECTED(
            "$auto varName${$BloombergLP$::$bdlb$::$ScopeExitUtil$::$"
                                          "makeScopeExit$($exitFunc$)$}$");
#else
        const bsl::string_view EXPECTED(
            "$BloombergLP$::$bdlb$::$ScopeExitAny varName${$exitFunc$}$");
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
        ASSERTV(counter, 1 == counter);

        counter = 0;
        {
            bdlb::ScopeExitAny mObj((ExitFunctionParam(&counter)));
        }
        ASSERTV(counter, 2 == counter);

        counter = 0;
        {
            bdlb::ScopeExitAny mObj((CopyOnlyExitFunction(&counter)));
        }
        ASSERTV(counter, 5 == counter);

        using bdlb::ScopeExitAny;

        counter = 0;
        {
            ScopeExitAny mObj((MoveThrowCopyThrowExitFunction(&counter)));
        }
        ASSERTV(counter, 6 == counter);

        counter = 0;
        {
            ScopeExitAny mObj((MoveNothrowCopyThrowExitFunction(&counter)));
        }
        ASSERTV(counter, 6 == counter);

        counter = 0;
        {
            ScopeExitAny mObj((MoveThrowCopyNothrowExitFunction(&counter)));
        }
        ASSERTV(counter, 6 == counter);

        counter = 0;
        {
            ScopeExitAny mObj((MoveNothrowCopyNothrowExitFunction(&counter)));
        }
        ASSERTV(counter, 6 == counter);
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
        //: 3 The factory function perfect forwards the argument and it returns
        //    a temporary that will be moved by the compiler.
        //:
        //: 4 Move only parameter and exit types work.
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
        //: 5
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
        ASSERTV(counter, 4 == counter);

        // Verifying that we perfect forward to the converting constructor
        // which then choses a 'noexcept' move over copy.  (The second move of
        // the return value is eliminated by copy elision.)
        counter = 0;
        {
            bslma::TestAllocatorMonitor dam(&defaultAllocator);

            const unsigned& moveCount = MoveCopyCounts::s_moveCount;
            const unsigned& copyCount = MoveCopyCounts::s_copyCount;

            MoveCopyCounts::resetCopyMoveCounts();
            auto mObj = bdlb::ScopeExitUtil::makeScopeExit(
                                   MoveNothrowCopyThrowExitFunction(&counter));
            ASSERTV(moveCount, copyCount, moveCount == 1 && copyCount == 0);
            ASSERT(dam.isTotalSame());
        }
        ASSERTV(counter, 6 == counter);
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
        //: 5 The move operation is 'noexcept' in Modern C++ when either of the
        //:   move- or copy-constructor is noexcept.  In C++03 only the pseudo
        //:   move constructor can be tested for no exceptions
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
        //:     to control the destruction order (works in C++03, unlike
        //:     'unique_ptr'.
        //:
        //: 2 For the 'EXIT_FUNC' type that supports both move and copy verify
        //:   that a move takes place and no copy during the explicit move.
        //:   (We also verify the implicit move from temporary in Modern C++.)
        //:
        //: 3 The noexcept operator is used, if present, to verify the
        //:   exception specification.
        //
        // Testing:
        //   ScopeExit(bslmf::MovableRef<ScopeExit> original)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMOVE CONSTRUCTOR"
                             "\n================" << endl;

#define u_MAKE_MANAGED bslma::ManagedPtrUtil::makeManaged<TestedScopeExitType>
    // Shorter, more readable lines

        int counter = 0;
        {
            typedef CopyOnlyExitFunction                TestedExitFuncType;
            typedef bdlb::ScopeExit<TestedExitFuncType> TestedScopeExitType;

            {
                TestedScopeExitType source((TestedExitFuncType(&counter)));
                {
                    TestedScopeExitType moveTarget(u_MOVE(source));
                    ASSERTV(counter, 0 == counter);
                }
                ASSERTV(counter, 5 == counter);
            }
            ASSERTV(counter, 5 == counter);

            counter = 0;
            {
                typedef bslma::ManagedPtr<TestedScopeExitType>
                                                            TestedScopeExitPtr;
                TestedScopeExitPtr source(
                                 u_MAKE_MANAGED(TestedExitFuncType(&counter)));
                TestedScopeExitPtr moveTarget(u_MAKE_MANAGED(u_MOVE(*source)));

                ASSERTV(counter, 0 == counter);

                source.reset();
                ASSERTV(counter, 0 == counter);

                moveTarget.reset();
                ASSERTV(counter, 5 == counter);
            }
            ASSERTV(counter, 5 == counter);
        }

        counter = 0;
        {
            typedef MoveOnlyExitFunction                TestedExitFuncType;
            typedef bdlb::ScopeExit<TestedExitFuncType> TestedScopeExitType;

            {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TestedScopeExitType source((TestedExitFuncType(&counter)));
#else
                // C++03 cannot move from an unnamed temporary
                TestedExitFuncType  exitFuncParam(&counter);
                TestedScopeExitType source(u_MOVE(exitFuncParam));
#endif
                {
                    const TestedScopeExitType moveTarget(u_MOVE(source));
                    ASSERTV(counter, 0 == counter);
                }
                ASSERTV(counter, 4 == counter);
            }
            ASSERTV(counter, 4 == counter);

            counter = 0;
            {
                typedef bslma::ManagedPtr<TestedScopeExitType>
                                                            TestedScopeExitPtr;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                TestedScopeExitPtr source(u_MAKE_MANAGED(
                                                TestedExitFuncType(&counter)));
#else
                // C++03 cannot move from an unnamed temporary
                TestedExitFuncType exitFuncParam(&counter);
                TestedScopeExitPtr source(
                                        u_MAKE_MANAGED(u_MOVE(exitFuncParam)));
#endif
                TestedScopeExitPtr moveTarget(u_MAKE_MANAGED(u_MOVE(*source)));
                ASSERTV(counter, 0 == counter);

                source.reset();
                ASSERTV(counter, 0 == counter);

                moveTarget.reset();
                ASSERTV(counter, 4 == counter);
            }
            ASSERTV(counter, 4 == counter);
        }

        counter = 0;
        {
            typedef MoveNothrowCopyThrowExitFunction    TestedExitFuncType;
            typedef bdlb::ScopeExit<TestedExitFuncType> TestedScopeExitType;

            const unsigned& moveCount = MoveCopyCounts::s_moveCount;
            const unsigned& copyCount = MoveCopyCounts::s_copyCount;

            {
                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitType source((TestedExitFuncType(&counter)));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // Modern C++ moves the unnamed temporary value.
#else
                ASSERTV(moveCount, copyCount,
                        moveCount == 0 && copyCount == 1);
                // C++03 will copy here because it has no real r-value
                // references to detect that our nameless temporary is an
                // r-value.
#endif
                {
                    MoveCopyCounts::resetCopyMoveCounts();
                    TestedScopeExitType moveTarget(u_MOVE(source));
                    ASSERTV(moveCount, copyCount,
                            moveCount == 1 && copyCount == 0);
                    // Here a move should happen regardless of C++ version
                    // because we explicitly change the type of the constructor
                    // argument to an r-value type (simulated or real).
                    ASSERTV(counter, 0 == counter);
                }
                ASSERTV(counter, 6 == counter);
            }
            ASSERTV(counter, 6 == counter);

            counter = 0;
            {
                typedef bslma::ManagedPtr<TestedScopeExitType>
                                                            TestedScopeExitPtr;

                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitPtr source(u_MAKE_MANAGED(
                                                TestedExitFuncType(&counter)));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // Modern C++ moves the unnamed temporary value.
#else
                ASSERTV(moveCount, copyCount,
                        moveCount == 0 && copyCount == 1);
                // C++03 will copy here because it has no real r-value
                // references to detect that our nameless temporary is an
                // r-value.
#endif
                MoveCopyCounts::resetCopyMoveCounts();
                TestedScopeExitPtr moveTarget(u_MAKE_MANAGED(u_MOVE(*source)));
                ASSERTV(moveCount, copyCount,
                        moveCount == 1 && copyCount == 0);
                // Here a move should happen regardless of C++ version
                // because we explicitly change the type of the constructor
                // argument to an r-value type (simulated or real).
                ASSERTV(counter, 0 == counter);

                source.reset();
                ASSERTV(counter, 0 == counter);

                moveTarget.reset();
                ASSERTV(counter, 6 == counter);
            }
            ASSERTV(counter, 6 == counter);
        }
#ifdef BDE_BUILD_TARGET_EXC
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
    // We can only test 'noexcept' specification if it is supported.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# define u_DECLVAL(type) bsl::declval<type>()
#else
# define u_DECLVAL(type) (*(const type*)(0))
#endif

        ASSERT(true == bsl::is_nothrow_move_constructible<
                           bdlb::ScopeExit<MoveNothrowCopyThrowExitFunction>
                       >::value);
        // We can detect 'noexcept' move via explicit traits specialization in
        // C++03, and type traits in Modern C++.

        ASSERT(true == bsl::is_nothrow_move_constructible<
                           bdlb::ScopeExit<MoveNothrowCopyNothrowExitFunction>
                       >::value);

        ASSERT(false == bsl::is_nothrow_move_constructible<
                            bdlb::ScopeExit<MoveThrowCopyThrowExitFunction>
                        >::value);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
    // With Modern C++ type traits support we use a smarter specification that
    // can also detect 'noexcept' copy constructors.
        ASSERT(true == bsl::is_nothrow_move_constructible<
                           bdlb::ScopeExit<MoveThrowCopyNothrowExitFunction>
                       >::value);
#endif
#undef u_DECLVAL
#endif // BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
#endif // BDE_BUILD_TARGET_EXC

#undef u_MAKE_MANAGED
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONVERTING CONSTRUCTOR
        //
        // Concerns:
        //: 1 The converting constructor is in the overload set when the
        //:   constructor argument is convertible to the template argument
        //:   'EXIT_FUNC' but its not the same type (after decay).
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
        //:   type for the constructor ('ExitFunctionParamThrow') that causes
        //:   the constructor to throw. C-2, C-4 ('ExitFunctionParamThrow' is
        //:   not copyable or movable.)
        //:
        //: 4 Verify that the guard invokes 'operator()' of the
        //:   'ExitFunctionParamThrow' argument. C-2
        //:
        //: 5 Use 'MoveOnlyExitFunc' as a parameter to verify that the code
        //:   compiles and works as intended by updating the counter.
        //:
        //: 6 Use the internal overload set of 'VerifyExplicitConstructorUtil'
        //:   to verify that no implicit conversion takes place.
        //
        // Testing:
        //   template ScopeExit::ScopeExit(EXIT_FUNC_PARAM function)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCONVERTING CONSTRUCTOR"
                          << "\n======================" << endl;

        int counter = 0;
        {
            bdlb::ScopeExit<ExitFunction> guard((ExitFunctionParam(&counter)));
        }
        ASSERTV(counter, 1 == counter);

        counter = 0;
        {
            const ExitFunctionParam p(&counter);
            bdlb::ScopeExit<ExitFunction> guard(p);
        }
        ASSERTV(counter, 1 == counter);

#ifdef BDE_BUILD_TARGET_EXC
        bool reallyThrewAnException = false;
        try {
            bdlb::ScopeExit<ExitFunction>
                                     guard((ExitFunctionParamThrow(&counter)));
        }
        catch (int) {
            reallyThrewAnException = true;
        }

        // Sanity check
        ASSERT(reallyThrewAnException);

        // 'ExitFunctionParamThrow' has increased the counter by two.
        ASSERTV(counter, 4 == counter);
#endif // BDE_BUILD_TARGET_EXC

        counter = 0;
        MoveOnlyExitFunction exitFunction(&counter);
        {
            bdlb::ScopeExit<MoveOnlyExitFunction> mObj(u_MOVE(exitFunction));
        }
        ASSERTV(counter, 4 == counter);

        ASSERT((VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<ExitFunction>,
                    ExitFunctionParam>::PASSED) == true);

#ifdef BDE_BUILD_TARGET_EXC
        ASSERT((VerifyExplicitConstructorUtil<
                    bdlb::ScopeExit<ExitFunction>,
                    ExitFunctionParamThrow>::PASSED) == true);
#endif
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
        //: 2 Create a 'bdlb::ScopeExit<ExitFunction>' object with a counter.
        //:   Verify that the counter has been increased by 1 after the object
        //:   is destroyed. C-1
        //:
        //: 2 Create a 'bdlb::ScopeExit<ExitFunction>' object with a counter,
        //:   then call 'release()' on ot.  Verify that the counter has *not*
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
        //:  1 A 'const ExitFunction', when called, increments its counter by 1
        //:    on every call, and does not change it otherwise (on construction
        //:    or destruction).
        //:
        //:  2 A 'const ExitFunctionParam' object, when called, increments its
        //:    counter by 2 on every call, and does not change it otherwise (on
        //:    construction, or conversion).
        //:
        //:  3 A 'const ExitFunctionParam' reference can be converted to an
        //:    'ExitFunction' object that will increment the same counter.
        //:
        //:  4 A 'const ExitFunctionParamThrow' object, when called, increments
        //:    its counter by 2 on every call, and does not change it otherwise
        //:    (on construction, destruction or conversion).
        //:
        //:  5 A conversion from 'const ExitFunctionParamThrow' reference to an
        //:    'ExitFunction' exists and throws int(42) when attempted.
        //:
        //:  6 'MoveOnlyExitFunction' can be moved but not copied, and it
        //:    increments its counter by 4 when called.
        //:
        //:  7 'CopyOnlyExitFunction' cannot be moved, and it increments its
        //:    by 5 when called.
        //:
        //:  8 'CopyMoveExitFunction' can be both moved and copied, it
        //:    increments the class-level copy and move counters properly, and
        //:    it increments its by 6 when called.  'reserCopyMoveCounts()'
        //:    sets the static counters to zero.
        //:
        //:  9 'VerifyExplicitConstructorUtil' detects 'explicit' construction
        //:    as intended.
        //:
        //: 10 'testMacroText' ignores extra spaces between identifier
        //:    tokens, but does not allows spaces into an identifier token.
        //
        // Plan:
        //: 1 C-1 to C8 is tested by creating a 'const' object with a counter,
        //:   doing the verifying that the counter is unchanged with "other"
        //:   operations, calling the functor in a short loop and verify that
        //:   the counter changes as expected, and finally verifying the
        //:   counter value has not been changed by destruction.  The throwing
        //:   parameter type conversion is tested by the usual 'try'-'catch'
        //:   plus a 'bool' test.
        //:
        //: 2 In C++03 it is not possible to verify that a type is non-copyable
        //:   without running into a compiler error, so we only verify that
        //:   'MoveOnlyExitFunction' is move-only when reliable modern C++
        //:   '<type_traits>' are present.
        //:
        //: 3 'CopyOnlyExitFunction' not declared move constructor cannot be
        //:   verified (even in Modern C++), because the copy constructor
        //:   "wins" the overload resolution. Copy-constructibility is verified
        //:   using type traits under Modern C++.  Notice that because we
        //:   explicitly require the template arguments to be
        //:   *MoveConstructible* 'CopyOnlyExitFunction' does not (and cannot)
        //:   '= delete' its move constructor, so 'is_move_constructible' will
        //:   say yes, because the type is constructible from an r-value
        //:   reference using the copy constructor.
        //:
        //: 4 The 'static' counters of 'CopyMoveExitFunction' are tested by
        //:   creating a copy from a 'const' source first and verifying the
        //:   counters, the move construct a copy and verify the counters.
        //:   Under Modern C++ we also verify move from a temporary object.
        //:
        //: 5 'VerifyExplicitConstructorUtil' is tested using 3 helper types:
        //:   two parameter types, and one that has an explicit, and an
        //:   implicit constructor using either of the two parameter types.
        //:   This works both in C++03 and modern C++.
        //
        // Testing:
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTEST MACHINERY"
                             "\n==============" << endl;

        int counter = 0;
        {
            const ExitFunction ef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                ef();
                ASSERTV(counter, i + 1, i + 1 == counter);
            }
        }
        ASSERTV(counter, 10 == counter);

        counter = 0;
        {
            const ExitFunctionParam efp(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                efp();
                ASSERTV(counter, i + 1, 2 * (i + 1) == counter);
            }

            const ExitFunction ef(efp);
            ASSERTV(counter, 20 == counter);

            ef();
            ASSERTV(counter, 21 == counter);
        }
        ASSERTV(counter, 21 == counter);

#ifdef BDE_BUILD_TARGET_EXC
        counter = 0;
        {
            const ExitFunctionParamThrow efpt(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                efpt();
                ASSERTV(counter, i + 1, 3 * (i + 1) == counter);
            }

            bool exceptionSeen = false;
            try {
                const ExitFunction ef(efpt);
                (void)ef;
            }
            catch (int exceptionValue) {
                exceptionSeen = true;
                ASSERTV(exceptionValue, 42 == exceptionValue);
            }
            ASSERTV(counter, 30 == counter);
            ASSERT(exceptionSeen);
        }
        ASSERTV(counter, 30 == counter);
#endif

        counter = 0;
        {
            const MoveOnlyExitFunction moef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                moef();
                ASSERTV(counter, i + 1, 4 * (i + 1) == counter);
            }
        }
        ASSERTV(counter, 40 == counter);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT(
            bsl::is_copy_constructible<MoveOnlyExitFunction>::value == false);
        ASSERT(
            bsl::is_copy_assignable<MoveOnlyExitFunction>::value == false);
        ASSERT(
            bsl::is_move_constructible<MoveOnlyExitFunction>::value == true);
#endif

        counter = 0;
        {
            const CopyOnlyExitFunction coef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                coef();
                ASSERTV(counter, i + 1, 5 * (i + 1) == counter);
            }
        }
        ASSERTV(counter, 50 == counter);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT(
            bsl::is_copy_constructible<CopyOnlyExitFunction>::value == true);
        ASSERT(
            bsl::is_copy_assignable<CopyOnlyExitFunction>::value == false);
        ASSERT(
            bsl::is_move_constructible<CopyOnlyExitFunction>::value == true);
            // 'is_move_constructible' tells if the type can be constructed
            // from an r-value reference of itself.  This type can, and it uses
            // its copy constructor to do that.
#endif

        counter = 0;
        {
            const MoveNothrowCopyNothrowExitFunction mcef(&counter);
            ASSERTV(counter, 0 == counter);

            for (int i = 0; i < 10; ++i) {
                mcef();
                ASSERTV(counter, i + 1, 6 * (i + 1) == counter);
            }
        }
        ASSERTV(counter, 60 == counter);

        const unsigned& moveCount = MoveCopyCounts::s_moveCount;
        const unsigned& copyCount = MoveCopyCounts::s_copyCount;

        ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);
        {
            const MoveNothrowCopyNothrowExitFunction source(&counter);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

            const MoveNothrowCopyNothrowExitFunction copied(source);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 1);

            counter = 0;
            copied();
            ASSERTV(counter, 6 == counter);
        }

        MoveCopyCounts::resetCopyMoveCounts();
        ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

        {
            MoveNothrowCopyNothrowExitFunction source(&counter);
            ASSERTV(moveCount, copyCount, moveCount == 0 && copyCount == 0);

            const MoveNothrowCopyNothrowExitFunction moveTarget(
                                                               u_MOVE(source));
            ASSERTV(moveCount, copyCount, moveCount == 1 && copyCount == 0);

            counter = 0;
            moveTarget();
            ASSERTV(counter, 6 == counter);
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        {
            MoveCopyCounts::resetCopyMoveCounts();
            const MoveNothrowCopyNothrowExitFunction fromTemporary(
                             makeMoveNothrowCopyNothrowExitFunction(&counter));
            ASSERTV(moveCount, copyCount, moveCount <= 2 && copyCount == 0);
            // Compilers are allowed to eliminate this move.

            counter = 0;
            fromTemporary();
            ASSERTV(counter, 6 == counter);
        }
#endif
#undef u_VERIFY_OPT_MOVE_COUNT
#undef u_VERIFY_MOVE_COPY_COUNT
#undef u_VERIFY_COPY_COUNT
#undef u_VERIFY_MOVE_COUNT

        ASSERT(
            (VerifyExplicitConstructorUtil<
                 VerifyExplicitConstructorUtilTester,
                 VerifyExplicitConstructorUtilTesterImplicitParam>::PASSED)
                                                                     == false);
        ASSERT(
            (VerifyExplicitConstructorUtil<
                 VerifyExplicitConstructorUtilTester,
                 VerifyExplicitConstructorUtilTesterExplicitParam>::PASSED)
                                                                      == true);

        // 'testMacroText' tests
        ASSERT(true  == testMacroText("$a$",  "a" ));
        ASSERT(true  == testMacroText("$ab$", "ab"));

        ASSERT(true  == testMacroText("$a$",  " a " ));
        ASSERT(true  == testMacroText("$ab$", " ab "));

        ASSERT(true  == testMacroText("$a$",  "a  " ));
        ASSERT(true  == testMacroText("$ab$", "ab  "));

        ASSERT(true  == testMacroText("$a$::$b$", "a:: b"));
        ASSERT(true  == testMacroText("$a$::$b$", "a ::b"));

        ASSERT(false == testMacroText("$ab$", "a b"));

        ASSERT(true  == testMacroText(
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
        ASSERTV(counter, 1 == counter);

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
