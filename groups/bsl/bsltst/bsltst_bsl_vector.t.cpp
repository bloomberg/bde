// bsltst_vector.t.cpp                                                -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_vector.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <set>
#include <vector>
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
// [ 4] Manipulators, Accessors, and Iterators Test
// [ 3] At Throws Exception Test
// [ 2] Memory Consumption and C'tor Test
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

    enum {
        BALLAST_SIZE = 4000
    };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg::TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(bslma::Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(BALLAST_SIZE);
    }
    Cargo(const Cargo& in, bslma::Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
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
      case 4: {
        // --------------------------------------------------------------------
        // MANIPULATORS, ACCESSORS, AND ITERATORS TEST
        //
        // Concerns:
        //   That vector manipulators, accessors, and iterators works as
        //   expected.
        //
        // Plan:
        //   Create a couple of vectors filled with values and manipulate
        //   them, verifying with accessors that the values are as expected.
        //   Traverse a vector with forward and backward iterators.
        // --------------------------------------------------------------------

        bsl::vector<int> v1(&ta);

        ASSERT(v1.empty());

        for (int i = 0; i < 20; ++i) {
            ASSERT((int) v1.size() == i);
            v1.push_back(i * i);
            ASSERT( ! v1.empty());
        }

        for (int i = 0; i < 20; ++i) {
            ASSERT(i * i == v1[i]);
        }

        bsl::vector<int> v(v1.begin(), v1.end());

        ASSERT(0 == v.front());
        ASSERT(19 * 19 == v.back());

        bsl::vector<int>::iterator it;

        int j;
        for (it = v.begin(), j = 0; v.end() != it; ++it, ++j) {
            ASSERT(*it == j * j);
        }
        ASSERT(20 == j);

        bsl::vector<int>::reverse_iterator rit;
        for (rit = v.rbegin(), j = 19; v.rend() != rit; ++rit, --j) {
            ASSERT(*rit == j * j);
        }
        ASSERT(-1 == j);

        for (int i = 0; i < 10; ++i) {
            std::swap(v1[i], v1.at(20-1-i));
        }

        v.swap(v1);
        for (it = v.begin(), j = 19; v.end() != it; ++it, --j) {
            ASSERT(*it == j * j);
        }
        ASSERT(-1 == j);

        for (rit = v.rbegin(), j = 0; v.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j * j);
        }
        ASSERT(20 == j);

        v.insert(v.begin() + 10, v1.begin(), v1.end());

        for (it = v.begin(), j = 19; v.end() != it; ++it, --j) {
            LOOP4_ASSERT(it - v.begin(), *it, j, j * j, *it == j * j);
            if (it == v.begin() + 9) {
                it += 20;
            }
        }
        ASSERT(-1 == j);

        for (it = v.begin() + 10, j = 0; v.end() - 10 != it; ++it, ++j) {
            LOOP4_ASSERT(it - v.begin(), *it, j, j*j, *it == j * j);
        }
        ASSERT(20 == j);

        for (rit = v.rbegin(), j = 0; v.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j * j);
            if (rit == v.rbegin() + 9) {
                rit += 20;
            }
        }
        ASSERT(20 == j);

        for (rit = v.rbegin() + 10, j = 19; v.rend() - 10 != rit; ++rit, --j) {
            ASSERT(*rit == j * j);
        }
        ASSERT(-1 == j);

        v.erase(v.begin() + 30, v.end());
        v.erase(v.begin(), v.begin() + 10);

        ASSERT(v == v1);
        ASSERT(v >= v1);
        ASSERT(v <= v1);
        ASSERT(! (v != v1));
        ASSERT(! (v < v1));
        ASSERT(! (v > v1));

        --v[10];

        ASSERT(v <  v1);
        ASSERT(v <= v1);
        ASSERT(v != v1);
        ASSERT(! (v >  v1));
        ASSERT(! (v >= v1));
        ASSERT(! (v == v1));
        ASSERT(v1 >  v);
        ASSERT(v1 >= v);
        ASSERT(! (v1 <  v));
        ASSERT(! (v1 <= v));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // AT THROWS EXCEPTION TEST
        //
        // Concerns:
        //   That the 'at' method correctly throws the right exception on out
        //   of bounds errors.
        //
        // Plan:
        //   Do out of bounds errors and catch the exceptions, do a normal
        //   access and verify no exception is thrown.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nAT THROWS EXCEPTION TEST\n"
                               "========================\n";

#ifdef BDE_BUILD_TARGET_EXC
        bsl::vector<int> v(20 /* items */, 5, &ta);

        bool caught = false;
        try {
            (void) v.at(20);
            ASSERT(0 && "didn't catch overflow");
        } catch (const std::out_of_range& e) {
            caught = true;;   // expected
        } catch (const std::exception& e) {
            ASSERT(0 && "unexpected exception");
            P(e.what());
        } catch (...) {
            ASSERT(0 && "really unexpected exception");
        }
        ASSERT(caught);

        caught = false;
        try {
            (void) v.at(10);    // legal access, shouldn't throw
        } catch (...) {
            caught = true;
            ASSERT(0);
        }
        ASSERT(!caught);

        caught = false;
        try {
            (void) v.at((bsl::vector<int>::size_type) -1);
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
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VECTOR MEMORY CONSUMPTION AND CONSTRUCTOR TEST
        //
        // Concerns:
        //   That c'tors of objects stored in the container are passed the
        //   memory allocator passed to the hash_map at construction.  Also
        //   test all c'tors of the container.
        //
        // Plan:
        //   The struct 'Cargo' is a type that dynamically allocates
        //   'Cargo::BULK_STORAGE' memory at construction, using the memory
        //   allocator passed to it.  Create a container and store 'Cargo'
        //   objects in it, and observe that the memory for them comes from
        //   the passed allocator and not the default allocator.  Test that
        //   all c'tors of the container compile.  Repeat the test where the
        //   the elements are ints (which do not allocate memory) instead of
        //   Cargo, to insure the c'tors correctly don't pass the allocator
        //   to the elements in that case.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMemory Consumption and C'tor Test\n"
                               "=================================\n";

        {
            bsl::vector<Cargo> v;

            v.push_back(Cargo());
            (void) v[0];
        }

        {
            int memUsed = ta.numBytesInUse();

            bsl::vector<Cargo> v(&ta);

            Cargo c;

            int defaultMemUsed = tda.numBytesInUse();

            v.reserve(10);
            for (int i = 0; i < 10; ++i) {
                v.push_back(c);
            }

            int calcMem = memUsed + 10 * Cargo::BALLAST_SIZE +
                                                      10 * (int) sizeof(Cargo);
            LOOP3_ASSERT(ta.numBytesInUse(), calcMem, memUsed,
                         ta.numBytesInUse() >= calcMem);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<Cargo> vDefault((size_t) 10);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            // this constructor is not specified in the library, but it exists
            // in our implementation
            bsl::vector<Cargo> v((size_t) 10, &ta);
            v.push_back(Cargo());

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * (int) sizeof(int));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }
        {
            bsl::vector<Cargo> vDefault((size_t) 10);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<Cargo> v((size_t) 10, &ta);
            v.push_back(Cargo());

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * Cargo::BALLAST_SIZE +
                                                     11 * (int) sizeof(Cargo));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<Cargo> vDefault((size_t) 10);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<Cargo> v((size_t) 10, &ta);
            v.push_back(Cargo());

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * Cargo::BALLAST_SIZE +
                                                     11 * (int) sizeof(Cargo));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            Cargo cx;

            bsl::vector<Cargo> vDefault((size_t) 10, cx);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<Cargo> v((size_t) 10, cx, &ta);
            v.push_back(cx);

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * Cargo::BALLAST_SIZE +
                                                     11 * (int) sizeof(Cargo));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::multiset<Cargo> ms;

            Cargo cx;

            for (int i = 0; i < 10; ++i) {
                ms.insert(cx);
            }

            bsl::vector<Cargo> vDefault(ms.begin(), ms.end());

            ASSERT(vDefault.size() == 10);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<Cargo> v(ms.begin(), ms.end(), &ta);
            v.push_back(cx);

            ASSERT(v.size() == 11);

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * Cargo::BALLAST_SIZE +
                                                     11 * (int) sizeof(Cargo));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }
        {
            bsl::vector<Cargo> v;

            v.push_back(Cargo());
            (void) v[0];
        }

        {
            int memUsed = ta.numBytesInUse();

            bsl::vector<Cargo> v(&ta);

            Cargo c;

            int defaultMemUsed = tda.numBytesInUse();

            v.reserve(10);
            for (int i = 0; i < 10; ++i) {
                v.push_back(c);
            }

            int calcMem = memUsed + 10 * Cargo::BALLAST_SIZE +
                                                      10 * (int) sizeof(Cargo);
            LOOP3_ASSERT(ta.numBytesInUse(), calcMem, memUsed,
                         ta.numBytesInUse() >= calcMem);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<Cargo> vDefault((size_t) 10);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<Cargo> v((size_t) 10, &ta);
            v.push_back(Cargo());

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * (int) sizeof(int));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::vector<int> vDefault((size_t) 10);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<int> v((size_t) 10, &ta);
            v.push_back(int());

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * (int) sizeof(int));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            int ix = 5;

            bsl::vector<int> vDefault((size_t) 10, ix);

            ASSERT(10 == vDefault.size());
            vDefault[0] = vDefault[1];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<int> v((size_t) 10, ix, &ta);
            v.push_back(ix);

            ASSERT(11 == v.size());
            v[0] = v[1];

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * (int) sizeof(int));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::multiset<int> ms;

            int ix = 5;

            for (int i = 0; i < 10; ++i) {
                ms.insert(ix);
            }

            bsl::vector<int> vDefault(ms.begin(), ms.end());

            ASSERT(vDefault.size() == 10);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::vector<int> v(ms.begin(), ms.end(), &ta);
            v.push_back(ix);

            ASSERT(v.size() == 11);

            ASSERT(ta.numBytesInUse() >= memUsed + 11 * (int) sizeof(int));
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
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

        ASSERT(true  == sameType(bsl::vector<int>(), std::vector<int>()));
        ASSERT(false == sameType(bsl::vector<int>(),
                                 native_std::vector<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::vector<int>()));
        ASSERT(true  == usesBslmaAllocator(std::vector<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::vector<int>()));

        bsl::vector<int>::iterator bvi;
        std::vector<int>::iterator svi;
        native_std::vector<int>::iterator nvi;

        ASSERT(true  == sameType(bvi, svi));

//      bvi is 'int *', nvi is a complicated type
//      ASSERT(true  == sameType(bvi, nvi));

        ASSERT(false == usesBslmaAllocator(bvi));
        ASSERT(false == usesBslmaAllocator(svi));
        ASSERT(false == usesBslmaAllocator(nvi));
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
