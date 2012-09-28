// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_pair.h>

#include <bslalg_bidirectionallink.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_stdtestallocator.h>

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

// To resolve gcc warnings, while printing 'size_t' arguments portably on
// Windows, we use a macro and string literal concatenation to produce the
// correct 'printf' format flag.
#ifdef ZU
#undef ZU
#endif

#if defined BSLS_PLATFORM__CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
//
// (informal plan to be written up later)
// HashTable is most similar to the 'unordered_multimap' container, although
// the 'key' is implicitly also part of the 'value_type', rather than being
// storing as distinct fields in a pair.
//
// There are a wide variety of potential types to independently instantiate
// each of the template parameters of HashTable with.  To ease testing, we
// will have a primary test driver having a single type parameter that can be
// easily driven by the template testing facility, delegated to by multiple
// test configuration classes, which act as a C++03 template alias to an
// appropriate configuration of the test driver.  The main 'switch' will
// therefore run multiple instantiations of each test driver function to
// establish the necessary properties.
//
// First we will validate that HashTable is a valid value-semantic type.  This
// is difficult in the case that the stored elements are not themselves value-
// semantic, so this early testing will be limited to only those types that
// provide a full range of required behavior; testing of non-value semantic
// elements, or awkward hash and compare functors, will be deferred past the
// initial 10 cases.
// To establish value semantics, we will test the following class members, and
// a couple of specific test-support functions that simplify the test space:
//     default constructor
//     copy constructor
//     destructor
//     copy assignment operator
//     operator ==/!=
//
//   Accessors and manipulators
//     'HashTable::allocator'
//     'HashTable::elementListRoot'
//     insertValue        - a test function using 'insert' restricted
//                          to ValueType
//     verifyListContents - key accessor to validate the list root points to a
//                          list having the right set of values, and arranged
//                          so that elements with equivalent keys, determined
//                          by a supplied comparator, are stored contiguously.
//
// Therefore, 'hasher' and 'comparator' are not salient attributes, even though
// value ultimately depends on 'comparator' to define key-equivalent groups.
// Likewise, no 'insert*' operation forms the primary manipulator, nor is
// 'maxLoadFactor' a concern in establishing value - insert operations must
// satisfy constraints implied by all these additional moving parts, and will
// all be established in test cases following the value-semantic test sequence.
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//  TBD....
//
//
//           ( A '*' IN THE TABLE BELOW INDICATES THE TEST CASE HAS )
//           (   BEEN IDENTIFIED BUT HAS NOT YET BEEN COMPLETED.    )
//
//           ( NOTE THAT ALL TESTING AT THE MOMENT ASSUMES A 'set'- )
//           ( LIKE KEY_CONFIG AND THERE IS NO 'map'-LIKE EQUIVALENT)
//           ( NOR MACHINERY FOR CREATING SUITABLE TEST SEQUENCES.  )
//
//           ( WE ARE STILL LOOKING FOR A MINIMAL-BUT-COMPLETE SET  )
//           ( OF FUNCTORS AND NON-BDE ALLOCATORS TO COMPRISE TEST  )
//           (          KITS TO INVOKE FOR EACH TEST CASE.          )
//
// TYPES
//*[23] typedef ALLOCATOR                              AllocatorType;
//*[23] typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[23] typedef typename KEY_CONFIG::KeyType           KeyType;
//*[23] typedef typename KEY_CONFIG::ValueType         ValueType;
//*[23] typedef bslalg::BidirectionalNode<ValueType>   NodeType;
//*[23] typedef typename AllocatorTraits::size_type    SizeType;
//
// CREATORS
//*[11] HashTable(const ALLOCATOR&  allocator = ALLOCATOR());
//*[ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
//*[ 7] HashTable(const HashTable& original);
//*[ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
//*[ 2] ~HashTable();
//
// MANIPULATORS
//*[ 9] operator=(const HashTable& rhs);
//*[15] insert(const SOURCE_TYPE& obj);
//*[15] insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
//*[16] insertIfMissing(bool *isInsertedFlag, const SOURCE_TYPE& obj);
//*[17] insertIfMissing(const KeyType& key);
//*[12] remove(bslalg::BidirectionalLink *node);
//*[ 2] removeAll();
//*[13] rehashForNumBuckets(SizeType newNumBuckets);
//*[13] rehashForNumElements(SizeType numElements);
//*[18] setMaxLoadFactor(float loadFactor);
//*[ 8] swap(HashTable& other);
//
//      ACCESSORS
//*[ 4] allocator() const;
//*[ 4] comparator() const;
//*[ 4] hasher() const;
//*[ 4] size() const;
//*[22] maxSize() const;
//*[ 4] numBuckets() const;
//*[22] maxNumBuckets() const;
//*[14] loadFactor() const;
//*[ 4] maxLoadFactor() const;
//*[ 4] elementListRoot() const;
//*[19] find(const KeyType& key) const;
//*[20] findRange(BLink **first, BLink **last, const KeyType& k) const;
//*[ 6] findEndOfRange(bslalg::BidirectionalLink *first) const;
//*[ 4] bucketAtIndex(SizeType index) const;
//*[ 4] bucketIndexForKey(const KeyType& key) const;
//*[21] countElementsInBucket(SizeType index) const;
//
//*[ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
//*[ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
//// specialized algorithms:
//*[ 8] void swap(HashTable& a, HashTable& b);
//
// [ 2] insert (boostrap)
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// Class HashTable_ImpDetails
// [  ] size_t nextPrime(size_t n);
// [  ] bslalg::HashTableBucket *defaultBucketAddress();
//
// Class HashTable_Util<ALLOCATOR> 
// [  ] initAnchor(bslalg::HashTableAnchor *, SizeType, const ALLOC&); 
// [  ] destroyBucketArray(bslalg::HashTableBucket *, SizeType, const ALLOC&) 
//
// Class HashTable_ListProctor
// [  ] TBD...
//
// Class HashTable_ArrayProctor 
// [  ] TBD...
//
// TEST TEST APPARATUS AND GENERATOR FUNCTIONS
//*[ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
//*[ 3] HashTable& gg(HashTable *object, const char *spec);
//*[ 2] insertElement(HashTable<K, H, E, A> *, const K::ValueType&)
//*[ 3] verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
//
// [  ] CONCERN: The type is compatible with STL allocator.
// [  ] CONCERN: The type has the necessary type traits.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    2, "AA",                "AA" },
    { L_,   20, "B",                 "B" },
    { L_,    5, "AB",                "AB" },
    { L_,    5, "BA",                "AB" },
    { L_,   19, "AC",                "AC" },
    { L_,   23, "CD",                "CD" },
    { L_,    7, "ABC",               "ABC" },
    { L_,    7, "ACB",               "ABC" },
    { L_,    7, "BAC",               "ABC" },
    { L_,    7, "BCA",               "ABC" },
    { L_,    7, "CAB",               "ABC" },
    { L_,    7, "CBA",               "ABC" },
    { L_,   17, "BAD",               "ABD" },
    { L_,    4, "ABCA",              "AABC" },
    { L_,    6, "ABCB",              "ABBC" },
    { L_,    8, "ABCC",              "ABCC" },
    { L_,    3, "ABCABC",            "AABBCC" },
    { L_,    3, "AABBCC",            "AABBCC" },
    { L_,    9, "ABCD",              "ABCD" },
    { L_,    9, "ACBD",              "ABCD" },
    { L_,    9, "BDCA",              "ABCD" },
    { L_,    9, "DCBA",              "ABCD" },
    { L_,   18, "BEAD",              "ABDE" },
    { L_,   21, "BCDE",              "BCDE" },
    { L_,   10, "ABCDE",             "ABCDE" },
    { L_,   10, "ACBDE",             "ABCDE" },
    { L_,   10, "CEBDA",             "ABCDE" },
    { L_,   10, "EDCBA",             "ABCDE" },
    { L_,   22, "FEDCB",             "BCDEF" },
    { L_,   11, "FEDCBA",            "ABCDEF" },
    { L_,   12, "ABCDEFG",           "ABCDEFG" },
    { L_,   13, "ABCDEFGH",          "ABCDEFGH" },
    { L_,   14, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   15, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_,   15, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_,   16, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   16, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};

static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;
typedef bsltf::StdTestAllocator<int> StlTestIntAllocator;

typedef ::bsl::hash<int>     TestIntHash;
typedef ::bsl::equal_to<int> TestIntEqual;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

}  // close namespace bsl


namespace BloombergLP {
namespace bslstl {
// HashTable-specific print function.
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t)
{
    if (0 == t.size()) {
        printf("<empty>");
    }
    else {

        for (Link *it = t.elementListRoot(); it; it = it->nextLink()) {
            const typename KEY_CONFIG::KeyType& key =
                                           ImpUtil::extractKey<KEY_CONFIG>(it);
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                             bsltf::TemplateTestFacility::getIdentifier(key)));
        }
    }
    fflush(stdout);
}

}  // close namespace BloombergLP::bslstl
}  // close namespace BloombergLP


namespace {

struct BoolArray {
    // This class holds a set of boolean flags...

    explicit BoolArray(size_t n)
    : d_data(new bool[n])
    {
        for (size_t i = 0; i != n; ++i) {
            d_data[i] = false;
        }
    }

    ~BoolArray()
    {
        delete[] d_data;
    }

    bool& operator[](size_t index) { return d_data[index]; }
    bool *d_data;
};


template<class KEY_CONFIG, class COMPARATOR, class VALUES>
int verifyListContents(Link              *containerList,
                       const COMPARATOR&  compareKeys,
                       const VALUES&      expectedValues,
                       size_t             expectedSize)
    // Verify the specified 'containerList' has the specified 'expectedSize'
    // number of elements, and contains the same values as the array in the
    // specified 'expectedValues', and that the elements in the list are
    // arranged so that elements whose keys compare equal using the specified
    // 'compareKeys' predicate are all arranged contiguously within the list.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    // Check to avoid creating an array of length zero.
    if (0 == containerList) {
        ASSERTV(0 == expectedSize);
        return 0;                                                     // RETURN
    }

    BoolArray foundValues(expectedSize);
    size_t i = 0;
    for (Link *cursor = containerList;
         cursor;
         cursor = cursor->nextLink(), ++i)
    {
        const ValueType& element = ImpUtil::extractValue<KEY_CONFIG>(cursor);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);
        size_t j = 0;
        do {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                ASSERTV(j, expectedValues[j], element, !foundValues[j]);
                foundValues[j] = true;
                break;
            }
        }
        while (++j != expectedSize);
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return -2;                                                    // RETURN
    }

    size_t missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            ++missing;
        }
    }

    if (missing > 0) {
        return missing;                                               // RETURN
    }

    // All elements are present, check the contiguity requirement
    // Note that this test is quadratic in the length of the list, although we
    // will optimize for the case of duplicates actually occurring.
    for (Link *cursor = containerList; cursor; cursor = cursor->nextLink()) {
        const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(cursor);

        Link *next = cursor->nextLink();
        // Walk to end of key-equivalent sequence
        while (next &&
               compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            cursor = next;
            next   = next->nextLink();
        }

        // Check there are no more equivalent keys in the list
        while (next &&
               !compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            next = next->nextLink();
        }

        if (0 != next) {
            return -3; // code for discontiguous list                 // RETURN
        }
    }

    return 0;  // 0 indicates a successful test!
}
#if 0
// This may be brought back as an optimized 'verifyListContents' that can be
// used for test cases subsequent to some to-be-determined test case higher
// than 10.
template<class KEY_CONFIG, class CONTAINER, class VALUES>
int verifyContainer(const CONTAINER& container,
                    const VALUES&    expectedValues,
                    size_t           expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    typedef typename KEY_CONFIG::ValueType ValueType;

    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if(expectedSize != container.size()) {
        return -1;                                                    // RETURN
    }

    // Check to avoid creating an array of length zero.
    if (0 == expectedSize) {
        ASSERTV(0 == container.size());
        return 0;                                                     // RETURN
    }

    BoolArray foundValues(container.size());
    size_t i = 0;
    for (Link *it = container.elementListRoot(); it; it = it->nextLink(),++i) {
        const ValueType& element = ImpUtil::extractKey<KEY_CONFIG>(it);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);
        size_t j = 0;
        do {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                ASSERTV(j, expectedValues[j], element, !foundValues[j]);
                foundValues[j] = true;
                break;
            }
        }
        while (++j != container.size());
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return -2;                                                    // RETURN
    }

    size_t missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            ++missing;
        }
    }

    return missing;  // 0 indicates a successful test!
}
#endif

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

bool g_enableTestEqualityComparator = true;
bool g_enableTestHashFunctor = true;

template <class TYPE, int GROUP_SIZE>
class GroupedEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE', but in a
    // way that multiple distinct value will compare equal with each other.  The
    // function-call operator compares the integer returned by the class method
    // 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // divided by 'GROUP_SIZE' (rounded down) is equal to than integer
        // representation of the specified 'rhs' divided by 'GROUP_SIZE'.
    {
        int leftValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
                    / GROUP_SIZE;
        int rightValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs)
                        / GROUP_SIZE;
        return leftValue == rightValue;
    }
};

template <class TYPE, class HASHER, int GROUP_SIZE>
class GroupedHasher : private HASHER{
    // This test class provides a mechanism that defines a function-call
    // operator that returns the same hash for multiple values.  The
    // function-call operator invoke the (template parameter) type 'HASHER' on
    // the integer returned by the class method
    // 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.  'HASHER' shall be a class that can be invoked as
    // if it has the following method:
    //..
    //  int operator()(int value);
    //..

  public:
    // ACCESSORS
    int operator() (const TYPE& value) const
        // Return the hash value of the integer representation of the specified
        // 'value' divided by 'GROUP_SIZE' (rounded down) is equal to than
        // integer representation of the specified 'rhs' divided by
        // 'GROUP_SIZE'.
    {
        int groupNum = bsltf::TemplateTestFacility::getIdentifier<TYPE>(value)
                     / GROUP_SIZE;
        return ((HASHER *)this)->operator()(groupNum);
    }
};


                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableTestEqualityComparator = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableTestEqualityComparator = true;
    }

    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // MANIPULATORS
    void setId(int value)
    {
        d_id = value;
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableTestEqualityComparator) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator== (const TestEqualityComparator& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableTestHashFunctor = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableTestHashFunctor = true;
    }

    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableTestHashFunctor) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator== (const TestHashFunctor& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class KEY>
class StatefulHash : bsl::hash<KEY> {
    typedef bsl::hash<KEY> Base;

    template <class OTHER_KEY>
    friend
    bool operator==(const StatefulHash<OTHER_KEY>& lhs,
                    const StatefulHash<OTHER_KEY>& rhs);

  private:
    // DATA
    native_std::size_t d_mixer;

  public:
    explicit StatefulHash(native_std::size_t mixer = 0xffff)
    : d_mixer(mixer)
    {
    }

    void setMixer(int value)
    {
        d_mixer = value;
    }

    native_std::size_t operator()(const KEY& k) const
    {
        return Base::operator()(k) ^ d_mixer;
    }
};

template <class KEY>
bool operator==(const StatefulHash<KEY>& lhs, const StatefulHash<KEY>& rhs)
{
    return lhs.d_mixer == rhs.d_mixer;
}

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestFacilityHasher : public HASHER { // exploit empty base
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.  The function-call operator is implemented by calling the wrapper
    // functor, 'HASHER', with integers converted from objects of 'KEY' by the
    // class method 'TemplateTestFacility::getIdentifier'.

  public:
    TestFacilityHasher(const HASHER& hash = HASHER())               // IMPLICIT
    : HASHER(hash)
    {
    }

    // ACCESSORS
    native_std::size_t operator() (const KEY& k) const
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
    {
        return HASHER::operator()(
                           bsltf::TemplateTestFacility::getIdentifier<KEY>(k));
    }
};

void setHasherState(bsl::hash<int> *hasher, int id)
{
    (void) hasher;
    (void) id;
}

void setHasherState(StatefulHash<int> *hasher, int id)
{
    hasher->setMixer(id);
}

bool isEqualHasher(const bsl::hash<int>&, const bsl::hash<int>&)
    // Provide an overloaded function to compare comparators.  Return 'true'
    // because 'bsl::hash' is stateless.
{
    return true;
}

bool isEqualHasher(const StatefulHash<int>& lhs,
                   const StatefulHash<int>& rhs)
    // Provide an overloaded function to compare comparators.  Return 'true'
    // because 'bsl::equal_to' is stateless.
{
    return lhs == rhs;
}

template <class KEY>
void setComparatorState(bsl::equal_to<KEY> *comparator, int id)
{
    (void) comparator;
    (void) id;
}

template <class KEY>
void setComparatorState(TestEqualityComparator<KEY> *comparator, int id)
{
    comparator->setId(id);
}


template <class KEY>
bool isEqualComparator(const bsl::equal_to<KEY>&, const bsl::equal_to<KEY>&)
    // Provide an overloaded function to compare comparators.  Return 'true'
    // because 'bsl::equal_to' is stateless.
{
    return true;
}

template <class KEY>
bool isEqualComparator(const TestEqualityComparator<KEY>& lhs,
                       const TestEqualityComparator<KEY>& rhs)
    // Provide an overloaded function to compare comparators.  Return
    // 'lhs == rhs'.
{
    return lhs == rhs;
}

// test support function
template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
                  bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
                  const typename KEY_CONFIG::ValueType&              value)
    // Insert an element into the specified 'hashTable' and return the address
    // of the new node, unless the insertion would cause the hash table to
    // exceed its 'maxLoadFactor' and rehash, in which case return a null
    // pointer value.
{
    BSLS_ASSERT(hashTable);
    if ((hashTable->size() + 1.0) / hashTable->numBuckets()
        > hashTable->maxLoadFactor() ) {
        return 0;
    }
    return hashTable->insert(value);
}

template <class KEY_CONFIG, class HASHER>
bool isValidHashTable(bslalg::BidirectionalLink      *listRoot,
                      const bslalg::HashTableBucket&  arrayRoot,
                      native_std::size_t              arrayLength)
{
    // We perform the const-cast inside this function as we know:
    // i/  The function we call does not make any writes to the bucket array.
    // ii/ It is much simpler to cast in this one place than in each of our
    //     call sites.
    bslalg::HashTableAnchor anchor(
                             const_cast<bslalg::HashTableBucket *>(&arrayRoot),
                             arrayLength,
                             listRoot);
    return bslalg::HashTableImpUtil::isWellFormed<KEY_CONFIG, HASHER>(anchor);
}

#if 0
// code copied from bslstl_set.t.cpp for inspiration later
// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
         < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
}
#endif

}  // close unnamed namespace

// ============================================================================
//                         TEST DRIVER HARNESS
// ----------------------------------------------------------------------------
#if 0
template<class KEY, class MAPPED>
struct TestMapKeyPolicy
{
    typedef KEY KeyType;
    typedef MAPPED MappedType;
    typedef bsl::pair<const KEY, MAPPED> ValueType;

    static const KEY& extractKey(const ValueType& value) {
        return value.first;
    }

//    static const ValueType& extractMappedValue(const ValueType& value) {
//        return value.second;
//    }
        // This method does not appear to be used, YET.
};
#endif
                       // =========================
                       // class CharToPairConverter
                       // =========================

#if defined(SPAC)
template <class KEY, class VALUE>
class CharToPairConverter {
    // Convert a 'char' value to a 'bsl::pair' of the parameterized 'KEY' and
    // 'VALUE' type.

  public:
    std::pair<const KEY, VALUE> operator()(char value)
    {
        // Use different values for 'KEY' and 'VALUE'

        return bsl::pair<const KEY, VALUE> (
                bsltf::TemplateTestFacility::create<KEY>(value),
                bsltf::TemplateTestFacility::create<VALUE>(value - 'A' + '0'));
    }
};
#endif

template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'HashTable'
    // container.  The parameterized 'KEY_CONFIG', 'HASHER', 'COMPARATOR' and
    // 'ALLOCATOR' specifies the configuration, hasher type, comparator type
    // and allocator type respectively.  Each "testCase*" method tests a
    // specific aspect of
    // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.  Each test case
    // should be invoked with various parameterized type to fully test the
    // container.

  private:
    // TYPES
    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR> Obj;
        // Type under testing.

    typedef typename Obj::SizeType   SizeType;
    typedef typename Obj::KeyType    KeyType;
    typedef typename Obj::ValueType  ValueType;
        // Shorthands

#if defined(SPAC)
    typedef bsltf::TestValuesArray<ValueType,
                          CharToPairConverter<KeyType, ValueType> > TestValues;
#else
    typedef bsltf::TestValuesArray<ValueType> TestValues;
#endif
  public:
    typedef bsltf::StdTestAllocator<ValueType> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'map<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

  public:
    // TEST CASES
    static void testCase7();

    static void testCase6();

    static void testCase4();

    static void testCase3();

    static void testCase2();

    static void testCase1(KeyType                        *testKeys,
                          typename KEY_CONFIG::ValueType *testValues,
                          size_t                          numValues);
};

template <class ELEMENT>
struct BasicKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value;
    }
};

template <class ELEMENT>
struct TestDriver_BasicConfiguation {
    typedef TestDriver< BasicKeyConfig<ELEMENT>
                      , TestFacilityHasher<ELEMENT>
                      , ::bsl::equal_to<ELEMENT>
                      , ::bsl::allocator<ELEMENT>
                      > Type;

    // TEST CASES
    static void testCase7() { Type::testCase7(); }

    static void testCase4() { Type::testCase4(); }

    static void testCase3() { Type::testCase3(); }

    static void testCase2() { Type::testCase2(); }
};

template <class ELEMENT>
struct TestDriver_StatefulConfiguation {
    typedef TestDriver< BasicKeyConfig<ELEMENT>
                      , TestFacilityHasher<ELEMENT, StatefulHash<int> >
                      , TestEqualityComparator<ELEMENT>
                      , ::bsl::allocator<ELEMENT>
                      > Type;

    // TEST CASES
    static void testCase7() { Type::testCase7(); }

    static void testCase4() { Type::testCase4(); }

    static void testCase3() { Type::testCase3(); }

    static void testCase2() { Type::testCase2(); }
};

template <class ELEMENT>
struct TestCase6_Configuration {
    // Test case 6 (equality comparator) must be run with
    // 'GroupedEqualityComparator'.

    typedef TestDriver< BasicKeyConfig<ELEMENT>
                      , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                      , GroupedEqualityComparator<ELEMENT, 5>
                      , ::bsl::allocator<ELEMENT>
                      > Type;

    // TEST CASES
    static void testCase6() { Type::testCase6(); }
};


                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
int TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ggg(
                                                           Obj        *object,
                                                           const char *spec,
                                                           int         verbose)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
//            object->insert(VALUES[spec[i] - 'A']);
            insertElement(object, VALUES[spec[i] - 'A']);
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::gg(Obj        *object,
                                                          const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //
    // Concern:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'push_back'.  Using the 'operator!=' verify that y differs from x and
    //:   w, and verify that the capacity of y changes correctly.  (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   multimap(const multimap& original);
    //   multimap(const multimap& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    typedef typename KEY_CONFIG::KeyType KEY;
    typedef typename KEY_CONFIG::ValueType VALUE;

    const TestValues VALUES;

    const int TYPE_ALLOC =
         bslalg::HasTrait<KEY, bslalg::TypeTraitUsesBslmaAllocator>::VALUE
         + bslalg::HasTrait<VALUE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A"
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ"
        };

        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length %d:\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW(HASHER(), COMPARATOR(), LENGTH);
            const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(HASHER(), COMPARATOR(), LENGTH, &oa);
            const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(HASHER(), COMPARATOR(), LENGTH, &oa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W, Y0, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Link *RESULT = Y1.insert(VALUES['Z' - 'A']);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, 0 != RESULT);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                bslma::TestAllocatorMonitor oam(&oa);

                Obj Y11(X, &oa);

                if (LENGTH == 0) {
                    ASSERTV(SPEC, oam.isTotalSame());
                    ASSERTV(SPEC, oam.isInUseSame());
                }
                else {
                    //const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    //ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    //ASSERTV(SPEC, B, A, B + 1 + TYPE_ALLOCS ==  A);
                    ASSERTV(SPEC, oam.isTotalUp());
                    ASSERTV(SPEC, oam.isInUseUp());
                }

                oam.reset(&oa);

                Y11.removeAll();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, oam.isTotalSame());

                ASSERTV(SPEC, 0 == Y11.size());
                if (LENGTH != 0) {
                    ASSERTV(SPEC, W != Y11);
                    ASSERTV(SPEC, X != Y11);
                }
                ASSERTV(SPEC, Y11.allocator() == X.allocator());
            }
            {   // Exception checking.

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.allocator() == X.allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (LENGTH == 0) {
                    ASSERTV(SPEC, oam.isTotalSame());
                    ASSERTV(SPEC, oam.isInUseSame());
                }
                else {
                    ASSERTV(SPEC, oam.isInUseSame());
                }
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes (i.e., 'allocator') participate.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 'operator==' is defined in terms of 'operator==(KEY)' instead of the
    //:   supplied comparator function.
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //:
    //:14 Two object with the same elements in different permutation compare
    //:   equal to each other.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a"scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create two (object) allocators, 'oax' and 'oay'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2'.
    //:
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const map<K, C, A>& lhs, const map<K, C, A>& rhs);
    //   bool operator!=(const map<K, C, A>& lhs, const map<K, C, A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        using namespace bslstl;

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void)operatorEq;  // quash potential compiler warnings
        (void)operatorNe;
    }

    static const struct {
        int         d_line;   // source line number
        int         d_index;  // lexical order
        const char *d_spec;   // specification string, for input to 'gg'
    } DATA[] = {
        //line idx  spec
        //---- ---  --------
        { L_,    0, "" },
        { L_,    1, "A" },
        { L_,    2, "AA" },
        { L_,    3, "AB" },
        { L_,    4, "ABC" },
        { L_,    4, "BAC" },
        { L_,    4, "BCA" },
        { L_,    5, "ABD" },
        { L_,    6, "ABCDE" },
        { L_,    6, "CEBAD" },
        { L_,    7, "AAAAA" },
        { L_,    8, "ABCDA" },
        { L_,    9, "ABCDF" },
        { L_,    9, "FDCBA" },
        { L_,   10, "AFKPUZ" },
        { L_,   11, "ABFGKLPQUVZ" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(SPEC1);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(HASHER(), COMPARATOR(), LENGTH1, &scratch);
                const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(SPEC2);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(HASHER(), COMPARATOR(), LENGTH1, &xa);
                    const Obj& X = gg(&mX, SPEC1);
                    Obj mY(HASHER(), COMPARATOR(), LENGTH2, &ya);
                    const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    //TestEqualityComparator<KEY>::disableFunctor();

                    ASSERTV(LINE1, LINE2, X, Y, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    //TestEqualityComparator<KEY>::enableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - elementListRoot
    //     - size
    //     - allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 'elementListRoot' refers to the root of a list with exactly 'size()'
    //:   elements, and a null pointer value if 'size() == 0'.
    //:
    //: 5 'bucketAtIndex' returns a valid bucket for all 0 <= index <
    //:   'numBuckets'.
    //:
    //: 6 QoI: Assert precondition violations for 'bucketAtIndex' when
    //:   'size <= index' are detected in SAFE builds.
    //:
    //: 7 For any value of key, 'bucketIndexForKey' returns a bucket number
    //:   less than 'numBuckets'.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Value construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'allocator' method.
    //:
    //:     3 Verify the object all attributes are as expected.
    //:
    //:     4 Use 'verifyListContents' to validate the list rooted at
    //:       'elementListRoot'.
    //:
    //:     5 TBD: Use 'validateBucket to validate the buckets returned by
    //:       'bucketAtIndex'.
    //:
    //:     6 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //*  allocator() const;
    //*  comparator() const;
    //*  hasher() const;
    //*  size() const;
    //*  numBuckets() const;
    //*  maxLoadFactor() const;
    //*  elementListRoot() const;
    //*  bucketAtIndex(SizeType index) const;
    //*  bucketIndexForKey(const KeyType& key) const;
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;           // source line number
        const char *d_spec;           // specification string
        float       d_maxLoadFactor;  // max load factor
        size_t      d_numBuckets;     // number of buckets
        const char *d_results;        // expected results
    } DATA[] = {
        //line  spec                 result
        //----  --------             ------
        { L_,   "",       1.0,   1,  ""       },
        { L_,   "A",      0.9,   2,  "A"      },
        { L_,   "AB",     0.8,   3,  "AB"     },
        { L_,   "ABC",    0.7,   5,  "ABC"    },
        { L_,   "ABCD",   0.6,   8,  "ABCD"   },
        { L_,   "ABCDE",  0.5,  13,  "ABCDE"  }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const COMPARATOR EQUAL = COMPARATOR();

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE            = DATA[ti].d_line;
            const char *const SPEC            = DATA[ti].d_spec;
            const int         LENGTH          = strlen(DATA[ti].d_results);
            const float       MAX_LOAD_FACTOR = DATA[ti].d_maxLoadFactor;
            const size_t      NUM_BUCKETS     = DATA[ti].d_numBuckets;
            const TestValues  EXP(DATA[ti].d_results);

            HASHER hash;
            setHasherState(&hash, ti);
            COMPARATOR comp;
            setComparatorState(&comp, ti);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",    veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint",  veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(hash, comp, NUM_BUCKETS);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(hash,
                                            comp,
                                            NUM_BUCKETS,
                                            (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(hash, comp, NUM_BUCKETS, &sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(hash, comp, NUM_BUCKETS, &sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                mX.setMaxLoadFactor(MAX_LOAD_FACTOR);

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.allocator());
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualComparator(comp, X.comparator()));
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualHasher(hash, X.hasher()));
                ASSERTV(LINE, SPEC, CONFIG, NUM_BUCKETS <= X.numBuckets());
                ASSERTV(LINE, SPEC, CONFIG,
                        MAX_LOAD_FACTOR == X.maxLoadFactor());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == (int)X.size());

                ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           EQUAL,
                                                           EXP,
                                                           LENGTH));

                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);


        if (veryVerbose) printf("\t'bucketAtIndex'\n");
        {
            Obj mX(HASHER(), COMPARATOR(), 1);  const Obj& X = mX;
            int numBuckets = X.numBuckets();
            ASSERT_SAFE_PASS(X.bucketAtIndex(numBuckets - 1));
            ASSERT_SAFE_FAIL(X.bucketAtIndex(numBuckets));
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //:
    //: 3 'verifyListContents' confirms there is a one-to-one mapping between
    //:   the supplied list and the expected values array, or both are empty.
    //:
    //: 4 'isValidHashTable' returns 'true' if the supplied arguments can
    //:   create a well-formed hash table anchor, and 'false' otherwise.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //*  int ggg(HashTable *object, const char *spec, int verbose = 1);
    //*  HashTable& gg(HashTable *object, const char *spec);
    //*  verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    //*  bool isValidHashTable(Link *, const HashTableBucket&, int numbucket);
    // ------------------------------------------------------------------------

#if 0
    bslma::TestAllocator oa(veryVeryVerbose);

    const COMPARATOR EQUAL = COMPARATOR();

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj mY(&oa);
            const Obj& Y = gg(&mY, SPEC);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
                T_ T_ T_ P(Y);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                       EQUAL,
                                                       EXP,
                                                       LENGTH));
            ASSERT(0 == verifyListContents<KEY_CONFIG>(Y.elementListRoot(),
                                                       EQUAL,
                                                       EXP,
                                                       LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },
            { L_,   "E",       -1,    }, // control
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = (int)strlen(SPEC);

            Obj mX(&oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                 ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that a 'HashTable' object can be constructed into
    //   a (valid) default state, then through use of manipulators brought into
    //   any other valid state default constructor, and finally that the object
    //   destroys all its elements and leaks no memory on destruction.  For the
    //   purposes of testing, the default state will be a 'HashTable' having no
    //   elements, having default constructed hasher, comparator and allocator,
    //   and initially having no buckets.  The primary manipulators will be a
    //   free function that inserts an element of a specific type (created for
    //   the purpose of testing) and the 'removeAll' method.
    //
    // Concerns:
    //: 1 An object created with the value constructor (with or without a
    //:   supplied allocator) has the supplied hasher, comparator, at least the
    //:   initial number of buckets and allocator.
    //:
    //: 2 The number of buckets is 1 or a prime number.
    //:
    //: 3 If the allocator is a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default allocator in effect at
    //:   the time of construction becomes the object allocator for the
    //:   resulting object.
    //:
    //: 4 If the allocator is not a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default constructed allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 5 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 6 If the allocator is a 'bsl::allocator', supplying a null allocator
    //:   address has the same effect as not supplying an allocator.
    //:
    //: 7 Supplying an allocator to the value constructor has no effect on
    //:   subsequent object values.
    //:
    //: 8 Any memory allocation is from the object allocator.
    //:
    //: 9 There is no temporary allocation from any allocator.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //:
    //:11 QoI: The value constructor allocates no memory if the initial number
    //:   of bucket is 0.
    //:
    //:12 'insertElement' increase the size of the object by 1.
    //:
    //:13 'insertElement' returns the address of the newly added element.
    //:
    //:14 'insertElement' puts the element into the list of element defined by
    //:   'elementListRoot'.
    //:
    //:15 'insertElement' adds an additional element in the bucket returned by
    //:   the 'bucketFromKey' method.
    //:
    //:16 'insertElement' returns a null pointer if adding one more element
    //:   will exceed the 'maxLoadFactor'.
    //:
    //:17 Elements having the same keys (retrieved from the 'extractKey' method
    //:   of the KEY_CONFIG) according to the supplied comparator are inserted
    //:   contiguously of the beginning of the range of existing equivalent
    //:   elements, without changing their relative order.
    //:
    //:18 'removeAll' properly destroys each contained element value.
    //:
    //:19 'removeAll' does not allocate memory.
    //:
    //:20 'setBootstrapMaxLoadFactor' modifies the 'maxLoadFactor' attribute
    //:   unless the supplied value is less than or equal to 'loadFactor'.
    //:
    //:21 'setBootstrapMaxLoadFactor' returns 'true' if it successessfully
    //:   changes the 'maxLoadFactor', and 'false' otherwise.
    //:
    //:22 Any argument can be 'const'.
    //:
    //:23 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'get_allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presense of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 14..15)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-12..13)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //:
    //:    11 Insert 'L' distinct elements and record the iterators returned.
    //:
    //:    12 Insert the same 'L' elements again and verify that incrementing
    //:       the iterators returned gives the iterator to the next smallest
    //:       value.
    //:
    //:    13 Perform P-1.2.12 again.  (C-11)
    //
    //
    // Testing:
    //*  HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOC&)
    //   ~HashTable();
    //   insertElement  (test driver function, proxy for basic manipulator)
    //   void removeAll();
    //   setBootstrapMaxLoadFactor();
    // ------------------------------------------------------------------------

#if 0
    typedef typename KEY_CONFIG::ValueType     Element;

    const bool VALUE_TYPE_USES_ALLOC  =
         bslalg::HasTrait<Element, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj((bslma::Allocator *)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
            ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(LENGTH, CONFIG, 0 == bucket.last());

            // Verify that remove-all on a default container has no effect.
            // Specifically, no memory allocated, and the root of list and
            // bucket array are unchanged.

            mX.removeAll();

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
            ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket2 = X.bucketAtIndex(0);
            ASSERTV(LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(LENGTH, CONFIG, 0 == bucket.last());

            ASSERTV(LENGTH, CONFIG, &bucket == &bucket2);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting 'insertElement' (bootstrap function).\n");
            }
            if (0 < LENGTH) {
                if (verbose) {
                  printf("\t\tOn an object of initial length "ZU".\n", LENGTH);
                }

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    const typename Obj::SizeType bucketCount = X.numBuckets();

                    bslma::TestAllocatorMonitor tam(&oa);
                    Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);

                    bool BUCKET_ARRAY_GREW = bucketCount != X.numBuckets();
#if 0
                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
                    if (VALUE_TYPE_USES_ALLOC || BUCKET_ARRAY_GREW) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }

                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    // BROKEN TEST CONDITION
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

                    ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
#endif

                    ASSERTV(LENGTH, CONFIG,
                            VALUES[LENGTH - 1] ==
                                      ImpUtil::extractKey<KEY_CONFIG>(RESULT));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundValues = new int[X.size()];
                    for (typename Obj::SizeType j = 0;j != X.size(); ++j) {
                        foundValues[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        size_t j = 0;
                        do {
                            if (VALUES[j] ==
                                         ImpUtil::extractKey<KEY_CONFIG>(it)) {
                                ASSERTV(LENGTH, CONFIG, VALUES[j],
                                        !foundValues[j]);
                                ++foundValues[j];
                            }
                        }
                        while (++j != X.size());
                    }
                    size_t missing = 0;
                    for (typename Obj::SizeType j = 0; j != X.size(); ++j) {
                        if (!foundValues[j]) { ++missing; }
                    }
                    ASSERTV(LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundValues;

                    ASSERTV(LENGTH, CONFIG, X.size() == i);
                }

            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.removeAll();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, 0 < X.numBuckets());
                ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
                ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            const typename Obj::SizeType bucketCount = X.numBuckets();

            if (veryVerbose) { printf(
                  "\n\tRepeat testing 'insertElement', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (verbose) {
                  printf("\t\tOn an object of initial length "ZU".\n", LENGTH);
                }

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    bslma::TestAllocatorMonitor tam(&oa);
                    Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);

                    // The number of buckets should not have changed, so no
                    // reason to allocate a fresh bucket array
                    ASSERTV(LENGTH, CONFIG, bucketCount, X.numBuckets(),
                            bucketCount == X.numBuckets());

                    // These tests assume that the object allocator is used
                    // only if stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
                    // Hence we run the same test sequence a second time after
                    // clearing the container, so we can validate knowing that
                    // no rehashes should be necessary, and will in fact show
                    // up as a memory use error.
                    if (VALUE_TYPE_USES_ALLOC) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }

#if 0
                    ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                                            oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
#endif

                    ASSERTV(LENGTH, CONFIG,
                            VALUES[LENGTH - 1] ==
                                      ImpUtil::extractKey<KEY_CONFIG>(RESULT));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundValues = new int[X.size()];
                    for (typename Obj::SizeType j = 0;j != X.size(); ++j) {
                        foundValues[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        size_t j = 0;
                        do {
                            if (VALUES[j] ==
                                         ImpUtil::extractKey<KEY_CONFIG>(it)) {
                                ASSERTV(LENGTH, CONFIG, VALUES[j],
                                        !foundValues[j]);
                                ++foundValues[j];
                            }
                        }
                        while (++j != X.size());
                    }
                    size_t missing = 0;
                    for (typename Obj::SizeType j = 0; j != X.size(); ++j) {
                        if (!foundValues[j]) { ++missing; }
                    }
                    ASSERTV(LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundValues;

                    ASSERTV(LENGTH, CONFIG, X.size() == i);
                }

            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.removeAll();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, bucketCount == X.numBuckets());
                ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
                ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
            {
                Link *ITER[MAX_LENGTH + 1];

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                            VALUES[tj] ==
                                    ImpUtil::extractKey<KEY_CONFIG>(ITER[tj]));
                }
                ITER[LENGTH] = 0;

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    RESULT = RESULT->nextLink();
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 2 * LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    RESULT = RESULT->nextLink();
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            const typename Obj::SizeType bucketCountWithDups = X.numBuckets();

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.removeAll();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, bucketCountWithDups == X.numBuckets());
                ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
                ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tRetesting 'insert' duplicated values.\n"); }
            {
                Link *ITER[MAX_LENGTH + 1];

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                            VALUES[tj] ==
                                    ImpUtil::extractKey<KEY_CONFIG>(ITER[tj]));
                }
                ITER[LENGTH] = 0;

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    RESULT = RESULT->nextLink();
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 2 * LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG,
                        VALUES[tj] == ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    RESULT = RESULT->nextLink();
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.removeAll();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, bucketCountWithDups == X.numBuckets());
                ASSERTV(LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(LENGTH, CONFIG, 1.0f == X.maxLoadFactor());
                ASSERTV(LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase1(
                                    KeyType                        *testKeys,
                                    typename KEY_CONFIG::ValueType *testValues,
                                    size_t                          numValues)
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Execute each methods to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------


    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR> Obj;
    typedef typename Obj::ValueType  Value;
    typedef bsl::pair<Link *, bool>  InsertResult;

    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Construct an empty HashTable.\n");
    }
    {
        // Note that 'HashTable' does not have a default constructor, so we
        // must explicitly supply a default for each attribute.
        Obj x(HASHER(), COMPARATOR(), 0, &objectAllocator); const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(0    <  X.maxSize());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == objectAllocator.numBytesInUse());
    }

   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

   if (veryVerbose) {
       printf("Test use of allocators.\n");
   }
   {
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       Obj o1(HASHER(), COMPARATOR(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
//           o1.insert(Value(testKeys[i], testValues[i]));
           o1.insert(Value(testKeys[i]));
       }
       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());
   }
   {
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       Obj o1(HASHER(), COMPARATOR(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
           bool isInsertedFlag = false;
           o1.insertIfMissing(&isInsertedFlag, Value(testKeys[i]));
           ASSERTV(isInsertedFlag, true == isInsertedFlag);
       }
       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());
   }
   {
#if defined(TESTING_PAIR_FOR_MAP)
       bslma::TestAllocator objectAllocator1("objectAllocator1");
       bslma::TestAllocator objectAllocator2("objectAllocator2");

       if(veryVerbose) printf("Use a different allocator\n");
       Obj o1(HASHER(), COMPARATOR(), 0, &objectAllocator1); const Obj& O1 = o1;
       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

       for (size_t i = 0; i < numValues; ++i) {
           o1.insertIfMissing(testKeys[i]);
       }

       ASSERTV(numValues == O1.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());
       ASSERTV(0 == objectAllocator2.numBytesInUse());


       if(veryVerbose) printf("Use a different allocator\n");
       {
           bslma::TestAllocatorMonitor monitor(&objectAllocator1);
           Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
           ASSERTV(&objectAllocator2 == O2.allocator().mechanism());
           ASSERTV(monitor.isInUseSame());
           ASSERTV(monitor.isTotalSame());
           ASSERTV(0 <  objectAllocator1.numBytesInUse());
           ASSERTV(0 <  objectAllocator2.numBytesInUse());
       }
       ASSERTV(0 ==  objectAllocator2.numBytesInUse());


       if(veryVerbose) printf("Copy construct O2(O1)\n");

       Obj o2(O1, &objectAllocator1); const Obj& O2 = o2;

       ASSERTV(&objectAllocator1 == O2.allocator().mechanism());

       ASSERTV(numValues == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       if(veryVerbose) printf("Default construct O3 and swap with O1\n");
       Obj o3(HASHER(), COMPARATOR(), 0, &objectAllocator1); const Obj& O3 = o3;
       ASSERTV(&objectAllocator1 == O3.allocator().mechanism());

       ASSERTV(numValues == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(0         == O3.size());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
       o1.swap(o3);
       ASSERTV(0         == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(numValues == O3.size());
       ASSERTV(monitor1.isInUseSame());
       ASSERTV(monitor1.isTotalSame());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       if(veryVerbose) printf("swap O3 with O2\n");
       o3.swap(o2);
       ASSERTV(0         == O1.size());
       ASSERTV(numValues == O2.size());
       ASSERTV(numValues == O3.size());
       ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                        // on implementation
       ASSERTV(!monitor1.isTotalUp());
       ASSERTV(0 <  objectAllocator1.numBytesInUse());

       ASSERTV(&objectAllocator1 == O1.allocator().mechanism());
       ASSERTV(&objectAllocator1 == O2.allocator().mechanism());
       ASSERTV(&objectAllocator1 == O3.allocator().mechanism());
#endif
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testKeys, testKeys + numValues);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(HASHER(), COMPARATOR(), 0, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            Obj z(X, &objectAllocator); const Obj& Z = z;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, 0 == X.find(testKeys[i]));

            // Test 'insert'.
            Value value(testKeys[i]);
            bool isInsertedFlag = false;
            Link *link = x.insertIfMissing(&isInsertedFlag, value);
            ASSERTV(0             != link);
            ASSERTV(true          == isInsertedFlag);
            ASSERTV(testKeys[i]   == ImpUtil::extractKey<KEY_CONFIG>(link));
            ASSERTV(Value(testKeys[i]) ==
                                      ImpUtil::extractValue<KEY_CONFIG>(link));

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(0 != X.size());

            // Test insert duplicate key
            ASSERTV(link    == x.insertIfMissing(&isInsertedFlag, value));
            ASSERTV(false   == isInsertedFlag);;
            ASSERTV(i + 1   == X.size());

            // Test find, operator[], at.
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(link) ==
                         ImpUtil::extractKey<KEY_CONFIG>(X.find(testKeys[i])));
            // ASSERTV(testValues[i] == x[testKeys[i]]);
            // ASSERTV(testValues[i] == x.at(testKeys[i]));
            // ASSERTV(testValues[i] == X.at(testKeys[i]));

#if 0
            // This test is supported only for 'std::pair' elements
            // Test insertIfMissing
            ASSERTV(!(X == Z));
            ASSERTV(  X != Z);
            const Value& V = ImpUtil::extractValue<KEY_CONFIG>(
                                           z.insertIfMissing(testKeys[i]));
            ASSERTV(Value(testKeys[i], typename KEY_CONFIG::MappedType()) == V);
            //z[testKeys[i]] = testValues[i];
            //ASSERTV(testValues[i] == z[testKeys[i]]);
            //ASSERTV( (X == Z));
            //ASSERTV(!(X != Z));
#endif

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());

        // Use erase(iterator) on all the elements.
        for (size_t i = 0; i < numValues; ++i) {
            Link *it     = x.find(testKeys[i]);
            Link *nextIt = it->nextLink();

            ASSERTV(0       != it);
            ASSERTV(testKeys[i]   == ImpUtil::extractKey<KEY_CONFIG>(it));
            ASSERTV(Value(testKeys[i]) ==
                                        ImpUtil::extractValue<KEY_CONFIG>(it));
            Link *resIt = x.remove(it);
            ASSERTV(resIt == nextIt);

            Link *resFind = x.find(testKeys[i]);
            ASSERTV(0 == resFind);

            ASSERTV(numValues - i - 1 == X.size());
        }
    } while (native_std::next_permutation(testKeys,
                                          testKeys + numValues));
//
//    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


    native_std::random_shuffle(testKeys,  testKeys + numValues);
    if (veryVerbose) {
        printf("Test 'remove(bslalg::BidirectionalLink *)'.\n");
    }
    {
        Obj x(HASHER(), COMPARATOR(), 0, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Value value(testKeys[i]);
            Link *result1 = x.insert(value);
            ASSERTV(0 != result1);
            Link *result2 = x.insert(value);
            ASSERTV(0 != result2);
            ASSERTV(result1 != result2);
            ASSERTV(2 * (i + 1) == X.size());

            Link *start;
            Link *end;
            x.findRange(&start, &end, testKeys[i]);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start) == testKeys[i]);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start->nextLink()) ==
                                                                  testKeys[i]);
            ASSERTV(start->nextLink()->nextLink() == end);
        }

        for (size_t i = 0; i < numValues; ++i) {
            KeyType key = ImpUtil::extractKey<KEY_CONFIG>(x.elementListRoot());
            Link *resIt1 = x.remove(x.elementListRoot());
            ASSERTV(x.find(key) == resIt1);
            ASSERTV(X.size(), (2 * numValues - (2 * (i + 1) - 1)) == X.size());
            Link *resIt2 = x.remove(x.elementListRoot());
            ASSERTV( x.find(key) == 0);
            ASSERTV(X.size(), (2 * numValues - 2 * (i + 1)) == X.size());
        }
    }

    if (veryVerbose) {
        printf("Test 'equal' and 'hasher'\n");
    }

}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
#if 0  // Planned test cases, not yet implemented
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'g'"
                            "\n===========\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
#endif
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestCase6_Configuration,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);


        // Further, need to validate the basic test facilities:
        //   verifyListContents
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BOOTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

//            typedef TestMapKeyPolicy<int, int> TestMapIntKeyPolicy;
            typedef BasicKeyConfig<int> TestMapIntKeyPolicy;

            TestDriver<TestMapIntKeyPolicy,
                       TestIntHash,
                       TestIntEqual,
                       StlTestIntAllocator >::testCase1(INT_VALUES,
                                                        INT_VALUES,
                                                        NUM_INT_VALUES);
        }
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
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
