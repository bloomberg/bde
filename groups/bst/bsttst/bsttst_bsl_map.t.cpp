// bsttst_map.t.cpp                                                   -*-C++-*-
#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
#include <bsttst_bsl_map.h>

#include <map>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bsls_util.h>

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
// [ 6] Multimap Manipulators / Accessors / Iterators Test
// [ 5] Map Manipulators / Accessors / Iterators Test
// [ 4] Multimap Memory Consumption and C'tor Test
// [ 3] Multimap Breathing Test
// [ 2] Map Memory Consumption and C'tor Test
// [ 1] Map Breathing Test
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

} // unnamed namespace

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

bool verbose             = false;
bool veryVerbose         = false;
bool veryVeryVerbose     = false;
bool veryVeryVeryVerbose = false;

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

// Makes up for the lack of operator[] in multimap -- return the first
// instance if exists, or create one if it doesn't.  Note doesn't work
// on const map, just like operator[].
// I could define this as operator[], but I'd rather make it blatant that it's
// a local subroutine and not a legitimate native operation.

template <typename MAPTYPE>
typename MAPTYPE::mapped_type& mmGet(MAPTYPE *mm, typename MAPTYPE::key_type key)
{
    typedef typename MAPTYPE::iterator ItType;
    ItType it = mm->find(key);
    if (mm->end() == it) {
        typename MAPTYPE::mapped_type d = typename MAPTYPE::mapped_type();
        it = mm->insert(bsl::make_pair(key, d));
        ASSERT(mm->end() != it);
        ASSERT(it->first == key);
    }
    return it->second;
}

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
struct TestType {
    // This class is a simple test class used for testing whether there are
    // possible cases in which a redefined 'operator&' might not work with this
    // container.

#ifdef BDE_USE_ADDRESSOF
  private:
    TestType *operator&();
#endif
  public:
    int d_theInt;  // int member

    // CREATORS
    TestType()
    : d_theInt(0) {}

    TestType(int theInt)
    : d_theInt(theInt) {}

    // MANUPULATORS
    void operator++() { d_theInt++; }

    // ACCESSORS
    int getTheInt() const { return d_theInt; }
};

bool operator==(const TestType& lhs, const TestType& rhs)
{
    return lhs.getTheInt() == rhs.getTheInt();
}

bool operator<(const TestType& lhs, const TestType& rhs)
{
    return lhs.getTheInt() < rhs.getTheInt();
}

bool operator>(const TestType& lhs, const TestType& rhs)
{
    return lhs.getTheInt() > rhs.getTheInt();
}

struct Cargo {
    // This 'struct' dynamically allocates memory, for verifying that the
    // memory allocator is properly passed to elements within a container.

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

struct CargoNoAddressOf {
    // This 'struct' is the same as 'CargoNoAddressOf' except that 'operator&'
    // is private for testing 'bsls_addressof'.

    void            *d_p;
    bslma::Allocator *d_alloc;


    BSLALG_DECLARE_NESTED_TRAITS(CargoNoAddressOf,
                                 bslalg::TypeTraitUsesBslmaAllocator);
      // Declare nested type traits for this class.

    explicit
    CargoNoAddressOf(bslma::Allocator *a = 0) {
        QV_("Default:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
    }
    CargoNoAddressOf(const CargoNoAddressOf& in, bslma::Allocator* a = 0) {
        QV_("Copy:"); PV(a);
        d_alloc = bslma::Default::allocator(a);
        d_p = d_alloc->allocate(4000);
        std::memcpy(d_p, in.d_p, 4000);
    }
    CargoNoAddressOf& operator=(const CargoNoAddressOf& in) {
        QV("Assign:");
        std::memcpy(d_p, in.d_p, 4000);
        return *this;
    }
    ~CargoNoAddressOf() {
        d_alloc->deallocate(d_p);
    }
#ifdef BDE_USE_ADDRESSOF
  private:
    CargoNoAddressOf *operator&();
#endif
};

template<class TYPE>
struct Greaterp {
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
    {
        return lhs > rhs;
    }
};

template<class TYPE>
struct Comparep {
    enum Direction { LESS, EQUAL, GREATER };
    Direction d_direction;
    Comparep(Direction dir) : d_direction(dir) {}
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
    {
        switch (d_direction) {
          case LESS:
            return lhs <  rhs;
          case EQUAL:
            return lhs == rhs;
          case GREATER:
            return lhs >  rhs;
        }
        return false;    // should never happen
    }
};

template <class KEY_TYPE,
          class VALUE_TYPE,
          class ALLOC = bsl::allocator<VALUE_TYPE> >
struct TestDriver {
    // This 'struct' provides the means for easily run the test driver on
    // differnt 'TYPE's.

    // TEST CASES
    static void testCase6();
        // Test 'multimap' MANIPULATORS, ACCESSORS, and iterators.

    static void testCase5();
        // Test 'map' MANIPULATORS, ACCESSORS, and iterators.

    static void testCase4();
        // Test memory allocation for 'multimap'. This function is designed to
        // work with types 'Cargo' and 'CargoNoAddressOf'.

    static void testCase3();
        // Execute breathing test for 'multimap'.

    static void testCase2();
        // Test memory allocation for 'map'. This function is designed to work
        // with types 'Cargo' and 'CargoNoAddressOf'.

    static void testCase1();
        // Breathing test for 'map'.  This test *exercises* basic functionality
        // but *test* nothing.
};

template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE, ALLOC>::testCase1()
{
    // --------------------------------------------------------------------
    // MAP BREATHING TEST
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

    if (verbose) cout << "\nMAP BREATHING TEST\n"
                           "==================\n";

    bslma::TestAllocator ta;

    bsl::map<KEY_TYPE, VALUE_TYPE> b(&ta);
    std::map<KEY_TYPE, VALUE_TYPE> s(&ta);

    // TBD: Cannot test with classes that redefine 'operator&'; using
    // explicitely 'Cargo' for now.

    native_std::map<Cargo, Cargo> n;

    typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator bit;
    typename std::map<KEY_TYPE, VALUE_TYPE>::iterator sit;
    typename native_std::map<KEY_TYPE, VALUE_TYPE>::iterator nit;

    ASSERT(true  == sameType(b, s));
    ASSERT(false == sameType(s, n));
    ASSERT(false == sameType(b, n));

    ASSERT(true  == sameType(bit, sit));
    ASSERT(false == sameType(sit, nit));
    ASSERT(false == sameType(bit, nit));

    ASSERT(true  == usesBslmaAllocator(b));
    ASSERT(true  == usesBslmaAllocator(s));
    ASSERT(false == usesBslmaAllocator(n));

    ASSERT(false == usesBslmaAllocator(bit));
    ASSERT(false == usesBslmaAllocator(sit));
    ASSERT(false == usesBslmaAllocator(nit));

}

template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE,  ALLOC>::testCase2()
{
        if (verbose) cout << "\nMAP STORING AND C'TOR TEST\n"
                               "==========================\n";
        bslma::TestAllocator ta;
        bslma::TestAllocator tda;

        bslma::DefaultAllocatorGuard defaultGuard(&tda);

        bsl::less<KEY_TYPE> lessTYPE;
        Greaterp<KEY_TYPE> greaterTYPE;

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> m;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 3; ++i) {
                m.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == m.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 12 * 1000);
            PV(tda.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> m(lessTYPE);

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 3; ++i) {
                m.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == m.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 12 * 1000);
            PV(tda.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> m(lessTYPE, &ta);

            PV((void *) &ta);

            int memUsed = ta.numBytesInUse();

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            int defaultMemUsed = tda.numBytesInUse();

            for (int i = 0; i < 3; ++i) {
                m.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == m.count(i));
            }

            LOOP_ASSERT(ta.numBytesInUse(),
                        ta.numBytesInUse() >= memUsed + 3 * 4000);
            LOOP_ASSERT(tda.numBytesInUse(),
                        defaultMemUsed == tda.numBytesInUse());
            PV(ta.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> mA;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                mA.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mA.count(i));
            }

            int defaultMemUsed = tda.numBytesInUse();
            bsl::map<KEY_TYPE, VALUE_TYPE> mB(mA.begin(), mA.end());
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mB.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 3 * 4000);
            PV(tda.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> mA;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                mA.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mA.count(i));
            }

            int defaultMemUsed = tda.numBytesInUse();
            bsl::map<KEY_TYPE, VALUE_TYPE> mB(mA.begin(), mA.end());
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mB.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 3 * 4000);
            PV(tda.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> mA;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                mA.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mA.count(i));
            }

            int defaultMemUsed = tda.numBytesInUse();
            bsl::map<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                         mB(mA.begin(), mA.end(), greaterTYPE);
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mB.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 3 * 4000);
            PV(tda.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> mA;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                mA.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mA.count(i));
            }

            int defaultMemUsed = tda.numBytesInUse();
            int memUsed = ta.numBytesInUse();

            bsl::map<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                    mB(mA.begin(), mA.end(), greaterTYPE, &ta);
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mB.count(i));
            }

            LOOP_ASSERT(ta.numBytesInUse(),
                        ta.numBytesInUse() >= memUsed + 3 * 4000);
            LOOP_ASSERT(tda.numBytesInUse(),
                        defaultMemUsed == tda.numBytesInUse());
            PV(ta.numBytesInUse());
        }

        {
            bsl::map<KEY_TYPE, VALUE_TYPE> mA;
            const bsl::map<KEY_TYPE, VALUE_TYPE>& MA = mA;

            VALUE_TYPE cx;
            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                mA.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mA.count(i));
            }

            int defaultMemUsed = tda.numBytesInUse();

            bsl::map<KEY_TYPE, VALUE_TYPE> mB(MA);
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == mB.count(i));
            }

            LOOP_ASSERT(tda.numBytesInUse(),
                        tda.numBytesInUse() >= defaultMemUsed + 3 * 4000);
            PV(tda.numBytesInUse());
        }

        {
            VALUE_TYPE cx;
            int defaultMemUsed = tda.numBytesInUse();

            bsl::map<KEY_TYPE, VALUE_TYPE> m(&ta);

            PV((void *) &ta);

            int memUsed = ta.numBytesInUse();

            PV(usesBslmaAllocator(cx));

            for (int i = 0; i < 3; ++i) {
                m.insert(bsl::make_pair(i, cx));
            }
            for (int i = 0; i < 10; ++i) {
                ASSERT((i < 3) == m.count(i));
            }

            LOOP_ASSERT(ta.numBytesInUse(),
                        ta.numBytesInUse() >= memUsed + 3 * 4000);
            LOOP_ASSERT(tda.numBytesInUse(),
                        defaultMemUsed == tda.numBytesInUse());
            PV(ta.numBytesInUse());
        }

        ASSERT(0 == ta.numBytesInUse());
}

template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE,  ALLOC>::testCase3()
{
    if (verbose) cout << "\nMULTIMAP BREATHING TEST\n"
                           "=======================\n";
    bslma::TestAllocator ta;

    bsl::multimap<KEY_TYPE, VALUE_TYPE> b(&ta);
    std::multimap<KEY_TYPE, VALUE_TYPE> s(&ta);

    // TBD: Cannot test with classes that redefine 'operator&'; using
    // explicitely 'int' and 'Cargo' for now.

    native_std::multimap<int, Cargo> n;

    typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator bit;
    typename std::multimap<KEY_TYPE, VALUE_TYPE>::iterator sit;
    typename native_std::multimap<KEY_TYPE, VALUE_TYPE>::iterator nit;

    ASSERT(true  == sameType(b, s));
    ASSERT(false == sameType(s, n));
    ASSERT(false == sameType(b, n));

    ASSERT(true  == sameType(bit, sit));
    ASSERT(false == sameType(sit, nit));
    ASSERT(false == sameType(bit, nit));

    ASSERT(true  == usesBslmaAllocator(b));
    ASSERT(true  == usesBslmaAllocator(s));
    ASSERT(false == usesBslmaAllocator(n));

    ASSERT(false == usesBslmaAllocator(bit));
    ASSERT(false == usesBslmaAllocator(sit));
    ASSERT(false == usesBslmaAllocator(nit));
}

template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE,  ALLOC>::testCase4()
{
    if (verbose) cout << "\nMULTIMAP STORING AND C'TOR TEST\n" << endl;

    bslma::TestAllocator ta;
    bslma::TestAllocator tda;

    bslma::DefaultAllocatorGuard defaultGuard(&tda);

    bsl::less<KEY_TYPE> lessTYPE;
    Greaterp<KEY_TYPE> greaterTYPE;

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> m;

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        int defaultMemUsed = tda.numBytesInUse();

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i / 2, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 2 ? 2 : 0));
        }

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() >= defaultMemUsed + 4 * 4000);
        PV(tda.numBytesInUse());
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> m(lessTYPE);

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        int defaultMemUsed = tda.numBytesInUse();

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i / 2, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 2 ? 2 : 0));
        }

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() >= defaultMemUsed + 4 * 4000);
        PV(tda.numBytesInUse());
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> m(lessTYPE, &ta);

        PV((void *) &ta);

        ASSERT(ta.numBytesInUse() < 100);
        PV(ta.numBytesInUse());

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i / 2, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 2 ? 2 : 0));
        }

        LOOP_ASSERT(ta.numBytesInUse(),
                    ta.numBytesInUse() >= memUsed + 16 * 1000);
        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() == defaultMemUsed);
        PV(ta.numBytesInUse());
    }

    {
        VALUE_TYPE cx;
        bsl::map<KEY_TYPE, VALUE_TYPE> m;

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 4 ? 1 : 0));
        }

        ASSERT(ta.numBytesInUse() < 100);
        PV(ta.numBytesInUse());

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        bsl::multimap<KEY_TYPE, VALUE_TYPE> mm(m.begin(), m.end());
        for (int i = 0; i < 10; ++i) {
            ASSERT(mm.count(i) == (i < 4 ? 1 : 0));
        }

        LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                     ta.numBytesInUse() == memUsed);

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() >= defaultMemUsed + 16 * 1000);
        PV(ta.numBytesInUse());
    }

    {
        VALUE_TYPE cx;
        bsl::map<KEY_TYPE, VALUE_TYPE> m;

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 4 ? 1 : 0));
        }

        ASSERT(ta.numBytesInUse() < 100);
        PV(ta.numBytesInUse());

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        bsl::multimap<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                        mm(m.begin(), m.end(), greaterTYPE);
        for (int i = 0; i < 10; ++i) {
            ASSERT(mm.count(i) == (i < 4 ? 1 : 0));
        }

        LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                     ta.numBytesInUse() == memUsed);

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() >= defaultMemUsed + 16 * 1000);
        PV(ta.numBytesInUse());
    }

    {
        VALUE_TYPE cx;
        bsl::map<KEY_TYPE, VALUE_TYPE> m;

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(m.count(i) == (i < 4 ? 1 : 0));
        }

        ASSERT(ta.numBytesInUse() < 100);
        PV(ta.numBytesInUse());

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        bsl::multimap<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                  mm(m.begin(), m.end(), greaterTYPE, &ta);
        for (int i = 0; i < 10; ++i) {
            ASSERT(mm.count(i) == (i < 4 ? 1 : 0));
        }

        LOOP2_ASSERT(ta.numBytesInUse(), memUsed,
                     ta.numBytesInUse() >= memUsed + 16 * 1000);

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() == defaultMemUsed);
        PV(ta.numBytesInUse());
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mmA;
        const bsl::multimap<KEY_TYPE, VALUE_TYPE>& MMA = mmA;

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        int defaultMemUsed = tda.numBytesInUse();

        for (int i = 0; i < 4; ++i) {
            mmA.insert(bsl::make_pair(i / 2, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(mmA.count(i) == (i < 2 ? 2 : 0));
        }

        bsl::multimap<KEY_TYPE, VALUE_TYPE> mmB(MMA);
        for (int i = 0; i < 10; ++i) {
            ASSERT(mmB.count(i) == (i < 2 ? 2 : 0));
        }

        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() >= defaultMemUsed + 2 * 16 * 1000);
        PV(tda.numBytesInUse());
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> m(&ta);

        PV((void *) &ta);

        ASSERT(ta.numBytesInUse() < 100);
        PV(ta.numBytesInUse());

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        int defaultMemUsed = tda.numBytesInUse();

        for (int i = 0; i < 4; ++i) {
            m.insert(bsl::make_pair(i / 2, cx));
        }

        LOOP_ASSERT(ta.numBytesInUse(), ta.numBytesInUse() >= 16 * 1000);
        LOOP_ASSERT(tda.numBytesInUse(),
                    defaultMemUsed == tda.numBytesInUse());
        PV(ta.numBytesInUse());
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mmA;
        const bsl::multimap<KEY_TYPE, VALUE_TYPE>& MMA = mmA;

        VALUE_TYPE cx;
        PV(usesBslmaAllocator(cx));

        for (int i = 0; i < 4; ++i) {
            mmA.insert(bsl::make_pair(i / 2, cx));
        }
        for (int i = 0; i < 10; ++i) {
            ASSERT(mmA.count(i) == (i < 2 ? 2 : 0));
        }

        int memUsed = ta.numBytesInUse();
        int defaultMemUsed = tda.numBytesInUse();

        bsl::multimap<KEY_TYPE, VALUE_TYPE> mmB(MMA, &ta);
        for (int i = 0; i < 10; ++i) {
            ASSERT(mmB.count(i) == (i < 2 ? 2 : 0));
        }

        LOOP_ASSERT(ta.numBytesInUse(),
                    ta.numBytesInUse() >= memUsed + 16 * 1000);
        LOOP_ASSERT(tda.numBytesInUse(),
                    tda.numBytesInUse() == defaultMemUsed);
        PV(tda.numBytesInUse());
    }

    ASSERT(0 == ta.numBytesInUse());
}
template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE,  ALLOC>::testCase5()
{
    if (verbose) cout <<
                    "\nMAP BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
                      "================================================\n"
                      << endl;

    bslma::TestAllocator ta;

    bsl::less<KEY_TYPE> lessTYPE;
    Greaterp<KEY_TYPE> greaterTYPE;

    bsl::map<KEY_TYPE, VALUE_TYPE> ma(&ta), mb(&ta);
    int j;

    ma[0] = 0;
    ma[1] = 1;
    ma[2] = 2;
    ma[3] = 3;

    ASSERT(4 == ma.size());

    {
        bsl::map<KEY_TYPE, VALUE_TYPE> mTmp;
        ASSERT(mTmp.empty());
        mTmp = ma;
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        Comparep<KEY_TYPE>  comparator(Comparep<KEY_TYPE> ::LESS);
        bsl::multimap<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> >
                                                          mTmp(comparator);
        ASSERT(mTmp.empty());
        mTmp.insert(ma.begin(), ma.end());
        ASSERT(!mTmp.empty());
        j = 0;
        for (typename bsl::map<KEY_TYPE, VALUE_TYPE,
                               Comparep<KEY_TYPE> >::iterator it =
                                                              mTmp.begin();
             mTmp.end() != it; ++it, ++j) {
            LOOP_ASSERT(j, it->first  == j);
            LOOP_ASSERT(j, it->second == j);
        }
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE> mTmp(ma);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE> mTmp(ma, &ta);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE> mTmp(ma.begin(), ma.end());
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> >
                      mTmp(ma.begin(),
                           ma.end(),
                           Comparep<KEY_TYPE>(Comparep<KEY_TYPE>::LESS));
        ASSERT(!mTmp.empty());
        j = 0;
        for (typename bsl::map<KEY_TYPE, VALUE_TYPE,
                               Comparep<KEY_TYPE> >::iterator it =
                                                              mTmp.begin();
             mTmp.end() != it; ++it, ++j) {
            LOOP_ASSERT(j, it->first  == j);
            LOOP_ASSERT(j, it->second == j);
        }
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE> mTmp(ma.begin(),
                                            ma.end(),
                                            lessTYPE,
                                            &ta);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        typedef bsl::map<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                                                   BackMap;
        BackMap mBackwards(&ta);

        for (j = 0; j < 4; ++j) {
            mBackwards[j] = j;
        }

        ASSERT(mBackwards.size() == 4);

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mBackwards[j] == j);
        }

        typename BackMap::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT( mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    {
        typedef bsl::map<KEY_TYPE, VALUE_TYPE, Greaterp<KEY_TYPE> >
                                                                   BackMap;
        BackMap mBackwards(ma.begin(), ma.end(), greaterTYPE, &ta);

        ASSERT(mBackwards.size() == 4);
        ASSERT(mBackwards.size() < mBackwards.max_size());

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mBackwards[j] == j);
        }

        typename BackMap::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT(mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    {
        bsl::map<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> > mBackwards(
                         ma.begin(),
                         ma.end(),
                         Comparep<KEY_TYPE>(Comparep<KEY_TYPE>::GREATER), &ta);

        ASSERT(mBackwards.size() == 4);
        ASSERT(mBackwards.size() < mBackwards.max_size());

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mBackwards[j] == j);
        }

        typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT(mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    ASSERT(4 == ma.size());
    ASSERT(0 == mb.size());

    ma.swap(mb);

    ASSERT(0 == ma.size());
    ASSERT(4 == mb.size());

    bsl::swap(ma, mb);

    ASSERT(4 == ma.size());
    ASSERT(0 == mb.size());

    bsl::pair<const KEY_TYPE, VALUE_TYPE> prA(4, 4);

    bsl::pair<typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator, bool>
                                                    pResA = ma.insert(prA);
    ASSERT(pResA.second);
    ASSERT(pResA.first->first  == 4);
    ASSERT(pResA.first->second == 4);

    ASSERT(5 == ma.size());
    ASSERT(4 == ma[4]);

    bsl::pair<typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator, bool>
                                                    pResB = ma.insert(prA);

    pResB = ma.insert(prA);
    ASSERT(!pResB.second);
    ASSERT(pResA.first == pResB.first);

    ASSERT(5 == ma.size());
    ASSERT(4 == ma[4]);

    ASSERT(1 == ma.count(2));

    ma.erase(2);

    ASSERT(4 == ma.size());
    ASSERT(0 == ma.count(2));
    ASSERT(ma.end() == ma.find(2));

    ma.erase(ma.find(1), ma.find(4));

    ASSERT(2 == ma.size());

    const bsl::map<KEY_TYPE, VALUE_TYPE> mc(ma, &ta);
    ASSERT(mc == ma);

    {
        int presentAux[] = { 0, 1, 0, 0, 0, 1, 0 };
        int *present = presentAux + 1;

        for (int i = -1; i < 6; ++i) {
            if (present[i]) {
                ASSERT(1 == ma.count(i));
                ASSERT(ma.end() != ma.find(i));
            }
            else {
                ASSERT(!ma.count(i));
                ASSERT(ma.end() == ma.find(i));
            }
        }

        // check this also works for a const map
        for (int i = -1; i < 6; ++i) {
            if (present[i]) {
                ASSERT(1 == mc.count(i));
                ASSERT(mc.end() != mc.find(i));
            }
            else {
                ASSERT(!mc.count(i));
                ASSERT(mc.end() == mc.find(i));
            }
        }
    }

    typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator it;
    typename bsl::map<KEY_TYPE, VALUE_TYPE>::const_iterator itc;

    it = ma.lower_bound(2);
    ASSERT(4 == it->second);

    itc = mc.lower_bound(2);
    ASSERT(4 == itc->second);

    it = ma.lower_bound(0);
    ASSERT(0 == it->second);

    itc = mc.lower_bound(0);
    ASSERT(0 == itc->second);

    it = ma.upper_bound(2);
    ASSERT(4 == it->second);

    itc = mc.upper_bound(2);
    ASSERT(4 == itc->second);

    it = ma.upper_bound(0);
    ASSERT(4 == it->second);

    itc = mc.upper_bound(0);
    ASSERT(4 == itc->second);

    bsl::pair<typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator,
              typename bsl::map<KEY_TYPE, VALUE_TYPE>::iterator> prIt;
    bsl::pair<
            typename bsl::map<KEY_TYPE, VALUE_TYPE>::const_iterator,
            typename bsl::map<KEY_TYPE, VALUE_TYPE>::const_iterator> prCIt;

    prIt = ma.equal_range(0);
    ASSERT(prIt.first->first  == 0);
    ASSERT(prIt.second->first == 4);

    prCIt = mc.equal_range(0);
    ASSERT(prCIt.first->first  == 0);
    ASSERT(prCIt.second->first == 4);

    prIt = ma.equal_range(2);
    ASSERT(ma.find(4) == prIt.first);
    ASSERT(ma.find(4) == prIt.second);

    prCIt = mc.equal_range(2);
    ASSERT(mc.find(4) == prCIt.first);
    ASSERT(mc.find(4) == prCIt.second);

    ma.clear();
    mb.clear();

    ASSERT(ma.empty());

    ma[0] = 0;
    ma[1] = 1;

    mb = ma;

    ASSERT(ma == mb);
    ASSERT(!(ma != mb));
    ASSERT(ma >= mb);
    ASSERT(!(ma > mb));
    ASSERT(ma <= mb);
    ASSERT(!(ma < mb));

    mb[1] = 0;

    ASSERT(ma != mb);
    ASSERT(!(ma == mb));
    ASSERT(ma >= mb);
    ASSERT(ma >  mb);
    ASSERT(!(ma <= mb));
    ASSERT(!(ma <  mb));
}

template<class KEY_TYPE, class VALUE_TYPE, class ALLOC>
void TestDriver<KEY_TYPE, VALUE_TYPE,  ALLOC>::testCase6()
{
    if (verbose) cout <<
               "\nMULTIMAP BASIC MANIPULATOR / ACCESSOR / ITERATOR TEST\n"
                 "=====================================================\n";

    bslma::TestAllocator ta;

    bsl::less<KEY_TYPE> lessTYPE;
    Greaterp<KEY_TYPE> greaterTYPE;

    bsl::multimap<KEY_TYPE, VALUE_TYPE> ma(&ta), mb(&ta);
    int j;

    mmGet(&ma, 0) = 0;
    mmGet(&ma, 1) = 1;
    mmGet(&ma, 2) = 2;
    mmGet(&ma, 3) = 3;

    ASSERT(4 == ma.size());

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mTmp;
        ASSERT(mTmp.empty());
        mTmp = ma;
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        Comparep<KEY_TYPE> comparator(Comparep<KEY_TYPE>::LESS);
        bsl::multimap<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> >
                                                              mTmp(comparator);
        ASSERT(mTmp.empty());
        mTmp.insert(ma.begin(), ma.end());
        ASSERT(!mTmp.empty());
        j = 0;
        for (typename bsl::multimap<KEY_TYPE,
                                    VALUE_TYPE,
                                    Comparep<KEY_TYPE> >::iterator it =
                                                              mTmp.begin();
                                                              mTmp.end() != it;
                                                              ++it, ++j) {
            LOOP_ASSERT(j, it->first  == j);
            LOOP_ASSERT(j, it->second == j);
        }
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mTmp(ma);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mTmp(ma, &ta);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mTmp(ma.begin(), ma.end());
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> > mTmp(
                                ma.begin(),
                                ma.end(),
                                Comparep<KEY_TYPE> (Comparep<KEY_TYPE>::LESS));
        ASSERT(!mTmp.empty());
        j = 0;
        for (typename bsl::multimap<KEY_TYPE,
                                    VALUE_TYPE,
                                    Comparep<KEY_TYPE> >::iterator it =
                                                              mTmp.begin();
                                                              mTmp.end() != it;
                                                              ++it, ++j) {
            LOOP_ASSERT(j, it->first  == j);
            LOOP_ASSERT(j, it->second == j);
        }
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE> mTmp(ma.begin(),
                                                 ma.end(),
                                                 lessTYPE,
                                                 &ta);
        ASSERT(!mTmp.empty());
        ASSERT(mTmp == ma);
    }

    {
        typedef bsl::multimap<KEY_TYPE,
                              VALUE_TYPE,
                              Greaterp<KEY_TYPE> > BackMap;
        BackMap mBackwards(&ta);

        for (j = 0; j < 4; ++j) {
            mmGet(&mBackwards, j) = j;
        }

        ASSERT(mBackwards.size() == 4);

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mmGet(&mBackwards, j) == j);
        }

        typename BackMap::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT( mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    {
        typedef bsl::multimap<KEY_TYPE,
                              VALUE_TYPE,
                              Greaterp<KEY_TYPE> > BackMap;
        BackMap mBackwards(ma.begin(), ma.end(), greaterTYPE, &ta);

        ASSERT(mBackwards.size() == 4);
        ASSERT(mBackwards.size() < mBackwards.max_size());

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mmGet(&mBackwards, j) == j);
        }

        typename BackMap::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT(mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    {
        bsl::multimap<KEY_TYPE, VALUE_TYPE, Comparep<KEY_TYPE> > mBackwards(
                        ma.begin(),
                        ma.end(),
                        Comparep<KEY_TYPE> (Comparep<KEY_TYPE>::GREATER), &ta);

        ASSERT(mBackwards.size() == 4);
        ASSERT(mBackwards.size() < mBackwards.max_size());

        for (j = 0; j < 4; ++j) {
            LOOP_ASSERT(j, mmGet(&mBackwards, j) == j);
        }

        typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator it;
        for (it = mBackwards.begin(), j = 3;
             mBackwards.end() != it; ++it, --j) {
            LOOP_ASSERT(j, j == it->second);
        }
        ASSERT(-1 == j);

        ASSERT( mBackwards.key_comp()(3, 2));
        ASSERT(!mBackwards.key_comp()(3, 3));
        ASSERT(!mBackwards.key_comp()(2, 3));

        it = mBackwards.end();  --it;
        ASSERT(mBackwards.value_comp()(*mBackwards.begin(), *it));
        ASSERT(!mBackwards.value_comp()(*it, *mBackwards.begin()));
        ASSERT(!mBackwards.value_comp()(*it, *it));
    }

    ASSERT(4 == ma.size());
    ASSERT(0 == mb.size());

    ma.swap(mb);

    ASSERT(0 == ma.size());
    ASSERT(4 == mb.size());

    bsl::swap(ma, mb);

    ASSERT(4 == ma.size());
    ASSERT(0 == mb.size());

    bsl::pair<const KEY_TYPE, VALUE_TYPE> prA(4,4);

    typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator pResA =
                                                                ma.insert(prA);
    ASSERT(ma.end() != pResA);
    ASSERT(pResA->first  == 4);
    ASSERT(pResA->second == 4);

    ASSERT(5 == ma.size());
    ASSERT(4 == mmGet(&ma, 4));

    ma.erase(2);

    ASSERT(4 == ma.size());
    ASSERT(0 == ma.count(2));
    ASSERT(ma.end() == ma.find(2));

    ma.erase(ma.find(1), ma.find(4));

    ASSERT(2 == ma.size());

    const bsl::multimap<KEY_TYPE, VALUE_TYPE> mc(ma, &ta);
    ASSERT(mc == ma);

    {
        int presentAux[] = { 0, 1, 0, 0, 0, 1, 0 };
        int *present = presentAux + 1;

        for (int i = -1; i < 6; ++i) {
            if (present[i]) {
                ASSERT(1 == ma.count(i));
                ASSERT(ma.end() != ma.find(i));
            }
            else {
                ASSERT(!ma.count(i));
                ASSERT(ma.end() == ma.find(i));
            }
        }

        // check this also works for a const map
        for (int i = -1; i < 6; ++i) {
            if (present[i]) {
                ASSERT(1 == mc.count(i));
                ASSERT(mc.end() != mc.find(i));
            }
            else {
                ASSERT(!mc.count(i));
                ASSERT(mc.end() == mc.find(i));
            }
        }
    }

    typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator it;
    typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::const_iterator itc;

    it = ma.lower_bound(2);
    ASSERT(4 == it->second);

    itc = mc.lower_bound(2);
    ASSERT(4 == itc->second);

    it = ma.lower_bound(0);
    ASSERT(0 == it->second);

    itc = mc.lower_bound(0);
    ASSERT(0 == itc->second);

    it = ma.upper_bound(2);
    ASSERT(4 == it->second);

    itc = mc.upper_bound(2);
    ASSERT(4 == itc->second);

    it = ma.upper_bound(0);
    ASSERT(4 == it->second);

    itc = mc.upper_bound(0);
    ASSERT(4 == itc->second);

    bsl::pair<typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator,
              typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::iterator> prIt;
    bsl::pair<
           typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::const_iterator,
           typename bsl::multimap<KEY_TYPE, VALUE_TYPE>::const_iterator> prCIt;

    prIt = ma.equal_range(0);
    ASSERT(prIt.first->first  == 0);
    ASSERT(prIt.second->first == 4);

    prCIt = mc.equal_range(0);
    ASSERT(prCIt.first->first  == 0);
    ASSERT(prCIt.second->first == 4);

    prIt = ma.equal_range(2);
    ASSERT(ma.find(4) == prIt.first);
    ASSERT(ma.find(4) == prIt.second);

    prCIt = mc.equal_range(2);
    ASSERT(mc.find(4) == prCIt.first);
    ASSERT(mc.find(4) == prCIt.second);

    ma.clear();
    mb.clear();

    ASSERT(ma.empty());

    mmGet(&ma, 0) = 0;
    mmGet(&ma, 1) = 1;

    mb = ma;

    ASSERT(ma == mb);
    ASSERT(!(ma != mb));
    ASSERT(ma >= mb);
    ASSERT(!(ma > mb));
    ASSERT(ma <= mb);
    ASSERT(!(ma < mb));

    mmGet(&mb, 1) = 0;

    ASSERT(ma != mb);
    ASSERT(!(ma == mb));
    ASSERT(ma >= mb);
    ASSERT(ma >  mb);
    ASSERT(!(ma <= mb));
    ASSERT(!(ma <  mb));

    ma.clear();  mb.clear();

    for (int i = 0; i < 10; ++i) {
        mmGet(&ma, i) = 2 * i;
    }
    for (int i = 3; i < 7; ++i) {
        it = ma.find(i);
        ma.insert(it, bsl::make_pair(i, 2*i));
        ma.insert(it, std::make_pair(i, 2*i));
    }
    for (j = 0, it = ma.begin(); j < 10; ++j, ++it) {
        ASSERT(ma.end() != it);
        int k = j < 3 || j >= 7 ? 1 : 3;
        for (int m = 0; m < k; ++m) {
            if (m > 0) {
                ++it;
            }
            ASSERT(ma.end() != it);
            LOOP2_ASSERT(j, m, it->first == j);
            LOOP_ASSERT(j, it->second == 2 * j);
        }
    }
    ASSERT(ma.end() == it);
    ma.erase(4);    // erases all instances
    ma.erase(5);    // erases all instances
    for (j = 0, it = ma.begin(); j < 10; ++j, ++it) {
        if (4 == j) {
            j += 2;
        }

        ASSERT(ma.end() != it);
        int k = j < 3 || j >= 7 ? 1 : 3;
        for (int m = 0; m < k; ++m) {
            if (m > 0) {
                ++it;
            }
            ASSERT(ma.end() != it);
            LOOP2_ASSERT(j, m, it->first == j);
            LOOP_ASSERT(j, it->second == 2 * j);
        }
    }
    ASSERT(ma.end() == it);

    const bsl::multimap<KEY_TYPE, VALUE_TYPE> mc2 = ma;

    prIt = ma.equal_range(3);
    ASSERT(prIt.first->first   == 3);
    ASSERT(prIt.first->second  == 2 * 3);
    ASSERT(prIt.second->first  == 6);
    ASSERT(prIt.second->second == 2 * 6);

    for (it = prIt.first, j = 0; j < 3; ++j, ++it) {
        ASSERT(it->first  == 3);
        ASSERT(it->second == 2 * 3);
    }
    ASSERT(it->first  == 6);
    ASSERT(it->second == 2 * 6);

    prCIt = mc2.equal_range(6);
    ASSERT(prCIt.first->first   == 6);
    ASSERT(prCIt.first->second  == 2 * 6);
    ASSERT(prCIt.second->first  == 7);
    ASSERT(prCIt.second->second == 2 * 7);

    for (itc = prCIt.first, j = 0; j < 3; ++j, ++itc) {
        ASSERT(itc->first  == 6);
        ASSERT(itc->second == 2 * 6);
    }
    ASSERT(itc->first  == 7);
    ASSERT(itc->second == 2 * 7);

    ASSERT(mc2 == ma);
    ASSERT(mc2 >= ma);
    ASSERT(mc2 <= ma);
    ASSERT(!(mc2 != ma));
    ASSERT(!(mc2 <  ma));
    ASSERT(!(mc2 >  ma));

    ++mmGet(&ma, 7);
    ASSERT(mc2 != ma);
    ASSERT(mc2 <  ma);
    ASSERT(mc2 <= ma);
    ASSERT(!(mc2 == ma));
    ASSERT(!(mc2 >  ma));
    ASSERT(!(mc2 >= ma));
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
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator ta;
    bslma::TestAllocator tda;

    bslma::DefaultAllocatorGuard defaultGuard(&tda);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // MULTIMAP MANIPULATORS / ACCESSORS / ITERATORS TEST
        //
        // Concerns:
        //   Test the manipulators and accessors of map
        //
        // Plan:
        //   Since a multimap can do almost all the things a map can do, start
        //   out with a barely modified repeat of the map manipulators /
        //   accessor test, then add tests specific to multimap.
        //
        // Testing:
        // --------------------------------------------------------------------

        // TBD: This test can, at this time,  only be instantiated with types
        // that offer implicit conversion from int.

        if (verbose) cout << "Testing with Type 'TestType'" << endl;
        TestDriver<TestType, TestType>::testCase6();

        if (verbose) cout << "Testing with Type 'int' and 'TestType'" << endl;
        TestDriver<int, TestType>::testCase6();

        if (verbose) cout << "Testing with Type 'int'" << endl;
        TestDriver<int, int>::testCase6();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MAP MANIPULATORS / ACCESSORS / ITERATOR TEST
        //
        // Concerns:
        //   That the basic manipulators and accessors work as documented.
        //
        // Plan:
        //   Create a couple of containers and store things in them, verify
        //   with accessors that the contents are as expected.
        // --------------------------------------------------------------------

        // TBD: This test can, at this time,  only be instantiated with types
        // that offer implicit conversion from int.

        if (verbose) cout << "Testing with Type 'TestType'" << endl;
        TestDriver<TestType, TestType>::testCase5();

        if (verbose) cout << "Testing with Type 'int'" << endl;
        TestDriver<int, int>::testCase5();

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // MULTIMAP MEMORY CONSUMPTION AND C'TOR TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that all c'tors in the standard compile.
        //
        // Plan:
        //   Store the 'Cargo' struct, which dynamically allocates large
        //   amounts of memory, and verify that large amounts of memory are
        //   consumed in the allocator that was passed to the container.
        //   Repeat this test for all c'tors in the standard.
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing with Type 'TestType' and 'Cargo'" << endl;
        TestDriver<TestType, Cargo>::testCase4();

        if (verbose) cout << "Testing with Type 'TestType' and"
                          << " 'CargoNoAddressOf'"
                          << endl;
        TestDriver<TestType, CargoNoAddressOf>::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MULTIMAP BREATHING TEST
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
        if (verbose) cout << "Testing with Type 'TestType'" << endl;
        TestDriver<TestType, TestType>::testCase3();

        if (verbose) cout << "Testing with Type 'int'" << endl;
        TestDriver<int, int>::testCase3();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // MAP MEMORY CONSUMPTION AND CONSTRUCTOR TEST
        //
        // Concerns:
        //   That the memory allocator is getting properly passed to objects
        //   in the container, and that all constructors in the standard
        //   compile.
        //
        // Plan:
        //   Store the 'Cargo' and 'CargoNoAddressOf' 'struct', which
        //   dynamically allocates large amounts of memory, and verify that
        //   large amounts of memory are consumed in the allocator that was
        //   passed to the container.  Create maps with all c'tors in the
        //   standard.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing with Type 'TestType' and 'Cargo'" << endl;
        TestDriver<TestType, Cargo>::testCase2();

        if (verbose) cout << "Testing with Type 'TestType' and"
                          << " 'CargoNoAddressOf'"
                          << endl;
        TestDriver<TestType, CargoNoAddressOf>::testCase2();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // MAP BREATHING TEST
        //
        // Concerns:
        //   Verify that the container and its iterator are the same type in
        //   'bsl' and 'std', and that the native_std containter is different
        //   from the 'std' container.  Also verify that the bsl container
        //   uses the bslma allocator, and that the 'native_std' container
        //   doesn't.  Verify that iterators don't use the 'bslma' allocator.
        //
        // Plan:
        //   Use the templates 'sameType' and 'usesBslmaAllocator' (defined
        //   in this file) to do the testing.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing with Type 'TestType'" << endl;
        TestDriver<TestType, TestType>::testCase1();

        if (verbose) cout << "Testing with Type 'int'" << endl;
        TestDriver<int, int>::testCase1();
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
