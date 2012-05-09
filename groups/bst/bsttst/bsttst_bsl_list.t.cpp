// bsttst_list.t.cpp                                                  -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsttst_bsl_list.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <algorithm>
#include <list>

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
// [ 4] Advanced Manipulator / Accessor / Iterator Test
// [ 3] Basic Manipulator / Accessor / Iterator Test
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
    if (!(X)) { cout << #I << ": " << (I) << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " \
        << (J) << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) << "\t" \
              << #K << ": " << (K) << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) << \
       "\t" << #K << ": " << (K) << "\t" << #L << ": " << (L) << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) << \
       "\t" <<  #K << ": " << (K) << "\t" << #L << ": " << (L) << "\t" << \
       #M << ": " << (M) << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << (I) << "\t" << #J << ": " << (J) << \
       "\t" << #K << ": " << (K) << "\t" << #L << ": " << (L) << "\t" << \
       #M << ": " << (M) << "\t" << #N << ": " << (N) << "\n"; \
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

static
bool positive(int i) {
    return i > 0;
}

static
bool lessThan(int i, int j) {
    return i < j;
}

static
bool equals(int i, int j) {
    return i == j;
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

    bslma::TestAllocator ta;
    bslma::TestAllocator tda;

    bslma::DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // DEMO bslma_TestAllocatorMonitor::reset..
        //
        // Concerns:
        //   Demonstrate the usefulness of the 'reset' method of
        //   'bslma_TestAllocatorMonitor'.  There are no allocating types
        //   available at 'bslma' level or lower, so we do it here.
        //
        // Plan:
        //   Do operations on a list that allocate memory from an allocator
        //   that is being monitored, demonstrating the usefulness of being
        //   able to reset the monitor after every allocation takes place.
        // --------------------------------------------------------------------

        // In this example, we observe how some of the manipulators and
        // accessors of 'bsl::list' use memory.  First, we declare monitors
        // 'tam' and 'dam' that monitor the allocator passed to a list at
        // construction, and the default allocator, respectively.  The default
        // allocator 'tda' should never be used for anything in this exercise,
        // so we will only check its monitor 'dam' at the end of the example.

        bslma_TestAllocatorMonitor tam(&ta);
        bslma_TestAllocatorMonitor dam(&tda);

        // Then, we observe that creating an empty list allocates memory:

        bsl::list<int> la(&ta);

        ASSERT(tam.isTotalUp());

        // Next, we observe that 'push_back' also allocates memory:

        tam.reset();

        la.push_back(57);

        ASSERT(tam.isTotalUp());

        la.push_back(57);

        // Then, we observe that accessors 'back', 'front', and 'size' don't
        // cause any allocation.

        tam.reset();

        ASSERT(57 == la.back());
        ASSERT(57 == la.front());
        ASSERT(2  == la.size());

        ASSERT(tam.isTotalSame());

        // Next, we observe that manipulator 'push_front' causes memory
        // allocation:

        la.push_front(23);

        ASSERT(tam.isTotalUp());

        ASSERT(57 == la.back());
        ASSERT(23 == la.front());
        ASSERT(3  == la.size());

        // Then, we observe that 'reverse', though it's a manipulator, does no
        // memory allocation:

        tam.reset();

        la.reverse();

        ASSERT(tam.isInUseSame());
        ASSERT(tam.isTotalSame());

        ASSERT(23 == la.back());
        ASSERT(57 == la.front());
        ASSERT(3  == la.size());

        // Next, we observe that 'unique', in this case, will do no new memory
        // allocation and will in fact free some memory.

        tam.reset();

        la.unique();

        ASSERT(tam.isInUseDown());
        ASSERT(tam.isTotalSame());

        // Then, we observe the state of the list after 'unique'.
        
        ASSERT(23 == la.back());
        ASSERT(57 == la.front());
        ASSERT(2  == la.size());

        // Finally, we observe that no memory was ever allocated from the
        // default allocator.

        ASSERT(dam.isTotalSame());
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // ADVANCED MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors work as documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.
        // --------------------------------------------------------------------

        bsl::list<int> la(&ta);
        for (int i = 0; i < 20; ++i) {
            la.push_back(i);
        }

        bsl::list<int> lb(&ta);

        int j;
        bsl::list<int>::iterator it;

        lb.resize(10, 100);
        ASSERT(10 == lb.size());
        for (j = 0, it = lb.begin(); lb.end() != it; ++j, ++it) {
            ASSERT(100 == *it);
        }
        ASSERT(10 == j);

        lb.unique();
        ASSERT(1 == lb.size());
        lb.resize(10, 200);

        for (j = 0, it = lb.begin(); lb.end() != it; ++j, ++it) {
            if (j) {
                ASSERT(200 == *it);
            }
            else {
                ASSERT(100 == *it);
            }
        }
        ASSERT(10 == j);
        lb.unique();

        it = lb.begin();  ++it;
        lb.splice(it, la);
        ASSERT(22 == lb.size());
        ASSERT(0  == la.size());

        for (it = lb.begin(), j = -1; lb.end() != it; ++it, ++j) {
            if (0 > j) {
                ASSERT(100 == *it);
            }
            else if (20 > j) {
                ASSERT(j == *it);
            }
            else {
                ASSERT(200 == *it);
            }
        }

        lb.erase(lb.begin());
        ASSERT(21 == lb.size());
        lb.resize(20);
        ASSERT(20 == lb.size());

        for (it = lb.begin(), j = 0; lb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        lb.reverse();
        bsl::list<int>::reverse_iterator rit;

        for (rit = lb.rbegin(), j = 0; lb.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j);
        }

        lb.reverse();

        la = lb;

        lb.resize(5);
        it = lb.begin();
        for (int i = -1; i >= -5; --i) {
            lb.push_front(i);
        }

        bsl::list<int>::iterator itb, itc = la.end();
        --itc;

        lb.splice(it, la, itc);
        ASSERT(11 == lb.size());
        ASSERT(19 == la.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            if (j < 0) {
                ASSERT(*itb == j);
            }
            else if (!j) {
                ASSERT(19 == *itb);
            } else {
                ASSERT(j - 1 == *itb);
            }
        }

        // now put it back
        --it;
        la.splice(la.end(), lb, it);
        ASSERT(10 == lb.size());
        ASSERT(20 == la.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        for (j = 0, itb = la.begin(); la.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        for (j = 0, it  = la.begin(); j < 5; ++j, ++it ) { }
        for (j = 0, itb = lb.begin(); j < 5; ++j, ++itb) { }

        lb.splice(itb, la, la.begin(), it);
        ASSERT(15 == lb.size());
        ASSERT(15 == la.size());

        for (j = -5, itc = lb.begin(); lb.end() != itc; ++j, ++itc) {
            ASSERT(*itc == (j < 5) ? j : j - 5);
        }

        for (j = 0, itc = lb.begin(); j < 5; ++j, ++itc) { }

        la.splice(it, lb, itc, itb);
        ASSERT(20 == la.size());
        ASSERT(10 == lb.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        for (j = 0, itb = la.begin(); la.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        lb.remove(0);
        ASSERT(9 == lb.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            LOOP3_ASSERT(*itb, j, (j < 0 ? j : j + 1),
                         *itb == (j < 0 ? j : j + 1));
        }

        for (j = 0, itc = lb.begin(); j < 5; ++j, ++itc) { }
        ASSERT(*itc == 1);

        lb.insert(itc, 0);
        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        lb.reverse();
        lb.sort();

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        lb.reverse();
        lb.sort(lessThan);

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        bsl::list<int> lc(la, &ta);

        lb.merge(la);
        ASSERT(30 == lb.size());
        ASSERT( 0 == la.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            LOOP2_ASSERT(*itb, j,
                         *itb == (j < 0 ? j : (j < 10 ? j / 2 : j - 5)));
        }

        lb.unique(equals);
        ASSERT(25 == lb.size());
        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(20 == j);

        lb.remove_if(positive);
        ASSERT(6 == lb.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(1 == j);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors work as documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.
        // --------------------------------------------------------------------

        bsl::list<int> la(&ta);

        ASSERT(la.empty());

        for (int i = 0; i < 20; ++i) {
            ASSERT((int) la.size() == i);
            la.push_back(i);
            ASSERT( ! la.empty());
        }

        bsl::list<int> lb(la.begin(), la.end(), &ta);

        ASSERT(0 == lb.front());

        bsl::list<int>::iterator it;

        int j;
        for (it = lb.begin(), j = 0; lb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        bsl::list<int>::reverse_iterator rit;
        for (rit = lb.rbegin(), j = 19; lb.rend() != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        for (j = 0, it = la.begin(), rit = la.rbegin(); j < 10;
                                                            ++j, ++it, ++rit) {
            bsl::swap(*it, *rit);
        }

        la.swap(lb);

        for (it = lb.begin(), j = 19; lb.end() != it; ++it, --j) {
            ASSERT(*it == j);
        }
        ASSERT(-1 == j);

        for (rit = lb.rbegin(), j = 0; lb.rend() != rit; ++rit, ++j) {
            ASSERT(*rit == j);
        }
        ASSERT(20 == j);

        bsl::list<int>::iterator itb;
        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }

        lb.insert(itb, la.begin(), la.end());
        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }

        bsl::list<int>::iterator itc;
        for (j = 0, itc = itb; j < 20; ++j, ++itc) { }

        LOOP_ASSERT(lb.size(), 40 == lb.size());

        for (it = lb.begin(), j = 19; lb.end() != it; --j) {
            LOOP2_ASSERT(*it, j, *it == j);
            ++it;
            if (it == itb) {
                it = itc;
            }
        }
        ASSERT(-1 == j);


        for (it = itb, j = 0; itc != it; ++it, ++j) {
            LOOP2_ASSERT(*it, j, *it == j);
        }
        ASSERT(20 == j);

        bsl::list<int>::reverse_iterator ritb;
        for (j = 0, ritb = lb.rbegin(); j < 10; ++j, ++ritb) { }
        bsl::list<int>::reverse_iterator ritc;
        for (j = 0, ritc = ritb; j < 20; ++j, ++ritc) { }

        for (rit = lb.rbegin(), j = 0; lb.rend() != rit; ++j) {
            ASSERT(*rit == j);
            ++rit;
            if (rit == ritb) {
                rit = ritc;
            }
        }
        ASSERT(20 == j);

        for (rit = ritb, j = 19; ritc != rit; ++rit, --j) {
            ASSERT(*rit == j);
        }
        ASSERT(-1 == j);

        lb.erase(itc, lb.end());
        lb.erase(lb.begin(), itb);

        ASSERT(lb == la);
        ASSERT(!(lb != la));
        ASSERT(lb >= la);
        ASSERT(lb <= la);
        ASSERT(!(lb < la));
        ASSERT(!(lb > la));

        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }
        -- *itb;

        ASSERT(lb != la);
        ASSERT(!(lb == la));
        ASSERT(lb <  la);
        ASSERT(!(la <  lb));
        ASSERT(lb <= la);
        ASSERT(!(la <= lb));
        ASSERT(la >= lb);
        ASSERT(!(lb >= la));
        ASSERT(la >  lb);
        ASSERT(!(lb >  la));

        ++ *itb;

        for (int i = -1; i >= -20; --i) {
            lb.push_front(i);
        }

        for (int i = 0, j = -20, k = 19; i < 10; ++i, ++j, --k) {
            ASSERT(lb.back() == k);
            ASSERT(lb.front() == j);
            lb.pop_back();
            lb.pop_front();
        }

        for (it = lb.begin(), j = -10; lb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(10 == j);

        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }
        ASSERT(*itb == 0);
        lb.insert(itb, 0);
        lb.insert(itb, 4 /* items */, 0);

        for (it = lb.begin(), j = -10; j < 0; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(0 == j);
        ASSERT(*it == 0);
        for (j = 0; j < 5; ++it, ++j) {
            ASSERT(0 == *it);
        }
        for (j = 0; j < 10; ++it, ++j) {
            ASSERT(lb.end() != it);
            ASSERT(*it == j);
        }
        ASSERT(10 == j);
        ASSERT(lb.end() == it);

        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }
        lb.erase(lb.begin());

        ASSERT(-9 == *lb.begin());

        lb.erase(lb.begin(), itb);
        lb.resize(5);

        for (j = 0, it = lb.begin(); lb.end() != it; ++j, ++it) {
            ASSERT(0 == *it);
        }

        lb.clear();
        ASSERT(0 == lb.size());
        ASSERT(lb.begin() == lb.end());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MEMORY CONSUMPTION AND C'TOR TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that this is true for all c'tors.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container.  Try
        //   this with all c'tors.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nMEMORY CONSUMPTION AND C'TOR TEST\n"
                               "=================================\n";

        {
            bsl::list<Cargo> ll;
            (void) ll.begin();
        }

        {
            bsl::list<Cargo> ll(&ta);

            int memUsed = ta.numBytesInUse();

            Cargo c;

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                ll.push_back(c);
            }

            ASSERT(ta.numBytesInUse() >= memUsed + 10 * Cargo::BALLAST_SIZE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            bsl::list<Cargo> ll((size_t) 10);
            ASSERT(10 == ll.size());
        }

#if 0
        // no such c'tor
        {
            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::list<Cargo> ll((size_t) 10, &ta);
            ASSERT(10 == ll.size());

            ASSERT(ta.numBytesInUse() >= memUsed + 10 * Cargo::BALLAST_SIZE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }
#endif

        {
            Cargo cx;

            bsl::list<Cargo> ll((size_t) 10, cx);
            ASSERT(10 == ll.size());
        }

        {
            Cargo cx;

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::list<Cargo> ll((size_t) 10, cx, &ta);
            ASSERT(10 == ll.size());

            ASSERT(ta.numBytesInUse() >= memUsed + 10 * Cargo::BALLAST_SIZE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);
        }

        {
            Cargo cs[10];

            int memUsed = ta.numBytesInUse();
            int defaultMemUsed = tda.numBytesInUse();

            bsl::list<Cargo> l2(cs, cs + 10, &ta);
            ASSERT(10 == l2.size());

            ASSERT(ta.numBytesInUse() >= memUsed + 10 * Cargo::BALLAST_SIZE);
            ASSERT(tda.numBytesInUse() == defaultMemUsed);

            memUsed = ta.numBytesInUse();

            bsl::list<Cargo> ld2(l2);
//          ASSERT(l2 == ld2);
            ASSERT(10 == ld2.size());

            ASSERT(ta.numBytesInUse() == memUsed);
            ASSERT(tda.numBytesInUse() >=
                                    defaultMemUsed + 10 * Cargo::BALLAST_SIZE);

            memUsed = ta.numBytesInUse();
            defaultMemUsed = tda.numBytesInUse();

            bsl::list<Cargo> lda2(ld2, &ta);
//          ASSERT(lda2 == ld2);
            ASSERT(10 == lda2.size());

            ASSERT(ta.numBytesInUse() >= memUsed + 10 * Cargo::BALLAST_SIZE);
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

        ASSERT(true  == sameType(bsl::list<int>(), std::list<int>()));
        ASSERT(false == sameType(bsl::list<int>(), native_std::list<int>()));
        ASSERT(true  == usesBslmaAllocator(bsl::list<int>()));
        ASSERT(true  == usesBslmaAllocator(std::list<int>()));
        ASSERT(false == usesBslmaAllocator(native_std::list<int>()));

        ASSERT(true  == sameType(bsl::list<int>::iterator(),
                                 std::list<int>::iterator()));
        ASSERT(false == sameType(bsl::list<int>::iterator(),
                                 native_std::list<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::list<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::list<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(native_std::list<int>::iterator()));
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
