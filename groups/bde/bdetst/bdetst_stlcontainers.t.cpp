// bdetst_stlcontainers.t.cpp                  -*-C++-*-

#include <bdetst_stlcontainers.h>
#include <bdema_defaultallocatorguard.h>

#include <cstdio>
#include <cstdlib>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
# define NAME_ASSERT(NAME, X) if (! (X)) { \
        printf("%s = %s\nError " __FILE__ "(%d): %s    (failed)\n", \
               #NAME, NAME, __LINE__, #X); \
        if (testStatus >= 0 && testStatus <= 100) ++testStatus; \
    }


//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

const int INT_DATA[] = { 1, 2, 3, 5, 7, 11, 13, 17, 19, 23 };
const int INT_DATA_LEN = sizeof(INT_DATA) / sizeof(INT_DATA[0]);
const int *const INT_DATA_END = INT_DATA + INT_DATA_LEN;

typedef pair<int, const char*> my_Pair;
const my_Pair PAIR_DATA[] = {
    my_Pair(1, "one"),
    my_Pair(2, "two"),
    my_Pair(3, "three"),
    my_Pair(5, "five"),
    my_Pair(7, "seven"),
    my_Pair(11, "eleven"),
    my_Pair(13, "thirteen"),
    my_Pair(17, "seventeen"),
    my_Pair(19, "nineteen"),
    my_Pair(23, "twenty three")
};
const int PAIR_DATA_LEN = sizeof(PAIR_DATA) / sizeof(PAIR_DATA[0]);
const my_Pair *const PAIR_DATA_END = PAIR_DATA + PAIR_DATA_LEN;

// Explicitly instantiate each container template.
// Don't instantiate string -- it is already explicitly instantiated
// in dll_main.
template class vector<int>;
template class deque<int>;
template class bdestl_Deque<int>;
template class list<int>;
template class set<int>;
template class multiset<int>;
template class hash_set<int>;
template class hash_multiset<int>;

template class map<int, const char*>;
template class multimap<int, const char*>;
template class hash_map<int, const char*>;
template class hash_multimap<int, const char*>;

template struct bdealg_TypeTraits<vector<int> >;

struct my_ClassRep
{
    // Data members that give my_ClassX size and alignment.
    bdema_Allocator            *d_allocator;
    bdema_AllocationHint::Hint  d_hint;
    int                         d_value;
};

class my_Class1
{
    // Class that doesn't take allocator or hints
    my_ClassRep d_rep;

  public:
    my_Class1(int v = 0) {
        d_rep.d_value = v; d_rep.d_allocator = NULL;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class1(const my_Class1& rhs) {
        d_rep.d_value = rhs.d_rep.d_value; d_rep.d_allocator = NULL;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }

    int value() const { return d_rep.d_value; }
    const bdema_Allocator* allocator() const { return d_rep.d_allocator; }
    bdema_AllocationHint::Hint hint() const { return d_rep.d_hint; }
};

inline bool operator==(const my_Class1& lhs, const my_Class1& rhs)
    { return lhs.value() == rhs.value(); }
inline bool operator<(const my_Class1& lhs, const my_Class1& rhs)
    { return lhs.value() < rhs.value(); }

class my_Class2
{
    // Class that takes allocator but not hints
    my_ClassRep d_rep;

  public:
    my_Class2(bdema_Allocator *a = NULL) {
        d_rep.d_value = 0; d_rep.d_allocator = 0;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class2(int v, bdema_Allocator *a = NULL) {
        d_rep.d_value = v; d_rep.d_allocator = a;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class2(const my_Class2& rhs, bdema_Allocator *a = NULL) {
        d_rep.d_value = rhs.d_rep.d_value; d_rep.d_allocator = a;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }

    int value() const { return d_rep.d_value; }
    const bdema_Allocator* allocator() const { return d_rep.d_allocator; }
    bdema_AllocationHint::Hint hint() const { return d_rep.d_hint; }
};

inline bool operator==(const my_Class2& lhs, const my_Class2& rhs)
    { return lhs.value() == rhs.value(); }
inline bool operator<(const my_Class2& lhs, const my_Class2& rhs)
    { return lhs.value() < rhs.value(); }

class my_Class3
{
    // Class that takes allocator and hints
    my_ClassRep d_rep;

  public:
    my_Class3(bdema_Allocator *a = NULL) {
        d_rep.d_value = 0; d_rep.d_allocator = 0;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class3(bdema_AllocationHint::Hint h, bdema_Allocator *a = NULL) {
        d_rep.d_value = 0; d_rep.d_allocator = 0; d_rep.d_hint = h; }
    my_Class3(int v, bdema_Allocator *a = NULL) {
        d_rep.d_value = v; d_rep.d_allocator = a;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class3(int v, bdema_AllocationHint::Hint h, bdema_Allocator *a = NULL) {
        d_rep.d_value = v; d_rep.d_allocator = a;
        d_rep.d_hint = h; }
    my_Class3(const my_Class3& rhs, bdema_Allocator *a = NULL) {
        d_rep.d_value = rhs.d_rep.d_value; d_rep.d_allocator = a;
        d_rep.d_hint = bdema_AllocationHint::PASS_THROUGH; }
    my_Class3(const my_Class3& rhs, bdema_AllocationHint::Hint h,
              bdema_Allocator *a = NULL) {
        d_rep.d_value = rhs.d_rep.d_value; d_rep.d_allocator = a;
        d_rep.d_hint = h; }

    int value() const { return d_rep.d_value; }
    const bdema_Allocator* allocator() const { return d_rep.d_allocator; }
    bdema_AllocationHint::Hint hint() const { return d_rep.d_hint; }
};

inline bool operator==(const my_Class3& lhs, const my_Class3& rhs)
    { return lhs.value() == rhs.value(); }
inline bool operator<(const my_Class3& lhs, const my_Class3& rhs)
    { return lhs.value() < rhs.value(); }

namespace BloombergLP
{

template <> struct bdealg_TypeTraits<my_Class2>
    : bdealg_TypeTraitUsesBdemaAllocator { };

template <> struct bdealg_TypeTraits<my_Class3>
    : bdealg_TypeTraitUsesBdemaAllocatorWithHint { };

} // Close BloombergLP namespace

namespace BloombergLP_std
{
    // Add definitions to std namespace
    
template <> struct hash<my_Class1> {
    int operator()(const my_Class1& v) const { return v.value(); }
};

template <> struct hash<my_Class2> {
    int operator()(const my_Class2& v) const { return v.value(); }
};

template <> struct hash<my_Class3> {
    int operator()(const my_Class3& v) const { return v.value(); }
};

} // Close BloombergLP_std

const my_Class1 CLASS1_DATA[] = {
    my_Class1(1),
    my_Class1(2),
    my_Class1(3),
    my_Class1(5),
    my_Class1(7),
    my_Class1(11),
    my_Class1(13),
    my_Class1(17),
    my_Class1(19),
    my_Class1(23),
};
const int CLASS1_DATA_LEN = sizeof(CLASS1_DATA) / sizeof(CLASS1_DATA[0]);
const my_Class1 *const CLASS1_DATA_END = CLASS1_DATA + CLASS1_DATA_LEN;

const my_Class2 CLASS2_DATA[] = {
    my_Class2(1),
    my_Class2(2),
    my_Class2(3),
    my_Class2(5),
    my_Class2(7),
    my_Class2(11),
    my_Class2(13),
    my_Class2(17),
    my_Class2(19),
    my_Class2(23),
};
const int CLASS2_DATA_LEN = sizeof(CLASS2_DATA) / sizeof(CLASS2_DATA[0]);
const my_Class2 *const CLASS2_DATA_END = CLASS2_DATA + CLASS2_DATA_LEN;

const my_Class3 CLASS3_DATA[] = {
    my_Class3(1),
    my_Class3(2),
    my_Class3(3),
    my_Class3(5),
    my_Class3(7),
    my_Class3(11),
    my_Class3(13),
    my_Class3(17),
    my_Class3(19),
    my_Class3(23),
};
const int CLASS3_DATA_LEN = sizeof(CLASS3_DATA) / sizeof(CLASS3_DATA[0]);
const my_Class3 *const CLASS3_DATA_END = CLASS3_DATA + CLASS3_DATA_LEN;

typedef pair<const my_Class1, const char*> my_Class1pair;

const my_Class1pair CLASS1_PAIR_DATA[] = {
    my_Class1pair(1, "one"),
    my_Class1pair(2, "two"),
    my_Class1pair(3, "three"),
    my_Class1pair(5, "five"),
    my_Class1pair(7, "seven"),
    my_Class1pair(11, "eleven"),
    my_Class1pair(13, "thirteen"),
    my_Class1pair(17, "seventeen"),
    my_Class1pair(19, "nineteen"),
    my_Class1pair(23, "twenty three")
};
const int CLASS1_PAIR_DATA_LEN =
        sizeof(CLASS1_PAIR_DATA) / sizeof(CLASS1_PAIR_DATA[0]);
const my_Class1pair *const CLASS1_PAIR_DATA_END =
        CLASS1_PAIR_DATA + CLASS1_PAIR_DATA_LEN;

typedef pair<const my_Class2, const char*> my_Class2pair;

const my_Class2pair CLASS2_PAIR_DATA[] = {
    my_Class2pair(1, "one"),
    my_Class2pair(2, "two"),
    my_Class2pair(3, "three"),
    my_Class2pair(5, "five"),
    my_Class2pair(7, "seven"),
    my_Class2pair(11, "eleven"),
    my_Class2pair(13, "thirteen"),
    my_Class2pair(17, "seventeen"),
    my_Class2pair(19, "nineteen"),
    my_Class2pair(23, "twenty three")
};
const int CLASS2_PAIR_DATA_LEN =
        sizeof(CLASS2_PAIR_DATA) / sizeof(CLASS2_PAIR_DATA[0]);
const my_Class2pair *const CLASS2_PAIR_DATA_END =
        CLASS2_PAIR_DATA + CLASS2_PAIR_DATA_LEN;

typedef pair<const my_Class3, const char*> my_Class3pair;

const my_Class3pair CLASS3_PAIR_DATA[] = {
    my_Class3pair(1, "one"),
    my_Class3pair(2, "two"),
    my_Class3pair(3, "three"),
    my_Class3pair(5, "five"),
    my_Class3pair(7, "seven"),
    my_Class3pair(11, "eleven"),
    my_Class3pair(13, "thirteen"),
    my_Class3pair(17, "seventeen"),
    my_Class3pair(19, "nineteen"),
    my_Class3pair(23, "twenty three")
};
const int CLASS3_PAIR_DATA_LEN =
        sizeof(CLASS3_PAIR_DATA) / sizeof(CLASS3_PAIR_DATA[0]);
const my_Class3pair *const CLASS3_PAIR_DATA_END =
        CLASS3_PAIR_DATA + CLASS3_PAIR_DATA_LEN;

template <class CONTAINER>
struct passThroughTest
{
    static void testSingle(bool elemUsesAlloc, bool elemUsesHint,
                           const char* containerName);
    static void testPair(bool elemUsesAlloc, bool elemUsesHint,
                         const char* containerName);
};

template <class CONTAINER>
void passThroughTest<CONTAINER>::testSingle(bool elemUsesAlloc,
                                            bool elemUsesHint,
                                            const char *containerName)
{
    typedef typename CONTAINER::value_type T;
    typedef typename CONTAINER::const_iterator const_iterator;

    static const T T_DATA[] = {
        T(1), T(2), T(3), T(5), T(7), T(11), T(13), T(17), T(19), T(23)
    };
    static const int T_DATA_LEN = sizeof(T_DATA) / sizeof(T_DATA[0]);
    static const T *const T_DATA_END = T_DATA + T_DATA_LEN;

    bdema_TestAllocator testAlloc;
    const bdema_Allocator *const expectedElemAlloc = (elemUsesAlloc ?
                                                      &testAlloc : 0);
    bdema_TestAllocator defaultAlloc;
    bdema_DefaultAllocatorGuard defaultAllocGuard(&defaultAlloc);

//     const bdema_AllocationHint::Hint expectedElemHint =
//         (elemUsesHint ? bdema_AllocationHint::WRITE_MANY :
//          bdema_AllocationHint::PASS_THROUGH);
    const bdema_AllocationHint::Hint expectedElemHint =
        bdema_AllocationHint::PASS_THROUGH;

    {
        CONTAINER original(T_DATA, T_DATA_END);
        std::size_t baseBytes = defaultAlloc.numBytesInUse();

        CONTAINER c(original, &testAlloc);
        NAME_ASSERT(containerName, &testAlloc == c.get_allocator());
        NAME_ASSERT(containerName, 0 != testAlloc.numBytesInUse());
        NAME_ASSERT(containerName, baseBytes == defaultAlloc.numBytesInUse());

        NAME_ASSERT(containerName,
                    (bdealg_HasTrait<CONTAINER,
                                  bdealg_TypeTraitUsesBdemaAllocator>::VALUE));

        NAME_ASSERT(containerName,
                    (bdealg_HasTrait<CONTAINER,
                                  bdealg_TypeTraitHasStlIterators>::VALUE));

        const T* first = T_DATA;
        const T* last = T_DATA_END;
        int originalStatus = testStatus;
        for (const_iterator i = c.begin(); i != c.end(); ++i, ++first) {
            NAME_ASSERT(containerName, first != last);
            NAME_ASSERT(containerName, *first == *i);

            // Report pass-through errors only once:
            if (testStatus != originalStatus) continue;

            NAME_ASSERT(containerName, expectedElemAlloc == i->allocator());
            NAME_ASSERT(containerName, expectedElemHint == i->hint());
        }

        NAME_ASSERT(containerName, first == last);
    }

    NAME_ASSERT(containerName, 0 == testAlloc.numBytesInUse());
}

template <class CONTAINER>
void passThroughTest<CONTAINER>::testPair(bool elemUsesAlloc,
                                          bool elemUsesHint,
                                          const char *containerName)
{
    typedef typename CONTAINER::value_type T;
    typedef typename T::first_type K;
    typedef typename T::second_type V;
    typedef typename CONTAINER::const_iterator const_iterator;

    static const T T_DATA[] = {
        T(1, "one"),
        T(2, "two"),
        T(3, "three"),
        T(5, "five"),
        T(7, "seven"),
        T(11, "eleven"),
        T(13, "thirteen"),
        T(17, "seventeen"),
        T(19, "nineteen"),
        T(23, "twenty three")
    };
    static const int T_DATA_LEN = sizeof(T_DATA) / sizeof(T_DATA[0]);
    static const T *const T_DATA_END = T_DATA + T_DATA_LEN;

    bdema_TestAllocator testAlloc;
    const bdema_Allocator *const expectedElemAlloc = (elemUsesAlloc ?
                                                      &testAlloc : 0);
    bdema_TestAllocator defaultAlloc;
    bdema_DefaultAllocatorGuard defaultAllocGuard(&defaultAlloc);

//     const bdema_AllocationHint::Hint expectedElemHint =
//         (elemUsesHint ? bdema_AllocationHint::WRITE_MANY :
//          bdema_AllocationHint::PASS_THROUGH);
    const bdema_AllocationHint::Hint expectedElemHint =
        bdema_AllocationHint::PASS_THROUGH;

    {
        CONTAINER original(T_DATA, T_DATA_END);
        std::size_t baseBytes = defaultAlloc.numBytesInUse();

        CONTAINER c(original, &testAlloc);
        NAME_ASSERT(containerName, &testAlloc == c.get_allocator());
        NAME_ASSERT(containerName, 0 != testAlloc.numBytesInUse());
        NAME_ASSERT(containerName, baseBytes == defaultAlloc.numBytesInUse());

        NAME_ASSERT(containerName,
                    (bdemf_IsConvertible<bdema_Allocator*,
                     typename CONTAINER::allocator_type>::VALUE));

        NAME_ASSERT(containerName,
                    (bdealg_HasTrait<CONTAINER,
                                  bdealg_TypeTraitUsesBdemaAllocator>::VALUE));

        NAME_ASSERT(containerName,
                    (bdealg_HasTrait<CONTAINER,
                                  bdealg_TypeTraitHasStlIterators>::VALUE));

        const T* first = T_DATA;
        const T* last = T_DATA_END;
        int originalStatus = testStatus;
        for (const_iterator i = c.begin(); i != c.end(); ++i, ++first) {
            NAME_ASSERT(containerName, first != last);
            NAME_ASSERT(containerName, *first == *i);

            // Report pass-through errors only once:
            if (testStatus != originalStatus) continue;

            NAME_ASSERT(containerName, expectedElemAlloc == i->first.allocator());
            NAME_ASSERT(containerName, expectedElemHint == i->first.hint());
        }

        NAME_ASSERT(containerName, first == last);
    }

    NAME_ASSERT(containerName, 0 == testAlloc.numBytesInUse());
    NAME_ASSERT(containerName, 0 == defaultAlloc.numBytesInUse());
}

#define TEST_PASS_THROUGH_SINGLE(C,A,H) passThroughTest<C >::testSingle((A),(H),#C)
#define TEST_PASS_THROUGH_PAIR(C1, C2,A,H) passThroughTest<C1, C2 >::testPair((A),(H),#C1 "," #C2)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TEMPLATE CTOR TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   DRQS 4868710
        // --------------------------------------------------------------------

        if (verbose) printf("\nTEMPLATE CTOR TEST"
                            "\n==================\n");

        if (verbose) printf("\n1. Testing allocator.\n");
        {
            // testing copy ctor
            std::allocator<my_Class1> mX;
            std::allocator<my_Class1> mY(mX);
        }

        if (verbose) printf("\n2. Testing vector.\n");
        {
            // testing iterator ctor
            std::vector<my_Class1> mX(CLASS1_DATA, CLASS1_DATA_END);

            ASSERT(CLASS1_DATA_LEN == mX.size());

            for (int i = 0; i < CLASS1_DATA_LEN; ++i) {
                ASSERT(mX[i].value() == CLASS1_DATA[i].value());
            }
        }

        if (verbose) printf("\n3. Testing deque.\n");
        {
            // testing iterator ctor
            std::deque<my_Class1> mX(CLASS1_DATA, CLASS1_DATA_END);

            ASSERT(CLASS1_DATA_LEN == mX.size());

            for (int i = 0; i < CLASS1_DATA_LEN; ++i) {
                ASSERT(mX[i].value() == CLASS1_DATA[i].value());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PASS-THROUGH TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nPASS-THROUGH TEST"
                            "\n=================\n");

        TEST_PASS_THROUGH_SINGLE(vector<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(deque<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(bdestl_Deque<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(list<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(set<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(multiset<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(hash_set<my_Class1>, false, false);
        TEST_PASS_THROUGH_SINGLE(hash_multiset<my_Class1>, false, false);

        TEST_PASS_THROUGH_PAIR(map<my_Class1, const char*>, false, false);
        TEST_PASS_THROUGH_PAIR(multimap<my_Class1, const char*>, false, false);
        TEST_PASS_THROUGH_PAIR(hash_map<my_Class1, const char*>, false, false);
        TEST_PASS_THROUGH_PAIR(hash_multimap<my_Class1, const char*>,
                               false, false);

        TEST_PASS_THROUGH_SINGLE(vector<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(deque<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(bdestl_Deque<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(list<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(set<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(multiset<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(hash_set<my_Class2>, true, false);
        TEST_PASS_THROUGH_SINGLE(hash_multiset<my_Class2>, true, false);

        TEST_PASS_THROUGH_PAIR(map<my_Class2, const char*>, true, false);
        TEST_PASS_THROUGH_PAIR(multimap<my_Class2, const char*>, true, false);
        TEST_PASS_THROUGH_PAIR(hash_map<my_Class2, const char*>, true, false);
        TEST_PASS_THROUGH_PAIR(hash_multimap<my_Class2, const char*>,
                               true, false);

        TEST_PASS_THROUGH_SINGLE(vector<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(deque<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(bdestl_Deque<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(list<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(set<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(multiset<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(hash_set<my_Class3>, true, true);
        TEST_PASS_THROUGH_SINGLE(hash_multiset<my_Class3>, true, true);

        TEST_PASS_THROUGH_PAIR(map<my_Class3, const char*>, true, true);
        TEST_PASS_THROUGH_PAIR(multimap<my_Class3, const char*>, true, true);
        TEST_PASS_THROUGH_PAIR(hash_map<my_Class3, const char*>, true, true);
        TEST_PASS_THROUGH_PAIR(hash_multimap<my_Class3, const char*>,
                               true, true);

      } break;
        
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Construct with default allocator
        string                          s(INT_DATA, INT_DATA_END);
        vector<int>                     vi(INT_DATA, INT_DATA_END);
        deque<int>                      di(INT_DATA, INT_DATA_END);
        bdestl_Deque<int>               bdi(INT_DATA, INT_DATA_END);
        list<int>                       li(INT_DATA, INT_DATA_END);
        set<int>                        si(INT_DATA, INT_DATA_END);
        multiset<int>                   msi(INT_DATA, INT_DATA_END);
        hash_set<int>                   hsi(INT_DATA, INT_DATA_END);
        hash_multiset<int>              hmsi(INT_DATA, INT_DATA_END);

        map<int, const char*>           mp(PAIR_DATA, PAIR_DATA_END);
        multimap<int, const char*>      mmp(PAIR_DATA, PAIR_DATA_END);
        hash_map<int, const char*>      hmp(PAIR_DATA, PAIR_DATA_END);
        hash_multimap<int, const char*> hmmp(PAIR_DATA, PAIR_DATA_END);

        map<pair<int, int>, pair<int, int> > mpipi;
        mpipi[make_pair(3, 4)] = make_pair(4, 5);

        bdema_Allocator *const defaultAlloc =
            bdema_Default::defaultAllocator();

        ASSERT(defaultAlloc == s.get_allocator());
        ASSERT(defaultAlloc == vi.get_allocator());
        ASSERT(defaultAlloc == di.get_allocator());
        ASSERT(defaultAlloc == bdi.get_allocator());
        ASSERT(defaultAlloc == li.get_allocator());
        ASSERT(defaultAlloc == si.get_allocator());
        ASSERT(defaultAlloc == msi.get_allocator());
        ASSERT(defaultAlloc == hsi.get_allocator());
        ASSERT(defaultAlloc == hmsi.get_allocator());
        ASSERT(defaultAlloc == mp.get_allocator());
        ASSERT(defaultAlloc == mmp.get_allocator());
        ASSERT(defaultAlloc == hmp.get_allocator());
        ASSERT(defaultAlloc == hmmp.get_allocator());

        bdema_TestAllocator ta;
        
        // Copy construct with explicit allocator argument
        string                          s2(s, &ta);
        vector<int>                     vi2(vi, &ta);
        deque<int>                      di2(di, &ta);
        bdestl_Deque<int>               bdi2(bdi, &ta);
        list<int>                       li2(li, &ta);
        set<int>                        si2(si, &ta);
        multiset<int>                   msi2(msi, &ta);
        hash_set<int>                   hsi2(hsi, &ta);
        hash_multiset<int>              hmsi2(hmsi, &ta);

        map<int, const char*>           mp2(mp, &ta);
        multimap<int, const char*>      mmp2(mmp, &ta);
        hash_map<int, const char*>      hmp2(hmp, &ta);
        hash_multimap<int, const char*> hmmp2(hmmp, &ta);

        ASSERT(&ta == s2.get_allocator());
        ASSERT(&ta == vi2.get_allocator());
        ASSERT(&ta == di2.get_allocator());
        ASSERT(&ta == bdi2.get_allocator());
        ASSERT(&ta == li2.get_allocator());
        ASSERT(&ta == si2.get_allocator());
        ASSERT(&ta == msi2.get_allocator());
        ASSERT(&ta == hsi2.get_allocator());
        ASSERT(&ta == hmsi2.get_allocator());
        ASSERT(&ta == mp2.get_allocator());
        ASSERT(&ta == mmp2.get_allocator());
        ASSERT(&ta == hmp2.get_allocator());
        ASSERT(&ta == hmmp2.get_allocator());

        // Copy construct with default allocator
        string                          s3(s2);
        vector<int>                     vi3(vi2);
        deque<int>                      di3(di2);
        bdestl_Deque<int>               bdi3(bdi2);
        list<int>                       li3(li2);
        set<int>                        si3(si2);
        multiset<int>                   msi3(msi2);
        hash_set<int>                   hsi3(hsi2);
        hash_multiset<int>              hmsi3(hmsi2);

        map<int, const char*>           mp3(mp2);
        multimap<int, const char*>      mmp3(mmp2);
        hash_map<int, const char*>      hmp3(hmp2);
        hash_multimap<int, const char*> hmmp3(hmmp2);

        ASSERT(defaultAlloc == s3.get_allocator());
        ASSERT(defaultAlloc == vi3.get_allocator());
        ASSERT(defaultAlloc == di3.get_allocator());
        ASSERT(defaultAlloc == bdi3.get_allocator());
        ASSERT(defaultAlloc == li3.get_allocator());
        ASSERT(defaultAlloc == si3.get_allocator());
        ASSERT(defaultAlloc == msi3.get_allocator());
        ASSERT(defaultAlloc == hsi3.get_allocator());
        ASSERT(defaultAlloc == hmsi3.get_allocator());
        ASSERT(defaultAlloc == mp3.get_allocator());
        ASSERT(defaultAlloc == mmp3.get_allocator());
        ASSERT(defaultAlloc == hmp3.get_allocator());
        ASSERT(defaultAlloc == hmmp3.get_allocator());

      } break;

      case -1: {
        // --------------------------------------------------------------------
        // TEST SAFE MODE
        // Perform undefined behavior and confirm that STLPort produces an
        // understandable error message and aborts.  Will not produce an error
        // if not compiled in safe mode (though there is a small chance it will
        // segv).
        // --------------------------------------------------------------------

        // Switch on last character of argv[1]
        switch (argv[1][std::strlen(argv[1]) - 1]) {
          case 'v': { // vector test
            std::vector<int> x(5, 2);
            x[5] = 3; // Out-of-bounds error
          } break;

          case 'l': { // list test
            std::list<int> x, y;
            std::list<int>::iterator i = x.begin();
            y.insert(i, 3);     // Specified iterator from wrong container
          } break;

          case 'd': { // deque test
            std::deque<int> x(5, 2);
            std::deque<int>::iterator i1 = x.begin(), i2 = i1;
            x.erase(i1);
            *i2 = 4;    // Use of invalidated iterator
          } break;

          case 's': { // set test
            std::set<int>::iterator i;
            {
              std::set<int> x;
              x.insert(3);
              i = x.begin();
              int v = *i;
            }

            int v2 = *i;  // Iterator to destroyed set
            
          } break;

          case 'S': { // multiset test
            std::multiset<int>::iterator i;
            int v = *i;       // Uninitialized iterator
          } break;

          case 'm': { // map test
            typedef std::map<int, int> Obj;
            Obj x;
            x[3] = 5;
            x[5] = 12;
            x[3] = 4;
            ASSERT(2 == x.size());
            int v = x.find(6)->second; // Dereference end() iterator
          } break;

          case 'M': { // multimap test
            typedef std::multimap<int, int> Obj;
            Obj x;
            x.insert(Obj::value_type(3, 5));
            x.insert(Obj::value_type(5, 12));
            x.insert(Obj::value_type(3, 4));
            ASSERT(3 == x.size());
            Obj::const_iterator i = x.lower_bound(4);  // points to (5, 12)
            x.erase(5);
            int v = i->second;  // dereference invalidated iterator
          } break;

          case 'h': { // hash_set test
            static const int DATA[] = { 3, 2, 1, 2 };
            std::hash_set<int> x(DATA, DATA + 4);
            ASSERT(3 == x.size());
            std::hash_set<int>::iterator i = x.begin();
            x.erase(1);
            int v = *i;    // Use of invalidated iterator
          } break;

          case 'H': { // hash_multiset test
            static const int DATA[] = { 3, 2, 1, 2 };
            std::hash_multiset<int> x(DATA, DATA + 4);
            ASSERT(4 == x.size());
            std::hash_set<int>::const_iterator i = x.begin();
            ++i;
            x.erase(2);
            ASSERT(2 == x.size());
            int v = *i;    // Use of invalidated iterator
          } break;

          case 'x': { // hash_map test
            typedef std::hash_map<int, int> Obj;
            Obj x;
            x[3] = 5;
            x[5] = 12;
            x[3] = 4;
            ASSERT(2 == x.size());
            int v = x.find(6)->second; // Dereference end() iterator
          } break;

          case 'X': { // hash_multimap test
            typedef std::hash_multimap<int, int> Obj;
            Obj x;
            x.insert(Obj::value_type(3, 5));
            x.insert(Obj::value_type(5, 12));
            x.insert(Obj::value_type(3, 4));
            ASSERT(3 == x.size());
            Obj::const_iterator i = x.find(5);  // points to (5, 12)
            x.erase(5);
            int v = i->second;  // dereference invalidated iterator
          } break;

          default:
            fprintf(stderr, "Error: Must specify [vldsSmMhHxX] after -1 "
                    "(e.g. -1v for vector)\n");
            testStatus = -1;
        } // End switch

      } break;

      default: {
          fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
          testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
