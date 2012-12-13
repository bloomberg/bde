// bslmf_removepointer.t.cpp                                          -*-C++-*-
#include <bslmf_removepointer.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <cstdio>
#include <cstdlib>

using namespace bsl;
using namespace BloombergLP;

using std::printf;
using std::fprintf;
using std::atoi;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::remove_pointer',
// that transforms a pointer type to the type pointed to by the pointer type.
// We need to ensure that the values returned by the meta-function are correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPES
// [ 1] bsl::remove_pointer::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct TestType {
   // This user-defined type is intended to be used during testing as an
   // argument for a template parameter.
};

void funcWithDefaultArg(int arg = 0);

template <typename TYPE>
void testFuncPtrType(TYPE)
    // Removing a pointer from some function pointer types can be problematic
    // for some compilers (e.g. AIX xlC).
{
    // First remove the pointer from the function pointer type and make sure it
    // compiles.
    typedef typename bsl::remove_pointer<TYPE>::type FUNC_TYPE;

    // Now add the pointer back and verify that we end up with the same type,
    // unless on a compiler that where removing the pointer didn't produce the
    // expected result.
#if !defined(BSLS_PLATFORM_CMP_AIX)
    ASSERT((bsl::is_same<TYPE, FUNC_TYPE *>::value));
#endif
}

void funcWithDefaultArg(int arg)
{
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void)veryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
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

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Get the Type Pointed to by a Pointer Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to get the type pointed to by a pointer type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType') and the type
// pointed to by the pointer type ('MyType'):
//..
    typedef int  MyType;
    typedef int *MyPtrType;
//..
// Now, we get the type pointed to by 'MyPtrType' using 'bsl::remove_pointer'
// and verify that the resulting type is the same as 'MyType':
//..
    ASSERT((bsl::is_same<bsl::remove_pointer<MyPtrType>::type,
                         MyType>::value));
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_pointer::type'
        //   Ensure that the 'typedef' 'type' of 'bsl::remove_pointer' has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        //: 1 'bsl::remove_pointer' transforms a (possibly cv-qualified)
        //:   pointer type to the type pointed to by that pointer type.
        //:
        //: 2 'bsl::remove_pointer' returns the same type as the argument when
        //:   it is not a pointer type.
        //
        // Plan:
        //   Verify that 'bsl::remove_pointer::type' has the correct type for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::remove_pointer::type
        // --------------------------------------------------------------------

        if (verbose) printf("\n'bsl::remove_pointer::type'\n"
                            "\n===========================\n");

        // C-1
        ASSERT((is_same<remove_pointer<int *>::type, int>::value));
        ASSERT((is_same<remove_pointer<void *>::type, void>::value));
        ASSERT((is_same<remove_pointer<TestType *>::type, TestType>::value));

        ASSERT((is_same<remove_pointer<int * const>::type, int>::value));
        ASSERT((is_same<remove_pointer<int * volatile>::type, int>::value));
        ASSERT((is_same<remove_pointer<int * const volatile>::type,
                                                                 int>::value));
        ASSERT((is_same<remove_pointer<int const *>::type, int const>::value));

        // C-2
        ASSERT((is_same<remove_pointer<int>::type, int>::value));
        ASSERT((is_same<remove_pointer<int *&>::type, int *&>::value));

        printf("*: %d\n",
               is_same<bslmf::RemovePointer_Imp<int * const volatile>::Type,
                       int * const volatile>::value);

        printf("*: %d\n",
               is_same<bslmf::RemovePointer_Imp<int * const volatile>::Type,
                       int>::value);

        // Test removing a pointer from some function pointer types.
        testFuncPtrType(&funcWithDefaultArg);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
