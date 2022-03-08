// bslmf_memberpointertraits.t.cpp                                    -*-C++-*-
#include <bslmf_memberpointertraits.h>

#include <bsla_maybeunused.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>
#include <stdlib.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test defines only one public facing meta-function,
// 'bslmf::MemberPointerTraits', that determines the types of the pointed to
// member 'MemberType', and the class 'ClassType' to which the member bleongs.
// These tests will ensure that all types of pointers to members provide the
// correct types.
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [2 ] BloombergLP::bslmf::MemberPointerTraits::MemberType
// [2 ] BloombergLP::bslmf::MemberPointerTraits::ClassType
// ----------------------------------------------------------------------------
// [1 ] Usage example

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {
using namespace BloombergLP;

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s   (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

#define ASSERT_SAME(T1,T2) ASSERT((1 == bsl::is_same<T1,T2>::value))

struct TEST_CLASS_TYPE{};
struct TEST_PARAMETER_0{};
struct TEST_PARAMETER_1{};
struct BASE{
    // primitive members
    int d_int;
    const int d_constInt;
    volatile int d_volatileInt;
    const volatile int d_constVolatileInt;

    // pointer-to-primitive members
    int* d_ptrToInt_p;
    const int* d_ptrToConstInt_p;
    volatile int* d_ptrToVolatileInt_p;
    const volatile int* d_ptrToConstVolatileInt_p;
    int * const d_constPtrToInt_p;
    int * volatile d_volatilePtrToInt_p;
    int * const volatile d_constVolatilePtrToInt_p;

    // non-primitive members
    TEST_CLASS_TYPE d_class;
    const TEST_CLASS_TYPE d_constClass;
    volatile TEST_CLASS_TYPE d_volatileClass;
    const volatile TEST_CLASS_TYPE d_constVolatileClass;

    // pointer-to-non-primitive members
    TEST_CLASS_TYPE* d_ptrToClass_p;
    const TEST_CLASS_TYPE* d_ptrToConstClass_p;
    volatile TEST_CLASS_TYPE* d_ptrToVolatileClass_p;
    const volatile TEST_CLASS_TYPE* d_ptrToConstVolatileClass_p;
    TEST_CLASS_TYPE * const d_constPtrToClass_p;
    TEST_CLASS_TYPE * volatile d_volatilePtrToClass_p;
    TEST_CLASS_TYPE * const volatile d_constVolatilePtrToClass_p;

    // array of primitive members
    int d_arrayOfInt[4];
    const int d_constArrayOfInt[4];
    volatile int d_volatileArrayOfInt[4];
    const volatile int d_constVolatileArrayOfInt[4];

    // array of non primitive members
    TEST_CLASS_TYPE d_arrayOfClass[4];
    const TEST_CLASS_TYPE d_constArrayOfClass[4];
    volatile TEST_CLASS_TYPE d_volatileArrayOfClass[4];
    const volatile TEST_CLASS_TYPE d_constVolatileArrayOfClass[4];

    // member functions returning primitives
    int funcReturningInt(){return d_int;}
    int constFuncReturningInt() const {return d_int;}
    int volatileFuncReturningInt() volatile {return d_int;}
    int constVolatileFuncReturningInt() const volatile {return d_int;}

    // member functions returning non-primitives
    TEST_CLASS_TYPE funcReturningClass() {return d_class;}
    const TEST_CLASS_TYPE funcReturningConstClass() {return d_class;}
    volatile TEST_CLASS_TYPE funcReturningVolatileClass() {return d_class;}
    const volatile TEST_CLASS_TYPE funcReturningConstVolatileClass()
        {return d_class;}
    TEST_CLASS_TYPE funcReturningClassWithParam(TEST_PARAMETER_0)
        {return d_class;}
    TEST_CLASS_TYPE funcReturningClassWithParams(TEST_PARAMETER_0,
                                                 TEST_PARAMETER_1)
        {return d_class;}

    // virtual member functions
    virtual int virtualFuncReturningInt(){return d_int;}
    virtual int virtualConstFuncReturningInt() const {return d_int;}
    virtual int virtualVolatileFuncReturningInt() volatile {return d_int;}
    virtual int virtualConstVolatileFuncReturningInt() const volatile
        {return d_int;}
    virtual TEST_CLASS_TYPE virtualFuncReturningClass() {return d_class;}
    virtual TEST_CLASS_TYPE virtualFuncReturningClassWithParam(
            TEST_PARAMETER_0)
        {return d_class;}
    virtual TEST_CLASS_TYPE virtualFuncReturningClassWithParams(
            TEST_PARAMETER_0,
            TEST_PARAMETER_1)
        {return d_class;}

    int funcWithElipses(...) {return d_int;}
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    int funcRValueRef() && {return d_int;}
    int funcLValueRef() & {return d_int;}
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
    int funcNoExcept() noexcept {return d_int;}
#endif

    // default constructor setting const members
    BASE()
        : d_constInt(0)
        , d_constVolatileInt(0)
        , d_constPtrToInt_p()
        , d_constVolatilePtrToInt_p()
        , d_constClass()
        , d_constVolatileClass()
        , d_constPtrToClass_p()
        , d_constVolatilePtrToClass_p()
        , d_constArrayOfInt()
        , d_constVolatileArrayOfInt()
        , d_constArrayOfClass()
        , d_constVolatileArrayOfClass()
    {}
};

// pointer to member primitive typedefs
typedef int BASE::*PtrToMemberInt;
typedef const int BASE::*PtrToMemberConstInt;
typedef volatile int BASE::*PtrToMemberVolatileInt;
typedef const volatile int BASE::*PtrToMemberConstVolatileInt;

// pointer to member pointer to primitive typedefs
typedef int* BASE::*PtrToMemberPtrToInt;
typedef const int* BASE::*PtrToMemberPtrToConstInt;
typedef volatile int* BASE::*PtrToMemberPtrToVolatileInt;
typedef const volatile int* BASE::*PtrToMemberPtrToConstVolatileInt;
typedef int * const BASE::*PtrToMemberConstPtrToInt;
typedef int * volatile BASE::*PtrToMemberVolatilePtrToint;
typedef int * const volatile BASE::*PtrToMemberConstVolatilePtrToInt;

// pointer to member non-primitive typedefs
typedef TEST_CLASS_TYPE BASE::*PtrToMemberClass;
typedef const TEST_CLASS_TYPE BASE::*PtrToMemberConstClass;
typedef volatile TEST_CLASS_TYPE BASE::*PtrToMemberVolatileClass;
typedef const volatile TEST_CLASS_TYPE BASE::*PtrToMemberConstVolatileClass;

// pointer to member pointer to non-primitive typedefs
typedef TEST_CLASS_TYPE* BASE::*PtrToMemberPtrToClass;
typedef const TEST_CLASS_TYPE* BASE::*PtrToMemberPtrToConstClass;
typedef volatile TEST_CLASS_TYPE* BASE::*PtrToMemberPtrToVolatileClass;
typedef const volatile TEST_CLASS_TYPE*
    BASE::*PtrToMemberPtrToConstVolatileClass;
typedef TEST_CLASS_TYPE * const BASE::*PtrToMemberConstPtrToClass;
typedef TEST_CLASS_TYPE * volatile BASE::*PtrToMemberVolatilePtrToClass;
typedef TEST_CLASS_TYPE * const volatile
    BASE::*PtrToMemberConstVolatilePtrToClass;

// pointer to member array of primitive typedefs
typedef int (BASE::*PtrToMemberArrayOfInt)[4];
typedef const int (BASE::*PtrToMemberConstArrayOfInt)[4];
typedef volatile int (BASE::*PtrToMemberVolatileArrayOfInt)[4];
typedef const volatile int (BASE::*PtrToMemberConstVolatileArrayOfInt)[4];

// pointer to member array of non-primitive typedefs
typedef TEST_CLASS_TYPE (BASE::*PtrToMemberArrayOfClass)[4];
typedef const TEST_CLASS_TYPE (BASE::*PtrToMemberConstArrayOfClass)[4];
typedef volatile TEST_CLASS_TYPE (BASE::*PtrToMemberVolatileArrayOfClass)[4];
typedef const volatile TEST_CLASS_TYPE
    (BASE::*PtrToMemberConstVolatileArrayOfClass)[4];

// pointer to member function typedefs
typedef int (BASE::*PtrToMemberFuncReturningInt)();
typedef int (BASE::*PtrToMemberConstFuncReturningInt)() const;
typedef int (BASE::*PtrToMemberVolatileFuncReturningInt)() volatile;
typedef int (BASE::*PtrToMemberConstVolatileFuncReturningInt)() const volatile;
typedef TEST_CLASS_TYPE (BASE::*PtrToMemberFuncReturningClass)();
typedef const TEST_CLASS_TYPE (BASE::*PtrToMemberFuncReturningConstClass)();
typedef volatile TEST_CLASS_TYPE
    (BASE::*PtrToMemberFuncReturningVolatileClass)();
typedef const volatile TEST_CLASS_TYPE
    (BASE::*PtrToMemberFuncReturningConstVolatileClass)();
typedef TEST_CLASS_TYPE
    (BASE::*PtrToMemberFuncReturningClassWithParam)(TEST_PARAMETER_0);
typedef TEST_CLASS_TYPE
    (BASE::*PtrToMemberFuncReturningClassWithParams)(TEST_PARAMETER_0,
                                                     TEST_PARAMETER_1);

typedef int (BASE::*PtrToMemberFuncWithElipses)(...);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
typedef int (BASE::*PtrToMemberFuncRValueRef)() &&;
typedef int (BASE::*PtrToMemberFuncLValueRef)() &;
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
typedef int (BASE::*PtrToMemberFuncNoExcept)() noexcept;
#endif

// ============================================================================
//                                   USAGE
// ----------------------------------------------------------------------------
// Define the following 'struct' with the following members:
//..
    struct MyTestClass {
        int func1(int) { return 0; }
        int d_int;
    };
//..
// In order to deduce the types of 'func1' and 'd_int', we will use
// 'bslmf::MemberPointerTraits'.
//..
    template <class MEMBER, class CLASS, class TYPE>
    void checkMemberPointer(TYPE pointer)
    {
        (void) pointer;
        typedef typename bslmf::MemberPointerTraits<TYPE>::MemberType
            MemberType;
        typedef typename bslmf::MemberPointerTraits<TYPE>::ClassType
            ClassType;
        ASSERT(1 == (bsl::is_same<MemberType, MEMBER>::value));
        ASSERT(1 == (bsl::is_same<ClassType, CLASS>::value));
    }
//..
// The following program should compile and run without errors:
//..
    void usageExample()
    {
        checkMemberPointer<int(int), MyTestClass>(&MyTestClass::func1);
        checkMemberPointer<int, MyTestClass>(&MyTestClass::d_int);
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::MemberPointerTraits'
        //   Ensure that the typedef 'MemberType' matches expectations for each
        //   tested member type, and Ensure that the typedef 'ClassType' is the
        //   type to which the member belongs.
        //
        // Concerns:
        //: 1 'pointerToMemberTraits::ClassType' is 'BASE' for all member
        //:   types.
        //:
        //: 2 'pointerToMemberTraits::MemberType' is as expected for all member
        //:   types.
        //:
        //: 3 A member of BASE of the corresponding type (including virtual and
        //:   non-virtual functions) can be assigned to the tested type.
        //
        // Plan:
        //   Assert correct types, and instantiate a local variable pointing to
        //   an actual member of all types to be tested.
        //
        // Testing:
        //   bslmf::MemberPointerTraits
        // --------------------------------------------------------------------
    if (verbose) {
        printf("TESTING 'bslmf::MemberPointerTraits\n");
        printf("===================================\n");
    }
    {
        typedef PtrToMemberInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_int; (void) x;
        ASSERT_SAME(int, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constInt; (void) x;
        ASSERT_SAME(const int, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatileInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatileInt; (void) x;
        ASSERT_SAME(volatile int, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatileInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatileInt; (void) x;
        ASSERT_SAME(const volatile int, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToInt_p; (void) x;
        ASSERT_SAME(int*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToConstInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToConstInt_p; (void) x;
        ASSERT_SAME(const int*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToVolatileInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToVolatileInt_p; (void) x;
        ASSERT_SAME(volatile int*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToConstVolatileInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToConstVolatileInt_p; (void) x;
        ASSERT_SAME(const volatile int*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstPtrToInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constPtrToInt_p; (void) x;
        ASSERT_SAME(int * const, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatilePtrToint TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatilePtrToInt_p; (void) x;
        ASSERT_SAME(int * volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatilePtrToInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatilePtrToInt_p; (void) x;
        ASSERT_SAME(int * const volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_class; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constClass; (void) x;
        ASSERT_SAME(const TEST_CLASS_TYPE, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatileClass; (void) x;
        ASSERT_SAME(volatile TEST_CLASS_TYPE, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatileClass; (void) x;
        ASSERT_SAME(const volatile TEST_CLASS_TYPE, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToClass_p; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToConstClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToConstClass_p; (void) x;
        ASSERT_SAME(const TEST_CLASS_TYPE*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToVolatileClass_p; (void) x;
        ASSERT_SAME(volatile TEST_CLASS_TYPE*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberPtrToConstVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_ptrToConstVolatileClass_p; (void) x;
        ASSERT_SAME(const volatile TEST_CLASS_TYPE*, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstPtrToClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constPtrToClass_p; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE * const, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatilePtrToClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatilePtrToClass_p; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE * volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatilePtrToClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatilePtrToClass_p; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE * const volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberArrayOfInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_arrayOfInt; (void) x;
        ASSERT_SAME(int  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstArrayOfInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constArrayOfInt; (void) x;
        ASSERT_SAME(const int  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatileArrayOfInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatileArrayOfInt; (void) x;
        ASSERT_SAME(volatile int  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatileArrayOfInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatileArrayOfInt; (void) x;
        ASSERT_SAME(const volatile int  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberArrayOfClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_arrayOfClass; (void) x;
        ASSERT_SAME(TEST_CLASS_TYPE  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstArrayOfClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constArrayOfClass; (void) x;
        ASSERT_SAME(const TEST_CLASS_TYPE  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatileArrayOfClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_volatileArrayOfClass; (void) x;
        ASSERT_SAME(volatile TEST_CLASS_TYPE  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatileArrayOfClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::d_constVolatileArrayOfClass; (void) x;
        ASSERT_SAME(const volatile TEST_CLASS_TYPE  [4], X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningInt; (void) x;
        TestType v = &BASE::virtualFuncReturningInt; (void) v;
        ASSERT_SAME(int(), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstFuncReturningInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::constFuncReturningInt; (void) x;
        TestType v = &BASE::virtualConstFuncReturningInt; (void) v;
        ASSERT_SAME(int   () const, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberVolatileFuncReturningInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::volatileFuncReturningInt; (void) x;
        TestType v = &BASE::virtualVolatileFuncReturningInt; (void) v;
        ASSERT_SAME(int () volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberConstVolatileFuncReturningInt TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::constVolatileFuncReturningInt; (void) x;
        TestType v = &BASE::virtualConstVolatileFuncReturningInt; (void) v;
        ASSERT_SAME(int () const volatile, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningClass; (void) x;
        TestType v = &BASE::virtualFuncReturningClass; (void) v;
        ASSERT_SAME(TEST_CLASS_TYPE (), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningConstClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningConstClass; (void) x;
        ASSERT_SAME(const TEST_CLASS_TYPE (), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningVolatileClass; (void) x;
        ASSERT_SAME(volatile TEST_CLASS_TYPE (), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningConstVolatileClass TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningConstVolatileClass; (void) x;
        ASSERT_SAME(const volatile TEST_CLASS_TYPE (), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningClassWithParam TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningClassWithParam; (void) x;
        TestType v = &BASE::virtualFuncReturningClassWithParam; (void) v;
        ASSERT_SAME(TEST_CLASS_TYPE (TEST_PARAMETER_0), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncReturningClassWithParams TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcReturningClassWithParams; (void) x;
        TestType v = &BASE::virtualFuncReturningClassWithParams; (void) v;
        ASSERT_SAME(TEST_CLASS_TYPE (TEST_PARAMETER_0, TEST_PARAMETER_1),
                X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncWithElipses TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcWithElipses; (void) x;
        ASSERT_SAME(int (...), X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    {
        typedef PtrToMemberFuncRValueRef TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcRValueRef; (void) x;
        ASSERT_SAME(int ()  &&, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
    {
        typedef PtrToMemberFuncLValueRef TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcLValueRef; (void) x;
        ASSERT_SAME(int () &, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES
    {
        typedef PtrToMemberFuncNoExcept TestType;
        typedef bslmf::MemberPointerTraits<TestType> X;
        TestType x = &BASE::funcNoExcept; (void) x;
        ASSERT_SAME(int () noexcept, X::MemberType);
        ASSERT_SAME(BASE, X::ClassType);
    }
#endif
      } break;
      case 1: {
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

        if (verbose) {
            printf("USAGE EXAMPLE\n");
            printf("=============\n");
        }

        usageExample();
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
