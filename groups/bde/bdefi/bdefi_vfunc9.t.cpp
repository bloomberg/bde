// bdefi_vfunc9.t.cpp              -*-C++-*-

#include <bdefi_vfunc9.h>

#include <bdema_default.h>      // for testing only
#include <bdema_testallocator.h>           // for testing only
#include <iostream>
#include <stdlib.h>     // atoi()
using namespace BloombergLP;
using namespace std;


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
// non-'const' member function for every 'bdefi_Vfunc9MN' class.  Also, verify
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
// 'bdefi_Vfunc9Null' methods:
// [ 9] bdefi_Vfunc9Null<A1-9>(ba *);
// [ 9] void execute<A1-9>(c_A1-9&) const;
// [ 9] ~bdefi_Vfunc9Null<A1-9>();
//-----------------------------------------------------------------------------
// 'bdefi_Vfunc9F9' methods:
// [ 9] bdefi_Vfunc9F9<F, A1-9>(c_F&, ba *);
// [ 9] void execute<F, A1-9>(c_A1-9&) const;
// [ 9] ~bdefi_Vfunc9F9<F, A1-9>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc9C9' methods:
// [ 9] bdefi_Vfunc9C9<OBJ, C, A1-9>( OBJ *, c_C&, ba *);
// [ 9] void execute<OBJ, C, A1-9>(c_A1-9&) const;
// [ 9] ~bdefi_Vfunc9C9<OBJ, C, A1-9>();
// ----------------------------------------------------------------------------
// 'bdefi_Vfunc9M9' methods:
// [ 9] bdefi_Vfunc9M9<OBJ, M, A1-9>( OBJ *, c_M&, ba *);
// [ 9] void execute<OBJ, M, A1-9>(c_A1-9&) const;
// [ 9] ~bdefi_Vfunc9M9<OBJ, M, A1-9>();
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
// The following constants provide unique data used to verify the position
// correctness of functor arguments.  The constants are used in conjunction
// with the 'checkArgumentValues' family of helper functions.

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
// parameters ranging from two to nine.
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
//       // that 'bdefi_vfunc9X4' classes pass when calling the callback
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

    void checkArgumentValues6(int *a1, int *a2, int *a3, int *a4, int *a5,
                              int *a6) const
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
                              int *a6, int *a7) const
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
                              int *a6, int *a7, int *a8) const
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
                              int *a6, int *a7, int *a8, int *a9) const
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

    void checkParameterTypesC2(T1, T2) const { }
    void checkParameterTypesC3(T1, T2, T3) const { }
    void checkParameterTypesC4(T1, T2, T3, T4) const { }
    void checkParameterTypesC5(T1, T2, T3, T4, T5) const { }
    void checkParameterTypesC6(T1, T2, T3, T4, T5, T6) const { }
    void checkParameterTypesC7(T1, T2, T3, T4, T5, T6, T7) const { }
    void checkParameterTypesC8(T1, T2, T3, T4, T5, T6, T7, T8) const { }
    void checkParameterTypesC9(T1, T2, T3, T4, T5, T6, T7, T8, T9) const { }

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

void checkArgumentValues5(int *a1, int *a2, int *a3, int *a4,
                          int *a5)
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

void checkParameterTypes2(T1, T2) { }
void checkParameterTypes3(T1, T2, T3) { }
void checkParameterTypes4(T1, T2, T3, T4) { }
void checkParameterTypes5(T1, T2, T3, T4, T5) { }
void checkParameterTypes6(T1, T2, T3, T4, T5, T6) { }
void checkParameterTypes7(T1, T2, T3, T4, T5, T6, T7) { }
void checkParameterTypes8(T1, T2, T3, T4, T5, T6, T7, T8) { }
void checkParameterTypes9(T1, T2, T3, T4, T5, T6, T7, T8, T9) { }

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

    bdema_TestAllocator ta(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
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
        //       functions by 'bdefi_Vfunc9M9' instances.
        //    5. Destructor of each class under test is properly executed.
        //
        // Plan:
        //   Create objects of every 'bdefi_Vfunc9X9' class.  Initialize
        //   'bdefi_Vfunc9F9' to have 'checkArgumentValues9' as the callback
        //   function.  Initialize 'bdefi_Vfunc9C9' and 'bdefi_Vfunc9M9' to
        //   have 'checkArgumentValues9' method of the 'CbClass' class as the
        //   callback function.  Call the 'execute' method for each
        //   'bdefi_Vfunc9X9' class.  The arguments in this call are
        //   initialized with values of 'ARG1', 'ARG2', 'ARG3', 'ARG4',
        //   'ARG5', 'ARG6', 'ARG7', 'ARG8', and 'ARG9' respectively.  When
        //   called, the 'checkArgumentValues9' will emit an error, if the
        //   value of the first argument is not the same as the value of
        //   'ARG1', value of the second argument is not equal to the value of
        //   'ARG2', etc.  Verify that the method was called by checking that
        //   the first argument passed in the 'execute' method was incremented
        //   to the value of 'ARG1_PLUS1'.
        //
        //   Create objects of every 'bdefi_Vfunc9X9' class.  Initialize
        //   'bdefi_Vfunc9F9' to have 'checkParameterTypes9' as the callback
        //   function.  Initialize 'bdefi_Vfunc9C9' to have
        //   'checkParameterTypesC9' method of the 'CbClass' class as the
        //   callback function.  Initialize 'bdefi_Vfunc9M9' to have
        //   'checkParameterTypesM9' method of the 'CbClass' class as the
        //   callback function.  The latter will test wether the
        //   'bdefi_Vfunc9M9' can have non-'const' functions as the callback
        //   function.
        //
        //   Having a function of 'checkParameterTypes' family as a callback
        //   function forces 'bdefi_Vfunc9X9' to be templatized so that each
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
        //   'bdefi_Vfunc9F9' methods:
        //   bdefi_Vfunc9F9<F, A1-9>(c_F&, ba *);
        //   void execute<F, A1-9>(c_A1-9&) const;
        //   ~bdefi_Vfunc9F9<F, A1-9>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc9C9' methods:
        //   bdefi_Vfunc9C9<OBJ, C, A1-9>( OBJ *, c_C&, ba *);
        //   void execute<OBJ, C, A1-9>(c_A1-9&) const;
        //   ~bdefi_Vfunc9C9<OBJ, C, A1-9>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc9M9' methods:
        //   bdefi_Vfunc9M9<OBJ, M, A1-9>( OBJ *, c_M&, ba *);
        //    void execute<OBJ, M, A1-9>(c_A1-9&) const;
        //   ~bdefi_Vfunc9M9<OBJ, M, A1-9>();
        // --------------------------------------------------------------------
        //   'bdefi_Vfunc9Null' methods:
        //   bdefi_Vfunc9Null<A1-9>(ba *);
        //   void execute<A1-7>(c_A1-9&) const;
        //   ~bdefi_Vfunc9Null<A1-9>();
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
            typedef bdefr_Vfunc9<int *, int *, int *, int *, int *,
                                 int *, int *, int *, int *> ObjBase;

            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;
            int a8 = ARG8;
            int a9 = ARG9;

            if (verbose) cout << "\t'bdefi_Vfunc9F9' object." << endl;
            {
                typedef bdefi_Vfunc9F9<CbFuncF9,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *, int *> Obj;

                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(checkArgumentValues9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc9C9' object." << endl;
            {
                typedef bdefi_Vfunc9C9<CbClass, CbFuncC9, int *, int *,
                                       int *, int *, int *, int *, int *,
                                       int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkArgumentValues9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
                ObjBase::deleteObject(x);
            }

            if (verbose) cout << "\t'bdefi_Vfunc9M9' object." << endl;
            {
                typedef bdefi_Vfunc9M9<CbClass, CbFuncC9,
                                       int *, int *, int *, int *,
                                       int *, int *, int *, int *, int *> Obj;
                CbClass cbObject;
                if (verbose) cout << "\t\tTesting constructor." << endl;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkArgumentValues9, &ta);
                if (veryVerbose) { cout << "\t\t\t"; P(x); }

                if (verbose) cout << "\t\tTesting argument values." << endl;
                x->execute(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
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
            typedef bdefr_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> ObjBase;

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
                cout << "\tWhen calling 'bdefi_Vfunc9F9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc9F9<CbFuncF9, T1, T2, T3, T4, T5, T6, T7,
                                       T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = DESTRUCTOR_FLAG9;
                numDeallocations = ta.numDeallocation();

                ObjBase *x = new(ta) Obj(checkParameterTypes9, &ta);
                x->execute(a1, a2, a3, a4, a5, a6, a7, a8, a9);
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocation());
                ASSERT(sizeof(Obj) == ta.lastAllocateNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc9C9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc9C9<CbClass, CbFuncC9, T1, T2,
                                       T3, T4, T5, T6, T7, T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = DESTRUCTOR_FLAG9;
                numDeallocations = ta.numDeallocation();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(cbObject,
                                         &CbClass::checkParameterTypesC9, &ta);
                x->execute(a1, a2, a3, a4, a5, a6, a7, a8, a9);
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocation());
                ASSERT(sizeof(Obj) == ta.lastAllocateNumBytes());
            }

            if (verbose)
                cout << "\tWhen calling 'bdefi_Vfunc9M9' 'execute' method."
                     << endl;
            {
                typedef bdefi_Vfunc9M9<CbClass, CbFuncM9, T1, T2, T3, T4,
                                       T5, T6, T7, T8, T9> Obj;
                dcEnabled = 0;
                destructorCheck = DESTRUCTOR_FLAG9;
                numDeallocations = ta.numDeallocation();

                CbClass cbObject;
                ObjBase *x = new(ta) Obj(&cbObject,
                                         &CbClass::checkParameterTypesM9, &ta);
                x->execute(a1, a2, a3, a4, a5, a6, a7, a8, a9);
                dcEnabled = 1;
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocation());
                ASSERT(sizeof(Obj) == ta.lastAllocateNumBytes());
            }

            if (verbose)
                cout << "\nTesting bdefi_Vfunc9Null object." << endl;
            {
                destructorCheck = DESTRUCTOR_FLAG9;
                numDeallocations = ta.numDeallocation();

                typedef
                    bdefi_Vfunc9Null<T1, T2, T3, T4, T5, T6, T7, T8, T9> Obj;
                ObjBase *x = new(ta) Obj(&ta);
                x->execute(a1, a2, a3, a4, a5, a6, a7, a8, a9);
                ObjBase::deleteObject(x);
                ASSERT(DESTRUCTOR_FLAG9 == destructorCheck);
                ASSERT(numDeallocations + 1 == ta.numDeallocation());
                ASSERT(sizeof(Obj) == ta.lastAllocateNumBytes());
            }
        }
      } break;
      case 1: {
        // Not Applicable
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
