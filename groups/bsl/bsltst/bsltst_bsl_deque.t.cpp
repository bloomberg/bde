// bsltst_deque.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif

#include <bsltst_bsl_deque.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <deque>
#include <algorithm>

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
// [ 4] Manipulators, Accessors, & Iterators Test
// [ 3] At Exception Test
// [ 2] Memory Consumption & C'tors Test
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

struct Cargo {
    // This struct dynamically allocates memory, for verifying that the
    // memory allocator is properly passed to elements within a container.

    void            *d_p;
    bslma_Allocator *d_alloc;

    enum {
        BALLAST_SIZE = 4000
    };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg_TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(bslma_Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BALLAST_SIZE);
    }
    Cargo(const Cargo& in, bslma_Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BALLAST_SIZE);
        std::memcpy(d_p, in.d_p, BALLAST_SIZE);
    }
    Cargo& operator=(const Cargo& in) {
        QV("Assign:");
        std::memcpy(d_p, in.d_p, BALLAST_SIZE);
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

    bslma_TestAllocator ta;
    bslma_TestAllocator tda;

    bslma_DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // MANIPULATORS, ACCESSORS, AND ITERATORS TEST
        //
        // Concerns:
        //   Test the deque forward and reverse iterators.
        //
        // Plan:
        //   Construct a couple of deques, traverse with forward and reverse
        //   iterators.
        //
        // Note:
        //   This test was just copied from vector, since a deque can do
        //   anything a vector can do, with some push_front's added at the
        //   end.
        // --------------------------------------------------------------------

        bsl::deque<int> d1(&ta);

        ASSERT(d1.empty());

        for (int i = 0; i < 20; ++i) {
            ASSERT((int) d1.size() == i);
            d1.push_back(i);
            ASSERT( ! d1.empty());
        }

        bsl::deque<int> d(d1.begin(), d1.end());

        ASSERT(0 == d.front());
        ASSERT(19 == d.back());

        bsl::deque<int>::iterator it;

        int j;
        for (it = d.begin(), j = 0; d.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        bsl::deque<int>::reverse_iterator rit;
        for (rit = d.rbegin(), j = 19; d.rend() != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        for (int i = 0; i < 10; ++i) {
            std::swap(d1[i], d1[20-1-i]);
        }

        d.swap(d1);
        for (it = d.begin(), j = 19; d.end() != it; ++it, --j) {
            ASSERT(*it == j);
        }
        ASSERT(-1 == j);

        for (rit = d.rbegin(), j = 0; d.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j);
        }
        ASSERT(20 == j);

        d.insert(d.begin() + 10, d1.begin(), d1.end());

        for (it = d.begin(), j = 19; d.end() != it; ++it, --j) {
            LOOP3_ASSERT(it - d.begin(), *it, j, *it == j);
            if (it == d.begin() + 9) {
                it += 20;
            }
        }
        ASSERT(-1 == j);

        for (it = d.begin() + 10, j = 0; d.end() - 10 != it; ++it, ++j) {
            LOOP3_ASSERT(it - d.begin(), *it, j, *it == j);
        }
        ASSERT(20 == j);

        for (rit = d.rbegin(), j = 0; d.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j);
            if (rit == d.rbegin() + 9) {
                rit += 20;
            }
        }
        ASSERT(20 == j);

        for (rit = d.rbegin() + 10, j = 19; d.rend() - 10 != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        d.erase(d.begin() + 30, d.end());
        d.erase(d.begin(), d.begin() + 10);

        ASSERT(d == d1);
        ASSERT(d >= d1);
        ASSERT(d <= d1);
        ASSERT(! (d != d1));

        --d[10];

        ASSERT(d <  d1);
        ASSERT(d <= d1);
        ASSERT(d1 >  d);
        ASSERT(d1 >= d);
        ASSERT(d != d1);
        ASSERT(! (d >  d1));
        ASSERT(! (d >= d1));
        ASSERT(! (d1 <  d));
        ASSERT(! (d1 <= d));
        ASSERT(! (d1 == d));

        ++d[10];

        for (int i = -1; i >= -20; --i) {
            d.push_front(i);
        }

        for (int i = 0, j = -20, k = 19; i < 10; ++i, ++j, --k) {
            LOOP2_ASSERT(d.back(), k, d.back() == k);
            ASSERT(d.front() == j);
            d.pop_back();
            d.pop_front();
        }

        for (it = d.begin(), j = -10; d.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // AT THROWS EXCEPTION TEST
        //
        // Concerns:
        //   That 'at' throws the right exception when attempting to access an
        //   an out of bounds element.
        //
        // Plan:
        //   Attempt to access out of bounds elements in a deque in try-catch
        //   blocks and verify that the correct exception is thrown.
        // --------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
        bsl::deque<int> d(20 /* items */, 5, &ta);

        bool caught = false;
        try {
            (void) d.at(20);
            ASSERT(0 && "didn't catch overflow");
        } catch (const std::out_of_range& e) {
            caught = true;   // expected
        } catch (const std::exception& e) {
            ASSERT(0 && "unexpected exception");
            P(e.what());
        } catch (...) {
            ASSERT(0 && "really unexpected exception");
        }
        ASSERT(caught);

        caught = false;
        try {
            (void) d.at(10);
        } catch(...) {
            ASSERT(0 && "unexpected throw");
        }
        ASSERT(! caught);

        caught = false;
        try {
            (void) d.at((bsl::deque<int>::size_type) -1);
            ASSERT(0 && "didn't catch underflow");
        } catch (const std::out_of_range& e) {
            caught = true;   // expected
        } catch (const std::exception& e) {
            ASSERT(0 && "unexpected exception");
            P(e.what());
        } catch (...) {
            ASSERT(0 && "really unexpected exception");
        }
        ASSERT(caught);
#endif // BDE_BUILD_TARGET_EXC
      }
      case 2: {
        // --------------------------------------------------------------------
        // MEMORY CONSUMPTION TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that all constructors compile.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container.
        //   Verify this for all c'tors.
        // --------------------------------------------------------------------

        {
            bsl::deque<Cargo> d;

            Cargo cx;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            d.reserve(10);
            for (int i = 0; i < 10; ++i) {
                d.push_back(cx);
            }

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == memUsed);
            LOOP_ASSERT(tda.numBytesInUse(), tda.numBytesInUse() >=
                                    defaultMemUsed + 10 * Cargo::BALLAST_SIZE);
        }

        {
            bsl::deque<Cargo> d(&ta);

            Cargo cx;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            d.reserve(10);
            for (int i = 0; i < 10; ++i) {
                d.push_back(cx);
            }

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >=
                                           memUsed + 10 * Cargo::BALLAST_SIZE);
            LOOP_ASSERT(tda.numBytesInUse(),
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> d(10);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == memUsed);
            LOOP_ASSERT(tda.numBytesInUse(), tda.numBytesInUse() >=
                                    defaultMemUsed + 10 * Cargo::BALLAST_SIZE);
        }

        {
            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            // this c'tor is not part of the standard, but our implementation
            // supports it
            bsl::deque<Cargo> d(10, &ta);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >=
                                           memUsed + 10 * Cargo::BALLAST_SIZE);
            LOOP_ASSERT(tda.numBytesInUse(),
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            Cargo cx;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> d(10, cx);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == memUsed);
            LOOP_ASSERT(tda.numBytesInUse(), tda.numBytesInUse() >=
                                    defaultMemUsed + 10 * Cargo::BALLAST_SIZE);
        }

        {
            Cargo cx;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> d(10, cx, &ta);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >=
                                           memUsed + 10 * Cargo::BALLAST_SIZE);
            LOOP_ASSERT(tda.numBytesInUse(),
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            Cargo cs[10];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> d(cs, cs + 10);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == memUsed);
            LOOP_ASSERT(tda.numBytesInUse(), tda.numBytesInUse() >=
                                    defaultMemUsed + 10 * Cargo::BALLAST_SIZE);
        }

        {
            Cargo cs[10];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> d(cs, cs + 10, &ta);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >=
                                           memUsed + 10 * Cargo::BALLAST_SIZE);
            LOOP_ASSERT(tda.numBytesInUse(),
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            Cargo cs[10];

            bsl::deque<Cargo> dA(cs, cs + 10);
            const bsl::deque<Cargo>& DA = dA;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> dB(DA);
            dB.push_back(cs[0]);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() == memUsed);
            LOOP_ASSERT(tda.numBytesInUse(), tda.numBytesInUse() >=
                                    defaultMemUsed + 11 * Cargo::BALLAST_SIZE);
        }

        {
            Cargo cs[10];

            bsl::deque<Cargo> dA(cs, cs + 10);
            const bsl::deque<Cargo>& DA = dA;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::deque<Cargo> dB(DA, &ta);
            dB.push_back(cs[0]);

            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >=
                                           memUsed + 11 * Cargo::BALLAST_SIZE);
            LOOP_ASSERT(tda.numBytesInUse(),
                                        tda.numBytesInUse() == defaultMemUsed);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
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

        if (verbose) cout << "\nBREATHING TEST\n"
                               "==============\n";

        ASSERT(true  == sameType(bsl::deque<int>(), std::deque<int>()));
        ASSERT(false == sameType(std::deque<int>(), native_std::deque<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::deque<int>()));
        ASSERT(true  == usesBslmaAllocator(std::deque<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::deque<int>()));

        ASSERT(true  == sameType(bsl::deque<int>::iterator(),
                                 std::deque<int>::iterator()));
        ASSERT(false == sameType(bsl::deque<int>::iterator(),
                          native_std::deque<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(bsl::deque<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::deque<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                    native_std::deque<int>::iterator()));
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
