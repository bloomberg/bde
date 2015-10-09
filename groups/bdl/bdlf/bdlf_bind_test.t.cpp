// bdlf_bind_test.t.cpp                                               -*-C++-*-
#include <bdlf_bind_test.h>

#include <bsls_bsltestutil.h>

#include <bdlf_placeholder.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_nil.h>

#include <bsls_platform.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()

using namespace BloombergLP;
using namespace bsl;

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
// The 'bdlf_bind' is a large component by the number of lines of code, and
// even more so since it is a template and has at least *four* orthogonal
// dimensions: the family of factory methods ('bind', bindA', or 'bindR'), the
// nature of the bound object (free function pointer or reference, member
// function pointer, and function object by reference or by value), the number
// of arguments (from 0 up to 14 for function objects, and 0 up to 13 for free
// and member functions since the first argument is consumed by the test object
// pointer), and the invocation (any of the up to 2^14 possible combinations of
// placeholders or bound arguments).  In addition, in order to test the memory
// allocation model, we must use test classes that do and that do not allocate
// memory.
//
// Instead of restricting the test coverage, we split the testing of this
// component into 14 pieces.  This test component provides a testing apparatus
// for 'bdlf_bind'.  Our main concerns are that the apparatus works as intended
// for tracking the forwarding of parameters in the binders, the number of
// memory allocations and the forwarding of allocators for allocated objects,
// etc.  Although the 'bdlf::Bind_TestType*' classes have value semantics, a
// full value-semantic test driver is an overkill here.
//-----------------------------------------------------------------------------
// [ 1] TESTING HELPER FUNCTIONS/CLASSES
// [ 2] TESTING BSLALG_DECLARE_NESTED_TRAITS
// [ 3] TESTING USAGE EXAMPLE
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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64 Int64;

typedef bdlf::Bind_TestTypeNoAlloc  NoAllocTestType;
typedef bdlf::Bind_TestTypeAlloc    AllocTestType;

typedef bdlf::Bind_TestSlotsNoAlloc SlotsNoAlloc;
typedef bdlf::Bind_TestSlotsAlloc   SlotsAlloc;

// The whole bdlf_bind component currently works with up to 14 arguments.

const int BIND_MAX_ARGUMENTS = 14;
const int NUM_SLOTS = BIND_MAX_ARGUMENTS+1;

// Nil value (uninitialized).
const int N1 = -1;

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

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// The usage example requires 'bdlf_bind' but we want to explicitly avoid a
// dependency on 'bdlf_bind' since this component will be used to test
// 'bdlf_bind'.  So we provide a poor man's binder solely for the purpose of
// compiling the usage example.  Note that the usage example is actually tested
// with a real 'Bind_Impl' object in the test driver of 'Bind_Impl', with 0 up
// to 14 parameters.  There is thus no reason to do anything more than just the
// bare bones here.

namespace BloombergLP {

    template <class RET, class FUNC, class LIST>
    struct Bind_Impl
    {
        // This struct implements a binder which is created from a pointer to a
        // function object and a list of bound arguments which must necessarily
        // be a 'bdlf::Bind_Tuple1' instance containing a single bound
        // argument.  If the binder is invoked with no arguments, the bound
        // argument is used.  If the binder is invoked with one argument, it is
        // passed to the function object; the bound argument is assumed to be a
        // place-holder and discarded.
        //
        // *NOTE THAT* this binder is *incomplete* and barely functional enough
        // for the usage example.  It does not implement the proper allocator
        // model.  It only works with return types that are integer.  It does
        // not even bind properly if invoked with one argument and created with
        // a bound argument that is not a placeholder.  This travesty of a
        // 'Bind_Impl' implementation really must not be used for anything
        // else!

        // PRIVATE INSTANCE DATA
        FUNC  d_func;
        int   d_arg;

      public:
        // CREATORS
        Bind_Impl(FUNC               func,
                  LIST const&        list,
                  bslma::Allocator * = 0)
            // Create a 'Bind_Impl' object that is bound to the specified
            // 'func' invocable object.
        : d_func(func)
        , d_arg(list.value())
        {
        }

        Bind_Impl(const Bind_Impl& other, bslma::Allocator * = 0)
            // Create a 'Bind_Impl' object that is bound to the same invocable
            // object with the same bound parameters as the specified 'other',
            // using the optionally specified 'allocator' to supply memory.
        : d_func(other.d_func), d_arg(other.d_arg)
        {
        }

        // MANIPULATORS
        int operator()()
            // Invoke the modifiable bound object using the invocation template
            // provided at construction of this 'Bind_Impl' object, and return
            // the result.
        {
            return (*d_func)(d_arg);
        }

        template <class P1>
        int operator()(P1 const& p1)
            // Invoke the modifiable bound object using the invocation template
            // provided at construction of this 'Bind_Impl' object,
            // substituting place-holders for argument 1 with the value of the
            // specified un-modifiable argument 'p1'.  Return the result.
        {
            return (*d_func)(p1.value());
        }

        int operator()(int p1)
            // Invoke the modifiable bound object using the invocation template
            // provided at construction of this 'Bind_Impl' object,
            // substituting place-holders for argument 1 with the value of the
            // specified modifiable argument 'p1'.  Return the result.
        {
            return (*d_func)(p1);
        }
    };

namespace bdlf {
    template <class T>
    class Bind_Tuple1
    {
        // This 'class' holds an integer argument.

        // INSTANCE DATA
        int d_a1;

      public:
        // CREATORS
        inline Bind_Tuple1(T const& a1)
            // Create an instance storing the specified value 'a1'.
        : d_a1(a1.value())
        {}

        // ACCESSORS
        int value() const
        {
            return d_a1;
        }
    };

    template <>
    class Bind_Tuple1<PH1>
    {
        // This 'class' is a place-holder that stores nothing.  Its 'value'
        // method should never even be called, but in order to simplify the
        // logic in the binder, we let it return 0.

      public:
        // CREATORS
        inline Bind_Tuple1(PH1 const&)
            // Create an instance.
        {}

        // ACCESSORS
        int value() const
        {
            return 0;
        }
    };
}  // close package namespace


}  // close enterprise namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
namespace BDEF_BIND_TEST_USAGE_EXAMPLE {

///Testing 'Bind_Impl' *without* allocators
///- - - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdlf::Bind_TestTypeNoAlloc' invocable by a
// 'Bind_Impl' object constructed with and without placeholders.
//..
    void usageExampleNoAlloc(int veryVeryVerbose)
    {
        bslma::TestAllocator          allocator0(veryVeryVerbose);
        bslma::TestAllocator         *Z0 = &allocator0;
        bslma::DefaultAllocatorGuard  allocGuard(Z0);

        const int                         N1 = -1;
        const bdlf::Bind_TestArgNoAlloc<1> I1 = 1;

        // 1 argument to function object, without placeholders.
        {
            const Int64 NUM_ALLOCS = Z0->numAllocations();

                  bdlf::Bind_TestTypeNoAlloc  mX;
            const bdlf::Bind_TestTypeNoAlloc& X = mX;

            const bdlf::Bind_TestTypeNoAlloc EXPECTED_X(I1);
            const bdlf::Bind_TestTypeNoAlloc DEFAULT_X(N1);
            ASSERT(EXPECTED_X != X);
            ASSERT(DEFAULT_X  == X);

            // For passing to the constructor of 'Bind_Impl'.

            typedef bdlf::Bind_TestTypeNoAlloc                      *FUNC;
            typedef bdlf::Bind_Tuple1<bdlf::Bind_TestArgNoAlloc<1> >  ListType;
            typedef Bind_Impl<bslmf::Nil, FUNC, ListType>           Bind;

                  ListType  mL(I1);     // list of arguments
            const ListType& L = mL;     // non-modifiable list of arguments

                  ListType  mM(N1);     // list of arguments
            const ListType& M = mM;     // non-modifiable list of arguments

            // Testing 'Bind_Impl' objects constructed explicitly.

                  Bind  mB1(&mX, L);
            const Bind& B1 = mB1;
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB1());
            ASSERT(EXPECTED_X == X);

                  Bind  mB2(&mX, M, Z0);
            const Bind& B2 = mB2;
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB2());
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB3());
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z0);
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB4());
            ASSERT(DEFAULT_X == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }

        // 1 argument to function object, *with* placeholders.
        {
            using namespace bdlf::PlaceHolders;

            const Int64 NUM_ALLOCS = Z0->numAllocations();

                  bdlf::Bind_TestTypeNoAlloc  mX;
            const bdlf::Bind_TestTypeNoAlloc& X = mX;

            const bdlf::Bind_TestTypeNoAlloc EXPECTED_X(I1);
            const bdlf::Bind_TestTypeNoAlloc DEFAULT_X(N1);
            ASSERT(EXPECTED_X != X);
            ASSERT(DEFAULT_X  == X);

            // For passing to the constructor of 'Bind_Impl'.

            typedef bdlf::Bind_TestTypeNoAlloc             *FUNC;
            typedef bdlf::Bind_Tuple1<PH1>                  ListType;
            typedef Bind_Impl<bslmf::Nil, FUNC, ListType>  Bind;

                  ListType  mL(_1);     // list of arguments
            const ListType& L = mL;     // non-modifiable list of arguments

                  Bind  mB1(&mX, L);
            const Bind& B1 = mB1;
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB1(I1));
            ASSERT(EXPECTED_X == X);

                  Bind  mB2(&mX, L, Z0);
            const Bind& B2 = mB2;
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB2(N1));
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB3(I1));
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z0);
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB4(N1));
            ASSERT(DEFAULT_X == X);

            ASSERT(NUM_ALLOCS == Z0->numAllocations());
        }
    }
//..
//
///Testing 'bdlf_bind' *with* allocators
///- - - - - - - - - - - - - - - - - - - -
// In this test code, we bind a 'bdlf::Bind_TestTypeAlloc' invocable using a
// 'Bind_Impl' object constructed with and without placeholders.
//..
    void usageExampleAlloc(int veryVeryVerbose)
    {
        bslma::TestAllocator  allocator0(veryVeryVerbose);
        bslma::TestAllocator  allocator1(veryVeryVerbose);
        bslma::TestAllocator  allocator2(veryVeryVerbose);

        bslma::TestAllocator *Z0 = &allocator0;
        bslma::TestAllocator *Z1 = &allocator1;
        bslma::TestAllocator *Z2 = &allocator2;

        bslma::DefaultAllocatorGuard allocGuard(Z0);

        const bdlf::Bind_TestArgAlloc<1> NV1 = -1;
        const bdlf::Bind_TestArgAlloc<1>  V1 =  1;

        // 1 argument to function object, *without* placeholders.
        {
                  bdlf::Bind_TestTypeAlloc  mX(Z1);
            const bdlf::Bind_TestTypeAlloc& X = mX;

            const bdlf::Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
            const bdlf::Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
            ASSERT(EXPECTED_X != X);
            ASSERT(DEFAULT_X  == X);

            typedef bdlf::Bind_TestTypeAlloc *FUNC;
            typedef bdlf::Bind_Tuple1<bdlf::Bind_TestArgAlloc<1> > ListType;
            typedef Bind_Impl<bslmf::Nil, FUNC, ListType> Bind;

            // For passing to the constructor of 'Bind_Impl'.

                  ListType  mL(V1);  // list of arguments
            const ListType& L = mL;  // non-modifiable list of arguments

                  ListType  mM(NV1); // list of arguments
            const ListType& M = mM;  // non-modifiable list of arguments

                  Bind  mB1(&mX, L);
            const Bind& B1 = mB1;
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB1());
            ASSERT(EXPECTED_X == X);

                  Bind  mB2(&mX, M, Z2);
            const Bind& B2 = mB2;
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB2());
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB3());
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z2);
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB4());
            ASSERT(DEFAULT_X == X);
        }

        // 1 argument to function object, *with* placeholders.
        {
            using namespace bdlf::PlaceHolders;

                  bdlf::Bind_TestTypeAlloc  mX(Z1);
            const bdlf::Bind_TestTypeAlloc& X = mX;

            const bdlf::Bind_TestTypeAlloc EXPECTED_X(Z1, V1);
            const bdlf::Bind_TestTypeAlloc DEFAULT_X(Z1, NV1);
            ASSERT(EXPECTED_X != X);
            ASSERT(DEFAULT_X  == X);

            typedef bdlf::Bind_TestTypeAlloc *FUNC;
            typedef bdlf::Bind_Tuple1<PH1> ListType;
            typedef Bind_Impl<bslmf::Nil, FUNC, ListType> Bind;

            // For passing to the constructor of 'Bind_Impl'.

                  ListType  mL(_1);  // list of arguments
            const ListType& L = mL;  // non-modifiable list of arguments

                  Bind  mB1(&mX, L);
            const Bind& B1 = mB1;
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB1(V1));
            ASSERT(EXPECTED_X == X);

                  Bind  mB2(&mX, L, Z2);
            const Bind& B2 = mB2;
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB2(NV1));
            ASSERT(DEFAULT_X == X);

            Bind mB3(B1);
            ASSERT(EXPECTED_X != X);
            ASSERT(1 == mB3(V1));
            ASSERT(EXPECTED_X == X);

            Bind mB4(B2, Z2);
            ASSERT(DEFAULT_X != X);
            ASSERT(1 == mB4(NV1));
            ASSERT(DEFAULT_X == X);
        }
    }
//..

}  // close namespace BDEF_BIND_TEST_USAGE_EXAMPLE
// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) veryVerbose;      // kill warning about unused variable
    (void) veryVeryVerbose;  // kill warning about unused variable

    // The following machinery is for use in conjunction with the 'resetSlots'
    // and 'VerifyNoAllocSlots' functions.  The slots are set when the
    // corresponding function objector free function is called with 'NumArgs'
    // arguments.

    const int NO_ALLOC_SLOTS[][NUM_SLOTS]= {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14    NumArgs
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 0
        { -1,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 1
        { -1,  1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 2
        { -1,  1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 3
        { -1,  1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 4
        { -1,  1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, }, // 5
        { -1,  1,  2,  3,  4,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, }, // 6
        { -1,  1,  2,  3,  4,  5,  6,  7, -1, -1, -1, -1, -1, -1, -1, }, // 7
        { -1,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1, }, // 8
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, }, // 9
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, -1, -1, -1, -1, }, // 10
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, -1, -1, -1, }, // 11
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, -1, -1, }, // 12
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, -1, }, // 13
        { -1,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, }, // 14
    };
    const int NO_ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {
          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };

    // Values that do not take an allocator.
    const bdlf::Bind_TestArgNoAlloc<1>  I1  = 1;
    const bdlf::Bind_TestArgNoAlloc<2>  I2  = 2;
    const bdlf::Bind_TestArgNoAlloc<3>  I3  = 3;
    const bdlf::Bind_TestArgNoAlloc<4>  I4  = 4;
    const bdlf::Bind_TestArgNoAlloc<5>  I5  = 5;
    const bdlf::Bind_TestArgNoAlloc<6>  I6  = 6;
    const bdlf::Bind_TestArgNoAlloc<7>  I7  = 7;
    const bdlf::Bind_TestArgNoAlloc<8>  I8  = 8;
    const bdlf::Bind_TestArgNoAlloc<9>  I9  = 9;
    const bdlf::Bind_TestArgNoAlloc<10> I10 = 10;
    const bdlf::Bind_TestArgNoAlloc<11> I11 = 11;
    const bdlf::Bind_TestArgNoAlloc<12> I12 = 12;
    const bdlf::Bind_TestArgNoAlloc<13> I13 = 13;
    const bdlf::Bind_TestArgNoAlloc<14> I14 = 14;

    // The following machinery is for use in conjunction with the 'resetSlots'
    // and 'VerifyAllocSlots' functions.  The slots are set when the
    // corresponding function object or free function is called with 'NumArgs'
    // arguments.

    bslma::TestAllocator allocator0(veryVeryVerbose);
    bslma::TestAllocator allocator1(veryVeryVerbose);
    bslma::TestAllocator allocator2(veryVeryVerbose);

    bslma::TestAllocator *Z0 = &allocator0;
    bslma::TestAllocator *Z1 = &allocator1;
    bslma::TestAllocator *Z2 = &allocator2;

    SlotsAlloc::setZ0(Z0);
    SlotsAlloc::setZ1(Z1);
    SlotsAlloc::setZ2(Z2);

    bslma::DefaultAllocatorGuard allocGuard(Z0);

    const bslma::Allocator *ALLOC_SLOTS[][NUM_SLOTS] = {
        // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14    NumArgs
        { Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 0
        { Z0, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 1
        { Z0, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 2
        { Z0, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 3
        { Z0, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 4
        { Z0, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 5
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 6
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, Z0, }, // 7
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, Z0, }, // 8
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, Z0, }, // 9
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, Z0, }, // 10
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, Z0, }, // 11
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, Z0, }, // 12
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z0, }, // 13
        { Z0, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z2, }, // 14
        { Z0, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, Z1, }, // 15
    };
    const bslma::Allocator * ALLOC_SLOTS_DEFAULT[NUM_SLOTS] = {
          Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0, Z0,
    };

    // Values that do take an allocator.
    const bdlf::Bind_TestArgAlloc<1>  V1(1, Z2),     NV1(-1);
    const bdlf::Bind_TestArgAlloc<2>  V2(20, Z2),    NV2(-20);
    const bdlf::Bind_TestArgAlloc<3>  V3(23, Z2),    NV3(-23);
    const bdlf::Bind_TestArgAlloc<4>  V4(44, Z2),    NV4(-44);
    const bdlf::Bind_TestArgAlloc<5>  V5(66, Z2),    NV5(-66);
    const bdlf::Bind_TestArgAlloc<6>  V6(176, Z2),   NV6(-176);
    const bdlf::Bind_TestArgAlloc<7>  V7(878, Z2),   NV7(-878);
    const bdlf::Bind_TestArgAlloc<8>  V8(8, Z2),     NV8(-8);
    const bdlf::Bind_TestArgAlloc<9>  V9(912, Z2),   NV9(-912);
    const bdlf::Bind_TestArgAlloc<10> V10(102, Z2),  NV10(-120);
    const bdlf::Bind_TestArgAlloc<11> V11(111, Z2),  NV11(-111);
    const bdlf::Bind_TestArgAlloc<12> V12(333, Z2),  NV12(-333);
    const bdlf::Bind_TestArgAlloc<13> V13(712, Z2),  NV13(-712);
    const bdlf::Bind_TestArgAlloc<14> V14(1414, Z2), NV14(-1414);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // ------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage examples provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and change the 'main' function into
        //   'usageExample*'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // ------------------------------------------------------------------


        if (verbose) printf("\nTESTING USAGE EXAMPLE"
                            "\n==============\n");

        using namespace BDEF_BIND_TEST_USAGE_EXAMPLE;

        usageExampleNoAlloc(veryVeryVerbose);
        usageExampleAlloc(veryVeryVerbose);

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concern: that 'bdlf::Bind_TestTypeNoAlloc' does not have the
        //   allocator traits, but 'bdlf::Bind_TestArgAlloc' and
        //   'bdlf::Bind_TestTypeNoAlloc' have.
        //
        // Plan:
        //   The test class 'TestArg' is a template, so it is
        //   sufficient to test for the expected traits with only one set of
        //   template parameters.
        //
        // Testing:
        //   BSLALG_DECLARE_NESTED_TRAITS
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING TRAITS"
                            "\n==============\n");

        if (verbose) printf("\tAsserting traits of test classes.\n");
        {
            ASSERT(0 == (bslalg::HasTrait<bdlf::Bind_TestArgNoAlloc<1>,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

            ASSERT(0 == (bslalg::HasTrait<bdlf::Bind_TestTypeNoAlloc,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

            ASSERT(1 == (bslalg::HasTrait<bdlf::Bind_TestArgAlloc<1>,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

            ASSERT(1 == (bslalg::HasTrait<bdlf::Bind_TestTypeAlloc,
                                 bslalg::TypeTraitUsesBslmaAllocator>::VALUE));
        }

      } break;
      case 1: {
        // ------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //
        // Concerns:
        //   This component has a rather large apparatus of test helper
        //   functions, classes, and macros.  We need to make sure they all
        //   work as intended.
        //
        // Plan:
        //   'isBitwiseMoveableType': Returns true if called on bitwise
        //       moveable types and false otherwise.
        //   '...NoAllocSlots':      set slots in sequence to produce the
        //       various rows of the 'NO_ALLOC_SLOTS' matrix.
        //   'class NoAllocTestType': check the constructor and comparison
        //       operator, then check every member function
        //       'testFunction[0-14]' including its side-effects on the slots.
        //   global 'function[0-14]': same as for member functions.
        //   '...AllocSlots':      set slots in sequence to produce the
        //       various rows of the 'NO_ALLOC_SLOTS' matrix.
        //   'class AllocTestType': check the constructor and comparison
        //       operator, then check every member function
        //       'testFunction[0-14]' including its side-effects on the slots.
        //   global 'function[0-14]': same as for member functions.
        //
        // Testing:
        //   TESTING HELPER FUNCTIONS/CLASSES
        // ------------------------------------------------------------------

        if (verbose) printf("\nTESTING HELPER FUNCTIONS/CLASSES"
                            "\n================================\n");

        if (verbose) printf("\tTestUtil machinery.\n");
        ASSERT( bdlf::Bind_TestUtil::isBitwiseMoveableType(3));
        ASSERT( bdlf::Bind_TestUtil::isBitwiseMoveableType(
                    bdlf::Bind_TestArgNoAlloc<1>(0)));
        ASSERT(!bdlf::Bind_TestUtil::isBitwiseMoveableType(
                    bdlf::Bind_TestArgAlloc<1>(0)));

        if (verbose) printf("\tNoAllocSlots machinery.\n");
        {
            for (int i = 0; i<NUM_SLOTS; ++i) {
                SlotsNoAlloc::resetSlots(N1);
                ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS_DEFAULT,
                            veryVerbose));
                for (int j = 1; j <= i; ++j) {
                    SlotsNoAlloc::setSlot(j, j);
                }
                ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[i],
                            veryVerbose));
            }
        }

        if (verbose) printf("\tclass NoAllocTestType.\n");
        {
            NoAllocTestType mX; NoAllocTestType const& X = mX;
            NoAllocTestType mY; NoAllocTestType const& Y = mY;
            ASSERT(X == Y);

            const NoAllocTestType EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(0  == mX.testFunc0());
            ASSERT(EXPECTED0 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[0], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(0  == mY());
            ASSERT(EXPECTED0 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[0], veryVerbose));

            const NoAllocTestType EXPECTED1(I1);
            mX = EXPECTED1;
            ASSERT(EXPECTED1 == X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(1  == mX.testFunc1(I1));
            ASSERT(EXPECTED1 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[1], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(1  == mY(I1));
            ASSERT(EXPECTED1 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[1], veryVerbose));

            const NoAllocTestType EXPECTED2 (I1,I2);
            mX = EXPECTED2;
            ASSERT(EXPECTED2 == X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(2  == mX.testFunc2(I1,I2));
            ASSERT(EXPECTED2 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[2], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(2  == mY(I1,I2));
            ASSERT(EXPECTED2 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[2], veryVerbose));

            const NoAllocTestType EXPECTED3 (I1,I2,I3);
            mX = EXPECTED3;
            ASSERT(EXPECTED3 == X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(3  == mX.testFunc3(I1,I2,I3));
            ASSERT(EXPECTED3 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[3], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(3  == mY(I1,I2,I3));
            ASSERT(EXPECTED3 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[3], veryVerbose));

            const NoAllocTestType EXPECTED4 (I1,I2,I3,I4);
            mX = EXPECTED4;
            ASSERT(EXPECTED4 == X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(4  == mX.testFunc4(I1,I2,I3,I4));
            ASSERT(EXPECTED4 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[4], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(4  == mY(I1,I2,I3,I4));
            ASSERT(EXPECTED4 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[4], veryVerbose));

            const NoAllocTestType EXPECTED5 (I1,I2,I3,I4,I5);
            mX = EXPECTED5;
            ASSERT(EXPECTED5 == X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(5  == mX.testFunc5(I1,I2,I3,I4,I5));
            ASSERT(EXPECTED5 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[5], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(5  == mY(I1,I2,I3,I4,I5));
            ASSERT(EXPECTED5 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[5], veryVerbose));

            const NoAllocTestType EXPECTED6 (I1,I2,I3,I4,I5,I6);
            mX = EXPECTED6;
            ASSERT(EXPECTED6 == X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(6  == mX.testFunc6(I1,I2,I3,I4,I5,I6));
            ASSERT(EXPECTED6 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[6], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(6  == mY(I1,I2,I3,I4,I5,I6));
            ASSERT(EXPECTED6 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[6], veryVerbose));

            const NoAllocTestType EXPECTED7 (I1,I2,I3,I4,I5,I6,I7);
            mX = EXPECTED7;
            ASSERT(EXPECTED7 == X);
            ASSERT(EXPECTED6 != X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(7  == mX.testFunc7(I1,I2,I3,I4,I5,I6,I7));
            ASSERT(EXPECTED7 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[7], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(7  == mY(I1,I2,I3,I4,I5,I6,I7));
            ASSERT(EXPECTED7 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[7], veryVerbose));

            const NoAllocTestType EXPECTED8 (I1,I2,I3,I4,I5,I6,I7,I8);
            mX = EXPECTED8;
            ASSERT(EXPECTED8 == X);
            ASSERT(EXPECTED7 != X);
            ASSERT(EXPECTED6 != X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(8  == mX.testFunc8(I1,I2,I3,I4,I5,I6,I7,I8));
            ASSERT(EXPECTED8 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[8], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(8  == mY(I1,I2,I3,I4,I5,I6,I7,I8));
            ASSERT(EXPECTED8 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[8], veryVerbose));

            const NoAllocTestType EXPECTED9 (I1,I2,I3,I4,I5,I6,I7,I8,I9);
            mX = EXPECTED9;
            ASSERT(EXPECTED9 == X);
            ASSERT(EXPECTED8 != EXPECTED9);
            ASSERT(EXPECTED7 != EXPECTED9);
            ASSERT(EXPECTED6 != EXPECTED9);
            ASSERT(EXPECTED5 != EXPECTED9);
            ASSERT(EXPECTED4 != EXPECTED9);
            ASSERT(EXPECTED3 != EXPECTED9);
            ASSERT(EXPECTED2 != EXPECTED9);
            ASSERT(EXPECTED1 != EXPECTED9);
            ASSERT(EXPECTED0 != EXPECTED9);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(9  == mX.testFunc9(I1,I2,I3,I4,I5,I6,I7,I8,I9));
            ASSERT(EXPECTED9 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[9], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(9  == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9));
            ASSERT(EXPECTED9 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[9], veryVerbose));

            const NoAllocTestType EXPECTED10(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10);
            mX = EXPECTED10;
            ASSERT(EXPECTED10 == X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(10 == mX.testFunc10(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10));
            ASSERT(EXPECTED10== X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[10], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(10 == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10));
            ASSERT(EXPECTED10== Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[10], veryVerbose));

            const NoAllocTestType EXPECTED11(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                                          I11);
            mX = EXPECTED11;
            ASSERT(EXPECTED11 == X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(11 == mX.testFunc11(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11));
            ASSERT(EXPECTED11 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[11], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(11 == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11));
            ASSERT(EXPECTED11 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[11], veryVerbose));

            const NoAllocTestType EXPECTED12(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                                      I11,I12);
            mX = EXPECTED12;
            ASSERT(EXPECTED12 == X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(12 == mX.testFunc12(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,
                                                                         I12));
            ASSERT(EXPECTED12 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[12], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(12 == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12));
            ASSERT(EXPECTED12 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[12], veryVerbose));

            const NoAllocTestType EXPECTED13(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                                  I11,I12,I13);
            mX = EXPECTED13;
            ASSERT(EXPECTED13 == X);
            ASSERT(EXPECTED12 != X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(13 == mX.testFunc13(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,
                                                                     I12,I13));
            ASSERT(EXPECTED13 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[13], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(13 == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13));
            ASSERT(EXPECTED13 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[13], veryVerbose));

            const NoAllocTestType EXPECTED14(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                              I11,I12,I13,I14);
            mX = EXPECTED14;
            ASSERT(EXPECTED14 == X);
            ASSERT(EXPECTED13 != X);
            ASSERT(EXPECTED12 != X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsNoAlloc::resetSlots(N1);
            ASSERT(14 == mX.testFunc14(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,
                                                                 I12,I13,I14));
            ASSERT(EXPECTED14 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[14], veryVerbose));
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(14 == mY(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14));
            ASSERT(EXPECTED14 == Y);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[14], veryVerbose));
        }

        if (verbose) printf("\tglobal func0-14 functions.\n");
        {
            NoAllocTestType mX; NoAllocTestType const& X = mX;

            const NoAllocTestType EXPECTED0;
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(0  == bdlf::Bind_TestFunctionsNoAlloc::func0(&mX));
            ASSERT(EXPECTED0 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[0], veryVerbose));

            const NoAllocTestType EXPECTED1(I1);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(1  == bdlf::Bind_TestFunctionsNoAlloc::func1(&mX,I1));
            ASSERT(EXPECTED1 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[1], veryVerbose));

            const NoAllocTestType EXPECTED2 (I1,I2);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(2  == bdlf::Bind_TestFunctionsNoAlloc::func2(&mX,I1,I2));
            ASSERT(EXPECTED2 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[2], veryVerbose));

            const NoAllocTestType EXPECTED3 (I1,I2,I3);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(3  == bdlf::Bind_TestFunctionsNoAlloc::func3(&mX,I1,I2,I3));
            ASSERT(EXPECTED3 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[3], veryVerbose));

            const NoAllocTestType EXPECTED4 (I1,I2,I3,I4);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(4  == bdlf::Bind_TestFunctionsNoAlloc::func4(&mX,
                                                                 I1,I2,I3,I4));
            ASSERT(EXPECTED4 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[4], veryVerbose));

            const NoAllocTestType EXPECTED5 (I1,I2,I3,I4,I5);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(5  == bdlf::Bind_TestFunctionsNoAlloc::func5(&mX,
                                                              I1,I2,I3,I4,I5));
            ASSERT(EXPECTED5 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[5], veryVerbose));

            const NoAllocTestType EXPECTED6 (I1,I2,I3,I4,I5,I6);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(6  == bdlf::Bind_TestFunctionsNoAlloc::func6(&mX,
                                                           I1,I2,I3,I4,I5,I6));
            ASSERT(EXPECTED6 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[6], veryVerbose));

            const NoAllocTestType EXPECTED7 (I1,I2,I3,I4,I5,I6,I7);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(7  == bdlf::Bind_TestFunctionsNoAlloc::func7(&mX,
                                                        I1,I2,I3,I4,I5,I6,I7));
            ASSERT(EXPECTED7 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[7], veryVerbose));

            const NoAllocTestType EXPECTED8 (I1,I2,I3,I4,I5,I6,I7,I8);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(8  == bdlf::Bind_TestFunctionsNoAlloc::func8(&mX,
                                                     I1,I2,I3,I4,I5,I6,I7,I8));
            ASSERT(EXPECTED8 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[8], veryVerbose));

            const NoAllocTestType EXPECTED9 (I1,I2,I3,I4,I5,I6,I7,I8,I9);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(9  == bdlf::Bind_TestFunctionsNoAlloc::func9(&mX,
                                                  I1,I2,I3,I4,I5,I6,I7,I8,I9));
            ASSERT(EXPECTED9 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[9], veryVerbose));

            const NoAllocTestType EXPECTED10(I1,I2,I3,I4,I5,I6,I7,I8,I9,
                                                          I10);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(10 == bdlf::Bind_TestFunctionsNoAlloc::func10(&mX,
                                              I1,I2,I3,I4,I5,I6,I7,I8,I9,I10));
            ASSERT(EXPECTED10== X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[10], veryVerbose));

            const NoAllocTestType EXPECTED11(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                             I11);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(11 == bdlf::Bind_TestFunctionsNoAlloc::func11(&mX,
                                          I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11));
            ASSERT(EXPECTED11 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[11], veryVerbose));

            const NoAllocTestType EXPECTED12(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                                      I11,I12);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(12 == bdlf::Bind_TestFunctionsNoAlloc::func12(&mX,
                                      I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12));
            ASSERT(EXPECTED12 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[12], veryVerbose));

            const NoAllocTestType EXPECTED13(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                                  I11,I12,I13);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(13 == bdlf::Bind_TestFunctionsNoAlloc::func13(&mX,
                                 I1,I2,I3,I4,I5,I6,I7,I8,I9,I10, I11,I12,I13));
            ASSERT(EXPECTED13 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[13], veryVerbose));

            const NoAllocTestType EXPECTED14(I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,
                                                              I11,I12,I13,I14);
            SlotsNoAlloc::resetSlots(N1);
            ASSERT(14 == bdlf::Bind_TestFunctionsNoAlloc::func14(&mX,
                              I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14));
            ASSERT(EXPECTED14 == X);
            ASSERT(SlotsNoAlloc::verifySlots(NO_ALLOC_SLOTS[14], veryVerbose));
        }

        if (verbose) printf("\tAllocSlots machinery.\n");
        {
            SlotsAlloc::resetSlots(Z0);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS_DEFAULT, veryVerbose));

            for (int i = 0; i<NUM_SLOTS; ++i) {
                SlotsAlloc::resetSlots(Z0);
                for (int j = 1; j <= i; ++j) {
                    SlotsAlloc::setSlot(Z2, j);
                }
                ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[i], veryVerbose));
            }
        }

        if (verbose) printf("\tclass bdlf::Bind_TestArgAlloc.\n");
        {
            const Int64 NUM_ALLOCS_Z0 = Z0->numAllocations();
            const Int64 NUM_ALLOCS_Z1 = Z1->numAllocations();

            // Concern: creation should allocate an int using proper allocator.
            bdlf::Bind_TestArgAlloc<0> mX(1,Z1);
            bdlf::Bind_TestArgAlloc<0> const& X = mX;
            ASSERT(NUM_ALLOCS_Z0   == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+1 == Z1->numAllocations());

            // Concern: creation should allocate an int using proper allocator.
            bdlf::Bind_TestArgAlloc<0> mY(2,Z1);
            bdlf::Bind_TestArgAlloc<0> const& Y = mY;
            ASSERT(NUM_ALLOCS_Z0   == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+2 == Z1->numAllocations());

            // For testing, a value equal to 'X' with a default allocator.
            bdlf::Bind_TestArgAlloc<0> mZ(1,Z0);
            bdlf::Bind_TestArgAlloc<0> const& Z = mZ;
            ASSERT(NUM_ALLOCS_Z0+1 == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+2 == Z1->numAllocations());

            // Concerns: comparison operator should not copy, and should not be
            // influenced by the allocator.
            ASSERT(X != Y);
            ASSERT(X == Z);
            ASSERT(NUM_ALLOCS_Z0+1 == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+2 == Z1->numAllocations());

            // Concern: copying to self does not allocate.
            mX = X;
            ASSERT(X == Z);
            ASSERT(NUM_ALLOCS_Z0+1 == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+2 == Z1->numAllocations());

            // Concern: copying from non-self allocates from proper allocator
            // and value is correct.
            mX = Y;
            ASSERT(X == Y);
            ASSERT(NUM_ALLOCS_Z0+1 == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+3 == Z1->numAllocations());

            // Concern: copying does not acquire different allocator.
            mX = Z;
            ASSERT(X == Z);
            ASSERT(NUM_ALLOCS_Z0+1 == Z0->numAllocations());
            ASSERT(NUM_ALLOCS_Z1+4 == Z1->numAllocations());

            // Concern: member functions 'allocator' and 'value'.
            ASSERT(Z1 == X.allocator());
            ASSERT(Z1 == Y.allocator());
            ASSERT(Z0 == Z.allocator());
            ASSERT(1 == X.value());
            ASSERT(2 == Y.value());
            ASSERT(1 == Z.value());
        }

        if (verbose) printf("\tclass AllocTestType.\n");
        {
            AllocTestType mX(Z1); AllocTestType const& X = mX;
            AllocTestType mY(Z1); AllocTestType const& Y = mY;

            const AllocTestType EXPECTED0(Z0);
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(0  == mX.testFunc0());
            ASSERT(EXPECTED0 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[0], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(0  == mY());
            ASSERT(EXPECTED0 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[0], veryVerbose));

            const AllocTestType EXPECTED1(Z0,V1);
            mX = EXPECTED1;
            ASSERT(EXPECTED1 == X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(1  == mX.testFunc1(V1));
            ASSERT(EXPECTED1 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[1], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(1  == mY(V1));
            ASSERT(EXPECTED1 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[1], veryVerbose));

            const AllocTestType EXPECTED2(Z0,V1,V2);
            mX = EXPECTED2;
            ASSERT(EXPECTED2 == X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(2  == mX.testFunc2(V1,V2));
            ASSERT(EXPECTED2 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[2], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(2  == mY(V1,V2));
            ASSERT(EXPECTED2 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[2], veryVerbose));

            const AllocTestType EXPECTED3(Z0,V1,V2,V3);
            mX = EXPECTED3;
            ASSERT(EXPECTED3 == X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(3  == mX.testFunc3(V1,V2,V3));
            ASSERT(EXPECTED3 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[3], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(3  == mY(V1,V2,V3));
            ASSERT(EXPECTED3 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[3], veryVerbose));

            const AllocTestType EXPECTED4(Z0,V1,V2,V3,V4);
            mX = EXPECTED4;
            ASSERT(EXPECTED4 == X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(4  == mX.testFunc4(V1,V2,V3,V4));
            ASSERT(EXPECTED4 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[4], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(4  == mY(V1,V2,V3,V4));
            ASSERT(EXPECTED4 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[4], veryVerbose));

            const AllocTestType EXPECTED5(Z0,V1,V2,V3,V4,V5);
            mX = EXPECTED5;
            ASSERT(EXPECTED5 == X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(5  == mX.testFunc5(V1,V2,V3,V4,V5));
            ASSERT(EXPECTED5 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[5], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(5  == mY(V1,V2,V3,V4,V5));
            ASSERT(EXPECTED5 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[5], veryVerbose));

            const AllocTestType EXPECTED6(Z0,V1,V2,V3,V4,V5,V6);
            mX = EXPECTED6;
            ASSERT(EXPECTED6 == X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(6  == mX.testFunc6(V1,V2,V3,V4,V5,V6));
            ASSERT(EXPECTED6 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[6], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(6  == mY(V1,V2,V3,V4,V5,V6));
            ASSERT(EXPECTED6 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[6], veryVerbose));

            const AllocTestType EXPECTED7(Z0,V1,V2,V3,V4,V5,V6,V7);
            mX = EXPECTED7;
            ASSERT(EXPECTED7 == X);
            ASSERT(EXPECTED6 != X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(7  == mX.testFunc7(V1,V2,V3,V4,V5,V6,V7));
            ASSERT(EXPECTED7 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[7], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(7  == mY(V1,V2,V3,V4,V5,V6,V7));
            ASSERT(EXPECTED7 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[7], veryVerbose));

            const AllocTestType EXPECTED8(Z0,V1,V2,V3,V4,V5,V6,V7,V8);
            mX = EXPECTED8;
            ASSERT(EXPECTED8 == X);
            ASSERT(EXPECTED7 != X);
            ASSERT(EXPECTED6 != X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(8  == mX.testFunc8(V1,V2,V3,V4,V5,V6,V7,V8));
            ASSERT(EXPECTED8 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[8], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(8  == mY(V1,V2,V3,V4,V5,V6,V7,V8));
            ASSERT(EXPECTED8 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[8], veryVerbose));

            const AllocTestType EXPECTED9(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9);
            mX = EXPECTED9;
            ASSERT(EXPECTED9 == X);
            ASSERT(EXPECTED8 != X);
            ASSERT(EXPECTED7 != X);
            ASSERT(EXPECTED6 != X);
            ASSERT(EXPECTED5 != X);
            ASSERT(EXPECTED4 != X);
            ASSERT(EXPECTED3 != X);
            ASSERT(EXPECTED2 != X);
            ASSERT(EXPECTED1 != X);
            ASSERT(EXPECTED0 != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0 == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(9  == mX.testFunc9(V1,V2,V3,V4,V5,V6,V7,V8,V9));
            ASSERT(EXPECTED9 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[9], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(9  == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9));
            ASSERT(EXPECTED9 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[9], veryVerbose));

            const AllocTestType EXPECTED10(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10);
            mX = EXPECTED10;
            ASSERT(EXPECTED10 == X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(10 == mX.testFunc10(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10));
            ASSERT(EXPECTED10== X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[10], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(10 == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10));
            ASSERT(EXPECTED10== Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[10], veryVerbose));

            const AllocTestType EXPECTED11(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                          V11);
            mX = EXPECTED11;
            ASSERT(EXPECTED11 == X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(11 == mX.testFunc11(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11));
            ASSERT(EXPECTED11 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[11], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(11 == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11));
            ASSERT(EXPECTED11 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[11], veryVerbose));

            const AllocTestType EXPECTED12(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                      V11,V12);
            mX = EXPECTED12;
            ASSERT(EXPECTED12 == X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(12 == mX.testFunc12(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                       V11,V12));
            ASSERT(EXPECTED12 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[12], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(12 == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12));
            ASSERT(EXPECTED12 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[12], veryVerbose));

            const AllocTestType EXPECTED13(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                  V11,V12,V13);
            mX = EXPECTED13;
            ASSERT(EXPECTED13 == X);
            ASSERT(EXPECTED12 != X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(13 == mX.testFunc13(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,
                                                                         V13));
            ASSERT(EXPECTED13 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[13], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(13 == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,V13));
            ASSERT(EXPECTED13 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[13], veryVerbose));

            const AllocTestType EXPECTED14(Z0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                              V11,V12,V13,V14);
            mX = EXPECTED14;
            ASSERT(EXPECTED14 == X);
            ASSERT(EXPECTED13 != X);
            ASSERT(EXPECTED12 != X);
            ASSERT(EXPECTED11 != X);
            ASSERT(EXPECTED10 != X);
            ASSERT(EXPECTED9  != X);
            ASSERT(EXPECTED8  != X);
            ASSERT(EXPECTED7  != X);
            ASSERT(EXPECTED6  != X);
            ASSERT(EXPECTED5  != X);
            ASSERT(EXPECTED4  != X);
            ASSERT(EXPECTED3  != X);
            ASSERT(EXPECTED2  != X);
            ASSERT(EXPECTED1  != X);
            ASSERT(EXPECTED0  != X);
            mX = EXPECTED0;
            ASSERT(EXPECTED0  == X);

            SlotsAlloc::resetSlots(Z0);
            ASSERT(14 == mX.testFunc14(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,
                                       V13,V14));
            ASSERT(EXPECTED14 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[14], veryVerbose));
            mX.setSlots();
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[15], veryVerbose));
            SlotsAlloc::resetSlots(Z0);
            ASSERT(14 == mY(V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,V13,V14));
            ASSERT(EXPECTED14 == Y);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[14], veryVerbose));
        }

        if (verbose) printf("\tglobal func0-14 functions.\n");
        {
            AllocTestType mX(Z1); AllocTestType const& X = mX;

            const AllocTestType EXPECTED0(Z1);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(0  == bdlf::Bind_TestFunctionsAlloc::func0(&mX));
            ASSERT(EXPECTED0 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[0], veryVerbose));

            const AllocTestType EXPECTED1(Z1,V1);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(1  == bdlf::Bind_TestFunctionsAlloc::func1(&mX,V1));
            ASSERT(EXPECTED1 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[1], veryVerbose));

            const AllocTestType EXPECTED2(Z1,V1,V2);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(2  == bdlf::Bind_TestFunctionsAlloc::func2(&mX,V1,V2));
            ASSERT(EXPECTED2 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[2], veryVerbose));

            const AllocTestType EXPECTED3(Z1,V1,V2,V3);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(3  == bdlf::Bind_TestFunctionsAlloc::func3(&mX,V1,V2,V3));
            ASSERT(EXPECTED3 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[3], veryVerbose));

            const AllocTestType EXPECTED4(Z1,V1,V2,V3,V4);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(4  == bdlf::Bind_TestFunctionsAlloc::func4(&mX,
                                                                 V1,V2,V3,V4));
            ASSERT(EXPECTED4 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[4], veryVerbose));

            const AllocTestType EXPECTED5(Z1,V1,V2,V3,V4,V5);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(5  == bdlf::Bind_TestFunctionsAlloc::func5(&mX,
                                                              V1,V2,V3,V4,V5));
            ASSERT(EXPECTED5 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[5], veryVerbose));

            const AllocTestType EXPECTED6 (Z1,V1,V2,V3,V4,V5,V6);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(6  == bdlf::Bind_TestFunctionsAlloc::func6(&mX,
                                                           V1,V2,V3,V4,V5,V6));
            ASSERT(EXPECTED6 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[6], veryVerbose));

            const AllocTestType EXPECTED7(Z1,V1,V2,V3,V4,V5,V6,V7);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(7  == bdlf::Bind_TestFunctionsAlloc::func7(&mX,
                                                        V1,V2,V3,V4,V5,V6,V7));
            ASSERT(EXPECTED7 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[7], veryVerbose));

            const AllocTestType EXPECTED8(Z1,V1,V2,V3,V4,V5,V6,V7,V8);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(8  == bdlf::Bind_TestFunctionsAlloc::func8(&mX,
                                                     V1,V2,V3,V4,V5,V6,V7,V8));
            ASSERT(EXPECTED8 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[8], veryVerbose));

            const AllocTestType EXPECTED9(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(9  == bdlf::Bind_TestFunctionsAlloc::func9(&mX,
                                                  V1,V2,V3,V4,V5,V6,V7,V8,V9));
            ASSERT(EXPECTED9 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[9], veryVerbose));

            const AllocTestType EXPECTED10(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(10 == bdlf::Bind_TestFunctionsAlloc::func10(&mX,
                                              V1,V2,V3,V4,V5,V6,V7,V8,V9,V10));
            ASSERT(EXPECTED10== X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[10], veryVerbose));

            const AllocTestType EXPECTED11(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                          V11);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(11 == bdlf::Bind_TestFunctionsAlloc::func11(&mX,
                                          V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11));
            ASSERT(EXPECTED11 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[11], veryVerbose));

            const AllocTestType EXPECTED12(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                      V11,V12);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(12 == bdlf::Bind_TestFunctionsAlloc::func12(&mX,
                                      V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12));
            ASSERT(EXPECTED12 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[12], veryVerbose));

            const AllocTestType EXPECTED13(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                                  V11,V12,V13);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(13 == bdlf::Bind_TestFunctionsAlloc::func13(&mX,
                                  V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,V13));
            ASSERT(EXPECTED13 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[13], veryVerbose));

            const AllocTestType EXPECTED14(Z1,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,
                                                              V11,V12,V13,V14);
            SlotsAlloc::resetSlots(Z0);
            ASSERT(14 == bdlf::Bind_TestFunctionsAlloc::func14(&mX,
                              V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,V13,V14));
            ASSERT(EXPECTED14 == X);
            ASSERT(SlotsAlloc::verifySlots(ALLOC_SLOTS[14], veryVerbose));
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
