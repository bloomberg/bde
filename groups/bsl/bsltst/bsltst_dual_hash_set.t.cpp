// bsltst_dual_hash_set.t.cpp                  -*-C++-*-
#undef BSL_OVERRIDES_STD

#include <bsl_hash_set.h>

#ifndef BSL_INCOMPLETE_INCLUDE
//# include <hash_set>
#endif

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
using std::atoi;

//=============================================================================
//                             TEST PLAN
// There is no std::hash_set, there is a native hash_set on some, but not all
// platforms, but it is not in a consistent namespace, and even if there were,
// it would not be our responsibility to test it.  All we test is that things
// are in the right namespace and use the bslma_Allocator.  More detailed
// testing of bsl::hash_set and bsl::hash_multiset is in
// bsltst_bsl_hash_set.t.cpp.
//-----------------------------------------------------------------------------
// [ 2] Hash_multiset Breathing Test
// [ 1] Hash_set Breathing Test
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
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

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
        // MULTISET BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASH_MULTISET BREATHING TEST\n"
                               "============================\n";

        // there is no native hash_multiset

        bsl::hash_multiset<int> hms(&ta);

//      ASSERT(true  == sameType(hms, std::hash_multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(hms));
//      ASSERT(true  == usesBslmaAllocator(std::hash_multiset<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::hash_multiset<int>()));

//      ASSERT(true  == sameType(bsl::hash_multiset<int>::iterator(),
//                               std::hash_multiset<int>::iterator()));
//      ASSERT(false == sameType(bsl::hash_multiset<int>::iterator(),
//                        native_std::hash_multiset<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(
                                         bsl::hash_multiset<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(
//                                       std::hash_multiset<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(
//                                native_std::hash_multiset<int>::iterator()));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // SET BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASH SET BREATHING TEST\n"
                               "=====-----=============\n";

        bsl::hash_set<int> hs(&ta);

        // there is no native hash_set

//      ASSERT(false == sameType(hs, std::hash_set<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::hash_set<int>()));
//      ASSERT(false == usesBslmaAllocator(std::hash_set<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::hash_set<int>()));

//      ASSERT(true  == sameType(bsl::hash_set<int>::iterator(),
//                               std::hash_set<int>::iterator()));
//      ASSERT(false == sameType(bsl::hash_set<int>::iterator(),
//                        native_std::hash_set<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::hash_set<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(std::hash_set<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(
//                                  native_std::hash_set<int>::iterator()));
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
