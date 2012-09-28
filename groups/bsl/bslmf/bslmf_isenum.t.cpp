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
    ASSERT(1 == bslmf::IsEnum<Enum>::value);
    ASSERT(0 == bslmf::IsEnum<Class>::value);
    ASSERT(0 == bslmf::IsEnum<int>::value);
    ASSERT(0 == bslmf::IsEnum<int *>::value);
//..
// Note that the 'bslmf::IsEnum' meta-function also evaluates to true (i.e., 1)
// when applied to references to enumeration types:
//..
     ASSERT(1 == bslmf::IsEnum<const Enum&>::value);
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
        //   Verify that 'bslmf::IsEnum' produces the expected results
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
        //   bslmf::IsEnum<TYPE>::value
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPLETE TEST"
                            "\n=============\n");

        ASSERT(  bslmf::IsEnum<my_Enum>::value);
        ASSERT(  bslmf::IsEnum<const my_Enum>::value);
        ASSERT(  bslmf::IsEnum<volatile my_Enum>::value);
        ASSERT(  bslmf::IsEnum<const volatile my_Enum>::value);
        ASSERT(  bslmf::IsEnum<my_Enum&>::value);
        ASSERT(  bslmf::IsEnum<const my_Enum&>::value);
        ASSERT(  bslmf::IsEnum<volatile my_Enum&>::value);
        ASSERT(  bslmf::IsEnum<const volatile my_Enum&>::value);

        ASSERT(! bslmf::IsEnum<bool>::value);
        ASSERT(! bslmf::IsEnum<char>::value);
        ASSERT(! bslmf::IsEnum<signed char>::value);
        ASSERT(! bslmf::IsEnum<unsigned char>::value);
        ASSERT(! bslmf::IsEnum<wchar_t>::value);
        ASSERT(! bslmf::IsEnum<short>::value);
        ASSERT(! bslmf::IsEnum<unsigned short>::value);
        ASSERT(! bslmf::IsEnum<int>::value);
        ASSERT(! bslmf::IsEnum<unsigned int>::value);
        ASSERT(! bslmf::IsEnum<long>::value);
        ASSERT(! bslmf::IsEnum<unsigned long>::value);
        ASSERT(! bslmf::IsEnum<long long>::value);
        ASSERT(! bslmf::IsEnum<unsigned long long>::value);
        ASSERT(! bslmf::IsEnum<bsls::Types::Int64>::value);
        ASSERT(! bslmf::IsEnum<bsls::Types::Uint64>::value);
        ASSERT(! bslmf::IsEnum<float>::value);
        ASSERT(! bslmf::IsEnum<double>::value);
        ASSERT(! bslmf::IsEnum<long double>::value);
        ASSERT(! bslmf::IsEnum<char*>::value);
        ASSERT(! bslmf::IsEnum<void*>::value);
        ASSERT(! bslmf::IsEnum<char[5]>::value);
        ASSERT(! bslmf::IsEnum<my_Enum*>::value);
        ASSERT(! bslmf::IsEnum<my_Struct>::value);
        ASSERT(! bslmf::IsEnum<int&>::value);

        ASSERT(! bslmf::IsEnum<const int>::value);
        ASSERT(! bslmf::IsEnum<const int&>::value);
        ASSERT(! bslmf::IsEnum<const char*>::value);
        ASSERT(! bslmf::IsEnum<const char* const>::value);
        ASSERT(! bslmf::IsEnum<const char* volatile>::value);
        ASSERT(! bslmf::IsEnum<const char* const volatile>::value);
        ASSERT(! bslmf::IsEnum<const my_Struct>::value);

        ASSERT(! bslmf::IsEnum<volatile int>::value);
        ASSERT(! bslmf::IsEnum<volatile int&>::value);
        ASSERT(! bslmf::IsEnum<volatile char*>::value);
        ASSERT(! bslmf::IsEnum<volatile char* const>::value);
        ASSERT(! bslmf::IsEnum<volatile char* volatile>::value);
        ASSERT(! bslmf::IsEnum<volatile char* const volatile>::value);
        ASSERT(! bslmf::IsEnum<volatile my_Struct>::value);

        ASSERT(! bslmf::IsEnum<const volatile int>::value);
        ASSERT(! bslmf::IsEnum<const volatile int&>::value);
        ASSERT(! bslmf::IsEnum<const volatile char*>::value);
        ASSERT(! bslmf::IsEnum<const volatile char* const>::value);
        ASSERT(! bslmf::IsEnum<const volatile char* volatile>::value);
        ASSERT(! bslmf::IsEnum<const volatile char* const volatile>::value);
        ASSERT(! bslmf::IsEnum<const volatile my_Struct>::value);

        ASSERT(! bslmf::IsEnum<void>::value);
        ASSERT(! bslmf::IsEnum<my_IncompleteType>::value);
        ASSERT(! bslmf::IsEnum<my_IncompleteType&>::value);

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
