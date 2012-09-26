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
// Windows, we use a macro and string literal concatenion to produce the
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
// semantic, so this early testing will be limitted to only those types that
// provide a full range of required behavior; testing of non-value semantic
// elements, or awkward hash and compare functors, will be deferred past the
// initial 10 cases.
// To establish value semnatics, we will test the following class members, and
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
//     insertValue        - a test function using 'insert' resticted
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
// satisfy contraints implied by all these additional moving parts, and will
// all be established in test cases following the value-semantic test sequence.
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//  Chief concerns are that operator() is callable by const-objects, accepts
//  const-references to arguments of the user-specified type, calls the
//  correctly overloaded operator, has the standard-mandated typedefs (but not
//  the base class, not required by C++11 spec) and is both bitwise movable
//  and bitwise copyable.
//  As QoI the class should be an empty class, and no operation should allocate
//  or consume memory, unless the user-suppled overloaded 'operator==' does so.
//
//
//           ( A '!' IN THE TABLE BELOW INDICATES THE TEST CASE HAS )
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
//
//![ 2] HashTable(const ALLOCATOR&  allocator = ALLOCATOR());
// [  ] HashTable(const HASHER&     hash,
//                const COMPARATOR& compare,
//                SizeType          initialBucketCount,
//                const ALLOCATOR&  allocator = ALLOCATOR());
//![ 7] HashTable(const HashTable& original);
//![ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
//![ 2] ~HashTable();
// [  ] HashTable& operator=(const HashTable& rhs);
//      template <class SOURCE_TYPE>
// [  ] bslalg::BidirectionalLink *insertIfMissing(
//                                          bool               *isInsertedFlag,
//                                          const SOURCE_TYPE&  obj);
// [  ] bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);
// [  ] bslalg::BidirectionalLink *insertIfMissing(const KeyType& key);
//      template <class SOURCE_TYPE>
// [  ] bslalg::BidirectionalLink *insert(const SOURCE_TYPE& obj);
// [  ] bslalg::BidirectionalLink *insert(
//                                      const ValueType& obj,
//                                      const bslalg::BidirectionalLink *hint);
// [  ] void rehashForNumBuckets(SizeType newNumBuckets);
// [  ] void rehashForNumElements(SizeType numElements);
// [  ] void removeAll();
// [  ] void setMaxLoadFactor(float loadFactor);
//![ 8] void swap(HashTable& other);
//
//      ACCESSORS
//![ 4] bslalg::BidirectionalLink *elementListRoot() const;
//![13] bslalg::BidirectionalLink *find(const KeyType& key) const;
//![ 4] SizeType size() const;
//![ 4] ALLOCATOR allocator() const;
// [  ] const COMPARATOR& comparator() const;
// [  ] const HASHER& hasher()     const;
// [  ] float maxLoadFactor() const;
// [  ] SizeType bucketIndexForKey(const KeyType& key) const;
// [  ] const bslalg::HashTableBucket& bucketAtIndex(SizeType index) const;
// [  ] SizeType countElementsInBucket(SizeType index) const;
// [  ] void findRange(bslalg::BidirectionalLink **first,
//                   bslalg::BidirectionalLink **last,
//                   const KeyType&              k) const;
// [  ] bslalg::BidirectionalLink *findEndOfRange(
//                                     bslalg::BidirectionalLink *first) const;
// [  ] float loadFactor() const;
// [  ] SizeType maxNumBuckets() const;
// [  ] SizeType maxSize() const;
// [  ] SizeType numBuckets() const;
//
//![ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
//![ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
//// specialized algorithms:
//![ 8] void swap(HashTable& a, HashTable& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
//![ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
//![ 3] HashTable& gg(HashTable *object, const char *spec);
//![11] HashTable g(const char *spec);
//
// [  ] CONCERN: The type is compatible with STL allocator.
// [  ] CONCERN: The type has the necessary type traits.
// [  ] CONCERN: The type provides the full interface defined by the standard.

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

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;
typedef bsltf::StdTestAllocator<int> StlTestIntAllocator;

typedef ::bsl::hash<int>     TestIntHash;
typedef ::bsl::equal_to<int> TestIntEqual;

namespace bsl {

template <class FIRST, class SECOND>
inline void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    bsls::BslTestUtil::callDebugprint(p.first);
    bsls::BslTestUtil::callDebugprint(p.second);
}

} // close namespace bsl


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

} // close namespace BloombergLP::bslstl
} // close namespace BloombergLP


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
    // 'compareKeys' prediate are all arranged contiguously within the list.
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
  private:
    // DATA
    native_std::size_t d_mixer;

  public:
    explicit StatefulHash(native_std::size_t mixer = 0xffff)
    : d_mixer(mixer)
    {
    }

    native_std::size_t operator()(const KEY& k) const {
        return Base::operator()(k) ^ d_mixer;
    }
};

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestFacilityHasher : HASHER { // exploit empty base
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


// test support function
template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
   ::BloombergLP::bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
   const typename KEY_CONFIG::ValueType&                             value)
{
    return hashTable->insert(value);
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

    static const KeyType& extractKey(const ValueType& value) {
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
    static void testCase4() { Type::testCase4(); }

    static void testCase3() { Type::testCase3(); }

    static void testCase2() { Type::testCase2(); }
};

template <class ELEMENT>
struct TestDriver_StatefulConfiguation {
    typedef TestDriver< BasicKeyConfig<ELEMENT>
                      , TestFacilityHasher<ELEMENT, StatefulHash<int> >
                      , ::bsl::equal_to<ELEMENT>
                      , ::bsl::allocator<ELEMENT>
                      > Type;

    // TEST CASES
    static void testCase4() { Type::testCase4(); }

    static void testCase3() { Type::testCase3(); }

    static void testCase2() { Type::testCase2(); }
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
    //: 4 The range '[cbegin(), cend())' contains inserted elements, but not
    //:   necessarily in any give order.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'get_allocator' method.
    //:
    //:     3 Verify the object contains the expected number of elements.
    //:
    //:     4 Use 'cbegin' and 'cend' to iterate through all elements and
    //:       verify the values are as expected.  (C-1..2, 4)
    //:
    //:     5 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   bslalg::BidirectionalLink *elementListRoot() const;
    //   SizeType size() const;
    //   ALLOCATOR allocator() const;
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;                     // source line number
        const char *d_spec;                     // specification string
        const char *d_results;                  // expected results
    } DATA[] = {
        //line  spec      result
        //----  --------  ------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "AB",     "AB"    },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const COMPARATOR EQUAL = COMPARATOR();

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
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
                      objPtr = new (fa) Obj(&sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
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

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.allocator());
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
    //   unordered_multiset<K,H,E,A>& gg(unordered_multiset<K,H,E,A> *object, const char *spec);
    //   int ggg(unordered_multiset<K,H,E,A> *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

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
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    //:10 'insert' adds an additional element to the object and returns the
    //:   iterator to the newly added element.
    //:
    //:11 Duplicated values are inserted contiguously in the range of existing
    //:   equivalent elements, without changing their relative order.
    //:
    //:12 'removeAll' properly destroys each contained element value.
    //:
    //:13 'removeAll' does not allocate memory.
    //:
    //:14 Any argument can be 'const'.
    //:
    //:15 Any memory allocation is exception neutral.
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
    //   default constructor (bootstrap) 
    //   ~HashTable();
    //   insertElement  (test driver function, proxy for basic manipulator)
    //   void removeAll();
    // ------------------------------------------------------------------------

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
#if 1
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
                    // no rehashes should be necesssary, and will in fact show
                    // up as a memory use error.
//                    if (VALUE_TYPE_USES_ALLOC || expectToAllocate(LENGTH)) {
                    if (VALUE_TYPE_USES_ALLOC) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }

#if 1
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
#if 0  // Planned test caes, not yet implemented
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

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
#endif
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
        // BOOSTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS
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
