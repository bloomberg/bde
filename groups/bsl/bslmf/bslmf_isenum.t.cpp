// bslmf_isenum.t.cpp                                                 -*-C++-*-

#include <bslmf_isenum.h>

#include <bsls_types.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD
//-----------------------------------------------------------------------------
// [ 1] THOROUGH TEST
// [ 2] USAGE EXAMPLE
//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

enum my_Enum { MY_FIRST, MY_SECOND };

struct my_Struct { operator my_Enum() const; };

struct my_IncompleteType;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// For example:
//..
    enum Enum { MY_ENUMERATOR = 5 };
    class Class { Class(Enum); };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
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

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        if (verbose) cout << "\nTesting usage example." << endl;

///Usage
///-----
// For example:
//..
//  enum Enum { MY_ENUMERATOR = 5 };
//  class Class { Class(Enum); };
//
    ASSERT(1 == bslmf_IsEnum<Enum>::VALUE);
    ASSERT(0 == bslmf_IsEnum<Class>::VALUE);
    ASSERT(0 == bslmf_IsEnum<int>::VALUE);
    ASSERT(0 == bslmf_IsEnum<int *>::VALUE);
//..
// Note that the 'bslmf_IsEnum' meta-function also evaluates to true (i.e., 1)
// when applied to references to enumeration types:
//..
     ASSERT(1 == bslmf_IsEnum<const Enum&>::VALUE);
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // THOROUGH TEST
        //
        // Concerns:
        //   Enum type is detected as such
        //   Reference-to-enum is detected as an enum
        //   All fundamental types are detected as non-enums
        //   Pointers are detected as non-enums
        //   Structs and classes are detected as non-enums
        //   Detection is unaffected by cv-qualification
        //
        // Plan:
        //   Verify that 'bslmf_IsEnum' produces the expected results
        //   (according to the above concerns) when instantiated with each of
        //   the following types:
        //
        //   - An enumerated type and a reference to an enumerated type
        //     using every combination of cv-qualification.
        //   - Each fundamental type
        //   - An array
        //   - A pointer to void
        //   - A pointer to char
        //   - A struct type, 'my_Struct'
        //   - Every cv-qualification combination for 'int', 'int&', 'char*'
        //     and 'my_Struct'
        //
        // Tactics:
        //   Area testing
        //   Ad-hoc data selection
        //
        // Testing:
        //   bslmf_IsEnum<TYPE>::VALUE
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPLETE TEST"
                            "\n=============\n");

        ASSERT(  bslmf_IsEnum<my_Enum>::VALUE);
        ASSERT(  bslmf_IsEnum<const my_Enum>::VALUE);
        ASSERT(  bslmf_IsEnum<volatile my_Enum>::VALUE);
        ASSERT(  bslmf_IsEnum<const volatile my_Enum>::VALUE);
        ASSERT(  bslmf_IsEnum<my_Enum&>::VALUE);
        ASSERT(  bslmf_IsEnum<const my_Enum&>::VALUE);
        ASSERT(  bslmf_IsEnum<volatile my_Enum&>::VALUE);
        ASSERT(  bslmf_IsEnum<const volatile my_Enum&>::VALUE);

        ASSERT(! bslmf_IsEnum<bool>::VALUE);
        ASSERT(! bslmf_IsEnum<char>::VALUE);
        ASSERT(! bslmf_IsEnum<signed char>::VALUE);
        ASSERT(! bslmf_IsEnum<unsigned char>::VALUE);
        ASSERT(! bslmf_IsEnum<wchar_t>::VALUE);
        ASSERT(! bslmf_IsEnum<short>::VALUE);
        ASSERT(! bslmf_IsEnum<unsigned short>::VALUE);
        ASSERT(! bslmf_IsEnum<int>::VALUE);
        ASSERT(! bslmf_IsEnum<unsigned int>::VALUE);
        ASSERT(! bslmf_IsEnum<long>::VALUE);
        ASSERT(! bslmf_IsEnum<unsigned long>::VALUE);
        ASSERT(! bslmf_IsEnum<long long>::VALUE);
        ASSERT(! bslmf_IsEnum<unsigned long long>::VALUE);
        ASSERT(! bslmf_IsEnum<bsls_Types::Int64>::VALUE);
        ASSERT(! bslmf_IsEnum<bsls_Types::Uint64>::VALUE);
        ASSERT(! bslmf_IsEnum<float>::VALUE);
        ASSERT(! bslmf_IsEnum<double>::VALUE);
        ASSERT(! bslmf_IsEnum<long double>::VALUE);
        ASSERT(! bslmf_IsEnum<char*>::VALUE);
        ASSERT(! bslmf_IsEnum<void*>::VALUE);
        ASSERT(! bslmf_IsEnum<char[5]>::VALUE);
        ASSERT(! bslmf_IsEnum<my_Enum*>::VALUE);
        ASSERT(! bslmf_IsEnum<my_Struct>::VALUE);
        ASSERT(! bslmf_IsEnum<int&>::VALUE);

        ASSERT(! bslmf_IsEnum<const int>::VALUE);
        ASSERT(! bslmf_IsEnum<const int&>::VALUE);
        ASSERT(! bslmf_IsEnum<const char*>::VALUE);
        ASSERT(! bslmf_IsEnum<const char* const>::VALUE);
        ASSERT(! bslmf_IsEnum<const char* volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<const char* const volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<const my_Struct>::VALUE);

        ASSERT(! bslmf_IsEnum<volatile int>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile int&>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile char*>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile char* const>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile char* volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile char* const volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<volatile my_Struct>::VALUE);

        ASSERT(! bslmf_IsEnum<const volatile int>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile int&>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile char*>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile char* const>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile char* volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile char* const volatile>::VALUE);
        ASSERT(! bslmf_IsEnum<const volatile my_Struct>::VALUE);

        ASSERT(! bslmf_IsEnum<void>::VALUE);
        ASSERT(! bslmf_IsEnum<my_IncompleteType>::VALUE);
        ASSERT(! bslmf_IsEnum<my_IncompleteType&>::VALUE);

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
