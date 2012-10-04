// bslmf_removereference.t.cpp                                        -*-C++-*-

#include <bslmf_removereference.h>

#include <bslmf_issame.h>          // for testing only

#include <bsls_bsltestutil.h>

#include <cstdlib>    // atoi()
#include <cstdio>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function, 'bsl::remove_reference', which
// transform a template parameter 'TYPE' to its rvalue reference type.  Thus,
// we need to ensure that the value returned by this meta-functions is correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::remove_reference::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

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

enum   Enum   {};
struct Struct {};
union  Union  {};
class  Class  {};

typedef int INT;

typedef void      F ();
typedef void ( & RF)();
typedef void (*  PF)();
typedef void (*&RPF)();

typedef void    Fi  (int);
typedef void (&RFi) (int);
typedef void    FRi (int&);
typedef void (&RFRi)(int&);

typedef char    A [5];
typedef char (&RA)[5];

#define ASSERT_SAME(X, Y)                                                     \
    ASSERT((bsl::is_same<bsl::remove_reference<X>::type, Y>::value))

#define ASSERT_REMOVE_REF(X, Y)                                               \
    ASSERT_SAME(X, Y)                                                         \
    ASSERT_SAME(const X, const Y)                                             \
    ASSERT_SAME(volatile X, volatile Y)                                       \
    ASSERT_SAME(const volatile X, const volatile Y)

#define ASSERT_SAME2(X, Y)                                                    \
    ASSERT((bsl::is_same<bslmf::RemoveReference<X>::Type, Y>::value))

#define ASSERT_REMOVE_REF2(X, Y)                                              \
    ASSERT_SAME2(X, Y)                                                        \
    ASSERT_SAME2(const X, const Y)                                            \
    ASSERT_SAME2(volatile X, volatile Y)                                      \
    ASSERT_SAME2(const volatile X, const volatile Y)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;          // eliminate unused variable warning
    (void) veryVerbose;      // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Pointer Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to remove reference-ness on a couple of types.
//
// Now, we instantiate the 'bsl::remove_reference' template for each of these
// types, and use 'bsl::is_same' meta-function to assert the 'type' static data
// member of each instantiation:
//..
    ASSERT(true  ==
               (bsl::is_same<bsl::remove_reference<int&>::type, int >::value));
    ASSERT(false ==
               (bsl::is_same<bsl::remove_reference<int&>::type, int&>::value));
    ASSERT(true  ==
               (bsl::is_same<bsl::remove_reference<int >::type, int >::value));
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // 'bslmf::RemoveReference::Type'
        //   Ensure that the static data member 'Type' of
        //   'bslmf::RemoveReference' instantiations having various (template
        //   parameter) 'TYPE' has the correct value.
        //
        // Concerns:
        //: 1 'RemoveReference::Type' correctly removes reference-ness from
        //:   'TYPE' if 'TYPE' is a reference type.
        //:
        //: 2 'RemoveReference::Type' does not transform 'TYPE' when 'TYPE' is
        //:   not a reference type.
        //
        // Plan:
        //   Instantiate 'bslmf::RemoveReference' with various types and
        //   verify that the 'Type' member is initialized properly.
        //
        // Testing:
        //   bslmf::RemoveReference::Type
        // --------------------------------------------------------------------

        if (verbose) printf("bslmf::RemoveReference\n"
                            "======================\n");

        ASSERT_REMOVE_REF2(int,  int);
        ASSERT_REMOVE_REF2(int&, int);

        ASSERT_REMOVE_REF2(void*,  void*);
        ASSERT_REMOVE_REF2(void*&, void*);

        ASSERT_REMOVE_REF2(Enum,  Enum);
        ASSERT_REMOVE_REF2(Enum&, Enum);

        ASSERT_REMOVE_REF2(Struct,  Struct);
        ASSERT_REMOVE_REF2(Struct&, Struct);

        ASSERT_REMOVE_REF2(Union,  Union);
        ASSERT_REMOVE_REF2(Union&, Union);

        ASSERT_REMOVE_REF2(Class,  Class);
        ASSERT_REMOVE_REF2(Class&, Class);

        ASSERT_REMOVE_REF2(int Class::*,  int Class::*);
        ASSERT_REMOVE_REF2(int Class::*&, int Class::*);

        ASSERT_SAME2(  F,  F);
        ASSERT_SAME2( RF,  F);
        ASSERT_SAME2( PF, PF);
        ASSERT_SAME2(RPF, PF);

        ASSERT_SAME2(  Fi,  Fi);
        ASSERT_SAME2( RFi,  Fi);
        ASSERT_SAME2( FRi, FRi);
        ASSERT_SAME2(RFRi, FRi);

        ASSERT_SAME2(A,  A);
        ASSERT_SAME2(RA, A);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::remove_reference::type'
        //   Ensure that the static data member 'type' of
        //   'bsl::remove_reference' instantiations having various (template
        //   parameter) 'TYPE' has the correct value.
        //
        // Concerns:
        //: 1 'remove_reference::type' correctly removes reference-ness from
        //:   'TYPE' if 'TYPE' is a reference type.
        //:
        //: 2 'remove_reference::type' does not transform 'TYPE' when 'TYPE' is
        //:   not a reference type.
        //
        // Plan:
        //   Instantiate 'bsl::remove_reference' with various types and
        //   verify that the 'type' member is initialized properly.
        //
        // Testing:
        //   bsl::remove_reference::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::remove_reference\n"
                            "=====================\n");

        ASSERT_REMOVE_REF(int,  int);
        ASSERT_REMOVE_REF(int&, int);

        ASSERT_REMOVE_REF(void*,  void*);
        ASSERT_REMOVE_REF(void*&, void*);

        ASSERT_REMOVE_REF(Enum,  Enum);
        ASSERT_REMOVE_REF(Enum&, Enum);

        ASSERT_REMOVE_REF(Struct,  Struct);
        ASSERT_REMOVE_REF(Struct&, Struct);

        ASSERT_REMOVE_REF(Union,  Union);
        ASSERT_REMOVE_REF(Union&, Union);

        ASSERT_REMOVE_REF(Class,  Class);
        ASSERT_REMOVE_REF(Class&, Class);

        ASSERT_REMOVE_REF(int Class::*,  int Class::*);
        ASSERT_REMOVE_REF(int Class::*&, int Class::*);

        ASSERT_SAME(  F,  F);
        ASSERT_SAME( RF,  F);
        ASSERT_SAME( PF, PF);
        ASSERT_SAME(RPF, PF);

        ASSERT_SAME(  Fi,  Fi);
        ASSERT_SAME( RFi,  Fi);
        ASSERT_SAME( FRi, FRi);
        ASSERT_SAME(RFRi, FRi);

        ASSERT_SAME(A,  A);
        ASSERT_SAME(RA, A);
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
