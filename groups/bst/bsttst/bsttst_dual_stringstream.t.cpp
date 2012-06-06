// bsttst_dual_stringstream.t.cpp                                     -*-C++-*-
#undef  BSL_OVERRIDES_STD
#include <bsttst_dual_stringstream.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bsl_sstream.h>
#include <bsl_iostream.h>

#ifndef BSL_INCOMPLETE_INCLUDE
# include <sstream>
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
// All we test here is that bsl stringstreams are distinct types from
// std stringstreams, and that only bsl stringstreams use the bslma_Allocator.
// More detailed testing of bsl stringstreams is in
// bsttst_bsl_stringstream.t.cpp.
//-----------------------------------------------------------------------------
// [ 1] Breathing Test
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
bool sameType(const LHSTYPE& lhs, const RHSTYPE& rhs)
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
    // returns 'true' if 'TYPE' uses bslma::Allocator and 'false' otherwise.
{
    (void) arg;

    return bslalg::HasTrait<TYPE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;
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

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That bsl::stringstream is a different type than std::stringstream,
        //   and that bsl::stringstream allocates while std::stringstream
        //   does not.
        //
        // Plan:
        //   Use the 'sameType' and 'usesBslmaAllocator' template functions
        //   to perform the tests.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        // technically, if one binds a (ref to a const) to a temporary, the
        // C++ std says the temporary is to be copied to another temporary
        // that is bound to the reference.  But the compiler is allowed to
        // optimize this copy away and just bind to the original temporary.
        // Pretty much all compilers do this optimization, but the g++
        // compiler, being paranoid, checks that you are able to do the copy.
        // stringstream contains things with private copy c'tors, so it cannot
        // be copied, causing errors.  So we pass non-temporaries to the
        // routines.

        bsl::stringstream b;
        std::stringstream s;
        native_std::stringstream n;

        bsl::istringstream ib;
        std::istringstream is;
        native_std::istringstream in;

        bsl::ostringstream ob;
        std::ostringstream os;
        native_std::ostringstream on;

        ASSERT(false == sameType(b, n));
        ASSERT(true  == sameType(s, n));
        ASSERT(false == sameType(b, s));
        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(false == usesBslmaAllocator(s));
        ASSERT(false == usesBslmaAllocator(n));

        ASSERT(false == sameType(ib, in));
        ASSERT(true  == sameType(is, in));
        ASSERT(false == sameType(ib, is));
        ASSERT(true  == usesBslmaAllocator(ib));
        ASSERT(false == usesBslmaAllocator(is));
        ASSERT(false == usesBslmaAllocator(in));

        ASSERT(false == sameType(ob, on));
        ASSERT(true  == sameType(os, on));
        ASSERT(false == sameType(ob, os));
        ASSERT(true  == usesBslmaAllocator(ob));
        ASSERT(false == usesBslmaAllocator(os));
        ASSERT(false == usesBslmaAllocator(on));
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
