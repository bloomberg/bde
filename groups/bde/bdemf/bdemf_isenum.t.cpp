// bdemf_isenum.t.cpp                  -*-C++-*-

#include <bdemf_isenum.h>

#include <bsls_platformutil.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

enum myEnum { MY_FIRST, MY_SECOND};

struct myStruct { operator myEnum() const; };

struct my_IncompleteType;

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
        //   Verify that 'bdemf_IsEnum' produces the expected results
        //   (according to the above concerns) when instantiated with each of
        //   the following types:
        //
        //   - An enumerated type and a reference to an enumerated type
        //     using every combination of cv-qualification.
        //   - Each fundamental type
        //   - An array
        //   - A pointer to void
        //   - A pointer to char
        //   - A struct type, 'myStruct'
        //   - Every cv-qualification combination for 'int', 'int&', 'char*'
        //     and 'myStruct'
        //
        // Tactics:
        //   Area testing
        //   Ad-hoc data selection
        //
        // Testing:
        //   bdemf_IsEnum<TYPE>::VALUE
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPLETE TEST"
                            "\n=============\n");

        ASSERT(  bdemf_IsEnum<myEnum>::VALUE);
        ASSERT(  bdemf_IsEnum<const myEnum>::VALUE);
        ASSERT(  bdemf_IsEnum<volatile myEnum>::VALUE);
        ASSERT(  bdemf_IsEnum<const volatile myEnum>::VALUE);
        ASSERT(! bdemf_IsEnum<myEnum&>::VALUE);
        ASSERT(! bdemf_IsEnum<const myEnum&>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile myEnum&>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile myEnum&>::VALUE);

        ASSERT(! bdemf_IsEnum<bool>::VALUE);
        ASSERT(! bdemf_IsEnum<char>::VALUE);
        ASSERT(! bdemf_IsEnum<signed char>::VALUE);
        ASSERT(! bdemf_IsEnum<unsigned char>::VALUE);
        ASSERT(! bdemf_IsEnum<wchar_t>::VALUE);
        ASSERT(! bdemf_IsEnum<short>::VALUE);
        ASSERT(! bdemf_IsEnum<unsigned short>::VALUE);
        ASSERT(! bdemf_IsEnum<int>::VALUE);
        ASSERT(! bdemf_IsEnum<unsigned int>::VALUE);
        ASSERT(! bdemf_IsEnum<long>::VALUE);
        ASSERT(! bdemf_IsEnum<unsigned long>::VALUE);
        ASSERT(! bdemf_IsEnum<long long>::VALUE);
        ASSERT(! bdemf_IsEnum<unsigned long long>::VALUE);
        ASSERT(! bdemf_IsEnum<bsls_PlatformUtil::Int64>::VALUE);
        ASSERT(! bdemf_IsEnum<bsls_PlatformUtil::Uint64>::VALUE);
        ASSERT(! bdemf_IsEnum<float>::VALUE);
        ASSERT(! bdemf_IsEnum<double>::VALUE);
        ASSERT(! bdemf_IsEnum<long double>::VALUE);
        ASSERT(! bdemf_IsEnum<char*>::VALUE);
        ASSERT(! bdemf_IsEnum<void*>::VALUE);
        ASSERT(! bdemf_IsEnum<char[5]>::VALUE);
        ASSERT(! bdemf_IsEnum<myEnum*>::VALUE);
        ASSERT(! bdemf_IsEnum<myStruct>::VALUE);
        ASSERT(! bdemf_IsEnum<int&>::VALUE);

        ASSERT(! bdemf_IsEnum<const int>::VALUE);
        ASSERT(! bdemf_IsEnum<const int&>::VALUE);
        ASSERT(! bdemf_IsEnum<const char*>::VALUE);
        ASSERT(! bdemf_IsEnum<const char* const>::VALUE);
        ASSERT(! bdemf_IsEnum<const char* volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<const char* const volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<const myStruct>::VALUE);

        ASSERT(! bdemf_IsEnum<volatile int>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile int&>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile char*>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile char* const>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile char* volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile char* const volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<volatile myStruct>::VALUE);

        ASSERT(! bdemf_IsEnum<const volatile int>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile int&>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile char*>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile char* const>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile char* volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile char* const volatile>::VALUE);
        ASSERT(! bdemf_IsEnum<const volatile myStruct>::VALUE);

        ASSERT(! bdemf_IsEnum<void>::VALUE);
        ASSERT(! bdemf_IsEnum<my_IncompleteType>::VALUE);
        ASSERT(! bdemf_IsEnum<my_IncompleteType&>::VALUE);

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
