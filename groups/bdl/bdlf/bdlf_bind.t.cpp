// bdlf_bind.t.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlf_bind.h>

#include <bdlf_bind_test.h>
#include <bdlf_placeholder.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_nil.h>

#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>        // 'atoi'
#include <bsl_cstring.h>        // 'strcpy'
#include <bsl_functional.h>     // 'ref', 'cref'
#include <bsl_string.h>


using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The 'bdlf_bind' component defines a parameterized type for binding an
// "invocable" (to respect the terminology of the component-level
// documentation), which is either a free function, a member function, or a
// function object (either by value or reference).  The binding produces a
// "bound object" (that can be passed by value) that encapsulates both the
// function called (by address for free functions, and by owning a copy of the
// underlying object otherwise) and arguments passed to it (by value).  Because
// it allocates space for storing all this information, there are a lot of
// concerns about memory allocation.  In addition, there are some concerns
// about properly passing the parameters by reference or by value to respect
// the signature of the invocable.
//
// Our main objectives are to make sure that (1) the bound object or function
// gets called appropriately with the correct arguments, in all various
// combinations of function types (free functions, member functions, function
// objects, and function object references) and parameters (value types,
// placeholders, and 'bdlf_bind' objects), and that (2) the binder correctly
// propagates its allocator to all objects under its management (including all
// the copies of the function objects and parameters that it owns).  In
// addition, we would like to make sure that (3) the signature of the invocable
// is respected, in that arguments that are passed by reference are not copied,
// and that const-correctness is respected.
//
// This is a large component by the number of lines of code, and even more so
// since it is a template and has at least *four* orthogonal dimensions: the
// family of factory methods ('bind', bindR', or 'bindS'), the nature of the
// bound object (free function pointer or reference, member function pointer,
// and function object by reference or by value), the number of arguments (from
// 0 up to 14 for function objects, and 0 up to 13 for free and member
// functions since the first argument is consumed by the test object pointer),
// and the invocation (any of the up to 2^14 possible combinations of
// placeholders or bound arguments).  In addition, in order to test the memory
// allocation model, we must use test classes that do and that do not allocate
// memory.  This is clearly too tall an order.
//
// Even if we test only all placeholders or all bound arguments (a more
// elaborate mix of placeholders and arguments is explored in a separate test
// case [3]), it is impractical to test the remaining almost thousand
// combinations in a single component (due to limitation in the compilation
// time of test driver).  We therefore split the testing into 14 components and
// delegate the combinations of the family of factory methods ('bind', bindR',
// or 'bindS'), the nature of the bound object (free function pointer or
// reference, member function pointer, and function object by reference or by
// value), and the allocation model, when invoked with 'N' arguments to the
// component 'bdlf::bind_testN'.  It remains here only to test the concerns
// that do not explicitly depend on the number of arguments passed to the bound
// object.
//
// Our test plan proceeds by checking that the traits are set up so that
// allocators know about 'bdlf::Bind' using the 'bslma::Allocator' protocol
// (case 2).  Then we test the mix of placeholders and bound arguments (case
// 3).  We also test the additional concerns about passing 'bdlf_bind' objects
// as parameters to the various 'bdlf::BindUtil::bind", 'bindR' and 'bindS'
// (case 4) and about respecting the signature of the invocable (case 5).
// Finally, we make sure the usage example compiles and runs as advertised
// (case 6).
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING BSLALG_DECLARE_NESTED_TRAITS
// [ 3] MIXING BOUND ARGUMENTS AND PLACEHOLDERS
// [ 4] PASSING 'bdlf_bind' OBJECTS AS PARAMETERS
// [ 5] RESPECTING THE SIGNATURE OF THE INVOKABLE
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------

#ifdef BSLS_PLATFORM_CMP_MSVC
// disable warning about decorated name length being exceeded
#pragma warning( disable : 4503 )
#endif

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64 Int64;

bool globalVerbose = false;

// The whole bdlf_bind component currently works with up to 14 arguments.

const int BIND_MAX_ARGUMENTS = 14;
const int NUM_SLOTS = BIND_MAX_ARGUMENTS+1;

// Nil value (uninitialized).

const int N1 = -1;

// Typedefs for argument and test types.

typedef bdlf::Bind_TestSlotsNoAlloc    SlotsNoAlloc;

typedef bdlf::Bind_TestArgNoAlloc<1>   NoAllocTestArg1;
typedef bdlf::Bind_TestArgNoAlloc<2>   NoAllocTestArg2;
typedef bdlf::Bind_TestArgNoAlloc<3>   NoAllocTestArg3;
typedef bdlf::Bind_TestArgNoAlloc<4>   NoAllocTestArg4;
typedef bdlf::Bind_TestArgNoAlloc<5>   NoAllocTestArg5;
typedef bdlf::Bind_TestArgNoAlloc<6>   NoAllocTestArg6;
typedef bdlf::Bind_TestArgNoAlloc<7>   NoAllocTestArg7;
typedef bdlf::Bind_TestArgNoAlloc<8>   NoAllocTestArg8;
typedef bdlf::Bind_TestArgNoAlloc<9>   NoAllocTestArg9;
typedef bdlf::Bind_TestArgNoAlloc<10>  NoAllocTestArg10;
typedef bdlf::Bind_TestArgNoAlloc<11>  NoAllocTestArg11;
typedef bdlf::Bind_TestArgNoAlloc<12>  NoAllocTestArg12;
typedef bdlf::Bind_TestArgNoAlloc<13>  NoAllocTestArg13;
typedef bdlf::Bind_TestArgNoAlloc<14>  NoAllocTestArg14;

typedef bdlf::Bind_TestTypeNoAlloc     NoAllocTestType;

typedef bdlf::Bind_TestSlotsAlloc      SlotsAlloc;

typedef bdlf::Bind_TestArgAlloc<1>     AllocTestArg1;
typedef bdlf::Bind_TestArgAlloc<2>     AllocTestArg2;
typedef bdlf::Bind_TestArgAlloc<3>     AllocTestArg3;
typedef bdlf::Bind_TestArgAlloc<4>     AllocTestArg4;
typedef bdlf::Bind_TestArgAlloc<5>     AllocTestArg5;
typedef bdlf::Bind_TestArgAlloc<6>     AllocTestArg6;
typedef bdlf::Bind_TestArgAlloc<7>     AllocTestArg7;
typedef bdlf::Bind_TestArgAlloc<8>     AllocTestArg8;
typedef bdlf::Bind_TestArgAlloc<9>     AllocTestArg9;
typedef bdlf::Bind_TestArgAlloc<10>    AllocTestArg10;
typedef bdlf::Bind_TestArgAlloc<11>    AllocTestArg11;
typedef bdlf::Bind_TestArgAlloc<12>    AllocTestArg12;
typedef bdlf::Bind_TestArgAlloc<13>    AllocTestArg13;
typedef bdlf::Bind_TestArgAlloc<14>    AllocTestArg14;

typedef bdlf::Bind_TestTypeAlloc       AllocTestType;

// Placeholder types for the corresponding _1, _2, etc.

typedef bdlf::PlaceHolder<1>  PH1;
typedef bdlf::PlaceHolder<2>  PH2;
typedef bdlf::PlaceHolder<3>  PH3;
typedef bdlf::PlaceHolder<4>  PH4;
typedef bdlf::PlaceHolder<5>  PH5;
typedef bdlf::PlaceHolder<6>  PH6;
typedef bdlf::PlaceHolder<7>  PH7;
typedef bdlf::PlaceHolder<8>  PH8;
typedef bdlf::PlaceHolder<9>  PH9;
typedef bdlf::PlaceHolder<10> PH10;
typedef bdlf::PlaceHolder<11> PH11;
typedef bdlf::PlaceHolder<12> PH12;
typedef bdlf::PlaceHolder<13> PH13;
typedef bdlf::PlaceHolder<14> PH14;

// Values that do not take an allocator.

static const NoAllocTestArg1  I1  = 1;
static const NoAllocTestArg2  I2  = 2;
static const NoAllocTestArg3  I3  = 3;
static const NoAllocTestArg4  I4  = 4;
static const NoAllocTestArg5  I5  = 5;
static const NoAllocTestArg6  I6  = 6;
static const NoAllocTestArg7  I7  = 7;
static const NoAllocTestArg8  I8  = 8;
static const NoAllocTestArg9  I9  = 9;
static const NoAllocTestArg10 I10 = 10;
static const NoAllocTestArg11 I11 = 11;
static const NoAllocTestArg12 I12 = 12;
static const NoAllocTestArg13 I13 = 13;
static const NoAllocTestArg14 I14 = 14;

// ============================================================================
//                     GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

class ConvertibleFromToInt {
    // Object that is convertible from and to 'int'.  Used to make sure that
    // 'bindR' can overwrite the return type of a bound object to an arbitrary
    // type that is convertible from the actual return type of the bound
    // object.  It is also convertible to 'int' to enable comparison to an int
    // for the verification of the return value of the binder invocation.

    int d_value; // int value of this object

    public:
    // CREATORS
    ConvertibleFromToInt() : d_value() { }
    ConvertibleFromToInt(int value) : d_value(value) { }

    // MANIPULATORS
    operator int&() { return d_value; }

    // ACCESSORS
    operator int() const { return d_value; }
};

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//                   TESTING FUNCTIONS/CLASSES FOR CASE 5
// ----------------------------------------------------------------------------
extern "C" { // cannot be part of a namespace (namespace would be ignored)

int myFunctionWithExternCLinkage(int x)
{
    return x;
}

}  // extern "C"

namespace BDLF_BIND_TEST_CASE_5 {

int myFunctionWithConstnessMix(const int   a1,
                               const int&  a2,
                               int&        a3,
                               const int*& a4)
{
    // Note that this function can generate an explicit binder since the
    // signature is explicitly known at compile time.  This allows the bound
    // arguments to be stored with the actual type of the above parameters.

    a3 = a1;
    a4 = &a2;
    return 1;
}

int myFunctionWithNoConstnessMix(int& a1, int& a2, int& a3, const int*& a4)
{
    // This function is a non-const-safe version of the above function, which
    // allows to take true references, since there is no mix between lvalues
    // and rvalues in the arguments.  This is to forgo our limitation to the
    // const forwarding problem.

    a3 = a1;
    a4 = &a2;
    return 1;
}

int myFunctionWithVolatile(volatile int const& a1, volatile int* a2)
{
    *a2 = a1;
    return 1;
}

struct MyFunctionObjectWithMultipleSignatures {
    // This stateless 'struct' provides several function operators with
    // different signatures, including one of them being a template, all
    // returning a different value.

    // TYPES
    typedef int ResultType;

    // ACCESSORS
    int operator()(int) const
    {
        return 1;
    }

    int operator()(const NoAllocTestArg1&) const
    {
        return 2;
    }

    template <class T>
    int operator()(const T&) const
    {
        return 3;
    }
};

template <class Binder>
void testMultipleSignatureBinder(Binder binder)
{
    int                   i1(1);
    const NoAllocTestArg1 I1(1);

    ASSERT(1 == binder(i1));
    ASSERT(2 == binder(I1));

    // The type 'int' is not an exact match for double and so the operator
    // template is preferred.

    ASSERT(3 == binder(1.0));
}

struct MyFunctionObjectWithConstAndNonConstOperator {
    // This stateless 'struct' provides several function operators with const
    // and non-const signature.

    // TYPES
    typedef int ResultType;

    // MANIPULATORS
    int operator()(ConvertibleFromToInt const&)
    {
        return 0;
    }

    // ACCESSORS
    int operator()(int) const
    {
        return 1;
    }
};

void testMyFunctionObjectWithConstAndNonConstOperator()
{
    using namespace bdlf::PlaceHolders;

    MyFunctionObjectWithConstAndNonConstOperator        mBinder;
    const MyFunctionObjectWithConstAndNonConstOperator& binder = mBinder;

    const ConvertibleFromToInt X(1); // value is irrelevant
    const int I(2);                  // value is irrelevant

    // When functor is taken by address, the non-const operator will be an
    // exact match for argument 'x', and the const operator will be an exact
    // match for argument 'i'.
    ASSERT(0 == bdlf::BindUtil::bind(&mBinder, _1)(X));
    // We need to pass 'binder' and not 'mBinder' otherwise there is an
    // ambiguity between converting I to a 'ConvertibleFromToInt const&', or
    // '*(&binder)' to a 'MyFunctionObjectWithConstAndNonConstOperator const&'.
    ASSERT(1 == bdlf::BindUtil::bind(&binder, _1)(I));

    // When functor is taken by value, the non-const operator will no longer be
    // an exact match for argument 'x' but the const operator will pick up via
    // a conversion to int, and the const operator will still be an exact match
    // for argument 'i'.
    ASSERT(1 == bdlf::BindUtil::bind(mBinder, _1)(X));
    ASSERT(1 == bdlf::BindUtil::bind(mBinder, _1)(I));
}

struct BaseClass {
    // This struct is so small that we inline its members to keep it more
    // readable.  Its 'test' method is virtual, and returns 1 for the base
    // class.

    // CREATORS
    BaseClass() { }
    virtual ~BaseClass() { }

    // ACCESSORS
    virtual int test() const { return 1; }
};

struct DerivedClass : public BaseClass {
    // This struct is so small that we inline its members to keep it more
    // readable.  Its 'test' method returns 2 for this derived class.

    // CREATORS
    DerivedClass() { }
    virtual ~DerivedClass() { }

    // ACCESSORS
    virtual int test() const { return 2; }
};

struct AbstractBaseClass {
    // This struct is so small that we inline its members to keep it more
    // readable.  Its 'test' method is virtual, and returns 1 for the base
    // class.

    // CREATORS
    AbstractBaseClass() { }
    virtual ~AbstractBaseClass() { }

    // ACCESSORS
    virtual int test() const = 0;
};

struct ConcreteDerivedClass : public AbstractBaseClass {
    // This struct is so small that we inline its members to keep it more
    // readable.  Its 'test' method returns 2 for this derived class.

    // CREATORS
    ConcreteDerivedClass() { }
    virtual ~ConcreteDerivedClass() { }

    // ACCESSORS
    virtual int test() const { return 2; }
};

}  // close namespace BDLF_BIND_TEST_CASE_5

// ----------------------------------------------------------------------------
//                   TESTING FUNCTIONS/CLASSES FOR CASE 4
// ----------------------------------------------------------------------------
namespace BDLF_BIND_TEST_CASE_4 {

#define BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS                                  \
                    NoAllocTestArg1  const& a1,  NoAllocTestArg2  const& a2,  \
                    NoAllocTestArg3  const& a3,  NoAllocTestArg4  const& a4,  \
                    NoAllocTestArg5  const& a5,  NoAllocTestArg6  const& a6,  \
                    NoAllocTestArg7  const& a7,  NoAllocTestArg8  const& a8,  \
                    NoAllocTestArg9  const& a9,  NoAllocTestArg10 const& a10, \
                    NoAllocTestArg11 const& a11, NoAllocTestArg12 const& a12, \
                    NoAllocTestArg13 const& a13, NoAllocTestArg14 const& a14
    // The declarations of 14 arguments for each of these functions gets a
    // little repetitive for no benefits, it is clearer to make it into a
    // macro.

#define USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS                              \
        (void) a1, (void) a2, (void) a3, (void) a4, (void) a5, (void) a6,     \
        (void) a7, (void) a8, (void) a9, (void) a10, (void) a11, (void) a12,  \
        (void) a13, (void) a14;
    // The use of the 14 arguments for each of these functions to avoid getting
    // an unused parameter compiler warning.

int sumOf14Arguments( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    return a1.value()
         + a2.value()
         + a3.value()
         + a4.value()
         + a5.value()
         + a6.value()
         + a7.value()
         + a8.value()
         + a9.value()
         + a10.value()
         + a11.value()
         + a12.value()
         + a13.value()
         + a14.value();
}

NoAllocTestArg1 selectArgument1( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a1.value(),  1);
    return a1;
}

NoAllocTestArg2 selectArgument2( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a2.value(),  2);
    return a2;
}

NoAllocTestArg3 selectArgument3( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a3.value(),  3);
    return a3;
}

NoAllocTestArg4 selectArgument4( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a4.value(),  4);
    return a4;
}

NoAllocTestArg5 selectArgument5( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a5.value(),  5);
    return a5;
}

NoAllocTestArg6 selectArgument6( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a6.value(),  6);
    return a6;
}

NoAllocTestArg7 selectArgument7( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a7.value(),  7);
    return a7;
}

NoAllocTestArg8 selectArgument8( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a8.value(),  8);
    return a8;
}

NoAllocTestArg9 selectArgument9( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a9.value(),  9);
    return a9;
}

NoAllocTestArg10 selectArgument10( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a10.value(),  10);
    return a10;
}

NoAllocTestArg11 selectArgument11( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a11.value(),  11);
    return a11;
}

NoAllocTestArg12 selectArgument12( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a12.value(),  12);
    return a12;
}

NoAllocTestArg13 selectArgument13( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a13.value(),  13);
    return a13;
}

NoAllocTestArg14 selectArgument14( BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsNoAlloc::setSlot(a14.value(),  14);
    return a14;
}

#undef BDLF_BIND_TEST_NO_ALLOC_14_ARGUMENTS

#define BDLF_BIND_TEST_ALLOC_14_ARGUMENTS                                     \
                        AllocTestArg1  const& a1,  AllocTestArg2  const& a2,  \
                        AllocTestArg3  const& a3,  AllocTestArg4  const& a4,  \
                        AllocTestArg5  const& a5,  AllocTestArg6  const& a6,  \
                        AllocTestArg7  const& a7,  AllocTestArg8  const& a8,  \
                        AllocTestArg9  const& a9,  AllocTestArg10 const& a10, \
                        AllocTestArg11 const& a11, AllocTestArg12 const& a12, \
                        AllocTestArg13 const& a13, AllocTestArg14 const& a14

// Note that the return value is a const& to avoid inadvertently triggering a
// copy that would use the default allocator.

AllocTestArg1 const& selectAllocArgument1( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a1.allocator(),  1);
    return a1;
}

AllocTestArg2 const& selectAllocArgument2( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a2.allocator(),  2);
    return a2;
}

AllocTestArg3 const& selectAllocArgument3( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a3.allocator(),  3);
    return a3;
}

AllocTestArg4 const& selectAllocArgument4( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a4.allocator(),  4);
    return a4;
}

AllocTestArg5 const& selectAllocArgument5( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a5.allocator(),  5);
    return a5;
}

AllocTestArg6 const& selectAllocArgument6( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a6.allocator(),  6);
    return a6;
}

AllocTestArg7 const& selectAllocArgument7( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a7.allocator(),  7);
    return a7;
}

AllocTestArg8 const& selectAllocArgument8( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a8.allocator(),  8);
    return a8;
}

AllocTestArg9 const& selectAllocArgument9( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a9.allocator(),  9);
    return a9;
}

AllocTestArg10 const&
                     selectAllocArgument10( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a10.allocator(),  10);
    return a10;
}

AllocTestArg11 const&
                     selectAllocArgument11( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a11.allocator(),  11);
    return a11;
}

AllocTestArg12 const&
                     selectAllocArgument12( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a12.allocator(),  12);
    return a12;
}

AllocTestArg13 const&
                     selectAllocArgument13( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a13.allocator(),  13);
    return a13;
}

AllocTestArg14 const&
                     selectAllocArgument14( BDLF_BIND_TEST_ALLOC_14_ARGUMENTS )
{
    USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
    SlotsAlloc::setSlot(a14.allocator(),  14);
    return a14;
}

#undef USE_BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS
#undef BDLF_BIND_TEST_ALLOC_14_ARGUMENTS

}  // close namespace BDLF_BIND_TEST_CASE_4

// ----------------------------------------------------------------------------
//                   TESTING FUNCTIONS/CLASSES FOR CASE 3
// ----------------------------------------------------------------------------
namespace BDLF_BIND_TEST_CASE_3 {

template <// Types of bound arguments, could be I1-14, or placeholder.
          class B1,  class B2,  class B3,  class B4,  class B5,
          class B6,  class B7,  class B8,  class B9,  class B10,
          class B11, class B12, class B13, class B14,
          // Types of invocation arguments, could be I1-14, or (int)N1.
          class A1,  class A2,  class A3,  class A4,  class A5,
          class A6,  class A7,  class A8,  class A9,  class A10,
          class A11, class A12, class A13, class A14>
void testPlaceHolder(
        // Bound arguments follow.
        B1 b1,  B2 b2,  B3 b3,   B4  b4,  B5  b5,  B6  b6,  B7  b7,
        B8 b8,  B9 b9,  B10 b10, B11 b11, B12 b12, B13 b13, B14 b14,
        // Invocation argument follows.
        A1 a1,  A2 a2,  A3 a3,   A4  a4,  A5  a5,  A6  a6,  A7  a7,
        A8 a8,  A9 a9,  A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
{
    bslma::TestAllocator  ta0;
    bslma::Allocator     *Z0 = &ta0;

    // This test code is taken from case 2 of 'bdlf_bind_test14', with the
    // checks for allocation removed.

          NoAllocTestType  mX;
    const NoAllocTestType& X = mX;
    const NoAllocTestType  EXPECTED(I1,I2,I3,I4,I5,I6,I7,
                                    I8,I9,I10,I11,I12,I13,I14);
    ASSERT( EXPECTED != X);

    ASSERT(14 == bdlf::BindUtil::bind(
                                                   &mX, // invocable
        b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14) // bound arguments
     (a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)); // invocation arguments

    ASSERT(EXPECTED == X);

    ASSERT(14 == bdlf::BindUtil::bindS(Z0,
                                                   &mX, // invocable
        b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14) // bound arguments
     (a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)); // invocation arguments

    ASSERT(EXPECTED == X);
}

}  // close namespace BDLF_BIND_TEST_CASE_3

// ============================================================================
//                   BREATHING TEST CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------
namespace BDLF_BIND_BREATHING_TEST {

using namespace bdlf::PlaceHolders;

///Elementary construction and usage of 'bdlf::Bind' objects
///--------------------------------------------------------
// Bound objects are generally constructed by invoking the 'bdlf::BindUtil'
// with an "invocation template".  An invocation template is a series of one or
// more arguments that describe how to invoke the bound object.  Each argument
// can be either a place-holder or a literal value.  Literal values are stored
// by value in the binder and directly forwarded to the bound object when
// invoked.  Place-holders are substituted with the respective argument
// provided at invocation of the binder.  For example, given the following
// 'invocable' (here a free function for simplicity):
//..
    void invocable(int i, int j, const char *str) {
        // Do something with 'i', 'j' and 'str' ... e.g.:
        printf("Invoked with: %d %d %s\n", i, j, str);
    }
//..
// and the following (global) string:
//..
    const char *someString = "p3"; // for third parameter to 'invocable'
//..
// we can bind the parameters of 'invocable' to the following arguments:
//..
    void bindTest(bslma::Allocator *allocator = 0) {
        bdlf::BindUtil::bind(&invocable,         // bound object and
                            10, 14, (const char*)"p3") // bound arguments
//..
// and the binder declared above can be passed invocation arguments directly,
// as follows (here we specify zero invocation arguments since all the bound
// arguments are fully specified):
//..
                                                 (); // invocation
//..
// Similarly, we can also create a reference-counted shared binder using the
// 'bindS' method:
//..
        bdlf::BindUtil::bindS(allocator,                    // allocator,
                              &invocable,                   // bound object and
                              10, 14, (const char*)"p3")(); // bound arguments
//..

     }
//..
// In the function call above, the 'invocable' will be bound with the arguments
// '10', '14', and '"p3"' respectively, then invoked with those bound
// arguments.  Going forward we will show function invocations that use both
// 'bind' and 'bindS' variations to highlight that the binder can be created as
// a regular object or a shared pointer.
//
// In the next example, place-holders are used to forward user-provided
// arguments to the bound object.  We separate the invocation of the binder
// into a function template to avoid having to declare the type of the binder:
//..
    template <class BINDER>
    void callBinder(BINDER const& b)
    {
        b(10, 14);
    }
//..
// The creation of the binder is as follows:
//..
    void bindTest1(bslma::Allocator *allocator = 0) {
        callBinder(bdlf::BindUtil::bind(&invocable, _1, _2, someString));

        callBinder(
             bdlf::BindUtil::bindS(allocator, &invocable, _1, _2, someString));
    }
//..
// In these code snippets, the 'callBinder' template function is invoked with a
// binder bound to the specified 'invocable' and having the invocation
// template '_1', '_2', and '"p3"' respectively.  The two special parameters
// '_1' and '_2' are place-holders for arguments one and two, respectively,
// which will be specified to the binder at invocation time.  Each place-holder
// will be substituted with the corresponding positional argument when invoked.
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(10, 14, "p3");
//..
// Place-holders can appear anywhere in the invocation template, and in any
// order.  The same place-holder can appear multiple times.  Each instance will
// be substituted with the same value.  For example, in the following snippet
// of code, the 'callBinder' function, is invoked with a binder such
// that argument one (10) of the binder is passed as parameter two
// and argument two (14) is passed as (i.e., bound to) parameter one:
//..
    void bindTest2(bslma::Allocator *allocator = 0) {
        callBinder(bdlf::BindUtil::bind(&invocable, _2, _1, someString));

        callBinder(
            bdlf::BindUtil::bindS(allocator, &invocable, _2, _1, someString));
    }
//..
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(14, 10, "p3");
//..
// The following snippet of code illustrates a number of ways to call
// 'bdlf::BindUtil' and their respective output:
//..
    int test1(int i, int j) {
        return i + j;
    }

    int abs(int x) {
        return (x > 0) ? x : -x;
    }

    void bindTest3(bslma::Allocator *allocator = 0) {
        using namespace bdlf::PlaceHolders;
        ASSERT(24 == bdlf::BindUtil::bind(&test1, _1, _2)(10, 14));
        ASSERT(24 == bdlf::BindUtil::bind(&test1, _1, 14)(10));
        ASSERT(24 == bdlf::BindUtil::bind(&test1, 10, _1 )(14));
        ASSERT(24 == bdlf::BindUtil::bind(&test1, 10, 14)());
        ASSERT(24 == bdlf::BindUtil::bind(&test1,
                                      bdlf::BindUtil::bind(&abs,_1), 14)(-10));

        ASSERT(24 == bdlf::BindUtil::bindS(allocator, &test1, _1, _2)(10, 14));
        ASSERT(24 == bdlf::BindUtil::bindS(allocator, &test1, _1, 14)(10));
        ASSERT(24 == bdlf::BindUtil::bindS(allocator, &test1, 10, _1 )(14));
        ASSERT(24 == bdlf::BindUtil::bindS(allocator, &test1, 10, 14)());
        ASSERT(24 == bdlf::BindUtil::bindS(allocator, &test1,
                         bdlf::BindUtil::bindS(allocator, &abs, _1), 14)(-10));
    }
//..
// The usage example below provides a more comprehensive series of calling
// sequences.
//
///Binding Data
///------------
// The main use of 'bdlf::Bind' is to invoke bound objects with additional data
// that is not specified by the caller.  For that purpose, place-holders are
// key.  There are a couple of issues to understand in order to properly use
// this component.  The bound arguments must be of a value-semantic type
// (unless they are place-holders or 'bdlf::Bind' objects).  They are evaluated
// at binding time once and only once and their value copied into the binder
// (using the default allocator to supply memory unless an allocator is
// specified).  A 'bdlf::Bind' object always invokes its bound object with only
// the arguments listed as bound arguments, regardless of how many arguments
// are specified to the binder at invocation time.  Invocation arguments that
// are not referenced through a place-holder are simply discarded.  Invocation
// arguments that are duplicated (by using the same place-holder several times)
// are simply copied several times.  The following examples should make things
// perfectly clear.
//
///Ignoring parameters
///- - - - - - - - - -
// It is possible to pass more invocation arguments to a binder than was
// specified in the signature by the number of bound arguments.  Invocation
// arguments not referenced by any placeholder, as well as extra invocation
// arguments, will be ignored.  Note that they will nevertheless be evaluated
// even though their value will be unused.  Consider, for example, the
// following snippet of code:
//..
    int marker = 0;
    int singleArgumentFunction(int x) {
        return x;
    }

    int identityFunctionWithSideEffects(int x)
    {
        printf("Calling 'identityFunctionWithSideEffects' with %d\n", x);
        marker += x;
        return x;
    }

    template <class BINDER>
    void callBinderWithSideEffects1(BINDER const& binder)
    {
        ASSERT(14 == binder(identityFunctionWithSideEffects(10), 14));
    }

    void bindTest4(bslma::Allocator *allocator = 0) {
        marker = 0;
        callBinderWithSideEffects1(
                            bdlf::BindUtil::bind(&singleArgumentFunction, _2));

//..
// In the above snippets of code, 'singleArgumentFunction' will be called with
// only the second argument (14) specified to the binder at invocation time in
// the 'callBinderWithSideEffects1' function.  Thus the return value of the
// invocation must be 14.  The 'identityFunctionWithSideEffects(10)' will be
// evaluated, even though its return value (10) will be discarded.  We can
// check this as follows:
//..
        LOOP_ASSERT(marker, 10 == marker);

//..
// We repeat the same call using 'bindS' below:
//..
        marker = 0;
        callBinderWithSideEffects1(
                bdlf::BindUtil::bindS(allocator, &singleArgumentFunction, _2));
        LOOP_ASSERT(marker, 10 == marker);
//..
    }
//..
///Duplicating parameters
///- - - - - - - - - - -
// Consider another example that reuses the 'identityFunctionWithSideEffects'
// of the previous example:
//..
    int doubleArgumentFunction(int x, int y) {
        return x+y;
    }

    template <class BINDER>
    void callBinderWithSideEffects2(BINDER const& binder)
    {
        const int RET1 = binder(10);
        ASSERT(20 == RET1);
        const int RET2 = binder(identityFunctionWithSideEffects(10));
        ASSERT(20 == RET2);
    }

    void bindTest5(bslma::Allocator *allocator = 0) {
        marker = 0;
        callBinderWithSideEffects2(
                        bdlf::BindUtil::bind(&doubleArgumentFunction, _1, _1));
//..
// In the above snippet of code, 'doubleArgumentFunction' will be called with
// the first argument ('identityFunctionWithSideEffects(10)') specified to
// the binder, computed only once at invocation time.  We can check this as
// follows:
//..
        LOOP_ASSERT(marker, 10 == marker);

//..
// We repeat the same call using 'bindS' below:
//..
        marker = 0;
        callBinderWithSideEffects2(
            bdlf::BindUtil::bindS(allocator, &doubleArgumentFunction, _1, _1));
        LOOP_ASSERT(marker, 10 == marker);
    }
//..
// The return value (10) will, however, be copied twice before being passed to
// the 'doubleArgumentFunction' which will return their sum (20).
//
///Bound objects
///-------------
// There are a few issues to be aware of concerning the kind of bound objects
// that can successfully be used with this component.
//
///Binding to member functions
///- - - - - - - - - - - - - -
// Although member function pointers are not invoked in syntactically the same
// manner as free functions, they can still be used directly in conjunction
// with 'bdlf::Bind'.  When the binder detects that a member function pointer
// was specified, it automatically wraps it in a 'bdlf::MemFn' object.  In this
// case a pointer to the object must be passed as the first argument to bind,
// followed by the remaining arguments.  See the usage example "Binding to
// Member Functions" below.
//
///Binding to functions with an ellipsis
///- - - - - - - - - - - - - - - - - - -
// It is possible to create a binder with a free function (pointer or
// reference) that takes an ellipsis (e.g., 'int printf(const char*, ...').
// This component does not support ellipsis in member function pointers,
// however.  See the 'bindTest7' example function at the end of the usage
// example below.
//
///Binding to function objects by value or by address
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Although function objects are invoked in syntactically the same manner as
// free functions, they can be used by value or by address in conjunction with
// 'bdlf::Bind'.  When the binder detects that a pointer to a function object
// was specified, it automatically dereferences that pointer prior to invoking
// the function object.  The difference between the two usages is that the
// binder object holds a copy of the whole object or of its address only.  In
// particular, when passing by value an object that takes an allocator, the
// copy held by the binder uses the same allocator as the binder object (if
// constructed by 'bdlf::BindUtil::bindA') or the default allocator (if
// constructed by 'bdlf::BindUtil::bind' or 'bdlf::BindUtil::bindR'), but *not*
// the allocator of the original object.  For keeping the same allocator, pass
// the object by address to the binder instead.  See the section "Binding with
// allocators" and the usage example sections "Binding to Function Objects" and
// "Binding to Function Objects by Reference" below.
//
///Inferring the signature of the bound object
///- - - - - - - - - - - - - - - - - - - - - -
// A 'bdlf::Bind' object will strive to properly and automatically resolve the
// signature of its bound object between different overloads of that invocable.
// The signature of the bound object is inferred from that of the bound object
// and the type of the arguments either at binding or invocation time.  The
// signature of invocables that are not function objects (i.e., free functions
// with C++ linkage and member functions) must be determined at binding time
// (in particular, overloads must be disambiguated when obtaining the address
// of the function).  In those cases, the bound arguments will be of the
// corresponding type and any values passed as bound arguments (except
// placeholders) will be cast to the corresponding type at binding time and
// stored by value unless the argument type is a reference type.
//
// Invocation arguments will be cast in place of their corresponding
// place-holder(s) to the corresponding type only at invocation time.  If the
// signature of the bound object is known, the invocation argument will be
// forwarded using the most efficient type (in particular, unnecessary copies
// will be avoided for non fundamental types).  This has the implication that
// even if an invocation argument is passed as non-modifiable reference, it can
// be copied and taking its address may yield the address of a temporary.  The
// safe rule is: to take an argument by reference, pass its address as a
// pointer type, not its reference.
//
// Some invocable objects, however, may not allow the binder to detect their
// signature until invocation.  This is the case for function objects, for free
// functions with C linkage (e.g., 'printf'), if a bound argument is a nested
// binder, or if a placeholder is used in two positions in the bound arguments.
// In that case, the bound arguments are stored in their own types, and cast to
// the corresponding argument type of the signature only when the signature is
// determined at invocation time.  Place-holders, likewise, are not typed and
// will acquire the type of their corresponding invocation argument when
// invoked, which will be cast to the corresponding argument type of the
// signature.  In particular, the same binder constructed with a functor and
// place-holders in place of the bound arguments can invoke several overloads
// of the 'operator()' of the functor, depending on the type of the invocation
// arguments.
//
// Although function objects are invoked in syntactically the same manner as
// free functions, their return type cannot always be inferred.  The same
// limitation applies to free functions with 'extern "C"' linkage.  In that
// case, the return type has to be given explicitly to the binder.  This can be
// done by using the 'bdlf::BindUtil::bindR' function.  Note that all
// 'bsl::function' objects have a public type 'ResultType' to assist the
// compiler in that respect and can be used with 'bdlf::BindUtil::bind'.  See
// the usage example "Binding to a Function Object with Explicit Return Type"
// below.
//
///Binding with constants and temporaries
/// - - - - - - - - - - - - - - - - - - -
// Due to a technical restriction of the C++ language known as the "forwarding
// problem", it is not possible to match the signature of a function object
// exactly when passing a mix of non-'const' lvalues and rvalues as bound
// arguments.  Nevertheless, this component supports passing literal values and
// temporaries as invocation arguments to a 'bdlf::Bind' object.  There is
// however one limitation: if *any* of the arguments in the signature of the
// bound object should be of a modifiable reference type, then *all* the
// invocation arguments need to be (either constant or modifiable) references.
// That is, it is not possible to pass a literal value to some argument of a
// bound object when another argument expects a modifiable reference.  Note
// that a direct call to the bound object (without the binder) would accept
// such an argument.  This is not a severe limitation, and the workaround is to
// pass instead a local non-modifiable variable initialized to the literal
// value.
//
///Binding with allocators
///-----------------------
// The copy of the bound object and arguments are created as members of the
// 'bdlf::Bind' object, so no memory is allocated for those.  When the bound
// object or arguments use memory allocation, however, that memory is supplied
// by the default allocator unless 'bdlf::BindUtil::bindA' is used to specify
// the allocator to be passed to the copy constructors of the bound object and
// arguments.  Note that the invocation arguments, passed to the binder at
// invocation time, are passed "as is" to the bound object, and are not copied
// if the bound object takes them by modifiable or non-modifiable reference.
//
// In order to make clear where the allocation occurs, we will wrap "p3" into a
// type that takes an allocator, e.g., a class 'MyString' (kept minimal here
// for the purpose of exposition):
//..
    class MyString {
        // PRIVATE INSTANCE DATA
        bslma::Allocator *d_allocator_p;
        char             *d_string_p;

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(MyString, bslma::UsesBslmaAllocator);

        //CREATORS
        MyString(const char *str, bslma::Allocator *allocator = 0)
        : d_allocator_p(bslma::Default::allocator(allocator))
        , d_string_p((char*)d_allocator_p->allocate(1 + strlen(str)))
        {
            strcpy(d_string_p, str);
        }

        MyString(MyString const& rhs, bslma::Allocator *allocator = 0)
        : d_allocator_p(bslma::Default::allocator(allocator))
        , d_string_p((char*)d_allocator_p->allocate(1 + strlen(rhs)))
        {
            strcpy(d_string_p, rhs);
        }

        ~MyString() {
            d_allocator_p->deallocate(d_string_p);
        }

        // ACCESSORS
        operator const char*() const { return d_string_p; }
    };
//..
// We will also use a 'bslma::TestAllocator' to keep track of the memory
// allocated:
//..
    void bindTest6() {
        bslma::TestAllocator allocator;
        MyString myString((const char*)"p3", &allocator);
        const Int64 NUM_ALLOCS = allocator.numAllocations();
//..
// To expose that the default allocator is not used, we will use a default
// allocator guard, which will re-route any default allocation to the local
// 'defaultAllocator':
//..
        bslma::TestAllocator defaultAllocator("Default", globalVerbose);
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
        const Int64 NUM_DEFAULT_ALLOCS = defaultAllocator.numAllocations();
//..
// We now create a shared binder object with 'allocator' using 'bindS'.  If the
// bound object were an instance of a class taking an allocator, then
// 'allocator' would be passed to its copy constructor; in this case.  In this
// case, 'allocator' will be used to make the copy of 'myString' held by the
// binder:
//..
        callBinder(
              bdlf::BindUtil::bindS(&allocator, &invocable, _1, _2, myString));
//..
// We now check that memory was allocated from the test allocator, and none
// from the default allocator:
//..
        ASSERT(NUM_ALLOCS != allocator.numAllocations());
//#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERTV(NUM_DEFAULT_ALLOCS,   defaultAllocator.numAllocations(),
                NUM_DEFAULT_ALLOCS == defaultAllocator.numAllocations());
//#endif
    }
//..

}  // close namespace BDLF_BIND_BREATHING_TEST

// ============================================================================
//                    USAGE EXAMPLE CLASSES AND FUNCTIONS
// ----------------------------------------------------------------------------
namespace BDLF_BIND_USAGE_EXAMPLE {

using namespace bdlf::PlaceHolders;

///Usage
///-----
// What follows is a series of code snippets illustrating detailed aspects of
// typical usage of 'bdlf::Bind' and 'bdlf::BindUtil'.  For these examples, we
// will use a typical pair of event and scheduler classes, where the event is
// defined as:
//..
    struct MyEvent {
        // Event data, for illustration purpose here:
        int d_value;
    };
//..
// and the scheduler is defined as follows:
//..
    class MyEventScheduler {
        // This class owns a callback function object that takes an 'int' and
        // an instance of 'MyEvent'.  When the 'run' method is called, it
        //invokes the callback with a series of events that it obtains using
        //its own stream of events.

        // PRIVATE INSTANCE DATA
        bsl::function<void(int, MyEvent)> d_callback;
//..
// For illustration purposes, we give this scheduler a dummy stream of events:
//..
        int d_count;

        // PRIVATE MANIPULATORS
        int getNextEvent(MyEvent *eventBuffer) {
            // Create a copy of the next event in the specified 'eventBuffer'
            // Return 0 on success, and non-zero if no event is available.

            if (--d_count) {
                eventBuffer->d_value = d_count;
            }
            return d_count;
        }
//..
// A scheduler is created with a callback function object as follows:
//..
      public:
        // CREATORS
        MyEventScheduler(bsl::function<void(int, MyEvent)> const& callback)
        : d_callback(callback)
        {
        }
//..
// and its main function is to invoke the callback on the series of events as
// obtained by 'getNextEvent':
//..
        // MANIPULATORS
        void run(int n)
        {
            MyEvent e;
            d_count = n;
            while (!getNextEvent(&e)) {
                d_callback(0, e);
            }
        }
    };
//..
///Binding to Free Functions
/// - -  - - - - - - - - - -
// We illustrate how to use a scheduler with free callback functions that have
// various signatures by passing a binder as the callback function of the
// scheduler, and how to use the binder to match the signature of the callback
// function.  Note that at the point of invocation in 'run' the binder will be
// invoked with two invocation arguments, thus we may only use place-holders
// '_1' and '_2'.  In the following snippet of code, the binder passes its
// invocation arguments straight through to the callback:
//..
    void myCallback(int, MyEvent const&)
    {
        // Do something ...
    }

    void myMainLoop(bslma::Allocator *allocator = 0)
    {
        MyEventScheduler schedA(bdlf::BindUtil::bind(&myCallback, _1, _2));
        schedA.run(10);

        MyEventScheduler schedB(
                        bdlf::BindUtil::bindS(allocator, &myCallback, _1, _2));
        schedB.run(10);
    }
//..
// Next we show how to bind some of the callback arguments at binding time,
// while letting the invocation arguments straight through to the callback as
// the first two arguments:
//..
    void myCallbackWithUserArgs(int, MyEvent const&, int, double)
    {
        // Do something ...
    }

    void myMainLoop2(bslma::Allocator *allocator = 0)
    {
        MyEventScheduler schedA(
             bdlf::BindUtil::bind(&myCallbackWithUserArgs, _1, _2, 360, 3.14));
        schedA.run(10);

        MyEventScheduler schedB(bdlf::BindUtil::bindS(allocator,
                                                      &myCallbackWithUserArgs,
                                                      _1,
                                                      _2,
                                                      360,
                                                      3.14));
        schedB.run(10);
    }
//..
// In the next snippet of code, we show how to reorder the invocation arguments
// before they are passed to the callback:
//..
    void myCallbackWithUserArgsReordered(int, int, double, MyEvent const&)
    {
        // Do something ...
    }


    void myMainLoop3(bslma::Allocator *allocator = 0)
    {
        MyEventScheduler schedA(bdlf::BindUtil::bind(
                                              &myCallbackWithUserArgsReordered,
                                              _1,
                                              360,
                                              3.14,
                                              _2));
        schedA.run(10);

        MyEventScheduler schedB(bdlf::BindUtil::bindS(
                                              allocator,
                                              &myCallbackWithUserArgsReordered,
                                              _1,
                                              360,
                                              3.14,
                                              _2));
        schedB.run(10);
    }
//..
// And finally, we illustrate that the signature of the callback can be
// *smaller* than expected by the scheduler by letting the binder ignore its
// first argument:
//..
    void myCallbackThatDiscardsResult(MyEvent const&)
    {
        // Do something ...
    }

    void myMainLoop4(bslma::Allocator *allocator = 0)
    {
        MyEventScheduler schedA(
                      bdlf::BindUtil::bind(&myCallbackThatDiscardsResult, _2));
        schedA.run(10);

        MyEventScheduler schedB(bdlf::BindUtil::bindS(
                                                 allocator,
                                                 &myCallbackThatDiscardsResult,
                                                 _2));
        schedB.run(10);
    }
//..
///Binding to Function Objects
///- - - - - - - - - - - - - -
// In the next example, we wrap the callback function into a function object
// which is bound by value.  For brevity, we only present the basic example of
// passing the arguments straight through to the actual callback 'operator()',
// but all the variations of the previous example could be given as well.
//..
    struct MyCallbackObject {
        typedef void ResultType;
        MyCallbackObject()
        {
        }
        void operator() (int result, MyEvent const& event) const
        {
            myCallback(result, event);
        }
    };

    void myMainLoop5(bslma::Allocator *allocator = 0)
    {
        MyCallbackObject objA;
        MyEventScheduler schedA(bdlf::BindUtil::bind(objA, _1, _2));
        schedA.run(10);

        MyCallbackObject objB;
        MyEventScheduler schedB(bdlf::BindUtil::bindS(allocator,objB, _1, _2));
        schedB.run(10);
    }
//..
///Binding to Function Objects by Reference
/// - - - - - - - - - - - - - - - - - - - -
// The following example reuses the 'MyCallbackObject' of the previous example,
// but illustrates that it can be passed by reference as well as by value:
//..
    void myMainLoop6(bslma::Allocator *allocator = 0)
    {
        MyCallbackObject objA;
        MyEventScheduler schedA(bdlf::BindUtil::bind(&objA, _1, _2));
        schedA.run(10);

        MyCallbackObject objB;
        MyEventScheduler schedB(
                              bdlf::BindUtil::bindS(allocator, &objB, _1, _2));
        schedB.run(10);
    }
//..
///Binding to Member Functions
///- - - - - - - - - - - - - -
// In the next example, we show that the callback function can be a member
// function, in which case there are three, not two, bound arguments.  The
// first bound argument must be a pointer to an instance of the class owning
// the member function.
//..
    struct MyStatefulObject {
      // State info
      public:
        void callback(int, MyEvent const&)
        {
            // Do something that may modify the state info...
        }
    };

    void myMainLoop7(bslma::Allocator *allocator = 0)
    {
        MyStatefulObject objA;
        MyEventScheduler schedA(
             bdlf::BindUtil::bind(&MyStatefulObject::callback, &objA, _1, _2));
        schedA.run(10);

        MyStatefulObject objB;
        MyEventScheduler schedB(bdlf::BindUtil::bindS(
                                                   allocator,
                                                   &MyStatefulObject::callback,
                                                   &objB,
                                                   _1,
                                                   _2));
        schedB.run(10);
    }
//..
///Nesting Bindings
/// - - - - - - - -
// We now show that it is possible to provide a binder as an argument to
// 'bdlf::BindUtil'.  Upon invocation, the invocation arguments are forwarded
// to the nested binder.
//..
    MyEvent annotateEvent(int, MyEvent const& event) {
        // Do something to 'event' ...
        return event;
    }

    void myMainLoop8(bslma::Allocator *allocator = 0)
    {
        MyCallbackObject objA;
        MyEventScheduler schedA(bdlf::BindUtil::bind(
                                &objA,
                                _1,
                                bdlf::BindUtil::bind(&annotateEvent, _1, _2)));
        schedA.run(10);

        MyCallbackObject objB;
        MyEventScheduler schedB(bdlf::BindUtil::bindS(
                    allocator,
                    &objB,
                    _1,
                    bdlf::BindUtil::bindS(allocator, &annotateEvent, _1, _2)));
        schedB.run(10);
    }
//..
///Binding to a Function Object with Explicit Return Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the return type cannot be inferred from the bound object (using
// 'FUNC::result_type'), the binder needs an explicitly specification.  This is
// done by using the 'bdlf::BindUtil::bindR' function template as exemplified
// below:
//..
    typedef void GlobalResultType;
    struct MyCallbackObjectWithoutResultType {
        MyCallbackObjectWithoutResultType()
        {
        }
        GlobalResultType operator() (int result, MyEvent const& event) const
        {
            myCallback(result, event);
        }
    };

    void myMainLoop9(bslma::Allocator *allocator = 0)
    {
        MyCallbackObjectWithoutResultType objA;
        MyEventScheduler schedA(
                        bdlf::BindUtil::bindR<GlobalResultType>(objA, _1, _2));
        schedA.run(10);

        MyCallbackObjectWithoutResultType objB;
        MyEventScheduler schedB(
            bdlf::BindUtil::bindSR<GlobalResultType>(allocator, objB, _1, _2));
        schedB.run(10);
    }
//..
// Another situation where the return type (in fact, the whole signature)
// cannot be inferred from the bound object is the use of the free function
// with C linkage and variable number of arguments 'printf(const char*, ...)'.
// In the following code snippet, we show how the argument to the 'callBinder'
// function of section "Elementary construction and usage of 'bdlf::Bind'
// objects" above can be bound to 'printf':
//..
    void bindTest7(bslma::Allocator * = 0)
    {
        using namespace BDLF_BIND_BREATHING_TEST; // for testing only
#if 0
        const char* formatString = "Here it is: %d %d\n";
        callBinder(bdlf::BindUtil::bindR<int>(&printf, formatString, _1, _2));
#endif
    }
//..
// When called, 'bindTest7' will create a binder, pass it to 'callBinder' which
// will invoke it with arguments '10' and '14', and the output will be:
//..
//  Here it is: 10 14
//..

}  // close namespace BDLF_BIND_USAGE_EXAMPLE
// ============================================================================
//              USAGE EXAMPLE FROM OTHER TEST DRIVERS FUNCTIONS
// ----------------------------------------------------------------------------
namespace BDLF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS {

int onMasterCommand(const int&, int& stream, int *counter = 0)
{
    if (counter) {
        ++ *counter;
    }
    stream = *counter;
    return 0;
}

struct MyInt {
    int d_myInt;
    MyInt() : d_myInt(314159) {};
    MyInt(int x) : d_myInt(x) {};
    MyInt(MyInt const&x) : d_myInt(x.d_myInt) {};
    ~MyInt() { d_myInt = -1; };
};

void enqueuedJob(const MyInt& ptr) {
    ASSERT(314159 == ptr.d_myInt);
}

void enqueuedJob2(const MyInt& ptr1, const MyInt& ptr2) {
    ASSERT(314159 == ptr1.d_myInt);
    ASSERT(1618034 == ptr2.d_myInt);
}

}  // close namespace BDLF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS
// ============================================================================
//                MACROS EXPORTING INITIALIZATION OUT OF MAIN
// ----------------------------------------------------------------------------
#define DECLARE_MAIN_VARIABLES                                                \
    /*
    // The following machinery is for use in conjunction with the
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.
    // The slots are set when the corresponding function object or free
    // function is called with the appropriate number of arguments.
    */                                                                        \
                                                                              \
    const int NO_ALLOC_SLOTS[][NUM_SLOTS]= {                                  \
     /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  NumArgs */ \
     { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 0 */ \
     { -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 1 */ \
     { -1,  1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 2 */ \
     { -1,  1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 3 */ \
     { -1,  1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 4 */ \
     { -1,  1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 5 */ \
     { -1,  1,  2,  3,  4,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, }, /* 6 */ \
     { -1,  1,  2,  3,  4,  5,  6,  7, -1, -1, -1, -1, -1, -1, -1, }, /* 7 */ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1, }, /* 8 */ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, }, /* 9 */ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, -1, -1, -1, -1, }, /* 10*/ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, -1, -1, -1, }, /* 11*/ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, -1, -1, }, /* 12*/ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, -1, }, /* 13*/ \
     { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, }, /* 14*/ \
    };                                                                        \
    const int NO_ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {                           \
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,         \
    };                                                                        \
                                                                              \
    /*
    // Values that do not take an allocator are declared 'static const' above.

    // The following machinery is for use in conjunction with the
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.  The
    // slots are set when the corresponding function object or free function is
    // called the appropriate number of arguments.
    */                                                                        \
                                                                              \
                                                                              \
    bslma::TestAllocator allocator0(veryVeryVerbose);                         \
    bslma::TestAllocator allocator1(veryVeryVerbose);                         \
    bslma::TestAllocator allocator2(veryVeryVerbose);                         \
                                                                              \
    bslma::TestAllocator *Z0 = &allocator0;                                   \
    bslma::TestAllocator *Z1 = &allocator1;                                   \
    bslma::TestAllocator *Z2 = &allocator2;                                   \
                                                                              \
    bslma::DefaultAllocatorGuard allocGuard(Z0);                              \
    SlotsAlloc::setZ0(Z0);                                                    \
    SlotsAlloc::setZ1(Z1);                                                    \
    SlotsAlloc::setZ2(Z2);                                                    \
                                                                              \
    const bslma::Allocator *ALLOC_SLOTS[][NUM_SLOTS] = {                      \
     /* 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  NumArgs */ \
     { Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 0 */ \
     { Z0, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 1 */ \
     { Z0, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 2 */ \
     { Z0, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 3 */ \
     { Z0, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 4 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 5 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 6 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 7 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, }, /* 8 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, }, /* 9 */ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, }, /* 10*/ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, }, /* 11*/ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, }, /* 12*/ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, }, /* 13*/ \
     { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, }, /* 14*/ \
    };                                                                        \
    const bslma::Allocator *ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {                \
          Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0,         \
    };                                                                        \
                                                                              \
    /*
    // Values that do take an allocator (default allocator is used, which means
    // means we must perform the initialization *after* the allocator guard,
    // hence in main).
    */                                                                        \
                                                                              \
    const AllocTestArg1  V1(1),     NV1(-1);                                  \
    const AllocTestArg2  V2(20),    NV2(-20);                                 \
    const AllocTestArg3  V3(23),    NV3(-23);                                 \
    const AllocTestArg4  V4(44),    NV4(-44);                                 \
    const AllocTestArg5  V5(66),    NV5(-66);                                 \
    const AllocTestArg6  V6(176),   NV6(-176);                                \
    const AllocTestArg7  V7(878),   NV7(-878);                                \
    const AllocTestArg8  V8(8),     NV8(-8);                                  \
    const AllocTestArg9  V9(912),   NV9(-912);                                \
    const AllocTestArg10 V10(102),  NV10(-120);                               \
    const AllocTestArg11 V11(111),  NV11(-111);                               \
    const AllocTestArg12 V12(333),  NV12(-333);                               \
    const AllocTestArg13 V13(712),  NV13(-712);                               \
    const AllocTestArg14 V14(1414), NV14(-1414);

// ============================================================================
//                                TEST CASES
// ----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)

DEFINE_TEST_CASE(7) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING USAGE EXAMPLE FROM TEST DRIVERS
        //   Some usage examples in other package groups do not compile on
        //   AIX.  This test case is placed here for regression.
        //
        //   Also, make sure that a bound tuple contains a copy of the value,
        //   not a reference, that will outlive the local arguments passed to
        //   bdlf::BindUtil::bind.
        //
        // Plan:
        //   Incorporate usage example from other test drivers.  See comments
        //   in the code below.
        //
        // Testing:
        //   USAGE EXAMPLE FROM TEST DRIVERS
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTesting Usage Example from Other Test Drivers"
                   "\n=============================================\n");

        using namespace BDLF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS;
        using namespace bdlf::PlaceHolders;

        {
            // Explicitly declare the binder instead of using BindUtil::bind.
            int extra = 0;
            typedef bdlf::Bind_BoundTuple3<bdlf::PlaceHolder<1>,
                                          bdlf::PlaceHolder<2>,
                                          int*> LIST;
            typedef int (*FUNC)(const int &, int &, int *);
            bdlf::Bind<bslmf::Nil, FUNC, LIST> b(&onMasterCommand,
                                                 LIST(_1, _2, &extra));

            // Make sure the binder works properly.
            int stream = 0;
            int prefix = 0;
            ASSERT(0 == b(prefix, stream));
            ASSERT(1 == extra);
            ASSERT(1 == stream);

            // Make sure an equivalent binder constructed with BindUtil::bind
            // works similarly.
            ASSERT(0 == bdlf::BindUtil::bind(&onMasterCommand, _1, _2, &extra)
                                            (prefix, stream));
            ASSERT(2 == extra);
            ASSERT(2 == stream);

            extra = 0, stream = 0, prefix = 0;
            ASSERT(0 == bdlf::BindUtil::bindS(Z0,
                                              &onMasterCommand,
                                              _1,
                                              _2,
                                              &extra)(prefix, stream));
            ASSERT(1 == extra);
            ASSERT(1 == stream);
            ASSERT(0 == bdlf::BindUtil::bindS(Z0,
                                              &onMasterCommand,
                                              _1,
                                              _2,
                                              &extra)(prefix, stream));
            ASSERT(2 == extra);
            ASSERT(2 == stream);
        }

        {
            // Failure to copy MyInt by value (the signature of 'enqueueJob'
            // takes a 'const&') will trigger destruction of the MyInt object.

            bsl::function<void(void)> job;

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159);
                job = bdlf::BindUtil::bind(&enqueuedJob, i);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copy of 'myInt', make sure
                   // that the argument passed is 314159 (by value) and not 0
                   // (as would be if 'i' was stored by address).

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159);
                job = bdlf::BindUtil::bindS(Z0, &enqueuedJob, i);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copy of 'myInt', make sure
                   // that the argument passed is 314159 (by value) and not 0
                   // (as would be if 'i' was stored by address).

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159), j(1618034);
                job = bdlf::BindUtil::bind(&enqueuedJob2, i, j);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copies of 'myInt', make
                   // sure that the argument passed is 314159 (by value) and
                   // not 0 (as would be if 'i' was stored by address).

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159), j(1618034);
                job = bdlf::BindUtil::bindS(Z0, &enqueuedJob2, i, j);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copies of 'myInt', make
                   // sure that the argument passed is 314159 (by value) and
                   // not 0 (as would be if 'i' was stored by address).
        }
      }

DEFINE_TEST_CASE(6) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING USAGE EXAMPLES
        //   The usage examples provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage examples from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLES
        // ------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Examples"
                            "\n======================\n");

        using namespace BDLF_BIND_USAGE_EXAMPLE;

        if (verbose) printf("\tmyMainLoop1.\n");
        myMainLoop();
        if (verbose) printf("\tmyMainLoop2.\n");
        myMainLoop2();
        if (verbose) printf("\tmyMainLoop3.\n");
        myMainLoop3();
        if (verbose) printf("\tmyMainLoop4.\n");
        myMainLoop4();
        if (verbose) printf("\tmyMainLoop5.\n");
        myMainLoop5();
        if (verbose) printf("\tmyMainLoop6.\n");
        myMainLoop6();
        if (verbose) printf("\tmyMainLoop7.\n");
        myMainLoop7();
        if (verbose) printf("\tmyMainLoop8.\n");
        myMainLoop8();
        if (verbose) printf("\tmyMainLoop9.\n");
        myMainLoop9();
        if (verbose) printf("\tbindTest7.\n");
        bindTest7();
      }

DEFINE_TEST_CASE(5) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // RESPECTING THE SIGNATURE OF THE INVOCABLE
        //
        // Concerns:
        //   1. That we can bind functions with extern "C" linkage.
        //   2. That we forward reference types correctly.
        //   3. That we respect the constness of arguments (within the
        //   limitations imposed by our handling of the forwarding problem).
        //   4. That we correctly respect volatile qualifications.  This is a
        //   concern because the forwarding type invokes 'bslmf::RemoveCvq'.
        //   5. That we can bind a function object with placeholders, and later
        //   invoke the binder resolving to two different overloads based on
        //   the type of the invocation arguments.
        //   6. That we cannot call non-const member functions for a function
        //   object that is passed by value (it would only modify the temporary
        //   bound copy, probably not what the user intended).
        //
        // Plan:
        //   1. Create binders with bound objects that have extern "C" linkage
        //   and verify that the binders works as expected.
        //   2 and 3. Create binders with bound objects whose signatures are a
        //   mix of non-const and const rvalues and lvalues.  Verify that the
        //   code compiles and works as expected.
        //   4. Create binders with bound objects whose signatures are a mix
        //   of non-const and const volatile rvalues and lvalues.
        //   Verify that the code compiles and works as expected.
        //   5. Create a binder with a bound object that accepts multiple
        //   signatures and verify that the proper signature is called
        //   for various calling sequences.
        //   6. This one is tricky to test since we essentially want to make
        //   sure that there would be compilation failure, but this test driver
        //   cannot trigger it.  We create a binder with a bound object that
        //   takes an int and has an exact-match non-const operator, and a
        //   near-match (with conversion to int) const operator that takes a
        //   ConvertibleFromToInt.  When invoked with an int value, we make
        //   sure that the exact match is not considered by checking that the
        //   conversion took place.
        //
        // Testing:
        //   RESPECTING THE SIGNATURE OF THE INVOCABLE
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING RESPECTING THE SIGNATURE OF THE INVOCABLE"
                   "\n=================================================\n");

        using namespace BDLF_BIND_TEST_CASE_5;
        using namespace bdlf::PlaceHolders;

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // xlC compiler on AIX does not distinguish between the manglings of
        // extern "C" and C++ linkages.

        if (verbose) printf("\tRespecting extern \"C\" linkage.\n");
        {
            const int UNIQUE_INT = 12345678;

                // by reference, with no placeholder
            ASSERT(UNIQUE_INT == bdlf::BindUtil::bind(
                                                  myFunctionWithExternCLinkage,
                                                  UNIQUE_INT)());

            ASSERT(UNIQUE_INT == bdlf::BindUtil::bindS(
                                                  Z0,
                                                  myFunctionWithExternCLinkage,
                                                  UNIQUE_INT)());

                // by reference, with placeholder
            ASSERT(UNIQUE_INT == bdlf::BindUtil::bind(
                                                  myFunctionWithExternCLinkage,
                                                  _1)(UNIQUE_INT));

            ASSERT(UNIQUE_INT == bdlf::BindUtil::bindS(
                                                  Z0,
                                                  myFunctionWithExternCLinkage,
                                                  _1)(UNIQUE_INT));

                // by address, with no placeholder
            ASSERT(UNIQUE_INT == bdlf::BindUtil::bind(
                                                 &myFunctionWithExternCLinkage,
                                                 UNIQUE_INT)());

            ASSERT(UNIQUE_INT == bdlf::BindUtil::bindS(
                                                 Z0,
                                                 &myFunctionWithExternCLinkage,
                                                 UNIQUE_INT)());

                // by address, with placeholder
            ASSERT(UNIQUE_INT == bdlf::BindUtil::bind(
                                                 &myFunctionWithExternCLinkage,
                                                 _1)(UNIQUE_INT));

            ASSERT(UNIQUE_INT == bdlf::BindUtil::bindS(
                                                 Z0,
                                                 &myFunctionWithExternCLinkage,
                                                 _1)(UNIQUE_INT));
        }
#endif

        if (verbose)
         printf("\tRespecting mix of const and non-const lvalue arguments.\n");
        {
            int        mX1(1);     const int& X1 = mX1;
            int        mX2(2);     const int& X2 = mX2;
            int        mX3(3);
            const int *mX4 = &X1;

            // With an explicit binder, the signature known at compile time.
            // The bound arguments are stored with the appropriate type
            // (respecting non-const references).  Here, every invocation
            // argument is a reference.

            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithNoConstnessMix,
                                           _1,_2,_3,_4) // bound arguments
                                 (mX1, mX2, mX3, mX4)); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4);

            ASSERT(1 == bdlf::BindUtil::bindS(Z0,
                                              &myFunctionWithNoConstnessMix,
                                           _1,_2,_3,_4) // bound arguments
                                 (mX1, mX2, mX3, mX4)); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4);

            // Due to our limitation of the const forwarding problem, however,
            // since below *some* arguments are taken as const rvalues, the
            // rule is that *all* the arguments are taken by const rvalue
            // including 'mX3' and 'mx4' with the effect that the following
            // code would not compile:
            //..
            //  ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
            //                        X1, X2, mX3, mX4) // bound arguments
            //                                     ()); // invocation arguments
            //..
            // Instead, we must explicitly construct references with a wrapper,
            // so as to pass them by value as in the following:

#if !defined(BSLS_PLATFORM_CMP_SUN)
    // The combination of bcef::Bind, Sun CC, bsl::cref/ref, ForwardingType,
    // and who knows what else makes this test fail to compile.

            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
                      X1, cref(X2), ref(mX3), ref(mX4)) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithConstnessMix,
                      X1, cref(X2), ref(mX3), ref(mX4)) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.
#endif

            // Note that the wrapper for 'X2' is necessary, even though X2 is
            // taken by 'const&' since what will be passed to the invocable
            // will be the copy of 'X2' in the binder, not 'X2' itself.

            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
                            X1, X2, ref(mX3), ref(mX4)) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 != mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithConstnessMix,
                            X1, X2, ref(mX3), ref(mX4)) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 != mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.

            // With invocation arguments, however, this should not be necessary
            // when the binder is explicit since we should be able to pass
            // references directly, but in fact it still is for 'X2' since it
            // is passed by 'const&' and the forwarding type of an 'int' would
            // allow to take an 'int' instead of a 'const int&'.

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
                                        _1, _2, _3, _4) // bound arguments
                                   (X1, X2, mX3, mX4)); // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // In a change from BDE libraries prior to 2.24
                                // fundamental types also pass by reference.

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithConstnessMix,
                                        _1, _2, _3, _4) // bound arguments
                                   (X1, X2, mX3, mX4)); // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.

#if !defined(BSLS_PLATFORM_CMP_IBM) && !defined(BSLS_PLATFORM_CMP_SUN)
            // The IBM xlC compiler is broken in ellipsis matching, meaning
            // 'bdlf::Bind_FuncTraitsHasNoEllipsis' will return 0 even for
            // functions that does not have an ellipsis in its function
            // signature.  When this happens, implicit version of 'bdlf::Bind'
            // will be used, and X2 will be forwarded (thanks to the wrapper)
            // instead of copied.  Therefore, the following assert will fail:
            //..
            //  ASSERT(&X2 != mX4);
            //..

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
                                        _1, _2, _3, _4) // bound arguments
                             (X1, cref(X2), mX3, mX4)); // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // Passing a reference wrapper forwards a
                                // reference to the original object, rather
                                // than a distinct reference_wrapper object.
#endif

            // 'bdlf::Bind_FuncTraitsHasNoEllipsis' will return 0 even for
            // functions that does not have an ellipsis in its function
            // signature.  When this happens, implicit version of 'bdlf::Bind'
            // will be used, and X2 will be forwarded (thanks to the wrapper)
            // instead of copied.  Therefore, the following assert will fail:
            //..
            //  ASSERT(&X2 != mX4);
            //..

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithConstnessMix,
                                      _1, _2, _3, _4)  // bound arguments
                 (X1, cref(X2), ref(mX3), ref(mX4)));  // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // Passing a reference wrapper forwards a
                                // reference to the original object, rather
                                // than a distinct reference_wrapper object.

            // With a general (non-explicit) binder, signature is not detected
            // at compile time due to presence of duplicate placeholder.  And
            // so the reference invocation arguments must be wrapped again:

            mX3 = 3;
            mX4 = &mX1;
            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithConstnessMix,
                            _1, _1, ref(mX3), ref(mX4)) // bound arguments
                                           (cref(X2))); // invocation arguments
            ASSERT(X2  == mX3);
            ASSERT(&X2 == mX4); // Finally, it can work thanks to the wrapper
                                // and because the binder is *not* explicit.

            mX3 = 3;
            mX4 = &mX1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithConstnessMix,
                            _1, _1, ref(mX3), ref(mX4)) // bound arguments
                                           (cref(X2))); // invocation arguments
            ASSERT(X2  == mX3);
            ASSERT(&X2 == mX4); // Finally, it can work thanks to the wrapper
                                // and because the binder is *not* explicit.
        }

        if (verbose)
            printf("\tRespecting const volatile arguments.\n");
        {
            volatile int        mX1(1);     const volatile int& X1 = mX1;
            volatile int        mX2(2);     const volatile int& X2 = mX2;
            ASSERT(X1 != X2);

            // This code cannot compile because the component strips the
            // volatile qualifier in X1 when it is stored into the binder:
            //..
            // ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithVolatile,
            //                                X1, &mX2) // bound arguments
            //                                     ()); // invocation arguments
            //..
            // (Namely, 'p1', etc. are taken 'P1 const&' in
            // 'bdlf::BindUtil::bind' but are stored as 'P1' in the bound
            // tuple).  This is a bug, but there is a little to do about it
            // without much effort and the rare frequency of volatile in
            // binders does not warrant the effort to fix it.  In addition,
            // there is a simple fix, which is to wrap the rvalue reference:

            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithVolatile,
                                        cref(X1), &mX2) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1 == X2);

            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithVolatile,
                                        cref(X1), &mX2) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1 == X2);

            // With an explicit binder, the signature is known at compile time.
            // The invocation arguments respect the volatile qualifier.

            mX1 = 1;
            ASSERT(1 == bdlf::BindUtil::bind(&myFunctionWithVolatile,
                                                _1, _2) // bound arguments
                                           (X1, &mX2)); // invocation arguments
            ASSERT(X1 == X2);

            mX1 = 1;
            ASSERT(1 == bdlf::BindUtil::bindS(Z0, &myFunctionWithVolatile,
                                                _1, _2) // bound arguments
                                           (X1, &mX2)); // invocation arguments
            ASSERT(X1 == X2);
        }

        if (verbose)
            printf("\tAccepting multiple signatures\n");
        {
            MyFunctionObjectWithMultipleSignatures mX;

            testMultipleSignatureBinder(bdlf::BindUtil::bind(mX, _1));

            testMultipleSignatureBinder(bdlf::BindUtil::bindS(Z0, mX, _1));
        }

        if (verbose)
            printf("\tRespecting const-correctness of the invocable\n");
        {
            testMyFunctionObjectWithConstAndNonConstOperator();
        }

        if (verbose)
            printf("\tRespecting virtual functions\n");
        {
            BaseClass x;
            DerivedClass y;

            ASSERT(1 == bdlf::BindUtil::bind(
                                      bdlf::MemFnUtil::memFn(&BaseClass::test),
                                      _1)(&x));
            ASSERT(2 == bdlf::BindUtil::bind(
                                   bdlf::MemFnUtil::memFn(&DerivedClass::test),
                                   _1)(&y));
            ASSERT(2 == bdlf::BindUtil::bind(
                                      bdlf::MemFnUtil::memFn(&BaseClass::test),
                                      _1)((BaseClass*)&y));
        }

        if (verbose)
            printf("\tRespecting virtual functions\n");
        {
            ConcreteDerivedClass y;

            ASSERT(2 == bdlf::BindUtil::bind(
                              bdlf::MemFnUtil::memFn(&AbstractBaseClass::test),
                              _1)(&y));
            ASSERT(2 == bdlf::BindUtil::bind(
                           bdlf::MemFnUtil::memFn(&ConcreteDerivedClass::test),
                           _1)(&y));
        }
      }

DEFINE_TEST_CASE(4) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // PASSING 'Bind' and 'BindWrapper' OBJECTS AS PARAMETERS
        //
        // Concerns:
        //  That 'Bind' and 'BindWrapper' objects can be passed as bound
        //  arguments, and that the invocation arguments are properly passed to
        //  the place-holders of the nested 'bdlf::Bind' objects.  We want this
        //  to work on an arbitrary number of levels of recursion.
        //  Furthermore, we want to make sure that the allocator of the nested
        //  binders are inherited from the parent binder.
        //
        // Plan:
        //   Call 'bdlf::BindUtil::bind' with a 'NoAllocTestType' and 14
        //   arguments of type 'bdlf::Bind', each taking place-holder
        //   parameters '_1' up to '_14' and selecting its own argument;  upon
        //   invoking this binder with arguments 1 up to 14, check that the
        //   'noAllocSlots' are appropriately set (showing that the correct
        //   values are propagated to the nested binder's placeholders), and
        //   that the return value is correct.  Then do same test with
        //   'bdlf::BindUtil::bindA' and use 'allocTestType' and 'allocSlots'
        //   machinery to track allocators.  Finally, nest binders with three
        //   levels of recursion (enough to guarantee that recursion at any
        //   level will work) and again check that return value is correct.
        //
        // Testing:
        //   PASSING 'Bind' and 'BindWrapper' OBJECTS AS PARAMETERS
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING PASSING 'bdlf_bind' OBJECTS AS PARAMETERS"
                   "\n=================================================\n");

        using namespace BDLF_BIND_TEST_CASE_4;
        using namespace bdlf::PlaceHolders;

        if (verbose)
         printf("\tPropagating all invocation arguments to nested binders.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS_DEFAULT,
                                             veryVerbose));

#if    !defined(BSLS_PLATFORM_CMP_IBM) \
   && (!defined(BSLS_PLATFORM_CMP_SUN) || __SUNPRO_CC < 0x580)
            ASSERT(14 == bdlf::BindUtil::bind(&mX,
                              // first bound argument below
                              bdlf::BindUtil::bind(&selectArgument1,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // second bound argument below
                              bdlf::BindUtil::bind(&selectArgument2,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // third bound argument below
                              bdlf::BindUtil::bind(&selectArgument3,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // fourth bound argument below
                              bdlf::BindUtil::bind(&selectArgument4,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // fifth bound argument below
                              bdlf::BindUtil::bind(&selectArgument5,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // sixth bound argument below
                              bdlf::BindUtil::bind(&selectArgument6,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // seventh bound argument below
                              bdlf::BindUtil::bind(&selectArgument7,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // eighth bound argument below
                              bdlf::BindUtil::bind(&selectArgument8,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // ninth bound argument below
                              bdlf::BindUtil::bind(&selectArgument9,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // tenth bound argument below
                              bdlf::BindUtil::bind(&selectArgument10,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // eleventh bound argument below
                              bdlf::BindUtil::bind(&selectArgument11,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // twelfth bound argument below
                              bdlf::BindUtil::bind(&selectArgument12,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // thirteenth bound argument below
                              bdlf::BindUtil::bind(&selectArgument13,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // fourteenth bound argument below
                              bdlf::BindUtil::bind(&selectArgument14,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14))
                                       // invocation arguments follow
                                       (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                       I10, I11, I12, I13, I14));
#else
            // The AIX xlC compiler can compile the code above, but has
            // difficulty generating the stabstring (debug information) for the
            // type of the above binder and thus fails at linking time.  We
            // pass it a less complicated expression.

            ASSERT(14 == bdlf::BindUtil::bind(&mX,
                              // first bound argument below
                              bdlf::BindUtil::bind(&selectArgument1,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // second to thirteenth bound arguments below
                              _2, _3, _4, _5, _6, _7, _8, _9,
                              _10, _11, _12, _13,
                              // fourteenth bound argument below
                              bdlf::BindUtil::bind(&selectArgument14,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14))
                                       // invocation arguments follow
                                       (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                       I10, I11, I12, I13, I14));

#endif
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[14], veryVerbose));
            if (veryVerbose) { printf("%d: X: ", L_); X.print(); }
        }

        // The binders below hold their own copies of 'mX' (which uses 'Z1').
        // The copy of 'mX' should use the propagated allocator 'Z2', and thus
        // there should be *no* allocation taking place with 'Z1'.  The nested
        // binder (which uses 'Z0') has pointer semantics and thus is not
        // copied.  Each bind wrapper is going to use one allocation for the
        // shared pointer and the shared binder instance, since shared pointer
        // is created in place.

        if (verbose)
         printf("\tPropagating arguments to nested binders using 'bindS'\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS_DEFAULT,
                                             veryVerbose));

            ASSERT(14 == bdlf::BindUtil::bindS(Z0, mX,
                              // first bound argument below
                              bdlf::BindUtil::bindS(Z0, &selectArgument1,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // second to thirteenth bound arguments below
                              _2, _3, _4, _5, _6, _7, _8, _9,
                              _10, _11, _12, _13, _14)
                                       // invocation arguments follow
                                       (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                       I10, I11, I12, I13, I14));

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[14], veryVerbose));
            if (veryVerbose) { printf("%d: X: ", L_); X.print(); }
        }

#if 0
        if (verbose)
            printf("\tPropagating allocators to nested binders.\n");
        {
            AllocTestType mX(Z1);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            // The binder below holds its own copies of mX (which uses 'Z1')
            // and of all the nested binders (which use 'Z0').  All the copies
            // should use the propagated allocator 'Z2', and thus there should
            // be *no* allocation taking place with 'Z1', and only 14
            // allocations with 'Z0' for the temporary copy of 'mX' passed to
            // 'bdlf::BindUtil::bindA'.  When invoked with V1 up to V14 (which
            // use 'Z0') the allocator slots are set to 'Z0'.

            const int NUM_DEFAULT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

#if !defined(BSLS_PLATFORM_CMP_IBM)  \
  && (!defined(BSLS_PLATFORM_CMP_SUN) || __SUNPRO_CC < 0x580)
            ASSERT(14 == bdlf::BindUtil::bindA(Z2, mX,
                             // first bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument1,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // second bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument2,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // third bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument3,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // fourth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument4,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // fifth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument5,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // sixth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument6,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // seventh bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument7,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // eighth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument8,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // ninth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument9,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // tenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument10,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // eleventh bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument11,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // twelfth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument12,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // thirteenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument13,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // fourteenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument14,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14))
                                       // invocation arguments follow
                                       (V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                        V10, V11, V12, V13, V14));

#else
            // The AIX xlC compiler can compile the code above, but has
            // difficulty generating the stabstring (debug information) for the
            // type of the above binder and thus fails at linking time.  We
            // pass it a less complicated expression.

            ASSERT(14 == bdlf::BindUtil::bindA(Z2, mX,
                             // first bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument1,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // second to thirteenth bound arguments below
                             _2, _3, _4, _5, _6, _7, _8, _9,
                             _10, _11, _12, _13,
                             // fourteenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument14,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14))
                                       // invocation arguments follow
                                       (V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                        V10, V11, V12, V13, V14));
#endif
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations()
                                         - NUM_DEFAULT_ALLOCS_BEFORE;
            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;
#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so bindA does a copy
            // construction with the default allocator.
            LOOP_ASSERT(NUM_DEFAULT_ALLOCS, 14 == NUM_DEFAULT_ALLOCS);
#endif
            LOOP_ASSERT(NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        {
            AllocTestType mX(Z1);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            // The binder below holds its own copies of mX (which uses 'Z1')
            // and of all the nested binders (which use 'Z0').  There should be
            // 14^2=196 allocations taking place with 'Z0' for the nested
            // binders, and another 14+196 allocations with 'Z0' for the
            // temporary copies of 'mX' and the nested binders passed to the
            // top 'bdlf::BindUtil::bindA'.  All the copies inside the
            // top-level binder should use the propagated allocator 'Z2', and
            // thus when invoked with the binder copies of V1 up to V14 (which
            // use 'Z2') the allocator slots are set to 'Z2'.

            const int NUM_DEFAULT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

#if !defined(BSLS_PLATFORM_CMP_IBM)  \
  && (!defined(BSLS_PLATFORM_CMP_SUN) || __SUNPRO_CC < 0x580)
            ASSERT(14 == bdlf::BindUtil::bindA(Z2, mX,
                             // first bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument1,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // second bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument2,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // third bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument3,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // fourth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument4,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // fifth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument5,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // sixth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument6,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // seventh bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument7,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // eighth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument8,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // ninth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument9,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // tenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument10,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // eleventh bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument11,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // twelfth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument12,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // thirteenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument13,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14),
                             // fourteenth bound argument below
                             bdlf::BindUtil::bind(&selectAllocArgument14,
                                 V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                 V10, V11, V12, V13, V14))
                                             // invocation arguments follow
                                             ());

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations()
                                         - NUM_DEFAULT_ALLOCS_BEFORE;
#ifndef BSLS_PLATFORM_CMP_MSVC
            // MSVC 2005 does NOT use the RVO, so 'bindA' does a copy
            // construction with the default allocator.
            LOOP_ASSERT(NUM_DEFAULT_ALLOCS, 406 == NUM_DEFAULT_ALLOCS);
#endif
#else
            // The AIX xlC compiler can compile the code above, but has
            // difficulty generating the stabstring (debug information) for the
            // type of the above binder and thus fails at linking time.  We
            // pass it a less complicated expression.

            ASSERT(14 == bdlf::BindUtil::bindA(Z2, mX,
                              // first bound argument below
                              bdlf::BindUtil::bind(&selectAllocArgument1,
                                  V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                  V10, V11, V12, V13, V14),
                              // second to thirteenth bound arguments below
                              V2, V3, V4, V5, V6, V7, V8, V9,
                              V10, V11, V12, V13,
                              // fourteenth bound argument below
                              bdlf::BindUtil::bind(&selectAllocArgument14,
                                  V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                  V10, V11, V12, V13, V14))
                                       // invocation arguments follow
                                       ());

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations()
                                         - NUM_DEFAULT_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_DEFAULT_ALLOCS, 70 == NUM_DEFAULT_ALLOCS);
#endif
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[14], veryVerbose));

            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_ALLOCS, 0 == NUM_ALLOCS);
        }
#endif // #if 0

        if (verbose)
            printf("\tPropagating allocators to nested binders via 'bindS'\n");
        {
            AllocTestType mX(Z1);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            // There should be one allocation with 'Z0' (shared pointer and
            // binder instance) for the nested binder which has pointer
            // semantics and thus does not use 'Z2', plus another 14 for the
            // temporary copy of 'mX'.  When invoked with V1 up to V14 (which
            // use 'Z0') the allocator slots are set to 'Z0'.

            const int NUM_DEFAULT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            ASSERT(14 == bdlf::BindUtil::bindS(Z2, mX,
                             // first bound argument below
                             bdlf::BindUtil::bindS(Z0, &selectAllocArgument1,
                                 _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // second to thirteenth bound arguments below
                             _2, _3, _4, _5, _6, _7, _8, _9,
                             _10, _11, _12, _13, _14)
                                       // invocation arguments follow
                                       (V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                        V10, V11, V12, V13, V14));

            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations()
                                         - NUM_DEFAULT_ALLOCS_BEFORE;
            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_DEFAULT_ALLOCS, 15 == NUM_DEFAULT_ALLOCS);
            LOOP_ASSERT(NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        {
            AllocTestType mX(Z1);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            // The copy of 'mX' inside the top-level binder should use the
            // binder's allocator 'Z2', and thus when invoked with the binder
            // copies of V1 up to V14 (which use 'Z2') the allocator slots are
            // set to 'Z2', except for the first slot which should be set to
            // 'Z0'.  There should be 14 allocations for the nested binder's
            // bound argument plus another for the shared pointer and the
            // nested binder instance, plus 14 for the temporary copy of 'mX'
            // and another 14 for the temporary copies of the nested binder's
            // bound arguments.

            const int NUM_DEFAULT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            ASSERT(14 == bdlf::BindUtil::bindS(Z2, mX,
                              // first bound argument below
                              bdlf::BindUtil::bindS(Z0, &selectAllocArgument1,
                                  V1, V2, V3, V4, V5, V6, V7, V8, V9,
                                  V10, V11, V12, V13, V14),
                              // second to thirteenth bound arguments below
                              V2, V3, V4, V5, V6, V7, V8, V9,
                              V10, V11, V12, V13, V14)
                                       // invocation arguments follow
                                       ());

            const int NUM_DEFAULT_ALLOCS = Z0->numAllocations()
                                         - NUM_DEFAULT_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_DEFAULT_ALLOCS, 43 == NUM_DEFAULT_ALLOCS);

            const bslma::Allocator *BINDS_ALLOC_SLOTS[NUM_SLOTS] = {
                // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
                Z0, Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2,
            };

            ASSERT(SlotsAlloc::verifySlots(BINDS_ALLOC_SLOTS, veryVerbose));

            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tArbitrarily deeply nested binders.\n");
        {
            ASSERT(I1 == bdlf::BindUtil::bind(&selectArgument1,
                             // first bound argument, nested level 1
                             bdlf::BindUtil::bind(&selectArgument1,
                                 // first bound argument, nested level 2
                                 bdlf::BindUtil::bind(&selectArgument1,
                                     // all 14 bound arguments, nested level 3
                                     _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                     _10, _11, _12, _13, _14),
                                 // bound arguments 2 to 14, nested level 2
                                 _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // bound arguments 2 to 14, nested level 1
                             _2, _3, _4, _5, _6, _7, _8, _9,
                             _10, _11, _12, _13, _14)
                                     // invocation arguments follow
                                     (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                      I10, I11, I12, I13, I14));
        }

        if (verbose) printf("\tArbitrarily deeply nested binders "
                            "using 'bindS'\n");
        {
            ASSERT(I1 == bdlf::BindUtil::bindS(Z0, &selectArgument1,
                             // first bound argument, nested level 1
                             bdlf::BindUtil::bindS(Z0, &selectArgument1,
                                 // first bound argument, nested level 2
                                 bdlf::BindUtil::bindS(Z0, &selectArgument1,
                                     // all 14 bound arguments, nested level 3
                                     _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                     _10, _11, _12, _13, _14),
                                 // bound arguments 2 to 14, nested level 2
                                 _2, _3, _4, _5, _6, _7, _8, _9,
                                 _10, _11, _12, _13, _14),
                             // bound arguments 2 to 14, nested level 1
                             _2, _3, _4, _5, _6, _7, _8, _9,
                             _10, _11, _12, _13, _14)
                                     // invocation arguments follow
                                     (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                      I10, I11, I12, I13, I14));
        }
      }

DEFINE_TEST_CASE(3) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // MIXING BOUND ARGUMENTS AND PLACEHOLDERS
        //
        // Concern:
        //  Since the 'bdlf_bind_test[0-14]' components only test all bound
        //  arguments or all placeholders, we want to make sure other
        //  forwarding of the invocation arguments are supported in a
        //  type-safe way.  Also we want to make sure that unused invocation
        //  arguments (i.e., those not referred to by any placeholder) are
        //  indeed not used, but do not compromise the forwarding mechanism.
        //  Additionally, we want to confirm that the arguments bound using
        //  'bindS' are forwarded properly to the constructor of the binder by
        //  the 'bdlf::BindUtil::bind' methods.  Finally, the the invocation
        //  arguments passed to 'bindS' are forwarded properly to the
        //  invocation method of the 'bdlf::BindWrapper' object.
        //
        // Plan:
        //   Replicate some of the test cases from the 'bdlf_bind_test14'
        //   component with different forwarding strategies.  Because the
        //   mechanism is the same (template instantiation of the same code)
        //   for different invocables, it suffices to test with a function
        //   object passed by address (to keep things as simple as possible),
        //   and since we have ascertained that the 'bdlf::BindUtil' factory
        //   methods forward the arguments properly, it suffices to use all 14
        //   bound parameters (to exercise them all).  Instead of the 2^14*14!
        //   which amounts to 1428329123020800 possible combinations, we use a
        //   simple selection strategy:
        //   1. permute the order of the 14 placeholders (and the invocation
        //      arguments accordingly) in either a cyclic or reverse order.
        //   2. use a single placeholder (from _1 to _14) in each position with
        //      the remaining arguments bound to their respective values.
        //
        // Testing:
        //   TESTING MIXING BOUND ARGUMENTS AND PLACEHOLDERS
        // ------------------------------------------------------------------

        if (verbose) printf("\nMIXING BOUND ARGUMENTS AND PLACEHOLDERS"
                            "\n=======================================\n");

        using namespace BDLF_BIND_TEST_CASE_3;
        using namespace bdlf::PlaceHolders;

        if (verbose) printf("\tAll placeholders in cyclic permutation.\n");
        {
            testPlaceHolder(// Bound arguments follow.
                            _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,
                            // Invocation arguments follow.
                            I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14);

            testPlaceHolder(_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_1,
                            I14,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13);

            testPlaceHolder(_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_1,_2,
                            I13,I14,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12);

            testPlaceHolder(_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_1,_2,_3,
                            I12,I13,I14,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11);

            testPlaceHolder(_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_1,_2,_3,_4,
                            I11,I12,I13,I14,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10);

            testPlaceHolder(_6,_7,_8,_9,_10,_11,_12,_13,_14,_1,_2,_3,_4,_5,
                            I10,I11,I12,I13,I14,I1,I2,I3,I4,I5,I6,I7,I8,I9);

            testPlaceHolder(_7,_8,_9,_10,_11,_12,_13,_14,_1,_2,_3,_4,_5,_6,
                            I9,I10,I11,I12,I13,I14,I1,I2,I3,I4,I5,I6,I7,I8);

            testPlaceHolder(_8,_9,_10,_11,_12,_13,_14,_1,_2,_3,_4,_5,_6,_7,
                            I8,I9,I10,I11,I12,I13,I14,I1,I2,I3,I4,I5,I6,I7);

            testPlaceHolder(_9,_10,_11,_12,_13,_14,_1,_2,_3,_4,_5,_6,_7,_8,
                            I7,I8,I9,I10,I11,I12,I13,I14,I1,I2,I3,I4,I5,I6);

            testPlaceHolder(_10,_11,_12,_13,_14,_1,_2,_3,_4,_5,_6,_7,_8,_9,
                            I6,I7,I8,I9,I10,I11,I12,I13,I14,I1,I2,I3,I4,I5);

            testPlaceHolder(_11,_12,_13,_14,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,
                            I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I1,I2,I3,I4);

            testPlaceHolder(_12,_13,_14,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,
                            I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I1,I2,I3);

            testPlaceHolder(_13,_14,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,
                            I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I1,I2);

            testPlaceHolder(_14,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,
                            I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I1);

        }

        if (verbose) printf("\tSingle placeholder _1 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation argument follows.
                            I1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_1,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            I2,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_1,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            I3,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_1,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            I4,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_1,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            I5,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_1,I7,I8,I9,I10,I11,I12,I13,I14,
                            I6,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_1,I8,I9,I10,I11,I12,I13,I14,
                            I7,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_1,I9,I10,I11,I12,I13,I14,
                            I8,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_1,I10,I11,I12,I13,I14,
                            I9,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_1,I11,I12,I13,I14,
                            I10,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_1,I12,I13,I14,
                            I11,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_1,I13,I14,
                            I12,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_1,I14,
                            I13,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_1,
                            I14,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);
        }

#if !defined(BSLS_PLATFORM_CMP_GNU)
        // Gcc takes quite a bit longer than other compilers with many template
        // instantiations, and exceeds the prescribed timeout.  Since the
        // logic of this is not especially dependent on the compiler, it's
        // enough to test with SunPro and AIX xlC.

        if (verbose) printf("\tSingle placeholder _2 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _2,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,I1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,I2,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_2,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,I3,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_2,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,I4,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_2,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,I5,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_2,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,I6,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_2,I8,I9,I10,I11,I12,I13,I14,
                            N1,I7,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_2,I9,I10,I11,I12,I13,I14,
                            N1,I8,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_2,I10,I11,I12,I13,I14,
                            N1,I9,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_2,I11,I12,I13,I14,
                            N1,I10,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_2,I12,I13,I14,
                            N1,I11,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_2,I13,I14,
                            N1,I12,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_2,I14,
                            N1,I13,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_2,
                            N1,I14,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _3 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _3,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,I1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_3,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I2,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I3,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_3,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I4,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_3,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I5,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_3,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I6,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_3,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,I7,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_3,I9,I10,I11,I12,I13,I14,
                            N1,N1,I8,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_3,I10,I11,I12,I13,I14,
                            N1,N1,I9,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_3,I11,I12,I13,I14,
                            N1,N1,I10,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_3,I12,I13,I14,
                            N1,N1,I11,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_3,I13,I14,
                            N1,N1,I12,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_3,I14,
                            N1,N1,I13,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_3,
                            N1,N1,I14,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _4 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _4,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,I1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_4,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I2,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_4,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I3,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I4,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_4,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I5,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_4,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I6,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_4,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I7,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_4,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,I8,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_4,I10,I11,I12,I13,I14,
                            N1,N1,N1,I9,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_4,I11,I12,I13,I14,
                            N1,N1,N1,I10,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_4,I12,I13,I14,
                            N1,N1,N1,I11,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_4,I13,I14,
                            N1,N1,N1,I12,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_4,I14,
                            N1,N1,N1,I13,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_4,
                            N1,N1,N1,I14,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _5 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _5,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,I1,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_5,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I2,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_5,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I3,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_5,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I4,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I5,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_5,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I6,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_5,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I7,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_5,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I8,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_5,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,I9,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_5,I11,I12,I13,I14,
                            N1,N1,N1,N1,I10,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_5,I12,I13,I14,
                            N1,N1,N1,N1,I11,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_5,I13,I14,
                            N1,N1,N1,N1,I12,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_5,I14,
                            N1,N1,N1,N1,I13,N1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_5,
                            N1,N1,N1,N1,I14,N1,N1,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _6 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _6,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,I1,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_6,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I2,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_6,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I3,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_6,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I4,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_6,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I5,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I6,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_6,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I7,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_6,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I8,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_6,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I9,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_6,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,I10,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_6,I12,I13,I14,
                            N1,N1,N1,N1,N1,I11,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_6,I13,I14,
                            N1,N1,N1,N1,N1,I12,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_6,I14,
                            N1,N1,N1,N1,N1,I13,N1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_6,
                            N1,N1,N1,N1,N1,I14,N1,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _7 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _7,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,I1,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_7,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I2,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_7,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I3,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_7,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I4,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_7,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I5,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_7,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I6,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I7,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_7,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I8,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_7,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I9,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_7,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I10,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_7,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,I11,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_7,I13,I14,
                            N1,N1,N1,N1,N1,N1,I12,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_7,I14,
                            N1,N1,N1,N1,N1,N1,I13,N1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_7,
                            N1,N1,N1,N1,N1,N1,I14,N1,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _8 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _8,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,I1,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_8,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I2,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_8,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I3,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_8,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I4,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_8,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I5,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_8,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I6,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_8,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I7,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I8,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_8,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I9,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_8,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I10,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_8,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I11,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_8,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,I12,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_8,I14,
                            N1,N1,N1,N1,N1,N1,N1,I13,N1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_8,
                            N1,N1,N1,N1,N1,N1,N1,I14,N1,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _9 at floating position.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _9,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,I1,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,_9,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I2,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_9,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I3,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_9,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I4,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_9,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I5,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_9,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I6,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_9,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I7,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_9,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I8,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I9,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_9,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I10,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_9,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I11,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_9,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I12,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_9,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,I13,N1,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_9,
                            N1,N1,N1,N1,N1,N1,N1,N1,I14,N1,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _10 at floating pos.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _10,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I1,N1,N1,N1,N1);

            testPlaceHolder(I1,_10,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I2,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,_10,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I3,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_10,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I4,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_10,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I5,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_10,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I6,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_10,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I7,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_10,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I8,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_10,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I9,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I10,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_10,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I11,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_10,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I12,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_10,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I13,N1,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_10,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,I14,N1,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _11 at floating pos.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _11,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I1,N1,N1,N1);

            testPlaceHolder(I1,_11,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I2,N1,N1,N1);

            testPlaceHolder(I1,I2,_11,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I3,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,_11,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I4,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_11,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I5,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_11,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I6,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_11,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I7,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_11,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I8,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_11,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I9,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_11,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I10,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I11,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_11,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I12,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_11,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I13,N1,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_11,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I14,N1,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _12 at floating pos.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _12,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I1,N1,N1);

            testPlaceHolder(I1,_12,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I2,N1,N1);

            testPlaceHolder(I1,I2,_12,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I3,N1,N1);

            testPlaceHolder(I1,I2,I3,_12,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I4,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,_12,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I5,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_12,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I6,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_12,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I7,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_12,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I8,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_12,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I9,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_12,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I10,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_12,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I11,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I12,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_12,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I13,N1,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_12,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I14,N1,N1);
        }

        if (verbose) printf("\tSingle placeholder _13 at floating pos.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _13,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I1,N1);

            testPlaceHolder(I1,_13,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I2,N1);

            testPlaceHolder(I1,I2,_13,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I3,N1);

            testPlaceHolder(I1,I2,I3,_13,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I4,N1);

            testPlaceHolder(I1,I2,I3,I4,_13,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I5,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,_13,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I6,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_13,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I7,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_13,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I8,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_13,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I9,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_13,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I10,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_13,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I11,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_13,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I12,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I13,N1);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_13,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I14,N1);
        }
#endif

        if (verbose) printf("\tSingle placeholder _14 at floating pos.\n");
        {
            // All N1 invocation arguments are passed to the binder, but should
            // be unused.

            testPlaceHolder(// Bound arguments follow.
                            _14,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            // Invocation arguments follow.
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I1);

            testPlaceHolder(I1,_14,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I2);

            testPlaceHolder(I1,I2,_14,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I3);

            testPlaceHolder(I1,I2,I3,_14,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I4);

            testPlaceHolder(I1,I2,I3,I4,_14,I6,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I5);

            testPlaceHolder(I1,I2,I3,I4,I5,_14,I7,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I6);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,_14,I8,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I7);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,_14,I9,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I8);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,_14,I10,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I9);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,_14,I11,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I10);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,_14,I12,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I11);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,_14,I13,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I12);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,_14,I14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I13);

            testPlaceHolder(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,_14,
                            N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,N1,I14);
        }
      }

DEFINE_TEST_CASE(2) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concern:
        //   1. That the 'bslma::UsesBslmaAllocator' traits is correctly
        //      detected for 'bdlf::Bind' objects, in their two
        //      implementations.
        //   2. That the 'bslma::UsesBslmaAllocator' traits is correctly *NOT*
        //      detected for 'bdlf::BindWrapper' objects.
        //   3. That the 'bslmf::HasPointerSemantics' traits is correctly
        //      detected for 'bdlf::BindWrapper' objects.
        //
        // Plan:
        //   The test class 'bdlf::Bind', 'bdlf::Bind_Impl',
        //   'bdlf::Bind_ImplExplicit', and 'bdlf::BindWrapper' are templates,
        //   so it is sufficient to test for the expected traits with only one
        //   set of template parameters.
        //
        // Testing:
        //   TESTING BSLALG_DECLARE_NESTED_TRAITS
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        typedef NoAllocTestType *FUNC;
        typedef bdlf::Bind_Tuple1<PH1> ListType;

        if (verbose) printf("\tAsserting traits of 'bdlf::Bind'.\n");
        {
            ASSERT((bslma::UsesBslmaAllocator<
                             bdlf::Bind<bslmf::Nil, FUNC, ListType> >::value));
        }

        if (verbose) printf("\tAsserting traits of 'bdlf::BindWrapper'.\n");
        {
            ASSERT(!(bslma::UsesBslmaAllocator<
                      bdlf::BindWrapper<bslmf::Nil, FUNC, ListType> >::value));

            ASSERT((bslmf::HasPointerSemantics<
                      bdlf::BindWrapper<bslmf::Nil, FUNC, ListType> >::value));
        }
      }

DEFINE_TEST_CASE(1) {
        DECLARE_MAIN_VARIABLES
        // ------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace BDLF_BIND_BREATHING_TEST;

        if (verbose) printf("\tbindTest1.\n");
        bindTest1();
        if (verbose) printf("\tbindTest2.\n");
        bindTest2();
        if (verbose) printf("\tbindTest3.\n");
        bindTest3();
        if (verbose) printf("\tbindTest4.\n");
        bindTest4();
        if (verbose) printf("\tbindTest5.\n");
        bindTest5();
        if (verbose) printf("\tbindTest6.\n");
        bindTest6();

      }

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    globalVerbose = verbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                          \
      case NUMBER: {                                                          \
        testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose);              \
      } break
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
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
