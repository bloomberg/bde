// bslma_sharedptrinplacerep.t.cpp                                    -*-C++-*-
#include <bslma_sharedptrinplacerep.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

#include <new>                 // if so, need to include new as well

#ifdef BSLS_PLATFORM_CMP_MSVC  // Microsoft Compiler
#ifdef _MSC_EXTENSIONS         // Microsoft Extensions Enabled
#include <new>                 // if so, need to include new as well
#endif
#endif

// These warning suppressions will be eliminated in due course.
// BDE_VERIFY pragma: -FD01  // Test-machinery lacks a contract
// BDE_VERIFY pragma: -TP18  // Test-case banners are ALL-CAPS

using namespace BloombergLP;

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This test driver tests the functionality of the in-place shared pointer
// representation.
//-----------------------------------------------------------------------------
// bslma::SharedPtrInplaceRep
//---------------------------
// [ 2] SharedPtrInplaceRep(bslma::Allocator *basicAllocator);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1& a1);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a2);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a3);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a4);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a5);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a6);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a7);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a8);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a9);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a10);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a11);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a12);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a13);
// [ 3] SharedPtrInplaceRep(bslma::Allocator *allocator, const A1&...a14);
// [ 2] TYPE *ptr();
// [ 2] void disposeRep();
// [ 4] void disposeObject();
// [  ] void *getDeleter(const std::type_info& type);
// [ 2] void *originalPtr() const;
// [ 5] void releaseRef();
// [ 5] void releaseWeakRef();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

//=============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// 'MyTestObject' CLASS HIERARCHY (defined below)
class MyTestObject;

// OTHER TEST OBJECTS (defined below)
class MyInplaceTestObject;

// TYPEDEFS
typedef bslmf::MovableRefUtil MoveUtil;         // Commonly used utility

typedef bslma::SharedPtrInplaceRep<MyTestObject> Obj;

typedef bslma::SharedPtrInplaceRep<MyInplaceTestObject> TCObj;
                                                    // For testing constructors
typedef MyTestObject TObj;


                             // ==================
                             // class MyTestObject
                             // ==================

class MyTestObject {
    // This class provides a test object that keeps track of how many instances
    // have been deleted.  Optionally, also keeps track of how many instances
    // have been copied.

    // DATA
    volatile int *d_deleteCounter_p;
    volatile int *d_copyCounter_p;

  public:
    // CREATORS
    MyTestObject(const MyTestObject& orig);
    explicit MyTestObject(int *deleteCounter, int *copyCounter = 0);
    virtual ~MyTestObject();

    // ACCESSORS
    volatile int *copyCounter() const;
    volatile int *deleteCounter() const;
};

                             // ------------------
                             // class MyTestObject
                             // ------------------

// CREATORS
MyTestObject::MyTestObject(const MyTestObject& orig)
: d_deleteCounter_p(orig.d_deleteCounter_p)
, d_copyCounter_p(orig.d_copyCounter_p)
{
    if (d_copyCounter_p) ++(*d_copyCounter_p);
}

MyTestObject::MyTestObject(int *deleteCounter, int *copyCounter)
: d_deleteCounter_p(deleteCounter)
, d_copyCounter_p(copyCounter)
{
}

MyTestObject::~MyTestObject()
{
    ++(*d_deleteCounter_p);
}

// ACCESSORS
volatile int* MyTestObject::copyCounter() const
{
    return d_copyCounter_p;
}

volatile int* MyTestObject::deleteCounter() const
{
    return d_deleteCounter_p;
}

                        // ===============
                        // class MoveState
                        // ===============

struct MoveState {
  public:
    // TYPES
    enum Enum {
        // Enumeration of move state.

        e_NOT_MOVED,  // The type was not involved in a move operation.

        e_MOVED,      // The type was involved in a move operation.

        e_UNKNOWN     // The type does not expose move-state information.
    };

  public:
    // CLASS METHOD
    static const char *toAscii(MoveState::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << MoveState::toAscii(MoveState::e_MOVED);
        //..
        // will print the following on standard output:
        //..
        //  MOVED
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.

};

// FREE FUNCTIONS
inline
void debugprint(const MoveState::Enum& value)
    // Print the specified 'value' as a string.
{
    printf("%s", MoveState::toAscii(value));
}

                         // ====================
                         // class MyTestArg<int>
                         // ====================

template <int N>
class MyTestArg {
    // This class template declares a separate type for each template parameter
    // value 'N', 'MyTestArg<N>', that wraps an integer value and provides
    // implicit conversion to and from 'int'.  Its main purpose is that having
    // separate types for testing enables distinguishing them when calling
    // through a function template interface, thereby avoiding ambiguities or
    // accidental switching of arguments in the implementation of in-place
    // constructors.

    // DATA
    int              d_data;       // attribute value
    MoveState::Enum  d_movedFrom;  // moved-from state
    MoveState::Enum  d_movedInto;  // moved-into state

  public:
    // CREATORS
    MyTestArg();
        // Create a 'MyTestArg' object having the default attribute value '-1'.

    explicit MyTestArg(int value);
        // Create an 'MyTestArg' object having the specified 'value'. The
        // behavior is undefined unless 'value >= 0'.

    MyTestArg(const MyTestArg& original);
        // Create a 'MyTestArg' object having the same value as the specified
        // 'original'.

    MyTestArg(BloombergLP::bslmf::MovableRef<MyTestArg> original);
        // Create a 'MyTestArg' object having the same value as the specified
        // 'original'.  Note that 'original' is left in a valid but unspecified
        // state.

    //! ~ArgumentType() = default;
        // Destroy this object.

    // MANIPULATORS
    MyTestArg& operator=(const MyTestArg& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    MyTestArg& operator=(BloombergLP::bslmf::MovableRef<MyTestArg> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that 'rhs' is left in a valid but unspecified state.

    // ACCESSORS
    operator int() const;
        // Return the value of this object.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

};

// FREE FUNCTIONS
template <int N>
MoveState::Enum getMovedFrom(const MyTestArg<N>& object);
    // Return the move-from state of the specified 'object'.

template <int N>
MoveState::Enum getMovedInto(const MyTestArg<N>& object);
    // Return the move-into state of the specified 'object'.

typedef MyTestArg< 1> MyTestArg01;
typedef MyTestArg< 2> MyTestArg02;
typedef MyTestArg< 3> MyTestArg03;
typedef MyTestArg< 4> MyTestArg04;
typedef MyTestArg< 5> MyTestArg05;
typedef MyTestArg< 6> MyTestArg06;
typedef MyTestArg< 7> MyTestArg07;
typedef MyTestArg< 8> MyTestArg08;
typedef MyTestArg< 9> MyTestArg09;
typedef MyTestArg<10> MyTestArg10;
typedef MyTestArg<11> MyTestArg11;
typedef MyTestArg<12> MyTestArg12;
typedef MyTestArg<13> MyTestArg13;
typedef MyTestArg<14> MyTestArg14;
    // Define fourteen test argument types 'MyTestArg01..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

typedef MyTestArg01                                     ArgType01;
typedef MyTestArg02                                     ArgType02;
typedef MyTestArg03                                     ArgType03;
typedef MyTestArg04                                     ArgType04;
typedef MyTestArg05                                     ArgType05;
typedef MyTestArg06                                     ArgType06;
typedef MyTestArg07                                     ArgType07;
typedef MyTestArg08                                     ArgType08;
typedef MyTestArg09                                     ArgType09;
typedef MyTestArg10                                     ArgType10;
typedef MyTestArg11                                     ArgType11;
typedef MyTestArg12                                     ArgType12;
typedef MyTestArg13                                     ArgType13;
typedef MyTestArg14                                     ArgType14;
    // Define fourteen test argument types 'MyTestArg01..14' to be used with the
    // in-place constructors of 'MyInplaceTestObject'.

                         // =========================
                         // class MyInplaceTestObject
                         // =========================

class MyInplaceTestObject {
    // This class provides a test object used to check that the arguments
    // passed for creating a shared pointer with an in-place representation are
    // of the correct types and values.

    // DATA
    MyTestArg01 d_a01;
    MyTestArg02 d_a02;
    MyTestArg03 d_a03;
    MyTestArg04 d_a04;
    MyTestArg05 d_a05;
    MyTestArg06 d_a06;
    MyTestArg07 d_a07;
    MyTestArg08 d_a08;
    MyTestArg09 d_a09;
    MyTestArg10 d_a10;
    MyTestArg11 d_a11;
    MyTestArg12 d_a12;
    MyTestArg13 d_a13;
    MyTestArg14 d_a14;
    static int  s_numDeletes;

  public:
    // CREATORS
    MyInplaceTestObject();
    explicit MyInplaceTestObject(MyTestArg01 a1);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09,
                        MyTestArg10 a10);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09,
                        MyTestArg10 a10,
                        MyTestArg11 a11);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12,
                        MyTestArg13 a13);
    MyInplaceTestObject(MyTestArg01 a01,
                        MyTestArg02 a02,
                        MyTestArg03 a03,
                        MyTestArg04 a04,
                        MyTestArg05 a05,
                        MyTestArg06 a06,
                        MyTestArg07 a07,
                        MyTestArg08 a08,
                        MyTestArg09 a09,
                        MyTestArg10 a10,
                        MyTestArg11 a11,
                        MyTestArg12 a12,
                        MyTestArg13 a13,
                        MyTestArg14 a14);
        // Create a 'MyInplaceTestObject' by initializing the data members
        // 'd_a01'..'d_a14' with the specified 'a1'..'a14', and initializing any
        // remaining data members with their default value (-1).

    ~MyInplaceTestObject();
        // Increment the count of calls to this destructor, and destroy this
        // object.

    // ACCESSORS
    bool operator==(const MyInplaceTestObject& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as this
        // object, and 'false' otherwise.  Two 'MyInplaceTestObject' objects
        // have the same value if each of their corresponding data members
        // 'd1'..'d14' have the same value.

    const ArgType01& arg01() const;
    const ArgType02& arg02() const;
    const ArgType03& arg03() const;
    const ArgType04& arg04() const;
    const ArgType05& arg05() const;
    const ArgType06& arg06() const;
    const ArgType07& arg07() const;
    const ArgType08& arg08() const;
    const ArgType09& arg09() const;
    const ArgType10& arg10() const;
    const ArgType11& arg11() const;
    const ArgType12& arg12() const;
    const ArgType13& arg13() const;
    const ArgType14& arg14() const;
        // Return the value of the correspondingly numbered argument that was
        // passed to the constructor of this object.

    static int getNumDeletes();
        // Return the number of times an object of this type has been
        // destroyed.
};

//=============================================================================
//                      GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static const MyTestArg01 V01(1);
static const MyTestArg02 V02(20);
static const MyTestArg03 V03(23);
static const MyTestArg04 V04(44);
static const MyTestArg05 V05(66);
static const MyTestArg06 V06(176);
static const MyTestArg07 V07(878);
static const MyTestArg08 V08(8);
static const MyTestArg09 V09(912);
static const MyTestArg10 V10(102);
static const MyTestArg11 V11(111);
static const MyTestArg12 V12(333);
static const MyTestArg13 V13(712);
static const MyTestArg14 V14(1414);

//=============================================================================
//                 TEST MACHINERY FUNCTION DEFINITIONS
//-----------------------------------------------------------------------------

                     // ----------------
                     // struct MoveState
                     // ----------------

// CLASS METHODS
const char *MoveState::toAscii(MoveState::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(NOT_MOVED)
      CASE(MOVED)
      CASE(UNKNOWN)
      default: return "(* UNKNOWN *)";
    }

#undef CASE
}

                         // ---------------
                         // class MyTestArg
                         // ---------------

// CREATORS
template <int N>
inline
MyTestArg<N>::MyTestArg()
: d_data(-1)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
}

template <int N>
inline
MyTestArg<N>::MyTestArg(int value)
: d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
    BSLS_ASSERT_SAFE(value >= 0);
    d_data = value;
}

template <int N>
inline
MyTestArg<N>::MyTestArg(const MyTestArg& original)
: d_data(original.d_data)
, d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_NOT_MOVED)
{
}

template <int N>
inline
MyTestArg<N>::MyTestArg(
                  BloombergLP::bslmf::MovableRef<MyTestArg> original)
: d_movedFrom(MoveState::e_NOT_MOVED)
, d_movedInto(MoveState::e_MOVED)
{
    MyTestArg& lvalue = original;

    d_data = lvalue.d_data;

    lvalue.d_data = -1;
    lvalue.d_movedFrom = MoveState::e_MOVED;
    lvalue.d_movedInto = MoveState::e_NOT_MOVED;
}

// MANIPULATORS
template <int N>
inline
MyTestArg<N>& MyTestArg<N>::operator=(const MyTestArg& rhs)
{
    if (this != &rhs) {
        d_data = rhs.d_data;
        d_movedFrom = MoveState::e_NOT_MOVED;
        d_movedInto = MoveState::e_NOT_MOVED;
    }
    return *this;
}

template <int N>
inline
MyTestArg<N>&
MyTestArg<N>::operator=(BloombergLP::bslmf::MovableRef<MyTestArg> rhs)
{
    MyTestArg& lvalue = rhs;

    if (this != &lvalue) {
        d_data = lvalue.d_data;
        d_movedFrom = MoveState::e_NOT_MOVED;
        d_movedInto = MoveState::e_MOVED;

        lvalue.d_data = -1;
        lvalue.d_movedFrom = MoveState::e_MOVED;
        lvalue.d_movedInto = MoveState::e_NOT_MOVED;
    }
    return *this;
}

// ACCESSORS
template <int N>
inline
MyTestArg<N>::operator int() const
{
    return d_data;
}

template <int N>
inline
MoveState::Enum MyTestArg<N>::movedFrom() const
{
    return d_movedFrom;
}

template <int N>
inline
MoveState::Enum MyTestArg<N>::movedInto() const
{
    return d_movedInto;
}

// FREE FUNCTIONS
template <int N>
inline
MoveState::Enum getMovedFrom(const MyTestArg<N>& object)
{
    return object.movedFrom();
}

template <int N>
inline
MoveState::Enum getMovedInto(const MyTestArg<N>& object)
{
    return object.movedInto();
}


                         // -------------------------
                         // class MyInplaceTestObject
                         // -------------------------

int MyInplaceTestObject::s_numDeletes = 0;

// CREATORS
MyInplaceTestObject::MyInplaceTestObject()
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01)
: d_a01(a01)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02)
: d_a01(a01)
, d_a02(a02)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09,
                                         MyTestArg10 a10)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12,
                                         MyTestArg13 a13)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
{
}

MyInplaceTestObject::MyInplaceTestObject(MyTestArg01 a01,
                                         MyTestArg02 a02,
                                         MyTestArg03 a03,
                                         MyTestArg04 a04,
                                         MyTestArg05 a05,
                                         MyTestArg06 a06,
                                         MyTestArg07 a07,
                                         MyTestArg08 a08,
                                         MyTestArg09 a09,
                                         MyTestArg10 a10,
                                         MyTestArg11 a11,
                                         MyTestArg12 a12,
                                         MyTestArg13 a13,
                                         MyTestArg14 a14)
: d_a01(a01)
, d_a02(a02)
, d_a03(a03)
, d_a04(a04)
, d_a05(a05)
, d_a06(a06)
, d_a07(a07)
, d_a08(a08)
, d_a09(a09)
, d_a10(a10)
, d_a11(a11)
, d_a12(a12)
, d_a13(a13)
, d_a14(a14)
{
}

inline
MyInplaceTestObject::~MyInplaceTestObject()
{
    ++s_numDeletes;
}

inline
bool MyInplaceTestObject::operator==(MyInplaceTestObject const& rhs) const
{
    return d_a01 == rhs.d_a01 &&
           d_a01 == rhs.d_a01 &&
           d_a02 == rhs.d_a02 &&
           d_a03 == rhs.d_a03 &&
           d_a04 == rhs.d_a04 &&
           d_a05 == rhs.d_a05 &&
           d_a06 == rhs.d_a06 &&
           d_a07 == rhs.d_a07 &&
           d_a08 == rhs.d_a08 &&
           d_a09 == rhs.d_a09 &&
           d_a10 == rhs.d_a10 &&
           d_a11 == rhs.d_a11 &&
           d_a12 == rhs.d_a12 &&
           d_a13 == rhs.d_a13 &&
           d_a14 == rhs.d_a14;
}

// ACCESSORS
inline
const ArgType01& MyInplaceTestObject::arg01() const
{
    return d_a01;
}

inline
const ArgType02& MyInplaceTestObject::arg02() const
{
    return d_a02;
}

inline
const ArgType03& MyInplaceTestObject::arg03() const
{
    return d_a03;
}

inline
const ArgType04& MyInplaceTestObject::arg04() const
{
    return d_a04;
}

inline
const ArgType05& MyInplaceTestObject::arg05() const
{
    return d_a05;
}

inline
const ArgType06& MyInplaceTestObject::arg06() const
{
    return d_a06;
}

inline
const ArgType07& MyInplaceTestObject::arg07() const
{
    return d_a07;
}

inline
const ArgType08& MyInplaceTestObject::arg08() const
{
    return d_a08;
}

inline
const ArgType09& MyInplaceTestObject::arg09() const
{
    return d_a09;
}

inline
const ArgType10& MyInplaceTestObject::arg10() const
{
    return d_a10;
}

inline
const ArgType11& MyInplaceTestObject::arg11() const
{
    return d_a11;
}

inline
const ArgType12& MyInplaceTestObject::arg12() const
{
    return d_a12;
}

inline
const ArgType13& MyInplaceTestObject::arg13() const
{
    return d_a13;
}

inline
const ArgType14& MyInplaceTestObject::arg14() const
{
    return d_a14;
}

#if 0  // TBD Need an appropriately levelized usage example
                              // ================
                              // MySharedDatetime
                              // ================

class MySharedDatetime {
    // This class provide a reference counted smart pointer to support shared
    // ownership of a 'bdlt::Datetime' object.

  private:
    bdlt::Datetime      *d_ptr_p;  // pointer to the managed object
    bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object

  private:
    // NOT IMPLEMENTED
    MySharedDatetime& operator=(const MySharedDatetime&);

  public:
    // CREATORS
    MySharedDatetime();
        // Create an empty shared datetime.

    MySharedDatetime(bdlt::Datetime* ptr, bslma::SharedPtrRep* rep);
        // Create a shared datetime that adopts ownership of the specified
        // 'ptr' and the specified 'rep.

    MySharedDatetime(const MySharedDatetime& original);
        // Create a shared datetime that refers to the same object managed by
        // the specified 'original'

    ~MySharedDatetime();
        // Destroy this shared datetime and release the reference any object it
        // might be referring to.  If this is the last shared reference,
        // deleted the managed object.

    // MANIPULATORS
    void createInplace(bslma::Allocator *allocator,
                       int               year,
                       int               month,
                       int               day);
        // Create a new 'MySharedDatetimeRepImpl', using the specified
        // 'allocator' to supply memory, using the specified 'year', 'month'
        // and 'day' to initialize the 'bdlt::Datetime' within the newly
        // created 'MySharedDatetimeRepImpl', and make this 'MySharedDatetime'
        // refer to the 'bdlt::Datetime'.

    bdlt::Datetime& operator*() const;
        // Return a modifiable reference to the shared 'bdlt::Datetime' object.

    bdlt::Datetime *operator->() const;
        // Return the address of the modifiable 'bdlt::Datetime' to which this
        // object refers.

    bdlt::Datetime *ptr() const;
        // Return the address of the modifiable 'bdlt::Datetime' to which this
        // object refers.
};

                              // ----------------
                              // MySharedDatetime
                              // ----------------

MySharedDatetime::MySharedDatetime()
: d_ptr_p(0)
, d_rep_p(0)
{
}

MySharedDatetime::MySharedDatetime(bdlt::Datetime      *ptr,
                                   bslma::SharedPtrRep *rep)
: d_ptr_p(ptr)
, d_rep_p(rep)
{
}

MySharedDatetime::MySharedDatetime(const MySharedDatetime& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_ptr_p) {
        d_rep_p->acquireRef();
    } else {
        d_rep_p = 0;
    }
}

MySharedDatetime::~MySharedDatetime()
{
    if (d_rep_p) {
        d_rep_p->releaseRef();
    }
}

void MySharedDatetime::createInplace(bslma::Allocator *allocator,
                                     int               year,
                                     int               month,
                                     int               day)
{
    allocator = bslma::Default::allocator(allocator);
    bslma::SharedPtrInplaceRep<bdlt::Datetime> *rep = new (*allocator)
                          bslma::SharedPtrInplaceRep<bdlt::Datetime>(allocator,
                                                                     year,
                                                                     month,
                                                                     day);
    MySharedDatetime temp(rep->ptr(), rep);
    bsl::swap(d_ptr_p, temp.d_ptr_p);
    bsl::swap(d_rep_p, temp.d_rep_p);
}

bdlt::Datetime& MySharedDatetime::operator*() const {
    return *d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::operator->() const {
    return d_ptr_p;
}

bdlt::Datetime *MySharedDatetime::ptr() const {
    return d_ptr_p;
}
#endif


                       // ================
                       // class TestDriver
                       // ================

class TestDriver {
    // This class provide a namespace for testing the 'MyInplaceTestObject'.
    // Each "testCase*" method tests a specific aspect of 'MyInplaceTestObject'.

  private:
    // PRIVATE TYPES
    typedef bslma::SharedPtrInplaceRep<MyInplaceTestObject> Obj;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to create the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be used as the constructor arguments at the same position.
    // Character ' ' (space) corresponds to a default-constructed argument
    // value.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::= ""
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z' | <DEFAULT>
    //                  // unique but otherwise arbitrary
    //
    // <DEFAULT>    ::= ' ' (space)
    //                  // Default-constructed value
    //
    // For specification string of length 'N' use object constructor taking
    // exactly 'N' arguments with values corresponding to the character at the
    // character's position.
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Construct default object.
    // "A"          Construct the object with a single argument corresponding
    //              to A.
    // "ABC"        Construct the object with three arguments corresponding to
    //              A, B, and C, respectively.
    //-------------------------------------------------------------------------

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type)
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return  t;
    }

#if 0
    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Construct the specified 'object' according to the specified 'spec',
        // using the object constructor.  Optionally specify a zero 'verbose'
        // to suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value
        // constructed according to the specified 'spec'.

  public:
    // TEST CASES
    static void testCase12();
        // Test 'getNumDeletes' class method.

    static void testCase11();
        // Test 'isEqual' method.

    static void testCase9();
        // Test copy-assignment operator.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality and inequality operators ('operator==', 'operator!=').

    static void testCase4();
        // Test basic accessors.

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.
#endif
  public:
    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10,
              int N11,
              int N12,
              int N13,
              int N14>
    static void testCase3();
        // Test value constructors for the specified (template parameter)
        // number of arguments.  See the test case function for documented
        // concerns and test plan.
};


template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10,
          int N11,
          int N12,
          int N13,
          int N14>
void TestDriver::testCase3()
{
    // ------------------------------------------------------------------------
    // VALUE CONSTRUCTOR TEST HELPER
    //   This helper exercises value constructors that take 0..14 arguments.
    //
    // Concerns:
    //: 1 Constructor arguments are correcty passed to the corresponding
    //:   attributes of the object.
    //
    // Plan:
    //: 1 Create 14 argument values.
    //:
    //: 2 Based on the (first) template parameter indicating the number of
    //:   arguments to pass in, call the value constructor with the
    //:   corresponding number of arguments, performing an explicit move
    //:   of the argument if so indicated by the template parameter
    //:   corresponding to the argument.
    //:
    //: 3 Verify that the argument values were passed correctly.
    //:
    //: 4 Verify that the move-state for each argument is as expected.
    // ------------------------------------------------------------------------

    bslma::TestAllocator ta("Test case 3");
    bsls::Types::Int64 numDeallocations = ta.numAllocations();
    bsls::Types::Int64 numAllocations   = ta.numDeallocations();

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_11, N11 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_12, N12 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_13, N13 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_14, N14 == 1);

    ArgType01 A01(V01);
    ArgType02 A02(V02);
    ArgType03 A03(V03);
    ArgType04 A04(V04);
    ArgType05 A05(V05);
    ArgType06 A06(V06);
    ArgType07 A07(V07);
    ArgType08 A08(V08);
    ArgType09 A09(V09);
    ArgType10 A10(V10);
    ArgType11 A11(V11);
    ArgType12 A12(V12);
    ArgType13 A13(V13);
    ArgType14 A14(V14);

    Obj* xPtr = 0;
    switch (N_ARGS) {
      case 0: {
        xPtr = new(ta) Obj(&ta);
      } break;
      case 1: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01));
      } break;
      case 2: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02));
      } break;
      case 3: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03));
      } break;
      case 4: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04));
      } break;
      case 5: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05));
      } break;
      case 6: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06));
      } break;
      case 7: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07));
      } break;
      case 8: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08));
      } break;
      case 9: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09));
      } break;
      case 10: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10));
      } break;
      case 11: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10),
                            testArg(A11, MOVE_11));
      } break;
      case 12: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10),
                            testArg(A11, MOVE_11),
                            testArg(A12, MOVE_12));
      } break;
      case 13: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10),
                            testArg(A11, MOVE_11),
                            testArg(A12, MOVE_12),
                            testArg(A13, MOVE_13));
      } break;
      case 14: {
        xPtr = new(ta) Obj(&ta,
                            testArg(A01, MOVE_01),
                            testArg(A02, MOVE_02),
                            testArg(A03, MOVE_03),
                            testArg(A04, MOVE_04),
                            testArg(A05, MOVE_05),
                            testArg(A06, MOVE_06),
                            testArg(A07, MOVE_07),
                            testArg(A08, MOVE_08),
                            testArg(A09, MOVE_09),
                            testArg(A10, MOVE_10),
                            testArg(A11, MOVE_11),
                            testArg(A12, MOVE_12),
                            testArg(A13, MOVE_13),
                            testArg(A14, MOVE_14));
      } break;
      default: {
        ASSERTV(0);
      } return;
    }

    const MyInplaceTestObject& EXP = *xPtr->ptr();

    ASSERT(++numAllocations == ta.numAllocations());
    ASSERT(EXP == *(xPtr->ptr()));

    ASSERTV(MOVE_01, A01.movedFrom(),
                           MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()));
    ASSERTV(MOVE_02, A02.movedFrom(),
                           MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()));
    ASSERTV(MOVE_03, A03.movedFrom(),
                           MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()));
    ASSERTV(MOVE_04, A04.movedFrom(),
                           MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()));
    ASSERTV(MOVE_05, A05.movedFrom(),
                           MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()));
    ASSERTV(MOVE_06, A06.movedFrom(),
                           MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()));
    ASSERTV(MOVE_07, A07.movedFrom(),
                           MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()));
    ASSERTV(MOVE_08, A08.movedFrom(),
                           MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()));
    ASSERTV(MOVE_09, A09.movedFrom(),
                           MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()));
    ASSERTV(MOVE_10, A10.movedFrom(),
                           MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()));
    ASSERTV(MOVE_11, A11.movedFrom(),
                           MOVE_11 == (MoveState::e_MOVED == A11.movedFrom()));
    ASSERTV(MOVE_12, A12.movedFrom(),
                           MOVE_12 == (MoveState::e_MOVED == A12.movedFrom()));
    ASSERTV(MOVE_13, A13.movedFrom(),
                           MOVE_13 == (MoveState::e_MOVED == A13.movedFrom()));
    ASSERTV(MOVE_14, A14.movedFrom(),
                           MOVE_14 == (MoveState::e_MOVED == A14.movedFrom()));


    ASSERTV(V01, EXP.arg01(), V01 == EXP.arg01() || 2 == N01);
    ASSERTV(V02, EXP.arg02(), V02 == EXP.arg02() || 2 == N02);
    ASSERTV(V03, EXP.arg03(), V03 == EXP.arg03() || 2 == N03);
    ASSERTV(V04, EXP.arg04(), V04 == EXP.arg04() || 2 == N04);
    ASSERTV(V05, EXP.arg05(), V05 == EXP.arg05() || 2 == N05);
    ASSERTV(V06, EXP.arg06(), V06 == EXP.arg06() || 2 == N06);
    ASSERTV(V07, EXP.arg07(), V07 == EXP.arg07() || 2 == N07);
    ASSERTV(V08, EXP.arg08(), V08 == EXP.arg08() || 2 == N08);
    ASSERTV(V09, EXP.arg09(), V09 == EXP.arg09() || 2 == N09);
    ASSERTV(V10, EXP.arg10(), V10 == EXP.arg10() || 2 == N10);
    ASSERTV(V11, EXP.arg11(), V11 == EXP.arg11() || 2 == N11);
    ASSERTV(V12, EXP.arg12(), V12 == EXP.arg12() || 2 == N12);
    ASSERTV(V13, EXP.arg13(), V13 == EXP.arg13() || 2 == N13);
    ASSERTV(V14, EXP.arg14(), V14 == EXP.arg14() || 2 == N14);

    xPtr->disposeRep();
    ASSERT(++numDeallocations == ta.numDeallocations());
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;      // silence unused variable warnings
    (void)veryVeryVerbose;  // silence unused variable warnings

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    bslma::TestAllocator ta(veryVeryVeryVerbose);
    bsls::Types::Int64 numDeallocations = 0;
    bsls::Types::Int64 numAllocations   = 0;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#if 0  // TBD Need an appropriately levelized usage example
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concern:
        //   Usage example described in header doc compiles and run.
        //
        // Plan:
        //   Copy the usage example and strip the comments.  Then create simple
        //   test case to use the implementation described in doc.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------
        if (verbose) printf(endl
                        << "Testing Usage Example\n"
                        << "=====================\n");
        {
            MySharedDatetime dt1;
            ASSERT(0 == ta.numAllocations());

            dt1.createInplace(&ta, 2011, 1, 1);

            ASSERT(1 == ta.numAllocations());

            MySharedDatetime dt2(dt1);

            ASSERT(dt2.ptr() == dt1.ptr());
            ASSERT(*dt2 == *dt1);

            dt2->addDays(31);
            ASSERT(2 == dt2->month());
            ASSERT(*dt1 == *dt2);
            ASSERT(0 == ta.numDeallocations());
        }
        ASSERT(1 == ta.numDeallocations());
      } break;
#endif
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'releaseRef' and 'releaseWeakRef'
        //
        // Concerns:
        //: 1 'releaseRef' and 'releaseWeakRef' is decrementing the reference
        //:     count correctly.
        //:
        //: 2 disposeObject() is called when there is no shared reference.
        //:
        //: 3 disposeRep() is called only when there is no shared reference and
        //:   no weak reference.
        //
        // Plan:
        //: 1 Call 'acquireRef' then 'releaseRef' and verify 'numReference' did
        //:   not change.  Call 'acquireWeakRef' then 'releaseWeakRef' and
        //:   verify 'numWeakReference' did not change.
        //:
        //: 2 Call 'releaseRef' when there is only one reference remaining.
        //:   Then verify that both 'disposeObject' and 'disposeRep' is called.
        //:
        //: 3 Create another object and call 'acquireWeakRef' before calling
        //:   'releaseRef'.  Verify that only 'disposeObject' is called.  Then
        //:   call 'releaseWeakRef' and verify that 'disposeRep' is called.
        //
        // Testing:
        //   void releaseRef();
        //   void releaseWeakRef();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'releaseRef' and 'releaseWeakRef'"
                            "\n=========================================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireRef();
            x.releaseRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            x.acquireWeakRef();
            x.releaseWeakRef();

            ASSERT(1 == X.numReferences());
            ASSERT(0 == X.numWeakReferences());
            ASSERT(true == X.hasUniqueOwner());

            if (verbose) printf(
                        "\nTesting 'releaseRef' with no weak reference'"
                        "\n--------------------------------------------\n");

            x.releaseRef();

            ASSERT(1 == numDeletes);
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
        if (verbose) printf("\nTesting 'releaseRef' with weak reference'"
                            "\n-----------------------------------------\n");

        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);
            Obj& x = *xPtr;
            Obj const& X = *xPtr;

            ASSERT(++numAllocations == ta.numAllocations());

            x.acquireWeakRef();
            x.releaseRef();

            ASSERT(0 == X.numReferences());
            ASSERT(1 == X.numWeakReferences());
            ASSERT(false == X.hasUniqueOwner());
            ASSERT(1 == numDeletes);
            ASSERT(numDeallocations == ta.numDeallocations());

            x.releaseWeakRef();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'disposeObject'
        //
        // Concerns:
        //   The destructor of the object is called when 'disposeObject' is
        //   called.
        //
        // Plan:
        //   Call 'disposeObject' and verify that the destructor is called.
        //
        // Testing:
        //   void disposeObject();
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'disposeObject'"
                            "\n=======================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            int numDeletes = 0;
            Obj* xPtr = new(ta) Obj(&ta, &numDeletes);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(0 == numDeletes);

            xPtr->disposeObject();
            ASSERT(1 == numDeletes);

            xPtr->disposeRep();
            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   All constructor is able to initialize the object correctly.
        //
        // Plan:
        //   Call all 14 different constructors and supply it with the
        //   appropriate arguments.  Then verify that the object created inside
        //   the representation is initialized using the arguments supplied.
        //
        // Testing:
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a1);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a2);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a3);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a4);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a5);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a6);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a7);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a8);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a9);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a10);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a11);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a12);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a13);
        //   SharedPtrInplaceRep(bslma::Allocator *allocator, ...a14);
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
        if (verbose) printf("\nTesting constructor with no arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 1 argument"
                            "\n-----------------------------------\n");
        TestDriver::testCase3<1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<1,1,2,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 2 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<2,0,1,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<2,1,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<2,1,1,2,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 3 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<3,1,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<3,0,1,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<3,0,0,1,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<3,1,1,1,2,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 4 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<4,1,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<4,0,1,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<4,0,0,1,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<4,0,0,0,1,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<4,1,1,1,1,2,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 5 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,1,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,0,1,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,0,0,1,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,0,0,0,1,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,0,0,0,0,1,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<5,1,1,1,1,1,2,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 6 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,1,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,0,1,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,0,0,1,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,0,0,0,1,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,0,0,0,0,1,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,0,0,0,0,0,1,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3<6,1,1,1,1,1,1,2,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 7 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,1,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,1,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,0,1,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,0,0,1,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,0,0,0,1,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,0,0,0,0,1,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,0,0,0,0,0,0,1,2,2,2,2,2,2,2>();
        TestDriver::testCase3<7,1,1,1,1,1,1,1,2,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 8 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,1,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,1,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,1,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,0,1,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,0,0,1,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,0,0,0,1,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,0,0,0,0,1,0,2,2,2,2,2,2>();
        TestDriver::testCase3<8,0,0,0,0,0,0,0,1,2,2,2,2,2,2>();
        TestDriver::testCase3<8,1,1,1,1,1,1,1,1,2,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 9 arguments"
                            "\n------------------------------------\n");
        TestDriver::testCase3<9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,1,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,1,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,1,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,1,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,0,1,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,0,0,1,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,0,0,0,1,0,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,0,0,0,0,1,0,2,2,2,2,2>();
        TestDriver::testCase3<9,0,0,0,0,0,0,0,0,1,2,2,2,2,2>();
        TestDriver::testCase3<9,1,1,1,1,1,1,1,1,1,2,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 10 arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,1,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,1,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,1,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,1,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,1,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,1,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,0,1,0,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,0,0,1,0,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,0,0,0,1,0,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,0,0,0,0,1,2,2,2,2>();
        TestDriver::testCase3<10,1,1,1,1,1,1,1,1,1,1,2,2,2,2>();

        if (verbose) printf("\nTesting constructor with 11 arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,1,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,1,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,1,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,1,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,1,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,1,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,1,0,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,0,1,0,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,0,0,1,0,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,0,0,0,1,0,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,0,0,0,0,1,2,2,2>();
        TestDriver::testCase3<11,1,1,1,1,1,1,1,1,1,1,1,2,2,2>();

        if (verbose) printf("\nTesting constructor with 12 arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,1,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,1,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,1,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,1,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,1,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,1,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,1,0,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,1,0,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,1,0,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,0,1,0,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,0,0,1,0,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,0,0,0,1,2,2>();
        TestDriver::testCase3<12,1,1,1,1,1,1,1,1,1,1,1,1,2,2>();

        if (verbose) printf("\nTesting constructor with 13 arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,1,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,1,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,1,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,1,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,1,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,1,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,1,0,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,1,0,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,1,0,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,1,0,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,0,1,0,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,0,0,1,0,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,0,0,0,1,2>();
        TestDriver::testCase3<13,1,1,1,1,1,1,1,1,1,1,1,1,1,2>();

        if (verbose) printf("\nTesting constructor with 14 arguments"
                            "\n-------------------------------------\n");
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,1,0,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,1,0,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,1,0,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,1,0,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,1,0,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,1,0,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,1,0,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,1,0,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,1,0,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,1,0,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,1,0,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,0,1,0,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,0,0,1,0>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,0,0,0,1>();
        TestDriver::testCase3<14,1,1,1,1,1,1,1,1,1,1,1,1,1,1>();
#else
        TestDriver::testCase3< 0,2,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 1,0,2,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 2,0,0,2,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 3,0,0,0,2,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 4,0,0,0,0,2,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 5,0,0,0,0,0,2,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 6,0,0,0,0,0,0,2,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 7,0,0,0,0,0,0,0,2,2,2,2,2,2,2>();
        TestDriver::testCase3< 8,0,0,0,0,0,0,0,0,2,2,2,2,2,2>();
        TestDriver::testCase3< 9,0,0,0,0,0,0,0,0,0,2,2,2,2,2>();
        TestDriver::testCase3<10,0,0,0,0,0,0,0,0,0,0,2,2,2,2>();
        TestDriver::testCase3<11,0,0,0,0,0,0,0,0,0,0,0,2,2,2>();
        TestDriver::testCase3<12,0,0,0,0,0,0,0,0,0,0,0,0,2,2>();
        TestDriver::testCase3<13,0,0,0,0,0,0,0,0,0,0,0,0,0,2>();
        TestDriver::testCase3<14,0,0,0,0,0,0,0,0,0,0,0,0,0,0>();
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR
        //
        // Concerns:
        //   Verify that upon construction the object is properly initialized,
        //
        // Plan:
        //   Construct object using basic constructor and verify that accessors
        //   return the expected values.
        //
        // Testing:
        //   bslma::SharedPtrInplaceRep(bslma::Allocator *basicAllocator);
        //   TYPE *ptr();
        //   void disposeRep();
        //   void *originalPtr() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC CONSTRUCTOR"
                            "\n=========================\n");

        numAllocations = ta.numAllocations();
        numDeallocations = ta.numDeallocations();
        {
            static const MyInplaceTestObject EXP;

            // Dynamically allocate object as the destructor is declared as
            // private.

            TCObj* xPtr = new(ta) TCObj(&ta);

            ASSERT(++numAllocations == ta.numAllocations());
            ASSERT(EXP == *(xPtr->ptr()));
            ASSERT(xPtr->originalPtr() == static_cast<void*>(xPtr->ptr()));

            // Manually deallocate the representation using 'disposeRep'.

            xPtr->disposeRep();

            ASSERT(++numDeallocations == ta.numDeallocations());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   This test exercises basic functionality but tests nothing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
