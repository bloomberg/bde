// bdlb_variant.t.cpp                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_variant.h>

#include <bdlb_print.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_issame.h>
#include <bslmf_typelist.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#undef SS  // Solaris 5.10/x86 sys/regset.h via stdlib.h
#undef ES
#undef GS

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            XLC Warning
//                            -----------
// There is a large set of template instantiations located at the bottom of
// the file.  This was recommended by IBM support as a work-around for a
// a xlc compiler crash caused by test-case 18.
// 
//
//                              Overview
//                              --------
// The component under test is a value-semantic "variant" type, whose state is
// a type identifier (an index into an underlying type list) and a value of the
// corresponding type.  In addition, there are several versions of the variant
// type ('bdlb::VariantN') accepting different numbers of template arguments,
// and a basic version that accepts a type list ('bdlb::VariantImp').  The
// latter actually contains the implementation (hence its name).
//
// We have chosen the primary manipulators for the 'bdlb::Variant' class to be
// 'assign' and 'reset'.  The basic accessors are 'is<TYPE>', 'the<TYPE>', and
// 'typeIndex'.
//
//                      // ----------------------
//                      // class bdlb::VariantImp
//                      // ----------------------
//
// CREATORS:
// [ 2] bdlb::VariantImp();
// [ 2] bdlb::VariantImp(const TYPE_OR_ALLOCATOR& typeOrAlloc);  // allocator
// [12] bdlb::VariantImp(const TYPE_OR_ALLOCATOR& typeOrAlloc);  // type
// [12] bdlb::VariantImp(const TYPE& value, bslma::Allocator *ba);
// [ 7] bdlb::VariantImp(const bdlb::VariantImp& original, basicAllocator);
// [ 2] ~bdlb::VariantImp();
//
// MANIPULATORS:
// [13] bdlb::VariantImp& operator=(const TYPE& value);
// [ 9] bdlb::VariantImp& operator=(const bdlb::VariantImp& rhs);
// [ 2] void assign(const TYPE& value);
// [11] void assignTo(SOURCE const& value);
// [14] void createInPlace<TYPE>(...);                     // all 15 variations
// [ 2] void reset();
// [ 2] TYPE& the<TYPE>();
//
// // Testing return value aspect of 'apply'.
//
// [15] VISITOR::ResultType apply(VISITOR& visitor);
// [15] VISITOR::ResultType apply(const VISITOR& visitor);
// [15] VISITOR::ResultType applyRaw(const VISITOR& visitor);
// [15] void apply(VISITOR& visitor);
// [15] void apply(const VISITOR& visitor);
// [15] void applyRaw(const VISITOR& visitor);
// [15] RET_TYPE apply<RET_TYPE>(VISITOR& visitor);
// [15] RET_TYPE apply<RET_TYPE>(const VISITOR& visitor);
// [15] RET_TYPE applyRaw<RET_TYPE>(const VISITOR& visitor);
//
// // Testing handling of unset variant aspect of 'apply'.
//
// [16] VISITOR::ResultType apply(VISITOR& visitor, const TYPE& dVal);
// [16] void apply(VISITOR& visitor, const TYPE& dVal);
// [16] RET_TYPE apply(VISITOR& visitor, const TYPE& dVal);
// [16] VISITOR::ResultType applyRaw(VISITOR& visitor);
// [16] void applyRaw(VISITOR& visitor);
// [16] RET_TYPE applyRaw(VISITOR& visitor);
//
// ACCESSORS
// [ 4] bool is<Type>() const;
// [17] bool isUnset() const;
// [ 5] void print(bsl::ostream& stream, int level, int spacesPerLevel) const;
// [ 4] const TYPE& the<TYPE>() const;
// [ 4] int typeIndex() const;
//
// // Testing return value aspect of 'apply'.
//
// [15] VISITOR::ResultType apply(VISITOR& visitor) const;
// [15] VISITOR::ResultType apply(const VISITOR& visitor) const;
// [15] VISITOR::ResultType applyRaw(const VISITOR& visitor) const;
// [15] void apply(VISITOR& visitor) const;
// [15] void apply(const VISITOR& visitor) const;
// [15] void applyRaw(const VISITOR& visitor) const;
// [15] RET_TYPE apply<RET_TYPE>(VISITOR& visitor) const;
// [15] RET_TYPE apply<RET_TYPE>(const VISITOR& visitor) const;
// [15] RET_TYPE applyRaw<RET_TYPE>(const VISITOR& visitor) const;
//
// // Testing handling of unset variant aspect of 'apply'.
//
// [16] VISITOR::ResultType apply(VISITOR& visitor, const TYPE& dVal) const;
// [16] void apply(VISITOR& visitor, const TYPE& dVal) const;
// [16] RET_TYPE apply(VISITOR& visitor, const TYPE& dVal) const;
// [16] VISITOR::ResultType applyRaw(VISITOR& visitor) const;
// [16] void applyRaw(VISITOR& visitor) const;
// [16] RET_TYPE applyRaw(VISITOR& visitor) const;
//
// FREE OPERATORS:
// [ 6] operator==(const bdlb::VariantImp& lhs, const bdlb::VariantImp& rhs);
// [ 6] operator!=(const bdlb::VariantImp& lhs, const bdlb::VariantImp& rhs);
// [ 5] operator<<(bsl::ostream& stream, const bdlb::VariantImp& object);
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [22] USAGE EXAMPLE
// [18] CLASSES: 'bdlb::VariantN', 'bdlb::Variant'
// [ 3] int ggg(bdlb::Variant *o, const char *s, bool verbose);
// [ 3] bdlb::VariantImp& gg(bdlb::VariantImp *o, const char *s);
// [ 8] bdlb::VariantImp   g(const char *spec, const T *value);
// [19] CONCERN: No allocator pointer in object if not necessary.
// [19] CONCERN: No 'bslma::UsesBslmaAllocator' trait when no allocator.
// [19] CONCERN: 'bsl::is_trivially_copyable' trait
// [19] CONCERN: 'bslmf::IsBitwiseMoveable' trait
// [20] CONCERN: 'applyRaw' accepts VISITORs w/o a 'bslmf::Nil' overload.
// [10] Reserved for BDEX streaming.

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;
int verbose;
int veryVerbose;
int veryVeryVerbose;

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
//                   HELPER TYPEDEF/STRUCT/CLASS FOR TESTING
//-----------------------------------------------------------------------------

class  TestAllocObj;
class  TestInt;
class  TestString;
struct TestVoid;

enum {
    UNSET            = 0
  , INT_TYPE         = 1
  , STRING_TYPE      = 2
  , TEST_INT_TYPE    = 3
  , TEST_STRING_TYPE = 4
  , TEST_VOID_TYPE   = 5
};

typedef bslmf::TypeList<int, bsl::string, TestInt,
                        TestString, TestVoid> VariantTypes;

typedef bdlb::VariantImp<VariantTypes>        Obj;

//-----------------------------------------------------------------------------

                               // ==============
                               // class TestVoid
                               // ==============

struct TestVoid {
    // This class has no state, hence no value (all objects of this type
    // compare equal).
};

bool operator==(const TestVoid&, const TestVoid&)
{
    return true;
}

bool operator!=(const TestVoid& lhs, const TestVoid& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestVoid&)
{
    return stream << "TestVoid";
}

//-----------------------------------------------------------------------------

                               // ==================
                               // class TestAllocObj
                               // ==================

class TestAllocObj {
    // This class has no state, hence no value (all objects of this type
    // compare equal).  The object allocates during construction and
    // deallocates during destruction.

    void             *d_data_p;       // holds the memory allocated on
                                      // construction

    bslma::Allocator *d_allocator_p;  // pointer to allocator (held, not owned)

  public:
    // TYPES
    BSLMF_NESTED_TRAIT_DECLARATION(TestAllocObj, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TestAllocObj(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
        d_data_p = d_allocator_p->allocate(1);
    }

    TestAllocObj(const TestAllocObj&, bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(basicAllocator)
    {
        d_data_p = d_allocator_p->allocate(1);
    }

    ~TestAllocObj()
    {
        ASSERT(d_allocator_p);

        d_allocator_p->deallocate(d_data_p);
    }

    // MANIPULATORS
    TestAllocObj& operator=(const TestAllocObj&)
    {
        return *this;
    }
};

bool operator==(const TestAllocObj&, const TestAllocObj&)
{
    return true;
}

bool operator!=(const TestAllocObj& lhs, const TestAllocObj& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestAllocObj&)
{
    stream << "TestAllocObj";
    return stream;
}

//-----------------------------------------------------------------------------

                               // =============
                               // class TestInt
                               // =============

class TestInt {
    // This class, similar to 'TestString' (below), wraps an 'int' value, a
    // reference to which one can obtain using the 'theInt' method.  This class
    // also supports 'bdlb' print methods.

    // DATA
    int d_value;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestInt, bdlb::HasPrintMethod);

    // CREATORS
    explicit TestInt(int value = 0)
    : d_value(value)
    {
    }

    // MANIPULATORS
    TestInt& operator=(int value)
    {
        d_value = value;
        return *this;
    }

    int& theInt()
    {
        return d_value;
    }

    // ACCESSORS
    int theInt() const
    {
        return d_value;
    }

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    {
        if (0 > level) {
            level = -level;
        }
        else {
            bdlb::Print::indent(stream, level, spacesPerLevel);
        }
        stream << "[";
        bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        stream << d_value;
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "]";
        return stream;
    }
};

// FREE OPERATORS
bool operator==(const TestInt& lhs, const TestInt& rhs)
{
    return lhs.theInt() == rhs.theInt();
}

bool operator!=(const TestInt& lhs, const TestInt& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestInt& rhs)
{
    rhs.print(stream, 0, -1);
    return stream;
}

//-----------------------------------------------------------------------------

                               // ================
                               // class TestString
                               // ================

class TestString {
    // This class, similar to 'TestInt' (above), wraps a 'bsl::string' value, a
    // reference to which one can obtain using the 'theString' method.  This
    // class also supports 'bdlb' print methods.

    // DATA
    bsl::string d_value;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestString, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(TestString, bdlb::HasPrintMethod);

    // CREATORS
    explicit TestString(bslma::Allocator *allocator = 0)
    : d_value(allocator)
    {
    }

    explicit TestString(const bsl::string&  value,
                        bslma::Allocator   *allocator = 0)
    : d_value(value, allocator)
    {
    }

    TestString(const TestString& original, bslma::Allocator *allocator = 0)
    : d_value(original.theString(), allocator)
    {
    }

    // MANIPULATORS
    TestString& operator=(const bsl::string& value)
    {
        d_value = value;
        return *this;
    }

    bsl::string& theString()
    {
        return d_value;
    }

    // ACCESSORS
    const bsl::string& theString() const
    {
        return d_value;
    }

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    {
        if (0 > level) {
            level = -level;
        }
        else {
            bdlb::Print::indent(stream, level, spacesPerLevel);
        }
        stream << "[";
        bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        stream << '"' << d_value << '"';
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "]";
        return stream;
    }
};

// FREE OPERATORS
bool operator==(const TestString& lhs, const TestString& rhs)
{
    return lhs.theString() == rhs.theString();
}

bool operator!=(const TestString& lhs, const TestString& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const TestString& rhs)
{
    rhs.print(stream, 0, -1);
    return stream;
}

//-----------------------------------------------------------------------------

                               // =============
                               // class TestArg
                               // =============

template <int N>
class TestArg {
    // This class is identical to 'TestInt' except that it allows for several
    // distinct types (depending on the integral template parameter type 'N').

    // DATA
    int d_value;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestArg, bdlb::HasPrintMethod);

    // CREATORS
    explicit TestArg(int value = 0)
    : d_value(value)
    {
    }

    // MANIPULATORS
    TestArg& operator=(int value)
    {
        d_value = value;
        return *this;
    }

    int& theInt()
    {
        return d_value;
    }

    // ACCESSORS
    int theInt() const
    {
        return d_value;
    }

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const
    {
        if (0 > level) {
            level = -level;
        }
        else {
            bdlb::Print::indent(stream, level, spacesPerLevel);
        }
        stream << "[";
        bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);
        stream << d_value;
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "]";
        return stream;
    }
};

// FREE OPERATORS
template <int N>
bool operator==(const TestArg<N>& lhs, const TestArg<N>& rhs)
{
    return lhs.theInt() == rhs.theInt();
}

template <int N>
bool operator!=(const TestArg<N>& lhs, const TestArg<N>& rhs)
{
    return !(lhs == rhs);
}

template <int N>
bsl::ostream& operator<<(bsl::ostream& stream, const TestArg<N>& rhs)
{
    rhs.print(stream, 0, -1);
    return stream;
}

//-----------------------------------------------------------------------------

                               // ===============
                               // struct Copyable
                               // ===============

enum { MAX_COPYABLE_PARAMETERS = 14 };

struct Copyable {
    // This 'struct' is a simple mechanism for counting the number of copy
    // constructor calls.

    // PUBLIC CLASS DATA
    static bool s_copyConstructorCalled;  // flag indicating whether copy
                                          // constructor was called

    // PUBLIC DATA
    bool d_arguments[MAX_COPYABLE_PARAMETERS];  // the 14 arguments

    Copyable(bool a1  = false, bool a2  = false, bool a3  = false,
             bool a4  = false, bool a5  = false, bool a6  = false,
             bool a7  = false, bool a8  = false, bool a9  = false,
             bool a10 = false, bool a11 = false, bool a12 = false,
             bool a13 = false, bool a14 = false)
    {
        d_arguments[0]  = a1;  d_arguments[1]  = a2;  d_arguments[2]  = a3;
        d_arguments[3]  = a4;  d_arguments[4]  = a5;  d_arguments[5]  = a6;
        d_arguments[6]  = a7;  d_arguments[7]  = a8;  d_arguments[8]  = a9;
        d_arguments[9]  = a10; d_arguments[10] = a11; d_arguments[11] = a12;
        d_arguments[12] = a13; d_arguments[13] = a14;
    }

    Copyable(int)  // IMPLICIT
    {
    }

    Copyable(const Copyable&)
    {
        s_copyConstructorCalled = true;
    }
};

// PUBLIC CLASS DATA
bool Copyable::s_copyConstructorCalled = false;

void checkCopyableParameters(const Copyable& object, int numTrue)
    // Helper function that checks the specified 'numTrue' number of 'true'
    // parameters (in increasing order of the argument list) in the specified
    // 'Copyable' 'object'.
{
    for (int i = 0; i < MAX_COPYABLE_PARAMETERS; ++i) {
        LOOP2_ASSERT(i, numTrue, (i < numTrue) == object.d_arguments[i]);
    }
}

//-----------------------------------------------------------------------------

                             // ======================
                             // struct BitwiseCopyable
                             // ======================

template <int index>
struct BitwiseCopyable
    // This struct has the bitwise-copyable trait declared and is used for
    // testing traits in the variant.
{
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyable,
                                   bsl::is_trivially_copyable);

    // DATA
    int d_x;  // take up space
};

//-----------------------------------------------------------------------------

                             // ======================
                             // struct BitwiseMoveable
                             // ======================

template <int index>
struct BitwiseMoveable
    // This struct has the bitwise-moveable trait declared and is used for
    // testing traits in the variant.
{
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMoveable, bslmf::IsBitwiseMoveable);

    // DATA
    int d_x;  // take up space
};

//-----------------------------------------------------------------------------

                               // ====================
                               // struct UsesAllocator
                               // ====================

template <int index>
struct UsesAllocator
    // This struct has the uses-allocator trait declared and is used for
    // testing traits in the variant.
{
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocator, bslma::UsesBslmaAllocator);

    // DATA
    bslma::Allocator *allocator;
};

//-----------------------------------------------------------------------------

                                  // ================
                                  // struct NilTraits
                                  // ================

template <int index>
struct NilTraits
    // This struct has no traits declared and is used for testing traits in the
    // variant.
{
    // DATA
    int d_x;  // take up space
};

//=============================================================================
//                        GLOBAL CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

const int         VA = 123;
const int         VB = 456;
const int         VC = 789;
const int         VD = 147;
const int         VE = 369;

const TestInt     VF(123);
const TestInt     VG(456);
const TestInt     VH(789);
const TestInt     VI(147);
const TestInt     VJ(369);

const TestString  VK("StringK");
const TestString  VL("StringL");
const TestString  VM("StringM");
const TestString  VN("StringN");
const TestString  VO("StringO");

const bsl::string VS("StringS");
const bsl::string VT("StringT");
const bsl::string VU("StringU");
const bsl::string VV("StringV");
const bsl::string VW("StringW");

const int         INT_DATA[]         = { VA, VB, VC, VD, VE };
                                                     // 0 for default construct
const TestInt     TEST_INT_DATA[]    = { VF, VG, VH, VI, VJ };
const bsl::string STRING_DATA[]      = { VS, VT, VU, VV, VW };
const TestString  TEST_STRING_DATA[] = { VK, VL, VM, VN, VO };

//=============================================================================
//                      WRAPPERS AND VISITORS FOR TESTING
//-----------------------------------------------------------------------------

                          // ==================
                          // struct Convertible
                          // ==================

struct Convertible {
    // This struct is convertible from any type.  This is used to facilitate
    // testing for explicit return type specification on the 'apply' method of
    // the variant.  For more details, see the test plan for case 15.

    template <class TYPE>
    Convertible(const TYPE &)  // IMPLICIT
    {
    }

    int memberFunc()
    {
        // Return something to prevent the compiler from optimizing away the
        // function.

        static int x = 999;
        return x++;
    }
};

//-----------------------------------------------------------------------------

                          // =========================
                          // class my_ModifyingVisitor
                          // =========================

class my_ModifyingVisitor {
    // This class modifies the value of the visitor according to the type of
    // object being passed in.  This visitor only supports 4 types: 'int',
    // 'TestInt', 'bsl::string', and 'TestString'.

    // DATA
    int d_valueIdx;  // value index

  public:
    // CREATORS
    explicit my_ModifyingVisitor(int valueIdx)
        // Modifies the value of the variant visited to the value at the
        // specified 'valueIdx' in the test data.
    : d_valueIdx(valueIdx)
    {
    }

    // ACCESSORS
    void operator()(int& value) const
    {
        value = INT_DATA[d_valueIdx];
    }

    void operator()(TestInt& value) const
    {
        value = TEST_INT_DATA[d_valueIdx];
    }

    void operator()(bsl::string& value) const
    {
        value = STRING_DATA[d_valueIdx];
    }

    void operator()(TestString& value) const
    {
        value = TEST_STRING_DATA[d_valueIdx];
    }

    void operator()(bslmf::Nil) const
    {
        ASSERT("Should never be here!");
    }
};

//-----------------------------------------------------------------------------

                          // ===============================
                          // class my_DefaultNoReturnVisitor
                          // ===============================

class my_DefaultNoReturnVisitor {
    // This class modifies the value of the visitor according to the type of
    // object being passed in and returns the type index of the type modified.
    // This visitor only supports 4 types: 'int', 'TestInt', 'bsl::string', and
    // 'TestString'.

    // DATA
    my_ModifyingVisitor d_visitor;  // implementation of visitor

  public:
    // CREATORS
    explicit my_DefaultNoReturnVisitor(int valueIdx)
        // Modifies the value of the variant visited to the value at the
        // specified 'valueIdx' in the test data.
    : d_visitor(valueIdx)
    {
    }

    // ACCESSORS
    int operator()(int& value) const
    {
        d_visitor(value);
        return INT_TYPE;
    }

    int operator()(TestInt& value) const
    {
        d_visitor(value);
        return TEST_INT_TYPE;
    }

    int operator()(bsl::string& value) const
    {
        d_visitor(value);
        return STRING_TYPE;
    }

    int operator()(TestString& value) const
    {
        d_visitor(value);
        return TEST_STRING_TYPE;
    }

    int operator()(bslmf::Nil) const
    {
        ASSERT("Should never be here!");
        return -1;
    }
};

//-----------------------------------------------------------------------------

                          // =========================
                          // class my_ReturningVisitor
                          // =========================

class my_ReturningVisitor : public my_DefaultNoReturnVisitor {
    // This class modifies the value of the visitor according to the type of
    // object being passed in and returns the type index of the type modified.
    // Furthermore, this class specifies its 'operator()'s return type with a
    // 'typedef' of 'ResultType'.  This visitor only supports 4 types: 'int',
    // 'TestInt', 'bsl::string', and 'TestString'.

  public:
    // PUBLIC TYPES
    typedef int ResultType;

    explicit my_ReturningVisitor(int valueIdx)
    : my_DefaultNoReturnVisitor(valueIdx)
    {
    }
};

//-----------------------------------------------------------------------------

                          // =====================
                          // class my_ConstVisitor
                          // =====================

class my_ConstVisitor {
    // This class simply verifies the values being passed to the visitor.  This
    // visitor only supports 4 types: 'int', 'TestInt', 'bsl::string', and
    // 'TestString'.

    // DATA
    int d_valueIdx;  // value index

  public:
    // CREATORS
    explicit my_ConstVisitor(int valueIdx)
        // Modifies the value of the variant visited to the value at the
        // specified 'valueIdx' in the test data.
    : d_valueIdx(valueIdx)
    {
    }

    // ACCESSORS
    void operator()(const int& value) const
    {
        ASSERT(INT_DATA[d_valueIdx] == value);
    }

    void operator()(const TestInt& value) const
    {
        ASSERT(TEST_INT_DATA[d_valueIdx] == value);
    }

    void operator()(const bsl::string& value) const
    {
        ASSERT(STRING_DATA[d_valueIdx] == value);
    }

    void operator()(const TestString& value) const
    {
        ASSERT(TEST_STRING_DATA[d_valueIdx] == value);
    }

    void operator()(bslmf::Nil) const
    {
        ASSERT("Should never be here!");
    }
};

//-----------------------------------------------------------------------------

                          // ==============================
                          // class my_ConstReturningVisitor
                          // ==============================

class my_ConstReturningVisitor {
    // This class simply verifies the values being passed to the visitor and
    // returns the type of the value.  This visitor only supports 4 types:
    // 'int', 'TestInt', 'bsl::string', and 'TestString'.

  public:
    // PUBLIC TYPES
    typedef int ResultType;

  private:
    // DATA
    int d_valueIdx;  // value index

  public:
    // CREATORS
    explicit my_ConstReturningVisitor(int valueIdx)
        // Modifies the value of the variant visited to the value at the
        // specified 'valueIdx' in the test data.
    : d_valueIdx(valueIdx)
    {
    }

    // ACCESSORS
    ResultType operator()(const int& value) const
    {
        ASSERT(INT_DATA[d_valueIdx] == value);
        return INT_TYPE;
    }

    ResultType operator()(const TestInt& value) const
    {
        ASSERT(TEST_INT_DATA[d_valueIdx] == value);
        return TEST_INT_TYPE;
    }

    ResultType operator()(const bsl::string& value) const
    {
        ASSERT(STRING_DATA[d_valueIdx] == value);
        return STRING_TYPE;
    }

    ResultType operator()(const TestString& value) const
    {
        ASSERT(TEST_STRING_DATA[d_valueIdx] == value);
        return TEST_STRING_TYPE;
    }

    ResultType operator()(bslmf::Nil) const
    {
        ASSERT("Should never be here!");
        return -1;
    }
};

//-----------------------------------------------------------------------------

                          // ==============================
                          // class my_ConstReturningVisitor
                          // ==============================

class my_UnsetVariantVisitor {
    // This class simply records the type being passed to the visitor.

  public:
    // PUBLIC TYPES
    enum VisitType {
        BSLMF_NIL,  // 'bslmf::Nil'
        TEST_ARG,   // 'TestArg' being passed in
        GENERIC     // not unset
    };

    // DATA
    VisitType d_lastType;  // records the last type the variant invoked
                           // 'operator()' with

    // CREATORS
    my_UnsetVariantVisitor()
    : d_lastType(GENERIC)
    {
    }

    // MANIPULATORS
    template <class TYPE>
    void operator()(const TYPE&)
    {
        d_lastType = GENERIC;
    }

    void operator()(bslmf::Nil)
    {
        d_lastType = BSLMF_NIL;
    }

    void operator()(const TestArg<1>&)
    {
        d_lastType = TEST_ARG;
    }
};

//-----------------------------------------------------------------------------

                        // =========================
                        // class my_NilAssertVisitor
                        // =========================

class my_NilAssertVisitor {
    // This class is crafted to reproduce "The variable nil has not yet been
    // assigned a value" warning on Solaris, where 'nil' refers to an object in
    // the 'bdlb::Variant::apply' method.

    void *d_result_p;

  public:
    my_NilAssertVisitor(void *result)
    : d_result_p(result)
    {
    }

    void operator()(int) const
    {
    }

    void operator()(const bslmf::Nil) const
    {
        BSLS_ASSERT(false);
    }
};

void dummyConvert(void *result, const bdlb::Variant<int>& value)
{
    my_NilAssertVisitor visitor(result);
    value.apply(visitor);
}

//-----------------------------------------------------------------------------

                          // =======================
                          // class my_VariantWrapper
                          // =======================

template <class VARIANT>
class my_VariantWrapper {
    // This class wraps a variant object.  It also implements a similar 'apply'
    // interface as 'bdlb::VariantImp' that is used to keep track of which
    // 'apply' method is invoked.  A reference to the wrapped variant object is
    // accessible through 'theVariant' method.

  public:
    // PUBLIC TYPES
    enum VisitType {
        RESULT_TYPE_VISIT,        // VISITOR::ResultType visitor(...);
        RESULT_TYPE_VISIT_CONST,  // VISITOR::ResultType visitor(...) const;
        VOID_VISIT,               // void visitor(...);
        VOID_VISIT_CONST,         // void visitor(...) const;
        RET_TYPE_VISIT,           // RET_TYPE visitor(...);
        RET_TYPE_VISIT_CONST      // RET_TYPE visitor(...) const;
    };

  private:
    // DATA
    VARIANT           d_variant;        // internal variant implementation
    mutable VisitType d_lastVisitCall;  // last visit function invoked

    // NOT IMPLEMENTED
    my_VariantWrapper(const my_VariantWrapper&, bslma::Allocator * = 0);
    my_VariantWrapper& operator=(const my_VariantWrapper&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(my_VariantWrapper,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    my_VariantWrapper(bslma::Allocator *basicAllocator = 0);
        // Create a wrapper around an unset variant object.

    my_VariantWrapper(const VARIANT&    object,
                      bslma::Allocator *basicAllocator = 0);
        // Create a wrapper around the specified variant 'object'.

    ~my_VariantWrapper();
        // Destroy this wrapper object.

    // MANIPULATORS
    VARIANT& variant();
        // Return a reference providing modifiable access to the variant object
        // held by this wrapper.

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor)
    {
        d_lastVisitCall = RESULT_TYPE_VISIT;
        return d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor)
    {
        d_lastVisitCall = RESULT_TYPE_VISIT;
        return d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor)
    {
        d_lastVisitCall = RESULT_TYPE_VISIT;
        return d_variant.applyRaw(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    apply(VISITOR& visitor)
    {
        d_lastVisitCall = VOID_VISIT;
        d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    apply(const VISITOR& visitor)
    {
        d_lastVisitCall = VOID_VISIT;
        d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    applyRaw(const VISITOR& visitor)
    {
        d_lastVisitCall = VOID_VISIT;
        d_variant.applyRaw(visitor);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor);
        // Wrappers around the 'apply' method of 'bdlb::Variant'.  Records
        // which visit method is invoked.

    // ACCESSORS
    VisitType lastVisited() const { return d_lastVisitCall; }
        // Return the last visitor method invoked on this variant wrapper.

    const VARIANT& variant() const;
        // Return a reference providing non-modifiable access to the variant
        // object held by this wrapper.

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    apply(VISITOR& visitor) const
    {
        d_lastVisitCall = RESULT_TYPE_VISIT_CONST;
        return d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor) const
    {
        d_lastVisitCall = RESULT_TYPE_VISIT_CONST;
        return d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 1,
                          typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor) const
    {
        d_lastVisitCall = RESULT_TYPE_VISIT_CONST;
        return d_variant.applyRaw(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    apply(VISITOR& visitor) const
    {
        d_lastVisitCall = VOID_VISIT_CONST;
        d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    apply(const VISITOR& visitor) const
    {
        d_lastVisitCall = VOID_VISIT_CONST;
        d_variant.apply(visitor);
    }

    template <class VISITOR>
    typename bsl::enable_if<
                          bdlb::Variant_ReturnValueHelper<VISITOR>::value == 0,
                          void>::type
    applyRaw(const VISITOR& visitor) const
    {
        d_lastVisitCall = VOID_VISIT_CONST;
        d_variant.applyRaw(visitor);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor) const;
        // Wrappers around the 'apply' method of 'bdlb::Variant'.  Records
        // which visit method is invoked.
};

// FREE OPERATORS
template <class VARIANT>
bool operator==(const my_VariantWrapper<VARIANT>& lhs,
                const my_VariantWrapper<VARIANT>& rhs);
template <class VARIANT>
bool operator==(const VARIANT&                    lhs,
                const my_VariantWrapper<VARIANT>& rhs);
template <class VARIANT>
bool operator==(const my_VariantWrapper<VARIANT>& lhs,
                const VARIANT&                    rhs);
    // Return 'true' if the (variant object wrapped by the) specified 'lhs' has
    // the same value as the (variant object wrapped by the) specified 'rhs',
    // and 'false' otherwise.

template <class VARIANT>
bool operator!=(const my_VariantWrapper<VARIANT>& lhs,
                const my_VariantWrapper<VARIANT>& rhs);
template <class VARIANT>
bool operator!=(const VARIANT&                    lhs,
                const my_VariantWrapper<VARIANT>& rhs);
template <class VARIANT>
bool operator!=(const my_VariantWrapper<VARIANT>& lhs,
                const VARIANT&                    rhs);
    // Return 'true' if the (variant object wrapped by the) specified 'lhs'
    // does not have the same value as the (variant object wrapped by the)
    // specified 'rhs', and 'false' otherwise.

// CREATORS
template <class VARIANT>
my_VariantWrapper<VARIANT>::my_VariantWrapper(bslma::Allocator *basicAllocator)
: d_variant(basicAllocator)
{
}

template <class VARIANT>
my_VariantWrapper<VARIANT>::my_VariantWrapper(const VARIANT&    object,
                                              bslma::Allocator *basicAllocator)
: d_variant(object, basicAllocator)
{
}

template <class VARIANT>
my_VariantWrapper<VARIANT>::~my_VariantWrapper()
{
}

// MANIPULATORS
template <class VARIANT>
VARIANT& my_VariantWrapper<VARIANT>::variant()
{
    return d_variant;
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::apply(VISITOR& visitor)
{
    d_lastVisitCall = RET_TYPE_VISIT;
    return d_variant.apply(visitor);
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::apply(const VISITOR& visitor)
{
    d_lastVisitCall = RET_TYPE_VISIT;
    return d_variant.apply(visitor);
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::applyRaw(const VISITOR& visitor)
{
    d_lastVisitCall = RET_TYPE_VISIT;
    return d_variant.applyRaw(visitor);
}

// ACCESSORS
template <class VARIANT>
const VARIANT& my_VariantWrapper<VARIANT>::variant() const
{
    return d_variant;
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::apply(VISITOR& visitor) const
{
    d_lastVisitCall = RET_TYPE_VISIT_CONST;
    return d_variant.apply(visitor);
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::apply(const VISITOR& visitor) const
{
    d_lastVisitCall = RET_TYPE_VISIT_CONST;
    return d_variant.apply(visitor);
}

template <class VARIANT>
template <class RET_TYPE, class VISITOR>
RET_TYPE my_VariantWrapper<VARIANT>::applyRaw(const VISITOR& visitor) const
{
    d_lastVisitCall = RET_TYPE_VISIT_CONST;
    return d_variant.applyRaw(visitor);
}

// FREE OPERATORS
template <class VARIANT>
bool operator==(const my_VariantWrapper<VARIANT>& lhs,
                const my_VariantWrapper<VARIANT>& rhs)
{
    return lhs.variant() == rhs.variant();
}

template <class VARIANT>
bool operator==(const VARIANT& lhs, const my_VariantWrapper<VARIANT>& rhs)
{
    return lhs == rhs.variant();
}

template <class VARIANT>
bool operator==(const my_VariantWrapper<VARIANT>& lhs, const VARIANT& rhs)
{
    return lhs.variant() == rhs;
}

template <class VARIANT>
bool operator!=(const my_VariantWrapper<VARIANT>& lhs,
                const my_VariantWrapper<VARIANT>& rhs)
{
    return lhs.variant() != rhs.variant();
}

template <class VARIANT>
bool operator!=(const VARIANT& lhs, const my_VariantWrapper<VARIANT>& rhs)
{
    return lhs != rhs.variant();
}

template <class VARIANT>
bool operator!=(const my_VariantWrapper<VARIANT>& lhs, const VARIANT& rhs)
{
    return lhs.variant() != rhs;
}

template <class VARIANT>
bsl::ostream&
operator<<(bsl::ostream& stream, const my_VariantWrapper<VARIANT>& rhs)
{
    stream << rhs.variant();
    return stream;
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters '[A .. E]' correspond to arbitrary (but unique) 'int' values to be
// assigned to the 'bdlb::Variant' object (thus of type 'INT_TYPE').  Uppercase
// letters '[F .. J]' correspond to arbitrary (but unique) 'TestInt' values to
// be assigned to the 'bdlb::Variant' object (thus of type 'TEST_INT_TYPE').
// Uppercase letters '[S .. W]' correspond to arbitrary (but unique) 'string'
// values to be assigned to the 'bdlb::Variant' object (thus of type
// 'STRING_TYPE').  Uppercase letters '[K .. O]' correspond to arbitrary (but
// unique) 'TestString' values to be assigned to the 'bdlb::Variant' object
// (thus of type 'TEST_STRING_TYPE').  Uppercase letter 'Z' corresponds to an
// object of type 'TestVoid'.  A tilde ('~') indicates that the value of the
// object is to be set to its initial, unset state (via the 'reset' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM>       ::= <ELEMENT> | <RESET>
//
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E'   // 'int'
//                | 'S' | 'T' | 'U' | 'V' | 'W'   // 'string'
//                | 'F' | 'G' | 'H' | 'I' | 'J'   // 'TestInt'
//                | 'K' | 'L' | 'M' | 'N' | 'O'   // 'TestString'
//                | 'Z'                           // 'TestVoid' (unique but
                                                  // otherwise arbitrary)
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unset.
// "A"          Assign the value corresponding to 'A'.
// "AA"         Assign two values both corresponding to 'A'.
// "ABC"        Assign three values corresponding to 'A', 'B', and 'C'.  Note
//              that the generated value of the variant object is equal to 'C'.
//
//-----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, bool verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulators 'assign<>' and 'reset'.  Optionally
    // specify a 'false' 'verboseFlag' to suppress 'spec' syntax error
    // messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allow for verification of syntax error detection.
{
    ASSERT(object);
    ASSERT(spec);
    bool continueParse = true;
    const char *input  = spec;

    for (; *input && continueParse; ++input) {
        switch (*input) {
            // *** 'int' ***
          case 'A': object->assign<int>(VA); break;
          case 'B': object->assign<int>(VB); break;
          case 'C': object->assign<int>(VC); break;
          case 'D': object->assign<int>(VD); break;
          case 'E': object->assign<int>(VE); break;
            // *** 'TestInt' ***
          case 'F': object->assign<TestInt>(VF); break;
          case 'G': object->assign<TestInt>(VG); break;
          case 'H': object->assign<TestInt>(VH); break;
          case 'I': object->assign<TestInt>(VI); break;
          case 'J': object->assign<TestInt>(VJ); break;
            // *** 'TestString' ***
          case 'K': object->assign<TestString>(VK); break;
          case 'L': object->assign<TestString>(VL); break;
          case 'M': object->assign<TestString>(VM); break;
          case 'N': object->assign<TestString>(VN); break;
          case 'O': object->assign<TestString>(VO); break;
            // *** 'bsl::string' ***
          case 'S': object->assign<bsl::string>(VS); break;
          case 'T': object->assign<bsl::string>(VT); break;
          case 'U': object->assign<bsl::string>(VU); break;
          case 'V': object->assign<bsl::string>(VV); break;
          case 'W': object->assign<bsl::string>(VW); break;
            // *** 'TestVoid' ***
          case 'Z': object->assign<TestVoid>(TestVoid()); break;
            // *** reset ***
          case '~': object->reset(); break;
            // ** Parse Error ***
          default: continueParse = false; break;
        }
    }

    if (!continueParse) {
        int idx = static_cast<int>(input - spec) - 1;
        if (verboseFlag) {
            cout << "An error occurred near character ('" << spec[idx]
                 << "') in spec \"" << spec << "\" at position " << idx
                 << '.' << endl;
        }
        return idx;  // Discontinue processing this spec.             // RETURN
    }

    return -1; // All input was consumed.
}

Obj gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above.
{
    ASSERT(object); ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);

    return *object;
}

Obj g(const char *spec)
    // Return, by value, an object with its value adjusted according to the
    // specified 'spec' according to the custom language described above.
{
    ASSERT(spec);

    Obj object;
    return gg(&object, spec);
}

//=============================================================================
//             VISITORs Without a 'bslmf::Nil' Overload (case 20)
//-----------------------------------------------------------------------------

namespace nilvisitor {

struct TestVisitorWithResultType {
    // This test visitor explicitly does not provide an overload for
    // 'bslmf::Nil', and has a non-void 'ResultType'.

    typedef int ResultType;

    ResultType operator()(int) const { return 0; }
};

struct TestVisitorWithoutResultType {
    // This test visitor explicitly does not provide an overload for
    // 'bslmf::Nil', and has no 'ResultType'.

    void operator()(int) const {}
};

struct TestVisitorWithUndeclaredResultType {
    // This test visitor explicitly does not provide an overload for
    // 'bslmf::Nil', and has no 'ResultType'.

     int operator()(int) const { return 0; }
};

}  // close namespace nilvisitor

struct TestUtil {
   // This 'struct' defines several test cases outside of 'main' to avoid
   // out-of-memory errors with the XLC compiler.

   static void testCase21() 
   {
       if (verbose) cout << endl
                         << "TESTING 'swap'" << endl
                         << "==============" << endl;

       bslma::TestAllocator ta(veryVeryVerbose);
       bslma::TestAllocator tb(veryVeryVerbose);

       static struct {
               int         d_lineNum;
               const char *d_input;        // input specifications
               int         d_expTypeIdx;   // expected type index
               int         d_expValueIdx;  // expected value index (within type)
       } DATA[] = {
           // LINE INPUT         TYPE_IDX VALUE_IDX
           // ---- -----         -------- ---------
           { L_, "~",             UNSET,        0 },
           { L_, "A",          INT_TYPE,        0 },
           { L_, "B",          INT_TYPE,        1 },
           { L_, "F",     TEST_INT_TYPE,        0 },
           { L_, "G",     TEST_INT_TYPE,        1 },
           { L_, "S",       STRING_TYPE,        0 },
           { L_, "T",       STRING_TYPE,        1 },
           { L_, "K",  TEST_STRING_TYPE,        0 },
           { L_, "L",  TEST_STRING_TYPE,        1 },
       };

       const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

       for (int ti = 0; ti < NUM_DATA; ++ti) {
           const int   LINE1      = DATA[ti].d_lineNum;
           const char *INPUT1     = DATA[ti].d_input;
           const int   TYPE_IDX1  = DATA[ti].d_expTypeIdx;
           const int   VALUE_IDX1 = DATA[ti].d_expValueIdx;

           if (veryVerbose) {
               T_ P_(LINE1); P_(INPUT1); P_(TYPE_IDX1); P(VALUE_IDX1);
           }

           for (int tj = 0; tj < NUM_DATA; ++tj) {
               const int   LINE2      = DATA[ti].d_lineNum;
               const char *INPUT2     = DATA[ti].d_input;
               const int   TYPE_IDX2  = DATA[ti].d_expTypeIdx;
               const int   VALUE_IDX2 = DATA[ti].d_expValueIdx;

               if (veryVerbose) {
                   T_ P_(LINE2); P_(INPUT2); P_(TYPE_IDX2); P(VALUE_IDX2);
               }

               Obj mX(&ta); const Obj& X = mX;
               int retCode = ggg(&mX, INPUT1, false);
               ASSERT(-1 == retCode);

               Obj mY(&tb); const Obj& Y = mY;
               retCode = ggg(&mY, INPUT2, false);
               ASSERT(-1 == retCode);

               mX.swap(mY);
               swap(mX, mY);  // try the free function as well
               mY.swap(mX);

               switch (X.typeIndex()) {
                 case UNSET: {
                     ASSERT(TYPE_IDX2 == UNSET);
                 } break;
                 case INT_TYPE: {
                     ASSERT(INT_DATA[VALUE_IDX2] == X.the<int>());
                 } break;
                 case TEST_INT_TYPE: {
                     ASSERT(TEST_INT_DATA[VALUE_IDX2] == X.the<TestInt>());
                 } break;
                 case STRING_TYPE: {
                     ASSERT(STRING_DATA[VALUE_IDX2] == X.the<bsl::string>());
                 } break;
                 case TEST_STRING_TYPE: {
                     ASSERT(TEST_STRING_DATA[VALUE_IDX2] ==
                            X.the<TestString>());
                 } break;
                 default: LOOP2_ASSERT(LINE1, LINE2, 0);
               }

               switch (Y.typeIndex()) {
                 case UNSET: {
                     ASSERT(TYPE_IDX1 == UNSET);
                 } break;
                 case INT_TYPE: {
                     ASSERT(INT_DATA[VALUE_IDX1] == Y.the<int>());
                 } break;
                 case TEST_INT_TYPE: {
                     ASSERT(TEST_INT_DATA[VALUE_IDX1] == Y.the<TestInt>());
                 } break;
                 case STRING_TYPE: {
                     ASSERT(STRING_DATA[VALUE_IDX1] == Y.the<bsl::string>());
                 } break;
                 case TEST_STRING_TYPE: {
                     ASSERT(TEST_STRING_DATA[VALUE_IDX1] ==
                            Y.the<TestString>());
                 } break;
                 default: LOOP2_ASSERT(LINE1, LINE2, 0);
               }
           }
       }

   }
   static void testCase20()
   {
       using namespace nilvisitor;

       if (verbose)
           cout << endl
                << "CONCERN: 'applyRaw' & VISITOR w/o Nil overload" << endl
                << "==============================================" << endl;

       typedef bdlb::Variant<int> Variant;

       TestVisitorWithResultType           withResultType;
       TestVisitorWithoutResultType        withoutResultType;
       TestVisitorWithUndeclaredResultType withUndeclaredResultType;

       TestVisitorWithResultType&           vwrt  = withResultType;
       TestVisitorWithoutResultType&        vwort = withoutResultType;
       TestVisitorWithUndeclaredResultType& vwurt = withUndeclaredResultType;
       const TestVisitorWithResultType&           VWRT  = vwrt;
       const TestVisitorWithoutResultType&        VWORT = vwort;
       const TestVisitorWithUndeclaredResultType& VWURT = vwurt;

       // We will test 'applyRaw' on the cross product of:
       //: o 'const' and non-'const' variant.
       //: o 'const' and non-'const' visitor.
       //: o A visitor functor that:
       //:   o Declares a result type.
       //:   o Does not declare a result type and returns 'void'.
       //:   o Does not declare a result type and doesn't return 'void'.

       if (verbose) cout <<
           "\nCall applyRaw using template deduction for the return type."
                         << endl;
       {
           Variant value(1);  const Variant& X = value;

           X.applyRaw(vwrt);
           X.applyRaw(vwort);
           X.applyRaw(vwurt);

           X.applyRaw(VWRT);
           X.applyRaw(VWORT);
           X.applyRaw(VWURT);
       }

       if (verbose) cout <<
           "\nCall applyRaw using template w/o deduction for the return type."
                         << endl;
       {
           // Note that 'applyRaw<TYPE>' cannot have 'void' as the result type.

           Variant value(1);  const Variant& X = value;

           X.applyRaw<int>(vwrt);
           X.applyRaw<int>(vwurt);

           // X.applyRaw<int>(VWRT);
           // X.applyRaw<int>(VWURT);
       }
   }

   static void testCase19()
   {
       if (verbose) cout << endl << "TESTING TRAITS" << endl
                                 << "==============" << endl;

       typedef NilTraits<1>         NT1;    typedef NilTraits<2>         NT2;
       typedef NilTraits<3>         NT3;    typedef NilTraits<4>         NT4;
       typedef NilTraits<5>         NT5;    typedef NilTraits<6>         NT6;
       typedef NilTraits<7>         NT7;    typedef NilTraits<8>         NT8;
       typedef NilTraits<9>         NT9;    typedef NilTraits<10>        NT10;
       typedef NilTraits<11>        NT11;   typedef NilTraits<12>        NT12;
       typedef NilTraits<13>        NT13;   typedef NilTraits<14>        NT14;
       typedef NilTraits<15>        NT15;   typedef NilTraits<16>        NT16;
       typedef NilTraits<17>        NT17;   typedef NilTraits<18>        NT18;
       typedef NilTraits<19>        NT19;   typedef NilTraits<20>        NT20;

       typedef UsesAllocator<1>     UA1;    typedef UsesAllocator<2>     UA2;
       typedef UsesAllocator<3>     UA3;    typedef UsesAllocator<4>     UA4;
       typedef UsesAllocator<5>     UA5;    typedef UsesAllocator<6>     UA6;
       typedef UsesAllocator<7>     UA7;    typedef UsesAllocator<8>     UA8;
       typedef UsesAllocator<9>     UA9;    typedef UsesAllocator<10>    UA10;
       typedef UsesAllocator<11>    UA11;   typedef UsesAllocator<12>    UA12;
       typedef UsesAllocator<13>    UA13;   typedef UsesAllocator<14>    UA14;
       typedef UsesAllocator<15>    UA15;   typedef UsesAllocator<16>    UA16;
       typedef UsesAllocator<17>    UA17;   typedef UsesAllocator<18>    UA18;
       typedef UsesAllocator<19>    UA19;   typedef UsesAllocator<20>    UA20;

       typedef BitwiseCopyable<1>   BC1;    typedef BitwiseCopyable<2>   BC2;
       typedef BitwiseCopyable<3>   BC3;    typedef BitwiseCopyable<4>   BC4;
       typedef BitwiseCopyable<5>   BC5;    typedef BitwiseCopyable<6>   BC6;
       typedef BitwiseCopyable<7>   BC7;    typedef BitwiseCopyable<8>   BC8;
       typedef BitwiseCopyable<9>   BC9;    typedef BitwiseCopyable<10>  BC10;
       typedef BitwiseCopyable<11>  BC11;   typedef BitwiseCopyable<12>  BC12;
       typedef BitwiseCopyable<13>  BC13;   typedef BitwiseCopyable<14>  BC14;
       typedef BitwiseCopyable<15>  BC15;   typedef BitwiseCopyable<16>  BC16;
       typedef BitwiseCopyable<17>  BC17;   typedef BitwiseCopyable<18>  BC18;
       typedef BitwiseCopyable<19>  BC19;   typedef BitwiseCopyable<20>  BC20;

       typedef BitwiseMoveable<1>   BM1;    typedef BitwiseMoveable<2>   BM2;
       typedef BitwiseMoveable<3>   BM3;    typedef BitwiseMoveable<4>   BM4;
       typedef BitwiseMoveable<5>   BM5;    typedef BitwiseMoveable<6>   BM6;
       typedef BitwiseMoveable<7>   BM7;    typedef BitwiseMoveable<8>   BM8;
       typedef BitwiseMoveable<9>   BM9;    typedef BitwiseMoveable<10>  BM10;
       typedef BitwiseMoveable<11>  BM11;   typedef BitwiseMoveable<12>  BM12;
       typedef BitwiseMoveable<13>  BM13;   typedef BitwiseMoveable<14>  BM14;
       typedef BitwiseMoveable<15>  BM15;   typedef BitwiseMoveable<16>  BM16;
       typedef BitwiseMoveable<17>  BM17;   typedef BitwiseMoveable<18>  BM18;
       typedef BitwiseMoveable<19>  BM19;   typedef BitwiseMoveable<20>  BM20;

       if (verbose) cout << "\nSanity check for test driver defined types."
                         << endl;

       ASSERT(true == bslma::UsesBslmaAllocator<UA1>::value);
       ASSERT(true == bslma::UsesBslmaAllocator<UA10>::value);
       ASSERT(true == bslma::UsesBslmaAllocator<UA20>::value);

       ASSERT(true == bsl::is_trivially_copyable<BC1>::value);
       ASSERT(true == bsl::is_trivially_copyable<BC10>::value);
       ASSERT(true == bsl::is_trivially_copyable<BC20>::value);

       ASSERT(true == bslmf::IsBitwiseMoveable<BM1>::value);
       ASSERT(true == bslmf::IsBitwiseMoveable<BM10>::value);
       ASSERT(true == bslmf::IsBitwiseMoveable<BM20>::value);

       if (verbose) cout << "\nTesting size of the variant." << endl;
       {
           if (verbose) cout << "\tWithout types that use allocator." << endl;

           // Arbitrarily choose 3 arguments.
           typedef bdlb::Variant<NT1, NT2, NT3> SmallVariant;

           ASSERT(8 == sizeof(SmallVariant));  //   4 (type index)
           // + 4 (size of largest type)

           if (verbose) cout << "\tWith types that use allocator." << endl;

           typedef bdlb::Variant<UA1, NT2, NT3> BigVar1;
           typedef bdlb::Variant<NT1, UA2, NT3> BigVar2;
           typedef bdlb::Variant<NT1, NT2, UA3> BigVar3;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, UA18, NT19, NT20> BigVar18;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, UA19, NT20> BigVar19;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, NT19, UA20> BigVar20;

#if BSLS_PLATFORM_CPU_64_BIT
           LOOP_ASSERT(sizeof(BigVar1),  24 == sizeof(BigVar1));
           LOOP_ASSERT(sizeof(BigVar2),  24 == sizeof(BigVar2));
           LOOP_ASSERT(sizeof(BigVar3),  24 == sizeof(BigVar3));
           LOOP_ASSERT(sizeof(BigVar18), 24 == sizeof(BigVar18));
           LOOP_ASSERT(sizeof(BigVar19), 24 == sizeof(BigVar19));
           LOOP_ASSERT(sizeof(BigVar20), 24 == sizeof(BigVar20));
#else
           LOOP_ASSERT(sizeof(BigVar1),  12 == sizeof(BigVar1));
           LOOP_ASSERT(sizeof(BigVar2),  12 == sizeof(BigVar2));
           LOOP_ASSERT(sizeof(BigVar3),  12 == sizeof(BigVar3));
           LOOP_ASSERT(sizeof(BigVar18), 12 == sizeof(BigVar18));
           LOOP_ASSERT(sizeof(BigVar19), 12 == sizeof(BigVar19));
           LOOP_ASSERT(sizeof(BigVar20), 12 == sizeof(BigVar20));
#endif
       }

       if (verbose) cout << "\nTesting UsesBslmaAllocator trait." << endl;

       if (verbose) cout << "\tNone uses bslma::Allocator" << endl;
       {
           typedef bdlb::Variant<NT1, NT2, NT3> Obj;

           ASSERT(false == bslma::UsesBslmaAllocator<Obj>::value);
       }

       if (verbose) cout << "\tAll uses bslma::Allocator" << endl;
       {
           typedef bdlb::Variant<UA1, UA2,  UA3,  UA4,  UA5,  UA6,  UA7,  UA8,
                                 UA9, UA10, UA11, UA12, UA13, UA14, UA15,
                                 UA16, UA17, UA18, UA19, UA20> Obj;

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
       }

       if (verbose) cout << "\tSome uses bslma::Allocator" << endl;
       {
           typedef bdlb::Variant<UA1, NT2, NT3> Obj1;
           typedef bdlb::Variant<NT1, UA2, NT3> Obj2;
           typedef bdlb::Variant<NT1, NT2, UA3> Obj3;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, UA18, NT19, NT20> Obj18;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, UA19, NT20> Obj19;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, NT19, UA20> Obj20;

           ASSERT(true == bslma::UsesBslmaAllocator<Obj1>::value);
           ASSERT(true == bslma::UsesBslmaAllocator<Obj2>::value);
           ASSERT(true == bslma::UsesBslmaAllocator<Obj3>::value);
           ASSERT(true == bslma::UsesBslmaAllocator<Obj18>::value);
           ASSERT(true == bslma::UsesBslmaAllocator<Obj19>::value);
           ASSERT(true == bslma::UsesBslmaAllocator<Obj20>::value);
       }

       if (verbose) cout << "\nTesting bitwise copyable trait." << endl;

       if (verbose) cout << "\tNone are bitwise copyable." << endl;
       {
           typedef bdlb::Variant<NT1, NT2, NT3> Obj;

           ASSERT(false == bsl::is_trivially_copyable<Obj>::value);
       }

       if (verbose) cout << "\tAll are bitwise copyable." << endl;
       {
           typedef bdlb::Variant<BC1, BC2,  BC3,  BC4,  BC5,  BC6,  BC7,  BC8,
                                 BC9, BC10, BC11, BC12, BC13, BC14, BC15,
                                 BC16, BC17, BC18, BC19, BC20> Obj;

           ASSERT(true == bsl::is_trivially_copyable<Obj>::value);
       }

       if (verbose) cout << "\tSome are bitwise copyable." << endl;
       {
           typedef bdlb::Variant<BC1, NT2, NT3> Obj1;
           typedef bdlb::Variant<NT1, BC2, NT3> Obj2;
           typedef bdlb::Variant<NT1, NT2, BC3> Obj3;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, BC18, NT19, NT20> Obj18;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, BC19, NT20> Obj19;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, NT19, BC20> Obj20;

           ASSERT(false == bsl::is_trivially_copyable<Obj1>::value);
           ASSERT(false == bsl::is_trivially_copyable<Obj2>::value);
           ASSERT(false == bsl::is_trivially_copyable<Obj3>::value);
           ASSERT(false == bsl::is_trivially_copyable<Obj18>::value);
           ASSERT(false == bsl::is_trivially_copyable<Obj19>::value);
           ASSERT(false == bsl::is_trivially_copyable<Obj20>::value);
       }

       if (verbose) cout << "\nTesting bitwise moveable trait." << endl;

       if (verbose) cout << "\tNone are bitwise moveable." << endl;
       {
           typedef bdlb::Variant<NT1, NT2, NT3> Obj;

           ASSERT(false == bslmf::IsBitwiseMoveable<Obj>::value);
       }

       if (verbose) cout << "\tAll are bitwise moveable." << endl;
       {
           typedef bdlb::Variant<BM1, BM2,  BM3,  BM4,  BM5,  BM6,  BM7,  BM8,
                                 BM9, BM10, BM11, BM12, BM13, BM14, BM15,
                                 BM16, BM17, BM18, BM19, BM20> Obj;

           ASSERT(true == bslmf::IsBitwiseMoveable<Obj>::value);
       }

       if (verbose) cout << "\tSome are bitwise moveable." << endl;
       {
           typedef bdlb::Variant<BC1, NT2, NT3> Obj1;
           typedef bdlb::Variant<NT1, BC2, NT3> Obj2;
           typedef bdlb::Variant<NT1, NT2, BC3> Obj3;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, BC18, NT19, NT20> Obj18;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, BC19, NT20> Obj19;

           typedef bdlb::Variant<NT1,  NT2,  NT3,  NT4,
                                 NT5,  NT6,  NT7,  NT8,
                                 NT9,  NT10, NT11, NT12,
                                 NT13, NT14, NT15, NT16,
                                 NT17, NT18, NT19, BC20> Obj20;

           ASSERT(false == bslmf::IsBitwiseMoveable<Obj1>::value);
           ASSERT(false == bslmf::IsBitwiseMoveable<Obj2>::value);
           ASSERT(false == bslmf::IsBitwiseMoveable<Obj3>::value);
           ASSERT(false == bslmf::IsBitwiseMoveable<Obj18>::value);
           ASSERT(false == bslmf::IsBitwiseMoveable<Obj19>::value);
           ASSERT(false == bslmf::IsBitwiseMoveable<Obj20>::value);
       }
   }

   static void testCase18()
   {
       bslma::TestAllocator testAllocator(veryVeryVerbose);

       if (verbose) cout << endl
                         << "TESTING CLASSES 'bdlb::VariantN'."
                         << "=================================" << endl;

       typedef bslmf::TypeListNil  TestNil;  // for brevity

       typedef TestString   TestArg1;  // for concern 4
       typedef TestArg<2>   TestArg2;
       typedef TestArg<3>   TestArg3;
       typedef TestArg<4>   TestArg4;
       typedef TestArg<5>   TestArg5;
       typedef TestArg<6>   TestArg6;
       typedef TestArg<7>   TestArg7;
       typedef TestArg<8>   TestArg8;
       typedef TestArg<9>   TestArg9;
       typedef TestArg<10>  TestArg10;
       typedef TestArg<11>  TestArg11;
       typedef TestArg<12>  TestArg12;
       typedef TestArg<13>  TestArg13;
       typedef TestArg<14>  TestArg14;
       typedef TestArg<15>  TestArg15;
       typedef TestArg<16>  TestArg16;
       typedef TestArg<17>  TestArg17;
       typedef TestArg<18>  TestArg18;
       typedef TestArg<19>  TestArg19;
       typedef TestArg<20>  TestArg20;
       const TestArg1 V1("This is a string long enough to trigger allocation"
                         " even if Small-String-Optimization is used"
                         " in the 'bsl::string' implementation.");

       const TestArg2  V2 (2 );
       const TestArg3  V3 (3 );
       const TestArg4  V4 (4 );
       const TestArg5  V5 (5 );
       const TestArg6  V6 (6 );
       const TestArg7  V7 (7 );
       const TestArg8  V8 (8 );
       const TestArg9  V9 (9 );
       const TestArg10 V10(10);
       const TestArg11 V11(11);
       const TestArg12 V12(12);
       const TestArg13 V13(13);
       const TestArg14 V14(14);
       const TestArg15 V15(15);
       const TestArg16 V16(16);
       const TestArg17 V17(17);
       const TestArg18 V18(18);
       const TestArg19 V19(19);
       const TestArg20 V20(20);

       if (verbose) cout << "\nTesting 'bdlb::Variant2'." << endl;
       {
           typedef bdlb::Variant2<TestArg1, TestArg2> Obj;

           ASSERT(2 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator);         const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant3'." << endl;
       {
           typedef bdlb::Variant3<TestArg1, TestArg2, TestArg3> Obj;

           ASSERT(3 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant4'." << endl;
       {
           typedef bdlb::Variant4<TestArg1, TestArg2, TestArg3, TestArg4> Obj;

           ASSERT(4 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant5'." << endl;
       {
           typedef bdlb::Variant5<TestArg1, TestArg2, TestArg3, TestArg4,
                                  TestArg5> Obj;

           ASSERT(5 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant6'." << endl;
       {
           typedef bdlb::Variant6<TestArg1, TestArg2, TestArg3, TestArg4,
                                  TestArg5, TestArg6> Obj;

           ASSERT(6 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6, Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant7'." << endl;
       {
           typedef bdlb::Variant7<TestArg1, TestArg2, TestArg3, TestArg4,
                                  TestArg5, TestArg6, TestArg7> Obj;

           ASSERT(7 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6, Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7, Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant8'." << endl;
       {
           typedef bdlb::Variant8<TestArg1, TestArg2, TestArg3, TestArg4,
                                  TestArg5, TestArg6, TestArg7, TestArg8> Obj;

           ASSERT(8 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6, Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7, Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8, Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant9'." << endl;
       {
           typedef bdlb::Variant9<TestArg1, TestArg2, TestArg3, TestArg4,
                                  TestArg5, TestArg6, TestArg7, TestArg8,
                                  TestArg9> Obj;

           ASSERT(9 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6, Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7, Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8, Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9, Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,  Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant10'." << endl;
       {
           typedef bdlb::Variant10<TestArg1, TestArg2, TestArg3, TestArg4,
                                   TestArg5, TestArg6, TestArg7, TestArg8,
                                   TestArg9, TestArg10> Obj;

           ASSERT(10 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant11'." << endl;
       {
           typedef bdlb::Variant11<TestArg1, TestArg2, TestArg3, TestArg4,
                                   TestArg5, TestArg6, TestArg7, TestArg8,
                                   TestArg9, TestArg10, TestArg11> Obj;

           ASSERT(11 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant12'." << endl;
       {
           typedef bdlb::Variant12<TestArg1, TestArg2,  TestArg3, TestArg4,
                                   TestArg5, TestArg6,  TestArg7, TestArg8,
                                   TestArg9, TestArg10, TestArg11,
                                   TestArg12> Obj;

           ASSERT(12 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant13'." << endl;
       {
           typedef bdlb::Variant13<TestArg1, TestArg2,  TestArg3,  TestArg4,
                                   TestArg5, TestArg6,  TestArg7,  TestArg8,
                                   TestArg9, TestArg10, TestArg11, TestArg12,
                                   TestArg13> Obj;

           ASSERT(13 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant14'." << endl;
       {
           typedef bdlb::Variant14<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14> Obj;

           ASSERT(14 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant15'." << endl;
       {
           typedef bdlb::Variant15<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14, TestArg15> Obj;

           ASSERT(15 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant16'." << endl;
       {
           typedef bdlb::Variant16<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14, TestArg15,
                                   TestArg16> Obj;

           ASSERT(16 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestArg16, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX = V16;
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());

               Obj mX16(V16, &testAllocator);         const Obj& X16 = mX16;
               ASSERT(16 == X16.typeIndex());
               ASSERT(X16.is<TestArg16>());
               ASSERT(V16 == X16.the<TestArg16>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant17'." << endl;
       {
           typedef bdlb::Variant17<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14, TestArg15, TestArg16,
                                   TestArg17> Obj;

           ASSERT(17 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestArg16, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestArg17, Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;

               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX.assign<TestArg17>(V17);
               ASSERT(17 == X.typeIndex());
               ASSERT(X.is<TestArg17>());
               ASSERT(V17 == X.the<TestArg17>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX = V16;
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX = V17;
               ASSERT(17 == X.typeIndex());
               ASSERT(X.is<TestArg17>());
               ASSERT(V17 == X.the<TestArg17>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());

               Obj mX16(V16, &testAllocator);         const Obj& X16 = mX16;
               ASSERT(16 == X16.typeIndex());
               ASSERT(X16.is<TestArg16>());
               ASSERT(V16 == X16.the<TestArg16>());

               Obj mX17(V17, &testAllocator);         const Obj& X17 = mX17;
               ASSERT(17 == X17.typeIndex());
               ASSERT(X17.is<TestArg17>());
               ASSERT(V17 == X17.the<TestArg17>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant18'." << endl;
       {
           typedef bdlb::Variant18<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14, TestArg15, TestArg16,
                                   TestArg17, TestArg18> Obj;

           ASSERT(18 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestArg16, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestArg17, Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestArg18, Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX.assign<TestArg17>(V17);
               ASSERT(17 == X.typeIndex());
               ASSERT(X.is<TestArg17>());
               ASSERT(V17 == X.the<TestArg17>());

               mX.assign<TestArg18>(V18);
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX = V16;
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX = V18;
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());

               Obj mX16(V16, &testAllocator);         const Obj& X16 = mX16;
               ASSERT(16 == X16.typeIndex());
               ASSERT(X16.is<TestArg16>());
               ASSERT(V16 == X16.the<TestArg16>());

               Obj mX17(V17, &testAllocator);         const Obj& X17 = mX17;
               ASSERT(17 == X17.typeIndex());
               ASSERT(X17.is<TestArg17>());
               ASSERT(V17 == X17.the<TestArg17>());

               Obj mX18(V18, &testAllocator);         const Obj& X18 = mX18;
               ASSERT(18 == X18.typeIndex());
               ASSERT(X18.is<TestArg18>());
               ASSERT(V18 == X18.the<TestArg18>());
           }
       }

       if (verbose) cout << "\nTesting 'bdlb::Variant19'." << endl;
       {
           typedef bdlb::Variant19<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                   TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                   TestArg9,  TestArg10, TestArg11, TestArg12,
                                   TestArg13, TestArg14, TestArg15, TestArg16,
                                   TestArg17, TestArg18, TestArg19> Obj;

           ASSERT(19 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestArg16, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestArg17, Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestArg18, Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestArg19, Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil,   Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX.assign<TestArg17>(V17);
               ASSERT(17 == X.typeIndex());
               ASSERT(X.is<TestArg17>());
               ASSERT(V17 == X.the<TestArg17>());

               mX.assign<TestArg18>(V18);
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());

               mX.assign<TestArg19>(V19);
               ASSERT(19 == X.typeIndex());
               ASSERT(X.is<TestArg19>());
               ASSERT(V19 == X.the<TestArg19>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX = V16;
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX = V18;
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());

               mX = V19;
               ASSERT(19 == X.typeIndex());
               ASSERT(X.is<TestArg19>());
               ASSERT(V19 == X.the<TestArg19>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());

               Obj mX16(V16, &testAllocator);         const Obj& X16 = mX16;
               ASSERT(16 == X16.typeIndex());
               ASSERT(X16.is<TestArg16>());
               ASSERT(V16 == X16.the<TestArg16>());

               Obj mX17(V17, &testAllocator);         const Obj& X17 = mX17;
               ASSERT(17 == X17.typeIndex());
               ASSERT(X17.is<TestArg17>());
               ASSERT(V17 == X17.the<TestArg17>());

               Obj mX18(V18, &testAllocator);         const Obj& X18 = mX18;
               ASSERT(18 == X18.typeIndex());
               ASSERT(X18.is<TestArg18>());
               ASSERT(V18 == X18.the<TestArg18>());

               Obj mX19(V19, &testAllocator);         const Obj& X19 = mX19;
               ASSERT(19 == X19.typeIndex());
               ASSERT(X19.is<TestArg19>());
               ASSERT(V19 == X19.the<TestArg19>());
           }
       }

       if (verbose)
           cout << "\nTesting 'bdlb::Variant (with no types)'." << endl;
       {
           typedef bdlb::Variant<> Obj;

           ASSERT(0 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestNil, Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestNil, Obj::Type20>::VALUE));

           ASSERT(false == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true  == bdlb::HasPrintMethod<Obj>::value);

           Obj mX(&testAllocator); const Obj& X = mX;
           ASSERT(0 == X.typeIndex());
       }

       if (verbose)
           cout << "\nTesting 'bdlb::Variant (with 20 types)'." << endl;
       {
           typedef bdlb::Variant<TestArg1,  TestArg2,  TestArg3,  TestArg4,
                                 TestArg5,  TestArg6,  TestArg7,  TestArg8,
                                 TestArg9,  TestArg10, TestArg11, TestArg12,
                                 TestArg13, TestArg14, TestArg15, TestArg16,
                                 TestArg17, TestArg18, TestArg19,
                                 TestArg20> Obj;

           ASSERT(20 == Obj::TypeList::LENGTH);

           ASSERT((bsl::is_same<TestArg1,  Obj::Type1 >::VALUE));
           ASSERT((bsl::is_same<TestArg2,  Obj::Type2 >::VALUE));
           ASSERT((bsl::is_same<TestArg3,  Obj::Type3 >::VALUE));
           ASSERT((bsl::is_same<TestArg4,  Obj::Type4 >::VALUE));
           ASSERT((bsl::is_same<TestArg5,  Obj::Type5 >::VALUE));
           ASSERT((bsl::is_same<TestArg6,  Obj::Type6 >::VALUE));
           ASSERT((bsl::is_same<TestArg7,  Obj::Type7 >::VALUE));
           ASSERT((bsl::is_same<TestArg8,  Obj::Type8 >::VALUE));
           ASSERT((bsl::is_same<TestArg9,  Obj::Type9 >::VALUE));
           ASSERT((bsl::is_same<TestArg10, Obj::Type10>::VALUE));
           ASSERT((bsl::is_same<TestArg11, Obj::Type11>::VALUE));
           ASSERT((bsl::is_same<TestArg12, Obj::Type12>::VALUE));
           ASSERT((bsl::is_same<TestArg13, Obj::Type13>::VALUE));
           ASSERT((bsl::is_same<TestArg14, Obj::Type14>::VALUE));
           ASSERT((bsl::is_same<TestArg15, Obj::Type15>::VALUE));
           ASSERT((bsl::is_same<TestArg16, Obj::Type16>::VALUE));
           ASSERT((bsl::is_same<TestArg17, Obj::Type17>::VALUE));
           ASSERT((bsl::is_same<TestArg18, Obj::Type18>::VALUE));
           ASSERT((bsl::is_same<TestArg19, Obj::Type19>::VALUE));
           ASSERT((bsl::is_same<TestArg20, Obj::Type20>::VALUE));

           ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);
           ASSERT(true == bdlb::HasPrintMethod<Obj>::value);

           {
               if (verbose) cout << "\tTesting default constructor." << endl;
               Obj mX(&testAllocator); const Obj& X = mX;
               ASSERT(0 == X.typeIndex());

               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());
               ASSERT(0 == testAllocator.numBlocksInUse());

               if (verbose) cout << "\tTesting 'assign'." << endl;
               mX.assign<TestArg1>(V1);
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               mX.assign<TestArg2>(V2);
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX.assign<TestArg3>(V3);
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX.assign<TestArg4>(V4);
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX.assign<TestArg5>(V5);
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX.assign<TestArg6>(V6);
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX.assign<TestArg7>(V7);
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX.assign<TestArg8>(V8);
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX.assign<TestArg9>(V9);
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX.assign<TestArg10>(V10);
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX.assign<TestArg11>(V11);
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX.assign<TestArg12>(V12);
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX.assign<TestArg13>(V13);
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX.assign<TestArg14>(V14);
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX.assign<TestArg15>(V15);
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX.assign<TestArg16>(V16);
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX.assign<TestArg17>(V17);
               ASSERT(17 == X.typeIndex());
               ASSERT(X.is<TestArg17>());
               ASSERT(V17 == X.the<TestArg17>());

               mX.assign<TestArg18>(V18);
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());

               mX.assign<TestArg19>(V19);
               ASSERT(19 == X.typeIndex());
               ASSERT(X.is<TestArg19>());
               ASSERT(V19 == X.the<TestArg19>());

               mX.assign<TestArg20>(V20);
               ASSERT(20 == X.typeIndex());
               ASSERT(X.is<TestArg20>());
               ASSERT(V20 == X.the<TestArg20>());

               if (verbose) cout << "\tTesting 'operator='." << endl;
               mX = V1;
               ASSERT(1 == X.typeIndex());
               ASSERT(X.is<TestArg1>());
               ASSERT(V1 == X.the<TestArg1>());

               mX = V2;
               ASSERT(2 == X.typeIndex());
               ASSERT(X.is<TestArg2>());
               ASSERT(V2 == X.the<TestArg2>());

               mX = V3;
               ASSERT(3 == X.typeIndex());
               ASSERT(X.is<TestArg3>());
               ASSERT(V3 == X.the<TestArg3>());

               mX = V4;
               ASSERT(4 == X.typeIndex());
               ASSERT(X.is<TestArg4>());
               ASSERT(V4 == X.the<TestArg4>());

               mX = V5;
               ASSERT(5 == X.typeIndex());
               ASSERT(X.is<TestArg5>());
               ASSERT(V5 == X.the<TestArg5>());

               mX = V6;
               ASSERT(6 == X.typeIndex());
               ASSERT(X.is<TestArg6>());
               ASSERT(V6 == X.the<TestArg6>());

               mX = V7;
               ASSERT(7 == X.typeIndex());
               ASSERT(X.is<TestArg7>());
               ASSERT(V7 == X.the<TestArg7>());

               mX = V8;
               ASSERT(8 == X.typeIndex());
               ASSERT(X.is<TestArg8>());
               ASSERT(V8 == X.the<TestArg8>());

               mX = V9;
               ASSERT(9 == X.typeIndex());
               ASSERT(X.is<TestArg9>());
               ASSERT(V9 == X.the<TestArg9>());

               mX = V10;
               ASSERT(10 == X.typeIndex());
               ASSERT(X.is<TestArg10>());
               ASSERT(V10 == X.the<TestArg10>());

               mX = V11;
               ASSERT(11 == X.typeIndex());
               ASSERT(X.is<TestArg11>());
               ASSERT(V11 == X.the<TestArg11>());

               mX = V12;
               ASSERT(12 == X.typeIndex());
               ASSERT(X.is<TestArg12>());
               ASSERT(V12 == X.the<TestArg12>());

               mX = V13;
               ASSERT(13 == X.typeIndex());
               ASSERT(X.is<TestArg13>());
               ASSERT(V13 == X.the<TestArg13>());

               mX = V14;
               ASSERT(14 == X.typeIndex());
               ASSERT(X.is<TestArg14>());
               ASSERT(V14 == X.the<TestArg14>());

               mX = V15;
               ASSERT(15 == X.typeIndex());
               ASSERT(X.is<TestArg15>());
               ASSERT(V15 == X.the<TestArg15>());

               mX = V16;
               ASSERT(16 == X.typeIndex());
               ASSERT(X.is<TestArg16>());
               ASSERT(V16 == X.the<TestArg16>());

               mX = V18;
               ASSERT(18 == X.typeIndex());
               ASSERT(X.is<TestArg18>());
               ASSERT(V18 == X.the<TestArg18>());

               mX = V19;
               ASSERT(19 == X.typeIndex());
               ASSERT(X.is<TestArg19>());
               ASSERT(V19 == X.the<TestArg19>());

               mX = V20;
               ASSERT(20 == X.typeIndex());
               ASSERT(X.is<TestArg20>());
               ASSERT(V20 == X.the<TestArg20>());
           }

           if (verbose) cout << "\tTesting placement constructor." << endl;
           {
               const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

               Obj mX1(V1, &testAllocator);         const Obj& X1 = mX1;
               ASSERT(1 == X1.typeIndex());
               ASSERT(X1.is<TestArg1>());
               ASSERT(V1 == X1.the<TestArg1>());

               ASSERT(0        < testAllocator.numBlocksInUse());
               ASSERT(PREVIOUS < testAllocator.numBlocksTotal());

               Obj mX2(V2, &testAllocator);         const Obj& X2 = mX2;
               ASSERT(2 == X2.typeIndex());
               ASSERT(X2.is<TestArg2>());
               ASSERT(V2 == X2.the<TestArg2>());

               Obj mX3(V3, &testAllocator);         const Obj& X3 = mX3;
               ASSERT(3 == X3.typeIndex());
               ASSERT(X3.is<TestArg3>());
               ASSERT(V3 == X3.the<TestArg3>());

               Obj mX4(V4, &testAllocator);         const Obj& X4 = mX4;
               ASSERT(4 == X4.typeIndex());
               ASSERT(X4.is<TestArg4>());
               ASSERT(V4 == X4.the<TestArg4>());

               Obj mX5(V5, &testAllocator);         const Obj& X5 = mX5;
               ASSERT(5 == X5.typeIndex());
               ASSERT(X5.is<TestArg5>());
               ASSERT(V5 == X5.the<TestArg5>());

               Obj mX6(V6, &testAllocator);         const Obj& X6 = mX6;
               ASSERT(6 == X6.typeIndex());
               ASSERT(X6.is<TestArg6>());
               ASSERT(V6 == X6.the<TestArg6>());

               Obj mX7(V7, &testAllocator);         const Obj& X7 = mX7;
               ASSERT(7 == X7.typeIndex());
               ASSERT(X7.is<TestArg7>());
               ASSERT(V7 == X7.the<TestArg7>());

               Obj mX8(V8, &testAllocator);         const Obj& X8 = mX8;
               ASSERT(8 == X8.typeIndex());
               ASSERT(X8.is<TestArg8>());
               ASSERT(V8 == X8.the<TestArg8>());

               Obj mX9(V9, &testAllocator);         const Obj& X9 = mX9;
               ASSERT(9 == X9.typeIndex());
               ASSERT(X9.is<TestArg9>());
               ASSERT(V9 == X9.the<TestArg9>());

               Obj mX10(V10, &testAllocator);         const Obj& X10 = mX10;
               ASSERT(10 == X10.typeIndex());
               ASSERT(X10.is<TestArg10>());
               ASSERT(V10 == X10.the<TestArg10>());

               Obj mX11(V11, &testAllocator);         const Obj& X11 = mX11;
               ASSERT(11 == X11.typeIndex());
               ASSERT(X11.is<TestArg11>());
               ASSERT(V11 == X11.the<TestArg11>());

               Obj mX12(V12, &testAllocator);         const Obj& X12 = mX12;
               ASSERT(12 == X12.typeIndex());
               ASSERT(X12.is<TestArg12>());
               ASSERT(V12 == X12.the<TestArg12>());

               Obj mX13(V13, &testAllocator);         const Obj& X13 = mX13;
               ASSERT(13 == X13.typeIndex());
               ASSERT(X13.is<TestArg13>());
               ASSERT(V13 == X13.the<TestArg13>());

               Obj mX14(V14, &testAllocator);         const Obj& X14 = mX14;
               ASSERT(14 == X14.typeIndex());
               ASSERT(X14.is<TestArg14>());
               ASSERT(V14 == X14.the<TestArg14>());

               Obj mX15(V15, &testAllocator);         const Obj& X15 = mX15;
               ASSERT(15 == X15.typeIndex());
               ASSERT(X15.is<TestArg15>());
               ASSERT(V15 == X15.the<TestArg15>());

               Obj mX16(V16, &testAllocator);         const Obj& X16 = mX16;
               ASSERT(16 == X16.typeIndex());
               ASSERT(X16.is<TestArg16>());
               ASSERT(V16 == X16.the<TestArg16>());

               Obj mX17(V17, &testAllocator);         const Obj& X17 = mX17;
               ASSERT(17 == X17.typeIndex());
               ASSERT(X17.is<TestArg17>());
               ASSERT(V17 == X17.the<TestArg17>());

               Obj mX18(V18, &testAllocator);         const Obj& X18 = mX18;
               ASSERT(18 == X18.typeIndex());
               ASSERT(X18.is<TestArg18>());
               ASSERT(V18 == X18.the<TestArg18>());

               Obj mX19(V19, &testAllocator);         const Obj& X19 = mX19;
               ASSERT(19 == X19.typeIndex());
               ASSERT(X19.is<TestArg19>());
               ASSERT(V19 == X19.the<TestArg19>());

               Obj mX20(V20, &testAllocator);         const Obj& X20 = mX20;
               ASSERT(20 == X20.typeIndex());
               ASSERT(X20.is<TestArg20>());
               ASSERT(V20 == X20.the<TestArg20>());
           }
       }
   }

   static void testCase17()
   {
       bslma::TestAllocator testAllocator(veryVeryVerbose);
       if (verbose) cout << endl
                         << "TESTING 'isUnset'" << endl
                         << "=================" << endl;

       if (verbose) cout << "\nTesting 'isUnset' with 'reset'." << endl;
       {
           typedef bdlb::VariantImp<bslmf::TypeList<int, char> > Obj;

           Obj variant1, variant2;

           ASSERT(true == variant1.isUnset());
           ASSERT(true == variant2.isUnset());

           variant1 = 1;
           variant2 = 'a';

           ASSERT(false == variant1.isUnset());
           ASSERT(false == variant2.isUnset());

           variant1.reset();

           ASSERT(true  == variant1.isUnset());
           ASSERT(false == variant2.isUnset());
       }

       if (verbose) cout << "\nTesting 'isUnset' with 'bslmf::Nil'." << endl;
       {
           typedef bdlb::VariantImp<bslmf::TypeList<bslmf::Nil, int> > Obj;

           Obj variant = Obj(bslmf::Nil());

           ASSERT(false == variant.isUnset());
       }

   } 

   static void testCase16() 
   {
       if (verbose) cout << endl
                         << "TESTING VISITORS (unset variants)" << endl
                         << "=================================" << endl;

       typedef bdlb::VariantImp<
       bslmf::TypeList<int, TestInt, bsl::string, TestString> > Obj;

       if (verbose) cout << "\nTesting 'bslmf::Nil'." << endl;
       {
           my_UnsetVariantVisitor visitor;
           LOOP_ASSERT(visitor.d_lastType,
                      my_UnsetVariantVisitor::GENERIC == visitor.d_lastType);

           Obj mX;
           mX.apply(visitor);

           LOOP_ASSERT(visitor.d_lastType,
                      my_UnsetVariantVisitor::BSLMF_NIL == visitor.d_lastType);

           visitor.d_lastType = my_UnsetVariantVisitor::GENERIC;
           LOOP_ASSERT(visitor.d_lastType,
                      my_UnsetVariantVisitor::GENERIC == visitor.d_lastType);

           const Obj& X = mX;
           X.apply(visitor);

           LOOP_ASSERT(visitor.d_lastType,
                      my_UnsetVariantVisitor::BSLMF_NIL == visitor.d_lastType);
       }

       if (verbose) cout << "\nTesting user specified 'defaultValue'."<< endl;
       {
           my_UnsetVariantVisitor visitor;
           LOOP_ASSERT(visitor.d_lastType,
                       my_UnsetVariantVisitor::GENERIC == visitor.d_lastType);

           Obj mX;
           mX.apply(visitor, TestArg<1>());

           LOOP_ASSERT(visitor.d_lastType,
                       my_UnsetVariantVisitor::TEST_ARG == visitor.d_lastType);

           visitor.d_lastType = my_UnsetVariantVisitor::GENERIC;
           LOOP_ASSERT(visitor.d_lastType,
                       my_UnsetVariantVisitor::GENERIC == visitor.d_lastType);

           const Obj& X = mX;
           X.apply(visitor, TestArg<1>());

           LOOP_ASSERT(visitor.d_lastType,
                       my_UnsetVariantVisitor::TEST_ARG == visitor.d_lastType);
       }

       if (verbose) cout << "\nTesting a subtle warning case." << endl;
       {
           bdlb::Variant<int> v(1);
           dummyConvert(0, v);
       }

       if (verbose) cout << "\nNegative Testing 'applyRaw'." << endl;
       {
           bsls::AssertFailureHandlerGuard hG(
               bsls::AssertTest::failTestDriver);

           Obj                    mX;
           my_UnsetVariantVisitor visitor;

           mX.assign<int>(77);  ASSERT_SAFE_PASS(mX.applyRaw(visitor));
           mX.reset();          ASSERT_SAFE_FAIL(mX.applyRaw(visitor));
       }

   }

   static void testCase15() 
   {
       if (verbose) cout << endl
                         << "TESTING VISITORS (return values)" << endl
                         << "================================" << endl;

       bslma::TestAllocator testAllocator(veryVeryVerbose);
       
       typedef bdlb::VariantImp<
                  bslmf::TypeList<int, TestInt, bsl::string, TestString> > Obj;

       if (verbose) cout << "\nTesting visitor that modifies values" << endl;
       {
           enum { LENGTH = Obj::TypeList::LENGTH };
           ASSERT(4 == LENGTH);

           Obj mXs[LENGTH]; Obj mYs[LENGTH];
           mXs[0].createInPlace<int>(INT_DATA[0]);
           mXs[1].createInPlace<TestInt>(TEST_INT_DATA[0]);
           mXs[2].createInPlace<bsl::string>(STRING_DATA[0]);
           mXs[3].createInPlace<TestString>(TEST_STRING_DATA[0]);
           mYs[0].createInPlace<int>(INT_DATA[0]);
           mYs[1].createInPlace<TestInt>(TEST_INT_DATA[0]);
           mYs[2].createInPlace<bsl::string>(STRING_DATA[0]);
           mYs[3].createInPlace<TestString>(TEST_STRING_DATA[0]);

           for (int i = 0;
                i < static_cast<int>(sizeof STRING_DATA / sizeof *STRING_DATA);
                ++i) {

               const int         INTVAL        = mXs[0].the<int>();
               const TestInt     TESTINTVAL    = mXs[1].the<TestInt>();
               const bsl::string STRINGVAL     = mXs[2].the<bsl::string>();
               const TestString  TESTSTRINGVAL = mXs[3].the<TestString>();

               my_ModifyingVisitor visitor(i);

               LOOP_ASSERT(i, INTVAL        == mXs[0].the<int>());
               LOOP_ASSERT(i, TESTINTVAL    == mXs[1].the<TestInt>());
               LOOP_ASSERT(i, STRINGVAL     == mXs[2].the<bsl::string>());
               LOOP_ASSERT(i, TESTSTRINGVAL == mXs[3].the<TestString>());
               LOOP_ASSERT(i, INTVAL        == mYs[0].the<int>());
               LOOP_ASSERT(i, TESTINTVAL    == mYs[1].the<TestInt>());
               LOOP_ASSERT(i, STRINGVAL     == mYs[2].the<bsl::string>());
               LOOP_ASSERT(i, TESTSTRINGVAL == mYs[3].the<TestString>());

               // Visit the values.

               for (int j = 0; j < LENGTH; ++j) {
                   mXs[j].apply(visitor);
                   mYs[j].applyRaw(visitor);
               }

               if (veryVerbose) {
                   const int          theInt       =mXs[0].the<int>();
                   const TestInt&     theTestInt   =mXs[1].the<TestInt>();
                   const bsl::string& theString    =mXs[2].the<bsl::string>();
                   const TestString&  theTestString=mXs[3].the<TestString>();

                   T_ P_(i) P_(INT_DATA[i])    P_(TEST_INT_DATA[i])
                       P_(STRING_DATA[i]) P(TEST_STRING_DATA[i])
                       P_(theInt) P_(theTestInt) P_(theString) P(theTestString)
                       }

               LOOP_ASSERT(i, INT_DATA[i]        ==mXs[0].the<int>());
               LOOP_ASSERT(i, TEST_INT_DATA[i]   ==mXs[1].the<TestInt>());
               LOOP_ASSERT(i, STRING_DATA[i]     ==mXs[2].the<bsl::string>());
               LOOP_ASSERT(i, TEST_STRING_DATA[i]==mXs[3].the<TestString>());
               LOOP_ASSERT(i, INT_DATA[i]        ==mYs[0].the<int>());
               LOOP_ASSERT(i, TEST_INT_DATA[i]   ==mYs[1].the<TestInt>());
               LOOP_ASSERT(i, STRING_DATA[i]     ==mYs[2].the<bsl::string>());
               LOOP_ASSERT(i, TEST_STRING_DATA[i]==mYs[3].the<TestString>());
           }
       }

        if (verbose) cout << "\nTesting return values from visitor." << endl;

        typedef my_VariantWrapper<Obj> VWrap;

        if (verbose) cout << "\t1. No specified return value." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            my_DefaultNoReturnVisitor visitor(1);

            wrappedVariant.apply(visitor);
            wrappedVariant2.applyRaw(visitor);
            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::VOID_VISIT == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::VOID_VISIT == wrappedVariant2.lastVisited());
        }

        if (verbose) cout << "\t2. Using 'typedef' 'ResultType'." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            my_ReturningVisitor visitor(1);

            const int RET  = wrappedVariant.apply(visitor);
            const int RET2 = wrappedVariant2.applyRaw(visitor);
            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::RESULT_TYPE_VISIT
                        == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::RESULT_TYPE_VISIT
                        == wrappedVariant.lastVisited());

            ASSERT(RET  == INT_TYPE);
            ASSERT(RET2 == INT_TYPE);
        }

        if (verbose) cout << "\t3. Explicitly specifying 'RET_TYPE'." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            my_ReturningVisitor visitor(1);

            ASSERT(999 <= wrappedVariant.apply<Convertible>(
                       visitor).memberFunc());
            ASSERT(999 <= wrappedVariant2.applyRaw<Convertible>(
                       visitor).memberFunc());

            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::RET_TYPE_VISIT == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::RET_TYPE_VISIT ==
                        wrappedVariant2.lastVisited());
        }

        if (verbose) cout << "\nTesting 'const' 'apply' methods" << endl;

        if (verbose) cout << "\t1. No specified return value." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            const VWrap& WV  = wrappedVariant;
            const VWrap& WV2 = wrappedVariant2;

            wrappedVariant.variant().assign<int>(INT_DATA[1]);
            wrappedVariant2.variant().assign<int>(INT_DATA[1]);

            my_ConstVisitor visitor(1);
            WV.apply(visitor);
            WV2.applyRaw(visitor);

            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::VOID_VISIT_CONST
                        == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::VOID_VISIT_CONST
                        == wrappedVariant2.lastVisited());
        }

        if (verbose) cout << "\t2. Using 'typedef' 'ResultType'." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            const VWrap& WV  = wrappedVariant;
            const VWrap& WV2 = wrappedVariant2;

            wrappedVariant.variant().assign<int>(INT_DATA[1]);
            wrappedVariant2.variant().assign<int>(INT_DATA[1]);

            my_ConstReturningVisitor visitor(1);
            const int RET  = WV.apply(visitor);
            const int RET2 = WV2.applyRaw(visitor);

            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::RESULT_TYPE_VISIT_CONST
                        == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::RESULT_TYPE_VISIT_CONST
                        == wrappedVariant2.lastVisited());

            ASSERT(RET  == INT_TYPE);
            ASSERT(RET2 == INT_TYPE);
        }

        if (verbose) cout << "\t3. Explicitly specifying 'RET_TYPE'." << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            const VWrap& WV  = wrappedVariant;
            const VWrap& WV2 = wrappedVariant2;

            wrappedVariant.variant().assign<int>(INT_DATA[1]);
            wrappedVariant2.variant().assign<int>(INT_DATA[1]);

            my_ConstReturningVisitor visitor(1);
            ASSERT(999 <= WV.apply<Convertible>(visitor).memberFunc());
            ASSERT(999 <= WV2.applyRaw<Convertible>(visitor).memberFunc());

            LOOP_ASSERT(wrappedVariant.lastVisited(),
                        VWrap::RET_TYPE_VISIT_CONST
                        == wrappedVariant.lastVisited());
            LOOP_ASSERT(wrappedVariant2.lastVisited(),
                        VWrap::RET_TYPE_VISIT_CONST
                        == wrappedVariant2.lastVisited());
        }

        if (verbose) cout << "\nTesting that visitors do not modify the value."
                          << endl;
        {
            Obj tmp(1);  // dummy used to initialize the variant
            VWrap wrappedVariant(tmp, &testAllocator);
            VWrap wrappedVariant2(tmp, &testAllocator);
            wrappedVariant.variant().createInPlace<bsl::string>(
                STRING_DATA[1]);
            wrappedVariant2.variant().createInPlace<bsl::string>(
                STRING_DATA[1]);

            my_ConstVisitor visitor(1);
            wrappedVariant.apply(visitor);
            wrappedVariant2.applyRaw(visitor);

            ASSERT(STRING_DATA[1]
                   == wrappedVariant.variant().the<bsl::string>());
            ASSERT(STRING_DATA[1]
                   == wrappedVariant2.variant().the<bsl::string>());
        }
    }

    static void testCase14()
    {
        if (verbose) cout << endl
                          << "TESTING 'createInPlace'" << endl
                          << "=======================" << endl;

        typedef bdlb::VariantImp<bslmf::TypeList<Copyable> > Obj;

        if (verbose) cout << "\nTesting 'createInPlace' with no arg." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>();

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 0);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 1 arg." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 1);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 2 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 2);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 3 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 3);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 4 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 4);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 5 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 5);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 6 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 6);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 7 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 7);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 8 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 8);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 9 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 9);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 10 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 10);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 11 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 11);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 12 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true, true, true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 12);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 13 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true, true, true, true,
                                       true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 13);
        }

        if (verbose) cout << "\nTesting 'createInPlace' with 14 args." << endl;
        {
            Obj mX;     const Obj& X = mX;
            mX.createInPlace<Copyable>(true, true, true, true, true, true,
                                       true, true, true, true, true, true,
                                       true, true);

            ASSERT(false == Copyable::s_copyConstructorCalled);
            checkCopyableParameters(X.the<Copyable>(), 14);
        }

    }

    static void testCase13()
    {

        if (verbose) cout << endl
                          << "TESTING TYPE ASSIGNMENT" << endl
                          << "=======================" << endl;

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << "\nTesting assignment from different values."
                          << endl;

        static struct {
            int         d_lineNum;
            const char *d_input;        // input specifications
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE  INPUT           TYPE_IDX  VALUE_IDX
          // ----  -----           --------  ---------
            { L_,  "A",            INT_TYPE,         0 }
          , { L_,  "B",            INT_TYPE,         1 }
          , { L_,  "C",            INT_TYPE,         2 }
          , { L_,  "D",            INT_TYPE,         3 }
          , { L_,  "E",            INT_TYPE,         4 }
          , { L_,  "F",       TEST_INT_TYPE,         0 }
          , { L_,  "G",       TEST_INT_TYPE,         1 }
          , { L_,  "H",       TEST_INT_TYPE,         2 }
          , { L_,  "I",       TEST_INT_TYPE,         3 }
          , { L_,  "J",       TEST_INT_TYPE,         4 }
          , { L_,  "S",         STRING_TYPE,         0 }
          , { L_,  "T",         STRING_TYPE,         1 }
          , { L_,  "U",         STRING_TYPE,         2 }
          , { L_,  "V",         STRING_TYPE,         3 }
          , { L_,  "W",         STRING_TYPE,         4 }
          , { L_,  "K",    TEST_STRING_TYPE,         0 }
          , { L_,  "L",    TEST_STRING_TYPE,         1 }
          , { L_,  "M",    TEST_STRING_TYPE,         2 }
          , { L_,  "N",    TEST_STRING_TYPE,         3 }
          , { L_,  "O",    TEST_STRING_TYPE,         4 }
          , { L_,  "Z",      TEST_VOID_TYPE,         0 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1      = DATA[ti].d_lineNum;
            const char *INPUT1     = DATA[ti].d_input;
            const int   TYPE_IDX1  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX1 = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE1) P_(INPUT1) P_(TYPE_IDX1) P(VALUE_IDX1)
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2      = DATA[tj].d_lineNum;
                const int TYPE_IDX2  = DATA[tj].d_expTypeIdx;
                const int VALUE_IDX2 = DATA[tj].d_expValueIdx;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(TYPE_IDX2) P(VALUE_IDX2)
                }

                {
                    Obj mX(&testAllocator);     const Obj& X = mX;
                    gg(&mX, INPUT1);

                    switch (TYPE_IDX2) {
                      case INT_TYPE: {

                        int Y = INT_DATA[VALUE_IDX2];

                        // Assign to 'X' the value 'Y' and check the returned
                        // reference.

                        LOOP2_ASSERT(LINE1, LINE2, INT_DATA[VALUE_IDX2] ==
                                                         (mX = Y).the<int>());

                        // Check that 'Y' is not modified.

                        LOOP2_ASSERT(LINE1, LINE2, INT_DATA[VALUE_IDX2] == Y);

                        // Check that 'X' is modified.

                        LOOP2_ASSERT(LINE1, LINE2, Y == X.the<int>());

                      } break;
                      case TEST_INT_TYPE: {

                        TestInt Y = TEST_INT_DATA[VALUE_IDX2];

                        LOOP2_ASSERT(LINE1, LINE2, TEST_INT_DATA[VALUE_IDX2]
                                                  == (mX = Y).the<TestInt>());
                        LOOP2_ASSERT(LINE1, LINE2, TEST_INT_DATA[VALUE_IDX2]
                                                                         == Y);
                        LOOP2_ASSERT(LINE1, LINE2, Y == X.the<TestInt>());

                      } break;
                      case STRING_TYPE : {

                        bsl::string Y = STRING_DATA[VALUE_IDX2];

                        LOOP2_ASSERT(LINE1, LINE2, STRING_DATA[VALUE_IDX2]
                                               == (mX = Y).the<bsl::string>());
                        LOOP2_ASSERT(LINE1, LINE2, STRING_DATA[VALUE_IDX2]
                                                                         == Y);
                        LOOP2_ASSERT(LINE1, LINE2, Y == X.the<bsl::string>());

                      } break;
                      case TEST_STRING_TYPE: {

                        TestString Y = TEST_STRING_DATA[VALUE_IDX2];

                        LOOP2_ASSERT(LINE1, LINE2, TEST_STRING_DATA[VALUE_IDX2]
                                                == (mX = Y).the<TestString>());
                        LOOP2_ASSERT(LINE1, LINE2, TEST_STRING_DATA[VALUE_IDX2]
                                                                         == Y);
                        LOOP2_ASSERT(LINE1, LINE2, Y == X.the<TestString>());

                      } break;
                      case TEST_VOID_TYPE: {

                        TestVoid Y;

                        LOOP2_ASSERT(LINE1, LINE2, TestVoid()
                                                  == (mX = Y).the<TestVoid>());
                        LOOP2_ASSERT(LINE1, LINE2, TestVoid() == Y);
                        LOOP2_ASSERT(LINE1, LINE2, Y == X.the<TestVoid>());

                      } break;
                      default : ASSERT(!"Unreachable by design");
                    }

                    // Verify value after 'Y' goes out of scope.

                    switch (TYPE_IDX2) {
                      case INT_TYPE: {
                        LOOP2_ASSERT(LINE1, LINE2,
                                     INT_DATA[VALUE_IDX2] == X.the<int>());
                      } break;
                      case TEST_INT_TYPE: {
                        LOOP2_ASSERT(LINE1, LINE2,
                                     TEST_INT_DATA[VALUE_IDX2]
                                                          == X.the<TestInt>());
                      } break;
                      case STRING_TYPE: {
                        LOOP2_ASSERT(LINE1, LINE2,
                                     STRING_DATA[VALUE_IDX2]
                                                      == X.the<bsl::string>());
                      } break;
                      case TEST_STRING_TYPE: {
                        LOOP2_ASSERT(LINE1, LINE2,
                                     TEST_STRING_DATA[VALUE_IDX2]
                                                       == X.the<TestString>());
                      } break;
                      case TEST_VOID_TYPE: {
                        LOOP2_ASSERT(LINE1, LINE2,
                                     TestVoid() == X.the<TestVoid>());
                      } break;
                      default: ASSERT(!"Unreachable by design");
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting allocator with assignment." << endl;

        {
            bslma::TestAllocator ta, da;
            bslma::DefaultAllocatorGuard guard(&da);

            ASSERT(0 == ta.numBlocksTotal());
            ASSERT(0 == da.numBlocksTotal());

            TestAllocObj value(&ta);

            const int TATOTAL = static_cast<int>(ta.numBlocksTotal());
            ASSERT(TATOTAL == ta.numBlocksTotal());
            ASSERT(0       == da.numBlocksTotal());

            bdlb::Variant<bsl::string, TestAllocObj> mX(value);

            ASSERT(TATOTAL == ta.numBlocksTotal());
            ASSERT(TATOTAL == da.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting exception safety with assignment."
                          << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1      = DATA[ti].d_lineNum;
            const char *INPUT1     = DATA[ti].d_input;
            const int   TYPE_IDX1  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX1 = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE1) P_(INPUT1) P_(TYPE_IDX1) P(VALUE_IDX1)
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2      = DATA[tj].d_lineNum;
                const int TYPE_IDX2  = DATA[tj].d_expTypeIdx;
                const int VALUE_IDX2 = DATA[tj].d_expValueIdx;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(TYPE_IDX2) P(VALUE_IDX2)
                }

                Obj mX(&testAllocator);  const Obj& X = mX;
                gg(&mX, INPUT1);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  switch (TYPE_IDX2) {
                    case INT_TYPE: {
                      mX = INT_DATA[VALUE_IDX2];
                      LOOP2_ASSERT(LINE1, LINE2,
                                   INT_DATA[VALUE_IDX2] == X.the<int>());
                    } break;
                    case TEST_INT_TYPE: {
                      mX = TEST_INT_DATA[VALUE_IDX2];
                      LOOP2_ASSERT(LINE1, LINE2,
                                   TEST_INT_DATA[VALUE_IDX2]
                                                          == X.the<TestInt>());
                    } break;
                    case STRING_TYPE: {
                      mX = STRING_DATA[VALUE_IDX2];
                      LOOP2_ASSERT(LINE1, LINE2,
                                   STRING_DATA[VALUE_IDX2]
                                                      == X.the<bsl::string>());
                    } break;
                    case TEST_STRING_TYPE: {
                      mX = TEST_STRING_DATA[VALUE_IDX2];
                      LOOP2_ASSERT(LINE1, LINE2,
                                   TEST_STRING_DATA[VALUE_IDX2]
                                                       == X.the<TestString>());
                    } break;
                    case TEST_VOID_TYPE: {
                      mX = TestVoid();
                      LOOP2_ASSERT(LINE1, LINE2,
                                   TestVoid() == X.the<TestVoid>());
                    } break;
                    default: ASSERT(!"Unreachable by design");
                  }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
     
    static void testCase12() 
    {

        if (verbose) cout << endl
                          << "TESTING CREATORS" << endl
                          << "================" << endl;

        if (verbose) cout << "\nTesting one argument constructor with "
                             "allocator." << endl;

        {
            // Added 'TestAllocObj' to confirm allocation.
            typedef bdlb::Variant<TestAllocObj, int, bsl::string,
                                  TestInt, TestString, TestVoid> Obj;

            // 'da'  - default allocator
            // 'ta'  - test allocator passed to the variant
            // 'tmp' - allocator for temporaries

            bslma::TestAllocator da, ta, tmp;
            bslma::DefaultAllocatorGuard guard(&da);

            ASSERT(0 == da.numBlocksTotal());
            ASSERT(0 == ta.numBlocksTotal());
            ASSERT(0 == tmp.numBlocksTotal());

            {
                Obj mX(&ta);   const Obj& X = mX;
                ASSERT(0 == X.typeIndex());

                if (veryVerbose) { T_ P(X) }

                ASSERT(0 == X.is<TestAllocObj>());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == ta.numBlocksTotal());
                ASSERT(0 == tmp.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(ta.numBlocksTotal());

                bsl::string testString("Hello, this is a string long "
                                       " enough to force memory allocation",
                                       &tmp);

                ASSERT(0            == da.numBlocksTotal());
                ASSERT(CURRENTTOTAL == ta.numBlocksTotal());
                ASSERT(0            <  tmp.numBlocksTotal());

                mX.assign(testString);

                ASSERT(0            == da.numBlocksTotal());
                ASSERT(CURRENTTOTAL <  ta.numBlocksTotal());
                ASSERT(0            <  tmp.numBlocksTotal());
            }
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(0 == tmp.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting one argument constructor with "
                             "'TYPE' and two-argument constructor" << endl;

        static struct {
            int         d_lineNum;
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE          TYPE_IDX  VALUE_IDX
          // ----          --------  ---------
            { L_,          INT_TYPE,         0 }
          , { L_,          INT_TYPE,         1 }
          , { L_,          INT_TYPE,         2 }
          , { L_,          INT_TYPE,         3 }
          , { L_,          INT_TYPE,         4 }
          , { L_,     TEST_INT_TYPE,         0 }
          , { L_,     TEST_INT_TYPE,         1 }
          , { L_,     TEST_INT_TYPE,         2 }
          , { L_,     TEST_INT_TYPE,         3 }
          , { L_,     TEST_INT_TYPE,         4 }
          , { L_,       STRING_TYPE,         0 }
          , { L_,       STRING_TYPE,         1 }
          , { L_,       STRING_TYPE,         2 }
          , { L_,       STRING_TYPE,         3 }
          , { L_,       STRING_TYPE,         4 }
          , { L_,  TEST_STRING_TYPE,         0 }
          , { L_,  TEST_STRING_TYPE,         1 }
          , { L_,  TEST_STRING_TYPE,         2 }
          , { L_,  TEST_STRING_TYPE,         3 }
          , { L_,  TEST_STRING_TYPE,         4 }
          , { L_,    TEST_VOID_TYPE,         0 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE      = DATA[ti].d_lineNum;
            const int TYPE_IDX  = DATA[ti].d_expTypeIdx;
            const int VALUE_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE) P_(TYPE_IDX) P(VALUE_IDX)
            }

            bslma::TestAllocator da, ta;
            bslma::DefaultAllocatorGuard guard(&da);

            switch (TYPE_IDX) {
              case INT_TYPE: {

                ASSERT(0 == da.numBlocksTotal());

                Obj mX(INT_DATA[VALUE_IDX]);                 const Obj& X = mX;

                ASSERT(INT_TYPE            == X.typeIndex());
                ASSERT(1                   == X.is<int>());
                ASSERT(INT_DATA[VALUE_IDX] == X.the<int>());
                ASSERT(0                   == da.numBlocksTotal());
                ASSERT(0                   == ta.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(da.numBlocksTotal());

                Obj mY(INT_DATA[VALUE_IDX], &ta);            const Obj& Y = mY;

                ASSERT(INT_TYPE            == Y.typeIndex());
                ASSERT(1                   == Y.is<int>());
                ASSERT(INT_DATA[VALUE_IDX] == Y.the<int>());
                ASSERT(CURRENTTOTAL        == da.numBlocksTotal());
                ASSERT(0                   == ta.numBlocksTotal());

                Obj mZ;                                      const Obj& Z = mZ;
                mZ.assign(INT_DATA[VALUE_IDX]);

                ASSERT(Z.typeIndex() == X.typeIndex());
                ASSERT(Z.is<int>()   == X.is<int>());
                ASSERT(Z.the<int>()  == X.the<int>());
                ASSERT(Z.typeIndex() == Y.typeIndex());
                ASSERT(Z.is<int>()   == Y.is<int>());
                ASSERT(Z.the<int>()  == Y.the<int>());

              } break;
              case TEST_INT_TYPE : {

                ASSERT(0 == da.numBlocksTotal());

                Obj mX(TEST_INT_DATA[VALUE_IDX]);            const Obj& X = mX;

                ASSERT(TEST_INT_TYPE            == X.typeIndex());
                ASSERT(1                        == X.is<TestInt>());
                ASSERT(TEST_INT_DATA[VALUE_IDX] == X.the<TestInt>());
                ASSERT(0                        == da.numBlocksTotal());
                ASSERT(0                        == ta.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(da.numBlocksTotal());

                Obj mY(TEST_INT_DATA[VALUE_IDX], &ta);       const Obj& Y = mY;

                ASSERT(TEST_INT_TYPE            == Y.typeIndex());
                ASSERT(1                        == Y.is<TestInt>());
                ASSERT(TEST_INT_DATA[VALUE_IDX] == Y.the<TestInt>());
                ASSERT(CURRENTTOTAL             == da.numBlocksTotal());
                ASSERT(0                        == ta.numBlocksTotal());

                Obj mZ;                                      const Obj& Z = mZ;
                mZ.assign(TEST_INT_DATA[VALUE_IDX]);

                ASSERT(Z.typeIndex()    == X.typeIndex());
                ASSERT(Z.is<TestInt>()  == X.is<TestInt>());
                ASSERT(Z.the<TestInt>() == X.the<TestInt>());
                ASSERT(Z.typeIndex()    == Y.typeIndex());
                ASSERT(Z.is<TestInt>()  == Y.is<TestInt>());
                ASSERT(Z.the<TestInt>() == Y.the<TestInt>());

              } break;
              case STRING_TYPE : {

                ASSERT(0 == da.numBlocksTotal());

                Obj mX(STRING_DATA[VALUE_IDX]);              const Obj& X = mX;

                ASSERT(STRING_TYPE            == X.typeIndex());
                ASSERT(1                      == X.is<bsl::string>());
                ASSERT(STRING_DATA[VALUE_IDX] == X.the<bsl::string>());
                ASSERT(0                      == ta.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(da.numBlocksTotal());

                Obj mY(STRING_DATA[VALUE_IDX], &ta);         const Obj& Y = mY;

                ASSERT(STRING_TYPE            == Y.typeIndex());
                ASSERT(1                      == Y.is<bsl::string>());
                ASSERT(STRING_DATA[VALUE_IDX] == Y.the<bsl::string>());
                ASSERT(CURRENTTOTAL           == da.numBlocksTotal());

                Obj mZ;                                      const Obj& Z = mZ;
                mZ.assign(STRING_DATA[VALUE_IDX]);

                ASSERT(Z.typeIndex()        == X.typeIndex());
                ASSERT(Z.is<bsl::string>()  == X.is<bsl::string>());
                ASSERT(Z.the<bsl::string>() == X.the<bsl::string>());
                ASSERT(Z.typeIndex()        == Y.typeIndex());
                ASSERT(Z.is<bsl::string>()  == Y.is<bsl::string>());
                ASSERT(Z.the<bsl::string>() == Y.the<bsl::string>());

              } break;
              case TEST_STRING_TYPE : {

                ASSERT(0 == da.numBlocksTotal());

                Obj mX(TEST_STRING_DATA[VALUE_IDX]);         const Obj& X = mX;

                ASSERT(TEST_STRING_TYPE            == X.typeIndex());
                ASSERT(1                           == X.is<TestString>());
                ASSERT(TEST_STRING_DATA[VALUE_IDX] == X.the<TestString>());
                ASSERT(0                           == ta.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(da.numBlocksTotal());

                Obj mY(TEST_STRING_DATA[VALUE_IDX], &ta);    const Obj& Y = mY;

                ASSERT(TEST_STRING_TYPE            == Y.typeIndex());
                ASSERT(1                           == Y.is<TestString>());
                ASSERT(TEST_STRING_DATA[VALUE_IDX] == Y.the<TestString>());
                ASSERT(CURRENTTOTAL                == da.numBlocksTotal());

                Obj mZ;                                      const Obj& Z = mZ;
                mZ.assign(TEST_STRING_DATA[VALUE_IDX]);

                ASSERT(Z.typeIndex()       == X.typeIndex());
                ASSERT(Z.is<TestString>()  == X.is<TestString>());
                ASSERT(Z.the<TestString>() == X.the<TestString>());
                ASSERT(Z.typeIndex()       == Y.typeIndex());
                ASSERT(Z.is<TestString>()  == Y.is<TestString>());
                ASSERT(Z.the<TestString>() == Y.the<TestString>());

              } break;
              case TEST_VOID_TYPE : {

                ASSERT(0 == da.numBlocksTotal());

                // The extra parenthesis around 'TestVoid()' is necessary since
                // the compiler (both SUN and gcc) confuses the construction of
                // a temporary with a function pointer to the constructor.

                Obj mX((TestVoid()));                        const Obj& X = mX;

                ASSERT(TEST_VOID_TYPE == X.typeIndex());
                ASSERT(1              == X.is<TestVoid>());
                ASSERT(TestVoid()     == X.the<TestVoid>());
                ASSERT(0              == da.numBlocksTotal());
                ASSERT(0              == ta.numBlocksTotal());

                const int CURRENTTOTAL = static_cast<int>(da.numBlocksTotal());

                Obj mY(TestVoid(), &ta);                     const Obj& Y = mY;

                ASSERT(TEST_VOID_TYPE == Y.typeIndex());
                ASSERT(1              == Y.is<TestVoid>());
                ASSERT(TestVoid()     == Y.the<TestVoid>());
                ASSERT(CURRENTTOTAL   == da.numBlocksTotal());
                ASSERT(0              == ta.numBlocksTotal());

                Obj mZ;                                      const Obj& Z = mZ;
                mZ.assign(TestVoid());

                ASSERT(Z.typeIndex()     == X.typeIndex());
                ASSERT(Z.is<TestVoid>()  == X.is<TestVoid>());
                ASSERT(Z.the<TestVoid>() == X.the<TestVoid>());
                ASSERT(Z.typeIndex()     == Y.typeIndex());
                ASSERT(Z.is<TestVoid>()  == Y.is<TestVoid>());
                ASSERT(Z.the<TestVoid>() == Y.the<TestVoid>());

              } break;
              default : {
                ASSERT(!"Not reachable by design");
              }
            }

            const Obj X(int(3));
            ASSERT(1 == X.is<int>());
            ASSERT(3 == X.the<int>());
        }
    }

};

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

//..
    class my_PrintVisitor {
      public:
        template <class TYPE>
        void operator()(const TYPE& value) const
        {
            bsl::cout << value << bsl::endl;
        }

        void operator()(bslmf::Nil /* value */) const
        {
            bsl::cout << "null" << bsl::endl;
        }
    };
//..

//..
    class my_AddVisitor {
      public:
        typedef bool ResultType;

        //*************************************************************
        // Note that the return type of 'operator()' is 'ResultType'. *
        //*************************************************************

        template <class TYPE>
        ResultType operator()(TYPE& /* value */) const
            // Return 'true' when addition is performed successfully, and
            // 'false' otherwise.
        {
            if (bslmf::IsConvertible<TYPE, double>::VALUE) {

                // Add certain values to the variant.  The details are elided
                // as it is the return value that is the focus of this example.

                return true;                                          // RETURN
            }
            return false;
        }
    };
//..

//..
    class ThirdPartyVisitor {
      public:
        template <class TYPE>
        bsl::string operator()(const TYPE& /* value */) const /* ; */
            // Return the name of the specified 'value' as a 'bsl::string'.
            // Note that the implementation of this class is deliberately not
            // shown since this class belongs to a third-party library.
        {
            return "dummy";
        }
    };
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // zero is always the leading case.
      case 22: {
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

///Usage
///-----
// This section illustrates intended use of this component.
//
        {
///Example 1: Variant Construction
///- - - - - - - - - - - - - - - -
// The following example illustrates the different ways of constructing a
// 'bdlb::Variant':
//..
    typedef bdlb::Variant <int, double, bsl::string> List;
    typedef bdlb::Variant3<int, double, bsl::string> List3;  // equivalent
//..
// The contained types can be retrieved as a 'bslmf::TypeList' (using the
// 'TypeList' nested type), or individually (using 'TypeN', for 'N' varying
// from 1 to the length of the 'TypeList').  In the example below, we use the
// 'List' variant, but this could be substituted with 'List3' with no change
// to the code:
//..
    ASSERT(3 == List::TypeList::LENGTH);
    ASSERT(3 == List3::TypeList::LENGTH);
//..
// We can check whether the variant defaults to the unset state by using the
// 'is<TYPE>' and 'typeIndex' methods:
//..
    List x;

    ASSERT(!x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT(!x.is<bsl::string>());
    ASSERT(0 == x.typeIndex());
//..
// Single-argument construction from a type in the 'TypeList' of a variant is
// also supported.  This is more efficient than creating an unset variant and
// assigning a value to it:
//..
    List3 y(bsl::string("Hello"));

    ASSERT(!y.is<int>());
    ASSERT(!y.is<double>());
    ASSERT( y.is<bsl::string>());

    ASSERT("Hello" == y.the<bsl::string>());
//..
// Furthermore, 'createInPlace' is provided to support direct in-place
// construction.  This method allows users to directly construct the target
// type inside the variant, instead of first creating a temporary object, then
// copy constructing the object to initialize the variant:
//..
    List z;
    z.createInPlace<bsl::string>("Hello", 5);

    ASSERT(!z.is<int>());
    ASSERT(!z.is<double>());
    ASSERT( z.is<bsl::string>());

    ASSERT("Hello" == z.the<bsl::string>());
//..
// Up to 14 constructor arguments are supported for in-place construction of
// an object.  Users can also safely create another object of the same or
// different type in a variant that already holds a value using the
// 'createInPlace' method.  No memory will be leaked in all cases and the
// destructor for the currently held object will be invoked:
//..
    z.createInPlace<bsl::string>("Hello", 5);
    ASSERT(z.is<bsl::string>());
    ASSERT("Hello" == z.the<bsl::string>());

    z.createInPlace<double>(10.0);
    ASSERT(z.is<double>());
    ASSERT(10.0 == z.the<double>());

    z.createInPlace<int>(10);
    ASSERT(z.is<int>());
    ASSERT(10 == z.the<int>());
//..
        }
        {
//
///Example 2: Variant Assignment
///- - - - - - - - - - - - - - -
// A value of a given type can be stored in a variant in three different ways:
//..
//: o 'operator='
//: o 'assignTo<TYPE>'
//: o 'assign'
//..
// 'operator=' automatically deduces the type that the user is trying to assign
// to the variant.  This should be used most of the time.  The 'assignTo<TYPE>'
// method should be used when conversion to the type that the user is assigning
// to is necessary (see the first two examples below for more details).
// Finally, 'assign' is equivalent to 'operator=' and exists simply for
// backwards compatibility.
//
///'operator='
/// -  -  -  -
// The following example illustrates how to use 'operator=':
//..
    typedef bdlb::Variant <int, double, bsl::string> List;

    List x;

    List::Type1 v1 = 1;       // 'int'
    List::Type2 v2 = 2.0;     // 'double'
    List::Type3 v3("hello");  // 'bsl::string'

    x = v1;
    ASSERT( x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT(!x.is<bsl::string>());
    ASSERT(v1 == x.the<int>());

    x = v2;
    ASSERT(!x.is<int>());
    ASSERT( x.is<double>());
    ASSERT(!x.is<bsl::string>());
    ASSERT(v2 == x.the<double>());

    x = v3;
    ASSERT(!x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT( x.is<bsl::string>());
    ASSERT(v3 == x.the<bsl::string>());
//..
// Note that the type of the object can be deduced automatically during
// assignment, as in:
//..
//  x = v1;
//..
// This automatic deduction, however, cannot be extended to conversion
// constructors, such as:
//..
//  x = (const char *)"Bye";  // ERROR
//..
// The compiler will diagnose that 'const char *' is not a variant type
// specified in the list of parameter types used in the definition of 'List',
// and will trigger a compile-time assertion.  To overcome this problem, see
// the next usage example of 'assignTo<TYPE>'.
//
///'assignTo<TYPE>'
///-  -  -  -  -  -
// In the previous example, 'const char *' was not part of the variant's type
// list, which resulted in a compilation diagnostic.  The use of
// 'assignTo<TYPE>' explicitly informs the compiler of the intended type to
// assign to the variant:
//..
    x.assignTo<bsl::string>((const char*)"Bye");

    ASSERT(!x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT( x.is<bsl::string>());

    ASSERT("Bye" == x.the<bsl::string>());
//..
//
///'assign'
/// -  -  -
// Finally, for backwards compatibility, 'assign' can also be used in place of
// 'operator=' (but not 'assignTo'):
//..
    x.assign<int>(v1);
    ASSERT( x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT(!x.is<bsl::string>());
    ASSERT(v1 == x.the<int>());

    x.assign<double>(v2);
    ASSERT(!x.is<int>());
    ASSERT( x.is<double>());
    ASSERT(!x.is<bsl::string>());
    ASSERT(v2 == x.the<double>());

    x.assign<bsl::string>(v3);
    ASSERT(!x.is<int>());
    ASSERT(!x.is<double>());
    ASSERT( x.is<bsl::string>());
    ASSERT(v3 == x.the<bsl::string>());
//..
        }
        {
//
///Example 3: Visiting a Variant via 'apply'
///- - - - - - - - - - - - - - - - - - - - -
// As described in {Visitors} (above), there are different ways to invoke the
// 'apply' method.  The first two examples below illustrate the different ways
// to invoke 'apply' (with no return value) to control the behavior of visiting
// an unset variant:
//..
//: o 'bslmf::Nil' is passed to the visitor.
//:
//: o A user-specified default value is passed to the visitor.
//..
// A third example illustrates use of 'applyRaw', the behavior of which is
// undefined if the variant is unset.  Two final examples illustrate different
// ways to specify the return value from 'apply:
//..
//: o The return value is specified in the visitor.
//:
//: o The return value is specified with the function call.
//..
//
///'bslmf::Nil' Passed to Visitor
///-  -  -  -  -  -  -  -  -  - -
// A simple visitor that does not require any return value might be one that
// prints the value of the variant to 'stdout':
//..
//  class my_PrintVisitor {
//    public:
//      template <class TYPE>
//      void operator()(const TYPE& value) const
//      {
//          bsl::cout << value << bsl::endl;
//      }
//
//      void operator()(bslmf::Nil value) const
//      {
//          bsl::cout << "null" << bsl::endl;
//      }
//  };
//
    typedef bdlb::Variant <int, double, bsl::string> List;

    List x[4];

    //*************************************
    // Note that 'x[3]' is uninitialized. *
    //*************************************

    x[0].assign(1);
    x[1].assign(1.1);
    x[2].assignTo<bsl::string>((const char *)"Hello");

    my_PrintVisitor printVisitor;

    for (int i = 0; i < 4; ++i) {
        x[i].apply(printVisitor);
    }
//..
// The above prints the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  null
//..
// Note that 'operator()' is overloaded with 'bslmf::Nil'.  A direct match has
// higher precedence than a template parameter match.  When the variant is
// unset (such as 'x[3]'), a 'bslmf::Nil' is passed to the visitor.
//
///User-Specified Default Value Passed to Visitor
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Instead of using 'bslmf::Nil', users can also specify a default value to
// pass to the visitor when the variant is currently unset.  Using the same
// 'my_PrintVisitor' class from previous example:
//..
    for (int i = 0; i < 4; ++i) {
        x[i].apply(printVisitor, "Print this when unset");
    }
//..
// Now, the above code prints the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  Print this when unset
//..
// This variation of 'apply' is useful since the user can provide a default
// value to the visitor without incurring the cost of initializing the variant
// itself.
//
///'applyRaw' Undefined If Variant Is Unset
///-  -  -  -  -  -  -  -  -  -  -  -  -  -
// If it is certain that a variant is not unset, then the 'applyRaw' method can
// be used instead of 'apply'.  'applyRaw' is slightly more efficient than
// 'apply', but the behavior of 'applyRaw' is undefined if the variant is
// unset.  In the following application of 'applyRaw', we purposely circumvent
// 'x[3]' from being visited because we know that it is unset:
//..
    for (int i = 0; i < 3; ++i) {     // NOT 'i < 4' as above.
        ASSERT(!x[i].isUnset());
        x[i].applyRaw(printVisitor);  // undefined behavior for 'x[3]'
    }
    ASSERT(x[3].isUnset());
//..
//
        }
        {
///Return Value Specified in Visitor
///  -  -  -  -  -  -  -  -  -  -  -
// Users can also specify a return type that 'operator()' will return by
// specifying a 'typedef' with the name 'ResultType' in their functor class.
// This is necessary in order for the 'apply' method to know what type to
// return at compile time:
//..
//  class my_AddVisitor {
//    public:
//      typedef bool ResultType;
//
//      //*************************************************************
//      // Note that the return type of 'operator()' is 'ResultType'. *
//      //*************************************************************
//
//      template <class TYPE>
//      ResultType operator()(TYPE& value) const
//          // Return 'true' when addition is performed successfully, and
//          // 'false' otherwise.
//      {
//          if (bslmf::IsConvertible<TYPE, double>::VALUE) {
//
//              // Add certain values to the variant.  The details are elided
//              // as it is the return value that is the focus of this example.
//
//              return true;
//          }
//          return false;
//      }
//  };
//
    typedef bdlb::Variant <int, double, bsl::string> List;

    List x[3];

    x[0].assign(1);
    x[1].assign(1.1);
    x[2].assignTo<bsl::string>((const char *)"Hello");

    my_AddVisitor addVisitor;

    bool ret[3];

    for (int i = 0; i < 3; ++i) {
        ret[i] = x[i].apply(addVisitor);
        if (!ret[i]) {
            bsl::cout << "Cannot add to types not convertible to 'double'."
                      << bsl::endl;
        }
    }
    ASSERT(true  == ret[0]);
    ASSERT(true  == ret[1]);
    ASSERT(false == ret[2]);
//..
// The above prints the following on 'stdout':
//..
//  Cannot add to types not convertible to 'double'.
//..
// Note that if no 'typedef' is provided (as in the 'my_PrintVisitor' class),
// then the default return value is 'void'.
        }
        {
//
///Return Value Specified With Function Call
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -
// There may be some cases when a visitor interface is not owned by a client
// (hence the client cannot add a 'typedef' to the visitor), or the visitor
// could not determine the return type at design time.  In these scenarios,
// users can explicitly specify the return type when invoking 'apply':
//..
//  class ThirdPartyVisitor {
//    public:
//      template <class TYPE>
//      bsl::string operator()(const TYPE& value) const;
//          // Return the name of the specified 'value' as a 'bsl::string'.
//          // Note that the implementation of this class is deliberately not
//          // shown since this class belongs to a third-party library.
//  };
//
    typedef bdlb::Variant <int, double, bsl::string> List;

    List x[3];

    x[0].assign(1);
    x[1].assign(1.1);
    x[2].assignTo<bsl::string>((const char *)"Hello");

    ThirdPartyVisitor visitor;

    for (int i = 0; i < 3; ++i) {

        //*****************************************************
        // Note that the return type is explicitly specified. *
        //*****************************************************

        bsl::string ret = x[i].apply<bsl::string>(visitor);
        bsl::cout << ret << bsl::endl;
    }
//..
        }

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Concerns:
        //   1. That unset variants can be swapped successfully.
        //   2. That if one (and only one) of the variants is unset, the swap
        //      can be completed successfully.
        //   3. That if both variants are set, the swap can be completed
        //      successfully.
        //
        // Plan:
        //
        // Testing:
        //   void swap(bdlb::VariantImp& rhs);
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase21();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // CONCERN: 'applyRaw' accepts VISITORs without a 'bslmf::Nil' overload
        //
        // Concerns:
        //: 1 That 'applyRaw' can be called on a VISITOR functor that does not
        //:   provide an overload for 'bslmf::Nil'.
        //:
        //: 2 That 'applyRaw' can be called on both 'const' and non-'const'
        //:   VISITOR functors.
        //:
        //: 3 That 'applyRaw' can be called on both 'const' and non-'const'
        //:   variants.
        //:
        //: 4 That 'applyRaw' can be called on functors that define and do not
        //:   define a 'ResultType`.
        //
        // Plan:
        //: 1 Call 'applyRaw' using a 'VISITOR' that does not have an overload
        //:   for 'bslmf::Nil' on a cross product of:
        //:   o 'const' and non-'const' variants.
        //:   o 'const' and non-'const' visitors.
        //:   o A visitor functor that:
        //:     o Declares a result type.
        //:     o Does not declare a result type and returns 'void'.
        //:     o Does not declare a result type and doesn't return 'void'.
        //:
        //: 2 Call 'applyRaw<RESULT_TYPE>' (i.e., without using template
        //:   argument deduction to determine the result type) using a visitor
        //:   that does not have an overload of 'Nil' for the same cross
        //:   product of situations as (1).
        //
        // Testing:
        //  CONCERN: 'applyRaw' accepts VISITORs w/o a 'bslmf::Nil' overload
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase20();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //   1. If no allocator pointer is needed, it will not be a part of the
        //      variant members, i.e., 'sizeof(bdlb::Variant)' should go down
        //      by 'sizeof(bslma::Allocator *)'.
        //   2. That the 'bslma::UsesBslmaAllocator' trait is declared for the
        //      variant when any types it holds has the trait.  If none of the
        //      types has the trait, then the variant itself will not have the
        //      trait.
        //   3. When any types held by the variant does not have the
        //      'bsl::is_trivially_copyable' trait, then the variant itself
        //      will not have the bitwise moveable trait.
        //   4. When any types held by the variant does not have the
        //      'bslmf::IsBitwiseMoveable' trait, then the variant itself will
        //      not have the bitwise moveable trait.
        //
        // Plan:
        //   To address all the concerns, create a variety of types that have
        //   the following traits:
        //       o 'bslma::UsesBslmaAllocator'
        //       o 'bsl::is_trivially_copyable'
        //       o 'bslmf::IsBitwiseMoveable'
        //       o no traits
        //   Then populate the variant with different types and different
        //   scenarios outlined in the concerns section.  Finally, check
        //   whether the expected traits are declared (or not).
        //
        // Testing:
        //   CONCERN: No allocator pointer in object if not necessary.
        //   CONCERN: No 'bslma::UsesBslmaAllocator' trait when no allocator.
        //   CONCERN: 'bsl::is_trivially_copyable' trait
        //   CONCERN: 'bslmf::IsBitwiseMoveable' trait
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase19();

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING CLASSES 'bdlb::VariantN'
        //   So far, our testing has tested the core functionality of the
        //   'bdlb::VariantImp' class, through the thin 'bdlb::Variant'
        //   wrapper.  We have focused on the value-semantic portion of the
        //   functionality offered by this component.  We now address the
        //   concerns related to the implementation of the core
        //   'bdlb::VariantImp' and a multitude of thin wrappers (that derive
        //   from 'bdlb::VariantImp') that offer extra constructors and traits.
        //
        // Concerns:
        //   1. That the 'bdlb::VariantN' class templates take exactly 'N'
        //      template arguments.
        //   2. That the 'bdlb::VariantN' class constructor takes a single
        //      argument of one of the 'N' types specified in its type list.
        //   3. That the 'bdlb::VariantN' class 'operator=' takes a single
        //      argument of one of the 'N' types specified in its type list.
        //   4. That the 'bdlb::Variant' class template takes a variable
        //      number of template arguments.
        //   5. That the traits are set properly for the thin wrappers.
        //   6. That the constructors propagate the allocator properly.
        //   7. That the type indices are set properly.
        //
        // Plan:
        //   To address concern 1, we instantiate each type with up to 20
        //   parameters of distinct types 'TestArg<1>' to 'TestArg<20>'.
        //   For Concern 2, we instantiate 'bdlb::Variant' with each
        //   combination of up to 20 parameters and verify that the remaining
        //   ones default to 'bslmf::TypeListNil'.
        //
        //   For concern 3, we manually check the traits of each class.
        //
        //   For concern 4, we verify that the allocator is correctly
        //   propagated to a 'TestString' variant type.  Note that the
        //   treatment of allocators being done through an applicator is
        //   independent of the index of the type in the type list, and thus it
        //   is enough to put 'TestString' as the first type.
        //
        //   For concern 5, we check the index of every type in the variant and
        //   verify that it is as expected.  We also check the nested
        //   'TypeList' and 'TypeN' types.
        //
        // Testing:
        //   CLASSES: 'bdlb::VariantN', 'bdlb::Variant'
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase18();

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'isUnset'
        //
        // Concerns:
        //   1. That 'isUnset' returns 'true' for an unset variant (default
        //      constructed or after 'reset'), and 'false' otherwise.
        //   2. That when 'bslmf::Nil' is used as one of the types in the
        //      type list, 'isUnset' still returns false even when the variant
        //      is initialized to 'bslmf::Nil'.
        //
        // Plan:
        //   To address concern 1, create two variants, both default
        //   constructed, and verify that 'isUnset' returns 'true'.  Assign a
        //   value to both the variants, and verify that 'isUnset' returns
        //   'false'.  Finally, invoke 'reset' on the first variant, and verify
        //   that 'isUnset' returns 'true' on the first variant and 'false' on
        //   the second variant.
        //
        //   For concern 2, create a variant with 'bslmf::Nil' in its type
        //   list, then initialize it to a nil value.  Verify that 'isUnset'
        //   returns 'false'.
        //
        // Testing:
        //   bool isUnset() const;
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase17();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING VISITORS (unset variant)
        //
        // Concerns:
        //   1. That 'bslmf::Nil' is passed to the visitor if 'apply' is
        //      invoked on an unset variant.
        //   2. That, if specified, 'defaultValue' is passed to the visitor if
        //      'apply' is invoked on an unset variant.
        //   3. That an assert is triggered when 'applyRaw' is invoked on an
        //      unset variant in debug mode.
        //
        // Plan:
        //   To address concerns 1 and 2, create a visitor that has three
        //   'operator()' overloads.  The first one is a parameterized
        //   'operator()' that takes any type.  The second one is one that
        //   takes a 'bslmf::Nil'.  The last one is one that takes a
        //   user-defined type, and in this test case, 'TestArg<1>'.  Verify
        //   that the proper 'operator()' is invoked when the variant is unset.
        //
        //   To address concern 3, use a 'bsls::AssertFailureHandlerGuard' and
        //   provide an assertion handler.  In the assertion handler, record
        //   that an assert is fired in debug mode.
        //
        // Testing:
        //   VISITOR::ResultType apply(VISITOR& visitor, const TYPE& dVal);
        //   void apply(VISITOR& visitor, const TYPE& dVal);
        //   RET_TYPE apply(VISITOR& visitor, const TYPE& dVal);
        //   VISITOR::ResultType applyRaw(VISITOR& visitor);
        //   void applyRaw(VISITOR& visitor);
        //   RET_TYPE applyRaw(VISITOR& visitor);
        //   VISITOR::ResultType apply(VISITOR& v, const TYPE& dVal) const;
        //   void apply(VISITOR& visitor, const TYPE& dVal) const;
        //   RET_TYPE apply(VISITOR& visitor, const TYPE& dVal) const;
        //   VISITOR::ResultType applyRaw(VISITOR& visitor) const;
        //   void applyRaw(VISITOR& visitor) const;
        //   RET_TYPE applyRaw(VISITOR& visitor) const;
        // --------------------------------------------------------------------


        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase16();

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING VISITORS (return values)
        //
        // Concerns:
        //   1. Visitors can modify the value currently held by the variant
        //      (meaning visitors are not read-only).
        //   2. When no 'ResultType' is defined as a class type of the
        //      visitor, the 'apply' method will not return anything.
        //   3. When 'ResultType' is defined as a class type of the visitor,
        //      the 'apply' method returns 'ResultType'.
        //   4. When 'RET_TYPE' is specified as an explicit function template
        //      parameter, the 'apply' method returns 'RET_TYPE', overriding
        //      any return type specified in 'ResultType'.
        //   5. 'const' variants can be visited by visitors with an
        //      'operator()' that takes a 'const' reference to the value held
        //      by the visitor.
        //   6. Unless explicitly modified by the visitor, visiting the
        //      variant does not modify the variant in any way.
        //
        // Plan:
        //   To address concern 1, simply create a visitor that modifies
        //   values of the variant according to the type of object currently
        //   held by the variant.
        //
        //   To address concerns 2, 3, and 4, create a wrapper for
        //   'bdlb::Variant' that records which 'apply' method is invoked.
        //   Verify that the correct method is invoked depending on the
        //   requirements listed in the concerns.  Furthermore, for visitors
        //   that return values, verify that the value returned is as expected.
        //   Finally, to verify that the return value is overridden when
        //   explicitly specified in the 'apply' method, define a class
        //   'Convertible' that is implicitly convertible from any class.
        //   Specify 'Convertible' explicitly as the return type, and invoke
        //   a member function 'memberFunc' of 'Convertible' directly on the
        //   value returned by the 'apply' method:
        //
        //      (myVariant.apply<Convertible>(visitor)).memberFunc();
        //
        //   The above is necessary because we cannot do a comparison (due to
        //   implicit conversion of any type to class 'Convertible').  If the
        //   value returned is not the same as the explicitly specified type,
        //   'memberFunc' will not be defined and the above will fail to
        //   compile.
        //
        //   To address concern 5, create a 'const' variant and a visitor that
        //   has a 'operator()' that takes a 'const' reference to the value
        //   held by the visitor.  In the visitor, verify the value is properly
        //   passed.
        //
        //   To address concern 6, create a (non-'const') variant and use the
        //   visitor of concern 5 to visit it.  After visiting, verify that the
        //   value held by the variant is not modified in any way.
        //
        // Testing:
        //   VISITOR::ResultType apply(VISITOR& visitor);
        //   VISITOR::ResultType apply(const VISITOR& visitor);
        //   VISITOR::ResultType applyRaw(const VISITOR& visitor);
        //   void apply(VISITOR& visitor);
        //   void apply(const VISITOR& visitor);
        //   void applyRaw(const VISITOR& visitor);
        //   RET_TYPE apply<RET_TYPE>(VISITOR& visitor);
        //   RET_TYPE apply<RET_TYPE>(const VISITOR& visitor);
        //   RET_TYPE applyRaw<RET_TYPE>(const VISITOR& visitor);
        //   VISITOR::ResultType apply(VISITOR& visitor) const;
        //   VISITOR::ResultType apply(const VISITOR& visitor) const;
        //   VISITOR::ResultType applyRaw(const VISITOR& visitor) const;
        //   void apply(VISITOR& visitor) const;
        //   void apply(const VISITOR& visitor) const;
        //   void applyRaw(const VISITOR& visitor) const;
        //   RET_TYPE apply<RET_TYPE>(VISITOR& visitor) const;
        //   RET_TYPE apply<RET_TYPE>(const VISITOR& visitor) const;
        //   RET_TYPE applyRaw<RET_TYPE>(const VISITOR& visitor) const;
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase15();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'createInPlace'
        //
        // Concerns:
        //   1. The 'createInPlace' method does not invoke the copy constructor
        //      of the object being created.
        //   2. The variant holds the correct type and value of the object
        //      being created inside the variant (implying that all constructor
        //      arguments are forwarded properly).
        //
        // Plan:
        //   Define a 'Copyable' class whose copy constructor is monitored and
        //   has a constructor that takes up to 14 arguments.  Also provide an
        //   accessor to the 15 arguments being passed in.  Then invoke all 15
        //   versions of 'createInPlace' and verify the arguments are forwarded
        //   properly.  Also assert that the copy constructor is never invoked.
        //
        // Testing:
        //   void createInPlace<TYPE>(...);                // all 15 variations
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase14();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TYPE ASSIGNMENT
        //
        // Concerns:
        //   1. The value represented by any object of a variant-type can be
        //      assigned to by any other object of a type specified on
        //      that variant-type's type list.
        //   2. The assignment operator returns a reference to the destination
        //      object.
        //   3. The 'value' passed to 'operator=' must not be affected by the
        //      operation.
        //   4. The 'value' going out of scope has no effect on the value of
        //      the variant after the assignment.
        //   5. The allocator currently held by the variant is used to allocate
        //      memory for the assignment.
        //   6. The assignment operator must be exception neutral with no
        //      guarantee of rollback.
        //
        // Plan:
        //   To address concerns 1, 2, 3, and 4, specify a set S of unique
        //   object values with substantial and varied differences.  Construct
        //   variant(X) = Y for all (X, Y) in S x S.  Also create YY that has
        //   the same value as Y.  After the assignment, assert that
        //   variant(X), Y, and the value returned by the assignment operator
        //   all equal YY.  Let Y go out of scope and confirm that YY equals
        //   variant(X).
        //
        //   To address concern 5, we create an object with the default
        //   allocator and assign to it an object with a test allocator.
        //   Then we verify that the memory of the new object is drawn from the
        //   default allocator.
        //
        //   To address concern 6, we use a standard 'bslma' exception test.
        //
        // Testing:
        //   bdlb::VariantImp& operator=(const TYPE& value);
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase13();

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //   1. When a 'bslma::Allocator' pointer is passed to the one-argument
        //      constructor, the allocator should be used to supply memory
        //      for construction.
        //   2. When an object of a type in the variant's type list is passed
        //      to the one-argument constructor, the variant should be
        //      constructed with the specified type and value.  The currently
        //      installed default allocator should be used to supply memory for
        //      construction (if the default constructed variant uses an
        //      allocator).
        //   3. When the two-argument constructor is invoked, the variant
        //      should be constructed with the specified type and value.
        //      Memory should be supplied by the specified allocator (if the
        //      default constructed variant uses an allocator).
        //
        // Plan:
        //   To address concern 1, we make use of both the default allocator
        //   guard and the 'bslma::TestAllocator'.  First, create two test
        //   allocators, then hook one up with the default allocator guard.
        //   Next, construct a variant passing the second test allocator in.
        //   Verify that construction and subsequent allocations (using
        //   'assign') uses the allocator that is passed in.  Also verify that
        //   no memory allocations come from the first (default) allocator.
        //
        //   For concern 2, using the table-driven technique, construct every
        //   type in the type list of the variant by passing in a
        //   preconstructed object of that type.  Verify that the type and
        //   value are same as the preconstructed object, and also verify that
        //   the type and value are the same as a variant object that is first
        //   default constructed, then assigned to the preconstructed object.
        //   Finally, verify that all memory comes from the default allocator
        //   using a default allocator guard and a test allocator.
        //
        //   For concern 3, construct a variant using the two-argument
        //   constructor and verify both concerns 1 and 2 at the same time.
        //   The same table will be used to test this.
        //
        // Testing:
        //   bdlb::VariantImp(const TYPE_OR_ALLOCATOR& typeOrAlloc);
        //   bdlb::VariantImp(const TYPE& value, bslma::Allocator *ba);
        // --------------------------------------------------------------------

        // This test case is defined outside of 'main' to avoid out-of-memory
        // errors with the XLC compiler.

        TestUtil::testCase12();

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'assignTo'
        //
        // Concerns:
        //   A call to the 'assignTo' method that specifies the value type
        //   should not invoke the copy constructor.
        //
        // Plan:
        //   Define a 'copyable' class whose copy constructor is monitored.
        //   Invoke the 'assign' method of a variant that wraps this
        //   'copyable' class.  Verify that the copy constructor has not been
        //   called.
        //
        // Testing:
        //   bdlb::VariantImp& assignTo(SOURCE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'assignTo'" << endl
                          << "==================" << endl;

        if (verbose) cout << "\nWith custom 'Copyable' type." << endl;
        {
            ASSERT(false == Copyable::s_copyConstructorCalled);
            bdlb::VariantImp<bslmf::TypeList<Copyable> > variant;
            variant.assignTo<Copyable>(0);
            ASSERT(false == Copyable::s_copyConstructorCalled);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   'bdlb::Variant' does not support BDEX streaming.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for BDEX streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "BDEX streaming is not supported." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   1. The value represented by any object of a variant type can be
        //      assigned to by another object of the same variant type and that
        //      the assignment operator returns a reference to the destination
        //      object.
        //   2. The 'rhs' value must not be affected by the operation.
        //   3. 'rhs' going out of scope has no effect on the value of 'lhs'
        //      after the assignment.
        //   4. Aliasing (x = x): The assignment operator must always work --
        //      even when the 'lhs' and 'rhs' are identically the same object.
        //   5. The allocator value is not part of the assignment.
        //   6. The assignment operator must be exception neutral with no
        //      guarantee of rollback.
        //
        // Plan:
        //   To address concerns 1, 2, and 3, specify a set S of unique object
        //   values with substantial and varied differences.  Construct tests
        //   X = Y for all (X, Y) in S x S.  For each of these test, generate Y
        //   and YY using the same 'SPEC'.  After the assignment, assert that
        //   X, Y, and the value returned by the assignment operator all equal
        //   YY.  Let Y go out of scope and confirm that YY equals X.
        //
        //   To address concern 4, we perform Y = Y and verify that both the
        //   return value of the assignment and Y itself equal YY.
        //
        //   To address concern 5, we create an object with the default
        //   allocator and assign to it an object with a test allocator.  Then
        //   we verify that the memory of the new object is drawn from the
        //   default allocator.
        //
        //   To address concern 6, we use a standard 'bslma' exception test.
        //
        // Testing:
        //   bdlb::Variant& operator=(const bdlb::Variant& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
                "S", "T", "U", "V", "W", "K", "L", "M", "N", "O", "Z",
        0}; // Null string required as last element.

        for (int i = 0; SPECS[i]; ++i) {

            if (veryVerbose) {
                const Obj XX(g(SPECS[i]));
                T_ T_ P_(i); P_(SPECS[i]); P(XX);
            }

            for (int j = 0; SPECS[j]; ++j) {
                const Obj XX(g(SPECS[i]), &testAllocator);
                const Obj YY(g(SPECS[j]), &testAllocator);

                if (veryVerbose) {
                    T_ T_ P_(j) P_(SPECS[j]) P(YY)
                }

                // In the block below, objects 'mX' and 'mZ' are assigned the
                // value of 'mY'.  After 'mY' goes out of scope, we verify that
                // 'mX' still has the value of 'mY'.  And when 'mZ' goes out of
                // scope, its destructor will assert its invariants.

                {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    gg(&mX, SPECS[i]);
                    {
                        Obj mY(&testAllocator); const Obj& Y = mY;
                        Obj mZ(&testAllocator); const Obj& Z = mZ;
                        gg(&mY, SPECS[j]);
                        gg(&mZ, SPECS[i]);

                        LOOP_ASSERT(i, YY == (mY = Y));
                        LOOP_ASSERT(i, YY == Y);
                        LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                        LOOP2_ASSERT(i, j, YY == (mX = Y));
                        LOOP2_ASSERT(i, j, YY == (mZ = Y));
                        LOOP2_ASSERT(i, j, YY == Y);
                        LOOP2_ASSERT(i, j, YY == X);
                        LOOP2_ASSERT(i, j, YY == Z);
                    }
                    LOOP2_ASSERT(i, j, YY == X);
                }

                // Verify that the allocator value is not part of the
                // assignment.

                {
                    // Compute the number of blocks needed for this
                    // assignment.

                    Obj mX(&testAllocator);
                    gg(&mX, SPECS[i]);
                    int blocks =
                              static_cast<int>(testAllocator.numBlocksTotal());
                    mX = YY;
                    blocks = static_cast<int>(testAllocator.numBlocksTotal())
                             - blocks;

                    // Assign 'YY' (which uses a test allocator) to 'mY' (which
                    // uses the default allocator).  The allocator value of
                    // 'mY' should not be affected.

                    bslma::TestAllocator da; // default allocator
                    const bslma::DefaultAllocatorGuard DAG(&da);
                    Obj mY;
                    gg(&mY, SPECS[i]);
                    int defaultBlocks = static_cast<int>(da.numBlocksTotal());
                    int testBlocks    =
                             static_cast<int>(testAllocator.numBlocksTotal());
                    mY = YY;
                    defaultBlocks = static_cast<int>(da.numBlocksTotal())
                                    - defaultBlocks;

                    // Verify 'mY' still uses the default allocator to obtain
                    // memory and the test allocator used by 'YY' is not used,
                    // thus proving the allocator value of 'mY' is unaffected
                    // by the assignment.

                    LOOP3_ASSERT(i, blocks, defaultBlocks,
                                                      blocks == defaultBlocks);
                    LOOP3_ASSERT(i, testBlocks, testAllocator.numBlocksTotal(),
                                 testBlocks == testAllocator.numBlocksTotal());
                }

                // Testing exception safety.

                {
                    Obj mX(&testAllocator); const Obj& X = mX;
                    Obj mY(&testAllocator); const Obj& Y = mY;
                    gg(&mX, SPECS[i]);
                    gg(&mY, SPECS[j]);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                      mX = Y;

                      LOOP2_ASSERT(i, j, X == Y);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, compare the address of the
        //   temporary object returned by 'g' to show that 'g' returns an
        //   object by value.
        //
        // Testing:
        //   bdlb::Variant g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING GENERATOR FUNCTION 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
                "S", "T", "U", "V", "W", "K", "L", "M", "N", "O", "Z",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];

            if (veryVerbose) { P_(ti);  P(spec); }

            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;

            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }

            const int TOTAL_BLOCKS_BEFORE =
                              static_cast<int>(testAllocator.numBlocksTotal());
            const int IN_USE_BYTES_BEFORE =
                              static_cast<int>(testAllocator.numBytesInUse());
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER =
                              static_cast<int>(testAllocator.numBlocksTotal());
            const int IN_USE_BYTES_AFTER =
                              static_cast<int>(testAllocator.numBytesInUse());
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "K";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            const Obj& r1 = g(spec);
            const Obj& r2 = g(spec);
            ASSERT(&r2 != &r1);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //  1. The new object's value is the same as that of the original
        //     object.
        //  2. The value of the original object is left unaffected.
        //  3. Subsequent changes in or destruction of the source object have
        //     no effect on the copy-constructed object.
        //  4. The object has its internal memory management system hooked up
        //     properly so that *all* internally allocated memory draws from a
        //     user-supplied allocator whenever one is specified, and otherwise
        //     the default one.
        //
        // Plan:
        //  To address concern 1 - 3, specify a set S of object values with
        //  substantial and varied differences.  For each value in S,
        //  initialize objects W and X, copy construct Y from X, and use
        //  'operator==' to verify that both X and Y subsequently have the same
        //  value as W.  Let X go out of scope and again verify that W == Y.
        //
        //  To address concern 4, we will install a test allocator as the
        //  default and also supply a separate test allocator explicitly.  We
        //  will measure the total usage of both allocators before and after
        //  calling the copy constructor to ensure that no memory is allocated
        //  from the default allocator.  Then we will call the copy constructor
        //  to create another object using the default allocator and verify
        //  that the amount of memory used is the same as that with the
        //  supplied test allocator.
        //
        // Testing:
        //   bdlb::Variant(const bdlb::Variant&, bslma::Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        static struct {
            int         d_lineNum;
            const char *d_input;        // input specifications
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE INPUT               TYPE_IDX  VALUE_IDX
          // ---- -----               --------  ---------
            { L_, "",                    UNSET,         0 }
          , { L_, "A",                INT_TYPE,         0 }
          , { L_, "B",                INT_TYPE,         1 }
          , { L_, "C",                INT_TYPE,         2 }
          , { L_, "D",                INT_TYPE,         3 }
          , { L_, "E",                INT_TYPE,         4 }
          , { L_, "F",           TEST_INT_TYPE,         0 }
          , { L_, "G",           TEST_INT_TYPE,         1 }
          , { L_, "H",           TEST_INT_TYPE,         2 }
          , { L_, "I",           TEST_INT_TYPE,         3 }
          , { L_, "J",           TEST_INT_TYPE,         4 }
          , { L_, "S",             STRING_TYPE,         0 }
          , { L_, "T",             STRING_TYPE,         1 }
          , { L_, "U",             STRING_TYPE,         2 }
          , { L_, "V",             STRING_TYPE,         3 }
          , { L_, "W",             STRING_TYPE,         4 }
          , { L_, "K",        TEST_STRING_TYPE,         0 }
          , { L_, "L",        TEST_STRING_TYPE,         1 }
          , { L_, "M",        TEST_STRING_TYPE,         2 }
          , { L_, "N",        TEST_STRING_TYPE,         3 }
          , { L_, "O",        TEST_STRING_TYPE,         4 }
          , { L_, "Z",          TEST_VOID_TYPE,         0 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with allocators." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_lineNum;
            const char *INPUT     = DATA[ti].d_input;
            const int   TYPE_IDX  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE); P_(INPUT); P_(TYPE_IDX); P(VALUE_IDX);
            }

            Obj *pX = new Obj(&testAllocator);  // Dynamic allocation allows us
                                                // to easily control the
                                                // destruction of this object.
            Obj& mX = *pX;  const Obj& X = mX;
            Obj W(&testAllocator);

            gg(&mX, INPUT);
            gg(&W, INPUT);

            bslma::TestAllocator da; // default allocator
            const bslma::DefaultAllocatorGuard DAG(&da);
            bslma::TestAllocator a;  // specified allocator

            LOOP2_ASSERT(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            int blocks = static_cast<int>(a.numBlocksTotal());

            const Obj Y(X, &a);
            blocks = static_cast<int>(a.numBlocksTotal()) - blocks;
            LOOP2_ASSERT(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());

            int defaultBlocks = static_cast<int>(da.numBlocksTotal());
            const Obj Z(X);
            defaultBlocks = static_cast<int>(da.numBlocksTotal())
                            - defaultBlocks;
            LOOP3_ASSERT(LINE, blocks, defaultBlocks, blocks == defaultBlocks);

            LOOP_ASSERT(LINE, W == X);
            LOOP_ASSERT(LINE, W == Y);
            delete pX;  // X is now out of scope.
            LOOP_ASSERT(LINE, W == Y);
        }

        if (verbose)
            cout << "\nTesting with allocators in the presence of exceptions."
                 << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_lineNum;
            const char *INPUT     = DATA[ti].d_input;
            const int   TYPE_IDX  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE); P_(INPUT); P_(TYPE_IDX); P(VALUE_IDX);
            }

            Obj mX;  const Obj& X = mX;
            gg(&mX, INPUT);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const Obj Y(X, &testAllocator);  // TEST HERE

              LOOP_ASSERT(LINE, X == Y);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concern:
        //   For 'operator==', we have the following concerns:
        //   1. Return 'false' for two variants holding different types.
        //   2. Return 'false' for two variants holding the same type but not
        //      holding the same value.
        //   3. Return 'true' for two variants holding the same type and the
        //      same value.
        //   For 'operator!=', we have the same concerns, except the method
        //   should return 'false' whenever 'operator==' returns 'true' and
        //   'true' whenever 'operator==' returns 'false'.
        //
        // Plan:
        //   Using the table-driven technique, construct a set of specs
        //   containing similar but different data values.  Then loop through
        //   the cross product of the test data.  For each tuple, generate two
        //   objects 'U' and 'V' using the previously tested 'gg' function.
        //   Use the '==' and '!=' operators and check their return value for
        //   correctness.
        //
        // Testing:
        //   bool operator==(const bdlb::Variant& l, const bdlb::Variant& r);
        //   bool operator!=(const bdlb::Variant& l, const bdlb::Variant& r);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY OEPRATORS" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u,v) in S X S." << endl;

        static struct {
            int         d_lineNum;
            const char *d_input;        // input specifications
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE INPUT               TYPE_IDX  VALUE_IDX
          // ---- -----               --------  ---------
            { L_, "",                    UNSET,         0 }
          , { L_, "A",                INT_TYPE,         0 }
          , { L_, "B",                INT_TYPE,         1 }
          , { L_, "C",                INT_TYPE,         2 }
          , { L_, "D",                INT_TYPE,         3 }
          , { L_, "E",                INT_TYPE,         4 }
          , { L_, "F",           TEST_INT_TYPE,         0 }
          , { L_, "G",           TEST_INT_TYPE,         1 }
          , { L_, "H",           TEST_INT_TYPE,         2 }
          , { L_, "I",           TEST_INT_TYPE,         3 }
          , { L_, "J",           TEST_INT_TYPE,         4 }
          , { L_, "S",             STRING_TYPE,         0 }
          , { L_, "T",             STRING_TYPE,         1 }
          , { L_, "U",             STRING_TYPE,         2 }
          , { L_, "V",             STRING_TYPE,         3 }
          , { L_, "W",             STRING_TYPE,         4 }
          , { L_, "K",        TEST_STRING_TYPE,         0 }
          , { L_, "L",        TEST_STRING_TYPE,         1 }
          , { L_, "M",        TEST_STRING_TYPE,         2 }
          , { L_, "N",        TEST_STRING_TYPE,         3 }
          , { L_, "O",        TEST_STRING_TYPE,         4 }
          , { L_, "Z",          TEST_VOID_TYPE,         0 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE1      = DATA[ti].d_lineNum;
            const char *INPUT1     = DATA[ti].d_input;
            const int   TYPE1_IDX  = DATA[ti].d_expTypeIdx;
            const int   VALUE1_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE1) P_(INPUT1) P_(TYPE1_IDX) P(VALUE1_IDX)
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int   LINE2      = DATA[tj].d_lineNum;
                const char *INPUT2     = DATA[tj].d_input;
                const int   TYPE2_IDX  = DATA[tj].d_expTypeIdx;
                const int   VALUE2_IDX = DATA[tj].d_expValueIdx;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(INPUT2) P_(TYPE2_IDX) P(VALUE2_IDX)
                }

                Obj mX; const Obj& X = mX;
                gg(&mX, INPUT1);

                Obj mY; const Obj& Y = mY;
                gg(&mY, INPUT2);

                LOOP2_ASSERT(LINE1, LINE2, (ti == tj) == (X == Y));
                LOOP2_ASSERT(LINE1, LINE2, (ti != tj) == (X != Y));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR AND 'print'
        //
        // Concerns:
        //  We want to ensure that the 'print' method correctly formats a
        //  'bdlb::Variant' object for output with any valid 'level' and
        //  'spacesPerLevel' values and returns the specified stream.
        //
        // Plan:
        //  We will verify that 'print' has been properly hooked up with the
        //  'print' method of the variant object by using a small set of
        //  variant objects combined with some 'level' and 'spacesPerLevel'
        //  values as test vectors.  Since 'operator<<' is implemented based
        //  upon the 'print' method, we will check its output against the
        //  'print' method when the values of 'level' and 'spacesPerLevel'
        //  match those used to implement 'operator<<'.  We will also verify
        //  that 'operator<<' returns the specified stream.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const bdlb::VariantImp&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing OUTPUT (<<) OPERATOR AND 'print'" <<endl
                          << "========================================" <<endl;

        const char *F0 = "[ 123 ]";
        const char *F1 = "  [\n    123\n  ]";

        const char *G0 = "[ 456 ]";
        const char *G1 = "  [\n    456\n  ]";

        const char *K0 = "[ \"StringK\" ]";
        const char *K1 = "  [\n    \"StringK\"\n  ]";

        const char *L0 = "[ \"StringL\" ]";
        const char *L1 = "  [\n    \"StringL\"\n  ]";

        static const struct {
            int         d_lineNum;
            const char *d_spec;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_result;
        } DATA[] = {
          // LINE SPEC      LEVEL SPACE   RESULT
          // ---- ----      ----- -----   ------
          {  L_,  "",        0,    -1,    ""            },
          {  L_,  "",        1,     2,    ""            },
          {  L_,  "",       -1,    -2,    ""            },
          {  L_,  "A",       0,    -1,    "123"         },
          {  L_,  "A",       1,     2,    "  123\n"     },
          {  L_,  "A",      -1,    -2,    "123"         },
          {  L_,  "B",       0,    -1,    "456"         },
          {  L_,  "B",       1,     2,    "  456\n"     },
          {  L_,  "B",      -1,    -2,    "456"         },
          {  L_,  "S",       0,    -1,    "StringS"     },
          {  L_,  "S",       1,     2,    "  StringS\n" },
          {  L_,  "S",      -1,    -2,    "StringS"     },
          {  L_,  "U",       0,    -1,    "StringU"     },
          {  L_,  "U",       1,     2,    "  StringU\n" },
          {  L_,  "U",      -1,    -2,    "StringU"     },
          {  L_,  "F",       0,    -1,    F0            },
          {  L_,  "F",       1,     2,    F1            },
          {  L_,  "F",      -1,    -2,    F0            },
          {  L_,  "G",       0,    -1,    G0            },
          {  L_,  "G",       1,     2,    G1            },
          {  L_,  "G",      -1,    -2,    G0            },
          {  L_,  "K",       0,    -1,    K0            },
          {  L_,  "K",       1,     2,    K1            },
          {  L_,  "K",      -1,    -2,    K0            },
          {  L_,  "L",       0,    -1,    L0            },
          {  L_,  "L",       1,     2,    L1            },
          {  L_,  "L",      -1,    -2,    L0            },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_lineNum;
            const char        *SPEC   = DATA[ti].d_spec;
            const int          LEVEL  = DATA[ti].d_level;
            const int          SPACES = DATA[ti].d_spacesPerLevel;
            const bsl::string  RESULT = DATA[ti].d_result;

            if (veryVerbose) {
                T_ P_(LINE) P_(SPEC) P_(LEVEL) P_(SPACES) P(RESULT)
            }

            bsl::ostringstream printStream;
            bsl::ostringstream operatorStream;
            Obj mX(&testAllocator); const Obj& X = mX;

            gg(&mX, SPEC);

            LOOP_ASSERT(ti, &printStream == &X.print(printStream, LEVEL,
                                                                      SPACES));
            LOOP_ASSERT(ti, &operatorStream == &(operatorStream << X));

            if (veryVerbose) {
                T_ T_ P(printStream.str())
                T_ T_ P(operatorStream.str())
            }

            bsl::string cmp = printStream.str();
            LOOP3_ASSERT(LINE, cmp.size(), RESULT.size(),
                                                  cmp.size() == RESULT.size());
            for (int i = 0; i < static_cast<int>(cmp.size()); ++i) {
                LOOP4_ASSERT(LINE, i, cmp[i], RESULT[i], cmp[i] == RESULT[i]);
            }
            if (0 == LEVEL && -1 == SPACES) {
                bsl::string cmp2 = operatorStream.str();
                LOOP3_ASSERT(LINE, cmp.size(), cmp2.size(),
                                                    cmp.size() == cmp2.size());
                for (int i = 0; i < static_cast<int>(cmp.size()); ++i) {
                    LOOP4_ASSERT(LINE, i, cmp[i], cmp2[i], cmp[i] == cmp2[i]);
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Concerns:
        //   1. 'is<TYPE>()' reflects correct type information after both
        //      default construction and assignment.
        //   2. 'the<TYPE>()' returns the proper value after both default
        //      construction and assignment.
        //
        // Plan:
        //   Using the table-driven technique, create a table of test vectors
        //   that address both concerns.  Then pass the spec from the table to
        //   the generator function and verify against the expected result
        //   using the basic accessors.
        //
        // Testing:
        //   is<Type>() const;
        //   the<Type>() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        static struct {
            int         d_lineNum;
            const char *d_input;        // input specifications
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE INPUT               TYPE_IDX VALUE_IDX
          // ---- -----        --------------- ---------
            { L_, "",                    UNSET,       0 }
          , { L_, "A",                INT_TYPE,       0 }
          , { L_, "B",                INT_TYPE,       1 }
          , { L_, "C",                INT_TYPE,       2 }
          , { L_, "D",                INT_TYPE,       3 }
          , { L_, "E",                INT_TYPE,       4 }
          , { L_, "BA",               INT_TYPE,       0 }
          , { L_, "FA",               INT_TYPE,       0 }
          , { L_, "KA",               INT_TYPE,       0 }
          , { L_, "SA",               INT_TYPE,       0 }
          , { L_, "F",           TEST_INT_TYPE,       0 }
          , { L_, "G",           TEST_INT_TYPE,       1 }
          , { L_, "H",           TEST_INT_TYPE,       2 }
          , { L_, "I",           TEST_INT_TYPE,       3 }
          , { L_, "J",           TEST_INT_TYPE,       4 }
          , { L_, "AF",          TEST_INT_TYPE,       0 }
          , { L_, "GF",          TEST_INT_TYPE,       0 }
          , { L_, "KF",          TEST_INT_TYPE,       0 }
          , { L_, "SF",          TEST_INT_TYPE,       0 }
          , { L_, "S",             STRING_TYPE,       0 }
          , { L_, "T",             STRING_TYPE,       1 }
          , { L_, "U",             STRING_TYPE,       2 }
          , { L_, "V",             STRING_TYPE,       3 }
          , { L_, "W",             STRING_TYPE,       4 }
          , { L_, "AS",            STRING_TYPE,       0 }
          , { L_, "FS",            STRING_TYPE,       0 }
          , { L_, "KS",            STRING_TYPE,       0 }
          , { L_, "TS",            STRING_TYPE,       0 }
          , { L_, "K",        TEST_STRING_TYPE,       0 }
          , { L_, "L",        TEST_STRING_TYPE,       1 }
          , { L_, "M",        TEST_STRING_TYPE,       2 }
          , { L_, "N",        TEST_STRING_TYPE,       3 }
          , { L_, "O",        TEST_STRING_TYPE,       4 }
          , { L_, "AK",       TEST_STRING_TYPE,       0 }
          , { L_, "FK",       TEST_STRING_TYPE,       0 }
          , { L_, "LK",       TEST_STRING_TYPE,       0 }
          , { L_, "SK",       TEST_STRING_TYPE,       0 }
          , { L_, "Z",          TEST_VOID_TYPE,       0 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_lineNum;
            const char *INPUT     = DATA[ti].d_input;
            const int   TYPE_IDX  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE); P_(INPUT); P_(TYPE_IDX); P(VALUE_IDX);
            }

            Obj mX; const Obj& X = mX;
            gg(&mX, INPUT);

            LOOP3_ASSERT(LINE, TYPE_IDX, X.typeIndex(),
                         TYPE_IDX == X.typeIndex());

            switch (X.typeIndex()) {
              case UNSET: {
              } break;
              case TEST_VOID_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<int>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestInt>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<bsl::string>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestString>());
                LOOP2_ASSERT(LINE, VALUE_IDX,  X.is<TestVoid>());
                LOOP2_ASSERT(LINE, VALUE_IDX, TestVoid() == X.the<TestVoid>());
              } break;
              case INT_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX,  X.is<int>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestInt>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<bsl::string>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestString>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestVoid>());
                LOOP3_ASSERT(LINE, VALUE_IDX,  X.the<int>(),
                             INT_DATA[VALUE_IDX] == X.the<int>());
              } break;
              case TEST_INT_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<int>());
                LOOP2_ASSERT(LINE, VALUE_IDX,  X.is<TestInt>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<bsl::string>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestString>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestVoid>());
                LOOP3_ASSERT(LINE, VALUE_IDX,  X.the<TestInt>(),
                             TEST_INT_DATA[VALUE_IDX] == X.the<TestInt>());
              } break;
              case STRING_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<int>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestInt>());
                LOOP2_ASSERT(LINE, VALUE_IDX,  X.is<bsl::string>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestString>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestVoid>());
                LOOP3_ASSERT(LINE, VALUE_IDX,  X.the<bsl::string>(),
                             STRING_DATA[VALUE_IDX] == X.the<bsl::string>());
              } break;
              case TEST_STRING_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<int>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestInt>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<bsl::string>());
                LOOP2_ASSERT(LINE, VALUE_IDX,  X.is<TestString>());
                LOOP2_ASSERT(LINE, VALUE_IDX, !X.is<TestVoid>());
                LOOP3_ASSERT(LINE, VALUE_IDX,  X.the<TestString>(),
                           TEST_STRING_DATA[VALUE_IDX] == X.the<TestString>());
              } break;
              default: LOOP_ASSERT(LINE, 0);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS
        //
        // Developing a generator also means developing a test case to verify
        // it.  We will need to select an appropriate suite of inputs specs
        // and, using basic accessors, verify that valid syntax drives the
        // object to its desired state.
        //
        // We will also want to verify that simple typos are detected and
        // reported reliably.  In order to test that aspect, we will need to
        // supply invalid syntax.  But supplying invalid syntax to 'gg' will,
        // by design, result in a diagnostic message and a test failure.  To
        // address this testing issue, notice that the implementation of the
        // primitive generator 'gg' has been broken into two functions 'ggg'
        // and 'gg' with the latter implemented trivially in terms of the
        // former.  The primitive generator *implementation* 'ggg' exposes one
        // additional parameter that can be used to suppress output during what
        // is sometimes called "negative testing" -- i.e., making sure that the
        // function "breaks" when it should.  Instead of returning a reference,
        // the 'ggg' function returns the index of the character that caused
        // the error and a negative value on success.  This extra information
        // (useful only when testing the generator itself) ensures that
        // spec-parsing failed at the expected location and not accidentally
        // for some other reason.
        //
        // Concerns:
        //  1. Parsing stops at the first incorrect character of the spec.
        //  2. The resulting object has the correct type and value.
        //  3. All examples in the documentation are parsed as expected.
        //
        // Plan:
        //  Using the table-driven technique, create a table of test vectors
        //  containing the line number, spec, expected return code, expected
        //  type index, and value index.  For concern 1, make sure the return
        //  code is the same as the specified offset where the error occurred.
        //  For concerns 2 and 3, verify that the type index and value index
        //  are as expected.
        //
        // Testing:
        //   int ggg(bdlb::Variant *, const char *, bool = true);
        //   bdlb::Variant *gg(bdlb::Variant *, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMITIVE GENERATOR FUNCTION" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;

        static struct {
            int         d_lineNum;
            const char *d_input;        // input specifications
            int         d_retCode;      // return code of ggg()
            int         d_expTypeIdx;   // expected type index
            int         d_expValueIdx;  // expected value index (within type)
        } DATA[] = {
          // LINE INPUT        RC         TYPE_IDX VALUE_IDX
          // ---- -----        --         -------- ---------

          // concern 1
            { L_, "X",         0,            UNSET,       0 }
          , { L_, "WHAT?!#%#", 4,      STRING_TYPE,       1 }

          // concern 2
          , { L_, "",         -1,            UNSET,       0 }
          , { L_, "A",        -1,         INT_TYPE,       0 }
          , { L_, "B",        -1,         INT_TYPE,       1 }
          , { L_, "C",        -1,         INT_TYPE,       2 }
          , { L_, "D",        -1,         INT_TYPE,       3 }
          , { L_, "E",        -1,         INT_TYPE,       4 }
          , { L_, "BA",       -1,         INT_TYPE,       0 }
          , { L_, "FA",       -1,         INT_TYPE,       0 }
          , { L_, "KA",       -1,         INT_TYPE,       0 }
          , { L_, "SA",       -1,         INT_TYPE,       0 }
          , { L_, "F",        -1,    TEST_INT_TYPE,       0 }
          , { L_, "G",        -1,    TEST_INT_TYPE,       1 }
          , { L_, "H",        -1,    TEST_INT_TYPE,       2 }
          , { L_, "I",        -1,    TEST_INT_TYPE,       3 }
          , { L_, "J",        -1,    TEST_INT_TYPE,       4 }
          , { L_, "AF",       -1,    TEST_INT_TYPE,       0 }
          , { L_, "GF",       -1,    TEST_INT_TYPE,       0 }
          , { L_, "KF",       -1,    TEST_INT_TYPE,       0 }
          , { L_, "SF",       -1,    TEST_INT_TYPE,       0 }
          , { L_, "S",        -1,      STRING_TYPE,       0 }
          , { L_, "T",        -1,      STRING_TYPE,       1 }
          , { L_, "U",        -1,      STRING_TYPE,       2 }
          , { L_, "V",        -1,      STRING_TYPE,       3 }
          , { L_, "W",        -1,      STRING_TYPE,       4 }
          , { L_, "AS",       -1,      STRING_TYPE,       0 }
          , { L_, "FS",       -1,      STRING_TYPE,       0 }
          , { L_, "KS",       -1,      STRING_TYPE,       0 }
          , { L_, "TS",       -1,      STRING_TYPE,       0 }
          , { L_, "K",        -1, TEST_STRING_TYPE,       0 }
          , { L_, "L",        -1, TEST_STRING_TYPE,       1 }
          , { L_, "M",        -1, TEST_STRING_TYPE,       2 }
          , { L_, "N",        -1, TEST_STRING_TYPE,       3 }
          , { L_, "O",        -1, TEST_STRING_TYPE,       4 }
          , { L_, "AK",       -1, TEST_STRING_TYPE,       0 }
          , { L_, "FK",       -1, TEST_STRING_TYPE,       0 }
          , { L_, "LK",       -1, TEST_STRING_TYPE,       0 }
          , { L_, "SK",       -1, TEST_STRING_TYPE,       0 }
          , { L_, "Z",        -1,   TEST_VOID_TYPE,       0 }

          // concern 3
          , { L_, "",         -1,            UNSET,       0 }
          , { L_, "A",        -1,         INT_TYPE,       0 }
          , { L_, "AA",       -1,         INT_TYPE,       0 }
          , { L_, "ABC",      -1,         INT_TYPE,       2 }
          , { L_, "ABC~",     -1,            UNSET,       0 }
          , { L_, "ABC~DE",   -1,         INT_TYPE,       4 }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_lineNum;
            const char *INPUT     = DATA[ti].d_input;
            const int   RC        = DATA[ti].d_retCode;
            const int   TYPE_IDX  = DATA[ti].d_expTypeIdx;
            const int   VALUE_IDX = DATA[ti].d_expValueIdx;

            if (veryVerbose) {
                T_ P_(LINE); P_(INPUT); P_(RC); P_(TYPE_IDX); P(VALUE_IDX);
            }

            Obj mX; const Obj& X = mX;

            int retCode = ggg(&mX, INPUT, false);

            LOOP3_ASSERT(LINE, RC, retCode, RC == retCode);
            LOOP3_ASSERT(LINE, TYPE_IDX, X.typeIndex(),
                         TYPE_IDX == X.typeIndex());
            switch (X.typeIndex()) {
              case UNSET: {
              } break;
              case TEST_VOID_TYPE: {
                LOOP2_ASSERT(LINE, VALUE_IDX, TestVoid() == X.the<TestVoid>());
              } break;
              case INT_TYPE: {
                LOOP3_ASSERT(LINE, VALUE_IDX, X.the<int>(),
                                          INT_DATA[VALUE_IDX] == X.the<int>());
              } break;
              case TEST_INT_TYPE: {
                LOOP3_ASSERT(LINE, VALUE_IDX, X.the<TestInt>(),
                                 TEST_INT_DATA[VALUE_IDX] == X.the<TestInt>());
              } break;
              case STRING_TYPE: {
                LOOP3_ASSERT(LINE, VALUE_IDX, X.the<bsl::string>(),
                               STRING_DATA[VALUE_IDX] == X.the<bsl::string>());
              } break;
              case TEST_STRING_TYPE: {
                LOOP3_ASSERT(LINE, VALUE_IDX, X.the<TestString>(),
                           TEST_STRING_DATA[VALUE_IDX] == X.the<TestString>());
              } break;
              default: LOOP_ASSERT(LINE, 0);
            }

            // Testing the "reset" facility.

            if (veryVerbose)
                cout << "\t\twith appending '~' (reset).'" << endl;

            if (-1 == retCode) {
                bsl::string strInput(INPUT);
                strInput.push_back('~');

                Obj mY; const Obj& Y = mY;

                int ret = ggg(&mY, strInput.c_str(), false);
                LOOP_ASSERT(LINE, -1 == ret);
                LOOP_ASSERT(LINE,  0 == Y.typeIndex());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //  We want to exercise the set of primary manipulators that can put
        //  the object into any state.
        //
        // Concerns:
        //  1. That the default constructor:
        //     a. creates an object with the expected value.
        //     b. does not allocate and does not throw.
        //     c. properly wires the optionally-specified allocator.
        //
        //  2. That 'assign<TYPE>'
        //     a. properly assigns a value of the variant 'TYPE'.
        //     b. properly destroys the current value of the current type.
        //     c. uses assignment (not dtor+ctor) if assigning the same type.
        //     d. is exception-neutral with no guarantee of rollback.
        //
        //  3. That 'reset':
        //      a. Produces the expected state (unset).
        //      b. Properly destroys the current value of the current type.
        //      c. Leaves the object in a consistent state.
        //
        // Plan:
        //  To address concerns for 1, create an object using the default
        //  constructor:
        //    - Without passing an allocator, in which case the object will
        //      allocate memory using the default allocator (if default
        //      construction of the default type uses an allocator).
        //    - With an allocator, in which case the object will allocate
        //      memory using the specified allocator (if default construction
        //      of the default type uses an allocator).
        //    - Repeat the above two points using a first type that allocates
        //      memory.
        //    - In the presence of exceptions during memory allocations using
        //      a 'bslma::TestAllocator' and varying its allocation limit.
        //  Use 'typeIndex' to verify that the newly-created variant has the
        //  proper type.  Also use 'numBlocksTotal' of the default allocator
        //  and the specified allocator to verify that the constructor
        //  allocates memory from the correct source.
        //
        //  To address concerns for 2, create an object, use 'assign' to change
        //  the value, including a value of the same type and morphing
        //  between different types, and check if the variant type and value
        //  have been properly changed with 'is<TYPE>' and 'the<TYPE>'.  Use
        //  the exception safety test macros to verify that when a memory
        //  exception is thrown, the variant object is left in a consistent
        //  state.  Finally, use the allocator to make sure that when assigning
        //  to a variant object that owns memory, the object is properly
        //  destroyed and its memory reclaimed by the allocator.
        //
        //  To address concerns for 3, create an object, exercise 'assign', at
        //  will, then call 'reset'; check the value and then exercise
        //  'assign' again to check consistency.
        //
        //  Testing:
        //    bdlb::Variant();
        //    bdlb::Variant(const TYPEORALLOCATOR& typeOrAlloc);
        //    ~bdlb::Variant();
        //    void assign<TYPE>(const TYPE& value);
        //    void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        // Default construction does not allocate memory.

        if (verbose) cout << "\nTesting default constructor." << endl;

        {
            if (verbose) cout << "\twith default allocator" << endl;

            // Temporarily install a test allocator as the default allocator in
            // order to verify the default allocator is used when no allocator
            // is specified.

            bslma::TestAllocator da; // default allocator
            bslma::DefaultAllocatorGuard DAG(&da);
            int previousTotal = static_cast<int>(da.numBlocksTotal());
            const Obj X, Y((bslma::Allocator *)0);
            LOOP_ASSERT(X.typeIndex(), 0 == X.typeIndex());
            LOOP_ASSERT(X.typeIndex(), 0 == Y.typeIndex());
            LOOP_ASSERT(da.numBlocksTotal(),
                        da.numBlocksTotal() == previousTotal);
        }

        {
            if (verbose) cout << "\twith a specified allocator" << endl;
            int previousTotal =
                              static_cast<int>(testAllocator.numBlocksTotal());
            const Obj X(&testAllocator);
            LOOP_ASSERT(X.typeIndex(), 0 == X.typeIndex());
            LOOP_ASSERT(testAllocator.numBlocksTotal(),
                        testAllocator.numBlocksTotal() == previousTotal);
        }

        {
            if (verbose) cout << "\twith a specified allocator and exceptions"
                              << endl;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const int previousTotal =
                              static_cast<int>(testAllocator.numBlocksTotal());
              const Obj X(&testAllocator);
              ASSERT(0 == X.typeIndex());
              ASSERT(testAllocator.numBlocksTotal() == previousTotal);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) cout << "\nTesting 'assign'." << endl;

        if (verbose) cout << "\twithout allocators" << endl;

        {
            Obj mX;   const Obj& X = mX;

            if (veryVerbose) {
                cout << "\t\tassigning from " << X << " to " << VA << endl;
            }

            mX.assign<int>(VA);
            ASSERT(1  == X.is<int>());
            ASSERT(VA == X.the<int>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VB << " (same type)" << endl;
            }

            mX.assign<int>(VB);
            ASSERT(1  == X.is<int>());
            ASSERT(VB == X.the<int>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VF << " (different type)" << endl;
            }

            mX.assign<TestInt>(VF);
            ASSERT(1  == X.is<TestInt>());
            ASSERT(VF == X.the<TestInt>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VK << " (different type)" << endl;
            }

            mX.assign<TestString>(VK);
            ASSERT(1  == X.is<TestString>());
            ASSERT(VK == X.the<TestString>());

            mX.assign<bsl::string>(VS);
            ASSERT(1  == X.is<bsl::string>());
            ASSERT(VS == X.the<bsl::string>());
        }

        {
            Obj mX;   const Obj& X = mX;

            if (veryVerbose) {
                cout << "\t\tassigning from " << X << " to " << VS << endl;
            }

            mX.assign<bsl::string>(VS);
            ASSERT(1  == X.is<bsl::string>());
            ASSERT(VS == X.the<bsl::string>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VT << " (same type)" << endl;
            }

            mX.assign<bsl::string>(VT);
            ASSERT(1  == X.is<bsl::string>());
            ASSERT(VT == X.the<bsl::string>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VK << " (different type)" << endl;
            }

            mX.assign<TestString>(VK);
            ASSERT(1  == X.is<TestString>());
            ASSERT(VK == X.the<TestString>());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VF << " (different type)" << endl;
            }

            mX.assign<TestInt>(VF);
            ASSERT(1  == X.is<TestInt>());
            ASSERT(VF == X.the<TestInt>());

            mX.assign<int>(VA);
            ASSERT(1  == X.is<int>());
            ASSERT(VA == X.the<int>());
        }

        if (verbose) cout << "\twith specified allocator" << endl;

        {
            const int TOTAL = static_cast<int>(testAllocator.numBlocksTotal());

            Obj mX(&testAllocator);  const Obj& X = mX;

            if (veryVerbose) {
                cout << "\t\tassigning from " << X << " to " << VA << endl;
            }

            mX.assign<int>(VA);
            ASSERT(1     == X.is<int>());
            ASSERT(VA    == X.the<int>());
            ASSERT(0     == testAllocator.numBlocksInUse());
            ASSERT(TOTAL == testAllocator.numBlocksTotal());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VB << " (same type)" << endl;
            }

            mX.assign<int>(VB);
            ASSERT(1     == X.is<int>());
            ASSERT(VB    == X.the<int>());
            ASSERT(0     == testAllocator.numBlocksInUse());
            ASSERT(TOTAL == testAllocator.numBlocksTotal());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VF << " (different type)" << endl;
            }

            mX.assign<TestInt>(VF);
            ASSERT(1     == X.is<TestInt>());
            ASSERT(VF    == X.the<TestInt>());
            ASSERT(0     == testAllocator.numBlocksInUse());
            ASSERT(TOTAL == testAllocator.numBlocksTotal());

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VK << " (different type)" << endl;
            }

            mX.assign<TestString>(VK);
            ASSERT(1     == X.is<TestString>());
            ASSERT(VK    == X.the<TestString>());

            mX.assign<int>(VA);
            ASSERT(1      == X.is<int>());
            ASSERT(VA     == X.the<int>());
            ASSERT(0      == testAllocator.numBlocksInUse());
        }

        {
            const int TOTAL = static_cast<int>(testAllocator.numBlocksTotal());
            Obj mX(&testAllocator);  const Obj& X = mX;

            ASSERT(0     == testAllocator.numBlocksInUse());
            ASSERT(TOTAL == testAllocator.numBlocksTotal());

            if (veryVerbose) {
                cout << "\t\tassigning from " << X << " to " << VS << endl;
            }

            mX.assign<bsl::string>(VS);
            ASSERT(1  == X.is<bsl::string>());
            ASSERT(VS == X.the<bsl::string>());

            {
                const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

                if (veryVerbose) {
                    cout << "\t\t\tthen to " << VT << " (same type)" << endl;
                }

                mX.assign<bsl::string>(VT);
                ASSERT(1  == X.is<bsl::string>());
                ASSERT(VT == X.the<bsl::string>());
                ASSERT(PREVIOUS == testAllocator.numBlocksTotal());
            }

            if (veryVerbose) {
                cout << "\t\t\tthen to " << VK << " (different type)" << endl;
            }

            mX.assign<TestString>(VK);
            ASSERT(1  == X.is<TestString>());
            ASSERT(VK == X.the<TestString>());

            {
                const int PREVIOUS =
                              static_cast<int>(testAllocator.numBlocksTotal());

                if (veryVerbose) {
                    cout << "\t\t\tthen to " << VF << " (different type)"
                         << endl;
                }

                mX.assign<TestInt>(VF);
                ASSERT(1  == X.is<TestInt>());
                ASSERT(VF == X.the<TestInt>());
                ASSERT(0        == testAllocator.numBlocksInUse());
                ASSERT(PREVIOUS == testAllocator.numBlocksTotal());
            }
        }

        if (verbose) cout << "\tTesting 'reset'." << endl;
        {
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT( X.isUnset());

            mX.reset();
            ASSERT( X.isUnset());

            mX.assign<int>(VA);
            ASSERT(!X.isUnset());
            ASSERT( X.is<int>());

            mX.reset();
            ASSERT( X.isUnset());

            mX.assign<TestInt>(VF);
            ASSERT(!X.isUnset());
            ASSERT( X.is<TestInt>());

            mX.reset();
            ASSERT( X.isUnset());

            mX.assign<TestString>(VK);
            ASSERT(!X.isUnset());
            ASSERT( X.is<TestString>());

            mX.reset();
            ASSERT( X.isUnset());

            mX.assign<int>(VA);
            ASSERT(!X.isUnset());
            ASSERT( X.is<int>());

            mX.reset();
            ASSERT( X.isUnset());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - primary manipulators: 'assign'
        //      - basic accessors: 'is<TYPE>' and 'the<TYPE>' methods
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary (black box) manipulator [3, 5], copy
        //   constructor [2, 8], and assignment operator [9, 10] in situations
        //   where the internal data (i) does *not* and (ii) *does* have to
        //   morph.  Try aliasing with assignment for a non-null object [11]
        //   and allow the result to leave scope, enabling the destructor to
        //   assert internal object invariants.  Display object values
        //   frequently in verbose mode:
        //    1. Create an object x1 (default ctor).       x1:
        //    2. Create a second object x2 (copy from x1). x1:  x2:
        //    3. Append an element value A to x1).         x1:A x2:
        //    4. Append the same element value A to x2).   x1:A x2:A
        //    5. Append another element value B to x2).    x1:A x2:B
        //    6. Create a third object x3 (default ctor).  x1:A x2:B x3:
        //    7. Create a fourth object x4 (copy of x2).   x1:A x2:B x3:  x4:B
        //    8. Assign x2 = x1 (non-null becomes null).   x1:A x2:  x3:  x4:B
        //    9. Assign x3 = x4 (null becomes non-null).   x1:A x2:  x3:B x4:B
        //   10. Assign x4 = x4 (aliasing).                x1:A x2:  x3:B x4:B
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        typedef bdlb::Variant<int, const char *, TestVoid> Obj;

        int         VA = 123;
        const char *VB = "This is a null-terminated byte string.";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor).       "
                             "          { x1: }" << endl;

        Obj mX1; const Obj& X1 = mX1;
        if (verbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(0 == X1.typeIndex());
        ASSERT(0 == X1.is<int>());
        ASSERT(0 == X1.is<const char *>());
        ASSERT(0 == X1.is<TestVoid>());

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1."
                          << endl;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create a second object x2 (copy from x1). "
                             "          { x1: x2: }" << endl;
        Obj mX2(X1); const Obj& X2 = mX2;
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.typeIndex());
        ASSERT(0 == X2.is<int>());
        ASSERT(0 == X2.is<const char *>());
        ASSERT(0 == X2.is<TestVoid>());

        if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                          << endl;
        ASSERT(1 == (X2 == X1));          ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Assign an element value A to x1).         "
                             "          { x1:A x2: }" << endl;
        mX1.assign<int>(VA);
        if (verbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT( 1 == X1.typeIndex());
        ASSERT( 1 == X1.is<int>());
        ASSERT( 0 == X1.is<const char *>());
        ASSERT( 0 == X1.is<TestVoid>());
        ASSERT(VA == X1.the<int>());

        if (verbose) cout << "\tb. Try equality operators: x1 <op> x1, x2."
                          << endl;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));          ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Assign the same element value A to x2).   "
                             "          { x1:A x2:A }" << endl;
        mX2.assign<int>(VA);
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT( 1 == X2.typeIndex());
        ASSERT( 1 == X2.is<int>());
        ASSERT( 0 == X2.is<const char *>());
        ASSERT( 0 == X2.is<TestVoid>());
        ASSERT(VA == X2.the<int>());

        if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                          << endl;
        ASSERT(1 == (X2 == X1));          ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Assign another element value B to x2).    "
                             "          { x1:A x2:B }" << endl;

        mX2.assign<const char *>(VB);
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT( 2 == X2.typeIndex());
        ASSERT( 0 == X2.is<int>());
        ASSERT( 1 == X2.is<const char *>());
        ASSERT( 0 == X2.is<TestVoid>());
        ASSERT(VB == X2.the<const char *>());

        if (verbose) cout << "\tb. Try equality operators: x2 <op> x1, x2."
                          << endl;
        ASSERT(0 == (X2 == X1));          ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Create a third object x3 (default ctor).  "
                             "          { x1:A x2:B x3: }" << endl;

        Obj mX3; const Obj& X3 = mX3;
        if (verbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(0 == X3.typeIndex());
        ASSERT(0 == X3.is<int>());
        ASSERT(0 == X3.is<const char *>());
        ASSERT(0 == X3.is<TestVoid>());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));          ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));          ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));          ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create a fourth object x4 (copy of x2).   "
                             "          { x1:A x2:B x3: x4:B }" << endl;

        Obj mX4(X2); const Obj& X4 = mX4;
        if (verbose) { T_ P(X4) }

        if (verbose) cout << "\ta. Check new state of x4." << endl;
        ASSERT( 2 == X4.typeIndex());
        ASSERT( 0 == X4.is<int>());
        ASSERT( 1 == X4.is<const char *>());
        ASSERT( 0 == X4.is<TestVoid>());
        ASSERT(VB == X4.the<const char *>());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));          ASSERT(1 == (X4 != X1));
        ASSERT(1 == (X4 == X2));          ASSERT(0 == (X4 != X2));
        ASSERT(0 == (X4 == X3));          ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));          ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3 (non-null becomes null). "
                             "          { x1:A x2: x3: x4:B }" << endl;
        mX2 = X3;
        if (verbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.typeIndex());
        ASSERT(0 == X2.is<int>());
        ASSERT(0 == X2.is<const char *>());
        ASSERT(0 == X2.is<TestVoid>());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));          ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));          ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));          ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n9. Assign x3 = x4 (null becomes non-null). "
                             "          { x1:A x2: x3:B x4:B }" << endl;
        mX3 = X4;
        if (verbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT( 2 == X3.typeIndex());
        ASSERT( 0 == X3.is<int>());
        ASSERT( 1 == X3.is<const char *>());
        ASSERT( 0 == X3.is<TestVoid>());
        ASSERT(VB == X3.the<const char *>());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));          ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));          ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));          ASSERT(0 == (X3 != X3));
        ASSERT(1 == (X3 == X4));          ASSERT(0 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x4 = x4 (aliasing).                "
                             "          { x1: x2: x3:B x4:B }" << endl;
        mX4 = X4;
        if (verbose) { T_ P(X4) }

        if (verbose) cout << "\ta. Check new state of x4." << endl;
        ASSERT( 2 == X4.typeIndex());
        ASSERT( 0 == X4.is<int>());
        ASSERT( 1 == X4.is<const char *>());
        ASSERT( 0 == X4.is<TestVoid>());
        ASSERT(VB == X4.the<const char *>());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));          ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));          ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));          ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));          ASSERT(0 == (X4 != X4));

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


// TBD: The following list of explicit instantiations was provided by IBM as a
// work-around for an xlC compiler crash (unless case 18 is commented out).

#if BSLS_PLATFORM_CMP_IBM

#include <bslalg_hastrait.h>
#include <bslalg_typetraitbitwisecopyable.h>
#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslalg_typetraits.h>

template class BloombergLP::bdlb::Variant<int,double,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bslalg::HasTrait<UsesAllocator<1>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<UsesAllocator<10>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<UsesAllocator<20>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BitwiseCopyable<1>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BitwiseCopyable<10>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BitwiseCopyable<20>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BitwiseMoveable<1>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BitwiseMoveable<10>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BitwiseMoveable<20>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<UsesAllocator<1>,UsesAllocator<2>,UsesAllocator<3>,UsesAllocator<4>,UsesAllocator<5>,UsesAllocator<6>,UsesAllocator<7>,UsesAllocator<8>,UsesAllocator<9>,UsesAllocator<10>,UsesAllocator<11>,UsesAllocator<12>,UsesAllocator<13>,UsesAllocator<14>,UsesAllocator<15>,UsesAllocator<16>,UsesAllocator<17>,UsesAllocator<18>,UsesAllocator<19>,UsesAllocator<20> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<UsesAllocator<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,UsesAllocator<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,UsesAllocator<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,UsesAllocator<18>,NilTraits<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,UsesAllocator<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,NilTraits<19>,UsesAllocator<20> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BitwiseCopyable<1>,BitwiseCopyable<2>,BitwiseCopyable<3>,BitwiseCopyable<4>,BitwiseCopyable<5>,BitwiseCopyable<6>,BitwiseCopyable<7>,BitwiseCopyable<8>,BitwiseCopyable<9>,BitwiseCopyable<10>,BitwiseCopyable<11>,BitwiseCopyable<12>,BitwiseCopyable<13>,BitwiseCopyable<14>,BitwiseCopyable<15>,BitwiseCopyable<16>,BitwiseCopyable<17>,BitwiseCopyable<18>,BitwiseCopyable<19>,BitwiseCopyable<20> >,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BitwiseCopyable<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,BitwiseCopyable<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,BitwiseCopyable<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,BitwiseCopyable<18>,NilTraits<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,BitwiseCopyable<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,NilTraits<19>,BitwiseCopyable<20> >,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BitwiseMoveable<1>,BitwiseMoveable<2>,BitwiseMoveable<3>,BitwiseMoveable<4>,BitwiseMoveable<5>,BitwiseMoveable<6>,BitwiseMoveable<7>,BitwiseMoveable<8>,BitwiseMoveable<9>,BitwiseMoveable<10>,BitwiseMoveable<11>,BitwiseMoveable<12>,BitwiseMoveable<13>,BitwiseMoveable<14>,BitwiseMoveable<15>,BitwiseMoveable<16>,BitwiseMoveable<17>,BitwiseMoveable<18>,BitwiseMoveable<19>,BitwiseMoveable<20> >,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BitwiseCopyable<1>,NilTraits<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,BitwiseCopyable<2>,NilTraits<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,BitwiseCopyable<3>,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,BitwiseCopyable<18>,NilTraits<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,BitwiseCopyable<19>,NilTraits<20> >,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<NilTraits<1>,NilTraits<2>,NilTraits<3>,NilTraits<4>,NilTraits<5>,NilTraits<6>,NilTraits<7>,NilTraits<8>,NilTraits<9>,NilTraits<10>,NilTraits<11>,NilTraits<12>,NilTraits<13>,NilTraits<14>,NilTraits<15>,NilTraits<16>,NilTraits<17>,NilTraits<18>,NilTraits<19>,BitwiseCopyable<20> >,BloombergLP::bslalg::TypeTraitBitwiseMoveable>;
template class BloombergLP::bdlb::Variant2<TestString,TestArg<2> >;
template class TestArg<20>;
template class TestArg<19>;
template class TestArg<18>;
template class TestArg<17>;
template class TestArg<16>;
template class TestArg<15>;
template class TestArg<14>;
template class TestArg<13>;
template class TestArg<12>;
template class TestArg<11>;
template class TestArg<10>;
template class TestArg<9>;
template class TestArg<8>;
template class TestArg<7>;
template class TestArg<6>;
template class TestArg<5>;
template class TestArg<4>;
template class TestArg<3>;
template class TestArg<2>;
template struct BloombergLP::bslmf::IsSame<TestArg<2>,TestArg<2> >;
template struct BloombergLP::bslmf::IsSame<BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant2<TestString,TestArg<2> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant2<TestString,TestArg<2> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant3<TestString,TestArg<2>,TestArg<3> >;
template struct BloombergLP::bslmf::IsSame<TestArg<3>,TestArg<3> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant3<TestString,TestArg<2>,TestArg<3> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant3<TestString,TestArg<2>,TestArg<3> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >;
template struct BloombergLP::bslmf::IsSame<TestArg<4>,TestArg<4> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >;
template struct BloombergLP::bslmf::IsSame<TestArg<5>,TestArg<5> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >;
template struct BloombergLP::bslmf::IsSame<TestArg<6>,TestArg<6> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >;
template struct BloombergLP::bslmf::IsSame<TestArg<7>,TestArg<7> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >;
template struct BloombergLP::bslmf::IsSame<TestArg<8>,TestArg<8> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >;
template struct BloombergLP::bslmf::IsSame<TestArg<9>,TestArg<9> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >;
template struct BloombergLP::bslmf::IsSame<TestArg<10>,TestArg<10> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >;
template struct BloombergLP::bslmf::IsSame<TestArg<11>,TestArg<11> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >;
template struct BloombergLP::bslmf::IsSame<TestArg<12>,TestArg<12> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >;
template struct BloombergLP::bslmf::IsSame<TestArg<13>,TestArg<13> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >;
template struct BloombergLP::bslmf::IsSame<TestArg<14>,TestArg<14> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >;
template struct BloombergLP::bslmf::IsSame<TestArg<15>,TestArg<15> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >;
template struct BloombergLP::bslmf::IsSame<TestArg<16>,TestArg<16> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >;
template struct BloombergLP::bslmf::IsSame<TestArg<17>,TestArg<17> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >;
template struct BloombergLP::bslmf::IsSame<TestArg<18>,TestArg<18> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >;
template struct BloombergLP::bslmf::IsSame<TestArg<19>,TestArg<19> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<13> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<13>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<13>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<13> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList14<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14> >,TestArg<14> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<14>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<14>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<13> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<14> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList15<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15> >,TestArg<15> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<15>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<15>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<13> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<14> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<15> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList16<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16> >,TestArg<16> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<16>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<16>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<8> >;
template struct BloombergLP::bslmf::IsSame<bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,const char *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<13> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<14> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<15> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<16> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList17<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17> >,TestArg<17> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<17>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<17>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<12> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<13> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<14> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<15> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<16> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<17> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList18<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18> >,TestArg<18> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<18>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestString>;
template struct BloombergLP::bslmf::IsSame<TestArg<18>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<12> >;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<13> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<14> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<15> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<16> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<17> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<18> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,TestArg<19> >;  // 59672
template struct BloombergLP::bslalg::HasTrait<TestArg<19>,BloombergLP::bslalg::TypeTraitPair>;  // 41035
template struct BloombergLP::bslmf::IsSame<TestArg<19>,BloombergLP::bslmf::Nil>;  // 58554
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestString>;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<2> >;  // 59672
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant19<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant<BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::Variant<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >;
template struct BloombergLP::bslmf::IsSame<TestArg<20>,TestArg<20> >;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>;
template struct BloombergLP::bslalg::HasTrait<BloombergLP::bdlb::Variant<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,BloombergLP::bdlb::TypeTraitHasPrintMethod>;
template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<int,TestInt,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestString,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<BloombergLP::bslmf::Nil,int,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<int,char,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template class my_VariantWrapper<BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<int,TestInt,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestString,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> > >;
template struct BloombergLP::bdlb::Variant_ReturnValueHelper<my_ReturningVisitor>;
template struct bsl::enable_if<1,int>;
template struct bsl::enable_if<0,void>;
template struct BloombergLP::bdlb::Variant_ReturnValueHelper<my_ConstReturningVisitor>;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template class BloombergLP::bdlb::Variant<TestAllocObj,int,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestInt,TestString,TestVoid,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template class BloombergLP::bdlb::Variant<bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestAllocObj,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template class my_VariantWrapper<BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<int,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestInt,TestString,TestVoid,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> > >;
template class BloombergLP::bdlb::Variant<int,const char *,TestVoid,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>;
template class bsl::basic_ostringstream<char,bsl::char_traits<char>,bsl::allocator<char> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<int,double,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> > >,int>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<int,double,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> > >,double>;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;  // 80650
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<3> >;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<4> >;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<5> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<6> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<7> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<8> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<9> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<10> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<11> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<12> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<13> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<14> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<15> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<16> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<17> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<18> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<19> >;  // 59672
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList20<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13>,TestArg<14>,TestArg<15>,TestArg<16>,TestArg<17>,TestArg<18>,TestArg<19>,TestArg<20> >,TestArg<20> >;  // 59672
template struct BloombergLP::bslalg::HasTrait<TestArg<20>,BloombergLP::bslalg::TypeTraitPair>;  // 41035
template struct BloombergLP::bslmf::IsSame<TestArg<20>,BloombergLP::bslmf::Nil>;  // 58554
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<int,char,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,int>;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<int,char,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,char>;  // 59187
template struct BloombergLP::bslalg::HasTrait<char,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;  // 41032
template struct BloombergLP::bslalg::HasTrait<char,BloombergLP::bslalg::TypeTraitPair>;  // 41035
template struct BloombergLP::bslmf::IsSame<char,BloombergLP::bslmf::Nil>;  // 58554
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<BloombergLP::bslmf::Nil,int,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslmf::Nil>;  // 59035
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<int,TestInt,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestString,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,int>;  // 59187
template struct BloombergLP::bslmf::IsSame<TestInt,BloombergLP::bslmf::Nil>;  // 58554
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestAllocObj,int,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestInt,TestString,TestVoid>,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> > >;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,Copyable>;  // 59203
template struct BloombergLP::bslmf::IsSame<Copyable,int>;  // 59204
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;  // 80651
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<int,const char *,TestVoid>,int>;  // 59187
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<int,const char *,TestVoid>,const char *>;  // 59187
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<int,double,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> > >,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> > >;
template struct BloombergLP::bslmf::IsSame<double,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bslalg::HasTrait<double,BloombergLP::bslalg::TypeTraitBitwiseCopyable>;
template struct BloombergLP::bslalg::HasTrait<double,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsConvertible<BloombergLP::bslmf::Nil,double>;
template struct BloombergLP::bslmf::IsConvertible<int,double>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList<int,bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,TestInt,TestString,TestVoid,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil>,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList2<TestString,TestArg<2> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList2<TestString,TestArg<2> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList2<TestString,TestArg<2> >,TestArg<2> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<2>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestString,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<TestString,TestArg<2>,TestArg<3> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<TestString,TestArg<2>,TestArg<3> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<TestString,TestArg<2>,TestArg<3> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList3<TestString,TestArg<2>,TestArg<3> >,TestArg<3> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<3>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<2>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList4<TestString,TestArg<2>,TestArg<3>,TestArg<4> >,TestArg<4> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<4>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<3>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,TestString>;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bslmf::IsConvertible<double,double>;
template struct BloombergLP::bslmf::IsSame<TestArg<4>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList5<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5> >,TestArg<5> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<5>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<5>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList6<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6> >,TestArg<6> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<6>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<6>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestString>;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bslmf::IsConvertible<bsl::basic_string<char,bsl::char_traits<char>,bsl::allocator<char> >,double>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList7<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7> >,TestArg<7> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<7>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<7>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList8<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8> >,TestArg<8> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<8>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<8>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<5> >;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList9<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9> >,TestArg<9> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<9>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<9>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<6> >;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList10<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10> >,TestArg<10> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<10>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<10>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestString>;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<6> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList11<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11> >,TestArg<11> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<11>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<11>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<6> >;
//template class BloombergLP::bdlb::VariantImp<BloombergLP::bslmf::TypeList<Copyable,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil,BloombergLP::bslmf::Nil> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<7> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<8> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<9> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<10> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<11> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList12<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12> >,TestArg<12> >;
template struct BloombergLP::bslalg::HasTrait<TestArg<12>,BloombergLP::bslalg::TypeTraitPair>;
template struct BloombergLP::bslmf::IsSame<TestArg<12>,BloombergLP::bslmf::Nil>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,BloombergLP::bslma::TestAllocator *>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestString>;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<2> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<3> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<4> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<5> >;
template struct BloombergLP::bdlb::Variant_TypeIndex<BloombergLP::bslmf::TypeList13<TestString,TestArg<2>,TestArg<3>,TestArg<4>,TestArg<5>,TestArg<6>,TestArg<7>,TestArg<8>,TestArg<9>,TestArg<10>,TestArg<11>,TestArg<12>,TestArg<13> >,TestArg<6> >;

#endif  // BSLS_PLATFORM_CMP_IBM

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
