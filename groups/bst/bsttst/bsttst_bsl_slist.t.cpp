// bsttst_slist.t.cpp                                                 -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsttst_bsl_slist.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <slist>
#include <algorithm>

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
// [ 3] Advanced Manipulator / Accessor / Iterator Test
// [ 3] Basic Manipulator / Accessor / Iterator Test
// [ 2] Memory Consumption Test (broken)
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
#define PV(X) { if (verbose) cout << #X " = " << (X) << endl; }
#define QV(X) { if (verbose) cout << "<| " #X " |>" << endl; }
#define PV_(X) { if (verbose) cout << #X " = " << (X) << ", " << flush; }
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

static
bool positive(int i) {
    return i > 0;
}

static
bool lessThan(const int& i, const int& j) {
    return i < j;
}

static
bool greaterThan(const int& i, const int& j) {
    return i > j;
}

static
bool equals(const int& i, const int& j) {
    return i == j;
}

template <typename TYPE>
bool myLess(const TYPE& i, const TYPE& j) {
    return i < j;
}

template <typename TYPE>
void bubbleSort(bsl::slist<TYPE> *sl, bool (*less)(const TYPE &lhs, const TYPE &rhs) = 0)
{
    typename bsl::slist<TYPE>::iterator ita, itb;

    if (!less) less = &myLess<TYPE>;

    for (ita = sl->begin(); sl->end() != ita; ++ita) {
        for (itb = ita, ++itb; sl->end() != itb; ++itb) {
            if ((*less)(*itb, *ita)) {
                bsl::swap(*ita, *itb);
            }
        }
    }
}

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

struct Cargo {
    // This struct dynamically allocates memory, for verifying that the
    // memory allocator is properly passed to elements within a container.

    void            *d_p;
    bslma::Allocator *d_alloc;

    BSLALG_DECLARE_NESTED_TRAITS(Cargo, bslalg::TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    Cargo(bslma::Allocator *a = 0) {
        PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
    }
    Cargo(const Cargo& in, bslma::Allocator* a = 0) {
        PV(a);
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
      case 4: {
        // --------------------------------------------------------------------
        // ADVANCED MANIPULATOR / ACCESSOR / ITERATOR TEST
        //
        // Concerns:
        //   That the advanced manipulators and accessors work as documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.  Repeat the
        //   experiments with slists constructed with non-default allocators.
        // --------------------------------------------------------------------

        bsl::slist<int> la;
        for (int i = 19; i >= 0; --i) {
            la.push_front(i);
        }

        bsl::slist<int> lb;

        int j;
        bsl::slist<int>::iterator it;

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

        for (it = lb.begin(), j = 19; lb.end() != it; ++it, --j) {
            ASSERT(*it == j);
        }

        lb.reverse();

        la = lb;

        lb.resize(5);
        it = lb.begin();
        for (int i = -1; i >= -5; --i) {
            lb.push_front(i);
        }

        bsl::slist<int>::iterator itb, itc;
        for (itc = la.begin(), j = 0; j < 19; ++itc, ++j) { }

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
        for (it = lb.begin(), j = -5; j < 0; ++it, ++j) { }
        ASSERT(19 == *it);
        la.splice(la.end(), lb, it);
        ASSERT(10 == lb.size());
        ASSERT(20 == la.size());

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);
        for (j = 0, itb = la.begin(); la.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(20 == j);

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
        ASSERT(5 == j);

        lb.reverse();

        for (j = 4, itb = lb.begin(); lb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

        {
            bsl::slist<int> ms;

            ms.push_front(1);
            ms.push_front(4);
            ms.push_front(0);
            ms.push_front(3);
            ms.push_front(2);

            ms.sort();

            for (j = 0, itb = ms.begin(); ms.end() != itb; ++j, ++itb) {
                ASSERT(*itb == j);
            }
            ASSERT(5 == j);
        }

        lb.sort();

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        lb.reverse();

        for (j = 4, itb = lb.begin(); lb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

        lb.sort(lessThan);

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        lb.sort(greaterThan);   // sort backwards

        for (j = 4, itb = lb.begin(); lb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

        lb.sort();

        for (j = -5, itb = lb.begin(); lb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

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

        la.clear();
        lb.clear();

        for (j = 19; j >= 15; --j) {
            la.push_front(j);
        }
        for (j = 9; j >= 0; --j) {
            la.push_front(j);
        }
        for (j = 15; j >= 9; --j) {
            lb.push_front(j);
        }

        for (it = la.begin(); 9 != *it; ++it) { }
        itb = lb.begin();

        for (j = 0; j < 5; ++j) {
            la.splice_after(it, itb);
            ++it;
        }

        for (j = 0, it = la.begin(); la.end() != it; ++j, ++it) {
            LOOP2_ASSERT(*it, j, *it == j);
        }

        la.clear();
        lb.clear();

        for (j = 19; j >= 15; --j) {
            la.push_front(j);
        }
        for (j = 9; j >= 0; --j) {
            la.push_front(j);
        }
        for (j = 14; j >= 9; --j) {
            lb.push_front(j);
        }

        for (it = la.begin(); 9 != *it; ++it) { }
        itb = lb.begin();
        for (itc = itb, j = 9; j < 14; ++itc, ++j) { }
        ASSERT(lb.end() != itc);
        la.splice_after(it, itb, itc);

        for (j = 0, it = la.begin(); la.end() != it; ++j, ++it) {
            LOOP2_ASSERT(*it, j, *it == j);
        }

        // now do it with a non-default allocator

        bsl::slist<int> ma(&ta);
        for (int i = 19; i >= 0; --i) {
            ma.push_front(i);
        }

        bsl::slist<int> mb(&ta);

        mb.resize(10, 100);
        ASSERT(10 == mb.size());
        for (j = 0, it = mb.begin(); mb.end() != it; ++j, ++it) {
            ASSERT(100 == *it);
        }
        ASSERT(10 == j);

        mb.unique();
        ASSERT(1 == mb.size());
        mb.resize(10, 200);

        for (j = 0, it = mb.begin(); mb.end() != it; ++j, ++it) {
            if (j) {
                ASSERT(200 == *it);
            }
            else {
                ASSERT(100 == *it);
            }
        }
        ASSERT(10 == j);
        mb.unique();

        it = mb.begin();  ++it;
        mb.splice(it, ma);
        ASSERT(22 == mb.size());
        ASSERT(0  == ma.size());

        for (it = mb.begin(), j = -1; mb.end() != it; ++it, ++j) {
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

        mb.erase(mb.begin());
        ASSERT(21 == mb.size());
        mb.resize(20);
        ASSERT(20 == mb.size());

        for (it = mb.begin(), j = 0; mb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        mb.reverse();

        for (it = mb.begin(), j = 19; mb.end() != it; ++it, --j) {
            ASSERT(*it == j);
        }

        mb.reverse();

        ma = mb;

        mb.resize(5);
        it = mb.begin();
        for (int i = -1; i >= -5; --i) {
            mb.push_front(i);
        }

        for (itc = ma.begin(), j = 0; j < 19; ++itc, ++j) { }

        mb.splice(it, ma, itc);
        ASSERT(11 == mb.size());
        ASSERT(19 == ma.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
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
        for (it = mb.begin(), j = -5; j < 0; ++it, ++j) { }
        ASSERT(19 == *it);
        ma.splice(ma.end(), mb, it);
        ASSERT(10 == mb.size());
        ASSERT(20 == ma.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);
        for (j = 0, itb = ma.begin(); ma.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(20 == j);

        for (j = 0, it  = ma.begin(); j < 5; ++j, ++it ) { }
        for (j = 0, itb = mb.begin(); j < 5; ++j, ++itb) { }

        mb.splice(itb, ma, ma.begin(), it);
        ASSERT(15 == mb.size());
        ASSERT(15 == ma.size());

        for (j = -5, itc = mb.begin(); mb.end() != itc; ++j, ++itc) {
            ASSERT(*itc == (j < 5) ? j : j - 5);
        }

        for (j = 0, itc = mb.begin(); j < 5; ++j, ++itc) { }

        ma.splice(it, mb, itc, itb);
        ASSERT(20 == ma.size());
        ASSERT(10 == mb.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        for (j = 0, itb = ma.begin(); ma.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        mb.remove(0);
        ASSERT(9 == mb.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            LOOP3_ASSERT(*itb, j, (j < 0 ? j : j + 1),
                         *itb == (j < 0 ? j : j + 1));
        }

        for (j = 0, itc = mb.begin(); j < 5; ++j, ++itc) { }
        ASSERT(*itc == 1);

        mb.insert(itc, 0);
        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        mb.reverse();

        for (j = 4, itb = mb.begin(); mb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

#if 0
        // segfaults in sort
        {
            bsl::slist<int> ms(&ta);

            ms.push_front(1);
            ms.push_front(4);
            ms.push_front(0);
            ms.push_front(3);
            ms.push_front(2);

            ms.sort();

            for (j = 0, itb = ms.begin(); ms.end() != itb; ++j, ++itb) {
                ASSERT(*itb == j);
            }
            ASSERT(5 == j);
        }
#else
        Q(There is a bug in slist::sort if the list uses an allocator);
        Q(other than the default allocator);
#endif

        bubbleSort(&mb);

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        mb.reverse();

        for (j = 4, itb = mb.begin(); mb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

        bubbleSort(&mb, &lessThan);

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        bubbleSort(&mb, &greaterThan);    // sort backwards

        for (j = 4, itb = mb.begin(); mb.end() != itb; --j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(-6 == j);

        bubbleSort(&mb);

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(5 == j);

        mb.merge(ma);
        ASSERT(30 == mb.size());
        ASSERT( 0 == ma.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            LOOP2_ASSERT(*itb, j,
                         *itb == (j < 0 ? j : (j < 10 ? j / 2 : j - 5)));
        }

        mb.unique(equals);
        ASSERT(25 == mb.size());
        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(20 == j);

        mb.remove_if(positive);
        ASSERT(6 == mb.size());

        for (j = -5, itb = mb.begin(); mb.end() != itb; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(1 == j);

        ma.clear();
        mb.clear();

        for (j = 19; j >= 15; --j) {
            ma.push_front(j);
        }
        for (j = 9; j >= 0; --j) {
            ma.push_front(j);
        }
        for (j = 15; j >= 9; --j) {
            mb.push_front(j);
        }

        for (it = ma.begin(); 9 != *it; ++it) { }
        itb = mb.begin();

        for (j = 0; j < 5; ++j) {
            ma.splice_after(it, itb);
            ++it;
        }

        for (j = 0, it = ma.begin(); ma.end() != it; ++j, ++it) {
            LOOP2_ASSERT(*it, j, *it == j);
        }

        ma.clear();
        mb.clear();

        for (j = 19; j >= 15; --j) {
            ma.push_front(j);
        }
        for (j = 9; j >= 0; --j) {
            ma.push_front(j);
        }
        for (j = 14; j >= 9; --j) {
            mb.push_front(j);
        }

        for (it = ma.begin(); 9 != *it; ++it) { }
        itb = mb.begin();
        for (itc = itb, j = 9; j < 14; ++itc, ++j) { }
        ASSERT(mb.end() != itc);
        ma.splice_after(it, itb, itc);

        for (j = 0, it = ma.begin(); ma.end() != it; ++j, ++it) {
            LOOP2_ASSERT(*it, j, *it == j);
        }
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

        bsl::slist<int> la(&ta);

        ASSERT(la.empty());

        for (int i = 19; i >= 0; --i) {
            la.push_front(i);
            ASSERT( ! la.empty());
        }

        bsl::slist<int> lb(la.begin(), la.end(), &ta);

        ASSERT(0 == lb.front());

        bsl::slist<int>::iterator it;

        int j;
        for (it = lb.begin(), j = 0; lb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

        bsl::slist<int>::iterator itb;
        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) { }

        lb.insert(itb, la.begin(), la.end());
        for (j = 0, itb = lb.begin(); j < 10; ++j, ++itb) {
            ASSERT(*itb == j);
        }

        bsl::slist<int>::iterator itc;
        for (j = 0, itc = itb; j < 20; ++j, ++itc) {
            ASSERT(*itc == j);
        }

        LOOP_ASSERT(lb.size(), 40 == lb.size());

        for (it = lb.begin(), j = 0; lb.end() != it; ++j) {
            LOOP2_ASSERT(*it, j, *it == j);
            ++it;
            if (it == itb) {
                it = itc;
            }
        }
        ASSERT(20 == j);

        for (it = itb, j = 0; itc != it; ++it, ++j) {
            LOOP2_ASSERT(*it, j, *it == j);
        }
        ASSERT(20 == j);

        lb.erase(itc, lb.end());
        lb.erase(lb.begin(), itb);

        for (itb = lb.begin(), j = 0; lb.end() != itb; ++itb, ++j) {
            ASSERT(*itb == j);
        }
        ASSERT(20 == j);

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

        for (int j = -20; j < -10; ++j) {
            ASSERT(lb.front() == j);
            lb.pop_front();
        }

        for (it = lb.begin(), j = -10; lb.end() != it; ++it, ++j) {
            ASSERT(*it == j);
        }
        ASSERT(20 == j);

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
        for (j = 0; j < 20; ++it, ++j) {
            ASSERT(lb.end() != it);
            ASSERT(*it == j);
        }
        ASSERT(20 == j);
        ASSERT(lb.end() == it);

        for (j = -10, itb = lb.begin(); j < 0; ++j, ++itb) {
            ASSERT(*itb == j);
        }
        ASSERT(0 == *itb);
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
        // MEMORY CONSUMPTION TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container.
        // --------------------------------------------------------------------

        {
            bsl::slist<Cargo> sl(&ta);

            ASSERT(ta.numBytesInUse() < 100);
            PV(ta.numBytesInUse());

            Cargo cx;
            PV(usesBslmaAllocator(cx));

            // slist should use bslma allocators properly, but it doesn't

            for (int i = 0; i < 10; ++i) {
                Cargo c;

                sl.push_front(c);
                PV(ta.numBytesInUse());
            }

            PV(ta.numBytesInUse());
#if 0
            LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >= 40 * 1000);
#else
            Q(There is a bug where slist does not pass the allocator);
            Q(to the constructors of objects contained in the slist);
#endif
        }

        ASSERT(0 == ta.numBytesInUse());
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

        bsl::slist<int> sl(&ta);

        // note there is no native slist
        // slist should use bslma allocators properly, but it doesn't

        ASSERT(true  == sameType(bsl::slist<int>(), std::slist<int>()));
//      ASSERT(true  == usesBslmaAllocator(sl));
//      ASSERT(true  == usesBslmaAllocator(bsl::slist<int>()));
//      ASSERT(true  == usesBslmaAllocator(std::slist<int>()));
//      ASSERT(false == usesBslmaAllocator(native_std::slist<int>()));

        ASSERT(true  == sameType(bsl::slist<int>::iterator(),
                                 std::slist<int>::iterator()));
//      ASSERT(false == sameType(bsl::slist<int>::iterator(),
//                        native_std::slist<int>::iterator()));

        ASSERT(false == usesBslmaAllocator(bsl::slist<int>::iterator()));
        ASSERT(false == usesBslmaAllocator(std::slist<int>::iterator()));
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
