// bslmf_isarray.t.cpp                                                -*-C++-*-

#include <bslmf_isarray.h>

#include <bsls_platform.h>            // for testing only

#include <cstdlib>    // atoi()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The objects under test are two meta-functions, 'bsl::is_array' and
// 'bslmf::IsArray', that determine whether a template parameter type is an
// array type.  Thus, we need to ensure that the values returned by these
// meta-functions are correct for each possible category of types.  Since the
// two meta-functions are functionally equivalent, we will use the same set of
// types for both.
//
//-----------------------------------------------------------------------------
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

enum Enum {};

struct Struct {
    int a[4];
};

union Union {
    int  i;
    char a[sizeof(int)];
};

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
        //   Simple example illustrating use of 'bslmf::IsArray'.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// For example:
//..
     ASSERT(1 == bslmf::IsArray<int    [5]>::value);
     ASSERT(0 == bslmf::IsArray<int  *    >::value);
     ASSERT(0 == bslmf::IsArray<int (*)[5]>::value);
//..
// Note that the 'bslmf::IsArray' meta-function also evaluates to true (1) when
// applied to references to arrays:
//..
     ASSERT(1 == bslmf::IsArray<int (&)[5]>::value);
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::IsArray' with various types and verify
        //   that their 'value' member is initialized properly.
        //
        // Testing:
        //   bslmf::IsArray
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf::IsArray" << endl
                          << "==============" << endl;

        ASSERT(1 == bslmf::IsArray<char          [1]>::VALUE);
        ASSERT(1 == bslmf::IsArray<char const    [1]>::VALUE);
        ASSERT(1 == bslmf::IsArray<char       (&)[1]>::VALUE);
        ASSERT(0 == bslmf::IsArray<char             >::VALUE);
        ASSERT(0 == bslmf::IsArray<char const       >::VALUE);
        ASSERT(0 == bslmf::IsArray<char        *    >::VALUE);

        ASSERT(1 == bslmf::IsArray<void       *   [2]>::VALUE);
        ASSERT(1 == bslmf::IsArray<void const *   [2]>::VALUE);
        ASSERT(1 == bslmf::IsArray<void       *(&)[2]>::VALUE);
        ASSERT(0 == bslmf::IsArray<void       *      >::VALUE);
        ASSERT(0 == bslmf::IsArray<void              >::VALUE);

        ASSERT(1 == bslmf::IsArray<int                [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       [3]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile [3]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                   >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const             >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile          >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile    >::VALUE);

        ASSERT(1 == bslmf::IsArray<int                (&)[4]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          (&)[4]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       (&)[4]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile (&)[4]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                 &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const           &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile        &    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile  &    >::VALUE);

        ASSERT(1 == bslmf::IsArray<int                 * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const           * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile        * [5]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile  * [5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const          (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile       (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile (*)[5]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                 *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const           *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile        *    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile  *    >::VALUE);

        ASSERT(1 == bslmf::IsArray<int         [6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const   [6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int      (&)[6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int       * [6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int      (*)[6][6]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int *const    [6][6]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int *const (&)[6][6]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int *const (*)[6][6]>::VALUE);

        ASSERT(1 == bslmf::IsArray<void *[]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int                []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile []>::VALUE);

#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bslmf::IsArray<int                (&)[]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const          (&)[]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile       (&)[]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile (&)[]>::VALUE);
#endif

        ASSERT(1 == bslmf::IsArray<int                 * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const           * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int volatile        * []>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const volatile  * []>::VALUE);
        ASSERT(0 == bslmf::IsArray<int                (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const          (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int volatile       (*)[]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int const volatile (*)[]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int          [][7]>::VALUE);
        ASSERT(1 == bslmf::IsArray<int const    [][7]>::VALUE);
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bslmf::IsArray<int       (&)[][7]>::VALUE);
#endif
        ASSERT(1 == bslmf::IsArray<int        * [][7]>::VALUE);
        ASSERT(0 == bslmf::IsArray<int       (*)[][7]>::VALUE);

        ASSERT(1 == bslmf::IsArray<int *const    [][7]>::VALUE);
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bslmf::IsArray<int *const (&)[][7]>::VALUE);
#endif
        ASSERT(0 == bslmf::IsArray<int *const (*)[][7]>::VALUE);

        ASSERT(1 == bslmf::IsArray<Enum          [8]>::VALUE);
        ASSERT(1 == bslmf::IsArray<Enum       (&)[8]>::VALUE);
        ASSERT(1 == bslmf::IsArray<Enum const (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Enum             >::VALUE);

        ASSERT(1 == bslmf::IsArray<Struct    [8]>::VALUE);
        ASSERT(1 == bslmf::IsArray<Struct (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Struct       >::VALUE);

        ASSERT(1 == bslmf::IsArray<Union    [8]>::VALUE);
        ASSERT(1 == bslmf::IsArray<Union (&)[8]>::VALUE);
        ASSERT(0 == bslmf::IsArray<Union       >::VALUE);

        ASSERT(0 == bslmf::IsArray<int  Struct::*    >::VALUE);
        ASSERT(0 == bslmf::IsArray<int (Struct::*)[9]>::VALUE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bsl::is_array' with various types and verify
        //   that their 'value' member is initialized properly.
        //
        // Testing:
        //   bsl::is_array
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bsl::is_array" << endl
                          << "=============" << endl;

        ASSERT(1 == bsl::is_array<char          [1]>::value);
        ASSERT(1 == bsl::is_array<char const    [1]>::value);
        ASSERT(1 == bsl::is_array<char       (&)[1]>::value);
        ASSERT(0 == bsl::is_array<char             >::value);
        ASSERT(0 == bsl::is_array<char const       >::value);
        ASSERT(0 == bsl::is_array<char        *    >::value);

        ASSERT(1 == bsl::is_array<void       *   [2]>::value);
        ASSERT(1 == bsl::is_array<void const *   [2]>::value);
        ASSERT(1 == bsl::is_array<void       *(&)[2]>::value);
        ASSERT(0 == bsl::is_array<void       *      >::value);
        ASSERT(0 == bsl::is_array<void              >::value);

        ASSERT(1 == bsl::is_array<int                [3]>::value);
        ASSERT(1 == bsl::is_array<int const          [3]>::value);
        ASSERT(1 == bsl::is_array<int volatile       [3]>::value);
        ASSERT(1 == bsl::is_array<int const volatile [3]>::value);
        ASSERT(0 == bsl::is_array<int                   >::value);
        ASSERT(0 == bsl::is_array<int const             >::value);
        ASSERT(0 == bsl::is_array<int volatile          >::value);
        ASSERT(0 == bsl::is_array<int const volatile    >::value);

        ASSERT(1 == bsl::is_array<int                (&)[4]>::value);
        ASSERT(1 == bsl::is_array<int const          (&)[4]>::value);
        ASSERT(1 == bsl::is_array<int volatile       (&)[4]>::value);
        ASSERT(1 == bsl::is_array<int const volatile (&)[4]>::value);
        ASSERT(0 == bsl::is_array<int                 &    >::value);
        ASSERT(0 == bsl::is_array<int const           &    >::value);
        ASSERT(0 == bsl::is_array<int volatile        &    >::value);
        ASSERT(0 == bsl::is_array<int const volatile  &    >::value);

        ASSERT(1 == bsl::is_array<int                 * [5]>::value);
        ASSERT(1 == bsl::is_array<int const           * [5]>::value);
        ASSERT(1 == bsl::is_array<int volatile        * [5]>::value);
        ASSERT(1 == bsl::is_array<int const volatile  * [5]>::value);
        ASSERT(0 == bsl::is_array<int                (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int const          (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (*)[5]>::value);
        ASSERT(0 == bsl::is_array<int                 *    >::value);
        ASSERT(0 == bsl::is_array<int const           *    >::value);
        ASSERT(0 == bsl::is_array<int volatile        *    >::value);
        ASSERT(0 == bsl::is_array<int const volatile  *    >::value);

        ASSERT(1 == bsl::is_array<int         [6][6]>::value);
        ASSERT(1 == bsl::is_array<int const   [6][6]>::value);
        ASSERT(1 == bsl::is_array<int      (&)[6][6]>::value);
        ASSERT(1 == bsl::is_array<int       * [6][6]>::value);
        ASSERT(0 == bsl::is_array<int      (*)[6][6]>::value);

        ASSERT(1 == bsl::is_array<int *const    [6][6]>::value);
        ASSERT(1 == bsl::is_array<int *const (&)[6][6]>::value);
        ASSERT(0 == bsl::is_array<int *const (*)[6][6]>::value);

        ASSERT(1 == bsl::is_array<void *[]>::value);

        ASSERT(1 == bsl::is_array<int                []>::value);
        ASSERT(1 == bsl::is_array<int const          []>::value);
        ASSERT(1 == bsl::is_array<int volatile       []>::value);
        ASSERT(1 == bsl::is_array<int const volatile []>::value);

#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bsl::is_array<int                (&)[]>::value);
        ASSERT(1 == bsl::is_array<int const          (&)[]>::value);
        ASSERT(1 == bsl::is_array<int volatile       (&)[]>::value);
        ASSERT(1 == bsl::is_array<int const volatile (&)[]>::value);
#endif

        ASSERT(1 == bsl::is_array<int                 * []>::value);
        ASSERT(1 == bsl::is_array<int const           * []>::value);
        ASSERT(1 == bsl::is_array<int volatile        * []>::value);
        ASSERT(1 == bsl::is_array<int const volatile  * []>::value);
        ASSERT(0 == bsl::is_array<int                (*)[]>::value);
        ASSERT(0 == bsl::is_array<int const          (*)[]>::value);
        ASSERT(0 == bsl::is_array<int volatile       (*)[]>::value);
        ASSERT(0 == bsl::is_array<int const volatile (*)[]>::value);

        ASSERT(1 == bsl::is_array<int          [][7]>::value);
        ASSERT(1 == bsl::is_array<int const    [][7]>::value);
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bsl::is_array<int       (&)[][7]>::value);
#endif
        ASSERT(1 == bsl::is_array<int        * [][7]>::value);
        ASSERT(0 == bsl::is_array<int       (*)[][7]>::value);

        ASSERT(1 == bsl::is_array<int *const    [][7]>::value);
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT(1 == bsl::is_array<int *const (&)[][7]>::value);
#endif
        ASSERT(0 == bsl::is_array<int *const (*)[][7]>::value);

        ASSERT(1 == bsl::is_array<Enum          [8]>::value);
        ASSERT(1 == bsl::is_array<Enum       (&)[8]>::value);
        ASSERT(1 == bsl::is_array<Enum const (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Enum             >::value);

        ASSERT(1 == bsl::is_array<Struct    [8]>::value);
        ASSERT(1 == bsl::is_array<Struct (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Struct       >::value);

        ASSERT(1 == bsl::is_array<Union    [8]>::value);
        ASSERT(1 == bsl::is_array<Union (&)[8]>::value);
        ASSERT(0 == bsl::is_array<Union       >::value);

        ASSERT(0 == bsl::is_array<int  Struct::*    >::value);
        ASSERT(0 == bsl::is_array<int (Struct::*)[9]>::value);
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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
