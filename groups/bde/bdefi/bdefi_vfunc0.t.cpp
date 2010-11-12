// bdefi_vfunc0.t.cpp              -*-C++-*-

#include <bdefi_vfunc0.h>

#include <bslma_default.h>           // for testing only
#include <bslma_testallocator.h>           // for testing only
#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()
using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Verify that the members are properly initialized during construction.
// Verify the correctness of the argument passing in the call to the 'execute'
// method.  Verify the correctness of the types of the parameters in the call
// to the 'execute' method.  Verify that an instance can be initialized with
// non-'const' member function for every 'bdefi_Vfunc0MN' class.  Also, verify
// that the destructors of the tested classes are called and that the memory
// allocator, specified at construction is invoked at deallocation.
//
// We use the following abbreviations:
//  'bA'   for 'bdema_Allcoator'
//  'A1-N'    for 'A1, A2, ... , AN'
//  'D1-N'    for 'D1, D2, ... , DN'
//  'c_A1'    for 'const A1'
//  'c_A1-N   for 'const A1, const A2, ... ,  const AN'
//  'c_A1-N&' for 'const A1&, const A2&, ... , const AN&'
//  'c_D1'    for 'const D1'
//  'c_D1-N'  for 'const D1, const D2, ... , const DN'
//  'c_D1-N&' for 'const D1&, const D2&, ... , const DN&'
//  'c_F'     for 'const F'
//  'c_C'     for 'const C'
//  'c_M'     for 'const M'
//  'c_O'     for 'const OBJ'
//-----------------------------------------------------------------------------
// [ 1] 'bdefi_Vfunc0Null' methods:
// [ 1] bdefi_Vfunc0Null(ba *);
// [ 1] void execute() const;
// [ 1] ~bdefi_Vfunc0Null();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0F1' methods:
// [ 1] bdefi_Vfunc0F1<F>(c_F&, ba *);
// [ 1] void execute<F>() const;
// [ 1] ~bdefi_Vfunc0F1<F>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C1' methods:
// [ 1] bdefi_Vfunc0C1<OBJ, C>( OBJ *, c_C&, ba *);
// [ 1] void execute<OBJ, C>() const;
// [ 1] ~bdefi_Vfunc0C1<OBJ, C>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M1' methods:
// [ 1] bdefi_Vfunc0M1<OBJ, M>( OBJ *, c_M&, ba *);
// [ 1] void execute<OBJ, M>() const;
// [ 1] ~bdefi_Vfunc0M1<OBJ, M>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0F2' methods:
// [ 2] bdefi_Vfunc0F2<F, D1>(c_F&, c_D1&, ba *);
// [ 2] void execute<F, D1>() const;
// [ 2] ~bdefi_Vfunc0F2<F, D1>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C2' methods:
// [ 2] bdefi_Vfunc0C2<OBJ, C, D1>( OBJ *, c_C&, c_D1&, ba *);
// [ 2] void execute<OBJ, C, D1>() const;
// [ 2] ~bdefi_Vfunc0C2<OBJ, C, D1>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M2' methods:
// [ 2] bdefi_Vfunc0M2<OBJ, M, D1>( OBJ *, c_M&, c_D1&, ba *);
// [ 2] void execute<OBJ, M, D1>() const;
// [ 2] ~bdefi_Vfunc0M2<OBJ, M, D1>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F3' methods:
// [ 3] bdefi_Vfunc0F3<F, D1-2>(c_F&, c_D1-2&, ba *);
// [ 3] void execute<F, D1-2>() const;
// [ 3] ~bdefi_Vfunc0F3<F, D1-2>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C3' methods:
// [ 3] bdefi_Vfunc0C3<OBJ, C, D1-2>( OBJ *, c_C&, c_D1-2&, ba *);
// [ 3] void execute<OBJ, C, D1-2>() const;
// [ 3] ~bdefi_Vfunc0C3<OBJ, C, D1-2>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M3' methods:
// [ 3] bdefi_Vfunc0M3<OBJ, M, D1-2>( OBJ *, c_M&, c_D1-2, ba *);
// [ 3] void execute<OBJ, M, D1-2>() const;
// [ 3] ~bdefi_Vfunc0M3<OBJ, M, D1-2>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F4' methods:
// [ 4] bdefi_Vfunc0F4<F, D1-3>(c_F&, c_D1-3&, ba *);
// [ 4] void execute<F, D1-3>() const;
// [ 4] ~bdefi_Vfunc0F4<F, D1-3>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C4' methods:
// [ 4] bdefi_Vfunc0C4<OBJ, C, D1-3>( OBJ *, c_C&, c_D1-3&, ba *);
// [ 4] void execute<OBJ, C, D1-3>() const;
// [ 4] ~bdefi_Vfunc0C4<OBJ, C, D1-3>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M4' methods:
// [ 4] bdefi_Vfunc0M4<OBJ, M, D1-3>( OBJ *, c_M&, c_D1-3, ba *);
// [ 4] void execute<OBJ, M, D1-3>() const;
// [ 4] ~bdefi_Vfunc0M4<OBJ, M, D1-3>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F5' methods:
// [ 5] bdefi_Vfunc0F5<F, D1-4>(c_F&, c_D1-4&, ba *);
// [ 5] void execute<F, D1-4>() const;
// [ 5] ~bdefi_Vfunc0F5<F, D1-4>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C5' methods:
// [ 5] bdefi_Vfunc0C5<OBJ, C, D1-4>( OBJ *, c_C&, c_D1-4&, ba *);
// [ 5] void execute<OBJ, C, D1-4>() const;
// [ 5] ~bdefi_Vfunc0C5<OBJ, C, D1-4>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M5' methods:
// [ 5] bdefi_Vfunc0M5<OBJ, M, D1-4>( OBJ *, c_M&, c_D1-4, ba *);
// [ 5] void execute<OBJ, M, D1-4>() const;
// [ 5] ~bdefi_Vfunc0M5<OBJ, M, D1-4>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F6' methods:
// [ 6] bdefi_Vfunc0F6<F, D1-5>(c_F&, c_D1-5&, ba *);
// [ 6] void execute<F, D1-5>() const;
// [ 6] ~bdefi_Vfunc0F6<F, D1-5>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C6' methods:
// [ 6] bdefi_Vfunc0C6<OBJ, C, D1-5>( OBJ *, c_C&, c_D1-5&, ba *);
// [ 6] void execute<OBJ, C, D1-5>() const;
// [ 6] ~bdefi_Vfunc0C6<OBJ, C, D1-5>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M6' methods:
// [ 6] bdefi_Vfunc0M6<OBJ, M, D1-5>( OBJ *, c_M&, c_D1-5, ba *);
// [ 6] void execute<OBJ, M, D1-5>() const;
// [ 6] ~bdefi_Vfunc0M6<OBJ, M, D1-5>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F7' methods:
// [ 7] bdefi_Vfunc0F7<F, D1-6>(c_F&, c_D1-6&, ba *);
// [ 7] void execute<F, D1-6>() const;
// [ 7] ~bdefi_Vfunc0F7<F, D1-6>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C7' methods:
// [ 7] bdefi_Vfunc0C7<OBJ, C, D1-6>( OBJ *, c_C&, c_D1-6&, ba *);
// [ 7] void execute<OBJ, C, D1-6>() const;
// [ 7] ~bdefi_Vfunc0C7<OBJ, C, D1-6>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M7' methods:
// [ 7] bdefi_Vfunc0M7<OBJ, M, D1-6>( OBJ *, c_M&, c_D1-6, ba *);
// [ 7] void execute<OBJ, M, D1-6>() const;
// [ 7] ~bdefi_Vfunc0M7<OBJ, M, D1-6>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F8' methods:
// [ 8] bdefi_Vfunc0F8<F, D1-7>(c_F&, c_D1-7&, ba *);
// [ 8] void execute<F, D1-7>() const;
// [ 8] ~bdefi_Vfunc0F8<F, D1-7>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C8' methods:
// [ 8] bdefi_Vfunc0C8<OBJ, C, D1-7>( OBJ *, c_C&, c_D1-7&, ba *);
// [ 8] void execute<OBJ, C, D1-7>() const;
// [ 8] ~bdefi_Vfunc0C8<OBJ, C, D1-7>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M8' methods:
// [ 8] bdefi_Vfunc0M8<OBJ, M, D1-7>( OBJ *, c_M&, c_D1-7, ba *);
// [ 8] void execute<OBJ, M, D1-7>() const;
// [ 8] ~bdefi_Vfunc0M8<OBJ, M, D1-7>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc0F9' methods:
// [ 9] bdefi_Vfunc0F9<F, D1-8>(c_F&, c_D1-8&, ba *);
// [ 9] void execute<F, D1-8>() const;
// [ 9] ~bdefi_Vfunc0F9<F, D1-8>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0C9' methods:
// [ 9] bdefi_Vfunc0C9<OBJ, C, D1-8>( OBJ *, c_C&, c_D1-8&, ba *);
// [ 9] void execute<OBJ, C, D1-8>() const;
// [ 9] ~bdefi_Vfunc0C9<OBJ, C, D1-8>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc0M9' methods:
// [ 9] bdefi_Vfunc0M9<OBJ, M, D1-8>( OBJ *, c_M&, c_D1-8, ba *);
// [ 9] void execute<OBJ, M, D1-8>() const;
// [ 9] ~bdefi_Vfunc0M9<OBJ, M, D1-8>();
//-----------------------------------------------------------------------------
// [11] USAGE EXAMPLE
// [10] ARGUMENT LENGTH LIMIT TEST

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
// This constant is used to verify that callback functions with zero arguments
// are being executed, as the callback functions will increment this variable.

    int a0_global;

// The following constants provide unique data used to verify the position
// correctness of functor arguments.  The constants are used in conjunction
// with the 'checkArgumentValues' family of helper functions.

    const int ARG0       = 256;
    const int ARG1       = 1;
    const int ARG2       = 2;
    const int ARG3       = 3;
    const int ARG4       = 4;
    const int ARG5       = 5;
    const int ARG6       = 6;
    const int ARG7       = 7;
    const int ARG8       = 8;
    const int ARG9       = 9;
    const int ARG1_PLUS1 = ARG1 + 1;
    const int ARG0_PLUS1 = ARG0 + 1;

// The following constants are of the form 2^N where N is in the range [0..8].
// These constants are used in conjuction with classes T1, T2, ... Tn, defined
// below to verify that the destructor of the class under the test was
// successfully called.

    const int T1_FLAG    = 1;
    const int T2_FLAG    = T1_FLAG *2;
    const int T3_FLAG    = T2_FLAG *2;
    const int T4_FLAG    = T3_FLAG *2;
    const int T5_FLAG    = T4_FLAG *2;
    const int T6_FLAG    = T5_FLAG *2;
    const int T7_FLAG    = T6_FLAG *2;
    const int T8_FLAG    = T7_FLAG *2;
    const int T9_FLAG    = T8_FLAG *2;


// The following constants are the partial sums of the sequence of numbers
// above.  They are used in conjuction with classes T1, T2, ... Tn, defined
// below, to detect whether a destructor of the object under test, which uses
// 'N' arguments of types T1, T2, ... Tn was called.

    const int DESTRUCTOR_FLAG1 = T1_FLAG;
    const int DESTRUCTOR_FLAG2 = T2_FLAG + DESTRUCTOR_FLAG1;
    const int DESTRUCTOR_FLAG3 = T3_FLAG + DESTRUCTOR_FLAG2;
    const int DESTRUCTOR_FLAG4 = T4_FLAG + DESTRUCTOR_FLAG3;
    const int DESTRUCTOR_FLAG5 = T5_FLAG + DESTRUCTOR_FLAG4;
    const int DESTRUCTOR_FLAG6 = T6_FLAG + DESTRUCTOR_FLAG5;
    const int DESTRUCTOR_FLAG7 = T7_FLAG + DESTRUCTOR_FLAG6;
    const int DESTRUCTOR_FLAG8 = T8_FLAG + DESTRUCTOR_FLAG7;
    const int DESTRUCTOR_FLAG9 = T9_FLAG + DESTRUCTOR_FLAG8;

// These global variables are used to test that the destructors of the classes
// below were called.
    int dcEnabled;          // Switch to enable/disable the test of destructor
    int destructorCheck;    // Counter used for testing the destructor

// The following set of classes provide unique types used to verify that the
// template parameter types were implemented correctly.  These classes are
// used as argument types in the 'checkParameterTypes' family of helper
// functions.
//
// In addition, these classes are used to test the correctness of the call to
// the destructor of the object under test.  In the destructor for each class,
// we add to the global variable 'destructorCheck' the corresponding (unique)
// TN_FLAG value for that class.  This allows verification that the destructor
// was called correctly by comparing an appropriate 'DESTRUCTOR_FLAG' constant
// with the value of 'destructorCheck'.  In order to eliminate "noise" due to
// the destruction of the temporary objects, we use global variable
// 'dcEnabled' to turn this operation on and off.

struct T1 {
    ~T1() { if(dcEnabled) destructorCheck += T1_FLAG; }
};

struct T2 {
    ~T2() { if(dcEnabled) destructorCheck += T2_FLAG; }
};

struct T3 {
    ~T3() { if(dcEnabled) destructorCheck += T3_FLAG; }
};

struct T4 {
    ~T4() { if(dcEnabled) destructorCheck += T4_FLAG; }
};

struct T5 {
    ~T5() { if(dcEnabled) destructorCheck += T5_FLAG; }
};

struct T6 {
    ~T6() { if(dcEnabled) destructorCheck += T6_FLAG; }
};

struct T7 {
    ~T7() { if(dcEnabled) destructorCheck += T7_FLAG; }
};

struct T8 {
    ~T8() { if(dcEnabled) destructorCheck += T8_FLAG; }
};

struct T9 {
    ~T9() { if(dcEnabled) destructorCheck += T9_FLAG; }
};

//=============================================================================
//                  CALLBACK CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
//
// We implement two sets of callback functions that check respectively (a) the
// correct positional matching between the arguments and function parameters,
// i.e., that the first argument is passed as the first parameter, the second
// as the second, etc.  and (b) the type correctness of the parameters.  We
// implement multiple variations of each of these two functions as a free
// function and as a 'const'/non-'const' member function, with the number of
// parameters ranging from one to nine.
//
// Here is a sample of each type of function:
//..
//   static void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4)
//       // Invoke the global 'ASSERT' macro to verify that the specified
//       // arguments 'a1', 'a2', 'a3', and 'a4' are equal to the global values
//       // 'ARG1', 'ARG2', 'ARG3', and 'ARG4, respectively.  Then
//       // increment the 'a' by one.
//       //
//       // This function is used to test the correctness of the passed in
//       // arguments.  The arguments passed in for the specified 'a1', 'a2',
//       // 'a3', and 'a4' are initialized with the values of 'ARG1', 'ARG2',
//       // 'ARG3', and 'ARG4' respectively.  If the arguments are passed in in
//       // the wrong order, the ASSERT statement will catch the bug.  The last
//       // statement incrementing '*a1' allows the caller to verify that the
//       // function was actually called and 'ASSERT' statements were in fact
//       // executed.
//   {
//       ASSERT(*a1 == ARG1);
//       ASSERT(*a2 == ARG2);
//       ASSERT(*a3 == ARG3);
//       ASSERT(*a4 == ARG4);
//       ++*a1;
//   }
//
//   static void checkParameterTypes4(T1, T2, T3, T4)
//       // Do nothing.
//       //
//       // This function is used to test the correctness of the argument types
//       // that 'bdefi_vfunc0X4' classes pass when calling the callback
//       // function.
//       //
//       // The specified types 'T1', 'T2', 'T3', and 'T4' are not convertible
//       // to each other or to or from any other type.  This uniqueness
//       // property allows the compiler to detect type mismatches between
//       // the types of the function parameters and the types of the arguments
//       // passed in.
//       //
//       // In addition, the uniqueness of the parameters is used to verify
//       // that the destructor of the object under test is properly executed.
//   {
//       return;
//   }
//..

//-----------------------------------------------------------------------------
//                           CALLBACK CLASSES
//-----------------------------------------------------------------------------

class CbClass {
  public:
    void checkVfunc0ExecutionC() const
    {
        ++a0_global;
    }

    void checkVfunc0ExecutionM() const
    {
        ++a0_global;
    }

    void checkArgumentValues1(int *a1) const
    {
        ASSERT(*a1 == ARG1);
        ++*a1;
    }

    void checkArgumentValues2(int *a1, int *a2) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ++*a1;
    }

    void checkArgumentValues3(int *a1, int *a2, int *a3) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ++*a1;
    }

    void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ++*a1;
    }

    void checkArgumentValues5(int *a1, int *a2, int *a3, int *a4,
                              int *a5) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ++*a1;
    }

    void checkArgumentValues6(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ++*a1;
    }

    void checkArgumentValues7(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ++*a1;
    }

    void checkArgumentValues8(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7, int *a8) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ASSERT(*a8 == ARG8);
        ++*a1;
    }

    void checkArgumentValues9(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7, int *a8,
                              int *a9) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ASSERT(*a8 == ARG8);
        ASSERT(*a9 == ARG9);
        ++*a1;
    }

    void checkParameterTypesC1(T1) const { }
    void checkParameterTypesC2(T1, T2) const { }
    void checkParameterTypesC3(T1, T2, T3) const { }
    void checkParameterTypesC4(T1, T2, T3, T4) const { }
    void checkParameterTypesC5(T1, T2, T3, T4, T5) const { }
    void checkParameterTypesC6(T1, T2, T3, T4, T5, T6) const { }
    void checkParameterTypesC7(T1, T2, T3, T4, T5, T6, T7) const { }
    void checkParameterTypesC8(T1, T2, T3, T4, T5, T6, T7, T8) const { }
    void checkParameterTypesC9(T1, T2, T3, T4, T5, T6, T7, T8, T9) const { }

    void checkParameterTypesM1(T1) { }
    void checkParameterTypesM2(T1, T2) { }
    void checkParameterTypesM3(T1, T2, T3) { }
    void checkParameterTypesM4(T1, T2, T3, T4) { }
    void checkParameterTypesM5(T1, T2, T3, T4, T5) { }
    void checkParameterTypesM6(T1, T2, T3, T4, T5, T6) { }
    void checkParameterTypesM7(T1, T2, T3, T4, T5, T6, T7) { }
    void checkParameterTypesM8(T1, T2, T3, T4, T5, T6, T7, T8) { }
    void checkParameterTypesM9(T1, T2, T3, T4, T5, T6, T7, T8, T9) { }
};

//-----------------------------------------------------------------------------
//                           CALLBACK FREE FUNCTIONS
//-----------------------------------------------------------------------------

void checkVfunc0ExecutionF()
{
    ++a0_global;
}

void checkArgumentValues1(int *a1)
{
    ASSERT(*a1 == ARG1);
    ++*a1;
}

void checkArgumentValues2(int *a1, int *a2)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ++*a1;
}

void checkArgumentValues3(int *a1, int *a2, int *a3)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ++*a1;
}

void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ++*a1;
}

void checkArgumentValues5(int *a1, int *a2, int *a3, int *a4, int *a5)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ++*a1;
}

void checkArgumentValues6(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ++*a1;
}

void checkArgumentValues7(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ++*a1;
}

void checkArgumentValues8(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7, int *a8)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ASSERT(*a8 == ARG8);
    ++*a1;
}

void checkArgumentValues9(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7, int *a8, int *a9)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ASSERT(*a8 == ARG8);
    ASSERT(*a9 == ARG9);
    ++*a1;
}

void checkParameterTypes1(T1) { }
void checkParameterTypes2(T1, T2) { }
void checkParameterTypes3(T1, T2, T3) { }
void checkParameterTypes4(T1, T2, T3, T4) { }
void checkParameterTypes5(T1, T2, T3, T4, T5) { }
void checkParameterTypes6(T1, T2, T3, T4, T5, T6) { }
void checkParameterTypes7(T1, T2, T3, T4, T5, T6, T7) { }
void checkParameterTypes8(T1, T2, T3, T4, T5, T6, T7, T8) { }
void checkParameterTypes9(T1, T2, T3, T4, T5, T6, T7, T8, T9) { }

//=============================================================================
//  GLOBAL FUNCTIONS USED TO TEST THAT IDENTIFIER LENGTH COULD BE > 255 CHARS
//=============================================================================

    class rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr {
    };

    typedef rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr rrr;

    int rrrrr1 = 0;
    void rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr1(rrr *a1, rrr *a2, rrr *a3, rrr *a4,
                                         rrr *a5, rrr *a6, rrr *a7, rrr *a8,
                                         rrr *a9)
    {
        ++rrrrr1;
    }

    int rrrrr2 = 0;
    void rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr2(rrr *a1, rrr *a2, rrr *a3, rrr *a4,
                                         rrr *a5, rrr *a6, rrr *a7, rrr *a8,
                                         int *a9)
    {
        ++rrrrr2;
    }

//=============================================================================
//                    GLOBAL FUNCTION FOR USAGE EXAMPLE TEST
//-----------------------------------------------------------------------------

int myFunction(int a, double b, const char *c, int d)
    // Do nothing.
    //
    // This function is used to test the correctness of the 'usage' example.
{
    return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator ta(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;
        {
            int myFunction(int a, double b, const char *c, int d);
            typedef int (*MyFunctionPtr)(int, double, const char *, int);
            typedef bdefi_Vfunc0F4<MyFunctionPtr,
                        int, double, const char *, int> MyFunctorRep;
            bdefr_Vfunc0 *rep = new(ta) MyFunctorRep(
                                               myFunction, 1, 10, "3", 4, &ta);
            rep->execute();
            bdefr_Vfunc0::deleteObject(rep);
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ARGUMENT LENGTH COULD BE > 255 CHARS
        //   This problem occurs when using VC++ 6.0 compiler.  The compiler
        //   complains that the identifier length is > 255 characters.  The
        //   'pragma' in 'bdefi_vfunc0.h' fixes this problem.  This test
        //   verifies that the warnings are not just suppressed, and that
        //   compiler can actually distinguish between variables whose type
        //   difference starts after 255th character of the type name.
        //
        // Plan:
        //   Create two variables of the similar types so that the type
        //   identifiers have first 255 characters in common.  Verify that the
        //   correct function is called.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;


        {
            typedef void (*CbFuncF9)(rrr *, rrr *, rrr *, rrr *,
                                     rrr *, rrr *, rrr *, rrr *, rrr *);
            typedef void (*CbFuncF10)(rrr *, rrr *, rrr *, rrr *,
                                     rrr *, rrr *, rrr *, rrr *, int *);

            typedef bdefr_Vfunc0 ObjBase;

            typedef bdefi_Vfunc0F9<CbFuncF9,
                                       rrr *, rrr *, rrr *, rrr *,
                                       rrr *, rrr *, rrr *, rrr *, rrr *> Obj1;

            typedef bdefi_Vfunc0F9<CbFuncF10,
                                       rrr *, rrr *, rrr *, rrr *,
                                       rrr *, rrr *, rrr *, rrr *, int *> Obj2;

            rrr a1;
            int a2 = 12;

            ObjBase *x1 = new(ta) Obj1(rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr1, &a1,
                                        &a1, &a1, &a1, &a1, &a1, &a1, &a1,
                                        &a1, &ta);

            ObjBase *x2 = new(ta) Obj2(rrrrrrrrrrrrrrrrrrrrrrrrrrrrrr2, &a1,
                                        &a1, &a1, &a1, &a1, &a1, &a1,
                                        &a1, &a2, &ta);

            ASSERT(0 == rrrrr1);
            ASSERT(0 == rrrrr2);
            x1->execute();
            ASSERT(1 == rrrrr1);
            ASSERT(0 == rrrrr2);
            x2->execute();
            ASSERT(1 == rrrrr1);
            ASSERT(1 == rrrrr2);

            ObjBase::deleteObject(x1);
            ObjBase::deleteObject(x2);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 9 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M9' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X9' class.  Initialize
        //   'bdefi_Vfunc0F9' to have 'checkArgumentValues9' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C9' and 'bdefi_Vfunc0M9' to
        //   have 'checkArgumentValues9' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X9' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4',
        //   'ARG5', 'ARG6', 'ARG7', 'ARG8', and 'ARG9' respectively.  When
        //   called, the 'checkArgumentValues9' will emit an error, if the
        //   value of the first argument is not the same as the value of
        //   'ARG1', value of the second argument is not equal to the value of
        //   'ARG2', etc.  Verify that the method was called by checking that
        //   the first argument passed in the 'execute' method was incremented
        //   to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X9' class.  Initialize
        //   'bdefi_Vfunc0F9' to have 'checkParameterTypes9' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C9' to have
        //   'checkParameterTypesC9' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M9' to have
        //   'checkParameterTypesM9' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M9' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X9' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F9' methods:
        //   bdefi_Vfunc0F9<F, D1-9>(c_F&, c_D1-9&, ba *);
        //   void execute<F, D1-9>() const;
        //   ~bdefi_Vfunc0F9<F, D1-9>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0C9' methods:
        //   bdefi_Vfunc0C9<OBJ, C, D1-9>( OBJ *, c_C&, c_D1-9&, ba *);
        //   void execute<OBJ, C, D1-9>() const;
        //   ~bdefi_Vfunc0C9<OBJ, C, D1-9>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M9' methods:
        //   bdefi_Vfunc0M9<OBJ, M, D1-9>( OBJ *, c_M&, c_D1-9, ba *);
        //    void execute<OBJ, M, D1-9>() const;
        //   ~bdefi_Vfunc0M9<OBJ, M, D1-9>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 9 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF9)(int *, int *, int *, int *,
                                     int *, int *, int *, int *, int *);
            typedef void (CbClass::*CbFuncC9)(int *, int *, int *, int *,
                                              int *, int *, int *, int *,
                                              int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;
            int a8 = ARG8;
            int a9 = ARG9;

            if (verbose) cout << "\t'bdefi_Vfunc0F9' object." << endl;
            {
                typedef bdefi_Vfunc0F9<CbFuncF9,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues9, &a1, &a2, &a3,
                                          &a4, &a5, &a6, &a7, &a8, &a9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C9' object." << endl;
            {
                typedef bdefi_Vfunc0C9<CbClass, CbFuncC9, int *, int *,
                                       int *, int *, int *, int *, int *,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues9, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7, &a8,
                                          &a9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M9' object." << endl;
            {
                typedef bdefi_Vfunc0M9<CbClass, CbFuncC9,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues9, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7, &a8,
                                          &a9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF9)(T1, T2, T3, T4, T5, T6, T7, T8, T9);
            typedef void (CbClass::*CbFuncC9)(T1, T2, T3, T4, T5, T6, T7, T8,
                                              T9) const;
            typedef void (CbClass::*CbFuncM9)(T1, T2, T3, T4, T5, T6, T7, T8,
                                              T9);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;
            T7 a7;
            T8 a8;
            T9 a9;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F9<CbFuncF9, T1, T2, T3, T4, T5, T6, T7,
                                       T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes9, a1, a2, a3,
                                          a4, a5, a6, a7, a8, a9, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C9<CbClass, CbFuncC9, T1, T2,
                                       T3, T4, T5, T6, T7, T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC9, a1,
                                         a2, a3, a4, a5, a6, a7, a8, a9, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M9<CbClass, CbFuncM9, T1, T2, T3, T4,
                                       T5, T6, T7, T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM9, a1,
                                         a2, a3, a4, a5, a6, a7, a8, a9, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 8 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M8' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X8' class.  Initialize
        //   'bdefi_Vfunc0F8' to have 'checkArgumentValues8' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C8' and 'bdefi_Vfunc0M8' to
        //   have 'checkArgumentValues8' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X8' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4',
        //   'ARG5', 'ARG6', 'ARG7', and 'ARG8' respectively.  When called,
        //   the 'checkArgumentValues8' will emit an error, if the value of
        //   the first argument is not the same as the value of 'ARG1', value
        //   of the second argument is not equal to the value of 'ARG2', etc.
        //   Verify that the method was called by checking that the first
        //   argument passed in the 'execute' method was incremented to the
        //   value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X8' class.  Initialize
        //   'bdefi_Vfunc0F8' to have 'checkParameterTypes8' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C8' to have
        //   'checkParameterTypesC8' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M8' to have
        //   'checkParameterTypesM8' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M8' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X8' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F8' methods:
        //   bdefi_Vfunc0F8<F, D1-8>(c_F&, c_D1-8&, ba *);
        //   void execute<F, D1-8>() const;
        //   ~bdefi_Vfunc0F8<F, D1-8>();
        // -------------------------------------------------------------------
        //   'bdefi_Vfunc0C8' methods:
        //   bdefi_Vfunc0C8<OBJ, C, D1-8>( OBJ *, c_C&, c_D1-8&, ba *);
        //   void execute<OBJ, C, D1-8>() const;
        //   ~bdefi_Vfunc0C8<OBJ, C, D1-8>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M8' methods:
        //   bdefi_Vfunc0M8<OBJ, M, D1-8>( OBJ *, c_M&, c_D1-8, ba *);
        //    void execute<OBJ, M, D1-8>() const;
        //   ~bdefi_Vfunc0M8<OBJ, M, D1-8>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 8 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF8)(int *, int *, int *, int *,
                                     int *, int *, int *, int *);
            typedef void (CbClass::*CbFuncC8)(int *, int *, int *, int *,
                                              int *, int *, int *,
                                              int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;
            int a8 = ARG8;

            if (verbose) cout << "\t'bdefi_Vfunc0F8' object." << endl;
            {
                typedef bdefi_Vfunc0F8<CbFuncF8,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues8, &a1, &a2, &a3,
                                          &a4, &a5, &a6, &a7, &a8, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C8' object." << endl;
            {
                typedef bdefi_Vfunc0C8<CbClass, CbFuncC8, int *, int *,
                                       int *, int *, int *, int *,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues8, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7,
                                          &a8, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M8' object." << endl;
            {
                typedef bdefi_Vfunc0M8<CbClass, CbFuncC8,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues8, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7,
                                          &a8, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF8)(T1, T2, T3, T4, T5, T6, T7, T8);
            typedef void (CbClass::*CbFuncC8)(T1, T2, T3, T4, T5, T6, T7,
                                              T8) const;
            typedef void (CbClass::*CbFuncM8)(T1, T2, T3, T4, T5, T6, T7, T8);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;
            T7 a7;
            T8 a8;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F8' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F8<CbFuncF8, T1, T2, T3, T4, T5, T6, T7,
                                       T8> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes8, a1, a2, a3,
                                          a4, a5, a6, a7, a8, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG8 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C8' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C8<CbClass, CbFuncC8, T1, T2,
                                       T3, T4, T5, T6, T7, T8> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC8,
                                         a1, a2, a3, a4, a5, a6, a7, a8, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG8 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M8' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M8<CbClass, CbFuncM8, T1, T2, T3, T4,
                                       T5, T6, T7, T8> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM8,
                                         a1, a2, a3, a4, a5, a6, a7, a8, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG8 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 7 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M7' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X7' class.  Initialize
        //   'bdefi_Vfunc0F7' to have 'checkArgumentValues7' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C7' and 'bdefi_Vfunc0M7' to
        //   have 'checkArgumentValues7' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X7' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4',
        //   'ARG5', 'ARG6', and 'ARG7' respectively.  When called, the
        //   'checkArgumentValues7' will emit an error, if the value of the
        //   first argument is not the same as the value of 'ARG1', value of
        //   the second argument is not equal to the value of 'ARG2', etc.
        //   Verify that the method was called by checking that the first
        //   argument passed in the 'execute' method was incremented to the
        //   value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X7' class.  Initialize
        //   'bdefi_Vfunc0F7' to have 'checkParameterTypes7' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C7' to have
        //   'checkParameterTypesC7' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M7' to have
        //   'checkParameterTypesM7' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M7' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X7' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F7' methods:
        //   bdefi_Vfunc0F7<F, D1-7>(c_F&, c_D1-7&, ba *);
        //   void execute<F, D1-7>() const;
        //   ~bdefi_Vfunc0F7<F, D1-7>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0C7' methods:
        //   bdefi_Vfunc0C7<OBJ, C, D1-7>( OBJ *, c_C&, c_D1-7&, ba *);
        //   void execute<OBJ, C, D1-7>() const;
        //   ~bdefi_Vfunc0C7<OBJ, C, D1-7>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M7' methods:
        //   bdefi_Vfunc0M7<OBJ, M, D1-7>( OBJ *, c_M&, c_D1-7, ba *);
        //   void execute<OBJ, M, D1-7>() const;
        //   ~bdefi_Vfunc0M7<OBJ, M, D1-7>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 7 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF7)(int *, int *, int *,
                                     int *, int *, int *, int *);
            typedef void (CbClass::*CbFuncC7)(int *, int *, int *,
                                              int *, int *, int *,
                                              int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;

            if (verbose) cout << "\t'bdefi_Vfunc0F7' object." << endl;
            {
                typedef bdefi_Vfunc0F7<CbFuncF7,
                                       int *, int *, int *,
                                       int *, int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues7, &a1, &a2, &a3,
                                          &a4, &a5, &a6, &a7, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C7' object." << endl;
            {
                typedef bdefi_Vfunc0C7<CbClass, CbFuncC7, int *,
                                       int *, int *, int *, int *,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues7, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M7' object." << endl;
            {
                typedef bdefi_Vfunc0M7<CbClass, CbFuncC7,
                                       int *, int *, int *,
                                       int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues7, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &a7, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF7)(T1, T2, T3, T4, T5, T6, T7);
            typedef void (CbClass::*CbFuncC7)(T1, T2, T3, T4, T5, T6,
                                              T7) const;
            typedef void (CbClass::*CbFuncM7)(T1, T2, T3, T4, T5, T6, T7);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;
            T7 a7;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F7' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F7<CbFuncF7, T1, T2, T3, T4, T5, T6,
                                       T7> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes7, a1, a2, a3,
                                          a4, a5, a6, a7, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG7 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C7' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C7<CbClass, CbFuncC7, T1, T2,
                                       T3, T4, T5, T6, T7> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC7,
                                         a1, a2, a3, a4, a5, a6, a7, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG7 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M7' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M7<CbClass, CbFuncM7, T1, T2, T3, T4,
                                       T5, T6, T7> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM7,
                                         a1, a2, a3, a4, a5, a6, a7, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG7 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 6 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M6' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X6' class.  Initialize
        //   'bdefi_Vfunc0F6' to have 'checkArgumentValues6' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C6' and 'bdefi_Vfunc0M6' to
        //   have 'checkArgumentValues6' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X6' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4',
        //   'ARG5', and 'ARG6' respectively.  When called, the
        //   'checkArgumentValues6' will emit an error, if the value of the
        //   first argument is not the same as the value of 'ARG1', value of
        //   the second argument is not equal to the value of 'ARG2', etc.
        //   Verify that the method was called by checking that the first
        //   argument passed in the 'execute' method was incremented to the
        //   value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X6' class.  Initialize
        //   'bdefi_Vfunc0F6' to have 'checkParameterTypes6' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C6' to have
        //   'checkParameterTypesC6' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M6' to have
        //   'checkParameterTypesM6' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M6' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X6' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F6' methods:
        //   bdefi_Vfunc0F6<F, D1-6>(c_F&, c_D1-6&, ba *);
        //   void execute<F, D1-6>() const;
        //   ~bdefi_Vfunc0F6<F, D1-6>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0C6' methods:
        //   bdefi_Vfunc0C6<OBJ, C, D1-6>( OBJ *, c_C&, c_D1-6&, ba *);
        //   void execute<OBJ, C, D1-6>() const;
        //   ~bdefi_Vfunc0C6<OBJ, C, D1-6>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M6' methods:
        //   bdefi_Vfunc0M6<OBJ, M, D1-6>( OBJ *, c_M&, c_D1-6, ba *);
        //    void execute<OBJ, M, D1-6>() const;
        //   ~bdefi_Vfunc0M6<OBJ, M, D1-6>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 6 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF6)(int *, int *, int *,
                                     int *, int *, int *);
            typedef void (CbClass::*CbFuncC6)(int *, int *, int *,
                                              int *, int *, int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;

            if (verbose) cout << "\t'bdefi_Vfunc0F6' object." << endl;
            {
                typedef bdefi_Vfunc0F6<CbFuncF6,
                                       int *, int *, int *,
                                       int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues6, &a1, &a2, &a3,
                                          &a4, &a5, &a6, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C6' object." << endl;
            {
                typedef bdefi_Vfunc0C6<CbClass, CbFuncC6, int *,
                                       int *, int *, int *,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues6, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M6' object." << endl;
            {
                typedef bdefi_Vfunc0M6<CbClass, CbFuncC6,
                                       int *, int *, int *,
                                       int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues6, &a1,
                                          &a2, &a3, &a4, &a5, &a6, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF6)(T1, T2, T3, T4, T5, T6);
            typedef void (CbClass::*CbFuncC6)(T1, T2, T3, T4, T5, T6) const;
            typedef void (CbClass::*CbFuncM6)(T1, T2, T3, T4, T5, T6);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F6' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F6<CbFuncF6, T1, T2, T3, T4, T5, T6> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes6, a1, a2, a3,
                                          a4, a5, a6, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG6 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C6' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C6<CbClass, CbFuncC6, T1, T2,
                                       T3, T4, T5, T6> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC6,
                                         a1, a2, a3, a4, a5, a6, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG6 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M6' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M6<CbClass, CbFuncM6, T1, T2, T3, T4,
                                       T5, T6> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM6,
                                         a1, a2, a3, a4, a5, a6, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG6 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 5 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M5' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X5' class.  Initialize
        //   'bdefi_Vfunc0F5' to have 'checkArgumentValues5' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C5' and 'bdefi_Vfunc0M5' to
        //   have 'checkArgumentValues5' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X5' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4', and
        //   'ARG5' respectively.  When called, the 'checkArgumentValues5'
        //   will emit an error, if the value of the first argument is not the
        //   same as the value of 'ARG1', value of the second argument is not
        //   equal to the value of 'ARG2', etc.  Verify that the method was
        //   called by checking that the first argument passed in the
        //   'execute' method was incremented to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X5' class.  Initialize
        //   'bdefi_Vfunc0F5' to have 'checkParameterTypes5' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C5' to have
        //   'checkParameterTypesC5' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M5' to have
        //   'checkParameterTypesM5' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M5' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X5' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F5' methods:
        //   bdefi_Vfunc0F5<F, D1-5>(c_F&, c_D1-5&, ba *);
        //   void execute<F, D1-5>() const;
        //   ~bdefi_Vfunc0F5<F, D1-5>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0C5' methods:
        //   bdefi_Vfunc0C5<OBJ, C, D1-5>( OBJ *, c_C&, c_D1-5&, ba *);
        //   void execute<OBJ, C, D1-5>() const;
        //   ~bdefi_Vfunc0C5<OBJ, C, D1-5>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M5' methods:
        //   bdefi_Vfunc0M5<OBJ, M, D1-5>( OBJ *, c_M&, c_D1-5, ba *);
        //   void execute<OBJ, M, D1-5>() const;
        //   ~bdefi_Vfunc0M5<OBJ, M, D1-5>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 5 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF5)(int *, int *, int *, int *, int *);
            typedef void (CbClass::*CbFuncC5)(int *, int *,
                                              int *, int *, int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;

            if (verbose) cout << "\t'bdefi_Vfunc0F5' object." << endl;
            {
                typedef bdefi_Vfunc0F5<CbFuncF5, int *, int *,
                                       int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues5, &a1, &a2, &a3,
                                          &a4, &a5, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C5' object." << endl;
            {
                typedef bdefi_Vfunc0C5<CbClass, CbFuncC5, int *,
                                       int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues5, &a1,
                                          &a2, &a3, &a4, &a5, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M5' object." << endl;
            {
                typedef bdefi_Vfunc0M5<CbClass, CbFuncC5,
                                       int *, int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues5, &a1,
                                          &a2, &a3, &a4, &a5, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF5)(T1, T2, T3, T4, T5);
            typedef void (CbClass::*CbFuncC5)(T1, T2, T3, T4, T5) const;
            typedef void (CbClass::*CbFuncM5)(T1, T2, T3, T4, T5);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F5' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F5<CbFuncF5, T1, T2, T3, T4, T5> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes5, a1, a2, a3,
                                          a4, a5, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG5 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C5' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C5<CbClass, CbFuncC5, T1, T2,
                                       T3, T4, T5> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC5,
                                         a1, a2, a3, a4, a5, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG5 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M5' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M5<CbClass, CbFuncM5, T1, T2, T3, T4,
                                       T5> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM5,
                                         a1, a2, a3, a4, a5, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG5 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 4 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M4' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X4' class.  Initialize
        //   'bdefi_Vfunc0F4' to have 'checkArgumentValues4' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C4' and 'bdefi_Vfunc0M4' to
        //   have 'checkArgumentValues4' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X4' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', and 'ARG4'
        //   respectively.  When called, the 'checkArgumentValues4' will emit
        //   an error, if the value of the first argument is not the same as
        //   the value of 'ARG1', value of the second argument is not equal to
        //   the value of 'ARG2', etc.  Verify that the method was called by
        //   checking that the first argument passed in the 'execute' method
        //   was incremented to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X4' class.  Initialize
        //   'bdefi_Vfunc0F4' to have 'checkParameterTypes4' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C4' to have
        //   'checkParameterTypesC4' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M4' to have
        //   'checkParameterTypesM4' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M4' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X4' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F4' methods:
        //   bdefi_Vfunc0F4<F, D1-4>(c_F&, c_D1-4&, ba *);
        //   void execute<F, D1-4>() const;
        //   ~bdefi_Vfunc0F4<F, D1-4>();
        //  -------------------------------------------------------------------
        //   'bdefi_Vfunc0C4' methods:
        //   bdefi_Vfunc0C4<OBJ, C, D1-4>( OBJ *, c_C&, c_D1-4&, ba *);
        //   void execute<OBJ, C, D1-4>() const;
        //   ~bdefi_Vfunc0C4<OBJ, C, D1-4>();
        //  -------------------------------------------------------------------
        //   'bdefi_Vfunc0M4' methods:
        //   bdefi_Vfunc0M4<OBJ, M, D1-4>( OBJ *, c_M&, c_D1-4, ba *);
        //   void execute<OBJ, M, D1-4>() const;
        //   ~bdefi_Vfunc0M4<OBJ, M, D1-4>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 4 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF4)(int *, int *, int *, int *);
            typedef void (CbClass::*CbFuncC4)(int *, int *,
                                              int *, int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;

            if (verbose) cout << "\t'bdefi_Vfunc0F4' object." << endl;
            {
                typedef bdefi_Vfunc0F4<CbFuncF4,
                                       int *, int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues4, &a1, &a2,
                                         &a3, &a4, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C4' object." << endl;
            {
                typedef bdefi_Vfunc0C4<CbClass, CbFuncC4,
                                       int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkArgumentValues4,
                                         &a1, &a2, &a3, &a4, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M4' object." << endl;
            {
                typedef bdefi_Vfunc0M4<CbClass, CbFuncC4,
                                       int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkArgumentValues4,
                                         &a1, &a2, &a3, &a4, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF4)(T1, T2, T3, T4);
            typedef void (CbClass::*CbFuncC4)(T1, T2, T3, T4) const;
            typedef void (CbClass::*CbFuncM4)(T1, T2, T3, T4);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F4' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F4<CbFuncF4, T1, T2, T3, T4> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes4,
                                          a1, a2, a3, a4, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG4 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C4' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C4<CbClass, CbFuncC4, T1, T2,
                                       T3, T4> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC4,
                                         a1, a2, a3, a4, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG4 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M4' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M4<CbClass, CbFuncM4, T1, T2, T3, T4> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM4,
                                         a1, a2, a3, a4, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG4 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 3 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M3' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X3' class.  Initialize
        //   'bdefi_Vfunc0F3' to have 'checkArgumentValues3' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C3' and 'bdefi_Vfunc0M3' to
        //   have 'checkArgumentValues3' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X3' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', and 'ARG3'
        //   respectively.  When called, the 'checkArgumentValues3' will emit
        //   an error, if the value of the first argument is not the same as
        //   the value of 'ARG1', value of the second argument is not equal to
        //   the value of 'ARG2', etc.  Verify that the method was called by
        //   checking that the first argument passed in the 'execute' method
        //   was incremented to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X3' class.  Initialize
        //   'bdefi_Vfunc0F3' to have 'checkParameterTypes3' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C3' to have
        //   'checkParameterTypesC3' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M3' to have
        //   'checkParameterTypesM3' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M3' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X3' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F3' methods:
        //   bdefi_Vfunc0F3<F, D1-3>(c_F&, c_D1-3&, ba *);
        //   void execute<F, D1-3>() const;
        //   ~bdefi_Vfunc0F3<F, D1-3>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0C3' methods:
        //   bdefi_Vfunc0C3<OBJ, C, D1-3>( OBJ *, c_C&, c_D1-3&, ba *);
        //   void execute<OBJ, C, D1-3>() const;
        //   ~bdefi_Vfunc0C3<OBJ, C, D1-3>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M3' methods:
        //   bdefi_Vfunc0M3<OBJ, M, D1-3>( OBJ *, c_M&, c_D1-3, ba *);
        //   void execute<OBJ, M, D1-3>() const;
        //   ~bdefi_Vfunc0M3<OBJ, M, D1-3>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 3 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF3)(int *, int *, int *);
            typedef void (CbClass::*CbFuncC3)(int *, int *, int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;

            if (verbose) cout << "\t'bdefi_Vfunc0F3' object." << endl;
            {
                typedef bdefi_Vfunc0F3<CbFuncF3,
                                       int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues3, &a1,
                                          &a2, &a3, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C3' object." << endl;
            {
                typedef bdefi_Vfunc0C3<CbClass, CbFuncC3,
                                       int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkArgumentValues3,
                                         &a1, &a2, &a3, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M3' object." << endl;
            {
                typedef bdefi_Vfunc0M3<CbClass, CbFuncC3,
                                       int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkArgumentValues3,
                                         &a1, &a2, &a3, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF3)(T1, T2, T3);
            typedef void (CbClass::*CbFuncC3)(T1, T2, T3) const;
            typedef void (CbClass::*CbFuncM3)(T1, T2, T3);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;
            T3 a3;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F3' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F3<CbFuncF3, T1, T2, T3> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes3, a1,
                                          a2, a3, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG3 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C3' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C3<CbClass, CbFuncC3, T1, T2,
                                       T3> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC3,
                                         a1, a2, a3, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG3 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M3' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M3<CbClass, CbFuncM3, T1, T2, T3> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM3,
                                         a1, a2, a3, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG3 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 2 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M2' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X2' class.  Initialize
        //   'bdefi_Vfunc0F2' to have 'checkArgumentValues2' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C2' and 'bdefi_Vfunc0M2' to
        //   have 'checkArgumentValues2' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X2' class.  The arguments in this call are
        //   initialized with values of 'ARG1' and 'ARG2' respectively.  When
        //   called, the 'checkArgumentValues2' will emit an error, if the
        //   value of the first argument is not the same as the value of
        //   'ARG1', value of the second argument is not equal to the value of
        //   'ARG2', etc.  Verify that the method was called by checking that
        //   the first argument passed in the 'execute' method was incremented
        //   to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X2' class.  Initialize
        //   'bdefi_Vfunc0F2' to have 'checkParameterTypes2' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C2' to have
        //   'checkParameterTypesC2' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M2' to have
        //   'checkParameterTypesM2' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M2' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X2' to be templatized so that each
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of arguments passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        // Testing:
        //   'bdefi_Vfunc0F2' methods:
        //   bdefi_Vfunc0F2<F, D1-2>(c_F&, c_D1-2&, ba *);
        //   void execute<F, D1-2>(D1-2&) const;
        //   ~bdefi_Vfunc0F2<F, D1-2>();
        // -------------------------------------------------------------------
        //   'bdefi_Vfunc0C2' methods:
        //   bdefi_Vfunc0C2<OBJ, C, D1-2>( OBJ *, c_C&, c_D1-2&, ba *);
        //   void execute<OBJ, C, D1-2>() const;
        //   ~bdefi_Vfunc0C2<OBJ, C, D1-2>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M2' methods:
        //   bdefi_Vfunc0M2<OBJ, M, D1-2>( OBJ *, c_M&, c_D1-2&, ba *);
        //   void execute<OBJ, M, D1-2>() const;
        //   ~bdefi_Vfunc0M2<OBJ, M, D1-2>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 2 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF2)(int *, int *);
            typedef void (CbClass::*CbFuncC2)(int *, int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;

            if (verbose) cout << "\t'bdefi_Vfunc0F2' object." << endl;
            {
                typedef bdefi_Vfunc0F2<CbFuncF2,
                                       int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues2, &a1, &a2, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C2' object." << endl;
            {
                typedef bdefi_Vfunc0C2<CbClass, CbFuncC2,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues2, &a1,
                                          &a2, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M2' object." << endl;
            {
                typedef bdefi_Vfunc0M2<CbClass, CbFuncC2,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues2, &a1,
                                          &a2, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF2)(T1, T2);
            typedef void (CbClass::*CbFuncC2)(T1, T2) const;
            typedef void (CbClass::*CbFuncM2)(T1, T2);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;

            T1 a1;
            T2 a2;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F2' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F2<CbFuncF2, T1, T2> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes2, a1, a2, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG2 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C2' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C2<CbClass, CbFuncC2, T1, T2> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkParameterTypesC2, a1,
                                          a2, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG2 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M2' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M2<CbClass, CbFuncM2, T1, T2> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkParameterTypesM2, a1,
                                          a2, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG2 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR REPRESENTATIONS WITH 1 and 0 PARAMETERS
        //
        //   We have the following specific concerns:
        //    1. An instance of each class in this component can be created
        //       and that its members are correctly initialized.
        //    2. The 'execute' method for each class is called and the
        //       arguments are passed in as expected.
        //    3. The types of the arguments to the callback function match
        //       those intended as argument types for the function parameters.
        //    4. Non-'const' member functions can be used as callback
        //       functions by 'bdefi_Vfunc0M1' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc0X1' class.  Initialize
        //   'bdefi_Vfunc0F1' to have 'checkArgumentValues1' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C1' and 'bdefi_Vfunc0M1' to
        //   have 'checkArgumentValues1' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc0X1' class.  The argument in this call is initialized
        //   with value of 'ARG1'.  When called, the 'checkArgumentValues1'
        //   will emit an error, if the value of the first argument is not the
        //   same as the value of 'ARG1'.  Verify that the method was called
        //   by checking that the argument passed in the 'execute' method was
        //   incremented and is equal to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc0X1' class.  Initialize
        //   'bdefi_Vfunc0F1' to have 'checkParameterTypes1' as the callback
        //   function.  Initialize 'bdefi_Vfunc0C1' to have
        //   'checkParameterTypesC1' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc0M1' to have
        //   'checkParameterTypesM1' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc0M1' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc0X1' to be templatized so that the
        //   argument to the callback function will be of unique type.  Hence,
        //   if the types of argument passed in to the callback function is
        //   incorrect, the compiler will generate an error.
        //
        //   The uniqueness of the parameters of 'checkParameterTypes' also
        //   allows us to test that the destructors of the classes under test
        //   were properly executed.  For a detailed explanation of how this
        //   testing is done see the description of 'T1', 'T2' ... 'Tn'
        //   classes.
        //
        //   For 'bdefi_Vfunc0X0' class pass 'checkVfunc0Execution' function as
        //   a callback.  This function will increment a global counter and by
        //   checking it we will verify that the function was executed.
        //
        // Testing:
        //   'bdefi_Vfunc0F1' methods:
        //   bdefi_Vfunc0F1<F, D1>(c_F&, c_D1&, ba *);
        //   void execute<F, D1>() const;
        //   ~bdefi_Vfunc0F1<F, D1>();
        // -------------------------------------------------------------------
        //   'bdefi_Vfunc0C1' methods:
        //   bdefi_Vfunc0C1<OBJ, C, D1>( OBJ *, c_C&, c_D1&, ba *);
        //   void execute<OBJ, C, D1>() const;
        //   ~bdefi_Vfunc0C1<OBJ, C, D1>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M1' methods:
        //   bdefi_Vfunc0M1<OBJ, M, D1>( OBJ *, c_M&, c_D1&, ba *);
        //   void execute<OBJ, M, D1>() const;
        //   ~bdefi_Vfunc0M1<OBJ, M, D1>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0Null' methods:
        //   bdefi_Vfunc0Null(ba *);
        //   void execute() const;
        //   ~bdefi_Vfunc0Null();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0F0' methods:
        //   bdefi_Vfunc0F0<F>(c_F&, ba *);
        //   void execute<F>() const;
        //   ~bdefi_Vfunc0F0<F>();
        // -------------------------------------------------------------------
        //   'bdefi_Vfunc0C0' methods:
        //   bdefi_Vfunc0C0<OBJ, C>( OBJ *, c_C&, ba *);
        //   void execute<OBJ, C>() const;
        //   ~bdefi_Vfunc0C0<OBJ, C>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc0M0' methods:
        //   bdefi_Vfunc0M0<OBJ, M>( OBJ *, c_M&, ba *);
        //   void execute<OBJ, M>() const;
        //   ~bdefi_Vfunc0M0<OBJ, M>();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 1 Parameters" << endl
                 << "================================================="
                 << endl;

        if (verbose)
            cout << "\nTesting constructors and argument values." << endl;
        {
            typedef void (*CbFuncF1)(int *);
            typedef void (CbClass::*CbFuncC1)(int *) const;
            typedef bdefr_Vfunc0 ObjBase;

            int a1 = ARG1;

            if (verbose) cout << "\t'bdefi_Vfunc0F1' object." << endl;
            {
                typedef bdefi_Vfunc0F1<CbFuncF1, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues1, &a1, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C1' object." << endl;
            {
                typedef bdefi_Vfunc0C1<CbClass, CbFuncC1, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkArgumentValues1, &a1,
                                          &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M1' object." << endl;
            {
                typedef bdefi_Vfunc0M1<CbClass, CbFuncC1, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkArgumentValues1,
                                          &a1, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }
        }

        if (verbose)
            cout << "\nTesting parameter types and destructors." << endl;
        {
            typedef void (*CbFuncF1)(T1);
            typedef void (CbClass::*CbFuncC1)(T1) const;
            typedef void (CbClass::*CbFuncM1)(T1);
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations;
            T1 a1;

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0F1' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0F1<CbFuncF1, T1> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                ObjBase *x = new(ta) Obj(checkParameterTypes1, a1, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG1 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0C1' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0C1<CbClass, CbFuncC1, T1> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                          &CbClass::checkParameterTypesC1,
                                          a1, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG1 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc0M1' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc0M1<CbClass, CbFuncM1, T1> Obj;
                dcEnabled = 0;
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                          &CbClass::checkParameterTypesM1,
                                          a1, &ta);
                x->execute();
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG1 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose)
                cout << "\nTesting bdefi_Vfunc0Null object." << endl;
            {
                destructorCheck = 0;
                numDeallocations = ta.numDeallocations();

                typedef bdefi_Vfunc0Null Obj;
                ObjBase *x = new(ta) Obj(&ta);
                x->execute();
                ObjBase::deleteObject(x);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }

        if (verbose)
            cout << endl
                 << "Testing Functor Representations With 0 Parameters" << endl
                 << "================================================="
                 << endl;

        {
            typedef void (*CbFuncF0)();
            typedef void (CbClass::*CbFuncC0)() const;
            typedef bdefr_Vfunc0 ObjBase;

            int numDeallocations = ta.numDeallocations();

            if (verbose) cout << "\t'bdefi_Vfunc0F0' object." << endl;
            {
                typedef bdefi_Vfunc0F0<CbFuncF0> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkVfunc0ExecutionF, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                a0_global = ARG0;
                x->execute();
                ASSERT(ARG0_PLUS1 == a0_global);
                a0_global = ARG0;
                numDeallocations = ta.numDeallocations();
                ObjBase::deleteObject(x);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose) cout << "\t'bdefi_Vfunc0C0' object." << endl;
            {
                typedef bdefi_Vfunc0C0<CbClass, CbFuncC0> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkVfunc0ExecutionC, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }
                x->execute();
                ASSERT(ARG0_PLUS1 == a0_global);
                a0_global = ARG0;
                numDeallocations = ta.numDeallocations();
                ObjBase::deleteObject(x);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }

            if (verbose) cout << "\t'bdefi_Vfunc0M0' object." << endl;
            {
                typedef bdefi_Vfunc0M0<CbClass, CbFuncC0> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkVfunc0ExecutionM, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute();
                ASSERT(ARG0_PLUS1 == a0_global);
                a0_global = ARG0;
                numDeallocations = ta.numDeallocations();
                ObjBase::deleteObject(x);
                ASSERT(numDeallocations + 1 == ta.numDeallocations());
                ASSERT(sizeof(Obj) == ta.lastAllocatedNumBytes());
            }
        }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
