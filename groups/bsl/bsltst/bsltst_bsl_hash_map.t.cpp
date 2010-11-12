// bsltst_hash_map.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_hash_map.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <algorithm>
#include <hash_map>
#include <vector>

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
//-----------------------------------------------------------------------------
// [ 6] HASH_MULTIMAP BASIC MANIPULATOR, ACCESSOR, AND ITERATOR TEST
// [ 5] HASH_MAP BASIC MANIPULATOR, ACCESSOR, AND ITERATOR TEST
// [ 4] HASH_MULTIMAP STORING & C'TOR TEST
// [ 3] HASH_MULTIMAP BREATHING TEST
// [ 2] HASH_MAP STORING & C'TOR TEST
// [ 1] HASH_MAP BREATHING TEST
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

struct HashInt {
    static bool s_used;

    int operator()(const int& i) const
    {
        s_used = true;
        return i;
    }
};
bool HashInt::s_used = false;

struct EqualInt {
    static bool s_used;

    int operator()(const int& lhs, const int& rhs) const
    {
        s_used = true;
        return lhs == rhs;
    }
};
bool EqualInt::s_used = false;

struct Cargo {
    void            *d_p;
    bslma_Allocator *d_alloc;

    enum { BULK_STORAGE = 4000 };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg_TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(bslma_Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BULK_STORAGE);
    }
    Cargo(const Cargo& in, bslma_Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BULK_STORAGE);
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
    }
    Cargo& operator=(const Cargo& in) {
        QV("Assign:");
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
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

    bslma_TestAllocator tda;
    bslma_TestAllocator ta;

    bslma_DefaultAllocatorGuard guard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // HASH_MULTIMAP BASIC MANIPULATOR AND ACCESSOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors of hash_multimap work as
        //   documented.
        //
        // Plan:
        //   Since a hash_multimap can do most things a hash_map can do,
        //   repeat the test for hash_map here with minor modification, then
        //   go into specific hash_multimap behavior.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                       "\nHASH_MULTIMAP BASIC MANIPULATOR AND ACCESSOR TEST\n"
                         "=================================================\n";

        typedef bsl::hash_multimap<int, int, HashInt, EqualInt> Obj;
        Obj hma(&ta);

        typedef bsl::pair<const int, int> Pr;

        Obj::iterator it;
        Obj::const_iterator itc;

        ASSERT(hma.empty());
        ASSERT(0 == hma.size());

        hma.insert(Pr(0, 0));
        hma.insert(Pr(1, 1));

        ASSERT(!hma.empty());
        ASSERT(2 == hma.size());

        Obj hmb(hma, &ta);

        ASSERT(hma == hmb);
        ASSERT(!(hma != hmb));

        it = hma.find(0);
        it->second = -1;

        ASSERT(hma != hmb);
        ASSERT(!(hma == hmb));

        hma = hmb;

        ASSERT(hma == hmb);
        ASSERT(!(hma != hmb));

        int j = hma.bucket_count();
        j *= 4;
        hma.resize(j);
        ASSERT((int) hma.bucket_count() >= j);

        {
            HashInt hi = hma.hash_funct();
            ASSERT(0 == hi(0));
            EqualInt ei = hma.key_eq();
            ASSERT( ei(3, 3));
            ASSERT(!ei(2, 3));
        }

        hmb.insert(Pr(2, 2));
        hmb.insert(Pr(3, 3));
        hmb.insert(Pr(4, 4));
        hmb.insert(Pr(5, 5));

        hma.swap(hmb);

        ASSERT(6 == hma.size());
        ASSERT(2 == hmb.size());

        bsl::swap(hma, hmb);

        ASSERT(2 == hma.size());
        ASSERT(6 == hmb.size());

        hmb.swap(hma);

        it = hma.find(2);
        ASSERT(2 == it->first);
        ASSERT(2 == it->second);

        hma.erase(it);
        hma.erase(3);

        ASSERT(4 == hma.size());

        j = 0;
        for (int i = -5; i < 20; ++i) {
            ASSERT(hma.count(i) == (i >= 0 && i < 6 && 2 != i && 3 != i));
            if (hma.count(i)) {
                ++j;
            }
        }
        ASSERT(4 == j);

        const Obj hmc = hma;
        itc = hmc.find(1);
        ASSERT(1 == itc->first);
        ASSERT(1 == itc->second);

        bsl::pair<Obj::iterator, Obj::iterator> prI;
        prI = hma.equal_range(1);
        it = hma.find(1);
        ASSERT(prI.first == it);
        ASSERT(prI.first != prI.second);
        ASSERT(prI.second == hma.end() || 1 != prI.second->first);

        bsl::pair<Obj::const_iterator, Obj::const_iterator> prIC;
        prIC = hmc.equal_range(1);
        ASSERT(prIC.first == itc);
        ASSERT(prIC.first != prIC.second);
        ASSERT(prIC.second == hmc.end() || 1 != prIC.second->first);

        if (verbose) cout << "Specific multimap functionality\n";

        {
            const int vals[] = { 1, 1, 0, 0, 1, 1 };
            enum { VALS_SIZE = sizeof vals / sizeof *vals };

            for (int i = 0; i < VALS_SIZE; ++i) {
                if (vals[i]) {
                    hma.insert(Pr(i, i));
                }
            }
        }

        LOOP_ASSERT(hma.size(), 8 == hma.size());

        j = 0;
        int k = 0;
        for (int i = -5; i < 20; ++i) {
            LOOP2_ASSERT(i, hma.count(i), hma.count(i) ==
                              ((i >= 0 && i < 6 && 2 != i && 3 != i) ? 2 : 0));
            if (hma.count(i)) {
                ++j;
            }
            k += hma.count(i);
        }
        ASSERT(4 == j);
        ASSERT(8 == k);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // HASH_MAP BASIC MANIPULATOR AND ACCESSOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors of hash_multi work as
        //   documented.
        //
        // Plan:
        //   Create a couple of hash_maps, store things in them, observe
        //   them with the accessors, make sure the behavior is as documented.
        // --------------------------------------------------------------------

        if (verbose) cout <<"\nHASH_MAP BASIC MANIPULATOR AND ACCESSOR TEST\n"
                              "============================================\n";

        typedef bsl::hash_map<int, int, HashInt, EqualInt> Obj;
        Obj hma(&ta);

        typedef bsl::pair<const int, int> Pr;

        Obj::iterator it;
        Obj::const_iterator itc;

        ASSERT(hma.empty());
        ASSERT(0 == hma.size());

        hma[0] = 0;
        hma.insert(Pr(1, 1));

        ASSERT(!hma.empty());
        ASSERT(2 == hma.size());

        Obj hmb(hma, &ta);

        ASSERT(hma == hmb);
        ASSERT(!(hma != hmb));

        hma[0] = -1;

        ASSERT(hma != hmb);
        ASSERT(!(hma == hmb));

        hma = hmb;

        ASSERT(hma == hmb);
        ASSERT(!(hma != hmb));

        int j = hma.bucket_count();
        j *= 4;
        hma.resize(j);
        ASSERT((int) hma.bucket_count() >= j);

        {
            HashInt hi = hma.hash_funct();
            ASSERT(0 == hi(0));
            EqualInt ei = hma.key_eq();
            ASSERT( ei(3, 3));
            ASSERT(!ei(2, 3));
        }

        hmb.insert(Pr(2, 2));
        hmb.insert(Pr(3, 3));
        hmb.insert(Pr(4, 4));
        hmb.insert(Pr(5, 5));

        hma.swap(hmb);

        ASSERT(6 == hma.size());
        ASSERT(2 == hmb.size());

        bsl::swap(hma, hmb);

        ASSERT(2 == hma.size());
        ASSERT(6 == hmb.size());

        hmb.swap(hma);

        it = hma.find(2);
        ASSERT(2 == it->first);
        ASSERT(2 == it->second);

        hma.erase(it);
        hma.erase(3);

        ASSERT(4 == hma.size());

        j = 0;
        for (int i = -5; i < 20; ++i) {
            ASSERT(hma.count(i) == (i >= 0 && i < 6 && 2 != i && 3 != i));
            if (hma.count(i)) {
                ++j;
            }
        }
        ASSERT(4 == j);

        for (j = 0, it = hma.begin(); hma.end() != it; ++it) {
            ++j;
        }
        ASSERT(4 == j);

        const Obj hmc = hma;
        itc = hmc.find(1);
        ASSERT(1 == itc->first);
        ASSERT(1 == itc->second);

        bsl::pair<Obj::iterator, Obj::iterator> prI;
        prI = hma.equal_range(1);
        it = hma.find(1);
        ASSERT(prI.first == it);
        ASSERT(prI.first != prI.second);
        ASSERT(prI.second == hma.end() || 1 != prI.second->first);

        bsl::pair<Obj::const_iterator, Obj::const_iterator> prIC;
        prIC = hmc.equal_range(1);
        ASSERT(prIC.first == itc);
        ASSERT(prIC.first != prIC.second);
        ASSERT(prIC.second == hmc.end() || 1 != prIC.second->first);

        ASSERT(1 == hma[1]);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // HASH_MULTIMAP STORING AND CONSTRUCTOR TEST
        //
        // Concerns:
        //   That elements stored in the container are passed the right memroy
        //   allocator, and that all c'tors compile.
        //
        // Plan:
        //   Basically repeat the storing test done for hash_map adapted to
        //   multimap.  When inserting always use the same key to verify
        //   multimap stores multiple instances of things.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASH_MULTIMAP STORING & C'TOR TEST\n"
                               "==================================\n";

        {
            bsl::hash_multimap<int, Cargo> hm(&ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
        }

        {
            bsl::hash_multimap<int, Cargo> hm;
            (void) hm.count(0);
        }

        {
            bsl::hash_multimap<int, Cargo> hm(10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_multimap<int, Cargo> hm(10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt> hm(10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt> hm(10, HashInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt>
                                            hm(10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }
            for (int i = 0; i < 10; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt>
                                                 hm(10, HashInt(), EqualInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt> hm(10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }
            for (int i = 0; i < 10; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt> hm(10);
            (void) hm.count(0);
        }

        bsl::vector<bsl::pair<int, Cargo> > v;    // default allocator
        {
            Cargo cy;

            v.push_back(bsl::make_pair(20, cy));
            v.push_back(bsl::make_pair(21, cy));
        }

        {
            bsl::hash_multimap<int, Cargo> hm(v.begin(), v.end(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_multimap<int, Cargo> hm(v.begin(), v.end());
            (void) hm.count(0);
        }

        {
            bsl::hash_multimap<int, Cargo> hm(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_multimap<int, Cargo> hm(v.begin(), v.end(), 10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt>
                                    hm(v.begin(), v.end(), 10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt> hm(v.begin(), v.end(), 10,
                                                                    HashInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt> hm(v.begin(), v.end(), 10,
                                                                          &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt> hm(v.begin(), v.end(), 10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt>
                        hm(v.begin(), v.end(), 10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }
            for (int i = 0; i < 1000; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt>
                             hm(v.begin(), v.end(), 10, HashInt(), EqualInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt>
                                               hm(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(0, cx));
            }
            ASSERT(10 == hm.count(0));
            for (int i = 0; i < 1000; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multimap<int, Cargo, HashInt, EqualInt> hm(v.begin(),
                                                                  v.end(), 10);
            (void) hm.count(0);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // HASH_MULTIMAP BREATHING TEST
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

        if (verbose) cout << "\nHASH_MULTIMAP BREATHING TEST\n"
                               "============================\n";

        bsl::hash_multimap<int, int> b(&ta);
        std::hash_multimap<int, int> s(&ta);
//      native_std::hash_multimap<int, int> n;

        bsl::hash_multimap<int, int>::iterator bit;
        std::hash_multimap<int, int>::iterator sit;
//      native_std::hash_multimap<int, int>::iterator nit;

        ASSERT(true  == sameType(b, s));
//      ASSERT(false == sameType(s, n));
//      ASSERT(false == sameType(b, n));

        ASSERT(true  == sameType(bit, sit));
//      ASSERT(false == sameType(sit, nit));
//      ASSERT(false == sameType(bit, nit));

        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(true  == usesBslmaAllocator(s));
//      ASSERT(false == usesBslmaAllocator(n));

        ASSERT(false == usesBslmaAllocator(bit));
        ASSERT(false == usesBslmaAllocator(sit));
//      ASSERT(false == usesBslmaAllocator(nit));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HASH_MAP STORING AND CONSTRUCTOR TEST
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
        //   all c'tors of the container compile.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASH_MAP STORING AND C'TOR TEST\n"
                               "===============================\n";

        {
            bsl::hash_map<int, Cargo> hm(&ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_map<int, Cargo> hm;
            (void) hm.count(0);
        }

        {
            bsl::hash_map<int, Cargo> hm(10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_map<int, Cargo> hm(10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt> hm(10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt> hm(10, HashInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt, EqualInt>
                                            hm(10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }
        {
            bsl::hash_map<int, Cargo, HashInt, EqualInt>
                                                 hm(10, HashInt(), EqualInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt, EqualInt> hm(10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt, EqualInt> hm(10);
            (void) hm.count(0);
        }

        bsl::vector<bsl::pair<int, Cargo> > v;    // default allocator
        {
            Cargo cy;

            v.push_back(bsl::make_pair(20, cy));
            v.push_back(bsl::make_pair(21, cy));
        }

        {
            bsl::hash_map<int, Cargo> hm(v.begin(), v.end(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_map<int, Cargo> hm(v.begin(), v.end());
            (void) hm.count(0);
        }

        {
            bsl::hash_map<int, Cargo> hm(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                      ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_map<int, Cargo> hm(v.begin(), v.end(), 10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt>
                                    hm(v.begin(), v.end(), 10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt> hm(v.begin(), v.end(), 10,
                                                                    HashInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt> hm(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt> hm(v.begin(), v.end(), 10);
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt, EqualInt>
                        hm(v.begin(), v.end(), 10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 1000; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt, EqualInt>
                             hm(v.begin(), v.end(), 10, HashInt(), EqualInt());
            (void) hm.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_map<int, Cargo, HashInt, EqualInt>
                                               hm(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            Cargo cx;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                hm.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 1000; ++i) {
                hm.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed + 10 * Cargo::BULK_STORAGE);
            LOOP2_ASSERT(tda.numBytesInUse(), defaultMemUsed,
                                        tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_map<int, Cargo, HashInt, EqualInt> hm(v.begin(),
                                                                  v.end(), 10);
            (void) hm.count(0);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HASH_MAP BREATHING TEST
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

        if (verbose) cout << "\nHASH_MAP BREATHING TEST\n"
                               "=======================\n";

        // native hash_map doesn't exist

        bsl::hash_map<int, int> b(&ta);
        std::hash_map<int, int> s(&ta);
//      native_std::hash_map<int, int> n;

        bsl::hash_map<int, int>::iterator bit;
        std::hash_map<int, int>::iterator sit;
//      native_std::hash_map<int, int>::iterator nit;

        ASSERT(true  == sameType(b, s));
//      ASSERT(false == sameType(s, n));
//      ASSERT(false == sameType(b, n));

        ASSERT(true  == sameType(bit, sit));
//      ASSERT(false == sameType(sit, nit));
//      ASSERT(false == sameType(bit, nit));

        ASSERT(true  == usesBslmaAllocator(b));
        ASSERT(true  == usesBslmaAllocator(s));
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
