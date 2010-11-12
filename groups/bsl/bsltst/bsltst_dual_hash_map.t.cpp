// bsltst_dual_hash_map.t.cpp                  -*-C++-*-
#undef  BSL_OVERRIDES_STD
#include <bsltst_dual_hash_map.h>

#include <bsl_hash_map.h>
#include <bsl_utility.h>    // for pair: whether this should be needed is
                            // the subject of some controversy

#ifndef BSL_INCOMPLETE_INCLUDE
//# include <hash_map>    // native hash_map doesn't exist
#endif

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace BloombergLP;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::atoi;

//=============================================================================
//                             TEST PLAN
//
// There is no std::hash_map, there is a native hash_map on some, but not all
// platforms, but it is not in a consistent namespace, and even if there were,
// it would not be our responsibility to test it.  All we test is that things
// are in the right namespace, and that bsl::map and bsl::multimap allocate
// memory from bslma_Allocator.  More detailed testing of bsl::hash_map and
// bsl::hash_multimap is in bsltst_bsl_hash_map.t.cpp.
//-----------------------------------------------------------------------------
// [ 2] Hash_multimap Breathing Test
// [ 1] Hash_map Breathing Test
//-----------------------------------------------------------------------------

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

//--------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define Q_(X) cout << "<| " #X " |>" << ", " << flush;  // Quote identifier lit
#define PV(X) { if (verbose) { cout << #X " = " << (X) << endl; }}
#define QV(X) { if (verbose) { cout << "<| " #X " |>" << endl; }}
#define PV_(X) { if (verbose) { cout << #X " = " << (X) << ", " << flush; }}
#define QV_(X) { if (verbose) { cout << "<| " #X " |>" << ", " << flush; }}
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBALS, TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose         = 0;
int veryVerbose     = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// sameType -- returns 'true' if objects 'lhs' and 'rhs' are of exactly the
// same type and 'false' otherwise.  Note that C++ performs no implicit
// conversions on parameters to template functions.

template<typename LHSTYPE, typename RHSTYPE>
bool sameType(const LHSTYPE& lhs, const RHSTYPE rhs)
{
    (void) lhs;    (void) rhs;

    return false;
}

template<typename TYPE>
bool sameType(const TYPE& lhs, const TYPE& rhs)
{
    (void) lhs;    (void) rhs;

    return true;
}

// ----------------------------------------------------------------------------

template<typename TYPE>
bool usesBslmaAllocator(const TYPE& arg)
    // returns 'true' if 'TYPE' uses bslma_Allocator and 'false' otherwise.
{
    (void) arg;

    return bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;
}

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator ta;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // HASH_MULTIMAP BREATHING TEST
        //
        // Concerns:
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.  Unfortunately, this test can
        //   test little, since 'std::hash_multimap' doesn't exist.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        // native hash_multimap doesn't exist

        bsl::hash_multimap<int, int> b(&ta);
//      std::hash_multimap<int, int> s(&ta);
//      native_std::hash_multimap<int, int> n;

//      ASSERT(true  == sameType(b, s));
//      ASSERT(false == sameType(s, n));
//      ASSERT(false == sameType(b, n));

        ASSERT(true  == usesBslmaAllocator(b));
//      ASSERT(true  == usesBslmaAllocator(s));
//      ASSERT(false == usesBslmaAllocator(n));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HASH_MAP BREATHING TEST
        //
        // Concerns:
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.  Unfortunately, this test can
        //   test little, since 'std::hash_map' doesn't exist.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        // native hash_map doesn't exist

        bsl::hash_map<int, int> b(&ta);
//      std::hash_map<int, int> s(&ta);
//      native_std::hash_map<int, int> n;

//      ASSERT(true  == sameType(b, s));
//      ASSERT(false == sameType(s, n));
//      ASSERT(false == sameType(b, n));

        ASSERT(true  == usesBslmaAllocator(b));
//      ASSERT(true  == usesBslmaAllocator(s));
//      ASSERT(false == usesBslmaAllocator(n));
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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
