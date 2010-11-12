// bsltst_dual_set.t.cpp                  -*-C++-*-
#undef  BSL_OVERRIDES_STD
#include <bsltst_dual_set.h>

#include <bsl_set.h>

#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#ifndef BSL_INCOMPLETE_INCLUDE
# include <set>
#endif

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
//
// All we do here is verify that bsl::set and bsl::multiset are distinct from
// std::set and std::multiset.  More detailed testing of bsl::set and
// bsl::multiset is in bsltst_bsl_set.t.cpp.
//-----------------------------------------------------------------------------
// [ 2] Multiset Breathing Test
// [ 1] Set Breathing Test
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
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMULTISET BREATHING TEST\n"
                               "=======================\n";

        bsl::multiset<int> ms(&ta);

        ASSERT(false == sameType(ms, std::multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::multiset<int>()));
        ASSERT(false == usesBslmaAllocator(std::multiset<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::multiset<int>()));

        ASSERT(false == sameType(bsl::multiset<int>::iterator(),
                                 std::multiset<int>::iterator()));
        ASSERT(false == sameType(bsl::multiset<int>::iterator(),
                          native_std::multiset<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::multiset<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::multiset<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                    native_std::multiset<int>::iterator()));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // SET BREATHING TEST
        //
        // Concerns:
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSET BREATHING TEST\n"
                               "==================\n";

        bsl::set<int> s(&ta);

        ASSERT(false == sameType(s, std::set<int>()));
        ASSERT(true  == sameType(native_std::set<int>(), std::set<int>()));

        ASSERT(true  == usesBslmaAllocator(bsl::set<int>()));
        ASSERT(false == usesBslmaAllocator(std::set<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::set<int>()));

        ASSERT(false == sameType(bsl::set<int>::iterator(),
                                 std::set<int>::iterator()));
        ASSERT(false == sameType(bsl::set<int>::iterator(),
                          native_std::set<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::set<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::set<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                    native_std::set<int>::iterator()));
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
