// bslmf_ispointer.t.cpp                                              -*-C++-*-

#include <bslmf_ispointer.h>

#include <cstdlib>    // atoi()
#include <cstring>    // strcmp()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The objects under test are two meta-functions, 'bsl::is_pointer' and
// 'bslmf::IsPointer', that determines whether a template parameter type is a
// pointer type.  Thus, we need to ensure that the value returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are equivalent, we will use the same set of types for
// both.
//
// ----------------------------------------------------------------------------
// class methods:
// [ 2] BloombergLP::bslmf::IsPointer
// [ 1] bsl::is_pointer
//
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType
   // This user-defined type is intended to be used during testing as an
   // argument for the template parameter 'TYPE' of 'bsl::is_pointer'.
{
};

typedef int (TestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::is_pointer'.

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a pointer type.
//
// First, we create two 'typedef's -- a pointer type and a non-pointer type:
//..
        typedef int  MyType;
        typedef int *MyPtrType;
//..
// Now, we instantiate the 'bsl::is_pointer' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
        ASSERT(false == bsl::is_pointer<MyType>::value);
        ASSERT(true == bsl::is_pointer<MyPtrType>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::IsPointer' template
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsPointer'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsPointer::VALUE' is 0 when 'TYPE' is a primitve type (that may
        //:   be const-qualified or volatile-qualified).
        //
        //: 2 'IsPointer::VALUE' is 0 when 'TYPE' is a user-defined type (that
        //:   may be const-qualified or volatile-qualified).
        //:
        //: 3 'IsPointer::VALUE' is 0 when 'TYPE' is a pointer to a non-static
        //:   member (that may be const-qualified or volatile-qualified).
        //:
        //: 4 'IsPointer::VALUE' is 1 when 'TYPE' is a pointer type (that may
        //:   be const-qualified or volatile-qualified) to a type (that may be
        //:   const-qualified or volatile-qualified).
        //
        // Plan:
        //   Verify that 'bsl::IsPointer::VALUE' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::IsPointer::VALUE
        // --------------------------------------------------------------------

        // C-1
        ASSERT(0 == bslmf::IsPointer<int>::VALUE);
        ASSERT(0 == bslmf::IsPointer<int const>::VALUE);
        ASSERT(0 == bslmf::IsPointer<int volatile>::VALUE);
        ASSERT(0 == bslmf::IsPointer<int const volatile>::VALUE);

        // C-2
        ASSERT(0 == bslmf::IsPointer<TestType>::VALUE);
        ASSERT(0 == bslmf::IsPointer<TestType const>::VALUE);
        ASSERT(0 == bslmf::IsPointer<TestType volatile>::VALUE);
        ASSERT(0 == bslmf::IsPointer<TestType const volatile>::VALUE);

        // C-3
        ASSERT(0 == bslmf::IsPointer<MethodPtrTestType>::VALUE);
        ASSERT(0 == bslmf::IsPointer<MethodPtrTestType const>::VALUE);
        ASSERT(0 == bslmf::IsPointer<MethodPtrTestType volatile>::VALUE);
        ASSERT(0 == bslmf::IsPointer<MethodPtrTestType const volatile>::VALUE);

        // C-4
        ASSERT(1 == bslmf::IsPointer<int*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<int *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<int *volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<int *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const int*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const int *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const int *volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const int *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile int*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile int *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile int *volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile int *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const volatile int*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const volatile int *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const volatile int *volatile>::VALUE);
        ASSERT(1 ==
                  bslmf::IsPointer<const volatile int *const volatile>::VALUE);

        ASSERT(1 == bslmf::IsPointer<TestType*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<TestType *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<TestType *volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<TestType *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const TestType*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const TestType *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const TestType *volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const TestType *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile TestType*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile TestType *const>::VALUE);
        ASSERT(1 == bslmf::IsPointer<volatile TestType *volatile>::VALUE);
        ASSERT(1 ==
                   bslmf::IsPointer<volatile TestType *const volatile>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const volatile TestType*>::VALUE);
        ASSERT(1 == bslmf::IsPointer<const volatile TestType *const>::VALUE);
        ASSERT(1 ==
                   bslmf::IsPointer<const volatile TestType *volatile>::VALUE);
        ASSERT(1 ==
             bslmf::IsPointer<const volatile TestType *const volatile>::VALUE);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_pointer' template
        //   Ensure that the static data member 'value' of 'bsl::is_pointer'
        //   instantiations having various (template parameter) 'TYPES' has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_pointer::value' is 0 when 'TYPE' is a primitve type (that may
        //:   be const-qualified or volatile-qualified).
        //
        //: 2 'is_pointer::value' is 0 when 'TYPE' is a user-defined type (that
        //:   may be const-qualified or volatile-qualified).
        //:
        //: 3 'is_pointer::value' is 0 when 'TYPE' is a pointer to a non-static
        //:   member (that may be const-qualified or volatile-qualified).
        //:
        //: 4 'is_pointer::value' is 1 when 'TYPE' is a pointer type (that may
        //:   be const-qualified or volatile-qualified) to a type (that may be
        //:   const-qualified or volatile-qualified).
        //
        // Plan:
        //   Verify that 'bsl::is_pointer::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_pointer::value
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bsl::is_pointer" << endl
                          << "===============" << endl;

        // C-1
        ASSERT(0 == bsl::is_pointer<int>::value);
        ASSERT(0 == bsl::is_pointer<int const>::value);
        ASSERT(0 == bsl::is_pointer<int volatile>::value);
        ASSERT(0 == bsl::is_pointer<int const volatile>::value);

        // C-2
        ASSERT(0 == bsl::is_pointer<TestType>::value);
        ASSERT(0 == bsl::is_pointer<TestType const>::value);
        ASSERT(0 == bsl::is_pointer<TestType volatile>::value);
        ASSERT(0 == bsl::is_pointer<TestType const volatile>::value);

        // C-3
        ASSERT(0 == bsl::is_pointer<MethodPtrTestType>::value);
        ASSERT(0 == bsl::is_pointer<MethodPtrTestType const>::value);
        ASSERT(0 == bsl::is_pointer<MethodPtrTestType volatile>::value);
        ASSERT(0 == bsl::is_pointer<MethodPtrTestType const volatile>::value);

        // C-4
        ASSERT(1 == bsl::is_pointer<int*>::value);
        ASSERT(1 == bsl::is_pointer<int *const>::value);
        ASSERT(1 == bsl::is_pointer<int *volatile>::value);
        ASSERT(1 == bsl::is_pointer<int *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<const int*>::value);
        ASSERT(1 == bsl::is_pointer<const int *const>::value);
        ASSERT(1 == bsl::is_pointer<const int *volatile>::value);
        ASSERT(1 == bsl::is_pointer<const int *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<volatile int*>::value);
        ASSERT(1 == bsl::is_pointer<volatile int *const>::value);
        ASSERT(1 == bsl::is_pointer<volatile int *volatile>::value);
        ASSERT(1 == bsl::is_pointer<volatile int *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<const volatile int*>::value);
        ASSERT(1 == bsl::is_pointer<const volatile int *const>::value);
        ASSERT(1 == bsl::is_pointer<const volatile int *volatile>::value);
        ASSERT(1 == bsl::is_pointer<const volatile int *const volatile>::value);

        ASSERT(1 == bsl::is_pointer<TestType*>::value);
        ASSERT(1 == bsl::is_pointer<TestType *const>::value);
        ASSERT(1 == bsl::is_pointer<TestType *volatile>::value);
        ASSERT(1 == bsl::is_pointer<TestType *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<const TestType*>::value);
        ASSERT(1 == bsl::is_pointer<const TestType *const>::value);
        ASSERT(1 == bsl::is_pointer<const TestType *volatile>::value);
        ASSERT(1 == bsl::is_pointer<const TestType *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<volatile TestType*>::value);
        ASSERT(1 == bsl::is_pointer<volatile TestType *const>::value);
        ASSERT(1 == bsl::is_pointer<volatile TestType *volatile>::value);
        ASSERT(1 == bsl::is_pointer<volatile TestType *const volatile>::value);
        ASSERT(1 == bsl::is_pointer<const volatile TestType*>::value);
        ASSERT(1 == bsl::is_pointer<const volatile TestType *const>::value);
        ASSERT(1 == bsl::is_pointer<const volatile TestType *volatile>::value);
        ASSERT(1 ==
              bsl::is_pointer<const volatile TestType *const volatile>::value);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
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
