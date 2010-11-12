// bsltst_stringnative.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_string.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <string>

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
//-----------------------------------------------------------------------------
// [ 3] Storage Test
// [ 2] Breathing Test
// [ 1] Namespace & Allocator Test
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
    bslma_TestAllocator tda;

    bslma_DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // STRING STORAGE TEST
        //
        // Concerns:
        //   That string storage uses the proper allocator.
        //
        // Plan:
        //   Create a string, put some stuff in it, observe the memory comes
        //   from the right allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSTRING STORAGE TEST\n"
                               "===================\n";

        bsl::string bs(&ta);

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        // add 50 bytes
        bs += "                                                  ";

        for (int i = 0; i < 4; ++i) {
            bs += bs;
        }

        ASSERT(ta.numBytesInUse() >= memUsed + 50 * 16);
        ASSERT(tda.numBytesInUse() == defaultMemUsed);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Verify compatibility between native strings and bsl strings.
        //
        // Plan:
        //   Create native and bsl strings and make them interact.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        typedef bsl::string Bs;
        typedef native_std::string Ns;

        // note there is implicit conversion from Bs to Ns, but explicit
        // from Ns to Bs.  '==', '!=', etc are template functions, hence
        // no implicit conversions of args.

        bsl::string bs(&ta);
        native_std::string ns = "woof";

//      bs = ns;        // illegal
        bs = (Bs) ns;

        ASSERT((Ns) bs == ns);
        ASSERT(!((Ns) bs != ns));
        ASSERT((Ns) bs <= ns);
        ASSERT(!((Ns) bs < ns));
        ASSERT((Ns) bs >= ns);
        ASSERT(!((Ns) bs > ns));

        bs = "meow";

        ASSERT((Ns) bs != ns);
        ASSERT(!((Ns) bs == ns));
        ASSERT((Ns) bs <= ns);
        ASSERT((Ns) bs < ns);
        ASSERT(!((Ns) bs >= ns));
        ASSERT(!((Ns) bs >  ns));

        ns = bs;    // implicit conversion

        ASSERT((Ns) bs == ns);
        ASSERT(!((Ns) bs != ns));
        ASSERT((Ns) bs <= ns);
        ASSERT(!((Ns) bs < ns));
        ASSERT((Ns) bs >= ns);
        ASSERT(!((Ns) bs > ns));

        bs = ", arf";
        ns += bs;       // implicit conversion

        ns = "huey";
        bs = ", louie";

//      ns = ns + ", dewey" + bs;       // illegal
        ns = ns + ", dewey" + (Ns) bs;  // apparently + is a template function

        ASSERT(ns == "huey, dewey, louie");

        ns = "huey";
        bs = ", louie";

        bs = (Bs) ns + ", dewey" + bs;

        ASSERT(bs == "huey, dewey, louie");
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // NAMESPACE TEST
        //
        // Concerns:
        //   Verify that the container and its iterator are the same type in
        //   bsl and std, and that the native_std containter is different
        //   from the std container.  Also verify that the bsl container
        //   uses the bslma allocator, and that the native_std container
        //   doesn't.  Verify that iterators don't use the bslma allocator.
        //
        // Plan:
        //   Use the templates 'sameType' and 'usesBslmaAllocator' (defined
        //   in this file) to do the testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nNAMESPACE TEST\n"
                               "==============\n";

        ASSERT(false == sameType(bsl::string(), native_std::string()));
        ASSERT(true  == sameType(bsl::string(),        std::string()));
        ASSERT(true  == usesBslmaAllocator(bsl::string()));
        ASSERT(true  == usesBslmaAllocator(std::string()));
        ASSERT(false == usesBslmaAllocator(native_std::string()));
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
