// bsttst_stack.t.cpp                                                 -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsttst_bsl_stack.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>
#include <bslalg_typetraitusesbslmaallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <stack>
#include <vector>

#include <iostream>
#include <stdexcept>
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

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Cargo {
    void            *d_p;
    bslma::Allocator *d_alloc;

    enum {
        BALLAST_SIZE = 4000
    };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg::TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    // CREATORS
    explicit
    Cargo(bslma::Allocator *a = 0)
    {
        QV_("Default:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(BALLAST_SIZE);
    }
    Cargo(const Cargo& original, bslma::Allocator* a = 0)
    {
        QV_("Copy:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(BALLAST_SIZE);
        std::memcpy(d_p, original.d_p, BALLAST_SIZE);
    }
    Cargo& operator=(const Cargo& rhs)
    {
        QV("Assign:");
        std::memcpy(d_p, rhs.d_p, BALLAST_SIZE);
        return *this;
    }
    ~Cargo()
    {
        d_alloc->deallocate(d_p);
    }

    // MANIPULATORS
    void fill(char c)
    {
        std::memset(d_p, c, BALLAST_SIZE);
    }

    // ACCESSORS
    char firstChar() const { return * (char *) d_p; }
};

bool operator<(const Cargo& lhs, const Cargo& rhs)
{
    return * (int *) lhs.d_p < * (int *) rhs.d_p;
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

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator tda(veryVeryVerbose);

    bslma::DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Primitive, bare bones test.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        {
            ASSERT((bslmf::IsSame<bsl::stack<int>, std::stack<int> >::VALUE));

            bslma::TestAllocator ta;

            ASSERT(0 == ta.numBlocksTotal());

            bsl::stack<int> s(&ta);

            ASSERT(s.empty());

            s.push(4);
            s.push(5);
            s.push(6);

            ASSERT(! s.empty());
            ASSERT(6 == s.top());

            s.pop();

            ASSERT(! s.empty());
            ASSERT(5 == s.top());

            s.pop();

            ASSERT(! s.empty());
            ASSERT(4 == s.top());

            s.pop();

            ASSERT(s.empty());

            ASSERT(ta.numBlocksTotal() > 0);
        }

        {
            ASSERT(0 == ta.numBytesInUse());

            BSLMF_ASSERT(bslma::UsesBslmaAllocator<Cargo>::value);
            BSLMF_ASSERT(bslma::UsesBslmaAllocator<std::stack<Cargo> >::value);

            std::stack<Cargo, std::vector<Cargo> > s(&ta);

            ASSERT(0 == ta.numBytesInUse());

            Cargo cargo;

            for (std::size_t ti = 1; ti <= 10; ++ti) {
                cargo.fill('a' - 1 + ti);
                s.push(cargo);
                ASSERT(ta.numBytesInUse() > ti * Cargo::BALLAST_SIZE);
            }

            for (char c = 'a' + 9; c >= 'a'; --c) {
                ASSERT(s.top().firstChar() == c);
                s.pop();
            }

            ASSERT(0 == s.size());
        }
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
