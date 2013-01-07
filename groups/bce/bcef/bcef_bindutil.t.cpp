// bcef_bindutil.t.cpp                                                -*-C++-*-

#include <bcef_bindutil.h>

#include <bdef_bind.h>
#include <bdef_bind_test.h>
#include <bdef_function.h>
#include <bdef_placeholder.h>

#include <bslalg_typetraits.h>

#include <bslmf_nil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strcpy()
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The 'bcef_bindutil' component provides utility function that create a
// wrapper around a 'bdef_Bind' object.  'bdef_Bind' is a parameterized type
// for binding an "invocable" which is either a free function, a member
// function, or a function object (either by value or reference).  The binding
// functions produce a wrapper which is a shared pointer to a "binder" (that
// can be passed by value) that encapsulates both the function called (by
// address for free functions and function object pointers, and by owning a
// copy of the underlying object otherwise) and arguments passed to it (by
// value).  The concerns are similar to those of 'bdef_bind', and in addition
// we must make sure that the binder is created in a similar way (passing the
// correct arguments with the correct types, for both bound and invocation
// arguments).  However, we do not have any concerns as to the inner workings
// of the binder (forwarding of invocation parameters and place-holders) as
// those have been addressed by 'bdef_bind' already.
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TESTING BSLALG_DECLARE_NESTED_TRAITS
// [ 3] MIXING BOUND ARGUMENTS AND PLACEHOLDERS
// [ 4] PASSING 'bcef_bindwrapper' OBJECTS AS PARAMETERS
// [ 5] RESPECTING THE SIGNATURE OF THE INVOCABLE
// [ 6] USAGE EXAMPLE
// [ 6] MORE USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

static int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
        if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
        if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                            aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
        if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\n", #I, I, #J, J, #K, K); \
                            aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
        if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                      #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                     // current Line number
#define T_ printf("\t");                // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int globalVerbose = 0;

// The whole bdef_bind component currently works with up to 14 arguments.

const int BIND_MAX_ARGUMENTS = 14;
const int NUM_SLOTS = BIND_MAX_ARGUMENTS+1;

// Nil value (uninitialized).

const int N1 = -1;

// Typedefs for argument and test types.

typedef bdef_Bind_TestSlotsNoAlloc    SlotsNoAlloc;

typedef bdef_Bind_TestArgNoAlloc<1>   NoAllocTestArg1;
typedef bdef_Bind_TestArgNoAlloc<2>   NoAllocTestArg2;
typedef bdef_Bind_TestArgNoAlloc<3>   NoAllocTestArg3;
typedef bdef_Bind_TestArgNoAlloc<4>   NoAllocTestArg4;
typedef bdef_Bind_TestArgNoAlloc<5>   NoAllocTestArg5;
typedef bdef_Bind_TestArgNoAlloc<6>   NoAllocTestArg6;
typedef bdef_Bind_TestArgNoAlloc<7>   NoAllocTestArg7;
typedef bdef_Bind_TestArgNoAlloc<8>   NoAllocTestArg8;
typedef bdef_Bind_TestArgNoAlloc<9>   NoAllocTestArg9;
typedef bdef_Bind_TestArgNoAlloc<10>  NoAllocTestArg10;
typedef bdef_Bind_TestArgNoAlloc<11>  NoAllocTestArg11;
typedef bdef_Bind_TestArgNoAlloc<12>  NoAllocTestArg12;
typedef bdef_Bind_TestArgNoAlloc<13>  NoAllocTestArg13;
typedef bdef_Bind_TestArgNoAlloc<14>  NoAllocTestArg14;

typedef bdef_Bind_TestTypeNoAlloc     NoAllocTestType;

typedef bdef_Bind_TestSlotsAlloc      SlotsAlloc;

typedef bdef_Bind_TestArgAlloc<1>     AllocTestArg1;
typedef bdef_Bind_TestArgAlloc<2>     AllocTestArg2;
typedef bdef_Bind_TestArgAlloc<3>     AllocTestArg3;
typedef bdef_Bind_TestArgAlloc<4>     AllocTestArg4;
typedef bdef_Bind_TestArgAlloc<5>     AllocTestArg5;
typedef bdef_Bind_TestArgAlloc<6>     AllocTestArg6;
typedef bdef_Bind_TestArgAlloc<7>     AllocTestArg7;
typedef bdef_Bind_TestArgAlloc<8>     AllocTestArg8;
typedef bdef_Bind_TestArgAlloc<9>     AllocTestArg9;
typedef bdef_Bind_TestArgAlloc<10>    AllocTestArg10;
typedef bdef_Bind_TestArgAlloc<11>    AllocTestArg11;
typedef bdef_Bind_TestArgAlloc<12>    AllocTestArg12;
typedef bdef_Bind_TestArgAlloc<13>    AllocTestArg13;
typedef bdef_Bind_TestArgAlloc<14>    AllocTestArg14;

typedef bdef_Bind_TestTypeAlloc       AllocTestType;

// Placeholder types for the corresponding _1, _2, etc.

typedef bdef_PlaceHolder<1>  PH1;
typedef bdef_PlaceHolder<2>  PH2;
typedef bdef_PlaceHolder<3>  PH3;
typedef bdef_PlaceHolder<4>  PH4;
typedef bdef_PlaceHolder<5>  PH5;
typedef bdef_PlaceHolder<6>  PH6;
typedef bdef_PlaceHolder<7>  PH7;
typedef bdef_PlaceHolder<8>  PH8;
typedef bdef_PlaceHolder<9>  PH9;
typedef bdef_PlaceHolder<10> PH10;
typedef bdef_PlaceHolder<11> PH11;
typedef bdef_PlaceHolder<12> PH12;
typedef bdef_PlaceHolder<13> PH13;
typedef bdef_PlaceHolder<14> PH14;

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

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                 TESTING FUNCTIONS/CLASSES FOR CASE 5
//-----------------------------------------------------------------------------
extern "C" { // cannot be part of a namespace (namespace would be ignored)

    int myFunctionWithExternCLinkage(int x)
    {
        return x;
    }

}  // extern "C"

namespace BCEF_BIND_TEST_CASE_5 {

    int myFunctionWithConstnessMix(const int   a1,
                                   const int&  a2,
                                         int&  a3,
                                   const int*& a4)
    {
        // Note that this function can generate an explicit binder since the
        // signature is explicitly known at compile time.  This allows the
        // bound arguments to be stored with the actual type of the above
        // parameters.

        a3 = a1;
        a4 = &a2;
        return 1;
    }

    int myFunctionWithNoConstnessMix(int&  a1,
                                     int&  a2,
                                     int&  a3,
                                     const int*& a4)
    {
        // This function is a non-const-safe version of the above function,
        // which allows to take true references, since there is no mix between
        // lvalues and rvalues in the arguments.  This is to forgo our
        // limitation to the const forwarding problem.
        //

        a3 = a1;
        a4 = &a2;
        return 1;
    }

    int myFunctionWithVolatile(volatile int const&  a1,
                               volatile int        *a2)
    {
        *a2 = a1;
        return 1;
    }

    template <typename T>
    class ReferenceWrapper {
        T *d_ref;

      public:
        // CREATORS
        explicit ReferenceWrapper(T& t) : d_ref(&t) {}

        // ACCESSORS
        operator T&() const { return *d_ref; }
    };

    template <typename T>
    inline
    const ReferenceWrapper<T> ref(T& t)
    {
        return ReferenceWrapper<T>(t);
    }

    template <typename T>
    inline
    const ReferenceWrapper<const T> cref(const T& t)
    {
        return ReferenceWrapper<const T>(t);
    }

    struct MyFunctionObjectWithMultipleSignatures {
        // This stateless 'struct' provides several function operators with
        // different signatures, including one of them being a template, all
        // returning a different value.

        // TYPES
        typedef int ResultType;

        // MANIPULATORS
        int operator()(int x) const
        {
            return 1;
        }

        int operator()(const NoAllocTestArg1& x) const
        {
            return 2;
        }

        template <typename T>
        int operator()(const T& x) const
        {
            return 3;
        }
    };

    template <class Binder>
    void testMultipleSignatureBinder(Binder binder)
    {
        const NoAllocTestArg1 I1(1);

        ASSERT(1 == binder(1));
        ASSERT(2 == binder(I1));

        // The type 'int' is not an exact match for double and so the operator
        // template is preferred.

        ASSERT(3 == binder(1.0));
    }

}  // close namespace BCEF_BIND_TEST_CASE_5

//-----------------------------------------------------------------------------
//                 TESTING FUNCTIONS/CLASSES FOR CASE 4
//-----------------------------------------------------------------------------
namespace BCEF_BIND_TEST_CASE_4 {

#define BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS                                  \
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

int sumOf14Arguments( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
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

NoAllocTestArg1 selectArgument1( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a1.value(),  1);
    return a1;
}

NoAllocTestArg2 selectArgument2( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a2.value(),  2);
    return a2;
}

NoAllocTestArg3 selectArgument3( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a3.value(),  3);
    return a3;
}

NoAllocTestArg4 selectArgument4( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a4.value(),  4);
    return a4;
}

NoAllocTestArg5 selectArgument5( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a5.value(),  5);
    return a5;
}

NoAllocTestArg6 selectArgument6( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a6.value(),  6);
    return a6;
}

NoAllocTestArg7 selectArgument7( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a7.value(),  7);
    return a7;
}

NoAllocTestArg8 selectArgument8( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a8.value(),  8);
    return a8;
}

NoAllocTestArg9 selectArgument9( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a9.value(),  9);
    return a9;
}

NoAllocTestArg10 selectArgument10( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a10.value(),  10);
    return a10;
}

NoAllocTestArg11 selectArgument11( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a11.value(),  11);
    return a11;
}

NoAllocTestArg12 selectArgument12( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a12.value(),  12);
    return a12;
}

NoAllocTestArg13 selectArgument13( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a13.value(),  13);
    return a13;
}

NoAllocTestArg14 selectArgument14( BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS )
{
    SlotsNoAlloc::setSlot(a14.value(),  14);
    return a14;
}

#undef BCEF_BIND_TEST_NO_ALLOC_14_ARGUMENTS

#define BCEF_BIND_TEST_ALLOC_14_ARGUMENTS                                     \
                        AllocTestArg1  const& a1,  AllocTestArg2  const& a2,  \
                        AllocTestArg3  const& a3,  AllocTestArg4  const& a4,  \
                        AllocTestArg5  const& a5,  AllocTestArg6  const& a6,  \
                        AllocTestArg7  const& a7,  AllocTestArg8  const& a8,  \
                        AllocTestArg9  const& a9,  AllocTestArg10 const& a10, \
                        AllocTestArg11 const& a11, AllocTestArg12 const& a12, \
                        AllocTestArg13 const& a13, AllocTestArg14 const& a14

// Note that the return value is a 'const&' to avoid inadvertently triggering a
// copy that would use the default allocator.

AllocTestArg1 const& selectAllocArgument1(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a1.allocator(),  1);
    return a1;
}

AllocTestArg2 const& selectAllocArgument2(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a2.allocator(),  2);
    return a2;
}

AllocTestArg3 const& selectAllocArgument3(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a3.allocator(),  3);
    return a3;
}

AllocTestArg4 const& selectAllocArgument4(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a4.allocator(),  4);
    return a4;
}

AllocTestArg5 const& selectAllocArgument5(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a5.allocator(),  5);
    return a5;
}

AllocTestArg6 const& selectAllocArgument6(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a6.allocator(),  6);
    return a6;
}

AllocTestArg7 const& selectAllocArgument7(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a7.allocator(),  7);
    return a7;
}

AllocTestArg8 const& selectAllocArgument8(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a8.allocator(),  8);
    return a8;
}

AllocTestArg9 const& selectAllocArgument9(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a9.allocator(),  9);
    return a9;
}

AllocTestArg10 const& selectAllocArgument10(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a10.allocator(),  10);
    return a10;
}

AllocTestArg11 const& selectAllocArgument11(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a11.allocator(),  11);
    return a11;
}

AllocTestArg12 const& selectAllocArgument12(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a12.allocator(),  12);
    return a12;
}

AllocTestArg13 const& selectAllocArgument13(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a13.allocator(),  13);
    return a13;
}

AllocTestArg14 const& selectAllocArgument14(BCEF_BIND_TEST_ALLOC_14_ARGUMENTS)
{
    SlotsAlloc::setSlot(a14.allocator(),  14);
    return a14;
}

#undef BCEF_BIND_TEST_ALLOC_14_ARGUMENTS

}  // close namespace BCEF_BIND_TEST_CASE_4

//-----------------------------------------------------------------------------
//                 TESTING FUNCTIONS/CLASSES FOR CASE 3
//-----------------------------------------------------------------------------
namespace BCEF_BIND_TEST_CASE_3 {

template <// Types of bound arguments, could be I1-14, or placeholder.
          typename B1,  typename B2,  typename B3,  typename B4,  typename B5,
          typename B6,  typename B7,  typename B8,  typename B9,  typename B10,
          typename B11, typename B12, typename B13, typename B14,
          // Types of invocation arguments, could be I1-14, or (int)N1.
          typename A1,  typename A2,  typename A3,  typename A4,  typename A5,
          typename A6,  typename A7,  typename A8,  typename A9,  typename A10,
          typename A11, typename A12, typename A13, typename A14>
void testPlaceHolder(
        // Bound arguments follow.
        B1 b1,  B2 b2,  B3 b3,   B4  b4,  B5  b5,  B6  b6,  B7  b7,
        B8 b8,  B9 b9,  B10 b10, B11 b11, B12 b12, B13 b13, B14 b14,
        // Invocation argument follows.
        A1 a1,  A2 a2,  A3 a3,   A4  a4,  A5  a5,  A6  a6,  A7  a7,
        A8 a8,  A9 a9,  A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
{
    bslma_TestAllocator  ta0;
    bslma_Allocator    *Z0 = &ta0;

    // This test code is taken from case 2 of 'bdef_bind_test14', with the
    // checks for allocation removed.

          NoAllocTestType  mX;
    const NoAllocTestType& X = mX;
    const NoAllocTestType  EXPECTED(I1,I2,I3,I4,I5,I6,I7,
                                    I8,I9,I10,I11,I12,I13,I14);
    ASSERT( EXPECTED != X);

    ASSERT(14 == bcef_BindUtil::bind(Z0,
                                                   &mX, // invocable
        b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14) // bound arguments
     (a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)); // invocation arguments

    ASSERT(EXPECTED == X);
}

}  // close namespace BCEF_BIND_TEST_CASE_3

//=============================================================================
//                 BREATHING TEST CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------
namespace BCEF_BIND_BREATHING_TEST {

using namespace bdef_PlaceHolders;

///Elementary construction and usage of 'bcef_BindWrapper' objects
///---------------------------------------------------------------
// Bound objects are generally constructed by invoking the 'bcef_BindUtil' with
// an "invocation template".  An invocation template is a series of one or
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
    void bindTest(bslma_Allocator *allocator = 0) {
        bcef_BindUtil::bind(allocator,                 // allocator,
                            &invocable,                // bound object and
                            10, 14, (const char*)"p3") // bound arguments
//..
// and the binder declared above can be passed invocation arguments directly,
// as follows (here we specify zero invocation arguments since all the bound
// arguments are fully specified):
//..
                                                   (); // invocation
     }
//..
// In the function call above, the 'invocable' will be bound with the
// arguments '10', '14', and '"p3"' respectively, then invoked with those bound
// arguments.  In the next example, place-holders are used to forward
// user-provided arguments to the bound object.  We separate the invocation of
// the binder into a function template to avoid having to declare the type of
// the binder:
//..
    template <class BINDER>
    void callBinder(BINDER const& b)
    {
        b(10, 14);
    }
//..
// The creation of the binder is as follows:
//..
    void bindTest1(bslma_Allocator *allocator = 0) {
        callBinder(bcef_BindUtil::bind(allocator,
                                       &invocable,
                                       _1, _2, someString));
    }
//..
// In this code snippet, the 'callBinder' template function is invoked with a
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
    void bindTest2(bslma_Allocator *allocator = 0) {
        callBinder(bcef_BindUtil::bind(allocator,
                                       &invocable,
                                       _2, _1, someString));
    }
//..
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(14, 10, "p3");
//..
// The following snippet of code illustrates a number of ways to call
// 'bcef_BindUtil' and their respective output:
//..
    int test1(int i, int j) {
        return i + j;
    }

    int abs(int x) {
        return (x > 0) ? x : -x;
    }

    void bindTest3(bslma_Allocator *allocator = 0) {
        using namespace bdef_PlaceHolders;
        ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, _1, _2)(10, 14) );
        ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, _1, 14)(10) );
        ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, 10, _1 )(14) );
        ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, 10, 14)() );
        ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1,
                          bcef_BindUtil::bind(allocator, &abs, _1), 14)(-10) );
    }
//..
// The usage example below provides a more comprehensive series of calling
// sequences.
//
///Binding Data
///------------
// The main use of 'bdef_Bind' is to invoke bound objects with additional data
// that is not specified by the caller.  For that purpose, place-holders are
// key.  There are a couple of issues to understand in order to properly use
// this component.  The bound arguments must be of a value-semantic type
// (unless they are place-holders or 'bdef_Bind' objects).  They are evaluated
// at binding time once and only once and their value copied into the binder
// (using the default allocator to supply memory unless an allocator is
// specified).  A 'bdef_Bind' object always invokes its bound object with only
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

    void bindTest4(bslma_Allocator *allocator = 0) {
        marker = 0;
        callBinderWithSideEffects1(bcef_BindUtil::bind(allocator,
                                                       &singleArgumentFunction,
                                                       _2));
//..
// In the above snippet of code, 'singleArgumentFunction' will be called with
// only the second argument (14) specified to the binder at invocation time in
// the 'callBinderWithSideEffects1' function.  Thus the return value of the
// invocation must be 14.  The 'identityFunctionWithSideEffects(10)' will be
// evaluated, even though its return value (10) will be discarded.  We can
// check this as follows:
//..
        LOOP_ASSERT(marker, 10 == marker);
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

    void bindTest5(bslma_Allocator *allocator = 0) {
        marker = 0;
        callBinderWithSideEffects2(bcef_BindUtil::bind(allocator,
                                                       &doubleArgumentFunction,
                                                       _1, _1));
//..
// In the above snippet of code, 'doubleArgumentFunction' will be called with
// the first argument ('identityFunctionWithSideEffects(10)') specified to
// the binder, computed only once at invocation time.  We can check this as
// follows:
//..
        LOOP_ASSERT(marker, 10 == marker);
    }
//..
// The return value (10) will, however, be copied twice before being passed to
// the 'doubleArgumentFunction' which will return their sum (20).
//
///Bound objects
///-------------
// There are a few issues to be aware of concerning the kind of bound objects
// that can successfully be used with this component.  The issues are identical
// to 'bdef_bind', in fact the invocable object is forwarded in the same manner
// to 'bdef_bind', and so there no additional issues and we refer the reader to
// the 'bdef_bind' component for a treatment of bound objects.
//
///Binding with allocators
///-----------------------
// The copy of the bound object and arguments are created as members of the
// 'bdef_Bind' object, so no memory is allocated for those.  When the bound
// object or arguments use memory allocation, however, that memory is supplied
// by the default allocator unless 'bcef_BindUtil::bindA' is used to specify
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
        bslma_Allocator *d_allocator_p;
        char            *d_string_p;

      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(MyString,
                                           bslalg_TypeTraitUsesBslmaAllocator);

        //CREATORS
        MyString(const char *str, bslma_Allocator *allocator = 0)
        : d_allocator_p(bslma_Default::allocator(allocator))
        , d_string_p((char*)d_allocator_p->allocate(strlen(str)+1))
        {
            strcpy(d_string_p, str);
        }

        MyString(MyString const& rhs, bslma_Allocator *allocator = 0)
        : d_allocator_p(bslma_Default::allocator(allocator))
        , d_string_p((char*)d_allocator_p->allocate(strlen(rhs)+1))
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
// We will also use a 'bslma_TestAllocator' to keep track of the memory
// allocated:
//..
    void bindTest6() {
        bslma_TestAllocator allocator;
        MyString myString((const char*)"p3", &allocator);
        const int NUM_ALLOCS = allocator.numAllocations();
//..
// To expose that the default allocator is not used, we will use a default
// allocator guard, which will re-route any default allocation to the
// 'defaultAllocator':
//..
        bslma_TestAllocator defaultAllocator;
        bslma_DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
        const int NUM_DEFAULT_ALLOCS = defaultAllocator.numAllocations();
//..
// We now create a binder object with allocator using 'bindA'.  If the bound
// object were an instance of a class taking an allocator, then 'allocator'
// would be passed to its copy constructor; in this case, 'allocator' will be
// ignored.  But 'allocator' *will* be used to make the copy of 'myString' held
// by the binder:
//..
        callBinder(bcef_BindUtil::bind(&allocator,
                                       &invocable,
                                       _1, _2, myString));
//..
// We now check that memory was allocated from the test allocator, and none
// from the default allocator:
//..
        ASSERT(NUM_ALLOCS != allocator.numAllocations());
        ASSERT(NUM_DEFAULT_ALLOCS == defaultAllocator.numAllocations());
    }
//..
}  // close namespace BCEF_BIND_BREATHING_TEST

//=============================================================================
//                 USAGE EXAMPLE CLASSES AND FUNCTIONS
//-----------------------------------------------------------------------------
namespace BCEF_BIND_USAGE_EXAMPLE {

using namespace bdef_PlaceHolders;

///Usage
///-----
// What follows is a series of code snippets illustrating detailed aspects of
// typical usage of 'bcef_BindUtil'.  For these examples, we will use a typical
// pair of event and scheduler classes, where the event is defined as:
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
        bdef_Function<void (*)(int, MyEvent)>  d_callback;
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
        MyEventScheduler(bdef_Function<void(*)(int, MyEvent)> const& callback)
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
    void myCallback(int result, MyEvent const& event)
    {
        // Do something ...
    }

    void myMainLoop(bslma_Allocator *allocator = 0)
    {
        MyEventScheduler sched(bcef_BindUtil::bind(allocator,
                                                   &myCallback, _1, _2));
        sched.run(10);
    }
//..
// Next we show how to bind some of the callback arguments at binding time,
// while letting the invocation arguments straight through to the callback as
// the first two arguments:
//..
    void myCallbackWithUserArgs(int            result,
                                MyEvent const& event,
                                int            userArg1,
                                double         userArg2)
    {
        // Do something ...
    }

    void myMainLoop2(bslma_Allocator *allocator = 0)
    {
        MyEventScheduler sched(bcef_BindUtil::bind(allocator,
                                                   &myCallbackWithUserArgs,
                                                   _1, _2, 360, 3.14));
        sched.run(10);
    }
//..
// In the next snippet of code, we show how to reorder the invocation arguments
// before they are passed to the callback:
//..
    void myCallbackWithUserArgsReordered(int            result,
                                         int            userArg1,
                                         double         userArg2,
                                         MyEvent const& event)
    {
        // Do something ...
    }

    void myMainLoop3(bslma_Allocator *allocator = 0)
    {
        MyEventScheduler sched(bcef_BindUtil::bind(allocator,
                        &myCallbackWithUserArgsReordered, _1, 360, 3.14, _2));
        sched.run(10);
    }
//..
// And finally, we illustrate that the signature of the callback can be
// *smaller* than expected by the scheduler by letting the binder ignore its
// first argument:
//..
    void myCallbackThatDiscardsResult(MyEvent const& event)
    {
        // Do something ...
    }

    void myMainLoop4(bslma_Allocator *allocator = 0)
    {
        MyEventScheduler sched(bcef_BindUtil::bind(allocator,
                                          &myCallbackThatDiscardsResult, _2));
        sched.run(10);
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

    void myMainLoop5(bslma_Allocator *allocator = 0)
    {
        MyCallbackObject obj;
        MyEventScheduler sched(bcef_BindUtil::bind(allocator, obj, _1, _2));
        sched.run(10);
    }
//..
///Binding to Function Objects by Reference
/// - - - - - - - - - - - - - - - - - - - -
// The following example reuses the 'MyCallbackObject' of the previous example,
// but illustrates that it can be passed by reference as well as by value:
//..
    void myMainLoop6(bslma_Allocator *allocator = 0)
    {
        MyCallbackObject obj;
        MyEventScheduler sched(bcef_BindUtil::bind(allocator, &obj, _1, _2));
        sched.run(10);
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
        void callback(int, MyEvent const& event)
        {
            // Do something that may modify the state info...
        }
    };

    void myMainLoop7(bslma_Allocator *allocator = 0)
    {
        MyStatefulObject obj;
        MyEventScheduler sched(bcef_BindUtil::bind(allocator,
                                  &MyStatefulObject::callback, &obj, _1, _2));
        sched.run(10);
    }
//..
///Nesting Bindings
/// - - - - - - - -
// We now show that it is possible to provide a binder as an argument to
// 'bcef_BindUtil'.  Upon invocation, the invocation arguments are forwarded to
// the nested binder.
//..
    MyEvent annotateEvent(int, MyEvent const& event) {
        // Do something to 'event' ...
        return event;
    }

    void myMainLoop8(bslma_Allocator *allocator = 0)
    {
        MyCallbackObject obj;
        MyEventScheduler sched(
                bcef_BindUtil::bind(allocator, &obj, _1,
                      bcef_BindUtil::bind(allocator, &annotateEvent, _1, _2)));
        sched.run(10);
    }
//..
///Binding to a Function Object with Explicit Return Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the return type cannot be inferred from the bound object (using
// 'FUNC::ResultType'), the binder needs an explicitly specification.  This is
// done by using the 'bcef_BindUtil::bindR' function template as exemplified
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

    void myMainLoop9(bslma_Allocator *allocator = 0)
    {
        MyCallbackObjectWithoutResultType obj;
        MyEventScheduler sched(bcef_BindUtil::bindR<GlobalResultType>(
                                                      allocator, obj, _1, _2));
        sched.run(10);
    }
//..
// Another situation where the return type (in fact, the whole signature)
// cannot be inferred from the bound object is the use of the free function
// with C linkage and variable number of arguments 'printf(const char*, ...)'.
// In the following code snippet, we show how the argument to the 'callBinder'
// function of section "Elementary construction and usage of 'bdef_Bind'
// objects" above can be bound to 'printf':
//..
    void bindTest7(bslma_Allocator *allocator = 0)
    {
        using namespace BCEF_BIND_BREATHING_TEST; // for testing only
        const char* formatString = "Here it is: %d %d\n";
#if 0
        callBinder(bcef_BindUtil::bindR<int>(allocator,
                                             &printf, formatString, _1, _2));
#endif
    }
//..
// When called, 'bindTest7' will create a binder, pass it to 'callBinder' which
// will invoke it with arguments '10' and '14', and the output will be:
//..
//  Here it is: 10 14
//..

} // closing namespace BCEF_BIND_USAGE_EXAMPLE
//=============================================================================
//                 USAGE EXAMPLE FROM OTHER TEST DRIVERS FUNCTIONS
//-----------------------------------------------------------------------------
namespace BCEF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS {

int onMasterCommand(const int&  prefix,
                    int&        stream,
                    int        *counter = 0)
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

} // closing namespace BCEF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    globalVerbose = verbose;
    (void) veryVerbose;      // kill warning about unused variable
    (void) veryVeryVerbose;  // kill warning about unused variable

    // The following machinery is for use in conjunction with the
    // 'SlotsNoAlloc::resetSlots' and 'SlotsNoAlloc::verifySlots' functions.
    // The slots are set when the corresponding function object or free
    // function is called with the appropriate number of arguments.

    const int NO_ALLOC_SLOTS[NUM_SLOTS] = {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
          -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    };
    const int NO_ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };

    // Values that do not take an allocator are declared 'static const' above.

    // The following machinery is for use in conjunction with the
    // 'SlotsAlloc::resetSlots' and 'SlotsAlloc::verifySlots' functions.
    // The slots are set when the corresponding function object or free
    // function is called the appropriate number of arguments.

    bslma_TestAllocator allocator0(veryVeryVerbose);
    bslma_TestAllocator allocator1(veryVeryVerbose);
    bslma_TestAllocator allocator2(veryVeryVerbose);

    bslma_TestAllocator *Z0 = &allocator0;
    bslma_TestAllocator *Z1 = &allocator1;
    bslma_TestAllocator *Z2 = &allocator2;

    bslma_DefaultAllocatorGuard allocGuard(Z0);
    SlotsAlloc::setZ0(Z0);
    SlotsAlloc::setZ1(Z1);
    SlotsAlloc::setZ2(Z2);

    const bslma_Allocator *ALLOC_SLOTS[NUM_SLOTS] = {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
          Z0, Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2,
    };
    const bslma_Allocator *ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14
          Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0,
    };

    // Values that do take an allocator (default allocator is used, which means
    // we must perform the initialization *after* the allocator guard, hence in
    // main).

    const AllocTestArg1  V1(1),     NV1(-1);
    const AllocTestArg2  V2(20),    NV2(-20);
    const AllocTestArg3  V3(23),    NV3(-23);
    const AllocTestArg4  V4(44),    NV4(-44);
    const AllocTestArg5  V5(66),    NV5(-66);
    const AllocTestArg6  V6(176),   NV6(-176);
    const AllocTestArg7  V7(878),   NV7(-878);
    const AllocTestArg8  V8(8),     NV8(-8);
    const AllocTestArg9  V9(912),   NV9(-912);
    const AllocTestArg10 V10(102),  NV10(-120);
    const AllocTestArg11 V11(111),  NV11(-111);
    const AllocTestArg12 V12(333),  NV12(-333);
    const AllocTestArg13 V13(712),  NV13(-712);
    const AllocTestArg14 V14(1414), NV14(-1414);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // ------------------------------------------------------------------
        // TESTING USAGE EXAMPLE FROM TEST DRIVERS
        //   Some usage examples in other package groups do not compile on AIX
        //   with xlC.  This test case is placed here for regression.
        //
        // Plan:
        //   Incorporate usage example from other test drivers.
        //
        // Testing:
        //   USAGE EXAMPLE FROM TEST DRIVERS
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTesting Usage Example from Other Test Drivers"
                   "\n=============================================\n");

        using namespace BCEF_BIND_USAGE_EXAMPLE_FROM_OTHER_TEST_DRIVERS;
        using namespace bdef_PlaceHolders;

        {
            int extra = 0;
            typedef bdef_Bind_BoundTuple3<bdef_PlaceHolder<1>,
                                          bdef_PlaceHolder<2>,
                                          int*> LIST;
            typedef int (*FUNC)(const int &, int &, int *);
            bcef_BindWrapper<bslmf_Nil,FUNC,LIST> b(onMasterCommand,
                                                    LIST(_1, _2, &extra), Z0);

            int stream, prefix;
            ASSERT(0 == bcef_BindUtil::bind(Z0,
                            &onMasterCommand, _1, _2, &extra)(prefix, stream));
            ASSERT(1 == extra);
            ASSERT(1 == stream);
            ASSERT(0 == bcef_BindUtil::bind(Z0,
                            &onMasterCommand, _1, _2, &extra)(prefix, stream));
            ASSERT(2 == extra);
            ASSERT(2 == stream);
        }

        {
            // Failure to copy MyInt by value (the signature of 'enqueueJob'
            // takes a 'const&') will trigger destruction of the MyInt object.

            bdef_Function<void(*)(void)> job;

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159);
                job = bcef_BindUtil::bind(Z0, &enqueuedJob, i);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copy of 'myInt', make sure
                   // that the argument passed is 314159 (by value) and not 0
                   // (as would be if 'i' was stored by address).

            { // scope for 'MyInt i' (initialized at 314159)
                MyInt i(314159), j(1618034);
                job = bcef_BindUtil::bind(Z0, &enqueuedJob2, i, j);
            } // closing scope destroys the MyInt, resetting it to 0

            job(); // invoke 'enqueuedJob' with its copies of 'myInt', make
                   // sure that the argument passed is 314159 (by value) and
                   // not 0 (as would be if 'i' was stored by address).
        }

      } break;
      case 6: {
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

        using namespace BCEF_BIND_USAGE_EXAMPLE;

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

      } break;
      case 5: {
        // ------------------------------------------------------------------
        // RESPECTING THE SIGNATURE OF THE INVOCABLE
        // Concerns:
        //   1. That we can bind functions with extern "C" linkage.
        //   2. That we forward reference types correctly.
        //   3. That we respect the constness of arguments (within the
        //   limitations imposed by our handling of the forwarding problem).
        //   4. That we correctly respect volatile qualifications.  This is a
        //   concern because the forwarding type invokes 'bslmf_RemoveCvq'.
        //   5. That we can bind a function object with placeholders, and later
        //   invoke the binder resolving to two different overloads based on
        //   the type of the invocation arguments.
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
        //
        // Testing:
        //   RESPECTING THE SIGNATURE OF THE INVOCABLE
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING RESPECTING THE SIGNATURE OF THE INVOCABLE"
                   "\n=================================================\n");

        using namespace BCEF_BIND_TEST_CASE_5;
        using namespace bdef_PlaceHolders;

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // xlC compiler on AIX does not distinguish between the manglings of
        // extern "C" and C++ linkages.

        if (verbose) printf("\tRespecting extern \"C\" linkage.\n");
        {
            const int UNIQUE_INT = 12345678;

                // by reference, with no placeholder
            ASSERT(UNIQUE_INT == bcef_BindUtil::bind(Z0,
                                  myFunctionWithExternCLinkage, UNIQUE_INT)());

                // by reference, with placeholder
            ASSERT(UNIQUE_INT == bcef_BindUtil::bind(Z0,
                                myFunctionWithExternCLinkage, _1)(UNIQUE_INT));

                // by address, with no placeholder
            ASSERT(UNIQUE_INT == bcef_BindUtil::bind(Z0,
                                 &myFunctionWithExternCLinkage, UNIQUE_INT)());

                // by address, with placeholder
            ASSERT(UNIQUE_INT == bcef_BindUtil::bind(Z0,
                               &myFunctionWithExternCLinkage, _1)(UNIQUE_INT));
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

            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithNoConstnessMix,
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
            //  ASSERT(1 == bcef_BindUtil::bind(&myFunctionWithConstnessMix,
            //                        X1, X2, mX3, mX4) // bound arguments
            //                                     ()); // invocation arguments
            //..
            // Instead, we must explicitly construct references with a wrapper,
            // so as to pass them by value as in the following:

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithConstnessMix,
                      X1, cref(X2), ref(mX3), ref(mX4)) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1  == mX3);
            ASSERT(&X2 == mX4); // 'mX4' should hold the address of the
                                // binder's copy of 'X2', not of 'X2' itself.

            // Note that the wrapper for 'X2' is necessary, even though X2 is
            // taken by 'const&' since what will be passed to the invocable
            // will be the copy of 'X2' in the binder, not 'X2' itself.

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithConstnessMix,
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
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithConstnessMix,
                                        _1, _2, _3, _4) // bound arguments
                                   (X1, X2, mX3, mX4)); // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 != mX4); // Will pass 'X2' by value since the 'const&'
                                // will be removed by the forwarding type.

#if !defined(BSLS_PLATFORM_CMP_IBM)
            // 'bdef_Bind_FuncTraitsHasNoEllipsis' will return 0 even for
            // functions that does not have an ellipsis in its function
            // signature.  When this happens, implicit version of 'bdef_Bind'
            // will be used, and X2 will be forwarded (thanks to the wrapper)
            // instead of copied.  Therefore, the following assert will fail:
            //..
            //  ASSERT(&X2 != mX4);
            //..

            mX3 = 3;
            mX4 = &X1;
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithConstnessMix,
                                        _1, _2, _3, _4) // bound arguments
                        (X1, cref(X2), mX3, ref(mX4))); // invocation arguments

            ASSERT(X1  == mX3);
            ASSERT(&X2 != mX4); // Even though we wrapped X2 using the const
                                // reference wrapper, what's being passed will
                                // still be copied because the bind is
                                // explicit.  When a bind is explicit, the
                                // component will call 'operator(...)' using
                                // the most efficient forwarding types (based
                                // 'bslmf_ForwardingTypes').  By doing so, the
                                // second argument will be mapped to an 'int',
                                // which means the value will be copied.
#endif

            // With a general (non-explicit) binder, signature is not detected
            // at compile time due to presence of duplicate placeholder.  And
            // so the reference invocation arguments must be wrapped again:

            mX3 = 3;
            mX4 = &mX1;
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithConstnessMix,
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
            // ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithVolatile,
            //                                X1, &mX2) // bound arguments
            //                                     ()); // invocation arguments
            //..
            // (Namely, 'p1', etc. are taken 'P1 const&' in
            // 'bcef_BindUtil::bind' but are stored as 'P1' in the bound
            // tuple).  This is a bug, but there is a little to do about it
            // without much effort... and the rare frequency of volatile in
            // binders does not warrant the effort to fix it.  In addition,
            // there is a simple fix, which is to wrap the rvalue reference:

            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithVolatile,
                                        cref(X1), &mX2) // bound arguments
                                                   ()); // invocation arguments
            ASSERT(X1 == X2);

            // With an explicit binder, the signature is known at compile time.
            // The invocation arguments respect the volatile qualifier.

            mX1 = 1;
            ASSERT(1 == bcef_BindUtil::bind(Z0, &myFunctionWithVolatile,
                                                _1, _2) // bound arguments
                                           (X1, &mX2)); // invocation arguments
            ASSERT(X1 == X2);
        }

        if (verbose)
            printf("\tAccepting multiple signatures\n");
        {
            MyFunctionObjectWithMultipleSignatures mX;

            testMultipleSignatureBinder(bcef_BindUtil::bind(Z0, mX, _1));
        }

      } break;
      case 4: {
        // ------------------------------------------------------------------
        // PASSING 'bcef_BindWrapper' OBJECTS AS PARAMETERS
        // Concerns:
        //   That 'bcef_BindWrapper' objects can be passed as bound arguments,
        //   and that the invocation arguments are properly passed to the
        //   place-holders of the nested 'bdef_Bind' objects.  We want this to
        //   work on an arbitrary number of levels of recursion.
        //
        // Plan:
        //   The 'bdef_bind' test driver already established that all arguments
        //   are passed properly and evaluated in the same manner no matter
        //   where their position.  So it suffices to nest binders with three
        //   levels of recursion (enough to guarantee that recursion at any
        //   level will work) and again check that return value is correct.
        //
        // Testing:
        //   PASSING 'bcef_BindWrapper' OBJECTS AS PARAMETERS
        // ------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING PASSING 'bdef_bind' OBJECTS AS PARAMETERS"
                   "\n=================================================\n");

        using namespace BCEF_BIND_TEST_CASE_4;
        using namespace bdef_PlaceHolders;

        // The binders below hold their own copies of 'mX' (which uses 'Z1').
        // The copy of 'mX' should use the propagated allocator 'Z2', and thus
        // there should be *no* allocation taking place with 'Z1'.  The nested
        // binder (which uses 'Z0') has pointer semantics and thus is not
        // copied.  Each bind wrapper is going to use one allocation for the
        // shared pointer and the shared binder instance, since shared pointer
        // is created in place.

        if (verbose)
         printf("\tPropagating invocation arguments to nested binders.\n");
        {
                  NoAllocTestType  mX;
            const NoAllocTestType& X = mX;

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS_DEFAULT,
                                             veryVerbose));

            ASSERT(14 == bcef_BindUtil::bind(Z0, mX,
                              // first bound argument below
                              bcef_BindUtil::bind(Z0, &selectArgument1,
                                  _1, _2, _3, _4, _5, _6, _7, _8, _9,
                                  _10, _11, _12, _13, _14),
                              // second to thirteenth bound arguments below
                              _2, _3, _4, _5, _6, _7, _8, _9,
                              _10, _11, _12, _13, _14)
                                       // invocation arguments follow
                                       (I1, I2, I3, I4, I5, I6, I7, I8, I9,
                                       I10, I11, I12, I13, I14));

            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS, veryVerbose));
            if (veryVerbose) { printf("%d: X: ", L_); X.print(); }
        }

        if (verbose)
            printf("\tPropagating allocators to nested binders.\n");
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

            ASSERT(14 == bcef_BindUtil::bind(Z2, mX,
                             // first bound argument below
                             bcef_BindUtil::bind(Z0, &selectAllocArgument1,
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
            // 'Z0'.  There should be 14 allocations for the nested
            // binder's bound argument plus another for the shared pointer
            // and the nested binder instance, plus 14 for
            // the temporary copy of 'mX' and another 14 for the temporary
            // copies of the nested binder's bound arguments.

            const int NUM_DEFAULT_ALLOCS_BEFORE = Z0->numAllocations();
            const int NUM_ALLOCS_BEFORE = Z1->numAllocations();

            ASSERT(14 == bcef_BindUtil::bind(Z2, mX,
                              // first bound argument below
                              bcef_BindUtil::bind(Z0, &selectAllocArgument1,
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
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS, veryVerbose));

            const int NUM_ALLOCS = Z1->numAllocations() - NUM_ALLOCS_BEFORE;
            LOOP_ASSERT(NUM_ALLOCS, 0 == NUM_ALLOCS);
        }

        if (verbose) printf("\tArbitrarily deeply nested binders.\n");
        {
            ASSERT(I1 == bcef_BindUtil::bind(Z0, &selectArgument1,
                             // first bound argument, nested level 1
                             bcef_BindUtil::bind(Z0, &selectArgument1,
                                 // first bound argument, nested level 2
                                 bcef_BindUtil::bind(Z0, &selectArgument1,
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

      } break;
      case 3: {
        // ------------------------------------------------------------------
        // MIXING BOUND ARGUMENTS AND PLACEHOLDERS
        // Concern:
        //   1. That the bound arguments are forwarded properly to the
        //   constructor of the binder by the 'bcef_BindUtil::bind' methods.
        //   2. The the invocation arguments are forwarded properly to the
        //   invocation method of the 'bcef_BindWrapper' object.
        //
        // Plan:
        //   Replicate some of the test cases from the 'bdef_bind'
        //   component with different forwarding strategies.  Because the
        //   mechanism is the same (template instantiation of the same code)
        //   for different invocables, it suffices to test with a function
        //   object passed by address (to keep things as simple as possible),
        //   and since we have ascertained that the 'bdef_BindUtil' factory
        //   methods forward the arguments properly, it suffices to make sure
        //   that the 'bcef_BindUtil' forward their arguments properly to the
        //   'bdef_BindUtil' factory methods.  We use the following
        //   simple selection strategy:
        //   1. permute the order of the 14 placeholders (and the invocation
        //      arguments accordingly) in either a cyclic or reverse order.
        //   2. use a single placeholder (e.g., _1) in each position with
        //      the remaining arguments bound to their respective values.
        //
        // Testing:
        //   TESTING MIXING BOUND ARGUMENTS AND PLACEHOLDERS
        // ------------------------------------------------------------------

        if (verbose) printf("\nMIXING BOUND ARGUMENTS AND PLACEHOLDERS"
                            "\n=======================================\n");

        using namespace BCEF_BIND_TEST_CASE_3;
        using namespace bdef_PlaceHolders;

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

        // The testing here is not that all placeholders are forwarded to their
        // correct position at any place, it's here only to make sure that we
        // did not mess up any argument in the copy-paste of 'bcef_bindutil',
        // so once correctness is established for _1 in every position, our
        // concern regarding bound arguments is addressed for the parameters of
        // 'bcef_BindUtil::bind'.  The next sequence establishes correctness
        // for all the invocation arguments of the wrapper.

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

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TESTING TRAITS
        // Concern:
        //   1. that the 'bslalg_TypeTraitUsesBslmaAllocator' traits is
        //   correctly *NOT* detected for 'bcef_BindWrapper' objects.
        //   2. that the 'bslalg_TypeTraitHasPointerSemantics' traits is
        //   correctly detected for 'bcef_BindWrapper' objects.
        //
        // Testing:
        //   TESTING BSLALG_DECLARE_NESTED_TRAITS
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        if (verbose) printf("\tAsserting traits of 'bcef_BindWrapper'.\n");
        {
            typedef NoAllocTestType *FUNC;
            typedef bdef_Bind_Tuple1<PH1> ListType;

            ASSERT(0 == (bslalg_HasTrait<bcef_BindWrapper<bslmf_Nil,
                                                          FUNC,
                                                          ListType>,
                                  bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

            ASSERT(1 == (bslalg_HasTrait<bcef_BindWrapper<bslmf_Nil,
                                                          FUNC,
                                                          ListType>,
                                 bslalg_TypeTraitHasPointerSemantics>::VALUE));
        }

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // BREATHING TEST
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        using namespace BCEF_BIND_BREATHING_TEST;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
