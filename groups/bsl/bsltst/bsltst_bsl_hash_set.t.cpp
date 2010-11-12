// bsltst_hash_set.t.cpp                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsltst_bsl_hash_set.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <hash_set>
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
// [ 6] hash_multiset manipulator / accessor / iterator test
// [ 5] hash_set manipulator / accessor / iterator test
// [ 4] hash_multiset storage and c'tor test
// [ 3] hash_set storage and c'tor test
// [ 2] hash_multiset breathing test
// [ 1] hash_set breathing test
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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
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
    // This struct dynamically allocates memory, for verifying that the
    // memory allocator is properly passed to elements within a container.

    int              d_i;
    void            *d_p;
    bslma_Allocator *d_alloc;

    enum { BULK_STORAGE = 4000 };

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg_TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(int i, bslma_Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_i = i;
        d_alloc = bslma_Default::allocator(a);
        d_p = d_alloc->allocate(BULK_STORAGE);
    }
    Cargo(const Cargo& in, bslma_Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma_Default::allocator(a);
        d_i = in.d_i;
        d_p = d_alloc->allocate(BULK_STORAGE);
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
    }
    Cargo& operator=(const Cargo& in) {
        QV("Assign:");
        d_i = in.d_i;
        std::memcpy(d_p, in.d_p, BULK_STORAGE);
        return *this;
    }
    ~Cargo() {
        d_alloc->deallocate(d_p);
    }
};

bool operator==(const Cargo& lhs, const Cargo& rhs)
{
    return lhs.d_i == rhs.d_i;
}

namespace bsl {

template <>
struct hash<Cargo> {
    size_t operator()(const Cargo in) const
    {
        return (size_t) in.d_i;
    }
};

}

struct HashCargo {
    static bool s_used;

    int operator()(const Cargo& in) const
    {
        s_used = true;
        return in.d_i;
    }
};
bool HashCargo::s_used = false;

struct EqualCargo {
    static bool s_used;

    int operator()(const Cargo& lhs, const Cargo& rhs) const
    {
        s_used = true;
        return lhs == rhs;
    }
};
bool EqualCargo::s_used = false;

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
      case 6: {
        // --------------------------------------------------------------------
        // HASH_MULTISET BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the manipulators and accessors of hash_multiset work as
        //   expected.
        //
        // Plan:
        //   Since a hash multiset can do most things a hash set can do, we
        //   start by just repeating a barely modified version of the hash_set
        //   test, then branch out to the functionality specific to the
        //   hash_multiset.
        // --------------------------------------------------------------------

        if (verbose) cout <<
              "\nHASH_MULTISET BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
              "============================================================\n";

        typedef bsl::hash_multiset<int, HashInt, EqualInt> Obj;

        Obj hsa(&ta);

        ASSERT(hsa.empty());

        int j = hsa.bucket_count();
        j *= 4;
        hsa.resize(j);
        ASSERT((int) hsa.bucket_count() >= j);
        ASSERT(hsa.empty());

        {
            HashInt hi = hsa.hash_funct();
            ASSERT(0 ==  hi(0));
            EqualInt ei = hsa.key_eq();
            ASSERT(false == ei(0, 1));
        }

        hsa.insert(20);
        hsa.insert(21);

        ASSERT(!hsa.empty());

        Obj hsb(hsa, &ta);

        ASSERT(hsa == hsb);
        ASSERT(!(hsa != hsb));

        hsa.insert(22);

        ASSERT(hsa != hsb);
        ASSERT(!(hsa == hsb));

        hsa.swap(hsb);
        ASSERT( hsb.count(22));
        ASSERT(!hsa.count(22));

        hsb = hsa;

        ASSERT(hsa == hsb);
        ASSERT(!(hsa != hsb));

        ASSERT(!hsa.count(22));
        ASSERT(!hsb.count(22));

        ASSERT(hsa.size() == 2);

        hsb.clear();
        ASSERT(!hsb.count(20));
        ASSERT(!hsb.count(21));

        Obj::iterator it;
        it = hsb.insert(22);
        ASSERT(22 == *it);
        hsb.insert(23);
        hsb.insert(24);

        ASSERT(hsb.size() == 3);

        hsa.insert(hsb.begin(), hsb.end());
        ASSERT(hsa.size() == 5);
        for (int i = 0; i < 100; ++i) {
            ASSERT(hsa.count(i) == (i >= 20 && i < 25));
        }

        hsa.erase(24);
        ASSERT(hsa.size() == 4);
        for (int i = 0; i < 100; ++i) {
            ASSERT(hsa.count(i) == (i >= 20 && i < 24));
        }

        bsl::vector<bool> v(100, false);
        for (it = hsa.begin(), j = 0; hsa.end() != it; ++it, ++j) {
            v[*it] = true;
            ASSERT(*it >= 20 && *it < 24);
        }
        ASSERT(4 == j);
        j = 0;
        for (int i = 0; i < 100; ++i) {
            ASSERT(v[i] == (i >= 20 && i < 24));
            j += v[i];
        }
        ASSERT(4 == j);
        it = hsa.find(21);
        ASSERT(*it == 21);
        hsa.erase(it);
        it = hsa.find(22);
        ASSERT(*it == 22);
        hsa.erase(it);

        for (int i = 0; i < 100; ++i) {
            v[i] = false;
        }
        for (it = hsa.begin(), j = 0; hsa.end() != it; ++it, ++j) {
            v[*it] = true;
            ASSERT(20 == *it || 23 == *it);
        }
        ASSERT(2 == j);

        j = 0;
        for (int i = 0; i < 100; ++i) {
            ASSERT(v[i] == (20 == i || 23 == i));
            j += v[i];
        }
        ASSERT(2 == j);

        bsl::pair<Obj::iterator, Obj::iterator> prIt;
        prIt = hsa.equal_range(23);
        it = hsa.find(23);
        ASSERT(it == prIt.first);
        ASSERT(23 == *prIt.first);
        ASSERT(prIt.first != prIt.second);

        it = prIt.first;
        ++it;
        ASSERT(it == prIt.second);

        LOOP_ASSERT(hsa.count(23), 1 == hsa.count(23));
        LOOP_ASSERT(hsa.size(), 2 == hsa.size());       

        it = hsa.insert(23);        // redundant, will succeed
        ASSERT(23 == *it);

        LOOP_ASSERT(hsa.count(23), 2 == hsa.count(23));
        LOOP_ASSERT(hsa.size(), 3 == hsa.size());       

        prIt = hsa.equal_range(23);
        it = hsa.find(23);
        ASSERT(it == prIt.first);
        ASSERT(23 == *prIt.first);
        ASSERT(prIt.first != prIt.second);

        ++it;
        ASSERT(23 == *it);
        ++it;
        ASSERT(it == prIt.second);
        ASSERT(hsa.end() == it || 23 != *it);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // HASH_SET BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors work as documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                   "\nHASH_SET BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
                   "=======================================================\n";

        typedef bsl::hash_set<int, HashInt, EqualInt> Obj;

        Obj hsa(&ta);

        ASSERT(hsa.empty());

        int j = hsa.bucket_count();
        j *= 4;
        hsa.resize(j);
        ASSERT((int) hsa.bucket_count() >= j);
        ASSERT(hsa.empty());

        {
            HashInt hi = hsa.hash_funct();
            ASSERT(0 ==  hi(0));
            EqualInt ei = hsa.key_eq();
            ASSERT(false == ei(0, 1));
        }

        hsa.insert(20);
        hsa.insert(21);

        ASSERT(!hsa.empty());

        Obj hsb(hsa, &ta);

        ASSERT(hsa == hsb);
        ASSERT(!(hsa != hsb));

        hsa.insert(22);

        ASSERT(hsa != hsb);
        ASSERT(!(hsa == hsb));

        hsa.swap(hsb);
        ASSERT( hsb.count(22));
        ASSERT(!hsa.count(22));

        hsb = hsa;

        ASSERT(hsa == hsb);
        ASSERT(!(hsa != hsb));

        ASSERT(!hsa.count(22));
        ASSERT(!hsb.count(22));

        ASSERT(hsa.size() == 2);

        hsb.clear();
        ASSERT(!hsb.count(20));
        ASSERT(!hsb.count(21));

        bsl::pair<Obj::iterator, bool> pr;
        pr = hsb.insert(22);
        ASSERT(pr.second);
        ASSERT(22 == *pr.first);
        hsb.insert(23);
        hsb.insert(24);

        ASSERT(hsb.size() == 3);

        hsa.insert(hsb.begin(), hsb.end());
        ASSERT(hsa.size() == 5);
        for (int i = 0; i < 100; ++i) {
            ASSERT(hsa.count(i) == (i >= 20 && i < 25));
        }

        hsa.erase(24);
        ASSERT(hsa.size() == 4);
        for (int i = 0; i < 100; ++i) {
            ASSERT(hsa.count(i) == (i >= 20 && i < 24));
        }

        bsl::vector<bool> v(100, false);
        Obj::iterator it;
        for (it = hsa.begin(), j = 0; hsa.end() != it; ++it, ++j) {
            v[*it] = true;
            ASSERT(*it >= 20 && *it < 24);
        }
        ASSERT(4 == j);
        j = 0;
        for (int i = 0; i < 100; ++i) {
            ASSERT(v[i] == (i >= 20 && i < 24));
            j += v[i];
        }
        ASSERT(4 == j);
        it = hsa.find(21);
        ASSERT(*it == 21);
        hsa.erase(it);
        it = hsa.find(22);
        ASSERT(*it == 22);
        hsa.erase(it);

        for (int i = 0; i < 100; ++i) {
            v[i] = false;
        }
        for (it = hsa.begin(), j = 0; hsa.end() != it; ++it, ++j) {
            v[*it] = true;
            ASSERT(20 == *it || 23 == *it);
        }
        ASSERT(2 == j);

        j = 0;
        for (int i = 0; i < 100; ++i) {
            ASSERT(v[i] == (20 == i || 23 == i));
            j += v[i];
        }
        ASSERT(2 == j);

        pr = hsb.insert(23);        // redundant, will fail
        ASSERT(!pr.second);
        ASSERT(23 == *pr.first);    // finds element that was already there

        bsl::pair<Obj::iterator, Obj::iterator> prIt;
        prIt = hsa.equal_range(23);
        it = hsa.find(23);
        ASSERT(it == prIt.first);
        ASSERT(23 == *prIt.first);
        ASSERT(prIt.first != prIt.second);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // HASH_MULTISET STORAGE AND CONSTRUCTOR TEST
        //
        // Concerns:
        //   That hash multiset uses the allocator passed to it, and that all
        //   the expected c'tor exist.
        //
        // Plan:
        //   Just repeat the same tests done on the hash_set in case 3.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nHASH_MULTISET STORAGE AND CONSTRUCTOR TEST\n"
                               "==========================================\n";

        {
            bsl::hash_multiset<int> hs(&ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 10* 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_multiset<int> hs;
            (void) hs.count(0);
        }

        {
            bsl::hash_multiset<int> hs(10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_multiset<int> hs(10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multiset<int, HashInt> hs(10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt> hs(10, HashInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multiset<int, HashInt, EqualInt>
                                            hs(10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt, EqualInt>
                                                 hs(10, HashInt(), EqualInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multiset<int, HashInt, EqualInt> hs(10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt, EqualInt> hs(10);
            (void) hs.count(0);
        }

        bsl::vector<int> v;    // default allocator
        v.push_back(0);
        v.push_back(1);

        {
            bsl::hash_multiset<int> hs(v.begin(), v.end(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 10 * 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_multiset<int> hs(v.begin(), v.end());
            (void) hs.count(0);
        }

        {
            bsl::hash_multiset<int> hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_multiset<int> hs(v.begin(), v.end(), 10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multiset<int, HashInt>
                                    hs(v.begin(), v.end(), 10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt>
                                         hs(v.begin(), v.end(), 10, HashInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_multiset<int, HashInt> hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt> hs(v.begin(), v.end(), 10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multiset<int, HashInt, EqualInt>
                        hs(v.begin(), v.end(), 10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt, EqualInt>
                             hs(v.begin(), v.end(), 10, HashInt(), EqualInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_multiset<int, HashInt, EqualInt>
                                               hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_multiset<int, HashInt, EqualInt> hs(v.begin(), v.end(),
                                                                           10);
            (void) hs.count(0);
        }

        {
            bsl::hash_multiset<Cargo> hs(&ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10* 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_multiset<Cargo> hs(10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo> hs(10, HashCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo, EqualCargo>
                                        hs(10, HashCargo(), EqualCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo, EqualCargo> hs(10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        bsl::vector<Cargo> cv;    // default allocator
        cv.push_back(Cargo(-2));
        cv.push_back(Cargo(-1));

        {
            bsl::hash_multiset<Cargo> hs(cv.begin(), cv.end(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = -2; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }
            for (int i = -10; i > -2; ++i) {
                ASSERT(0 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_multiset<Cargo> hs(cv.begin(), cv.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo>
                                hs(cv.begin(), cv.end(), 10, HashCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo> hs(cv.begin(), cv.end(), 10,
                               &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo, EqualCargo>
                  hs(cv.begin(), cv.end(), 10, HashCargo(), EqualCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }
            for (int i = -3; i >= -10; --i) {
                ASSERT(0 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_multiset<Cargo, HashCargo, EqualCargo>
                                             hs(cv.begin(), cv.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
                ASSERT(0 == hs.count(Cargo(1000 + i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // HASH_SET STORAGE AND CONSTRUCTOR TEST
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

        if (verbose) cout << "\nHASH_SET STORAGE AND CONSTRUCTOR TEST\n"
                               "=====================================\n";

        {
            bsl::hash_set<int> hs(&ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 10* 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_set<int> hs;
            (void) hs.count(0);
        }

        {
            bsl::hash_set<int> hs(10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_set<int> hs(10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_set<int, HashInt> hs(10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt> hs(10, HashInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_set<int, HashInt, EqualInt>
                                            hs(10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt, EqualInt>
                                                 hs(10, HashInt(), EqualInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_set<int, HashInt, EqualInt> hs(10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt, EqualInt> hs(10);
            (void) hs.count(0);
        }

        bsl::vector<int> v;    // default allocator
        v.push_back(0);
        v.push_back(1);

        {
            bsl::hash_set<int> hs(v.begin(), v.end(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 10 * 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_set<int> hs(v.begin(), v.end());
            (void) hs.count(0);
        }

        {
            bsl::hash_set<int> hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
        }

        {
            bsl::hash_set<int> hs(v.begin(), v.end(), 10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_set<int, HashInt>
                                    hs(v.begin(), v.end(), 10, HashInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt> hs(v.begin(), v.end(), 10, HashInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            bsl::hash_set<int, HashInt> hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt> hs(v.begin(), v.end(), 10);
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_set<int, HashInt, EqualInt>
                        hs(v.begin(), v.end(), 10, HashInt(), EqualInt(), &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt, EqualInt>
                             hs(v.begin(), v.end(), 10, HashInt(), EqualInt());
            (void) hs.count(0);
        }

        {
            HashInt::s_used = false;
            EqualInt::s_used = false;
            bsl::hash_set<int, HashInt, EqualInt>
                                               hs(v.begin(), v.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(i);
            }
            for (int i = 0; i < 1000; ++i) {
                hs.count(i);
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(HashInt::s_used);
            ASSERT(EqualInt::s_used);
        }

        {
            bsl::hash_set<int, HashInt, EqualInt> hs(v.begin(), v.end(), 10);
            (void) hs.count(0);
        }

        {
            bsl::hash_set<Cargo> hs(&ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10* 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_set<Cargo> hs(10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo> hs(10, HashCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 100; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo, EqualCargo>
                                        hs(10, HashCargo(), EqualCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo, EqualCargo> hs(10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        bsl::vector<Cargo> cv;    // default allocator
        cv.push_back(Cargo(-2));
        cv.push_back(Cargo(-1));

        {
            bsl::hash_set<Cargo> hs(cv.begin(), cv.end(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = -2; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }
            for (int i = -10; i > -2; ++i) {
                ASSERT(0 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::hash_set<Cargo> hs(cv.begin(), cv.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo>
                                hs(cv.begin(), cv.end(), 10, HashCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo> hs(cv.begin(), cv.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo, EqualCargo>
                  hs(cv.begin(), cv.end(), 10, HashCargo(), EqualCargo(), &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
            }
            for (int i = -3; i >= -10; --i) {
                ASSERT(0 == hs.count(Cargo(i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }

        {
            HashCargo::s_used = false;
            EqualCargo::s_used = false;
            bsl::hash_set<Cargo, HashCargo, EqualCargo>
                                             hs(cv.begin(), cv.end(), 10, &ta);

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 1000; ++i) {
                hs.insert(Cargo(i));
            }
            for (int i = 0; i < 1000; ++i) {
                ASSERT(1 == hs.count(Cargo(i)));
                ASSERT(0 == hs.count(Cargo(1000 + i)));
            }

            ASSERT(ta.numBytesInUse() > memUsed);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
            ASSERT(HashCargo::s_used);
            ASSERT(EqualCargo::s_used);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HASH_MULTISET BREATHING TEST
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

        if (verbose) cout << "\nHASH_MULTISET BREATHING TEST\n"
                               "============================\n";

        // there is no native hash_multiset

        bsl::hash_multiset<int> hms(&ta);

        ASSERT(true  == sameType(hms, std::hash_multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::hash_multiset<int>()));
        ASSERT(true  == usesBslmaAllocator(std::hash_multiset<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::hash_multiset<int>()));

        ASSERT(true  == sameType(bsl::hash_multiset<int>::iterator(),
                                 std::hash_multiset<int>::iterator()));
//      ASSERT(false == sameType(bsl::hash_multiset<int>::iterator(),
//                        native_std::hash_multiset<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(
                                         bsl::hash_multiset<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(
                                         std::hash_multiset<int>::iterator()));
//      ASSERT(false == usesBslmaAllocator(
//                                native_std::hash_multiset<int>::iterator()));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HASH_SET BREATHING TEST
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

        if (verbose) cout << "\nHASH SET BREATHING TEST\n"
                               "=====-----=============\n";

        bsl::hash_set<int> hs(&ta);

        // there is no native hash_set

        ASSERT(true  == sameType(hs, std::hash_set<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::hash_set<int>()));
        ASSERT(true  == usesBslmaAllocator(std::hash_set<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::hash_set<int>()));

        ASSERT(true  == sameType(bsl::hash_set<int>::iterator(),
                                 std::hash_set<int>::iterator()));
//      ASSERT(false == sameType(bsl::hash_set<int>::iterator(),
//                        native_std::hash_set<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::hash_set<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::hash_set<int>::iterator()));
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
