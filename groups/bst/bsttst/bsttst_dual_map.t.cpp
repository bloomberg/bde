// bsttst_dual_map.t.cpp                                              -*-C++-*-
#undef  BSL_OVERRIDES_STD
#include <bsttst_dual_map.h>

#include <bsl_map.h>
#include <bsl_utility.h>    // for make_pair: whether this should be needed is
                            // the subject of some controversy

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#ifndef BSL_INCOMPLETE_INCLUDE
# include <map>
#endif

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
// All we test here is that bsl::map and bsl::multimap are distinct from
// std::map and std::multimap, and verify that bsl::map & multimap use memory
// correctly.  More detailed testing of bsl::map and bsl::multimap is done
// in bsttst_bsl_map.t.cpp.
//-----------------------------------------------------------------------------
// [ 4] Multimap Storing Test
// [ 3] Multimap Breathing Test
// [ 2] Map Storing Test
// [ 1] Map Breathing Test
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
    // returns 'true' if 'TYPE' uses bslma::Allocator and 'false' otherwise.
{
    (void) arg;

    return bslalg::HasTrait<TYPE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Cargo {
    void            *d_p;
    bslma::Allocator *d_alloc;


    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg::TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(bslma::Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
    }
    Cargo(const Cargo& in, bslma::Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
        std::memcpy(d_p, in.d_p, 4000);
    }
    Cargo& operator=(const Cargo& in) {
        QV("Assign:");
        std::memcpy(d_p, in.d_p, 4000);
        return *this;
    }
    ~Cargo() {
        d_alloc->deallocate(d_p);
    }
};

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

    bslma::TestAllocator ta;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // MULTIMAP STORING TEST
        //
        // Concerns:
        //   That memory allocated by the class comes from the bslma::Allocator
        //   passed at construction.
        //
        // Plan:
        //   Store objects of the locally defined 'Cargo' type, which
        //   allcocates a large amount of memory, in the container, and
        //   observe that all of the memory comes from the passed allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMAP STORING TEST\n"
                               "================\n";

        {
            bsl::multimap<int, Cargo> m(&ta);

            PV((void *) &ta);

            ASSERT(ta.numBytesInUse() < 100);
            PV(ta.numBytesInUse());

            Cargo cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 4; ++i) {
                m.insert(bsl::make_pair(i / 2, cx));
            }
            LOOP_ASSERT(m.size(), 4 == m.size());

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >= 16 * 1000);
            PV(ta.numBytesInUse());
        }

        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MULTIMAP BREATHING TEST
        //
        // Concerns:
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMULTIMAP BREATHING TEST\n"
                               "=======================\n";

        bsl::multimap<int, int> b(&ta);
        std::multimap<int, int> s;
        native_std::multimap<int, int> n;

        ASSERT(false == sameType(b, s));
        ASSERT(true  == sameType(s, n));
        ASSERT(false == sameType(b, n));

        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(false == usesBslmaAllocator(s));
        ASSERT(false == usesBslmaAllocator(n));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MAP STORING TEST
        //
        // Concerns:
        //   That memory allocated by the class comes from the bslma::Allocator
        //   passed at construction.
        //
        // Plan:
        //   Store objects of the locally defined 'Cargo' type, which
        //   allcocates a large amount of memory, in the container, and
    //   observe that all of the memory comes from the passed allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMAP STORING TEST\n"
                               "================\n";

        {
            bsl::map<int, Cargo> m(&ta);

            PV((void *) &ta);

            ASSERT(ta.numBytesInUse() < 100);
            PV(ta.numBytesInUse());

            Cargo cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                m.insert(bsl::make_pair(i, cx));
            }

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >= 12 * 1000);
            PV(ta.numBytesInUse());
        }

        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MAP BREATHING TEST
        //
        // Concerns:
        //   That bsl and std incarnations of this template class are distinct
        //   types, and only the bsl version uses bslma_Allocator.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMAP BREATHING TEST\n"
                               "==================\n";

        bsl::map<int, int> b(&ta);
        std::map<int, int> s;
        native_std::map<int, int> n;

        ASSERT(false == sameType(b, s));
        ASSERT(true  == sameType(s, n));
        ASSERT(false == sameType(b, n));

        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(false == usesBslmaAllocator(s));
        ASSERT(false == usesBslmaAllocator(n));
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
