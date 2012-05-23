// bsltst_dual_slist.t.cpp                                            -*-C++-*-
#undef  BSL_OVERRIDES_STD
#include <bsltst_dual_slist.h>

#include <bsl_slist.h>

#ifndef BSL_INCOMPLETE_INCLUDE
// get error: slist is not part of native library
//# include <slist>
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
using std::atoi;

//=============================================================================
//                             TEST PLAN
//
// All we do here is verify that the slist is not indicated as allocating
// memory via bslma_Allocator.  Note that there is a c'tor that takes a
// bslma::Allocator pointer, but the class does not actually allocate with
// it.  slist is not a standard part of the native libraries, it's part of
// the SGI STL extensions, which are available on some platforms but not
// on others, and not as part of the namespace std.
//-----------------------------------------------------------------------------
// [ 2] Storing Test (disabled)
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
        P(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
    }
    Cargo(const Cargo& in, bslma::Allocator* a = 0) {
        P(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
        std::memcpy(d_p, in.d_p, 4000);
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
      case 2: {
        // --------------------------------------------------------------------
        // SLIST STORING TEST
        //
        // Concerns:
        //   That memory allocated by the class comes from the bslma::Allocator
        //   passed at construction.
        //
        // Plan:
        //   Store objects of the locally defined 'Cargo' type, which
        //   allcocates a large amount of memory, in the container, and
    //   observe that all of the memory comes from the passed allocator.
    //   Note this test is disabled because slist does not yet have the
    //   correct behavior.
        // --------------------------------------------------------------------

        {
            bsl::slist<Cargo> sl(&ta);

            ASSERT(ta.numBytesInUse() < 100);
            P(ta.numBytesInUse());

            Cargo cx;
            P(usesBslmaAllocator(cx));

#if 0
            // slist should use bslma allocators properly, but it doesn't

            for (int i = 0; i < 10; ++i) {
                Cargo c;

                sl.push_front(c);
            }

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >= 40 * 1000);
#endif
        }

        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // SLIST BREATHING TEST
        //
        // Concerns:
        //   Since there is no standard std::slist, all we can really do here
    //   is verify that bsl::slist does not think it can allocate using
    //   bslma::Allocator, since there is a bug in it and it can't.
        //
        // Plan:
        //   Use the local created template functions 'sameType' and
        //   'usesBslmaAllocator' to verify.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        bsl::slist<int> sl(&ta);

        // note there is no native slist
        // slist should use bslma allocators properly, but it doesn't

//      ASSERT(true  == sameType(bsl::slist<int>(), std::slist<int>()));
//      ASSERT(true  == usesBslmaAllocator(sl));

        // make sure slist is not labeled as using bslma allocator, since
        // it doesn't properly
        ASSERT(false == usesBslmaAllocator(bsl::slist<int>()));

//      ASSERT(true  == usesBslmaAllocator(std::slist<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::slist<int>()));

//      ASSERT(true  == sameType(bsl::slist<int>::iterator(),
//                               std::slist<int>::iterator()));
//      ASSERT(false == sameType(bsl::slist<int>::iterator(),
//                        native_std::slist<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::slist<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(std::slist<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(
//                                  native_std::slist<int>::iterator()));
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
