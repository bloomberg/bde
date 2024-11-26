// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtableiterator.h>  // usage example
#include <bslstl_iterator.h>           // `distance`, in usage example

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_swaputil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_exceptionguard.h>
#include <bslma_rawdeleterguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isfunction.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_removeconst.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsltf_convertiblevaluewrapper.h>
#include <bsltf_degeneratefunctor.h>
#include <bsltf_evilbooleantype.h>
#include <bsltf_nonequalcomparabletesttype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslstl_pair.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    #include <random>
#endif
#include <stdexcept>  // to verify correct exceptions are thrown

#include <limits.h>
#include <math.h>
#include <stddef.h>  // `size_t`
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // `strcmp`

using namespace BloombergLP;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//@bdetdsplit PARTS (syntax version 1.0.0)
//@
//@# This test driver will be split into multiple parts for faster compilation
//@# using bde_xt_cpp_splitter.py from the bde-tools repo.  Each line below
//@# controls which test cases from this file will be included in one (or more)
//@# standalone test drivers.  Specific contents of each part can be further
//@# controlled by //@bdetdsplit comments throughout this file, for which full
//@# documentation can be found by running:
//@#    bde_xt_cpp_splitter --help usage-guide
//@
//@  CASES: 1, 24..25
//@  CASES: 2.SLICES
//@  CASES: 3.SLICES
//@  CASES: 4.SLICES, 5
//@  CASES: 6.SLICES
//@  CASES: 7.SLICES, 8
//@  CASES: 9.SLICES
//@  CASES: 10.SLICES, 11, 12
//@  CASES: 13.SLICES, 14
//@  CASES: 15.SLICES
//@  CASES: 16.SLICES
//@  CASES: 17.SLICES, 18..END
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a value-semantic container class template, whose
// elements are indexed by key, and duplicate key values are permitted.  The
// purpose of the component is to support implementing the four standard
// unordered containers, so it may be customized at compile time with a C++11
// standard conforming allocator, the most important of which is the `bsl`
// allocator that supports runtime customization of the object allocator.
//
// The basic test plan is to employ the standard 10 test cases for a value-
// semantic type, before incrementally validating the remaining methods.  One
// important issue is the selection of primary manipulators.  The `insert`
// operations of the container may trigger a `rehash` operation, which is
// unrelated to value.  As this is an operation we prefer to defer testing
// until after the initial ten value-semantic test cases, we create an
// `insertElement` function that checks if an insert would trigger a rehash
// before proceeding.  This becomes our primary manipulator, coupled with a
// strategy to always reserve enough space at construction to accommodate all
// the values inserted for that test scenario.  This is then sufficient to
// bring the object to any valid state.
//
// The Primary Manipulators and Basic Accessors are decided to be:
//
// Primary Manipulators:
//  - `insertElement` free function in this test driver
//  - `removeAll`
//
// Basic Accessors:
//  - `allocator`
//  - `comparator`
//  - `hasher`
//  - `size`
//  - `numBuckets`
//  - `maxLoadFactor`
//  - `rehashThreshold`
//  - `elementListRoot`
//  - `bucketAtIndex`
//  - `bucketIndexForKey`
//
// We note that of the basic accessors, only `size` and `elementListRoot`
// contribute to value; while none of the others are salient to value, they are
// important for maintaining the efficient indexing of the container, and are
// required to properly inspect the state of the container after any
// manipulation in the succeeding test cases.
//
// One unusual aspect of this class is that the default constructor creates an
// empty container with no space reserved for elements to insert.  As we are
// avoiding rehash operations during the (idiomatic) initial ten value-
// semantic class test cases, this means we must use the value constructor
// through the value-semantic bootstrap tests, as this constructor does allow
// us to specify an initial capacity.  Further, the default constructor has a
// more minimal set of requirements on the template arguments than other
// constructors, so we will test this constructor as the very final test case,
// to confirm that all the other validated functions operate correctly when
// instantiated with the more minimal contracts.
//
// As we are testing a template, we will make heavy use of the `bsltf` template
// test facility.  Our primary means of implementing each test case will be to
// write a single function template for the case, that can test and satisfy all
// the concerns of each of the eventual instantiations.  For example, while
// testing the range of potential allocator types, and the way memory is
// consumed by the object under test, we will arrange for all allocators to
// wrap a `bslma::TestAllocator`, which can separately be queried for the
// allocator usage, regardless of the interface of the actual allocator types
// themselves.  The concerns for each test case will be split into two
// sections: the type concerns will be documented and addressed in a dispatch
// function for each case, that will address the concerns by instantiated the
// test driver function for a sufficient variety of types to be sure that such
// concerns are addressed.  The test case implementation function template will
// list the runtime behavioral concerns of the test case, regardless of the
// types it is instantiated with, and address those concerns.  This combination
// of testing will ensure thorough testing covering all combinations of concern
// for the component.
//-----------------------------------------------------------------------------
// PUBLIC TYPES
//*[22] typedef ALLOCATOR                            AllocatorType;
//*[22] typedef bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[22] typedef typename KEY_CONFIG::KeyType         KeyType;
//*[22] typedef typename KEY_CONFIG::ValueType       ValueType;
//*[22] typedef bslalg::BidirectionalNode<ValueType> NodeType;
//*[22] typedef typename AllocatorTraits::size_type  SizeType;
//
// CREATORS
// [  ] HashTable(const ALLOCATOR& allocator = ALLOCATOR());
// [ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
// [ 7] HashTable(const HashTable& original);
// [ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
//*[ 8] HashTable(MovableRef<HashTable> original);
//*[ 8] HashTable(MovableRef<HashTable> original, const ALLOCATOR& allocator);
// [ 2] ~HashTable();
//
// MANIPULATORS
//*[ 9] operator=(const HashTable& rhs);
// [  ] HashTable& operator=(BloombergLP::bslmf::MovableRef<HashTable> rhs);
// [  ] template <class... Args> Link *emplaceIfMissing(Args&&... args);
//*[13] template <class P> Link *insert(P&& obj);
//*[13] template <class P> Link *insert(P&& obj, const Link *hint);
//*[16] template <class P> Link *insertIfMissing(bool *isInserted, P&& obj);
//*[16] Link *insertIfMissing(bool *isInsertedFlag, const ValueType& obj);
// [  ] Link *insertIfMissing(bool *isInsertedFlag, MovableRef<ValueType> obj);
//*[17] Link *insertIfMissing(const KeyType& key);
// [  ] remove(bslalg::BidirectionalLink *node);
// [ 2] removeAll();
//*[11] rehashForNumBuckets(SizeType newNumBuckets);
//*[12] reserveForNumElements(SizeType numElements);
//*[14] setMaxLoadFactor(float loadFactor);
// [ 8] swap(HashTable& other);
//
// ACCESSORS
// [ 4] allocator() const;
// [ 4] comparator() const;
// [ 4] hasher() const;
// [ 4] size() const;
//*[19] maxSize() const;
// [ 4] numBuckets() const;
//*[19] maxNumBuckets() const;
//*[14] loadFactor() const;
// [ 4] maxLoadFactor() const;
// [ 4] rehashThreshold() const;
// [ 4] elementListRoot() const;
//*[18] find(const KeyType& key) const;
//*[18] findRange(BLink **first, BLink **last, const KeyType& k) const;
//*[ 6] findEndOfRange(bslalg::BidirectionalLink *first) const;
// [ 4] bucketAtIndex(SizeType index) const;
// [ 4] bucketIndexForKey(const KeyType& key) const;
// [ 4] countElementsInBucket(SizeType index) const;
//
// [ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
// [ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
// ASPECTS:
// [ 8] void swap(HashTable& a, HashTable& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// class HashTable_ImpDetails
// [  ] bslalg::HashTableBucket *defaultBucketAddress();
// [  ] size_t growBucketsForLoadFactor(size_t *, size_t, size_t, double);
// [  ] bslma::Allocator *incidentalAllocator();
// [  ] size_t nextPrime(size_t n);
//
// class HashTable_Util
// [  ] initAnchor<ALLOC>(bslalg::HashTableAnchor *, size_t, const ALLOC&)
// [  ] destroyBucketArray<A>(bslalg::HashTableBucket *, size_t, const A&)
//
// class HashTable_NodeProctor
// [  ] TBD...
//
// class HashTable_ArrayProctor
// [  ] TBD...
//
// class HashTable_ComparatorWrapper
// [  ] TBD...
//
// class HashTable_HashWrapper
// [  ] TBD...
//
// TEST TEST APPARATUS AND GENERATOR FUNCTIONS
// [ 3] int ggg(HashTable *object, const char *spec, int verbosity = 1);
// [ 3] HashTable& gg(HashTable *object, const char *spec);
// [ 2] insertElement(HashTable<K, H, E, A> *, const K::ValueType&)
// [ 3] verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
// [  ] bool expectPoolToAllocate(size_t n)
// [  ] size_t predictNumBuckets(size_t length, float maxLoadFactor)
//
// [  ] CONCERN: The type employs the expected size optimizations.
// [  ] CONCERN: The type has the necessary type traits.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE  BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define BSL_TF_EQ      BSLTF_TEMPLATETESTFACILITY_COMPARE_EQUAL
#define BSL_TF_NOT_EQ  BSLTF_TEMPLATETESTFACILITY_COMPARE_NOT_EQUAL

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU // An alias for a string that can be
                                      // treated as the "%zu" format

#define TD BSLS_BSLTESTUTIL_FORMAT_TD // An alias for a string that can be
                                      // treated as the "%td" format

// ============================================================================
//                        TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

// #define BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK 1
// Define the macro `BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK`
// to provide a truly minimal test driver (fast enough to be suitable for
// testing during iterative development).

// # define BSLSTL_HASHTABLE_TRIM_TEST_CASE_COPY_ASSIGN_COMPLEXITY 1
// Define the macro `BSLSTL_HASHTABLE_TRIM_TEST_CASE_COPY_ASSIGN_COMPLEXITY` to
// minimize the runtime cost of the most time consuming test case.

// # define BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS 1
// Define the macro `BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS` for special
// test runs when we are prepared to raise the test-driver time-out to handle
// extreme edge cases of load factor.  This macro enables testing of
// non-realistic scenarios to find corner-case issues that might affect
// standard conformance of the unordered containers built on top of
// `bslstl::HashTable`.

#if __cplusplus <= 199711L && !defined(BSLS_PLATFORM_CMP_MSVC)
    // We note that certain test cases rely on the reference collapsing rules
    // that were adopted shortly after C++03, and so are not a feature of older
    // compilers, or perhaps compilers in strictly conforming modes.  MSVC is
    // an exception because it does not provided the real `__cplusplus` value
    // in some build modes, and we do not support so old versions of MSVC that
    // has no reference collapsing.
  #define u_NO_REFERENCE_COLLAPSING                                           1
#endif  // Too old and not MSVC

#ifdef BSLS_PLATFORM_CMP_IBM
// The IBM implementation of `abort` does not have a no-return annotation, so
// may yield warnings about falling off the end of a function without returning
// a value, where `abort` is being used as the fail-safe in some functions.
  #define u_SILENCE_NO_RETURN_WARNING throw 0
#else   // On IBM xlC
  #define u_SILENCE_NO_RETURN_WARNING (void)0
#endif  // Not IBM xlC

//=============================================================================
//                       GLOBALLY ACCESSIBLE TEST VERBOSITY
//-----------------------------------------------------------------------------

static bool             verbose = false;
static bool         veryVerbose = false;
static bool     veryVeryVerbose = false;
static bool veryVeryVeryVerbose = false;

//=============================================================================
//                  STANDARD LIBRARY TEMPLATE SPECIALIZATIONS
//-----------------------------------------------------------------------------

namespace bsl {

              // ================================================
              // bsl::equal_to<bsltf::NonEqualComparableTestType>
              // ================================================

/// This class template specialization provides a functor to determine when
/// two `::BloombergLP::bsltf::NonEqualComparableTestType` objects have the
/// same value.  Usually, the primary template calls `operator==` to
/// determine the result, but that cannot work for the test type that is
/// specifically designed to not have an `operator==`.  In such cases, we
/// must provide an explicit specialization of this template if we expect
/// the test type to work in our test cases along side other types.
template <>
struct equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType> {

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          first_argument_type;
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          second_argument_type;
    typedef bool                                          result_type;

    // ACCESSORS

    /// Return `true` if the specified `a` and `b` objects have the same
    /// value, and `false` otherwise.  Two `NonEqualComparableTestType`
    /// objects have the same value if ... (TBD)
    bool operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                    const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                         const;
};

              // ------------------------------------------------
              // bsl::equal_to<bsltf::NonEqualComparableTestType>
              // ------------------------------------------------

inline
bool equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
                    (const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                     const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                          const
{
    return BSL_TF_EQ(a, b);
}

                    // ========================================
                    // class template bsl::hash specializations
                    // ========================================

/// This class template specialization provides a functor to determine a
/// hash value for `::BloombergLP::bsltf::NonEqualComparableTestType`
/// objects.
template <>
struct hash< ::BloombergLP::bsltf::NonEqualComparableTestType> {

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType argument_type;
    typedef size_t                                           result_type;

    // ACCESSORS

    /// Return a hash for the specified `value` such that this functor
    /// satisfies the Hash requirements of the C++ Standard,
    /// [hash.requirements].
    size_t operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType&
                                                                  value) const;
};

                // ----------------------------------------
                // class template bsl::hash specializations
                // ----------------------------------------

// not inlining initially due to static local data
size_t hash< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
          (const ::BloombergLP::bsltf::NonEqualComparableTestType& value) const
{
    static const bsl::hash<int> k_EVALUATE_HASH = bsl::hash<int>();
    return k_EVALUATE_HASH(value.data());
}

}  // close namespace bsl

//=============================================================================
//                           GLOBALLY USED TYPEDEFS
//-----------------------------------------------------------------------------

typedef bslalg::HashTableImpUtil  ImpUtil;
typedef bslalg::BidirectionalLink Link;

//=============================================================================
//                `debugprint` overload for `bslstl::HashTable`
//-----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslstl {
// ADL-accessible `debugprint` function template for the test facilities to be
// able to print any `bslstl::HashTable`.  See `bslim_bsltestutil`.

/// Print the value of the specified `HashTable` `t` in a format suitable
/// for reading for debug purposes.
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
        const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t);
}  // close package namespace

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void bslstl::debugprint(
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t)
{
    if (0 == t.size()) {
        fputs("<empty>", stdout);
    }
    else {
        typedef bslalg::BidirectionalLink Link;
        for (Link *it = t.elementListRoot(); it; it = it->nextLink()) {
            const typename KEY_CONFIG::KeyType& key =
                                           ImpUtil::extractKey<KEY_CONFIG>(it);
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                             bsltf::TemplateTestFacility::getIdentifier(key)));
        }
    }
    fflush(stdout);
}

}  // close corporate namespace

//=============================================================================
//------------------------<| GENERAL TEST MACHINERY |>-------------------------
//=============================================================================

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

bslma::TestAllocator *g_bsltfAllocator_p = 0;

// Peeking into the "white box" of our standard table of prime numbers in
// `HashTable_Util`, we can see critical sequence lengths around 2, 5 and 13.
// That suggests we will want to pay attention to length 0, 1, 2, 3, 4, 5, 6,
// 12, 13 and 14.  We will pay special attention to all permutations around the
// smaller lengths, and test a representative range of concerns of distinct/
// duplicate values, in and out of order, for the longer sequences.

//@bdetdsplit FOR 3,4, 9,10, 13,14 BEGIN
struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to `gg` function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line grp              spec            results
    //---- ---  ----------------     --------------
    { L_,    0,               "",                "" },
    { L_,    1,              "A",               "A" },
    { L_,    2,              "B",               "B" },
    { L_,    3,             "AA",              "AA" },
    { L_,    4,             "AB",              "AB" },
    { L_,    4,             "BA",              "AB" },
    { L_,    5,             "AC",              "AC" },
    { L_,    6,             "BB",              "BB" },
    { L_,    7,             "CD",              "CD" },
    { L_,    8,            "AAA",             "AAA" },
    { L_,    9,            "AAB",             "AAB" },
    { L_,    9,            "ABA",             "AAB" },
    { L_,    9,            "BAA",             "AAB" },
    { L_,   10,            "ABC",             "ABC" },
    { L_,   10,            "ACB",             "ABC" },
    { L_,   10,            "BAC",             "ABC" },
    { L_,   10,            "BCA",             "ABC" },
    { L_,   10,            "CAB",             "ABC" },
    { L_,   10,            "CBA",             "ABC" },
    { L_,   11,            "BAD",             "ABD" },
    { L_,   12,           "AAAA",            "AAAA" },
    { L_,   13,           "AABA",            "AAAB" },
    { L_,   14,           "ABAB",            "AABB" },
    { L_,   15,           "AABC",            "AABC" },
    { L_,   15,           "ABCA",            "AABC" },
    { L_,   16,           "ABCB",            "ABBC" },
    { L_,   17,           "ABCC",            "ABCC" },
    { L_,   18,           "ABCD",            "ABCD" },
    { L_,   18,           "ACBD",            "ABCD" },
    { L_,   19,           "BEAD",            "ABDE" },
    { L_,   20,          "ABCDE",           "ABCDE" },
    { L_,   21,          "FEDCB",           "BCDEF" },
    { L_,   22,         "ABCABC",          "AABBCC" },
    { L_,   22,         "AABBCC",          "AABBCC" },
    { L_,   23,         "FEDCBA",          "ABCDEF" },
    { L_,   24,   "ABCDEFGHIJKL",    "ABCDEFGHIJKL" },
    { L_,   25,  "ABCDEFGHIJKLM",   "ABCDEFGHIJKLM" },
    { L_,   26, "ABCDEFGHIJKLMN",  "ABCDEFGHIJKLMN" },
};

static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;
//@bdetdsplit FOR 3,4, 9,10, 13,14 END

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//@bdetdsplit FOR 2..4, 6,7, 9,10, 13..15 BEGIN
static
const float DEFAULT_MAX_LOAD_FACTOR[] = {
#ifdef BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS
    // Note that using tiny values for `maxLoadFactor` can consume a huge
    // amount of memory for no real purpose, and greatly slows down the
    // running of the test case.  These test cases are disabled by default
    // as a practical matter, but can be run manually by defining the macro
    // `BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS` when building the test
    // driver.
            1e-5,    // This is small enough to test extremely low value
#endif  // BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS
            0.125f,  // Low value used throughout the test driver
            1.0f,    // Default and most common value
            8.0f     // High value used throughout the test driver
#ifdef BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS
    // When `maxLoadFactor` reaches infinity, then the hash table has a single
    // bucket, and the complexity behavior of a linked list.  This can impact
    // test times, so is reserved for infrequent validation builds when the
    // quadratic testing time can be supported.
          , std::numeric_limits<float>::infinity()  // edge case
#endif  // BSLSTL_HASHTABLE_TEST_EXTREME_LOAD_FACTORS
};
static const int DEFAULT_MAX_LOAD_FACTOR_SIZE =
              sizeof DEFAULT_MAX_LOAD_FACTOR / sizeof *DEFAULT_MAX_LOAD_FACTOR;
//@bdetdsplit FOR 2..4, 6,7, 9,10, 13..15 END

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes {

                       // ===========================
                       // class AwkwardMaplikeElement
                       // ===========================

/// This class provides an awkward value-semantic type, designed to be used
/// with a KEY_CONFIG policy for a HashTable that supplies a non-equality
/// comparable key-type, using `data` for the `extractKey` method, while the
/// class itself *is* equality-comparable (as required of a value-semantic
/// type) so that a HashTable of these objects should have a well-defined
/// `operator==`.  Note that this class is a specific example for a specific
/// problem, rather than a template providing the general test type for keys
/// distinct from values, as the template test facility requires an explicit
/// specialization of a function template,
/// `TemplateTestFacility::getIdentifier<T>`, which would require a partial
/// template specialization if this class were a template, and that is not
/// supported by the C++ language.
class AwkwardMaplikeElement {

  private:
    bsltf::NonEqualComparableTestType d_data;

  public:
    // CREATORS

    /// Create an `AwkwardMaplikeElement` element having `0` as its `data`.
    AwkwardMaplikeElement();

    /// Create an `AwkwardMaplikeElement` element having the specified
    /// `value` as its `data`.
    explicit
    AwkwardMaplikeElement(int value);

    /// Create an `AwkwardMaplikeElement` element having the specified
    /// `value` as its `data`.
    explicit
    AwkwardMaplikeElement(const bsltf::NonEqualComparableTestType& value);

    // MANIPULATORS

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;

    /// Return a reference offering non-modifiable access to the `key` of
    /// this object.
    const bsltf::NonEqualComparableTestType& key() const;
};

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `AwkwardMaplikeElement` objects have
/// the same value if ... (TBD)
bool operator==(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `AwkwardMaplikeElement` objects
/// do not have the same value if ... (TBD)
bool operator!=(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);

/// Print to the console a textual representation of the specified `value`.
/// Note that this representation is intended only to support error reports
/// and not as a portable format.
void debugprint(const AwkwardMaplikeElement& value);

                       // ---------------------------
                       // class AwkwardMaplikeElement
                       // ---------------------------
// CREATORS
inline
AwkwardMaplikeElement::AwkwardMaplikeElement()
: d_data()
{
}

inline
AwkwardMaplikeElement::AwkwardMaplikeElement(int value)
: d_data(value)
{
}

inline
AwkwardMaplikeElement::AwkwardMaplikeElement(
                                const bsltf::NonEqualComparableTestType& value)
: d_data(value)
{
}

// MANIPULATORS
inline
void AwkwardMaplikeElement::setData(int value)
{
    d_data.setData(value);
}

// ACCESSORS
inline
int AwkwardMaplikeElement::data() const
{
    return d_data.data();
}

inline
const bsltf::NonEqualComparableTestType& AwkwardMaplikeElement::key() const
{
    return d_data;
}

}  // close namespace TestTypes

inline
bool TestTypes::operator==(const AwkwardMaplikeElement& lhs,
                           const AwkwardMaplikeElement& rhs)
{
    return BSL_TF_EQ(lhs.data(), rhs.data());
}

inline
bool TestTypes::operator!=(const AwkwardMaplikeElement& lhs,
                           const AwkwardMaplikeElement& rhs)
{
    return !(lhs == rhs);
}

inline
void TestTypes::debugprint(const AwkwardMaplikeElement& value)
{
    bsls::debugprint(value.data());
}

                      // ----------------------------
                      // AwkwardMaplikeElement TRAITS
                      // ----------------------------

namespace BloombergLP {
namespace bsltf {

template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                                const TestTypes::AwkwardMaplikeElement& object)
{
    return object.data();
}

}  // close namespace bsltf
}  // close enterprise namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes {
                          // ======================
                          // class MostEvilTestType
                          // ======================

/// This class provides an awkward value-semantic type, designed to be used
/// with a KEY_CONFIG policy for a HashTable that supplies a non-equality
/// comparable key-type, using `data` for the `extractKey` method, while the
/// class itself *is* equality-comparable (as required of a value-semantic
/// type) so that a HashTable of these objects should have a well-defined
/// `operator==`.  Note that this class is a specific example for a specific
/// problem, rather than a template providing the general test type for keys
/// distinct from values, as the template test facility requires an explicit
/// specialization of a function template,
/// `TemplateTestFacility::getIdentifier<T>`, which would require a partial
/// template specialization if this class were a template, and that is not
/// supported by the C++ language.
class MostEvilTestType {

  private:
    bsltf::NonEqualComparableTestType d_data;

  private:
    // NOT IMPLEMENTED
    void operator=(MostEvilTestType&) BSLS_KEYWORD_DELETED;

    void operator&() BSLS_KEYWORD_DELETED;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&) BSLS_KEYWORD_DELETED;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&) BSLS_KEYWORD_DELETED;

    static void *operator new(std::size_t size) BSLS_KEYWORD_DELETED;
    static void *operator new(std::size_t size, void *ptr)
                                                          BSLS_KEYWORD_DELETED;

    // PRIVATE CLASS METHODS

    /// This method is called by `operator new` if the constructor throws,
    /// to free the footprint memory.  This should never be called, since
    /// `operator new` is not defined, but due to a compiler bug on AIX and
    /// Windows, it is referred to, but never called, if the destructor is
    /// non-trivial, resulting in link failures unless this method is
    /// defined.  So we booby-trap this method with an assert.
    static void operator delete(void *ptr);

  public:
    // CREATORS

    /// Create an `AwkwardMaplikeElement` object having the specified
    /// `value` as its `data`.
    explicit MostEvilTestType(int value);

    // explicit MostEvilTestType(const MostEvilTestType& original) = default;
        // Create a `MostEvilTestType` object having the same `data` value as
        // the specified `original`.

    // ~MostEvilTestType() = default;
        // Destroy this object.

    // MANIPULATORS

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;
};

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `MostEvilTestType` objects have the
/// same value if ... (TBD)
bool operator==(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `MostEvilTestType` objects do
/// not have the same value if ... (TBD)
bool operator!=(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);

/// Print to the console a textual representation of the specified `value`.
/// Note that this representation is intended only to support error reports
/// and not as a portable format.
void debugprint(const MostEvilTestType& value);

                       // ----------------------
                       // class MostEvilTestType
                       // ----------------------

// PRIVATE CLASS METHODS
void MostEvilTestType::operator delete(void *)
{
    BSLS_ASSERT_INVOKE_NORETURN("should never be called");
}

// CREATORS
inline
MostEvilTestType::MostEvilTestType(int value)
: d_data(value)
{
}

// MANIPULATORS
inline
void MostEvilTestType::setData(int value)
{
    d_data.setData(value);
}

// ACCESSORS
inline
int MostEvilTestType::data() const
{
    return d_data.data();
}

}  // close namespace TestTypes

inline
bool TestTypes::operator==(const MostEvilTestType& lhs,
                           const MostEvilTestType& rhs)
{
    return BSL_TF_EQ(lhs.data(), rhs.data());
}

inline
bool TestTypes::operator!=(const MostEvilTestType& lhs,
                           const MostEvilTestType& rhs)
{
    return !(lhs == rhs);
}

inline
void TestTypes::debugprint(const MostEvilTestType& value)
{
    bsls::debugprint(value.data());
}

                         // -----------------------
                         // MostEvilTestType TRAITS
                         // -----------------------

namespace BloombergLP {
namespace bsltf {

template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::MostEvilTestType>(
                                     const TestTypes::MostEvilTestType& object)
{
    return object.data();
}

}  // close namespace bsltf
}  // close enterprise namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace
#ifndef BSLS_PLATFORM_CMP_CLANG
    TestMachinery
#endif
{
                          // ====================
                          // class IsBslAllocator
                          // ====================
template <class TYPE>
struct IsBslAllocator : bsl::false_type {};

/// This simple traits class reports `true` if the (template parameter)
/// `TYPE` is an instantiation of the `bsl::allocator` template, and `false`
/// otherwise.
template <class TYPE>
struct IsBslAllocator<bsl::allocator<TYPE> > : bsl::true_type {};

                             // ===============
                             // class BoolArray
                             // ===============

/// This class holds a set of boolean flags, the number of which is
/// specified when such an object is constructed.  The number of flags is
/// fixed at the time of construction, and the object is neither copy-
/// constructible nor copy-assignable.  The flags can be set and tested
/// using `operator[]`.  This class is a lightweight alternative to
/// `vector<bool>` as there is no need to introduce such a component
/// dependency for a simple test facility (in the same package).
/// TBD: The constructor should support a `bslma::Allocator *` argument to
///      supply memory for the dynamically allocated array.
class BoolArray {

  private:
    // DATA
    bool   *d_data;
    size_t  d_size;

  private:
    // NOT IMPLEMENTED
    BoolArray(const BoolArray&); // = delete;
    BoolArray& operator=(const BoolArray&);  // = delete

  public:
    // CREATORS

    /// Create a `BoolArray` object holding the specified `n` boolean flags.
    explicit BoolArray(size_t n);

    /// Destroy this object, reclaiming any allocated memory.
    ~BoolArray();

    // MANIPULATORS

    /// Return a reference offering modifiable access to the boolean flag at
    /// the specified `index`.  The behavior is undefined unless
    /// `index < size()`.
    bool& operator[](size_t index);

    // ACCESSORS

    /// Return the value of the boolean flag at the specified `index`.  The
    /// behavior is undefined unless `index < size()`.
    bool operator[](size_t index) const;

    /// Return the number of boolean flags held by this object.
    size_t size() const;
};

                             // ---------------
                             // class BoolArray
                             // ---------------
// CREATORS
inline
BoolArray::BoolArray(size_t n)
: d_data(new bool[n])
, d_size(n)
{
    for (size_t i = 0; i != n; ++i) {
        d_data[i] = false;
    }
}

inline
BoolArray::~BoolArray()
{
    delete[] d_data;
}

inline
bool& BoolArray::operator[](size_t index)
{
    BSLS_ASSERT_SAFE(index < d_size);

    return d_data[index];
}

inline
bool BoolArray::operator[](size_t index) const
{
    BSLS_ASSERT_SAFE(index < d_size);

    return d_data[index];
}

inline
size_t BoolArray::size() const
{
    return d_size;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===============================
                       // class GroupedEqualityComparator
                       // ===============================

/// This test class provides a mechanism that defines a function-call
/// operator that compares two objects of the parameterized `TYPE`, but in a
/// way that multiple distinct value will compare equal with each other.
/// The function-call operator compares the integer returned by the class
/// method `TemplateTestFacility::getIdentifier` divided by the (template
/// parameter) GROUP_SIZE.
template <class TYPE, int GROUP_SIZE>
class GroupedEqualityComparator {

  public:
    // ACCESSORS

    /// Return `true` if the integer representation of the specified `lhs`
    /// divided by `GROUP_SIZE` (rounded down) is equal to than integer
    /// representation of the specified `rhs` divided by `GROUP_SIZE`.
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
    {
        const int leftId  = bsltf::TemplateTestFacility::getIdentifier(lhs);
        const int rightId = bsltf::TemplateTestFacility::getIdentifier(rhs);

        return (leftId / GROUP_SIZE) == (rightId / GROUP_SIZE);
    }

};

                            // ===================
                            // class GroupedHasher
                            // ===================

/// This test class provides a mechanism that defines a function-call
/// operator that returns the same hash for multiple values.  The
/// function-call operator invoke the (template parameter) type `HASHER` on
/// the integer returned by the class method
/// `TemplateTestFacility::getIdentifier` divided by the (template
/// parameter) GROUP_SIZE.  `HASHER` shall be a class that can be invoked as
/// if it has the following method:
/// ```
/// int operator()(int value);
/// ```
template <class TYPE, class HASHER, int GROUP_SIZE>
class GroupedHasher : private HASHER {

  public:
    // ACCESSORS

    /// Return the hash value of the integer representation of the specified
    /// `value` divided by `GROUP_SIZE` (rounded down) is equal to than
    /// integer representation of the specified `rhs` divided by
    /// `GROUP_SIZE`.
    size_t operator()(const TYPE& value) const
    {
        const int groupId = bsltf::TemplateTestFacility::getIdentifier(value);

        return HASHER::operator()(groupId / GROUP_SIZE);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                            // ====================
                            // struct TestException
                            // ====================

struct TestException : std::exception { };

                    // ================================
                    // class ThrowingEqualityComparator
                    // ================================

/// This test class provides a mechanism that defines a function-call
/// operator that compares two objects of the parameterized `TYPE`.  The
/// function-call operator is implemented with integer comparison using
/// integers converted from objects of `TYPE` by the class method
/// `TemplateTestFacility::getIdentifier`.  The function-call operator also
/// increments a counter used to keep track the method call count.  Object
/// of this class can be identified by an id passed on construction.
template <class TYPE>
class ThrowingEqualityComparator {

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times `operator()` is called
    size_t         d_throwInterval; // throw after this many `operator()` calls

  public:
    // CREATORS
    //! ThrowingEqualityComparator(const ThrowingEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified `original`.

    /// Create a `ThrowingEqualityComparator`.  Optionally, specify `id`
    /// that can be used to identify the object.
    explicit ThrowingEqualityComparator(size_t id = 0)
    : d_id(id)
    , d_count(0)
    , d_throwInterval(0)
    {
    }

    // MANIPULATORS

    /// Set the `id` of this object to the specified `value`.
    void setId(size_t value)
    {
        d_id = value;
    }

    /// Set to the specified `value` the number of times `operator()` may be
    /// called after throwing an exception before another such call would
    /// throw.  If `0 == value` then disable throwing of exceptions by
    /// `operator()`.
    ///
    /// TBD: Document behavior if `value` is less than the number of times
    ///      `operator()` has already been called.
    void setThrowInterval(size_t value)
    {
        d_throwInterval = value;
    }

    // ACCESSORS

    /// Increment a counter that records the number of times this method is
    /// called.   Return `true` if the integer representation of the
    /// specified `lhs` is less than integer representation of the specified
    /// `rhs`.
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
    {
        ++d_count;

        if (d_throwInterval && !(d_count % d_throwInterval)) {
            BSLS_THROW(TestException());
        }

        return bsltf::TemplateTestFacility::getIdentifier(lhs)
                            == bsltf::TemplateTestFacility::getIdentifier(rhs);
    }

    /// Return the number of times `operator()` is called.
    size_t count() const
    {
        return d_count;
    }

    /// Return the `id` of this object.
    size_t id() const
    {
        return d_id;
    }

    /// Return the number of times `operator()` may be called after throwing
    /// an exception before another such call would throw, or `0` if no
    /// exceptions are ever thrown.
    size_t throwInterval() const
    {
        return d_throwInterval;
    }
};

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `ThrowingEqualityComparator` functors have the
/// same value if they have the same `id`.
template <class TYPE>
inline
bool operator==(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() == rhs.id();
}

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `ThrowingEqualityComparator` functors
/// do not have the same value if they do not have the same `id`.
template <class TYPE>
inline
bool operator!=(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // =========================
                       // class ThrowingHashFunctor
                       // =========================

/// This value-semantic class meets the C++11 `Hash` requirements (C++11
/// [hash.requirements], 17.6.3.4) with an overload for `operator()` that
/// can be configured at runtime to throw exceptions after a user-supplied
/// number of function calls.  It can also be configured to never throw, and
/// merely count the number of invocations of the function call operator.
/// Note that this functor relies on "logical" rather than "physical"
/// `const` behavior, as it must count the number of times a
/// `const`-qualified function is invoked.
template <class TYPE>
class ThrowingHashFunctor {

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times `operator()` is called
    size_t         d_throwInterval; // throw after this many `operator()` calls

  public:
    // CREATORS
    //! ThrowingHashFunctor(const ThrowingHashFunctor& original) = default;
        // Create a copy of the specified `original`.

    /// Create a `ThrowingHashFunctor`.  Optionally, specify `id` that can
    /// be used to identify the object.
    explicit ThrowingHashFunctor(size_t id = 0)
    : d_id(id)
    , d_count(0)
    , d_throwInterval()
    {
    }

    // MANIPULATORS

    /// Set the `id` of this object to the specified value.  Note that the
    /// `id` contributes to the value produced by the `operator()` method,
    /// so the `id` of a `ThrowingHashFunctor` should not be changed for
    /// functors that are installed in `HashTable` objects.
    void setId(size_t value)
    {
        d_id = value;
    }

    /// Set to the specified `value` the number of times `operator()` may be
    /// called after throwing an exception before another such call would
    /// throw.  If `0 == value` then disable throwing of exceptions by
    /// `operator()`.
    ///
    /// TBD: Document behavior if `value` is less than the number of times
    ///      `operator()` has already been called.
    void setThrowInterval(size_t value)
    {
        d_throwInterval = value;
    }

    // ACCESSORS

    /// Increment a counter that records the number of times this method is
    /// called.   Return a hash value for the specified `obj`.  The behavior
    /// is undefined unless `obj` is a value supplied by the BSL template
    /// test facility.
    std::size_t operator()(const TYPE& obj) const
    {
        ++d_count;

        if (d_throwInterval && !(d_count % d_throwInterval)) {
            BSLS_THROW(TestException());
        }

        return bsltf::TemplateTestFacility::getIdentifier(obj) + d_id;
    }

    /// Return the number of times `operator()` is called.
    size_t count() const
    {
        return d_count;
    }

    /// Return the `id` of this object.
    size_t id() const
    {
        return d_id;
    }

    /// Return the number of times `operator()` may be called after throwing
    /// an exception before another such call would throw, or `0` if no
    /// exceptions are ever thrown.
    size_t throwInterval() const
    {
        return d_throwInterval;
    }
};

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `ThrowingHashFunctor` functors have the same
/// value if they have the same `id`.
template <class TYPE>
inline
bool operator==(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `ThrowingHashFunctor` functors do not
/// have the same value if they do not have the same `id`.
template <class TYPE>
inline
bool operator!=(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ========================
                       // class TestFacilityHasher
                       // ========================

/// This test class provides a mechanism that defines a function-call
/// operator that provides a hash code for objects of the parameterized
/// `KEY`.  The function-call operator is implemented by calling the wrapper
/// functor, `HASHER`, with integers converted from objects of `KEY` by the
/// class method `TemplateTestFacility::getIdentifier`.    Note that this
/// class privately inherits from the specified `HASHER` class in order to
/// exploit any compiler optimizations for empty base classes.
template <class KEY, class HASHER = bsl::hash<int> >
class TestFacilityHasher : private HASHER { // exploit empty base optimization

  public:
    /// Create a `TestFacilityHasher`.  Optionally, specify `hash` that can
    /// be used to initialize the underlying `HASHER` object implementation.
    TestFacilityHasher(const HASHER& hash = HASHER())               // IMPLICIT
    : HASHER(hash)
    {
    }

    // ACCESSORS

    /// Return a hash code for the specified `k` using the wrapped functor
    /// of (template parameter) type `HASHER` supplied at construction.
    std::size_t operator()(const KEY& k) const
    {
        const int id = bsltf::TemplateTestFacility::getIdentifier(k);
        return HASHER::operator()(id);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                      // ============================
                      // class ConvertibleValueHasher
                      // ============================

/// TBD: This test class provides...
template <class KEY, class HASHER = bsl::hash<int> >
class ConvertibleValueHasher : private TestFacilityHasher<KEY, HASHER> {

    typedef TestFacilityHasher<KEY, HASHER> Base;

  public:
    ConvertibleValueHasher(const HASHER& hash = HASHER())           // IMPLICIT
    : Base(hash)
    {
    }

    // ACCESSORS

    /// Return a hash code for the specified `k` using the wrapped functor
    /// of (template parameter) type `HASHER` supplied at construction.
    std::size_t operator()(const bsltf::ConvertibleValueWrapper<KEY>& k) const
    {
        return Base::operator()(k);
    }
};

                     // ================================
                     // class ConvertibleValueComparator
                     // ================================

/// This test class provides...
template <class KEY>
class ConvertibleValueComparator {

  public:
    // ACCESSORS

    /// Return a value convertible to `true` if the specified `a` has the
    /// same value as the specified `b`, and a value convertible to `false`
    /// otherwise..
    bsltf::EvilBooleanType operator()(
                            const bsltf::ConvertibleValueWrapper<KEY>& a,
                            const bsltf::ConvertibleValueWrapper<KEY>& b) const
    {
        return BSL_TF_EQ(static_cast<const KEY&>(a),
                         static_cast<const KEY&>(b));
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ========================
                       // struct GenericComparator
                       // ========================

struct GenericComparator {
    // ACCESSORS

    /// Return a value convertible to `true` if the specified `arg1` has the
    /// same value as the specified `arg2`, for some unspecified definition
    /// that defaults to `operator==`, but may use some other functionality.
    template <class ARG1_TYPE, class ARG2_TYPE>
    bsltf::EvilBooleanType operator()(ARG1_TYPE& arg1, ARG2_TYPE& arg2)

    {
        return BSL_TF_EQ(arg1, arg2);
    }
};

                       // ===================
                       // class GenericHasher
                       // ===================

/// This test class provides a mechanism that defines a function-call
/// operator that provides a hash code for objects of the parameterized
/// `KEY`.
class GenericHasher {

  public:
    // ACCESSORS

    /// Return a hash code for the specified `k`.
    template <class KEY>
    std::size_t operator()(KEY& k);
};

                       // -------------------
                       // class GenericHasher
                       // -------------------

// ACCESSORS
template <class KEY>
std::size_t GenericHasher::operator()(KEY& k)
{
    // do not inline initially due to static local data

    typedef typename bsl::remove_const<KEY>::type MUTABLE_KEY;

    static const TestFacilityHasher<MUTABLE_KEY> s_hasher;
    return s_hasher(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===========================
                       // struct ModifiableComparator
                       // ===========================

template <class KEY>
struct ModifiableComparator {
    // ACCESSORS

    /// Return a value convertible to `true` if the specified `arg1` has the
    /// same value as the specified `arg2`, for some unspecified definition
    /// that defaults to `operator==`, but may use some other functionality
    /// for `KEY` types that do not support this operator.
    bsltf::EvilBooleanType operator()(KEY& arg1, KEY& arg2)
    {
        return BSL_TF_EQ(arg1, arg2);
    }
};

                         // ======================
                         // class ModifiableHasher
                         // ======================

/// This test class provides a mechanism that defines a function-call
/// operator that provides a hash code for objects of the parameterized
/// `KEY`.
template <class KEY>
class ModifiableHasher  // exploit empty base optimization
: private TestFacilityHasher<typename bsl::remove_const<KEY>::type> {

  private:
    // PRIVATE TYPES
    typedef TestFacilityHasher<typename bsl::remove_const<KEY>::type> Base;

  public:
    // ACCESSORS

    /// Return a hash code for the specified `key`.
    std::size_t operator()(KEY& key)
    {
        return Base::operator()(key);
    }
};

                      // ==========================
                      // struct ModifiableKeyConfig
                      // ==========================

/// This class provides the most primitive possible KEY_CONFIG type that can
/// support a `HashTable`.  It might be consistent with use as a `set` or a
/// `multiset` container.  It also allows for functors that expect to take
/// their argument by a reference to non-`const`, although behavior will be
/// undefined should any such functor actually modify such an argument.
template <class ELEMENT>
struct ModifiableKeyConfig {

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    /// Return the specified `value`.
    static KeyType& extractKey(ValueType& value)
    {
        return value;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===========================
                       // class DefaultOnlyComparator
                       // ===========================

/// This test class provides a mechanism that defines a function-call
/// operator that compares two objects of the parameterized `TYPE`.  However
/// it is a perverse type intended for testing templates against their
/// minimum requirements, and as such is neither copyable nor swappable, and
/// is only default constructible.  The function call operator will return a
/// perverse type that is convertible-to-bool.
template <class TYPE>
class DefaultOnlyComparator {

  public:
    // ACCESSORS

    /// Return `true` if the specified `lhs` and `rhs` have the same value.
    bsltf::EvilBooleanType operator()(const TYPE& lhs, const TYPE& rhs)
    {
        return BSL_TF_EQ(lhs, rhs);
    }
};

                       // =======================
                       // class DefaultOnlyHasher
                       // =======================

/// This test class provides a mechanism that defines a function-call
/// operator that returns the same hash for `TYPE` values using the standard
/// `bsl::hash` functor.  However, this class is a perverse type intended
/// for testing templates against their minimum requirements, and as such is
/// neither copyable nor swappable, and is only default constructible.
template <class TYPE>
class DefaultOnlyHasher {

  private:
    // NOT IMPLEMENTED
    DefaultOnlyHasher(const DefaultOnlyHasher&); // = delete
    void operator=(const DefaultOnlyHasher&); // = delete;

    void operator&();  // = delete;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&); // = delete;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&); // = delete;

  public:
    // CREATORS
    DefaultOnlyHasher() {}

    // ACCESSORS

    /// Return the hash of the specified `value`.
    size_t operator()(const TYPE& value)
    {
        return bsl::hash<int>().operator()(
                      bsltf::TemplateTestFacility::getIdentifier(value));
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =============================
                       // class FunctionPointerPolicies
                       // =============================

template <class KEY>
struct FunctionPointerPolicies {
    typedef size_t HashFunction(const KEY&);
    typedef bool   ComparisonFunction(const KEY&, const KEY&);

    /// Return `true` if the specified `lhs` has the same value as the
    /// specified `rhs` when compared using the `bsltf` test facility
    /// `BSLTF_EQ`, and `false` otherwise.
    static bool compare(const KEY& lhs, const KEY& rhs)
    {
        return BSL_TF_EQ(lhs, rhs);
    }

    /// Return the hash value of the specified `k` according to the `bsltf`
    /// hash functor, `TestFacilityHasher<KEY>`.
    static size_t hash(const KEY& k)
    {
        static const TestFacilityHasher<KEY> s_hasher;
        return s_hasher(k);
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ========================
                       // class MakeCallableEntity
                       // ========================

template <class CALLABLE>
struct MakeCallableEntity {
    typedef CALLABLE CallableEntityType;

    /// Return a default-constructed object of the (template parameter) type
    /// `CALLABLE`.
    static CallableEntityType make();
};

template <class CALLABLE>
struct MakeCallableEntity<CALLABLE&> {
    typedef CALLABLE& CallableEntityType;

    /// Return a reference to a default-constructed object of the (template
    /// parameter) type `CALLABLE`.  Note that this is will be a reference
    /// to a singleton object; any modifications made to that object will be
    /// reflected in references to previous and subsequent calls to this
    /// function.
    static CallableEntityType make();
};

template <class FUNCTOR, bool ENABLE_SWAP>
struct MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> > {
    typedef bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> CallableEntityType;

    /// Return a functor wrapping a default-constructed object of the
    /// (template parameter) type `FUNCTOR`.  The adapter overloads all
    /// operations other than the function-call operator in a most awkward
    /// way, and is swappable if and only if (the template parameter)
    /// `ENABLE_SWAP` is `true`.
    static CallableEntityType make();
};

template <class KEY>
struct MakeCallableEntity<size_t (*)(const KEY&)> {
    typedef size_t FunctionType(const KEY&);
    typedef FunctionType *CallableEntityType;

    /// Return the address of a function that can compute hash values for
    /// objects of the (template parameter) type `KEY`.
    static CallableEntityType make();
};

template <class KEY>
struct MakeCallableEntity<bool (*)(const KEY&, const KEY&)> {
    typedef bool FunctionType(const KEY&, const KEY&);
    typedef FunctionType *CallableEntityType;

    /// Return the address of a function that can compare two objects of the
    /// (template parameter) type `KEY` for equality.
    static CallableEntityType make();
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(&)(ARG)> {
    typedef RESULT FunctionType(ARG);
    typedef FunctionType& CallableEntityType;

    /// Return a reference to a function that can compute hash values for
    /// objects of the (template parameter) type `KEY`.
    static CallableEntityType make();
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(&)(ARG1, ARG2)> {
    typedef RESULT FunctionType(ARG1, ARG2);
    typedef FunctionType& CallableEntityType;

    /// Return a reference to a function that can compare two objects of the
    /// (template parameter) type `KEY` for equality.
    static CallableEntityType make();
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(ARG)> {
    typedef RESULT FunctionType(ARG);
    typedef FunctionType& CallableEntityType;

    /// Return a reference to a function that can compute hash values for
    /// objects of the (template parameter) type `KEY`.
    static CallableEntityType make();
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(ARG1, ARG2)> {
    typedef RESULT FunctionType(ARG1, ARG2);
    typedef FunctionType& CallableEntityType;

    /// Return a reference to a function that can compute hash values for
    /// objects of the (template parameter) type `KEY`.
    static CallableEntityType make();
};

                       // ------------------------
                       // class MakeCallableEntity
                       // ------------------------

template <class CALLABLE>
inline
typename MakeCallableEntity<CALLABLE>::CallableEntityType
MakeCallableEntity<CALLABLE>::make()
{
    return CALLABLE();
}

template <class CALLABLE>
inline
typename MakeCallableEntity<CALLABLE &>::CallableEntityType
MakeCallableEntity<CALLABLE &>::make()
{
    static CALLABLE sharedCallable = MakeCallableEntity<CALLABLE>::make();
    return sharedCallable;
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
typename MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> >::
                                                             CallableEntityType
MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> >::make()
{
    return bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::
                          cloneBaseObject(MakeCallableEntity<FUNCTOR>::make());
}

template <class KEY>
inline
typename MakeCallableEntity<size_t (*)(const KEY&)>::CallableEntityType
MakeCallableEntity<size_t (*)(const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::hash;
}

template <class KEY>
inline
typename
       MakeCallableEntity<bool (*)(const KEY&, const KEY&)>::CallableEntityType
MakeCallableEntity<bool (*)(const KEY&, const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::compare;
}

template <class RESULT, class ARG>
inline
typename MakeCallableEntity<RESULT(&)(ARG)>::CallableEntityType
MakeCallableEntity<RESULT(&)(ARG)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

template <class RESULT, class ARG1, class ARG2>
inline
typename MakeCallableEntity<RESULT(&)(ARG1, ARG2)>::CallableEntityType
MakeCallableEntity<RESULT(&)(ARG1, ARG2)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

template <class RESULT, class ARG>
inline
typename MakeCallableEntity<RESULT(ARG)>::CallableEntityType
MakeCallableEntity<RESULT(ARG)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

template <class RESULT, class ARG1, class ARG2>
inline
typename MakeCallableEntity<RESULT(ARG1, ARG2)>::CallableEntityType
MakeCallableEntity<RESULT(ARG1, ARG2)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===================
                       // class MakeAllocator
                       // ===================

/// TBD: This utility class template ...
template <class ALLOCATOR>
struct MakeAllocator {

    // PUBLIC TYPES
    typedef ALLOCATOR AllocatorType;

    // CLASS METHODS

    /// Return a default-constructed `ALLOCATOR` object.
    static AllocatorType make(bslma::Allocator *)
    {
        return AllocatorType();
    }
};

/// TBD: This utility class template specialization...
template <class TYPE>
struct MakeAllocator<bsl::allocator<TYPE> > {

    // PUBLIC TYPES
    typedef bsl::allocator<TYPE> AllocatorType;

    // CLASS METHODS

    /// Return a `bsl::allocator<TYPE>` object wrapping the specified
    /// `basicAllocator`.
    static AllocatorType make(bslma::Allocator *basicAllocator)
    {
        return AllocatorType(basicAllocator);
    }
};

/// TBD: This utility class template specialization...
template <class TYPE>
struct MakeAllocator<bsltf::StdTestAllocator<TYPE> > {

    // PUBLIC TYPES
    typedef bsltf::StdTestAllocator<TYPE> AllocatorType;

    // CLASS METHODS

    /// Return a `bsltf::StdTestAllocator<TYPE>` object wrapping the
    /// specified `basicAllocator`.
    static AllocatorType make(bslma::Allocator *basicAllocator)
    {
        // TBD: This method is a little bit of overkill (heavy on the
        //      assertions) as a left-over from when we were trying hard to
        //      nail down a tricky bug that manifests only on the IBM AIX
        //      compiler.  It should probably be cleaned up for final release.

        ASSERT(g_bsltfAllocator_p);

        ASSERT(bsltf::StdTestAllocatorConfiguration::delegateAllocator());

        typedef bsltf::StdTestAllocatorConfiguration BsltfAllocConfig;
        bslma::Allocator *installed = BsltfAllocConfig::delegateAllocator();

        ASSERT(dynamic_cast<bslma::TestAllocator*>(installed));
        ASSERT(dynamic_cast<bslma::TestAllocator*>(basicAllocator));

#define u_AS_TA(allocParam) dynamic_cast<bslma::TestAllocator *>(allocParam)
        bslma::TestAllocator *currentTestAllocator   = u_AS_TA(installed);
        bslma::TestAllocator *parameterTestAllocator = u_AS_TA(basicAllocator);
#undef u_AS_TA

        ASSERTV(currentTestAllocator,         parameterTestAllocator,
                currentTestAllocator->name(), parameterTestAllocator->name(),
                currentTestAllocator == parameterTestAllocator);

        return AllocatorType();
    }
};

/// TBD: This utility class template specialization...
template <class TYPE, bool A, bool B, bool C, bool D>
struct MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> > {

    // PUBLIC TYPES
    typedef bsltf::StdStatefulAllocator<TYPE, A, B, C, D> AllocatorType;

    // CLASS METHODS

    /// Return a `bsltf::StdStatefulAllocator<TYPE, A, B, C, D>` object
    /// wrapping the specified `basicAllocator`.
    static AllocatorType make(bslma::Allocator *basicAllocator)
    {
        typedef bslma::TestAllocator * const TestAllocPtr;
        TestAllocPtr asTestAlloc = dynamic_cast<TestAllocPtr>(basicAllocator);
        ASSERT(asTestAlloc);
        return AllocatorType(asTestAlloc);
    }
};

                            // =================
                            // class ObjectMaker
                            // =================

// The `ObjectMaker` template and its associated specializations customize the
// act of creating a object-under-test, in-place, using an allocator configured
// according to some plan.  Generally, the plans are spelled out 'a' -> 'z',
// with each letter corresponding to a specific way of passing an allocator to
// the test object's constructor.  In practice, we currently define only the
// configurations 'a' -> 'd', and they are called sequentially.  As we become
// more thorough in testing, additional configurations will present themselves,
// and specific tests will want different subsets of the available range of
// configurations.  It is likely we will have functions that return a string
// literal describing the recommended range of configurations to test, for a
// given (named) plan, for a given specialization - as not all allocators will
// support all configurations.  Rather than mindlessly testing each option for
// each set of types for each test, it would make sense to offer a simplified
// set of configurations for the more restricted allocators.

/// TBD: This utility class template ...
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct ObjectMaker {

    typedef          ALLOCATOR             AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<ALLOCATOR>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>   Obj;

    typedef MakeAllocator<ALLOCATOR> AllocMaker;

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// allocator determined by the specified `config` to the constructor.
    /// Return an allocator object that will compare equal to the allocator
    /// that is expected to be used to construct the `HashTable` object.
    /// The specified `objectAllocator` may, or may not, be used to
    /// construct the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator)
    {
        switch (config) {
          case 'a': {
            ALLOCATOR objAlloc = AllocMaker::make(objectAllocator);
            *objPtr = new (*fa) Obj(objAlloc);
            return objAlloc;                                          // RETURN
          } break;
          case 'b': {
            *objPtr = new (*fa) Obj();
            return ALLOCATOR();                                       // RETURN
          } break;
          case 'c': {
            ALLOCATOR result = ALLOCATOR();
            *objPtr = new (*fa) Obj(result);
            return result;                                            // RETURN
          } break;
          case 'd': {
            using bslma::Default;
            ALLOCATOR objAlloc = AllocMaker::make(Default::defaultAllocator());
            *objPtr = new (*fa) Obj(objAlloc);
            return objAlloc;                                          // RETURN
          } break;
        }

        ASSERTV(config, "Bad allocator config.", false);
        abort();
        u_SILENCE_NO_RETURN_WARNING;
    }

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// specified `hash`, `compare`, `initialBuckets`,
    /// `initialMaxLoadFactor` and the allocator determined by the specified
    /// `config` to the constructor.  Return an allocator object that will
    /// return `true` when compared with the allocator that is expected to
    /// be used to construct the `HashTable` object using `operator==`.  The
    /// specified `objectAllocator` may, or may not, be used to construct
    /// the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor)
    {
        switch (config) {
          case 'a': {
            ALLOCATOR objAlloc = AllocMaker::make(objectAllocator);
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    objAlloc);
            return objAlloc;                                          // RETURN
          } break;
          case 'b': {
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor);
            return ALLOCATOR();                                       // RETURN
          } break;
          case 'c': {
            ALLOCATOR result = ALLOCATOR();
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    result);
            return result;                                            // RETURN
          } break;
          case 'd': {
              typedef bslma::Default Dflt;
              ALLOCATOR objAlloc = AllocMaker::make(Dflt::defaultAllocator());
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    objAlloc);
            return objAlloc;                                          // RETURN
          } break;
        }

        ASSERTV(config, "Bad allocator config.", false);
        abort();
        u_SILENCE_NO_RETURN_WARNING;
    }

    static const char *specForBootstrapTests() { return "abc";  }
    static const char *specForCopyTests()      { return "abcd"; }

    /// Return the set of configurations to pass to `makeObject` in order to
    /// test the behavior of a `HashTable` instantiated with the matching
    /// set of template parameters.
    static const char *specForDefaultTests()   { return "abcd"; }
};

/// TBD: This utility class template specialization...
template <class KEY_CONFIG, class HASHER, class COMPARATOR>
struct ObjectMaker<KEY_CONFIG,
                   HASHER,
                   COMPARATOR,
                   bsl::allocator<typename KEY_CONFIG::ValueType> > {

    typedef bsl::allocator<typename KEY_CONFIG::ValueType> AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// allocator determined by the specified `config` to the constructor.
    /// Return an allocator object that will compare equal to the allocator
    /// that is expected to be used to construct the `HashTable` object.
    /// The specified `objectAllocator` may, or may not, be used to
    /// construct the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator)
    {
        switch (config) {
          case 'a': {
            *objPtr = new (*fa) Obj(objectAllocator);
            return objectAllocator;                                   // RETURN
          } break;
          case 'b': {
            *objPtr = new (*fa) Obj();
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
          case 'c': {
            *objPtr = new (*fa) Obj(static_cast<bslma::Allocator *>(0));
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
          case 'd': {
            *objPtr = new (*fa) Obj(bslma::Default::defaultAllocator());
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
        }

        ASSERTV(config, "Bad allocator config.", false);
        abort();
        u_SILENCE_NO_RETURN_WARNING;
    }

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// specified `hash`, `compare`, `initialBuckets`,
    /// `initialMaxLoadFactor` and the allocator determined by the specified
    /// `config` to the constructor.  Return an allocator object that will
    /// return `true` when compared with the allocator that is expected to
    /// be used to construct the `HashTable` object using `operator==`.  The
    /// specified `objectAllocator` may, or may not, be used to construct
    /// the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor)
    {
        switch (config) {
          case 'a': {
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    objectAllocator);
            return objectAllocator;                                   // RETURN
          } break;
          case 'b': {
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor);
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
          case 'c': {
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    static_cast<bslma::Allocator *>(0));
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
          case 'd': {
            *objPtr = new (*fa) Obj(hash,
                                    compare,
                                    initialBuckets,
                                    initialMaxLoadFactor,
                                    bslma::Default::defaultAllocator());
            return AllocatorType(bslma::Default::allocator());        // RETURN
          } break;
        }

        ASSERTV(config, "Bad allocator config.", false);
        abort();
        u_SILENCE_NO_RETURN_WARNING;
    }

    static const char *specForBootstrapTests() { return "abc";  }
    static const char *specForCopyTests()      { return "abcd"; }

    /// Return the set of configurations to pass to `makeObject` in order to
    /// test the behavior of a `HashTable` instantiated with the matching
    /// set of template parameters.
    static const char *specForDefaultTests()   { return "abcd"; }
};

/// TBD: This utility class template specialization...
template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
struct ObjectMaker<
                KEY_CONFIG,
                HASHER,
                COMPARATOR,
                bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                             A, B, C, D> > {

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    typedef bsltf::StdStatefulAllocator<ValueType, A, B, C, D>   AllocatorType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type  SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    typedef MakeAllocator<AllocatorType> AllocMaker;

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// allocator determined by the specified `config` to the constructor.
    /// Return an allocator object that will compare equal to the allocator
    /// that is expected to be used to construct the `HashTable` object.
    /// The specified `objectAllocator` may, or may not, be used to
    /// construct the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator)
    {
        // `bsltf::StdStatefulAllocator` objects are not DefaultConstructible.
        // We know that the default allocator installed for this test driver
        // will be a test allocator, so we can safely expect a `dynamic_cast`
        // to not return a null pointer.  Likewise, the `objectAllocator`
        // should not be a null pointer either, so we can simply construct the
        // desired allocator object using the test allocator specified by the
        // `config` parameter, and use that to explicitly construct the desired
        // `HashTable` object into `*objPtr`.  Note that there is no
        // distinction between config 'a' or 'b' for this allocator, it would
        // be useful if we could find some way to skip config 'a' when running
        // the various test cases.

        using bslma::TestAllocator;
        using bslma::Default;

        TestAllocator *alloc = 'a' == config
                                    ? objectAllocator
                                    : dynamic_cast<TestAllocator *>(
                                                  Default::defaultAllocator());
        ASSERT(alloc);

        AllocatorType result = AllocMaker::make(alloc);
        *objPtr = new (*fa) Obj(result);
        return result;
    }

    /// Create a `HashTable` object at the specified `objPtr` address via an
    /// in-place `new` using the specified `fa` allocator, and passing the
    /// specified `hash`, `compare`, `initialBuckets`,
    /// `initialMaxLoadFactor` and the allocator determined by the specified
    /// `config` to the constructor.  Return an allocator object that will
    /// return `true` when compared with the allocator that is expected to
    /// be used to construct the `HashTable` object using `operator==`.  The
    /// specified `objectAllocator` may, or may not, be used to construct
    /// the `HashTable` object according to the `config`:
    /// ```
    /// config  allocator
    /// 'a'     use the specified `objectAllocator`
    /// 'b'     use the default supplied by the constructor
    /// 'c'     explicitly pass a null pointer of type `bslma::Allocator *`
    /// 'd'     explicitly pass the default allocator
    /// ```
    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor)
    {
        // `bsltf::StdStatefulAllocator` objects are not DefaultConstructible.
        // We know that the default allocator installed for this test driver
        // will be a test allocator, so we can safely expect a `dynamic_cast`
        // to not return a null pointer.  Likewise, the `objectAllocator`
        // should not be a null pointer either, so we can simply construct the
        // desired allocator object using the test allocator specified by the
        // `config` parameter, and use that to explicitly construct the desired
        // `HashTable` object into `*objPtr`.  Note that there is no
        // distinction between config 'a' or 'b' for this allocator, it would
        // be useful if we could find some way to skip config 'a' when running
        // the various test cases.

        using bslma::TestAllocator;
        using bslma::Default;

        TestAllocator *alloc = 'a' == config
                                    ? objectAllocator
                                    : dynamic_cast<TestAllocator *>(
                                                  Default::defaultAllocator());
        ASSERT(alloc);

        AllocatorType result = AllocMaker::make(alloc);
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                result);
        return result;
    }

    static const char *specForBootstrapTests() { return "ab"; }
    static const char *specForCopyTests()      { return "ab"; }

    /// Return the set of configurations to pass to `makeObject` in order to
    /// test the behavior of a `HashTable` instantiated with the matching
    /// set of template parameters.
    static const char *specForDefaultTests()   { return "ab"; }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                         test support functions

/// Return a null pointer value, as there is no way to extract a test
/// allocator from an unknown allocator type.  This function should be
/// overloaded for allocator types for which it is possible to extract a
/// test allocator.
template <class ALLOCATOR>
bslma::TestAllocator *extractTestAllocator(ALLOCATOR&)
{
    // Return a null pointer value.  Note that in general, there is no way to
    // extract a test allocator from a generic allocator type.
    return 0;
}

/// Return the address of the allocator `mechanism` wrapped by the specified
/// `alloc` if it is a test allocator (which can be found by `dynamic_cast`)
/// and a null pointer value otherwise.
template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsl::allocator<TYPE>& alloc)
{
    // Return the address of the test allocator wrapped by the specified
    // `alloc`, and a null pointer value if `alloc` does not wrap a
    // `bslma::TestAllocator`.
    return dynamic_cast<bslma::TestAllocator *>(alloc.mechanism());
}

/// Return the address of the installed StdTestAllocator if it is a test
/// allocator, and a null pointer value otherwise.  Note that all
/// `bsltf::StdTestAllocator`s share the same allocator, which is set by the
/// `bsltf::StdTestAllocatorConfiguration` utility.  We can determine if
/// this is wrapping a test allocator using `dynamic_cast`.
template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsltf::StdTestAllocator<TYPE>&)
{
    // All `bsltf::StdTestAllocator`s share the same allocator, which is set by
    // the `bsltf::StdTestAllocatorConfiguration` utility.  We can determine if
    // this is wrapping a test allocator using `dynamic_cast`.

    // Return the address of the test allocator wrapped by the standard test
    // allocator singleton, and a null pointer value if the standard test
    // allocator singleton `alloc` does not currently wrap a `TestAllocator`.

    return dynamic_cast<bslma::TestAllocator *>(
                    bsltf::StdTestAllocatorConfiguration::delegateAllocator());
}

/// Return the address of the test allocator wrapped by the specified
/// `alloc`.
template <class TYPE, bool A, bool B, bool C, bool D>
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc)
{
    return dynamic_cast<bslma::TestAllocator *>(alloc.allocator());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//       test support functions dealing with hash and comparator functors

/// Return `true`.  This function template provides a common signature that
/// may be overloaded for specific hasher types that can support the idea of
/// setting a state value.  It is assumed that any comparator that does not
/// overload this functor is stateless, and so equivalent to any other
/// comparator.
template <class COMPARATOR>
bool areEqualComparators(const COMPARATOR&, const COMPARATOR&)
{
    return true;
}

/// Return `true` if the specified function pointers `lhs` and `rhs` are the
/// same, and `false` otherwise.  To avoid ambiguities when compiling with
/// IBM xlC we have to pass the pointers by `const &`.
template <class RESULT, class ARG1, class ARG2>
bool areEqualComparators(RESULT (*const &lhs)(ARG1, ARG2),
                         RESULT (*const &rhs)(ARG1, ARG2))
{
    return lhs == rhs;
}

/// Provide an overloaded function to compare comparators.  Return `true` if
/// the specified `lhs` compares equal to the specified `rhs` under the
/// expression `lhs == rhs`.  This template will not instantiate if the
/// expression does not compile.
template <class KEY>
bool areEqualComparators(const ThrowingEqualityComparator<KEY>& lhs,
                         const ThrowingEqualityComparator<KEY>& rhs)
{
    return lhs == rhs;
}

//@bdetdsplit FOR 4 BEGIN

/// Return `true`.  This function template provides a common signature that
/// may be overloaded for specific hasher types that can support the idea of
/// setting a state value.
template <class HASHER>
bool areEqualHashers(const HASHER&, const HASHER&)
{
    return true;
}

/// Return `true` if the specified function pointers `lhs` and `rhs` are the
/// same, and `false` otherwise.  To avoid ambiguities when compiling with
/// IBM xlC we have to pass the pointers by `const &`.
template <class RESULT, class ARGUMENT>
bool areEqualHashers(RESULT (*const &lhs)(ARGUMENT),
                     RESULT (*const &rhs)(ARGUMENT))
{
    return lhs == rhs;
}

/// Provide an overloaded function to compare hash functors.  Return `true`
/// if the specified `lhs` compares equal to the specified `rhs` under the
/// expression `lhs == rhs`.  This template will not instantiate if the
/// expression does not compile.
BSLA_MAYBE_UNUSED
bool areEqualHashers(const ThrowingHashFunctor<int>& lhs,
                     const ThrowingHashFunctor<int>& rhs)
{
    return lhs == rhs;
}
//@bdetdsplit FOR 4 END

/// This is a null function, that has no effect and does not use the
/// specified `comparator` nor the specified `id`.  This function provides a
/// common signature that may be overloaded for specific `COMPARATOR` types
/// that can support the idea of setting a state value.  Test code can then
/// call a function with this signature and get the right behavior (without
/// a compile error) regardless of the properties of the `COMPARATOR` type.
template <class COMPARATOR>
void setComparatorState(COMPARATOR *comparator, size_t id)
{
    (void)comparator;
    (void)id;
}

template <class KEY>
void setComparatorState(ThrowingEqualityComparator<KEY> *comparator,size_t id)
{
    comparator->setId(id);
}


//@bdetdsplit FOR 4,7 BEGIN

/// This is a null function, that has no effect and does not use the
/// specified `hasher` nor the specified `id`.  This function provides a
/// common signature that may be overloaded for specific `HASHER` types
/// that can support the idea of setting a state value.  Test code can then
/// call a function with this signature and get the right behavior (without
/// a compile error) regardless of the properties of the `HASHER` type.
template <class HASHER>
void setHasherState(HASHER *hasher, size_t id)
{
    (void)hasher;
    (void)id;
}

BSLA_MAYBE_UNUSED
void setHasherState(ThrowingHashFunctor<int> *hasher, size_t id)
{
    hasher->setId(id);
}
//@bdetdsplit FOR 4,7 END

#ifndef BSLS_PLATFORM_CMP_CLANG
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

//@bdetdsplit FOR 15 BEGIN

/// Return `true` if the memory pool used by the container under test is
/// expected to allocate memory on the inserting the specified `n`th
/// element, and `false` otherwise.
bool expectPoolToAllocate(size_t n)
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when `n` is a power of 2
}
//@bdetdsplit FOR 15 END

/// Insert an element having the specified `value` into the specified
/// `hashTable` and return the address of the new node, unless the insertion
/// would cause the hash table to exceed its `maxLoadFactor` and rehash, in
/// which case return a null pointer value.  Return a null pointer value if
/// the `hashTable` address is a null pointer value.
template <class KEY_CONFIG, class HASHF, class EQUAL, class ALLOC>
Link *insertElement(
                 bslstl::HashTable<KEY_CONFIG, HASHF, EQUAL, ALLOC> *hashTable,
                 const typename KEY_CONFIG::ValueType&               value)
{
    if (!hashTable) {
        return 0;                                                     // RETURN
    }

    if (hashTable->size() == hashTable->rehashThreshold()) {
        return 0;                                                     // RETURN
    }

    return hashTable->insert(value);
}

/// Return the minimum number of buckets necessary to support the specified
/// `length` array of elements in a `HashTable` having the specified
/// `maxLoadFactor` without rehashing.  Note that typically the result will
/// be passed to a `HashTable` constructor or reserve call, which may in
/// turn choose to create even more buckets to preserve its growth strategy.
/// This function does not attempt to predict that growth strategy, but
/// merely predict the minimum number of buckets that strategy must
/// accommodate.
template <class SIZE_TYPE>
SIZE_TYPE predictNumBuckets(SIZE_TYPE length, float maxLoadFactor)
{
    if (!length) {
        return 0;                                                     // RETURN
    }

    if (1.0 / static_cast<double>(std::numeric_limits<SIZE_TYPE>::max())
                                                             > maxLoadFactor) {
        return std::numeric_limits<SIZE_TYPE>::max();                 // RETURN
    }

    SIZE_TYPE result = static_cast<SIZE_TYPE>(ceil(static_cast<double>(length)
                     / maxLoadFactor));
    return result ? result : 1;
}

template<class KEY_CONFIG, class COMPARATOR, class VALUE>
int verifyListContents(Link                                 *containerList,
                       const COMPARATOR&                     compareKeys,
                       const bsltf::TestValuesArray<VALUE>&  expectedValues,
                       size_t                                expectedSize)
    // =======================================================================
    // <WARNING>  THIS TEST HAS QUADRATIC COMPLEXITY ON LIST LENGTH  <WARNING>
    // <WARNING>          <| DON'T CALL IN A TIGHT LOOP! |>          <WARNING>
    // =======================================================================
    //
    // Verify the specified `containerList` has the specified `expectedSize`
    // number of elements, and contains the same values as the array in the
    // specified `expectedValues`, and that the elements in the list are
    // arranged so that elements whose keys compare equal using the specified
    // `compareKeys` predicate are all arranged contiguously within the list.
    // Return 0 if `container` has the expected values, and a non-zero value
    // otherwise.
{
    typedef typename KEY_CONFIG::ValueType ValueType;

    enum { SUCCESS          =  0,
           NO_LIST          = -1,
           LIST_TOO_SHORT   = -2,
           LIST_TOO_LONG    = -3,
           TOO_FEW_VALUES   = -4,
           DISCONTIGUOUS    = -5
    };

    // Check to avoid creating an array of length zero.
    if (0 == containerList) {
        return 0 == expectedSize ? SUCCESS : NO_LIST;                 // RETURN
    }

    if (0 == expectedSize) {
        return LIST_TOO_LONG;                                         // RETURN
    }

    if (expectedValues.size() < expectedSize) {
        return TOO_FEW_VALUES;                                        // RETURN
    }

#ifndef BSLS_PLATFORM_CMP_CLANG
    TestMachinery::
#endif
    BoolArray foundValues(expectedSize);

    size_t i = 0;
    for (Link *cursor = containerList;
         cursor;
         cursor = cursor->nextLink(), ++i)
    {
        const ValueType& element = ImpUtil::extractValue<KEY_CONFIG>(cursor);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);

        for (size_t j = 0; j != expectedSize; ++j) {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                if (!foundValues[j]) {
                    foundValues[j] = true;
                    break;
                }
            }
        }
    }

    if (expectedSize < i) {
        return LIST_TOO_LONG;                                         // RETURN
    }
    if (i < expectedSize) {
        return LIST_TOO_SHORT;                                        // RETURN
    }

    int missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            // Check to avoid spurious overflow warnings - we will never have
            // so many elements we need to worry about this.
            if (++missing == INT_MAX) {
                return missing;                                       // RETURN
            }
        }
    }

    if (missing > 0) {
        return missing;                                               // RETURN
    }

    // We make a copy of the comparator in case we are supplied a predicate
    // that has a non-`const` qualified `operator()`.  Note that we are now
    // requiring that the comparator be copy-constructible, which may be an
    // issue for testing a `HashTable` default-constructed with functors that
    // are only default-constructible themselves.

    typename bsl::conditional<bsl::is_function<COMPARATOR>::value,
                             COMPARATOR&,
                             COMPARATOR>::type doCompareKeys(compareKeys);

    // All elements are present, check the contiguity requirement.  Note that
    // this test is quadratic in the length of the list, although we will
    // optimize for the case of duplicates actually occurring.
    for (Link *cursor = containerList; cursor; cursor = cursor->nextLink()) {
        Link *next = cursor->nextLink();
        // Walk to end of key-equivalent sequence
        while (next && doCompareKeys(ImpUtil::extractKey<KEY_CONFIG>(cursor),
                                     ImpUtil::extractKey<KEY_CONFIG>(next))) {
            cursor = next;
            next   = next->nextLink();
        }

        // Check there are no more equivalent keys in the list.  Note that this
        // test also serves to check there are no duplicates in the preceding
        // part of the list, as this check would have failed earlier if that
        // were the case.
        for ( ; next; next = next->nextLink()) {
            if (doCompareKeys(ImpUtil::extractKey<KEY_CONFIG>(cursor),
                              ImpUtil::extractKey<KEY_CONFIG>(next))) {
                return DISCONTIGUOUS;                                 // RETURN
            }
        }
    }

    return SUCCESS;
}

}  // close unnamed namespace

// ============================================================================
//                          TEST DRIVER HARNESS
// ----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_CMP_CLANG
using namespace TestMachinery;
#endif

//- - - - - - - - - - - - Classes to implement test cases - - - - - - - - - - -

/// Forward declaration of the main test driver.  `CONFIGURATION_PARAMETERS`
/// must contain the following:
/// ```
/// TYPES:
///   KeyConfig
///   Hasher
///   Comparator
///   Allocator
///
/// CLASS METHODS:
///   const char *concernKind();
template <class CONFIGURATION_PARAMETERS>
class TestCases;

// Concerns for test configuration parameters
// ------------------------------------------
// The primary template requires four type parameters (within
// `CONFIGURATION_PARAMETERS`), each of which raises concerns that the template
// behaves correctly when instantiated with a variety of types that satisfy the
// requirements of the appropriate type parameter.  These concerns may affect
// all test cases, although a subset of the concerns may not apply in specific
// test cases where more precise requirements apply.  We enumerate the
// type-related concerns for each template parameter here, in a common place
// that we then may reference in the concerns for each test case.  There are
// common concerns for the function-like type parameters `Hasher` and
// `Comparator` that we catalog separately.
//
//KeyConfig
//- - - - -
//
//Test Plan
//---------
// We will use a variety of test allocators from `bsltf` to test the range of
// allocator-related concerns:
//
// 1. a minimal allocator that relyies on `allocator_traits` for everything
//
// 2. a complete allocator that supplies distinct, observable behavior for each
//    property supported by `allocator_traits`
//
// 3. allocators supporting each combination of trait that triggers different
//    allocator behavior:
//
//   1. type alias `propagate_on_container_copy_construction`  (true false)
//   2. type alias `propagate_on_container_copy_assignment`    (true false)
//   3. type alias `propagate_on_container_swap`               (true false)
//   4. type alias `pointer` is a native pointer, or a "smart" pointer
//
// TBD: Describe the "concept" of `KeyConfig`, the types and the `extractKey`
//      function it has to provide.  There ARE rules, just not documented, for
//      example on what the allowed `extractKey` signatures are.
//
// Note that for this initial release we can use a greatly simplified plan, as
// our current implementation of `allocator_traits` does not support any
// deduction of traits.  We will still use the minimal and maximal allocator
// interfaces supported by this implementation, and the range of customizable
// behaviors.
//
// Simplifying assumptions of our limited `allocator_traits` implementation:
//  - no support for allocators returning smart pointers
//  - no support for any of the fine-grained propagation traits
//  - no deducing typedefs, everything must be supplied in the allocator
//  - copy constructing a container copies the allocator of the original
//    unless it is `bsl::allocator`, in which case the default is used.
//
// - - - - - - - - - - Pre-packaged test harness adapters - - - - - - - - - - -
// The template test facility, `bsltf`, requires class templates taking a
// single argument, that is the element type to vary in the test.  We desire a
// variety of configurations pushing the various policy parameters of the
// `HashTable` class template, such as the type of functors, the key extraction
// policy, etc. so we write some simple adapters that will generate the
// appropriate instantiation of the test harness, from a template parameterized
// on only the element type (to be tested).  Note that in C++11 we would use
// the alias- template facility to define these templates, rather than using
// public inheritance through a dispatcher-shim.

// - - - - - - Configuration policies to instantiate HashTable with - - - - - -

/// This class provides the most primitive possible KEY_CONFIG type that can
/// support a `HashTable`.  It might be consistent with use as a `set` or a
/// `multiset` container.
template <class ELEMENT>
struct BasicKeyConfig {

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    /// Return the specified `value`.
    static const KeyType& extractKey(const ValueType& value)
    {
        return value;
    }
};

/// This class provides the most primitive possible KEY_CONFIG type that can
/// support a `HashTable`.  It might be consistent with use as a `set` or a
/// `multiset` container.
template <class ELEMENT>
struct BsltfKeyConfig {

    typedef int     KeyType;
    typedef ELEMENT ValueType;

    /// Return a reference to the key corresponding to the specified `value`
    /// if it were to be inserted into a `HashTable`.
    static const int& extractKey(const ValueType& value);
};

template <class ELEMENT>
const int& BsltfKeyConfig<ELEMENT>::extractKey(const ValueType& value)
{
    // Note that this function MUST return a reference, but we have no actual
    // storage to return a reference to.  As we know the specific usage
    // patterns of this test driver, we can ensure that no two hash
    // computations happen while the first result is still held as a reference.
    // Unfortunately, we have no such guarantee on simultaneous evaluations in
    // the HashTable facility itself, so we cycle through a cache of 64
    // results, as no reference should be so long lived that we see 64 live
    // references.  Note that an early version of this test driver demonstrated
    // that we could have at least 16 live references, before the limit was
    // raised to 64.

    static int results_cache[64] = {};
    static int index = -1;

    if (64 == ++index) {
        index = 0;
    }

    results_cache[index] = bsltf::TemplateTestFacility::getIdentifier(value);
    return results_cache[index];
}

/// This class provides the most primitive possible KEY_CONFIG type that can
/// support a `HashTable`.  It might be consistent with use as a `set` or a
/// `multiset` container.
struct TrickyKeyConfig {

    typedef bsltf::NonEqualComparableTestType KeyType;
    typedef TestTypes::AwkwardMaplikeElement  ValueType;

    /// Return the result of calling `key()` on the specified `value`.
    static const KeyType& extractKey(const ValueType& value)
    {
        return value.key();
    }
};

//- - - - - - - - - - - Test Driver Configuration classes - - - - - - - - - - -
//- - - - - - - (providing coverage of test-parameter concerns) - - - - - - - -

// Configurations concerned with the `KEY_CONFIG` template parameter

template <class ELEMENT>
struct BasicParams {
    // TYPES
    typedef BasicKeyConfig<     ELEMENT> KeyConfig;
    typedef TestFacilityHasher< ELEMENT> Hasher;
    typedef bsl::equal_to<      ELEMENT> Comparator;
    typedef bsl::allocator<     ELEMENT> Allocator;

};

/// Basic configuration that acts like a multi-set of `ELEMENT` values.
/// This is the most basic configuration that test the overwhelmingly common
/// case usage.
template <class ELEMENT>
struct TestCases_BasicConfiguration : TestCases<BasicParams<ELEMENT> > {
};
template <>
struct TestCases_BasicConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "basic configurations";
    }
};

template <class ELEMENT>
struct BsltfParams {
    // TYPES
    typedef BsltfKeyConfig< ELEMENT>  KeyConfig;
    typedef bsl::hash<      int>      Hasher;
    typedef bsl::equal_to<  int>      Comparator;
    typedef bsl::allocator< ELEMENT>  Allocator;
};

/// Basic configuration to test a key-type different to the value-type.
/// This is a simple attempt to deliver something approximating a map with
/// `int` as key, where the `int` is computed for each element.  This is the
/// simplest way to generate a configuration compatible with the `bsltf`
/// template testing framework used above, without rewriting each test case
/// to additional support for separate test tables of pairs to initialize a
/// more traditional-style map, with different math for computed values and
/// separate logic for duplicate elements and groups.
template <class ELEMENT>
struct TestCases_BsltfConfiguration : TestCases<BsltfParams<ELEMENT> > {
};
template <>
struct TestCases_BsltfConfiguration<void> {
    // CLASS METHODS
    static inline const char* concernKind() {
        return "map-like configuration";
    }
};

struct AwkwardMaplikeParams {
    // TYPES
    typedef TrickyKeyConfig                                KeyConfig;
    typedef TestFacilityHasher< TrickyKeyConfig::KeyType>  Hasher;
    typedef bsl::equal_to<      TrickyKeyConfig::KeyType>  Comparator;
    typedef bsl::allocator<     TrickyKeyConfig::KeyType>  Allocator;
};

/// This configuration is especially tricky, as the element type itself is
/// not equality comparable, but the key-type of the element, which is the
/// sole constituent of the element, does support key-comparison (but not
/// equality comparison) through the supplied comparison functor.
#define u_RUN_AWKWARD_MAP_LIKE(memberFunction) do {                     \
        if (veryVerbose) puts("\n\tdegenerate map-like configuration");  \
        TestCases<AwkwardMaplikeParams>::memberFunction();                \
    } while (false)

// Configurations concerned with the `HASHER` and `COMPARATOR` parameters
//
// Function-like types
// - - - - - - - - - -
// A function-like type used to instantiate the `HashTable` template may be:
//  - A function type
//  - A function pointer type
//  - An object type convertible to a function pointer
//  - An object type convertible to a function reference
//  - An object type that overloads `operator()`
//  - A reference to a function
//  - A reference to a function pointer
//  - A reference to an object type convertible to a function pointer
//  - A reference to an object type convertible to a function reference
//  - A reference to an object type that overloads `operator()`
//
// For each function-like type, the arguments to the function call me be:
//  - Passed by `const &`
//  - Passed by `const &` to something convertible from the desired argument
//  - Passed by const-unqualified '&' (subject to constraints below)
//  - Passed by value
//  - Passed by value convertible from the desired argument
//
// In each case, the callable signature may have a trailing ellipsis.
//
// For function, function-pointer, and reference-to-function types, the
// underlying function type may also have `extern "C"` linkage.
//
// For an object-type overloading `operator()`, there are additional concerns:
//  - The operator might be `const` qualified
//  - The operator might not be `const` qualified
//  - The operator may be a virtual function
//  - The operator may be a function template
//  - The object-type may be a class template
//  - THe object-type may be a union (THIS IS NOT SUPPORTED)
//
// For an object-type overloading `operator()`
//  - The type may be const-qualified
//  - The object may have internal state
//  - The object may allocate memory for state from its own allocator
// We note that a simple example of a function-like type holding state is a
// function pointer.
//
// Any invocation of a function-like type, within contract, may throw an
// exception of any type.
//
// For user-defined class types, any constructor may throw an exception of any
// type, as may any assignment operator.
//
// User defined types need not support all the implicitly declared operations,
// such as:
//  - default constructor
//  - copy constructor
//  - operator=
//  - operator&
//  - operator,
//  - operator new
//  - operator delete
//
// QoI: Where the function-like entity is an empty class type, it should not
//      add to the overall size of the `HashTable` object.
//
// We note, with mild relief, that the one thing we are not required to support
// is a pointer-to-member-function.
//
// HASHER
//- - - -
// `HASHER` is a function-like type that takes a single argument convertible
// from `typename KEY_CONFIG::KeyType` and returns a value of the type
// `std::size_t`.
//
// COMPARATOR
//- - - - - -
// `COMPARATOR` is a function-like type that takes two arguments, each
// convertible from `typename KEY_CONFIG::KeyType`, and returns a value of a
// type that is implicitly convertible to `bool`.

template <class ELEMENT>
struct StatefulParams {
    // TYPES
    typedef BasicKeyConfig<             ELEMENT> KeyConfig;
    typedef ThrowingHashFunctor<        ELEMENT> Hasher;
    typedef ThrowingEqualityComparator< ELEMENT> Comparator;
    typedef bsl::allocator<             ELEMENT> Allocator;
};

/// Simple configuration to test simple stateful functors for the hasher and
/// comparator policies.  Both functors support an `id` that provides state
/// that is not actually used when evaluating the function call operator,
/// and track the total number of calls to the function call operator.  An
/// additional member may be used to enable testing of exception safety, by
/// throwing exceptions from the function call operator every `N`th time it
/// is called.  This extra state is declared as `mutable` so that, as a
/// particularly unusual case, the function to change this state can be
/// declared `const` to better facilitate testing.
template <class ELEMENT>
struct TestCases_StatefulConfiguration : TestCases<StatefulParams<ELEMENT> > {
};
template <>
struct TestCases_StatefulConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "stateful functors";
    }
};

template <class ELEMENT>
struct GroupedUniqueKeysParams {
    // TYPES
    typedef BasicKeyConfig< ELEMENT>                     KeyConfig;
    typedef GroupedHasher<  ELEMENT, bsl::hash<int>, 5>  Hasher;
    typedef bsl::equal_to<  ELEMENT>                     Comparator;
    typedef bsl::allocator< ELEMENT>                     Allocator;
};

/// This configuration "groups" values into buckets by hashing 5 consecutive
/// values of the `ELEMENT` type to the same hash code, but maintaining a
/// unique key value for each of those cases.  This should lead to a higher
/// rate of collisions.
template <class ELEMENT>
struct TestCases_GroupedUniqueKeys
     : TestCases<GroupedUniqueKeysParams<ELEMENT> > {
};
template <>
struct TestCases_GroupedUniqueKeys<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "grouped hash with unique key values";
    }
};

template <class ELEMENT>
struct GroupedSharedKeysParams {
    // TYPES
    typedef BasicKeyConfig<            ELEMENT>                     KeyConfig;
    typedef GroupedHasher<             ELEMENT, bsl::hash<int>, 5>  Hasher;
    typedef GroupedEqualityComparator< ELEMENT,                 5>  Comparator;
    typedef bsl::allocator<            ELEMENT>                     Allocator;
};

/// This configuration "groups" values into buckets by hashing 5 consecutive
/// values of the `ELEMENT` type to the same hash code, and similarly
/// arranging for those keys to compare equal to each other.  This should
/// lead to behavior similar to a `multiset`.
template <class ELEMENT>
struct TestCases_GroupedSharedKeys
     : TestCases<GroupedSharedKeysParams<ELEMENT> > {
};
template <>
struct TestCases_GroupedSharedKeys<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "grouped hash with grouped key values";
    }
};

template <class ELEMENT>
struct DegenerateFunctorParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT> KeyConfig;

    typedef bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT> >      Hasher;

    typedef bsltf::DegenerateFunctor<ThrowingEqualityComparator<ELEMENT> >
                                                                    Comparator;
    typedef bsl::allocator<ELEMENT> Allocator;
};

/// This configuration utilizes awkward functors, that abuse the implicitly
/// declared operations (such as the address-of and comma operators) and
/// return awkward types from predicates, rather than a simple `bool`.
/// However, this configuration does support the `swap` operation in order
/// to support a broad range of test cases.
template <class ELEMENT>
struct TestCases_DegenerateConfiguration
     : TestCases<DegenerateFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_DegenerateConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "degenerate functors";
    }
};

template <class ELEMENT>
struct DegenerateNoSwapFunctorParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT>                     KeyConfig;

    typedef bsltf::DegenerateFunctor<
                TestFacilityHasher<ELEMENT>,
                false>                                  Hasher;

    typedef bsltf::DegenerateFunctor<
                ThrowingEqualityComparator<ELEMENT>,
                false>                                  Comparator;

    typedef bsl::allocator<ELEMENT>                     Allocator;
};

/// This configuration utilizes awkward functors, that abuse the implicitly
/// declared operations (such as the address-of and comma operators) and
/// return awkward types from predicates, rather than a simple `bool`.  As
/// this configuration does not support swapping of the functor types, it
/// does not support as broad a range of test cases as some of the other
/// configurations.
template <class ELEMENT>
struct TestCases_DegenerateConfigurationWithNoSwap
     : TestCases<DegenerateNoSwapFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_DegenerateConfigurationWithNoSwap<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "degenerate functors without swap";
    }
};

template <class ELEMENT>
struct ConstFunctorParams {
    // TYPES
    typedef BasicKeyConfig<           ELEMENT>  KeyConfig;
    typedef const TestFacilityHasher< ELEMENT>  Hasher;
    typedef const bsl::equal_to<      ELEMENT>  Comparator;
    typedef bsl::allocator<           ELEMENT>  Allocator;
};

/// This configuration employs const-qualified functors, and so cannot be
/// used to test behavior that may require modifying functors, such as the
/// assignment operator, or `swap`.
template <class ELEMENT>
struct TestCases_ConstFunctors : TestCases<ConstFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_ConstFunctors<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "`const` functors";
    }
};

template <class ELEMENT>
struct FunctorRefParams {
    // TYPES
    typedef BasicKeyConfig<     ELEMENT>   KeyConfig;
    typedef TestFacilityHasher< ELEMENT>&  Hasher;
    typedef bsl::equal_to<      ELEMENT>&  Comparator;
    typedef bsl::allocator<     ELEMENT>   Allocator;
};

/// This configuration instantiates the HashTable with reference type for
/// the callable template parameters, which reference types that overload
/// the function call operator.  Note that this is more general than C++11
/// requires, as the hasher must be a hash function *object*.  Technically
/// we must support the comparator being a function-type or a function-
/// reference, so we test this implementation-detail component supports the
/// hasher as well.
template <class ELEMENT>
struct TestCases_FunctorReferences : TestCases<FunctorRefParams<ELEMENT> > {
};
template <>
struct TestCases_FunctorReferences<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "functor references";
    }
};

#ifdef BSLS_PLATFORM_CMP_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4180)
    // qualifier applied to function type has no meaning: ignored
#elif defined(BSLS_PLATFORM_CMP_SUN)
    #pragma error_messages (off, reftoref)
        // Creation of reference to reference is an extension to C++03 standard
    #pragma error_messages (off, functypequal)
        // Meaningless const or volatile qualifier on function type, ignored
#endif

template <class ELEMENT>
struct FunctionTypeParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT> KeyConfig;

    typedef size_t Hasher(const ELEMENT&);

    typedef bool Comparator(const ELEMENT&, const ELEMENT&);

    typedef bsl::allocator<ELEMENT> Allocator;
};

/// This configuration instantiates the HashTable with function-type
/// arguments for the hash type and key-comparison type.  Note that this is
/// more general than C++11 requires, as the hasher must be a hash function
/// *object*.  Technically we must support the comparator being a
/// function-type or a function-reference, so we test this implementation-
/// detail component supports the hasher as well.
template <class ELEMENT>
struct TestCases_FunctionTypes : TestCases<FunctionTypeParams<ELEMENT> > {
};
template <>
struct TestCases_FunctionTypes<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "function types";
    }
};

template <class ELEMENT>
struct FunctionPtrParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT> KeyConfig;

    typedef size_t (*Hasher)(const ELEMENT&);

    typedef bool (*Comparator)(const ELEMENT&,const ELEMENT&);

    typedef bsl::allocator<ELEMENT> Allocator;
};

/// This configuration instantiates the HashTable with function-pointer type
/// arguments for the hash type and key-comparison type.  Note that this
/// corresponds to testing a stateful functor, with the awkward case that
/// the default value for the callable types is known to produce undefined
/// behavior (null pointer values for the function pointers).
template <class ELEMENT>
struct TestCases_FunctionPointers : TestCases<FunctionPtrParams<ELEMENT> > {
};
template <>
struct TestCases_FunctionPointers<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "function pointers";
    }
};

template <class ELEMENT>
struct FunctionRefParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT> KeyConfig;

    typedef size_t (&Hasher)(const ELEMENT&);

    typedef bool (&Comparator)(const ELEMENT&, const ELEMENT&);

    typedef bsl::allocator<ELEMENT> Allocator;
};

/// This configuration instantiates the HashTable with function-reference
/// arguments for the hash type and key-comparison type.  Note that this is
/// more general than C++11 requires, as the hasher must be a hash function
/// *object*.  Technically we must support the comparator being a
/// function-type or a function-reference, so we test this implementation-
/// detail component supports the hasher as well.
template <class ELEMENT>
struct TestCases_FunctionReferences : TestCases<FunctionRefParams<ELEMENT> > {
};
template <>
struct TestCases_FunctionReferences<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "function references";
    }
};

template <class ELEMENT>
struct ConvertibleValueParams {
    // TYPES
    typedef BasicKeyConfig<             ELEMENT>  KeyConfig;
    typedef ConvertibleValueHasher<     ELEMENT>  Hasher;
    typedef ConvertibleValueComparator< ELEMENT>  Comparator;
    typedef bsl::allocator<             ELEMENT>  Allocator;
};

/// This configuration tests functors whose arguments are convertible from
/// the key-type of the configuration.
template <class ELEMENT>
struct TestCases_ConvertibleValueConfiguration
     : TestCases<ConvertibleValueParams<ELEMENT> > {
};
template <>
struct TestCases_ConvertibleValueConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "functors taking convertible arguments";
    }
};

template <class ELEMENT>
struct GenericFunctorParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT>  KeyConfig;
    typedef GenericHasher            Hasher;
    typedef GenericComparator        Comparator;
    typedef bsl::allocator<ELEMENT>  Allocator;
};

/// This configuration tests functors whose function-call operator is a
/// function template that will deduce type as appropriate from the
/// arguments supplied to the function call.
template <class ELEMENT>
struct TestCases_GenericFunctors : TestCases<GenericFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_GenericFunctors<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "functors taking generic arguments";
    }
};

template <class ELEMENT>
struct ModifiableFunctorParams {
    // TYPES
    typedef ModifiableKeyConfig<  ELEMENT>  KeyConfig;
    typedef ModifiableHasher<     ELEMENT>  Hasher;
    typedef ModifiableComparator< ELEMENT>  Comparator;
    typedef bsl::allocator<       ELEMENT>  Allocator;
};

/// This configuration tests functors whose arguments to the function-call
/// operator are passed by non-`const` reference, but honor the spirit of
/// the rule by not actually making any modifications to those arguments.
/// This is something we test only because standard conformance requires it.
template <class ELEMENT>
struct TestCases_ModifiableFunctors
     : TestCases<ModifiableFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_ModifiableFunctors<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "functors taking modifiable arguments";
    }
};

// Configurations concerned with the `ALLOCATOR` template parameter

// ALLOCATOR
// - - - - -
// The `ALLOCATOR` argument must conform to the C++11 allocator requirements,
// and a minimal allocator would have (almost) all of its properties deduced by
// `std::allocator_traits`.  Therefore, allocators that do or do not supply
// each of the following should be supported, and behave as specified by
// `std::allocator_traits` (noting that `allocator_traits` is a separately
// implemented and tested component that we depend on - this is not intended to
// test `allocator_traits`, but merely that this component correctly uses
// `allocator_traits` rather than the object allocator directly):
//  - type alias `pointer`
//  - type alias `const_pointer`
//  - type alias `void_pointer`
//  - type alias `const_void_pointer`
//  - type alias `size_type`
//  - type alias `difference_type`
//  - type alias `propagate_on_container_copy_construction`
//  - type alias `propagate_on_container_copy_assignment`
//  - type alias `propagate_on_container_swap`
//  - alias template `rebind<OTHER>`
//  - `construct(ANY*, ARGS...)`
//  - `destroy(ANY*)`
//  - `max_size()`
//
// While an allocator must be CopyConstructible, there is no requirement that
// it support any of the other implicitly declared operations:
//  - default constructor
//  - operator=
//  - operator&
//  - operator,
//  - operator new
//  - operator delete
// Note that some specific functions, such as the `HashTable` default
// constructor, may require some of these extra functions, but there is no
// general requirement to supply these unless a specific function contract
// explicitly states that this operation is required, and that additional
// requirement applies to only that one function.
//
// When the allocator is `bsl::allocator<TYPE>` then we have a number of
// additional concerns, that are mostly runtime issues rather than type-
// related issues.  They will be raised and addressed in the usual manner under
// the actual test case.

template <class ELEMENT>
struct StdAllocatorParams {
    // TYPES
    typedef BasicKeyConfig<          ELEMENT>  KeyConfig;
    typedef GenericHasher                      Hasher;
    typedef bsl::equal_to<           ELEMENT>  Comparator;
    typedef bsltf::StdTestAllocator< ELEMENT>  Allocator;
};

/// This configuration uses a stateless std-conforming allocator, rather
/// than using the BDE allocator model.  To enhance test coverage of the
/// functor, we mix the generic hasher with `bsl::equal_to`, the default
/// comparator.  Note that all of these parameter types are empty, so we
/// expect to see the largest benefit from empty-base optimizations.
template <class ELEMENT>
struct TestCases_StdAllocatorConfiguration
     : TestCases<StdAllocatorParams<ELEMENT> > {
};
template <>
struct TestCases_StdAllocatorConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "stateless STL allocators";
    }
};

template <class ELEMENT>
struct StatefulAllocatorParams {
    // TYPES
    typedef BasicKeyConfig< ELEMENT>                      KeyConfig;
    typedef GroupedHasher<  ELEMENT,  bsl::hash<int>, 5>  Hasher;
    typedef GenericComparator                             Comparator;
    typedef bsltf::StdStatefulAllocator<ELEMENT,
                                        true,
                                        false,
                                        false,
                                        false>            Allocator;
};

/// This configuration uses a std-conforming allocator with per-object state
/// rather than using the BDE allocator model, and uses a different
/// allocator propagation strategy.  Allocators will propagate on copy
/// construction, but on no other operation.  We pick another mix of hasher
/// and comparator that were previously paired differently when testing
/// callable-parameter concerns.
template <class ELEMENT>
struct TestCases_StatefulAllocatorConfiguration
     : TestCases<StatefulAllocatorParams<ELEMENT> > {
};
template <>
struct TestCases_StatefulAllocatorConfiguration<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "stateful STL allocators";
    }
};

#ifdef BSLSTL_ALLOCATOR_TRAITS_COMPUTES_ALLOCATOR_PROPAGATION
// bsl::allocator_traits does not yet detect and support the allocator
// propagation predicate traits; these test configurations will be fully
// specced out once support is available

template <class ELEMENT,
          bool  COPY_PROPAGATES = true,
          bool  SWAP_PROPAGATES = true>
struct AllocatorPropagationParams {
    // TYPES
    typedef BasicKeyConfig<ELEMENT>                       KeyConfig;
    typedef GenericHasher                                 Hasher;
    typedef GenericComparator                             Comparator;
    typedef bsltf::StdStatefulAllocator<ELEMENT,
                                        COPY_PROPAGATES,
                                        SWAP_PROPAGATES>  Allocator;
};

template <class ELEMENT>
struct TestCases_StatefulAllocatorConfiguration1
     : TestCases<AllocatorPropagationParams<ELEMENT> > {
    // Propagate all allocator operations.
};
template <>
struct TestCases_StatefulAllocatorConfiguration1<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "fully propagating stateful STL allocators";
    }
};

template <class ELEMENT>
struct TestCases_StatefulAllocatorConfiguration2
     : TestCases<AllocatorPropagationParams<ELEMENT, false> > {
    // Propagate all allocator operations but copy construction.
};
template <>
struct TestCases_StatefulAllocatorConfiguration2<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "copy non-propagating stateful STL allocators";
    }
};

template <class ELEMENT>
struct TestCases_StatefulAllocatorConfiguration3
     : TestCases<AllocatorPropagationParams<ELEMENT, true, false> > {
    // Propagate all allocator operations but swap.
};
template <>
struct TestCases_StatefulAllocatorConfiguration3<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "swap non-propagating stateful STL allocators";
    }
};
#endif  // BSLSTL_ALLOCATOR_TRAITS_COMPUTES_ALLOCATOR_PROPAGATION

// - - - - Special configurations for testing the default constructor - - - - -

// These configurations are required only for a special set of tests using the
// default constructor for HashTable, which can be instantiated for hasher and
// comparator types that are default constructible, but awkward in every other
// way.  This will be the only test driver to examine the limits of default
// constructible (only) types.

template <class ELEMENT>
struct DefaultOnlyFunctorParams {
    // TYPES
    typedef BasicKeyConfig<        ELEMENT>  KeyConfig;
    typedef DefaultOnlyHasher<     ELEMENT>  Hasher;
    typedef DefaultOnlyComparator< ELEMENT>  Comparator;
    typedef bsl::allocator<        ELEMENT>  Allocator;
};

template <class ELEMENT>
struct TestCases_DefaultOnlyFunctors
     : TestCases<DefaultOnlyFunctorParams<ELEMENT> > {
};
template <>
struct TestCases_DefaultOnlyFunctors<void> {
    // CLASS METHODS
    static inline const char *concernKind() {
        return "non-copyable functors";
    }
};

struct AwkwardMaplikeForDefaultParams {
    // TYPES
    typedef TrickyKeyConfig                                   KeyConfig;
    typedef DefaultOnlyHasher<     TrickyKeyConfig::KeyType>  Hasher;
    typedef DefaultOnlyComparator< TrickyKeyConfig::KeyType>  Comparator;
    typedef bsl::allocator<        TrickyKeyConfig::KeyType>  Allocator;
};

// - - - - - - - - - - - - Test Driver Specific Macros - - - - - - - - - - - -

#define u_RUN_HARNESS_(HarnessTemplateName, memberFunc, ...) do {        \
    if (verbose) printf("\n\t%s\n",                                       \
                        HarnessTemplateName<void>::concernKind());         \
    RUN_EACH_TYPE(HarnessTemplateName, memberFunc, __VA_ARGS__); } while(0)

#define u_TEST_TYPES_ALL BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

#define u_TEST_TYPES_USER_DEFINED                            \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED

/// Note that `NonEqualComparableTestType` is not applicable here, because
/// the test case uses `operator==` on the container, so cannot work with
/// container-elements that do not define an `operator==`.
#define u_TESTED_TYPES_HAS_EQ                                 \
                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                bsltf::NonAssignableTestType,                   \
                bsltf::NonDefaultConstructibleTestType

/// Note that `NonEqualComparableTestType` is not applicable here, because
/// the test case uses `operator==` on the container, so cannot work with
/// container-elements that do not define an `operator==`.
#define u_TESTED_TYPES_HAS_EQ_PLUS_EVIL                               \
                       BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,  \
                       bsltf::NonAssignableTestType,                    \
                       bsltf::NonDefaultConstructibleTestType,           \
                       TestTypes::MostEvilTestType

// case 4
#define u_NOALLOC_PARTS_IN_TC4 BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD,\
                               BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE

// case 10

/// The type-list below must be non-allocating types.
#define u_YET_ANOTHER_NOALLOC BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,\
                              bsltf::NonAssignableTestType,                   \
                              bsltf::NonDefaultConstructibleTestType

// case 10
#define u_DEGEN_NOSWAP_COPYASSIGN u_TESTED_TYPES_HAS_EQ_WITH_EVIL,     \
                                   bsltf::AllocTestType,                \
                                   bsltf::AllocBitwiseMoveableTestType
        // Degenerate functors are not CopyAssignable, and rely on the
        // copy/swap idiom for the copy-assignment operator to function.

namespace {
                    // =============================
                    // class TransparentlyComparable
                    // =============================

// When one of these is passed to an `emplace`-like method, it is passed by
// universal reference, and can (and should) be converted to int as a non-const
// object.  However, C++03 doesn't have universal references, and instead we
// pass the object as a const lvalue.  This causes the creation of the object
// in the container to fail (at compile time), because `operator int()` is
// non-const.  So, for C++03, we make the conversion work with a const source.
class TransparentlyComparable {
    // DATA
#if BLSL_COMPILERFEATURES_CPLUPLUS < 201103L
    mutable
#endif
    int d_conversionCount;  // number of times `operator int` has been called
    int d_value;            // the value

    // NOT IMPLEMENTED
    TransparentlyComparable(const TransparentlyComparable&);  // = delete

  public:
    // CREATORS

    /// Create an object having the specified `value`.
    explicit TransparentlyComparable(int value)

    : d_conversionCount(0)
    , d_value(value)
    {
    }

    // MANIPULATORS

    /// Return the current value of this object.
    operator int()
#if BLSL_COMPILERFEATURES_CPLUPLUS < 201103L
    const
#endif
    {
        ++d_conversionCount;
        return d_value;
    }

    void resetConversionCount ()
    {
        d_conversionCount = 0;
    }
    // ACCESSORS

    /// Return the number of times `operator int` has been called.
    int conversionCount() const
    {
        return d_conversionCount;
    }

    /// Return the current value of this object.
    int value() const
    {
        return d_value;
    }

    /// Return `true` if the value of the specified `lhs` is equal to the
    /// specified `rhs`, and `false` otherwise.
    friend bool operator==(const TransparentlyComparable& lhs, int rhs)
    {
        return lhs.d_value == rhs;
    }

    /// Return `true` if the specified `lhs` is equal to the value of the
    /// specified `rhs`, and `false` otherwise.
    BSLA_MAYBE_UNUSED
    friend bool operator==(int lhs, const TransparentlyComparable& rhs)
    {
        return lhs == rhs.d_value;
    }
};

                      // ==========================
                      // struct MapKeyConfiguration
                      // ==========================

template <class KEY, class VALUE_TYPE>
struct MapKeyConfiguration {
  public:
    typedef   VALUE_TYPE    ValueType;
    typedef   KEY           KeyType;

    // CLASS METHODS

    /// Return the member `first` of the specified object `obj`.
    /// `obj.first` must be of type `VALUE_TYPE::first_type`, which is the
    /// `key` portion of `obj`.
    static const KeyType& extractKey(const VALUE_TYPE& obj)
    {
        return obj.first;
    }
};

                      // ==========================
                      // struct SetKeyConfiguration
                      // ==========================

template <class VALUE_TYPE>
struct SetKeyConfiguration {
  public:
    typedef VALUE_TYPE ValueType;
    typedef ValueType  KeyType;

    // CLASS METHODS

    /// Given a specified `object`, return a reference to the `KeyType`
    /// contained within that object.  In this case, the `KeyType` returned
    /// is simply the object itself.
    static const KeyType& extractKey(const VALUE_TYPE& obj)
    {
        return obj;
    }
};

                    // ============================
                    // struct TransparentComparator
                    // ============================

/// This class can be used as a comparator for containers.  It has a nested
/// type `is_transparent`, so it is classified as transparent by the
/// `bslmf::IsTransparentPredicate` metafunction and can be used for
/// heterogeneous comparison.
struct TransparentComparator
 {
    typedef void is_transparent;

    /// Return `true` if the specified `lhs` is equivalent to the specified
    /// `rhs` and `false` otherwise.
    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const
    {
        return lhs == rhs;
    }
};

                      // ========================
                      // struct TransparentHasher
                      // ========================

/// This class can be used as a comparator for containers.  It has a nested
/// type `is_transparent`, so it is classified as transparent by the
/// `bslmf::IsTransparentPredicate` metafunction and can be used for
/// heterogeneous comparison.
struct TransparentHasher
 {
    typedef void is_transparent;

    size_t operator () (const TransparentlyComparable &value) const
    {
        return static_cast<size_t>(value.value());
    }

    template <class VALUE>
    size_t operator()(const VALUE &value) const
    {
        return static_cast<size_t>(value);
    }
};

/// Search for a key equal to the specified `initKeyValue` in the specified
/// `container`, and count the number of conversions expected based on the
/// specified `isTransparent`.  Note that `Container` may resolve to a
/// `const`-qualified type, we are using the "reference" here as a sort of
/// universal reference; conceptually, the object remains constant, but we
/// want to test `const`-qualified and non-`const`-qualified overloads.
template <class Container>
void testSetTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
{
    typedef typename bslalg::BidirectionalLink      *Link;
    typedef typename Container::SizeType             Count;
    typedef typename Container::ValueType            Value;

    const int expectedConversionCount = isTransparent ? 0 : 1;

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    {
        // Testing `find`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Link EXISTING_F = container.find(existingKey);

        ASSERT(NULL                          !=  EXISTING_F);
        ASSERT(existingKey.value()           ==
         static_cast<bslalg::BidirectionalNode<Value> *>(EXISTING_F)->value());
        ASSERT(existingKey.conversionCount() == expectedConversionCount);

        const Link NON_EXISTING_F = container.find(nonExistingKey);
        ASSERT(NULL                             == NON_EXISTING_F);
        ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);
    }

    {
        // Testing `findRange`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        Link first, last;

        container.findRange(&first, &last, existingKey);

        ASSERT(expectedConversionCount == existingKey.conversionCount());
        ASSERT(first != last);
        for (Link it = first; it != last; it = it->nextLink()) {
            ASSERT(existingKey.value() ==
                 static_cast<bslalg::BidirectionalNode<Value> *>(it)->value());
        }

        container.findRange(&first, &last, nonExistingKey);
        ASSERT(first == last);
        ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
    }


    {
        // Testing `bucketIndexForKey`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Count bucketFound    = container.bucketIndexForKey(existingKey);
        const Count bucketNotFound = container.bucketIndexForKey(nonExistingKey);

        ASSERTV(expectedConversionCount,        existingKey.conversionCount(),
                  expectedConversionCount ==    existingKey.conversionCount());
        ASSERTV(expectedConversionCount,     nonExistingKey.conversionCount(),
                  expectedConversionCount == nonExistingKey.conversionCount());
    }
}

/// Search for a key equal to the specified `initKeyValue` in the specified
/// `container`, and count the number of conversions expected based on the
/// specified `isTransparent`.  Note that `Container` may resolve to a
/// `const`-qualified type, we are using the "reference" here as a sort of
/// universal reference; conceptually, the object remains constant, but we
/// want to test `const`-qualified and non-`const`-qualified overloads.
template <class Container>
void testMapTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
{
    typedef typename bslalg::BidirectionalLink      *Link;
    typedef typename Container::SizeType             Count;
    typedef typename Container::ValueType            Value;

    const int expectedConversionCount = isTransparent ? 0 : 1;

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    {
        // Testing `find`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Link EXISTING_F = container.find(existingKey);

        ASSERT(NULL                          !=  EXISTING_F);
        ASSERT(existingKey.value()           ==
               static_cast<bslalg::BidirectionalNode<Value> *>
                                                  (EXISTING_F)->value().first);
        ASSERT(existingKey.conversionCount() == expectedConversionCount);

        const Link NON_EXISTING_F = container.find(nonExistingKey);
        ASSERT(NULL                             == NON_EXISTING_F);
        ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);
    }

    {
        // Testing `findRange`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        Link first, last;

        container.findRange(&first, &last, existingKey);

        ASSERT(expectedConversionCount == existingKey.conversionCount());
        ASSERT(first != last);
        for (Link it = first; it != last; it = it->nextLink()) {
            ASSERT(existingKey.value() ==
                static_cast<bslalg::BidirectionalNode<Value> *>
                                                          (it)->value().first);
        }

        container.findRange(&first, &last, nonExistingKey);
        ASSERT(first == last);
        ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
    }


    {
        // Testing `bucketIndexForKey`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Count bucketFound    = container.bucketIndexForKey(existingKey);
        const Count bucketNotFound = container.bucketIndexForKey(nonExistingKey);

        ASSERTV(expectedConversionCount,        existingKey.conversionCount(),
                  expectedConversionCount ==    existingKey.conversionCount());
        ASSERTV(expectedConversionCount,     nonExistingKey.conversionCount(),
                  expectedConversionCount == nonExistingKey.conversionCount());
    }
}

/// Search for a value equal to the specified `initKeyValue` in the
/// specified `container`, and count the number of conversions expected
/// based on the specified `isTransparent`.  Since these tests can modify
/// the container, we make a copy of it for each test.
template <class Container>
void testSetTransparentComparatorMutable(const Container& container,
                                      bool             isTransparent,
                                      int              initKeyValue)
{
    // No transparent modifying operations on sets.
}

/// Search for a value equal to the specified `initKeyValue` in the
/// specified `container`, and count the number of conversions expected
/// based on the specified `isTransparent`.  Since these tests can modify
/// the container, we make a copy of it for each test.
template <class Container>
void testMapTransparentComparatorMutable(const Container& container,
                                      bool             isTransparent,
                                      int              initKeyValue)
{
//     bslalg::BidirectionalLink *>::type tryEmplace(

    typedef typename bslalg::BidirectionalLink      *Link;
    typedef typename Container::SizeType             Count;
    typedef typename Container::ValueType            Value;

    const int expectedConversionCount = isTransparent ? 0 : 1;
    const Count size = container.size();

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    {
        // Testing `tryEmplace`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        Container c(container);
        bool      wasInserted;
        Link      link;

        // with an existing key
        link = c.tryEmplace(&wasInserted, NULL, existingKey);
        ASSERT(!wasInserted);
        ASSERT(size == c.size());
        ASSERT(existingKey.value() ==
         static_cast<bslalg::BidirectionalNode<Value> *>(link)->value().first);
        ASSERTV(isTransparent,
                expectedConversionCount,   existingKey.conversionCount(),
                expectedConversionCount == existingKey.conversionCount());

        // with a non-existing key

        // Note: We always get a conversion here; if we don't have a
        // transparent comparator, then the value gets converted, and when the
        // lookup fails, it gets inserted into the map.  If we do have a
        // transparent comparator, the lookup is done w/o conversion, but then
        // the value gets converted in order to put it into the map.
        link = c.tryEmplace(&wasInserted, NULL, nonExistingKey);
        ASSERT( wasInserted);
        ASSERT(size + 1 == c.size());
        ASSERT(nonExistingKey.value() ==
         static_cast<bslalg::BidirectionalNode<Value> *>(link)->value().first);
        ASSERT(1 == nonExistingKey.conversionCount());
    }

}

}  // close unnamed namespace

//- - - - - - - - - - Test Machinery Verification Helpers - - - - - - - - - - -

//@bdetdsplit FOR 3 BEGIN

/// Return the original specified `value`, and set `value` to the specified
/// `newValue`.  Note that this function is a specific implementation of a
/// standard C++11 template, intended to eliminate use of the deprecated
/// post-increment operator on `bool`.
static inline bool exchange(bool &value, bool newValue)
{
    const bool result = value;
    value = newValue;
    return result;
}

template<class KEY_CONFIG, class COMPARATOR>
void testVerifyListContents(const COMPARATOR& compare)
    // ------------------------------------------------------------------------
    // VERIFY TEST MACHINERY FUNCTION `verifyListContents`
    //   This function tests the `verifyListContents` test machinery function
    //   using the specified `compare` functor to determine that two
    //   list elements have the same value.
    //
    // Concerns:
    // 1. Passing a null pointer value for the link returns an error code,
    //    unless the expected sequence has a length of zero.
    //
    // 2. Passing an expected size that is not the same as the length of the
    //    list rooted at passed link returns an error code.
    //
    // 3. Passing an expected size that is larger than the size of the passed
    //    value-array returns an error code.
    //
    // 4. Passing a value-list longer than the expected size is not an error.
    //
    // 5. An error code is returned if there are duplicate elements in the
    //    passed list, as determined by the passed comparator, and those
    //    duplicates do not form a contiguous subsequence within the list.
    //
    // 6. Success code of `0` is returned if the list rooted at the passed link
    //    holds elements corresponding to a permutation of the first N values
    //    in the value-list, where N is the length of the list.
    //
    // 7. If the list is not a permutation of the first N values, then return
    //    the number of elements that do not correspond to values in the
    //    first N positions of the value-list.
    //
    // 8. No memory is allocated by either the global or default allocators.
    //
    // Plan:
    // 1. TBD
    //
    // Testing:
    //   verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    // ------------------------------------------------------------------------
{
    static bool alreadyTested = false;
    if (exchange(alreadyTested, true)) {
        // This function may be invoked many times, but each instantiation need
        // only run the first time.

        return;                                                       // RETURN
    }

    typedef typename KEY_CONFIG::ValueType    ValueType;
    typedef bsltf::TestValuesArray<ValueType> TestValues;

    bslma::TestAllocator *gta = dynamic_cast<bslma::TestAllocator *>(
                                            bslma::Default::globalAllocator());
    if (!gta) {
        puts("Global allocator is not a `TestAllocator`, aborting.");
        abort();
    }

    bslma::TestAllocatorMonitor gam(gta);

    bslma::TestAllocator da("default testing verifyListContents",
                            veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocatorMonitor dam(&da);

    bslma::TestAllocator pa("pool allocator", veryVeryVeryVerbose);
    bslstl::BidirectionalNodePool<ValueType, bsl::allocator<ValueType> >
                                                                    pool(&pa);

    if (veryVerbose) puts("\nTesting verifyListContents initial conditions.");
    {
        static const struct {
            int         d_line;      // source line number
            const char *d_listSpec;  // specification string to build list
            const char *d_valSpec;   // specification string for test values
            size_t      d_expSz;     // expected size
            int         d_error;     // expected error code
        } DATA[] = {
            //line  list    vals  expSz  error
            //----  ----    ----  -----  -----
            { L_,     "",     "",     0,     0 }, // control
            { L_,     "",     "",     1,    -1 },
            { L_,     "",    "A",     0,     0 },
            { L_,     "",    "A",     1,    -1 },

            { L_,    "A",     "",     0,    -3 },
            { L_,    "A",     "",     1,    -4 },
            { L_,    "A",    "A",     0,    -3 },
            { L_,    "A",    "A",     1,     0 },
            { L_,    "A",    "A",     2,    -4 },
            { L_,    "A",    "B",     0,    -3 },
            { L_,    "A",    "B",     1,     1 },
            { L_,    "A",    "B",     2,    -4 },

            { L_,   "AA",     "",     0,    -3 },
            { L_,   "AA",     "",     1,    -4 },
            { L_,   "AA",     "",     2,    -4 },
            { L_,   "AA",     "",     3,    -4 },
            { L_,   "AA",    "A",     2,    -4 },
            { L_,   "AA",    "B",     2,    -4 },
            { L_,   "AA",   "AA",     2,     0 },
            { L_,   "AA",   "AB",     2,     1 },
            { L_,   "AA",   "BB",     2,     2 },
            { L_,   "AA",  "AAA",     2,     0 },
            { L_,   "AA",  "AAZ",     2,     0 },
            { L_,   "AA",  "ABC",     2,     1 },
            { L_,   "AA",  "AAZ",     3,    -2 },
            { L_,   "AA",  "BCD",     3,    -2 },

            { L_,   "AB",    "A",     2,    -4 },
            { L_,   "AB",    "B",     2,    -4 },
            { L_,   "AB",   "AA",     2,     1 },
            { L_,   "AB",   "AB",     2,     0 },
            { L_,   "AB",   "AC",     2,     1 },
            { L_,   "AB",   "BA",     2,     0 },
            { L_,   "AB",   "BB",     2,     1 },
            { L_,   "AB",   "BC",     2,     1 },
            { L_,   "AB",   "CA",     2,     1 },
            { L_,   "AB",   "CB",     2,     1 },
            { L_,   "AB",   "CC",     2,     2 },
            { L_,   "AB",  "AAA",     2,     1 },
            { L_,   "AB",  "ABC",     2,     0 },
            { L_,   "AB",  "ACB",     2,     1 },

            { L_,  "AAA",     "",     2,    -4 },
            { L_,  "AAA",  "AAA",     2,    -3 },
            { L_,  "AAA",  "AAA",     3,     0 },
            { L_,  "AAA",  "AAA",     4,    -4 },
            { L_,  "AAA",  "AAB",     3,     1 },
            { L_,  "AAA",  "ABA",     3,     1 },
            { L_,  "AAA",  "BAA",     3,     1 },
            { L_,  "AAA",  "ABB",     3,     2 },
            { L_,  "AAA",  "ABC",     3,     2 },
            { L_,  "AAA",  "BCD",     3,     3 },

            { L_,  "AAX",  "AAX",     3,     0 },
            { L_,  "AAX",  "AXA",     3,     0 },
            { L_,  "AXA",  "AAX",     3,    -5 },
            { L_,  "XAA",  "AAX",     3,     0 },
            { L_,  "BCA",  "CAB",     3,     0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti != NUM_DATA ; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const char *const LIST_SPEC = DATA[ti].d_listSpec;
            const char *const VAL_SPEC  = DATA[ti].d_valSpec;
            const size_t      EXP_SIZE  = DATA[ti].d_expSz;
            const int         EXP_ERR   = DATA[ti].d_error;

            bslma::TestAllocator tva("test values", veryVeryVeryVerbose);
            const TestValues  EXP_VALS(VAL_SPEC, &tva);

            bslma::TestAllocator lva("list values", veryVeryVeryVerbose);
            const TestValues  LIST_VALS(LIST_SPEC, &lva);

            Link *pRoot = 0;
            for (size_t i = LIST_VALS.size(); i; /**/) {
                Link *newNode = pool.emplaceIntoNewNode(LIST_VALS[--i]);
                bslalg::BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                                     newNode,
                                                                     pRoot);
                pRoot = newNode;
            }

            int failCode = verifyListContents<KEY_CONFIG>(pRoot,
                                                          compare,
                                                          EXP_VALS,
                                                          EXP_SIZE);
            ASSERTV(LINE, EXP_ERR, failCode, EXP_ERR == failCode);

            // Note that this clean-up loop at the end of scope is not
            // exception safe, but we know our test types should not throw
            // under these test conditions.

            while (pRoot) {
                Link *next = pRoot->nextLink();
                pool.deleteNode(pRoot);
                pRoot = next;
            }
        }
    }

    ASSERT(0 == dam.numBlocksTotalChange());
    ASSERT(0 == gam.numBlocksTotalChange());
}
//@bdetdsplit FOR 3 END

// - - - - - Main test driver harness, that implements the test cases - - - - -

/// This struct template provides a namespace for testing the `HashTable`
/// container.  Parameters are members of `CONFIGURATION_PARAMETERS`.  The
/// `KeyConfig`, `Hasher`, `Comparator` and `Allocator` type members
/// directly correspond to the `KEY_CONFIG`, `HASHER`, `COMPARATOR`, and
/// `ALLOCATOR` type parameters of the `bslstl::HashTable` template.  The
/// `concernKind` class method provides a short description of a concern the
/// full configuration (in `CONFIGURATION_PARAMETERS`) tests, for example
/// "functor references", or "stateful STL allocators".  Each `testCase*`
/// method tests a specific aspect of
/// `HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>`.  Each test case
/// should be instantiated and invoked with various
/// `CONFIGURATION_PARAMETERS` to fully test the container.
template <class CONFIGURATION_PARAMETERS>
class TestCases {

  private:
    // TYPES

    /// Shorthand
    typedef CONFIGURATION_PARAMETERS           ConfigParams;

    typedef typename ConfigParams::KeyConfig   KEY_CONFIG;
    typedef typename ConfigParams::Hasher      HASHER;
    typedef typename ConfigParams::Comparator  COMPARATOR;

    /// "Extracted" type parameters
    typedef typename ConfigParams::Allocator   ALLOCATOR;

    typedef typename bslstl::CallableVariable<    HASHER>::type     HASHER_VAR;

    /// See `bslstl::CallableVariable`.  Probably want to pick these up as
    /// values from some injected policy, so that we can test with stateful
    /// variants.  Alternatively, pass some seed to the `make` function to
    /// support stateful functors actually having different states.  Note
    /// that we need the `make` function to supply a default state for
    /// functors that are not default constructible.
    typedef typename bslstl::CallableVariable<COMPARATOR>::type COMPARATOR_VAR;

    /// The type under test
    typedef bslstl::HashTable<KEY_CONFIG,
                              HASHER,
                              COMPARATOR,
                              ALLOCATOR>       Obj;

    typedef typename Obj::SizeType             SizeType;
    typedef typename Obj::KeyType              KeyType;

    /// Shorthands
    typedef typename Obj::ValueType            ValueType;

    typedef bsltf::TestValuesArray<ValueType>  TestValues;

    typedef ObjectMaker<KEY_CONFIG,
                        HASHER,
                        COMPARATOR,
                        ALLOCATOR>             ObjMaker;

  private:
    // PRIVATE CLASS METHODS
    static
    typename MakeCallableEntity<HASHER>::CallableEntityType makeHasher()
    {
        return MakeCallableEntity<HASHER>::make();
    }

    static
    typename MakeCallableEntity<COMPARATOR>::CallableEntityType
    makeComparator()
    {
        return MakeCallableEntity<COMPARATOR>::make();
    }

    static
    ALLOCATOR makeAllocator(bslma::Allocator *basicAllocator)
    {
        return MakeAllocator< ALLOCATOR>::make(basicAllocator);
    }

    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given `spec` in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the `map<KEY, VALUE, COMP, ALLOC>` object.
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

    /// Return, by reference, the specified `object` with its value adjusted
    /// according to the specified `spec`.
    static Obj& gg(Obj *object, const char *spec)
    {
        const int result = ggg(object, spec);
        ASSERTV(result, -1 == result);
        return *object;
    }

    /// Configure the specified `object` according to the specified `spec`,
    /// using only the primary manipulator function `insert` and white-box
    /// manipulator `clear`.  Optionally specify `verbosity` as `false` to
    /// suppress `spec` syntax error messages.  Return the index of the
    /// first invalid character, and a negative value otherwise.  Returned
    /// values lower than `-1` indicate some error other than parsing the
    /// `spec` string.  Note that this function is used to implement `gg` as
    /// well as allow for verification of syntax error detection.
    static int ggg(Obj *object, const char *spec, bool verbosity = true);

  public:
    // TEST CASES

    // Test case 1 is "BREATHING TEST", in `main`, it is not a template

    static void traitsAndCompileTime();

    static void testCase2();
    static void testCase3();//@bdetdsplit FOR 3
    static void testCase4();//@bdetdsplit FOR 4
 // static void testCase5();   no output operator
    static void testCase6();//@bdetdsplit FOR 6
    static void testCase7();//@bdetdsplit FOR 7
 // static void testCase8();   move constructor TBD
    static void testCase9();//@bdetdsplit FOR 9
    static void testCase10();//@bdetdsplit FOR 10
//  static void testCase11();  Move assignment TBD
//  static void testCase12();  no bslx streaming
    static void testCase13();//@bdetdsplit FOR 13
    static void testCase14();//@bdetdsplit FOR 14
    static void testCase15();//@bdetdsplit FOR 15
    static void testCase16();//@bdetdsplit FOR 16
    static void testCase17();//@bdetdsplit FOR 17
    static void testCase18();//@bdetdsplit FOR 18
    static void testCase19();//@bdetdsplit FOR 19
    static void testCase20();//@bdetdsplit FOR 20
    static void testCase21();//@bdetdsplit FOR 21
    static void testCase22();//@bdetdsplit FOR 22
    static void testCase23();//@bdetdsplit FOR 23
};

#if defined(BSLS_PLATFORM_CMP_MSVC)
    #pragma warning(pop)
    // qualifier applied to function type has no meaning: ignored
#endif  // Function types warning need to be disabled on MSVC

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                              // ---------------
                              // class TestCases
                              // ---------------

// TEST APPARATUS
template <class CONFIGURATION_PARAMETERS>
int TestCases<CONFIGURATION_PARAMETERS>::ggg(Obj        *object,
                                             const char *spec,
                                             bool        verbosity)
{
    enum { SUCCESS = -1 };

    if (!object) {
        return -2;                                                    // RETURN
    }

    if (!spec) {
        return -3;                                                    // RETURN
    }

    if (object->rehashThreshold() < object->size() + strlen(spec)) {
        return -4;                                                    // RETURN
    }

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int i = 0; spec[i]; ++i) {
        if (spec[i] <'A' || 'Z' < spec[i]) {
            if (verbosity) {
                printf("Error, bad character (`%c`) "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;   // Discontinue processing this spec.          // RETURN
        }

        if (!insertElement(object, VALUES[spec[i] - 'A'])) {
            if (verbosity) {
                printf("Unexpected error inserting element no. %d from the "
                       "spec string (`%s`).\n", i, spec);
            }
            return -100 -i;   // Discontinue processing this spec.    // RETURN
        }
   }
   return SUCCESS;
}

// TEST CASES
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::traitsAndCompileTime()
{
    // ------------------------------------------------------------------------
    // traits and other compile-time properties
    //
    // Concerns:
    // 1. HashTable supports the expected traits
    //
    // 2. HashTable has the expected size
    //
    // Plan:
    // 1. For each value of increasing length, `L`:
    //
    //
    // Testing:
    //   traits
    //   sizeof(HashTable)
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    ASSERT((bsl::is_same<typename Obj::KeyType,   KeyType>::value));
    ASSERT((bsl::is_same<typename Obj::ValueType, ValueType>::value));

    // These traits should never be true

    ASSERT(!bsl::is_trivially_copyable<             Obj>::value);
    ASSERT(!bsl::is_trivially_default_constructible<Obj>::value);
    ASSERT(!bslmf::IsBitwiseCopyable<               Obj>::value);
    ASSERT(!bslmf::IsBitwiseEqualityComparable<     Obj>::value);

    // The uses-allocator trait should depend only on the allocator type.

    bool usesBslAllocator        = IsBslAllocator<ALLOCATOR>::value;
    bool objUsesBslmaAllocator   = bslma::UsesBslmaAllocator<Obj>::value;

    ASSERTV(usesBslAllocator, objUsesBslmaAllocator,
            usesBslAllocator == objUsesBslmaAllocator);

    // The stored type has no bearing on whether a node-based container is
    // bitwise-movable.  However, the allocator and two custom function-like
    // types must all be bitwise-movable for the whole container to be
    // bitwise-movable.

    bool allocIsBitwiseMoveable = bslmf::IsBitwiseMoveable<ALLOCATOR>::value;
    bool hashIsBitwiseMoveable  = bslmf::IsBitwiseMoveable<HASHER>::value;
    bool compIsBitwiseMoveable  = bslmf::IsBitwiseMoveable<COMPARATOR>::value;
    bool objIsBitwiseMoveable   = bslmf::IsBitwiseMoveable<Obj>::value;

    ASSERTV(allocIsBitwiseMoveable,
            hashIsBitwiseMoveable,
            compIsBitwiseMoveable,
            objIsBitwiseMoveable,
     (allocIsBitwiseMoveable && hashIsBitwiseMoveable && compIsBitwiseMoveable)
                                                      == objIsBitwiseMoveable);
}

//@bdetdsplit FOR 2 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase2()
{
    // ------------------------------------------------------------------------
    // PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that a `HashTable` object can be constructed into
    //   a (valid) well-defined state, then through use of manipulators brought
    //   into any other valid state, and finally that the object destroys all
    //   its elements and leaks no memory on destruction.  For the purposes of
    //   testing, the well-defined state will be a `HashTable` having no
    //   elements, having the specified or default-constructed allocator,
    //   having a hasher and comparator supplied by the `MakeCallableEntity`
    //   factory function (which provides a default-constructed functor where
    //   available, and is specialized to provide a standard functor object
    //   otherwise), and having a number of buckets calculated to be sufficient
    //   to support the largest number of elements required for that test
    //   iteration, at the specified maximum load factor.  The primary
    //   manipulators will be a free function (`insertElement`, created for the
    //   purpose of testing) that inserts a single element of a specific type,
    //   but only if there is sufficient capacity in the table to insert
    //   without requiring a rehash, and the `removeAll` method.
    //
    // Concerns:
    //  1. An object created with the value constructor (with or without a
    //     supplied allocator) has the supplied hasher, comparator, allocator
    //     and maxLoadFactor, and at least the supplied initial number of
    //     buckets.
    //
    //  2. The number of buckets is 1 or a prime number.
    //
    //  2. If the requested number of buckets cannot be supplied, an exception
    //     is thrown (need to nail down the contract on which exception)
    //
    //  2. If more than zero buckets are requested, there shall be at least
    //     enough buckets to insert one element, or else the constructor will
    //     fail with an exception.
    //
    //  2. The maximum load factor may be arbitrarily large, including
    //     infinity.
    //
    //  2. The maximum load factor may be arbitrarily tiny, as long as it is
    //     positive.
    //
    //  2. QoI: An assertion will be triggered in appropriate build modes if
    //     the requested max load factor is zero, negative, or a NaN.
    //
    //  3. If the allocator is a `bsl::allocator` and an allocator is NOT
    //     supplied to the value constructor, the default allocator in effect
    //     at the time of construction becomes the object allocator for the
    //     resulting object.
    //
    //  4. If the allocator is not a `bsl::allocator` and an allocator is NOT
    //     supplied to the value constructor, the default constructed allocator
    //     becomes the object allocator for the resulting object.
    //
    //  5. If an allocator IS supplied to the default constructor, that
    //     allocator becomes the object allocator for the resulting object.
    //
    //  6. If the allocator is a `bsl::allocator`, supplying a null allocator
    //     address has the same effect as not supplying an allocator.
    //
    //  7. Supplying an allocator to the value constructor has no effect on
    //     subsequent object values.
    //
    //  8. Any memory allocation is from the object allocator.
    //
    //  9. There is no temporary allocation from any allocator.
    //
    // 10. If any constructor fails by throwing an exception, no memory is
    //     leaked.
    //
    // 10. Every object releases any allocated memory at destruction.
    //
    // 11. QoI: The value constructor allocates no memory if the initial number
    //     of bucket is 0.
    //
    // 12. `insertElement` increases the size of the object by 1.
    //
    // 13. `insertElement` returns the address of the newly added element.
    //
    // 14. `insertElement` puts the element into the list of elements defined
    //     by `elementListRoot`.
    //
    // 15. `insertElement` adds an additional element in the bucket returned by
    //     the `bucketFromKey` method.
    //
    // 16. `insertElement` returns a null pointer if adding one more element
    //     will exceed the `maxLoadFactor`.
    //
    // 17. Elements having the same keys (retrieved from the `extractKey`
    //     method of the KEY_CONFIG) according to the supplied comparator are
    //     inserted contiguously at the beginning of the range of existing
    //     equivalent elements, without changing their relative order.
    //
    // 18. `removeAll` properly destroys each contained element value.
    //
    // 19. `removeAll` does not allocate memory.
    //
    // 20. Any argument can be `const`.
    //
    // 21. Any memory allocation is exception neutral.
    //
    // 22. The constructor fails by throwing a `std::length_error` if the
    //     initial length of the bucket array cannot be computed.
    //
    // 23. The constructor fails by the allocator throwing an exception if the
    //     initial bucket array is too large to allocate.
    //
    // Plan:
    // 1. For each value of increasing length, `L`:
    //
    //   2. Using a loop-based approach, value-construct three distinct empty
    //      objects, in turn, but configured differently: (a) without passing
    //      an allocator, (b) passing a null allocator address explicitly,
    //      and (c) passing the address of a test allocator distinct from the
    //      default.  For each of these three iterations:  (C-1..14)
    //
    //     1. Create three `bslma::TestAllocator` objects, and install one as
    //        the current default allocator (note that a ubiquitous test
    //        allocator is already installed as the global allocator).
    //
    //     2. Use the default constructor to dynamically create an object
    //        `X`, with its object allocator configured appropriately (see
    //        P-2); use a distinct test allocator for the object's footprint.
    //
    //     3. Use the (as yet unproven) `allocator` to ensure that its
    //        object allocator is properly installed.  (C-2..4)
    //
    //     4. Use the appropriate test allocators to verify that no memory is
    //        allocated by the default constructor.  (C-9)
    //
    //     5. Use the individual (as yet unproven) salient attribute accessors
    //        to verify the default-constructed value.  (C-1)
    //
    //     6. Insert `L - 1` elements in order of increasing value into the
    //        container.
    //
    //     7. Insert the `L`th value in the presense of exception and use the
    //        (as yet unproven) basic accessors to verify the container has the
    //        expected values.  Verify the number of allocation is as expected.
    //        (C-5..6, 14..15)
    //
    //     8. Verify that no temporary memory is allocated from the object
    //        allocator.  (C-7)
    //
    //     9. Invoke `clear` and verify that the container is empty.  Verify
    //        that no memory is allocated.  (C-12..13)
    //
    //    10. Verify that all object memory is released when the object is
    //        destroyed.  (C-8)
    //
    //    11. Insert `L` distinct elements and record the iterators returned.
    //
    //    12. Insert the same `L` elements again and verify that incrementing
    //        the iterators returned gives the iterator to the next smallest
    //        value.
    //
    //    13. Perform P-1.2.12 again.  (C-11)
    //
    //
    // Testing:
    //   HashTable(HASHER, COMPARATOR, SizeType, float, ALLOC)
    //   ~HashTable();
    //   void removeAll();
    //   insertElement      (test driver function, proxy for basic manipulator)
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::KeyType   Key;
    typedef typename KEY_CONFIG::ValueType Element;

    typedef bslalg::HashTableImpUtil       ImpUtil;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (veryVerbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);  // Contains 52 distinct increasing values.
    const SizeType MAX_LENGTH = 19; // This should be sufficient to bootstrap.

    {
        // Reassert a global invariant, that should not be necessary to state,
        // but did indeed turn up a real bug.

        bslma::Allocator *installedAlloc =
                     bsltf::StdTestAllocatorConfiguration::delegateAllocator();
        ASSERTV(installedAlloc,   g_bsltfAllocator_p,
                installedAlloc == g_bsltfAllocator_p);
    }

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (veryVerbose) puts("\nTesting fail-modes of `insertElement`.");
    {
        bslma::TestAllocator dummy("dummy allocator", veryVeryVeryVerbose);
        bsltf::StdTestAllocatorConfigurationGuard dummyGuard(&dummy);
        ALLOCATOR dummyAlloc = makeAllocator(&dummy);

        Link *inserted = insertElement(static_cast<Obj *>(0), VALUES[0]);
        ASSERT(0 == inserted);

        Obj mX(HASHF, COMPAREF, 0, 1.0f, dummyAlloc);
        ASSERTV(mX.rehashThreshold(), 0 == mX.rehashThreshold());

        inserted = insertElement(&mX, VALUES[0]);
        ASSERT(0 == inserted);

        // An infinite max load factor appears to imply "never rehash" but a
        // default-constructed HashTable has no storage.

        Obj mY(HASHF,
               COMPAREF,
               0,
               std::numeric_limits<float>::infinity(),
               dummyAlloc);
        ASSERTV(mY.rehashThreshold(), 0 == mY.rehashThreshold());

        inserted = insertElement(&mY, VALUES[0]);
        ASSERT(0 == inserted);
    }

    if (veryVerbose) puts("\nTesting with various allocator configurations.");

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
      for (SizeType ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            {
            bslma::DefaultAllocatorGuard dag(&da);

            // There is no easy way to create this guard for the specific test
            // case of the stateless `bsltf::StdTestAllocator`, nor without
            // second guessing the allocator to use based upon the `cfg` code.
            // By the time we return from `makeAllocator` the test allocator
            // will already have been installed, with no easy way to restore at
            // the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG('a' == CONFIG
                                                                   ? &sa
                                                                   : &da);

            // ----------------------------------------------------------------
            if (veryVerbose) puts("\n\tTesting bootstrap constructor");

            const SizeType NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            Obj       *objPtr;
            ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                       CONFIG,
                                                       &fa,
                                                       &sa,
                                                       HASHF,
                                                       COMPAREF,
                                                       NUM_BUCKETS,
                                                       MAX_LF);
            bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                       &fa);
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

            const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
            const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null pointer.
            BSLS_ASSERT_OPT(oa);
            BSLS_ASSERT_OPT(noa);

            // QoI: Verify no allocation from the object/non-object allocators
            // if no buckets are requested (as per the default constructor).
            if (0 == LENGTH) {
                ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                        0 ==  oa->numBlocksTotal());
            }
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Record blocks used by the initial bucket array
            const bsls::Types::Int64 INITIAL_OA_BLOCKS  = oa->numBlocksTotal();

            // Verify attributes of an empty container.

            // Note that not all of these attributes are salient to value.
            // None of these accessors are deemed tested until their own test
            // case, but many witnesses give us some confidence in the state.
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            // Verify that remove-all on a default container has no effect.
            // Specifically, no memory allocated, and the root of list and
            // bucket array are unchanged.

            mX.removeAll();

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                    INITIAL_OA_BLOCKS ==  oa->numBlocksTotal());
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Verify attributes of an empty container.

            // Note that not all of these attributes are salient to value.

            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket2 = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            ASSERTV(MAX_LF, LENGTH, CONFIG, &bucket == &bucket2);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                puts("\n\tTesting `insertElement` (bootstrap function)");
            }
            if (0 < LENGTH) {
                if (veryVerbose) printf("\t\tOn an object of initial length "
                                        ZU ".\n", static_cast<size_t>(LENGTH));

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa->numBlocksTotal(),   oa->numBlocksInUse(),
                        oa->numBlocksTotal() == oa->numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    Key key = ImpUtil::extractKey<KEY_CONFIG>(RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(elem),
                                     key));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                Element lastElem = VALUES[LENGTH - 1];
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(lastElem),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                // check elements with equivalent keys are contiguous

                // check expected elements are present in container, with
                // expected number of duplicates

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPAREF,
                                                           VALUES,
                                                           LENGTH));
            }

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting `removeAll`");
            {
                const bsls::Types::Int64 BB = oa->numBlocksTotal();

                mX.removeAll();

                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0  < X.numBuckets());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa->numBlocksTotal();

                ASSERTV(MAX_LF, LENGTH, CONFIG, BB == AA);
            }

            // ----------------------------------------------------------------

            // If we repeat the `insertElement` test, then memory usage is much
            // more predictable, as the node-pool should have sufficient free
            // nodes from the previous loop.

            if (veryVerbose) {
                puts("\n\tRepeat testing `insertElement`, with memory checks");
            }
            if (0 < LENGTH) {
                if (veryVerbose) printf("\t\tOn an object of initial length "
                                        ZU ".\n", static_cast<size_t>(LENGTH));

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(elem),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                Element lastElem = VALUES[LENGTH - 1];
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(lastElem),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPAREF,
                                                           VALUES,
                                                           LENGTH));
            }

            // ----------------------------------------------------------------

            if (veryVerbose)
                           puts("\n\tTesting `insert` with duplicated values");
            {
                Link *ITER[MAX_LENGTH + 1];

                // The first loop adds a duplicate in front of each already
                // inserted element
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != ITER[tj]);
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(elem),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        2 * LENGTH == X.size());

#if 0
                // The second loop adds another duplicate in front of each
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            X.COMPAREF()(
                                     KEY_CONFIG::extractKey(elem),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                         3 * LENGTH == X.size());
#endif
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            //fa.deleteObject(objPtr);
            }

            // Verify all memory is released on object destruction.

            ASSERTV(MAX_LF, LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
    }

    // ------------------------------------------------------------------------

    // Create some fresh allocators to use validating final corners of the
    // constructor behavior.  These are special case tests, and will not need
    // to loop many times, or test the contents of the container, so one set of
    // allocators will suffice to the end of the test case.

    bslma::TestAllocator da("corner-testing", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific test case of
    // the stateless `bsltf::StdTestAllocator`, nor without second guessing the
    // allocator to use based upon the `cfg` code.  By the time we return from
    // `makeAllocator` the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&da);
    ALLOCATOR objAlloc = makeAllocator(&da);

#if defined BDE_BUILD_TARGET_EXC
    if (veryVerbose)
               puts("\nTesting correct exceptions are thrown by constructor");
    {
        if (veryVerbose)
                     puts("\tmax SizeType buckets at max load factor of 1.0.");

        bool lengthErrorCaught    = false;
        bool wrongExceptionCaught = false;
        try {
            Obj mX(HASHF,
                   COMPAREF,
                   std::numeric_limits<SizeType>::max(),
                   1.0f,
                   objAlloc);
            ASSERT(false);
        }
        catch(const std::length_error&) {
            lengthErrorCaught = true;
        }
        catch(...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == lengthErrorCaught   );
        ASSERT(false == wrongExceptionCaught);

        if (veryVerbose)
               puts("\tmax SizeType buckets with an infinite max load factor");

        lengthErrorCaught    = false;
        wrongExceptionCaught = false;
        try {
            // Even with an infinite load factor, we cannot allocate more
            // buckets than fit into memory.

            Obj mX(HASHF,
                   COMPAREF,
                   std::numeric_limits<SizeType>::max(),
                   std::numeric_limits<float>::infinity(),
                   objAlloc);
            ASSERT(false);
        }
        catch(const std::length_error&) {
            lengthErrorCaught = true;
        }
        catch(...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == lengthErrorCaught   );
        ASSERT(false == wrongExceptionCaught);

#ifndef BSLS_PLATFORM_CPU_64_BIT
        // this will time out on 64-bit platforms, is it still probing anything
        // useful?  Yes - the `max` above will be larger than the max value in
        // the prime number table, and so generate a length_error from a
        // different check.
        if (veryVerbose) puts("\tmax `int` buckets at max load factor of 1.0");

        bool badAllocCaught  = false;
        wrongExceptionCaught = false;
        try {
            const Obj X(HASHF,
                        COMPAREF,
                        std::numeric_limits<int>::max(),
                        1.0f,
                        objAlloc);
            // It is unlikely this will fit into a 64-bit allocation, but we
            // will allow for the possibility.
            ASSERT(
                 static_cast<SizeType>(std::numeric_limits<int>::max())
                                                             < X.numBuckets());
        }
        catch(const std::bad_alloc&) {
            badAllocCaught = true;
        }
        catch(...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == badAllocCaught      );
        ASSERT(false == wrongExceptionCaught);
#endif  // not BSLS_PLATFORM_CPU_64_BIT
        if (veryVerbose)
                    puts("\tmax SizeType buckets with a tiny max load factor");

        lengthErrorCaught    = false;
        wrongExceptionCaught = false;
        try {
            Obj mBad(HASHF,
                     COMPAREF,
                     std::numeric_limits<SizeType>::max(),
                     1e-30f,
                     objAlloc);
            ASSERT(false);
        }
        catch(const std::length_error&) {
            lengthErrorCaught = true;
        }
        catch(...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == lengthErrorCaught   );
        ASSERT(false == wrongExceptionCaught);
    }
#endif

    if (veryVerbose) puts("\nNegative Testing.");
    {
        bsls::AssertTestHandlerGuard hG;

        const float FLT_TINY = std::numeric_limits<float>::denorm_min();
        const float FLT_NAN  = std::numeric_limits<float>::quiet_NaN();
        const float FLT_INF  = std::numeric_limits<float>::infinity();

        (void)FLT_TINY;
        (void)FLT_NAN;
        (void)FLT_INF;

        ASSERT_PASS_RAW(     Obj(HASHF, COMPAREF, 0,      1.0f, objAlloc));
        ASSERT_PASS_RAW(     Obj(HASHF, COMPAREF, 0,  FLT_TINY, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0,      0.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0,     -0.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0, -FLT_TINY, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0,     -1.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0,  -FLT_INF, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASHF, COMPAREF, 0,   FLT_NAN, objAlloc));
        ASSERT_PASS_RAW(     Obj(HASHF, COMPAREF, 0,   FLT_INF, objAlloc));
    }
}
//@bdetdsplit FOR 2 END

//@bdetdsplit FOR 3 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase3()
{
    // ------------------------------------------------------------------------
    // PRIMITIVE GENERATOR FUNCTIONS `gg` AND `ggg`:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    // 1. Valid generator syntax produces expected results
    //
    // 2. Invalid syntax is detected and reported.
    //
    // 3. `verifyListContents` confirms there is a one-to-one mapping between
    //    the supplied list and the expected values array, or both are empty.
    //
    //
    // Plan:
    // 1. For each of an enumerated sequence of `spec` values, ordered by
    //    increasing `spec` length:
    //
    //   1. Use the primitive generator function `gg` to set the state of a
    //      newly created object.
    //
    //   2. Verify that `gg` returns a valid reference to the modified argument
    //      object.
    //
    //   3. Use the basic accessors to verify that the value of the object is
    //      as expected.  (C-1)
    //
    // 2. For each of an enumerated sequence of `spec` values, ordered by
    //    increasing `spec` length, use the primitive generator function `ggg`
    //    to set the state of a newly created object.
    //
    //   1. Verify that `ggg` returns the expected value corresponding to the
    //      location of the first invalid value of the `spec`.  (C-2)
    //
    // Testing:
    //   int ggg(HashTable *object, const char *spec, bool verbosity);
    //   HashTable& gg(HashTable *object, const char *spec);
    //   verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    bslma::TestAllocator oa("generator allocator", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific test case of
    // the stateless `bsltf::StdTestAllocator`, nor without second guessing the
    // allocator to use based upon the `cfg` code.  By the time we return from
    // `makeAllocator` the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
    ALLOCATOR objAlloc = makeAllocator(&oa);

    if (veryVerbose) puts("\nTesting `testVerifyListContents`.");
    testVerifyListContents<KEY_CONFIG>(COMPAREF);

    if (veryVerbose) puts("\nTesting generator initial conditions.");
    {
        bslma::TestAllocator tda("bad generators", veryVeryVeryVerbose);
        Obj obj(HASHF, COMPAREF, 0, 1.0f, objAlloc);

        int failCode = ggg(&obj, "", veryVerbose);  // success!
        ASSERTV(&obj, failCode, -1 == failCode);

        failCode = ggg(0, "", veryVerbose);         // null OBJ address
        ASSERTV(failCode, -2 == failCode);

        failCode = ggg(&obj, 0, veryVerbose);       // null SPEC address
        ASSERTV(&obj, failCode, -3 == failCode);

        failCode = ggg(&obj, "A", veryVerbose);     // not enough buckets
        ASSERTV(&obj, failCode, -4 == failCode);
    }

    if (veryVerbose) puts("\nTesting generator on valid specs.");
    {
        if (veryVerbose) puts("\tTesting with Max Load Factor");

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
            const float MAX_LF  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            if (veryVeryVerbose) { T_ P(MAX_LF); }

            SizeType oldLen = 0;
            for (size_t ti = 0; ti != NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;
                const char *const EXP_S  = DATA[ti].d_results;

                const SizeType   LENGTH = static_cast<SizeType>(strlen(EXP_S));

                bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
                const TestValues  EXP(EXP_S, &tda);

                const SizeType curLen = static_cast<SizeType>(strlen(SPEC));

                if (curLen != oldLen || !oldLen) {
                    if (veryVeryVerbose) printf("\t\tof length " ZU ":\n",
                                                static_cast<size_t>(curLen));
                    ASSERTV(LINE, oldLen <= curLen);  // non-decreasing

                    // Let us check for a fail code reported from `ggg` for a
                    // SPEC string longer than can be supported without a
                    // rehash.

                    if (0 < oldLen) {
                        // First we compute a number of buckets to request that
                        // should be too few to support the required number of
                        // elements.

                        SizeType badLen = oldLen / 2;
                        if (0 < badLen) {
                            --badLen;
                        }

                        Obj mBadObj(HASHF,
                                    COMPAREF,
                                    static_cast<SizeType>(badLen),
                                    1.0f,
                                    objAlloc);
                        const int failCode = ggg(&mBadObj,
                                                 SPEC,
                                                 veryVeryVerbose);
                        ASSERTV(LINE, SPEC, LENGTH, badLen, failCode,
                                -4 == failCode);
                    }
                    oldLen = curLen;
                }

                const SizeType NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

                Obj mX(HASHF, COMPAREF, NUM_BUCKETS, MAX_LF, objAlloc);
                const Obj& X = gg(&mX, SPEC);   // original spec

                if (veryVerbose) {
                    printf("\t\tSpec = \"%s\"\n", SPEC);
                    T_ T_ T_ P(X);
                }

                ASSERTV(LINE, LENGTH == X.size());
                ASSERTV(LINE, MAX_LF, SPEC, EXP_S,
                    0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                        COMPAREF,
                                                        EXP,
                                                        LENGTH));
            }
        }
    }

    if (veryVerbose) puts("\nTesting generator on invalid specs.");
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

        size_t oldLen = 99;  // flag value longer than the longest `SPEC`
        for (int ti = 0; ti != NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const SizeType    LENGTH = static_cast<SizeType>(strlen(SPEC));

            Obj mX(HASHF, COMPAREF, LENGTH, 1.5f, objAlloc);

            if (LENGTH != oldLen) {
                if (veryVerbose) printf("\tof length " ZU ":\n",
                                        static_cast<size_t>(LENGTH));
                if (99 != oldLen) {  // i.e., not first pass
                    ASSERTV(LINE, oldLen,  LENGTH,
                                  oldLen < LENGTH);  // non-decreasing
                }
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}
//@bdetdsplit FOR 3 END

//@bdetdsplit FOR 4 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase4()
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
    // 1. Each accessor returns the value of the correct property of the
    //    object.
    //
    // 2. Each accessor method is declared `const`.
    //
    // 3. No accessor allocates any memory.
    //
    // 4. `elementListRoot` refers to the root of a list with exactly `size()`
    //    elements, and a null pointer value if `size() == 0`.
    //
    // 5. `bucketAtIndex` returns a valid bucket for all 0 <= index <
    //    `numBuckets`.
    //
    // 6. QoI: Assert precondition violations for `bucketAtIndex` when
    //    `size <= index` are detected in SAFE builds.
    //
    // 7. For any value of key, `bucketIndexForKey` returns a bucket number
    //    less than `numBuckets`.
    //
    // Plan:
    // 1. For each set of `SPEC` of different length:
    //
    //   1. Value construct the object with various configuration:
    //
    //     1. Use the `gg` function to populate the object based on the SPEC.
    //
    //     2. Verify the correct allocator is installed with the
    //        `allocator` method.
    //
    //     3. Verify the object all attributes are as expected.
    //
    //     4. Use `verifyListContents` to validate the list rooted at
    //        `elementListRoot`.
    //
    //     5. TBD: Use 'validateBucket to validate the buckets returned by
    //             `bucketAtIndex`.
    //
    //     6. Monitor the memory allocated from both the default and object
    //        allocators before and after calling the accessor; verify that
    //        there is no change in total memory allocation.  (C-3)
    //
    // 2. Verify that, in appropriate build modes, defensive checks are
    //    triggered for invalid attribute values, but not triggered for
    //    adjacent valid ones (using the `BSLS_ASSERTTEST_*` macros).  (C-6)
    //
    // Testing:
    //   allocator() const;
    //   comparator() const;
    //   hasher() const;
    //   size() const;
    //   numBuckets() const;
    //   maxLoadFactor() const;
    //   rehashThreshold() const;
    //   elementListRoot() const;
    //   bucketAtIndex(SizeType index) const;
    //   bucketIndexForKey(const KeyType& key) const;
    //   countElementsInBucket(SizeType index) const;
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    // If we use this predicate in more than one test case, add a trait to this
    // test driver that is has a partial specialization for `bsl::allocator<T>`
    // to return `true`, otherwise return `false`.
    const bool ALLOCATOR_IS_BSL_ALLOCATOR =
        bsl::is_same<typename bsl::allocator_traits<ALLOCATOR>::
                                   template rebind_traits<int>::allocator_type,
                     bsl::allocator<int> >::value;

    if (veryVerbose) {
        P(VALUE_TYPE_USES_ALLOCATOR);
        P(ALLOCATOR_IS_BSL_ALLOCATOR);
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (veryVerbose)
               puts("\nCreate objects with various allocator configurations.");
    {
        if (veryVerbose) puts("\tTesting with Max Load Factor");

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
            const float MAX_LF  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            if (veryVerbose) { T_ P(MAX_LF); }

            size_t oldLen = 0;
            for (int ti = 0; ti != NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;
                const char *const EXP_S  = DATA[ti].d_results;

                const SizeType   LENGTH = static_cast<SizeType>(strlen(EXP_S));

                bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
                const TestValues  EXP(EXP_S, &tda);

                const size_t curLen = strlen(SPEC);

                if (curLen != oldLen || !oldLen) {
                    if (veryVeryVerbose) printf("\tof length " ZU ":\n",
                                                curLen);
                    oldLen = curLen;
                }

                const SizeType NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

                const HASHER_VAR     hash = HASHF;
                const COMPARATOR_VAR comp = COMPAREF;
                setHasherState(bsls::Util::addressOf(hash), ti);
                setComparatorState(bsls::Util::addressOf(comp), ti);

                if (veryVerbose) { P_(LINE) P_(LENGTH) P(SPEC); }

                for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
                    const char CONFIG = *cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    // There is no easy way to create this guard for the
                    // specific test case of the stateless
                    // `bsltf::StdTestAllocator`, nor without second guessing
                    // the allocator to use based upon the `cfg` code.  By the
                    // time we return from `makeAllocator` the test allocator
                    // will already have been installed, with no easy way to
                    // restore at the end of the test case.

                    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(
                                                    'a' == CONFIG ? &sa : &da);

                    // --------------------------------------------------------

                    Obj       *objPtr;
                    ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                               CONFIG,
                                                               &fa,
                                                               &sa,
                                                               hash,
                                                               comp,
                                                               NUM_BUCKETS,
                                                               MAX_LF);

                    // Verify any attribute allocators are installed properly.

                    const bslma::TestAllocator  *oa = extractTestAllocator(
                                                                     expAlloc);
                    const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

                    // It is important that these allocators are found, or else
                    // the following tests will break severely, dereferencing
                    // null pointers.

                    BSLS_ASSERT_OPT(oa);
                    BSLS_ASSERT_OPT(noa);

                    const unsigned INTIIAL_BLOCKS = 0 == NUM_BUCKETS ? 0 : 1;

                    // Verify we are starting with a known, expected, number of
                    // allocations based on the number of requested buckets.
                    ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG, oa->numBlocksTotal(),
                            INTIIAL_BLOCKS == oa->numBlocksTotal());
                    ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG,
                            noa->numBlocksTotal(),
                            0 == noa->numBlocksTotal());

                    Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);

                    if (veryVerbose) {
                        printf("\t\tSpec = \"%s\"\n", SPEC);
                        T_ T_ T_ P(X);
                    }

                    ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

                    // --------------------------------------------------------

                    // Verify basic accessors

                    bslma::TestAllocatorMonitor oam(oa);

                    ASSERTV(LINE, SPEC, CONFIG,
                            areEqualComparators(comp, X.comparator()));
                    ASSERTV(LINE, SPEC, CONFIG,
                            areEqualHashers(hash, X.hasher()));

                    ASSERTV(LINE, SPEC, CONFIG, MAX_LF, X.maxLoadFactor(),
                            MAX_LF == X.maxLoadFactor());
                    ASSERTV(LINE, SPEC, CONFIG, LENGTH, X.size(),
                            LENGTH == X.size());

                    int VERIFY_ERROR_CODE = verifyListContents<KEY_CONFIG>(
                                                           X.elementListRoot(),
                                                           COMPAREF,
                                                           EXP,
                                                           LENGTH);
                    ASSERTV(VERIFY_ERROR_CODE, 0 == VERIFY_ERROR_CODE);

                    // Verify bucket accessors

                    const SizeType bucketCount = X.numBuckets();
                    ASSERTV(LINE, SPEC, CONFIG, NUM_BUCKETS, bucketCount,
                            NUM_BUCKETS <= bucketCount);

                    SizeType elementCount = 0;
                    for (SizeType iBkt = 0; iBkt != bucketCount; ++iBkt) {
                        const bslalg::HashTableBucket& bucket =
                                                         X.bucketAtIndex(iBkt);
                        bslalg::BidirectionalLink *bucketRoot = bucket.first();
                        SizeType bucketSize = X.countElementsInBucket(iBkt);
                        if (!bucketSize) {
                            ASSERTV(bucketSize, bucketRoot, 0 == bucketRoot);
                        }
                        else {
                            elementCount += bucketSize;
                            for (SizeType idx = 0; idx != bucketSize; ++idx) {
                                ASSERTV(idx, bucketRoot, 0 != bucketRoot);
                                SizeType index = X.bucketIndexForKey(
                                  ImpUtil::extractKey<KEY_CONFIG>(bucketRoot));
                                ASSERTV(iBkt, index, iBkt == index);
                                bucketRoot = bucketRoot->nextLink();
                            }
                        }
                    }
                    ASSERTV(X.size(), elementCount, X.size() == elementCount);

                    ASSERT(oam.isTotalSame());

                    // Ideally we would test this by inserting elements up to
                    // the threshold, and check that no rehash occurred, and
                    // then insert one more element and notice that the bucket
                    // count has changed.  This could get extremely expensive
                    // when the maxLoadFactor is high, or even infinite, and it
                    // messes up the existing memory-consumption calculations
                    // for little gain.  That part of the test will be deferred
                    // to test case XX for the `insert` operation, after
                    // `rehash` has already been tested.  To provide meaning,
                    // we compute the implications of adding `rehashThreshold`
                    // additional items to the current container, and
                    // demonstrate that adding one more should exceed the
                    // `maxLoadFactor`, unless it is infinite.

                    SizeType threshold = X.rehashThreshold();
                    double thresholdLoadFactor = static_cast<double>(threshold)
                                            / static_cast<double>(bucketCount);
                    ASSERTV(threshold, bucketCount,
                            thresholdLoadFactor,   X.maxLoadFactor(),
                            thresholdLoadFactor <= X.maxLoadFactor());

                    if (!threshold) {
                        // The HashTable is empty and no buckets have been
                        // allocated.

                        ASSERT(!X.size());
                        ASSERT(!X.elementListRoot());
                        ASSERTV(bucketCount, 1 == bucketCount);
                    }
                    else if (X.maxLoadFactor() <
                                      std::numeric_limits<float>::infinity()) {
                        ++threshold;
                        double exceedLoadFactor =
                                              static_cast<double>(threshold)
                                            / static_cast<double>(bucketCount);
                        ASSERTV(threshold, bucketCount,
                                X.maxLoadFactor(),  exceedLoadFactor,
                                X.maxLoadFactor() < exceedLoadFactor);
                    }
                    else {
                        ASSERTV(threshold,
                                threshold ==
                                         std::numeric_limits<SizeType>::max());
                    }

                    // --------------------------------------------------------

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify no allocation from the non-object allocator.

                    // In build configuration `a`, the `noa` allocator is the
                    // default allocator, otherwise `oa` is the default
                    // allocator.  If the allocator type is not
                    // `bsl::allocator` and the elements being inserted
                    // allocate memory, the element stored in the container
                    // will be created using the default allocator but there
                    // should be only one such allocation per element.
                    // Otherwise, there should be no residual evidence of using
                    // the `noa` allocator.

                    long long EXP_NUM_BLOCKS =  VALUE_TYPE_USES_ALLOCATOR  &&
                                               !ALLOCATOR_IS_BSL_ALLOCATOR &&
                                                'a' == CONFIG
                                             ? strlen(SPEC)
                                             : 0;

                    ASSERTV(LINE, CONFIG, EXP_NUM_BLOCKS,
                            noa->numBlocksTotal(),
                            EXP_NUM_BLOCKS == noa->numBlocksTotal());

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }
            }
        }
    }

    // Create some fresh allocators to use validating final corners of the
    // constructor behavior.  These are special case tests, and will not need
    // to loop many times, or test the contents of the container, so one set of
    // allocators will suffice to the end of the test case.

    bslma::TestAllocator na("negative testing", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific test case of
    // the stateless `bsltf::StdTestAllocator`, nor without second guessing the
    // allocator to use based upon the `cfg` code.  By the time we return from
    // `makeAllocator` the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&na);
    ALLOCATOR objAlloc = makeAllocator(&na);

    if (veryVerbose) puts("\nNegative Testing");
    {
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) puts("\t'bucketAtIndex'");
        {
            Obj mX(HASHF, COMPAREF, 1, 1.0f, objAlloc);  const Obj& X = mX;
            SizeType numBuckets = X.numBuckets();
            if (0 < numBuckets) { // always true, but needed for warnings
                ASSERT_SAFE_PASS(X.bucketAtIndex(numBuckets - 1));
                ASSERT_SAFE_FAIL(X.bucketAtIndex(numBuckets));

                ASSERT_SAFE_PASS(X.countElementsInBucket(numBuckets - 1));
                ASSERT_SAFE_FAIL(X.countElementsInBucket(numBuckets));
            }
        }
    }
}
//@bdetdsplit FOR 4 END

// Test case 5 is not needed, bslx streaming is not supported

//@bdetdsplit FOR 6 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase6()
{
    // ---------------------------------------------------------------------
    // EQUALITY OPERATORS
    //
    // Concerns:
    // 1. Two objects, `X` and `Y`, compare equal if and only if they contain
    //    the same values.
    //
    // 2. No non-salient attributes participate.  The non-salient attributes of
    //    a `HashTable` include the `allocator`, `loadFactor`, `maxLoadFactor`
    //    and `numBuckets`.
    //
    // 3. `true  == (X == X)` (i.e., identity)
    //
    // 4. `false == (X != X)` (i.e., identity)
    //
    // 5. `X == Y` if and only if `Y == X` (i.e., commutativity)
    //
    // 6. `X != Y` if and only if `Y != X` (i.e., commutativity)
    //
    // 7. `X != Y` if and only if `!(X == Y)`
    //
    // 8. Comparison is symmetric with respect to user-defined conversion
    //    (i.e., both comparison operators are free functions).
    //
    // 9. Non-modifiable objects can be compared (i.e., objects or references
    //    providing only non-modifiable access).
    //
    // 10. `operator==` is defined in terms of
    //    `operator==(KEY_CONFIG::ValueType)` instead of the supplied
    //    comparator function (which, along with the hasher, is still used to
    //    establish key-equivalent groups).
    //
    // 11. No memory allocation occurs as a result of comparison (e.g., the
    //    arguments are not passed by value).
    //
    // 12. The equality operator's signature and return type are standard.
    //
    // 13. The inequality operator's signature and return type are standard.
    //
    // 14. Two object with the same elements in different permutation compare
    //    equal to each other.
    //
    // Plan:
    // 1. Use the respective addresses of `operator==` and `operator!=` to
    //    initialize function pointers having the appropriate signatures and
    //    return types for the two homogeneous, free equality- comparison
    //    operators defined in this component.  (C-8..9, 12..13)
    //
    // 2. Create a `bslma::TestAllocator` object, and install it as the default
    //    allocator (note that a ubiquitous test allocator is already installed
    //    as the global allocator).
    //
    // 3. Using the table-driven technique, specify a set of distinct
    //    specifications for the `gg` function.
    //
    // 4. For each row `R1` in the table of P-3: (C-1..7)
    //
    //   1. Create a single object, using a comparator that can be disabled and
    //      a "scratch" allocator, and use it to verify the reflexive
    //      (anti-reflexive) property of equality (inequality) in the presence
    //      of aliasing.  (C-3..4)
    //
    //   2. For each row `R2` in the table of P-3: (C-1..2, 5..7)
    //
    //     1. Record, in `EXP`, whether or not distinct objects created from
    //        `R1` and `R2`, respectively, are expected to have the same value.
    //
    //     2. For each of two configurations, `a` and `b`: (C-1..2, 5..7)
    //
    //       1. Create three (object) allocators, `oax`, `oay` and `oaz`.
    //
    //       2. Create an object `X`, using `oax`, having the value `R1`.
    //
    //       3. Create an object `Y`, using `oax` in configuration `a` and
    //          `oay` in configuration `b`, having the value `R2`, and a
    //          `maxLoadFactor` of 10.
    //
    //       4. Create an object `Z`, using `oax` in configuration `a` and
    //          `oaz` in configuration `b`, having the value `R2`, and a
    //          `maxLoadFactor` of 0.01.
    //
    //       5. Verify the commutativity property and expected return value for
    //          both `==` and `!=`, while monitoring `oax`, `oay` and `oaz` to
    //          ensure that no object memory is ever allocated by either
    //          operator.  (C-1..2, 5..7)
    //
    //       6. Compare the ordering of elements in the lists accessed from
    //          `Y.elementListRoot()` and `Z.elementListRoot()`, which must
    //          always be permutations of each other, and set a test-wide flag
    //          to confirm that the lists have differing orders at least once.
    //          (C14)
    //
    // 5. Use the test allocator from P-2 to verify that no memory is ever
    //    allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const HashTable& lhs, const HashTable& rhs);
    //   bool operator!=(const HashTable& lhs, const HashTable& rhs);
    // ------------------------------------------------------------------------

    if (veryVerbose) puts("\nVerifying each operator signature");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = bslstl::operator==;
        operatorPtr operatorNe = bslstl::operator!=;

        // quash potential compiler warnings about unused variables

        ASSERT(operatorEq != operatorNe);
    }

    static const struct {
        int         d_line;   // source line number
        int         d_index;  // lexical order
        const char *d_spec;   // specification string, for input to `gg`
    } DATA[] = {
        //line idx  spec
        //---- ---  --------
        { L_,    0, "" },
        { L_,    1, "A" },
        { L_,    2, "AA" },
        { L_,    3, "AB" },
        { L_,    3, "BA" },
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
        { L_,   11, "ABFGKLPQUVZ" },
        { L_,   12, "AAABBCDEEFFFGGGHHIJKKLLLMMMNNOPQQRRRSSSTTUVWWXXXYYZ" },
        { L_,   12, "ABCDEFGHIJKLMNOPQRSTUVWXYZABEFGHKLMNQRSTWXYXRSLMGFA" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    // Create a variable to confirm that our tested data set has an expected
    // (and important) property.
    bool HAVE_TESTED_DISTINCT_PERMUTATIONS = false;

    // It would be interesting to make this variable static, so that different
    // test cases start off at different indexes, increasing the variety of the
    // test space.  Unfortunately, each instantiation of this test function
    // would obtain its own static variable, side-stepping the intent.  The
    // alternative would be to make this a global variable, which might then be
    // recycled for this purpose among multiple test cases.  Something to
    // consider for the future.

    size_t innerLoadFactorIndex = 0;

    if (veryVerbose) puts("\nCompare every test value with every test value");
    {
        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float       MAX_LF1  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1    = DATA[ti].d_line;
            const int         INDEX1   = DATA[ti].d_index;
            const char *const SPEC1    = DATA[ti].d_spec;

            const size_t      LENGTH1     = strlen(SPEC1);
            const size_t      NUM_BUCKETS = predictNumBuckets(LENGTH1,
                                                              MAX_LF1);

           if (veryVerbose) {
               T_ P_(LINE1) P_(MAX_LF1) P_(INDEX1) P_(LENGTH1) P(SPEC1);
           }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                ALLOCATOR objAlloc = makeAllocator(&scratch);

                Obj mX(HASHF, COMPAREF, NUM_BUCKETS, MAX_LF1, objAlloc);
                const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, MAX_LF1, SPEC1, X,   X == X);
                ASSERTV(LINE1, MAX_LF1, SPEC1, X, !(X != X));
            }

            for (int tj = 0; tj <= ti; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2      = strlen(SPEC2);
                const float       MAX_LF2  = DEFAULT_MAX_LOAD_FACTOR[
                        ++innerLoadFactorIndex % DEFAULT_MAX_LOAD_FACTOR_SIZE];

                const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                                   MAX_LF2);

                if (veryVerbose) {
                   T_ T_ P_(LINE2) P_(MAX_LF2) P_(INDEX2) P_(LENGTH2) P(SPEC2);
                }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines `Y`s allocator.

                    // Create two distinct test allocators, `oax` and `oay`.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects `X` and `Y` below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    ALLOCATOR allocX = makeAllocator(&xa);
                    ALLOCATOR allocY = makeAllocator(&ya);

                    Obj mX(HASHF, COMPAREF, NUM_BUCKETS,  MAX_LF1, allocX);
                    const Obj& X = gg(&mX, SPEC1);
                    Obj mY(HASHF, COMPAREF, NUM_BUCKETS2, MAX_LF2, allocY);
                    const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1, X.size(),
                            LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF1, X.maxLoadFactor(),
                            MAX_LF1 == X.maxLoadFactor());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2, Y.size(),
                            LENGTH2 == Y.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF2, Y.maxLoadFactor(),
                            MAX_LF2 == Y.maxLoadFactor());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaym.isTotalSame());

                    if (!HAVE_TESTED_DISTINCT_PERMUTATIONS && EXP) {
                        // Walk the lists of both `Y` and `X` to see if they
                        // they are distinct permutations.
                        Link *yCursor = Y.elementListRoot();
                        Link *xCursor = X.elementListRoot();

                        while (yCursor) {
                            ASSERT(xCursor); // lists should be the same length

                            if (!(ImpUtil::extractValue<KEY_CONFIG>(yCursor) ==
                                  ImpUtil::extractValue<KEY_CONFIG>(xCursor)))
                            {
                                HAVE_TESTED_DISTINCT_PERMUTATIONS = true;
                                break;
                            }
                            yCursor = yCursor->nextLink();
                            xCursor = xCursor->nextLink();
                        }
                    }
                }
            }
        }
        }
    }

    ASSERT(HAVE_TESTED_DISTINCT_PERMUTATIONS);
}
//@bdetdsplit FOR 6 END

//@bdetdsplit FOR 7 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase7()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR:
    //  One additional constraint to bear in mind, while testing the copy
    //  constructor, is that the copy does not give us any control over the
    //  `numBuckets` allocated for the new copy.  Given our primary manipulator
    //  is a special `insertElement` function that does not allow us to insert
    //  past the precomputed capacity of a HashTable, some test scenarios may
    //  produce copies that cannot be further modified by our chosen primary
    //  manipulator.  Therefor, we will test for a failed insertion in the case
    //  that the number of elements in the test data exactly matches the limit
    //  that can be allocated to the buckets without forcing a rehash, and in
    //  that case our mutate-event will be to clear the container instead.  We
    //  considered selectively avoiding this corner case with careful selection
    //  of test data, but realized that simply left a predictable, untested
    //  pattern in our test driver.  We will validate the proper behavior when
    //  an `insert` operation forces a rehash when testing the `insert` methods
    //  in a subsequent test case.
    //
    // Concern:
    // 1. The new object's value is the same as that of the original object
    //    (relying on the equality operator), the same `maxLoadFactor`, the
    //    same `hasher` and the same `comparator`.
    //
    // 2. All internal representations of a given value can be used to create a
    //    new object of equivalent value.
    //
    // 3. The value of the original object is left unaffected.
    //
    // 4. Subsequent changes in or destruction of the source object have no
    //    effect on the copy-constructed object.
    //
    // 5. Subsequent changes (`insert`s) on the created object have no
    //    effect on the original.
    //
    // 6. The object has its internal memory management system hooked up
    //    properly so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 7. The function is exception neutral w.r.t. memory allocation.
    //
    // 8. QoI The new object has a `loadFactor` <= its `maxLoadFactor`.
    //
    // Plan:
    // 1. Specify a set S of object values with substantial and varied
    //    differences, ordered by increasing length, to be used in the
    //    following tests.
    //
    // 2. For each value in S, initialize objects w and x, copy construct y
    //    from x and use `operator==` to verify that both x and y subsequently
    //    have the same value as w.  Let x go out of scope and again verify
    //    that w == y.  (C-1..4)
    //
    // 3. For each value in S initialize objects w and x, and copy construct y
    //    from x.  Change the state of y, by using the *primary* *manipulator*
    //    `insertElement` (or `removeAll` if `insertElement` returns a null
    //    pointer value).  Using the `operator!=` verify that y differs from x
    //    and w.  Then apply the same operation to `w` and verify that y still
    //    differs from x, and now has the same value as w. (C-5)
    //
    // 4. Perform tests performed as P-2:  (C-6)
    //   1. While passing a testAllocator as a parameter to the new object and
    //      ascertaining that the new object gets its memory from the provided
    //      testAllocator.
    //   2. Verify neither of global and default allocator is used to supply
    //      memory.  (C-6)
    //
    // 5. Perform tests as P-2 in the presence of exceptions during memory
    //    allocations using a `bslma::TestAllocator` and varying its
    //    *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   HashTable(const HashTable& original);
    //   HashTable(const HashTable& original, const A& allocator);
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::KeyType KEY;
    typedef typename KEY_CONFIG::ValueType VALUE;

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value
                         + bslma::UsesBslmaAllocator<VALUE>::value;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    if (veryVerbose) printf("\nTesting parameters: TYPE_ALLOC = %d\n",
                            TYPE_ALLOC);
    {
        static const char *SPECS[] = { "",
                                      "A",
                                     "AA",
                                     "AB",
                                     "BC",
                                    "AAA",
                                    "ABB",
                                    "CDE",
                                   "AAAA",
                                   "ABBA",
                                   "DEAB",
                                  "AAAAA",
                                  "AAABC",
                                  "ABBAC",
                                  "EABCD",
                                "ABCDEFG",
                               "HFGEDCBA",
                              "CFHEBIDGA",
                        "BENCKHGMALJDFOI",
                       "IDMLNEFHOPKGBCJA",
                      "OIQGDNPMLKBACHFEJ"
        };
        const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
            const float MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC        = SPECS[ti];

            const size_t      LENGTH      = strlen(SPEC);
            const size_t      NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            if (veryVeryVerbose) {
                T_ printf("\nFor an object of length " ZU ":\n", LENGTH);
                T_ P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

#if 0
            // It is not yet clear how to handle the "stateless" std-test
            // allocator in this one general function, as we now construct two
            // allocator objects with different test allocators, which violates
            // the contract that we install only the current default allocator
            // when creating a `stateless` object via the `make` call.
            //
            // There is no easy way to create this guard for the specific test
            // case of the stateless `bsltf::StdTestAllocator`, nor without
            // second guessing the allocator to use based upon the `cfg` code.
            // By the time we return from `makeAllocator` the test allocator
            // will already have been installed, with no easy way to restore at
            // the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
#endif

            HASHER     hash = HASHF;
            COMPARATOR comp = COMPAREF;
            setHasherState(bsls::Util::addressOf(hash), ti);
            setComparatorState(bsls::Util::addressOf(comp), ti);

            const ALLOCATOR dfltAlloc = makeAllocator(&da);
            const ALLOCATOR objAlloc  = makeAllocator(&oa);

            // Create control object w, with space for an extra element.
            Obj mW(hash, comp, NUM_BUCKETS + 1, MAX_LF, dfltAlloc);
            const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { fputs("\tControl Obj: ", stdout); P(W); }

            Obj mX(hash, comp, NUM_BUCKETS, MAX_LF, objAlloc);
            const Obj& X = gg(&mX, SPEC);

            // Sanity check only, previous test cases established this.
            ASSERTV(MAX_LF, SPEC, W, X,  W == X);

            if (veryVerbose) { fputs("\t\tDynamic Obj: ", stdout); P(X); }

            {   // Testing concern 1..4 and 8

                if (veryVeryVerbose) fputs("\t\t\tRegular Case :", stdout);

                Obj *pX = new Obj(HASHF,
                                  COMPAREF,
                                  NUM_BUCKETS,
                                  MAX_LF,
                                  objAlloc);
                gg(pX, SPEC);

                ASSERTV(MAX_LF, SPEC, *pX, X, *pX == X);
                ASSERTV(MAX_LF, SPEC, *pX, W, *pX == W);

                const Obj Y0(*pX);

                ASSERTV(MAX_LF, SPEC, *pX, Y0, *pX == Y0);
                ASSERTV(MAX_LF, SPEC,   X, Y0,   X == Y0);
                ASSERTV(MAX_LF, SPEC,   W, Y0,   W == Y0);
                ASSERTV(MAX_LF, SPEC,   W,  X,   W ==  X);

                const ALLOCATOR expectedAlloc =
                    bsl::allocator_traits<ALLOCATOR>::
                        select_on_container_copy_construction(pX->allocator());
                ASSERTV(MAX_LF, SPEC,
                        BSL_TF_EQ(Y0.allocator(), expectedAlloc));
                ASSERTV(MAX_LF, SPEC, pX->maxLoadFactor(), Y0.maxLoadFactor(),
                         pX->maxLoadFactor() == Y0.maxLoadFactor());
                ASSERTV(MAX_LF, SPEC, Y0.loadFactor(), Y0.maxLoadFactor(),
                        Y0.loadFactor() <= Y0.maxLoadFactor());
                delete pX;
                ASSERTV(MAX_LF, SPEC, W, Y0, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVeryVerbose) puts("\t\t\tInsert into created obj, "
                                          "without test allocator:");

                Obj Y1(W);

                if (veryVeryVerbose) {
                    fputs("\t\t\t\tBefore Insert: ", stdout); P(Y1);
                }

                Link *RESULT = insertElement(&Y1, VALUES['Z' - 'A']);
                if(0 == RESULT) {
                    Y1.removeAll();
                }

                if (veryVeryVerbose) {
                    fputs("\t\t\t\tAfter Insert : ", stdout); P(Y1);
                }

                if (RESULT || 0 != W.size()) {
                    const size_t EXPECTED_LENGTH = RESULT
                                                 ? LENGTH + 1
                                                 : 0;

                    ASSERTV(MAX_LF, SPEC, Y1.size(), EXPECTED_LENGTH,
                            Y1.size() == EXPECTED_LENGTH);
                    ASSERTV(MAX_LF, SPEC, W, Y1, W != Y1);
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }

                if (RESULT) {
                    RESULT = insertElement(&mW, VALUES['Z' - 'A']);
                    // Remember we reserved enough space at construction.
                    ASSERT(0 != RESULT);
                }
                else {
                    mW.removeAll();
                }

                ASSERTV(MAX_LF, SPEC, W, Y1, W == Y1);
                if (RESULT || 0 < X.size()) {
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose) puts("\t\t\tInsert into created obj, "
                                      "with test allocator:");

                bslma::TestAllocatorMonitor oam(&oa);

                Obj Y11(X, objAlloc);

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    //const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    //ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    //ASSERTV(SPEC, B, A, B + 1 + TYPE_ALLOCS ==  A);
                    ASSERTV(MAX_LF, SPEC, oam.isTotalUp());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseUp());
                }

                oam.reset(&oa);

                Y11.removeAll();

                if (veryVerbose) {
                    fputs("\t\t\t\tAfter Insert : ", stdout); P(Y11);
                }

                ASSERTV(SPEC, oam.isTotalSame());

                ASSERTV(SPEC, 0 == Y11.size());
                if (LENGTH != 0) {
                    ASSERTV(MAX_LF, SPEC, W != Y11 || !W.size());
                    ASSERTV(MAX_LF, SPEC, X != Y11);
                }
                ASSERTV(MAX_LF, SPEC, Y11.allocator() == X.allocator());
            }

            if (0 < X.size())
            {   // Exception checking.

                // There is nothing to test if `X` is empty, and several test
                // conditions would be complicated to allow for `W == X` in
                // this state.

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj Y2(X, objAlloc);
                    if (veryVerbose) {
                        puts("\t\t\tException Case  :");
                        fputs("\t\t\t\tObj : ", stdout); P(Y2);
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  != Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 ==  X);
                    ASSERTV(MAX_LF, SPEC, Y2.allocator() == X.allocator());

                    Link *RESULT = insertElement(&Y2, VALUES['Z' - 'A']);
                    if(0 == RESULT) {
                        Y2.removeAll();
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  == Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 !=  X);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
            }
        }
        }
    }
}
//@bdetdsplit FOR 7 END

// Test case 8 is TBD

//@bdetdsplit FOR 9 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase9()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free `swap` are implemented, we can
    //   exchange the values of any two objects that use the same allocator.
    //
    // Concerns:
    // 1. Both functions exchange the state of the (two) supplied objects,
    //    comprising their values, their functors, and their `maxLoadFactor`
    //    attribute.
    //
    // 2. Both functions have standard signatures and return types.
    //
    // 3. Using either function to swap an object with itself does not
    //    affect the value of the object (alias-safety).
    //
    // 4. If the two objects being swapped use the same allocator, neither
    //    function allocates memory from any allocator and the allocator
    //    address held by both objects is unchanged.
    //
    // 5. If the two objects being swapped uses different allocators and
    //    `AllocatorTraits::propagate_on_container_swap` is an alias to
    //    `false_type`, then both function may allocate memory and the
    //    allocator address held by both object is unchanged.
    //
    // 6. If the two objects being swapped uses different allocators and
    //    `AllocatorTraits::propagate_on_container_swap` is an alias to
    //    `true_type`, then no memory will be allocated and the allocators will
    //    also be swapped.
    //
    // 7. Both functions provides the strong exception guarantee w.t.r. to
    //    memory allocation .
    //
    // 8. The free `swap` function is discoverable through ADL (Argument
    //    Dependent Lookup).
    //
    // 9. `swap` does not invalidate any references or pointers to elements
    //    stored in either hash table, unless allocators are unequal and
    //    `AllocatorTraits::propagate_on_container_swap` is an alias to
    //    `false_type`.
    //
    // Plan:
    // 1. Use the addresses of the `swap` member and free functions defined
    //    in this component to initialize, respectively, member-function
    //    and free-function pointers having the appropriate signatures and
    //    return types.  (C-2)
    //
    // 2. Create a `bslma::TestAllocator` object, and install it as the
    //    default allocator (note that a ubiquitous test allocator is
    //    already installed as the global allocator).
    //
    // 3. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values (one per row) in
    //      terms of their individual attributes, including (a) first, the
    //      default value, (b) boundary values corresponding to every range
    //      of values that each individual attribute can independently
    //      attain, and (c) values that should require allocation from each
    //      individual attribute that can independently allocate memory.
    //
    //   2. Additionally, provide a (tri-valued) column, `MEM`, indicating
    //      the expectation of memory allocation for all typical
    //      implementations of individual attribute types: ('Y') "Yes",
    //      ('N') "No", or ('?') "implementation-dependent".
    //
    // 4. For each row `R1` in the table of P-3:  (C-1, 3..7)
    //
    //   1. Create a `bslma::TestAllocator` object, `oa`.
    //
    //   2. Use the value constructor and `oa` to create a modifiable
    //      `Obj`, `mW`, having the value described by `R1`; also use the
    //      copy constructor and a "scratch" allocator to create a `const`
    //      `Obj` `XX` from `mW`.
    //
    //   3. Use the member and free `swap` functions to swap the value of
    //      `mW` with itself; verify, after each swap, that:  (C-3..4)
    //
    //     1. The value is unchanged.  (C-3)
    //
    //     2. The allocator address held by the object is unchanged.  (C-4)
    //
    //     3. There was no additional object memory allocation.  (C-4)
    //
    //   4. For each row `R2` in the table of P-3:  (C-1, 4)
    //
    //     1. Use the copy constructor and `oa` to create a modifiable
    //        `Obj`, `mX`, from `XX` (P-4.2).
    //
    //     2. Use the value constructor and `oa` to create a modifiable
    //        `Obj`, `mY`, and having the value described by `R2`; also use
    //        the copy constructor to create, using a "scratch" allocator,
    //        a `const` `Obj`, `YY`, from `Y`.
    //
    //     3. Use, in turn, the member and free `swap` functions to swap
    //        the values of `mX` and `mY`; verify, after each swap, that:
    //        (C-1..2)
    //
    //       1. The values have been exchanged.  (C-1)
    //
    //       2. The common object allocator address held by `mX` and `mY`
    //          is unchanged in both objects.  (C-4)
    //
    //       3. There was no additional object memory allocation.  (C-4)
    //
    //     5. Use the value constructor and `oaz` to a create a modifiable
    //        `Obj` `mZ`, having the value described by `R2`; also use the copy
    //        constructor to create, using a "scratch" allocator, a const
    //        `Obj`, `ZZ`, from `Z`.
    //
    //     6. Use the member and free `swap` functions to swap the values of
    //        `mX` and `mZ` respectively (when
    //        AllocatorTraits::propagate_on_container_swap is an alias to
    //        false_type) under the presence of exception; verify, after each
    //        swap, that:  (C-1, 5, 7)
    //
    //       1. If exception occurred during the swap, both values are
    //          unchanged.  (C-7)
    //
    //       2. If no exception occurred, the values have been exchanged.
    //          (C-1)
    //
    //       3. The common object allocator address held by `mX` and `mZ` is
    //          unchanged in both objects.  (C-5)
    //
    //       4. Temporary memory were allocated from `oa` if `mZ` is not empty,
    //          and temporary memory were allocated from `oaz` if `mX` is not
    //          empty.  (C-5)
    //
    //     7. Create a new object allocator, `oap`.
    //
    //     8. Use the value constructor and `oap` to create a modifiable `Obj`
    //        `mP`, having the value described by `R2`; also use the copy
    //        constructor to create, using a "scratch" allocator, a const
    //        `Obj`, `PP`, from 'P.
    //
    //     9. Manually change `AllocatorTraits::propagate_on_container_swap` to
    //        be an alias to `true_type` (Instead of this manual step, use an
    //        allocator that enables propagate_on_container_swap when
    //        AllocatorTraits supports it) and use the member and free `swap`
    //        functions to swap the values `mX` and `mZ` respectively; verify,
    //        after each swap, that: (C-1, 6)
    //
    //       1. The values have been exchanged.  (C-1)
    //
    //       2. The allocators addresses have been exchanged.  (C-6)
    //
    //       3. There was no additional object memory allocation.  (C-6)
    //
    // 5. Verify that the free `swap` function is discoverable through ADL:
    //    (C-8)
    //
    //   1. Create a set of attribute values, `A`, distinct from the values
    //      corresponding to the default-constructed object, choosing
    //      values that allocate memory if possible.
    //
    //   2. Create a `bslma::TestAllocator` object, `oa`.
    //
    //   3. Use the default constructor and `oa` to create a modifiable
    //      `Obj` `mX` (having default attribute values); also use the copy
    //      constructor and a "scratch" allocator to create a `const` `Obj`
    //      `XX` from `mX`.
    //
    //   4. Use the value constructor and `oa` to create a modifiable `Obj`
    //      `mY` having the value described by the `Ai` attributes; also
    //      use the copy constructor and a "scratch" allocator to create a
    //      `const` `Obj` `YY` from `mY`.
    //
    //   5. Use the `invokeAdlSwap` helper function template to swap the
    //      values of `mX` and `mY`, using the free `swap` function defined
    //      in this component, then verify that:  (C-8)
    //
    //     1. The values have been exchanged.  (C-1)
    //
    //     2. There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(HashTable& other);
    //   void swap(HashTable<K, V, C, A>& a, HashTable<K, V, C, A>& b);
    // ------------------------------------------------------------------------

    if (veryVerbose) puts("\nVerify the signature of each function");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bslstl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const float       MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        const size_t      LENGTH1      = strlen(SPEC1);
        const size_t      NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = makeAllocator(&scratch);
        const ALLOCATOR objAlloc     = makeAllocator(&oa);

        Obj mW(HASHF, COMPAREF, NUM_BUCKETS1, MAX_LF1, objAlloc);
        const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, scratchAlloc);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // member `swap`
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, objAlloc == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function `swap`
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, objAlloc == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // There is a symmetry to testing, so test only the "lower triangle" of
        // indices.  Note that we want to support the limiting case where the
        // indices match.
        for (int lfj = 0; lfj <= lfi; ++lfj) {
        for (int  tj = 0;  tj <=  ti; ++ tj) {
            const float       MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            const size_t      LENGTH2      = strlen(SPEC2);
            const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                               MAX_LF2);

            Obj mX(XX, objAlloc);  const Obj& X = mX;

            Obj mY(HASHF, COMPAREF, NUM_BUCKETS2, MAX_LF2, objAlloc);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, scratchAlloc);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // free function `swap` is always well-defined
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // Member `swap` may not be defined if allocators are not the same,
            // although guaranteed in this case.
            {
                ASSERT(X.allocator() == Y.allocator()); // sanity check

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // We may consider restoring a test of unequal allocators if the
            // allocator propagate_on_container_swap trait is true, but
            // `bsl::allocator_traits` does not properly detect this trait yet.
        }
        }
    }
    }

    if (veryVerbose) puts("\nVerify ADL finds the free `swap` function");
    {
        // `A` values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = makeAllocator(&scratch);
        const ALLOCATOR objAlloc     = makeAllocator(&oa);

        Obj mX(HASHF, COMPAREF, 0, 1.0f, objAlloc);  const Obj& X = mX;
        const Obj XX(X, scratchAlloc);

        Obj mY(HASHF, COMPAREF, 40, 0.1f, objAlloc);
        const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, scratchAlloc);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        // We know that the types of `mX` and `mY` do not overload the unary
        // address-of `operator&`.

        bslalg::SwapUtil::swap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERTV(YY, X, YY.maxLoadFactor() == X.maxLoadFactor());
        ASSERTV(XX, Y, XX.maxLoadFactor() == Y.maxLoadFactor());
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }

    if (veryVerbose) puts("Check our implementation of undefined behavior");
    {
        // Ideally we would negatively test for an ASSERT in SAFE mode, and
        // test the strongly exception-safe `swap` only in other build modes.
        // We will test only one `swap` of two large containers of different
        // sizes, and having different load factors, as well as different
        // allocators to represent a fair test of our expected implementation
        // of the undefined behavior.

        const float       MAX_LF1 = 0.125f;
        const float       MAX_LF2 = 2.5f;

        const char *const SPEC1   = DATA[NUM_DATA-1].d_spec;
        const char *const SPEC2   = DATA[NUM_DATA-2].d_spec;

        const size_t      LENGTH1      = strlen(SPEC1);
        const size_t      NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

        const size_t      LENGTH2      = strlen(SPEC2);
        const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2, MAX_LF2);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        bslma::TestAllocator oax("x_object", veryVeryVeryVerbose);
        bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = makeAllocator(&scratch);
        const ALLOCATOR oaxAlloc     = makeAllocator(&oax);
        const ALLOCATOR oazAlloc     = makeAllocator(&oaz);

        Obj mX(HASHF, COMPAREF, NUM_BUCKETS1, MAX_LF1, oaxAlloc);
        const Obj& X = gg(&mX, SPEC1);
        const Obj XX(X, scratchAlloc);

        Obj mZ(HASHF, COMPAREF, NUM_BUCKETS2, MAX_LF2, oazAlloc);
        const Obj& Z = gg(&mZ, SPEC2);
        const Obj ZZ(Z, scratchAlloc);

        // Sanity check

        ASSERTV(XX, X, XX == X);
        ASSERTV(ZZ, Z, ZZ == Z);
        ASSERTV( X, Z,  X != Z);

        // free function `swap`
        {
            bslma::TestAllocatorMonitor oaxm(&oax);
            bslma::TestAllocatorMonitor oazm(&oaz);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oax) {
                bslma::ExceptionGuard<Obj> guardX(&X, L_, scratchAlloc);
                bslma::ExceptionGuard<Obj> guardZ(&Z, L_, scratchAlloc);

                swap(mX, mZ);

                guardX.release();
                guardZ.release();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(XX, Z, XX == Z);
            ASSERTV(ZZ, X, ZZ == X);
            ASSERTV(oaxAlloc == X.allocator());
            ASSERTV(oazAlloc == Z.allocator());

            // Concerns about allocated memory?
        }

        if (veryVerbose) puts("\nNegative Testing");
        {
            bsls::AssertTestHandlerGuard hG;

            // member `swap`
            ASSERT_PASS(mX.swap(mX));
            ASSERT_FAIL(mZ.swap(mX));
            ASSERT_FAIL(mX.swap(mZ));
            ASSERT_PASS(mZ.swap(mZ));
        }

    }
}
//@bdetdsplit FOR 9 END

//@bdetdsplit FOR 10 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase10()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    // 1. The assignment operator can change the value of any modifiable target
    //    object to that of any source object.
    //
    // 2. If allocator propagation is not enabled for copy assignment, the
    //    allocator address held by the target object is unchanged.
    //
    // 3. If allocator propagation is not enabled for copy assignment, any
    //    memory allocation is from the target object's allocator.
    //
    // 4. The signature and return type are standard.
    //
    // 5. The reference returned is to the target object (i.e., `*this`).
    //
    // 6. The value of the source object is not modified.
    //
    // 7. The allocator address held by the source object is unchanged.
    //
    // 8. QoI: Assigning a source object having the default-constructed value
    //    allocates no memory.
    //
    // 9. Any memory allocation is exception neutral.
    //
    // 10. Assigning an object to itself behaves as expected (alias-safety).
    //
    // 11. Every object releases any allocated memory at destruction.
    //
    // 12. If allocator propagation is enabled for copy assignment,
    //    any memory allocation is from the source object's
    //    allocator.
    //
    // 13. If allocator propagation is enabled for copy assignment, the
    //    allocator address held by the target object is changed to that of the
    //    source.
    //
    // 14. If allocator propagation is enabled for copy assignment, any memory
    //    allocation is from the original target allocator will be released
    //    after copy assignment.
    //
    // Plan:
    // 1. Use the address of `operator=` to initialize a member-function
    //    pointer having the appropriate signature and return type for the
    //    copy-assignment operator defined in this component.  (C-4)
    //
    // 2. Create a `bslma::TestAllocator` object, and install it as the default
    //    allocator (note that a ubiquitous test allocator is already installed
    //    as the global allocator).
    //
    // 3. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values.
    //
    // 4. For each row `R1` (representing a distinct object value, `V`) in the
    //    table described in P-3: (C-1..2, 5..8, 11)
    //
    //   1. Use the value constructor and a "scratch" allocator to create two
    //      `const` `Obj`, `Z` and `ZZ`, each having the value `V`.
    //
    //   2. Execute an inner loop that iterates over each row `R2`
    //      (representing a distinct object value, `W`) in the table described
    //      in P-3:
    //
    //   3. For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //
    //     1. Create a `bslma::TestAllocator` object, `oa`.
    //
    //     2. Use the value constructor and `oa` to create a modifiable `Obj`,
    //        `mX`, having the value `W`.
    //
    //     3. Assign `mX` from `Z` in the presence of injected exceptions
    //        (using the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros).
    //
    //     4. Verify that the address of the return value is the same as that
    //        of `mX`.  (C-5)
    //
    //     5. Use the equality-comparison operator to verify that: (C-1, 6)
    //
    //       1. The target object, `mX`, now has the same value as that of `Z`.
    //          (C-1)
    //
    //       2. `Z` still has the same value as that of `ZZ`.  (C-6)
    //
    //     6. Use the `allocator` accessor of both `mX` and `Z` to verify that
    //        the respective allocator addresses held by the target and source
    //        objects are unchanged.  (C-2, 7)
    //
    //     7. Use the appropriate test allocators to verify that: (C-8, 11)
    //
    //       1. For an object that (a) is initialized with a value that did NOT
    //          require memory allocation, and (b) is then assigned a value
    //          that DID require memory allocation, the target object DOES
    //          allocate memory from its object allocator only (irrespective of
    //          the specific number of allocations or the total amount of
    //          memory allocated); also cross check with what is expected for
    //          `mX` and `Z`.
    //
    //       2. An object that is assigned a value that did NOT require memory
    //          allocation, does NOT allocate memory from its object allocator;
    //          also cross check with what is expected for `Z`.
    //
    //       3. No additional memory is allocated by the source object.  (C-8)
    //
    //       4. All object memory is released when the object is destroyed.
    //          (C-11)
    //
    // 5. Repeat steps similar to those described in P-4 except that, this
    //    time, there is no inner loop (as in P-4.2); instead, the source
    //    object, `Z`, is a reference to the target object, `mX`, and both `mX`
    //    and `ZZ` are initialized to have the value `V`.  For each row
    //    (representing a distinct object value, `V`) in the table described in
    //    P-3: (C-9)
    //
    //   1. Create a `bslma::TestAllocator` object, `oa`.
    //
    //   2. Use the value constructor and `oa` to create a modifiable `Obj`
    //      `mX`; also use the value constructor and a distinct "scratch"
    //      allocator to create a `const` `Obj` `ZZ`.
    //
    //   3. Let `Z` be a reference providing only `const` access to `mX`.
    //
    //   4. Assign `mX` from `Z` in the presence of injected exceptions (using
    //      the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros).  (C-9)
    //
    //   5. Verify that the address of the return value is the same as that of
    //      `mX`.
    //
    //   6. Use the equality-comparison operator to verify that the target
    //      object, `mX`, still has the same value as that of `ZZ`.
    //
    //   7. Use the `allocator` accessor of `mX` to verify that it is still the
    //      object allocator.
    //
    //   8. Use the appropriate test allocators to verify that:
    //
    //     1. Any memory that is allocated is from the object allocator.
    //
    //     2. No additional (e.g., temporary) object memory is allocated when
    //        assigning an object value that did NOT initially require
    //        allocated memory.
    //
    //     3. All object memory is released when the object is destroyed.
    //
    // 6. Use the test allocator from P-2 to verify that no memory is ever
    //    allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   HashTable& operator=(const HashTable& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    // Repeatedly running the strong exception safety test can be expensive, so
    // we toggle a boolean variable to alternate tests in the cycle.  This
    // retains a fairly exhaustive coverage, while significantly improving test
    // timings.

    bool testForStrongExceptionSafety = false;

    if (veryVerbose) puts("\nCompare each pair of similar and different values"
                          " (u, ua, v, va) in S X A X S X A"
                          " without perturbation");
    {
        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;

            const size_t      LENGTH1 = strlen(SPEC1);
            const size_t NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const ALLOCATOR scratchAlloc = makeAllocator(&scratch);

            Obj mZ(HASHF, COMPAREF, NUM_BUCKETS1, MAX_LF1, scratchAlloc);
            const Obj& Z  = gg(&mZ,  SPEC1);
            const Obj ZZ(Z, scratchAlloc);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // should perform a self-assignment test here

            // Create second object
            for (int lfj = 0; lfj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfj) {
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const float MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2 = strlen(SPEC2);
                const size_t NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                              MAX_LF2);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                const ALLOCATOR objAlloc  = makeAllocator(&oa);

                {
                    Obj mX(HASHF, COMPAREF, NUM_BUCKETS2, MAX_LF2, objAlloc);
                    const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    testForStrongExceptionSafety =
                                                 !testForStrongExceptionSafety;
                    if (testForStrongExceptionSafety) {
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                            Obj *mR = &(mX = Z);
                            ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                            ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                            ASSERTV(LINE1, LINE2,
                                    Z.maxLoadFactor(),   X.maxLoadFactor(),
                                    Z.maxLoadFactor() == X.maxLoadFactor());

                        // If we can compare the two functor objects, we should
                        // do so, to verify stateful allocators are copied too.
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                    else {
                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                        ASSERTV(LINE1, LINE2,
                                Z.maxLoadFactor(),   X.maxLoadFactor(),
                                Z.maxLoadFactor() == X.maxLoadFactor());
                    }

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    // Should test that allocator propagates if, and only if,
                    // the allocator has the propagate_on_container_copy_assign
                    // trait.  Currently we assume (and test) that allocators
                    // never propagate, as our implementation of allocator
                    // traits for C++03 containers does not deduce or support
                    // any of the propagation traits.

                    ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                    ASSERTV(LINE1, LINE2, scratchAlloc == Z.allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }
            }
        }
        }
    }
}
//@bdetdsplit FOR 10 END

// Test case 11 is TBD: Move assignment

// Test case 12 (bslx streaming) is not supported

//@bdetdsplit FOR 13 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase13()
{
    // ------------------------------------------------------------------------
    // `rehashForNumBuckets`
    //
    // Concerns:
    // 1. `rehashForNumBuckets` allocates at least the specified number of
    //    buckets.
    //
    // 2. `rehashForNumBuckets` does not affect the value of the object.
    //
    // 3. `rehashForNumBuckets` does not affect the order of the inserted
    //    elements with the same value.
    //
    // 4. `rehashForNumBuckets` is a no-op if the requested number of buckets
    //    is less than the current `numBuckets` in the object.
    //
    // 5. Any memory allocation is from the object allocator.
    //
    // 6. The only memory allocation is a single allocation for the new bucket
    //    array; no new nodes are allocated, and the old array is reclaimed by
    //    the object allocator.
    //
    // 7. `rehashForNumBuckets` provides the strong exception guarantee if the
    //    hasher does not throw.
    //
    // 8. `rehashForNumBuckets` will reset the object to an empty container,
    //    without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    // 1. TBD
    //
    // Testing:
    //   rehashForNumBuckets(SizeType newNumBuckets);
    // ------------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const size_t REHASH_SIZE[] = {
           0,
           1,
           2,
           3,
           5,
           8,
          12,
          13,
        2099
    };
    const int NUM_REHASH_SIZE = sizeof REHASH_SIZE / sizeof *REHASH_SIZE;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        if (veryVerbose) printf("Testing rehashForNumBuckets on spec: %s,"
                                " index: %i\n", SPEC,  ti);

        for (int tj = 0; tj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++tj) {
            const float  MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[tj];
            const size_t NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = makeAllocator(&scratch);

            Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_REHASH_SIZE; ++tk) {
                bslma::TestAllocator  oa("object",  veryVeryVeryVerbose);
                ALLOCATOR objAlloc = makeAllocator(&oa);

                Obj mX(Z, objAlloc); const Obj& X = mX;

                if (veryVerbose) { T_ P_(LINE) P_(Z) P(X) }

                const size_t OLD_NUM_BUCKETS = X.numBuckets();
                const size_t NEW_NUM_BUCKETS = std::max(
                                      REHASH_SIZE[tk],
                                      predictNumBuckets(X.size() + 1, MAX_LF));

                const size_t EXP_NUM_BUCKETS =
                      bslstl::HashTable_ImpDetails::nextPrime(NEW_NUM_BUCKETS);

                bslma::TestAllocatorMonitor oam(&oa);

                mX.rehashForNumBuckets(REHASH_SIZE[tk]);

                ASSERTV(LINE, tk, MAX_LF, X == Z);
                ASSERTV(LINE, tk, MAX_LF, REHASH_SIZE[tk], X.numBuckets(),
                        REHASH_SIZE[tk] <= X.numBuckets());

                // If request is smaller than current number of buckets, there
                // should be no effect, no change in buckets, and no chance to
                // allocate memory.
                if (REHASH_SIZE[tk] <= OLD_NUM_BUCKETS) {
                    ASSERTV(LINE, tk, MAX_LF,
                            OLD_NUM_BUCKETS == X.numBuckets());
                    ASSERTV(LINE, tk, MAX_LF, oam.isTotalSame());
                    ASSERTV(LINE, tk, MAX_LF, oam.isInUseSame());
                }
                // Otherwise, we have computed the new expected number of
                // buckets, and there should be exactly one allocation for the
                // new bucket array.
                else {
                    ASSERTV(LINE, tk, MAX_LF,
                            EXP_NUM_BUCKETS,   X.numBuckets(),
                            EXP_NUM_BUCKETS == X.numBuckets());

                    ASSERTV(LINE, tk, oam.numBlocksTotalChange(),
                            1 == oam.numBlocksTotalChange());

                    // We release the old array allocation to make use of the
                    // new, so there should be no change in the number of
                    // allocated blocks in use.  However, in the case of the
                    // first allocation on an empty container, we will actually
                    // be creating the first array block.
                    if (0 < LENGTH ) {
                        ASSERTV(LINE, tk, oam.isInUseSame());
                    }
                    else {
                        ASSERTV(LINE, tk, oam.numBlocksInUseChange(),
                                1 == oam.numBlocksInUseChange());
                    }
                }
            }
        }

#ifdef BDE_BUILD_TARGET_EXC
        // The following set of tests are expected, at least in some test
        // configurations, to fail by throwing exceptions.
        if (veryVeryVerbose) printf("Testing behavior at extremities.\n");
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = makeAllocator(&scratch);

            Obj mX(HASHF, COMPAREF, 1, 1.0f, scratchAlloc);
            try {
                mX.rehashForNumBuckets(std::numeric_limits<size_t>::max());
                ASSERT("rehash(max size_t) should throw a `logic_error`" == 0);
            }
            // This is the expected code path
            catch (const std::logic_error &) {
            }
            catch (...) {
                ASSERT("rehash(max size_t) threw the wrong exception type"
                                                                         == 0);
            }
        }
#if 0
       {
           Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, 1e30f, scratchAlloc);
           const Obj& Z = gg(&mZ,  SPEC);
       }
       {
           Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, 1e-6f, scratchAlloc);
           const Obj& Z = gg(&mZ,  SPEC);
       }
       {
           Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, 1.0f, s cratchAlloc);
           const Obj& Z = gg(&mZ,  SPEC);
       }
       {
           Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, 1.0f, scratchAlloc);
           const Obj& Z = gg(&mZ,  SPEC);
       }
#endif  // if 0
#endif  // BDE_BUILD_TARGET_EXC
    }
}
//@bdetdsplit FOR 13 END

//@bdetdsplit FOR 14 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase14()
{
    // ------------------------------------------------------------------------
    // `reserveForNumElements`
    //
    // Concerns:
    // 1. `reserveForNumElements` allocates sufficient buckets so that, after
    //    the rehash, `numBuckets() / maxLoadFactor()` >= the specified number
    //    of elements.
    //
    // 2. `reserveForNumElements` does not affect the value of the object.
    //
    // 3. `reserveForNumElements` does not affect the order of the inserted
    //    elements with the same value.
    //
    // 4. `reserveForNumElements` is a no-op if the requested number of
    //    elements can already be accommodated without exceeding the
    //    `maxLoadFactor` of the object.
    //
    // 5. Any memory allocation is from the object allocator.
    //
    // 6. The only memory allocation is a single allocation for the new bucket
    //    array; no new nodes are allocated, and the old array is reclaimed by
    //    the object allocator.
    //
    // 7. `reserveForNumElements` provides the strong exception guarantee if
    //    the hasher does not throw.
    //
    // 8. `reserveForNumElements` will reset the object to an empty container,
    //    without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    // 1. TBD
    //
    // Testing:
    //   reserveForNumElements(SizeType numElements);
    // ------------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const size_t RESERVE_SIZE[] = {
           0,
           1,
           2,
           3,
           5,
           8,
          12,
          13,
        2099
    };
    const int NUM_RESERVE_SIZE = sizeof RESERVE_SIZE / sizeof *RESERVE_SIZE;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        if (veryVerbose) printf("Testing reserveForNumElements on spec: %s,"
                                " index: %i\n", SPEC,  ti);

        for (int tj = 0; tj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++tj) {
            const float  MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[tj];
            const size_t NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            if (veryVeryVerbose) printf("\twith max load factor of: %f\n",
                                        MAX_LF);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = makeAllocator(&scratch);

            Obj mZ(HASHF, COMPAREF, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_RESERVE_SIZE; ++tk) {
                bslma::TestAllocator reserve("reserveAlloc",
                                             veryVeryVeryVerbose);
                bslma::TestAllocator noReserve("noReserveAlloc",
                                               veryVeryVeryVerbose);

                ALLOCATOR reserveAlloc = makeAllocator(
                                                                     &reserve);
                ALLOCATOR noReserveAlloc = makeAllocator(
                                                                   &noReserve);

                Obj mResX(Z, reserveAlloc); const Obj& resX = mResX;
                Obj mNoResX(Z, noReserveAlloc); const Obj& noResX = mNoResX;

                if (veryVerbose) { T_ P_(LINE) P_(Z) P_(resX) P(noResX) }

                const size_t ADDITIONAL_ELEMENTS = RESERVE_SIZE[tk];
                const size_t INITIAL_SIZE = resX.size();
                const size_t RESERVE_ELEMENTS = ADDITIONAL_ELEMENTS +
                                                                  INITIAL_SIZE;

                if (veryVeryVerbose) printf(
                        "\t\treserve for " ZU " elements\n", RESERVE_ELEMENTS);

                mResX.reserveForNumElements(RESERVE_ELEMENTS);

                const bsls::Types::Int64 RESERVED_MEM =
                                                       reserve.numBytesTotal();

                mResX.reserveForNumElements(RESERVE_ELEMENTS);

                // Reserve doesn't allocate memory when some is already
                // available from previous reserve
                ASSERTV(LINE, tk, RESERVE_ELEMENTS, reserve.numBytesTotal() ==
                                                                 RESERVED_MEM);

                for (size_t tl = 0; tl < ADDITIONAL_ELEMENTS; ++tl) {
                    mResX.insert( VALUES[tl % 10] );
                    mNoResX.insert( VALUES[tl % 10] );
                }

                // Reserving X elements allocates same or less amount of memory
                // as inserting `X` elements one at a time
                ASSERTV(LINE, tk, RESERVE_ELEMENTS, reserve.numBytesTotal() <=
                                                    noReserve.numBytesTotal());
            }
        }
    }
}
//@bdetdsplit FOR 14 END

//@bdetdsplit FOR 15 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase15()
{
    // ------------------------------------------------------------------------
    // `insert` METHODS
    //
    // Concerns:
    // 1. ...
    //
    //
    // Plan:
    // 1. For each value of increasing length, `L`:
    //
    //
    // Testing:
    //   insert(const SOURCE_TYPE& obj);
    //   insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
    // ------------------------------------------------------------------------

    if (veryVerbose) puts("\nTesting is not yet implemented");

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (veryVerbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    const TestValues VALUES;  // contains 52 distinct increasing values

    // Probably want to pick these up as values from some injected policy, so
    // that we can test with stateful variants
    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    const size_t MAX_LENGTH = 9;

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            {
            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting bootstrap constructor");

            Obj *objPtr;

            //const size_t NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            ALLOCATOR expAllocator = ObjMaker::makeObject( &objPtr
                                                         , CONFIG
                                                         , &fa
                                                         , &sa
                                                         , HASHF
                                                         , COMPAREF
                                                         , 0
                                                         , MAX_LF);
            bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                       &fa);
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAllocator == X.allocator());

            bslma::TestAllocator  *oa = extractTestAllocator(expAllocator);
            bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null pointer.
            BSLS_ASSERT_OPT(oa);
            BSLS_ASSERT_OPT(noa);

            // QoI: Verify no allocation from the object/non-object allocators
            // if no buckets are requested (as per the default constructor).
            if (0 == LENGTH) {
                ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                        0 ==  oa->numBlocksTotal());
            }
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Record blocks used by the initial bucket array
            const bsls::Types::Int64 INITIAL_OA_BLOCKS  = oa->numBlocksTotal();

            // Verify attributes of an empty container.

            // Note that not all of these attributes are salient to value.
            // None of these accessors are deemed tested until their own test
            // case, but many witnesses give us some confidence in the state.
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                    INITIAL_OA_BLOCKS ==  oa->numBlocksTotal());
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting `insert(element)`");
            if (0 < LENGTH) {
                if (veryVerbose) printf("\t\tOn an object of initial length "
                                        ZU ".\n", LENGTH);

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa->numBlocksTotal(),   oa->numBlocksInUse(),
                        oa->numBlocksTotal() == oa->numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    fputs("\t\t\tBEFORE: ", stdout); P(X);
                }

                bslma::TestAllocator scratch1("scratch1", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch1);

                    // This will fail on the initial insert as we must also
                    // create the bucket array, so there is an extra pass.  Not
                    // sure why that means the block counts get out of sync
                    // though, is this catching a real bug?
#ifdef u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif  // u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This does
                    // not allow for rehashes as the container grows.
#ifdef u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS
                    if (VALUE_TYPE_USES_ALLOCATOR  ||
                                                expectPoolToAllocate(LENGTH)) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }
#endif  // u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS

                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    //
                    // !!! BROKEN TEST CONDITION !!!
                    //
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

#ifdef u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif  // u_HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS

                    ASSERTV(CONFIG, LENGTH - 1,
                            BSL_TF_EQ(
                                    KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                    ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(CONFIG, LENGTH - 1,
                            BSL_TF_EQ(
                                   VALUES[LENGTH - 1],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPAREF,
                                                           VALUES,
                                                           LENGTH));
                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0;j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    SizeType i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting `removeAll`");
            {
                const bsls::Types::Int64 BB = oa->numBlocksTotal();

                mX.removeAll();

                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa->numBlocksTotal();

                ASSERTV(MAX_LF, LENGTH, CONFIG, BB == AA);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting `insert(element)` again,"
                                                        " with memory checks");

            if (0 < LENGTH) {
                if (veryVeryVerbose)
                         printf("\t\tOn an object of initial length " ZU ".\n",
                                LENGTH);

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVeryVerbose) {
                    fputs("\t\t\tBEFORE: ", stdout); P(X);
                }

                bslma::TestAllocator scratch2("scratch2", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch2);

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // The number of buckets should not have changed, so no
                    // reason to allocate a fresh bucket array
//                    ASSERTV(MAX_LF, LENGTH, CONFIG,
//                            bucketCount,   X.numBuckets(),
//                            bucketCount == X.numBuckets());

                    // These tests assume that the object allocator is used
                    // only if stored elements also allocate memory.  This does
                    // not allow for rehashes as the container grows.  Hence we
                    // run the same test sequence a second time after clearing
                    // the container, so we can validate knowing that no
                    // rehashes should be necessary, and will in fact show up
                    // as a memory use error.  `LENGTH` was the high-water mark
                    // of the initial run on the container before removing all
                    // elements.
                    if ((LENGTH < X.size() && expectPoolToAllocate(LENGTH))
                        || VALUE_TYPE_USES_ALLOCATOR) {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalUp());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalSame());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseSame());
                    }

                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            BSL_TF_EQ(
                                    KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                    ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[LENGTH - 1],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0; j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) puts("\n\tTesting `insert` duplicated values");
            {
                Link *ITER[MAX_LENGTH + 1];

                // The first loop adds a duplicate in front of each already
                // inserted element
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = mX.insert(VALUES[tj]);
                    ASSERT(0 != ITER[tj]);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(VALUES[tj]),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        2 * LENGTH == X.size());

                // The second loop adds another duplicate in front of each of
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            //fa.deleteObject(objPtr);
            }

            // Verify all memory is released on object destruction.

            ASSERTV(MAX_LF, LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
    }

    // ----------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
    {
        Obj mR(HASHF,
               COMPAREF,
               0,
               std::numeric_limits<float>::denorm_min());

        bool lengthErrorCaught    = false;
        bool wrongExceptionCaught = false;
        try {
            mR.insert(VALUES[0]);

            P(mR.numBuckets())
            P(mR.size());

            ASSERT(false);
        }
        catch(const std::length_error& e) {
            lengthErrorCaught = true;
        }
        catch(...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == lengthErrorCaught   );
        ASSERT(false == wrongExceptionCaught);

    }
#endif  // BDE_BUILD_TARGET_EXC
}
//@bdetdsplit FOR 15 END

//@bdetdsplit FOR 16 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase16()
{
    // ------------------------------------------------------------------------
    // setMaxLoadFactor METHOD
    //
    // Concerns:
    // 1. ...
    //
    //
    // Plan:
    // 1. For each value of increasing length, `L`:
    //
    //
    // Testing:
    //   setMaxLoadFactor
    // ------------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ALLOCATOR       scratchAlloc = makeAllocator(&scratch);

    const HASHER_VAR     HASHF    = makeHasher();
    const COMPARATOR_VAR COMPAREF = makeComparator();

    if (veryVerbose) puts("\nTesting extreme values");
    {

        Obj mX(HASHF, COMPAREF, 1, 1.0f, scratchAlloc);  const Obj& X = mX;

        mX.setMaxLoadFactor(std::numeric_limits<float>::max());
        ASSERT(std::numeric_limits<float>::max() == X.maxLoadFactor());

        mX.setMaxLoadFactor(std::numeric_limits<float>::infinity());
        ASSERT(std::numeric_limits<float>::infinity() == X.maxLoadFactor());
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (veryVerbose) puts("\nTesting exceptional trigger values");
    {
        Obj mX(HASHF, COMPAREF, 1, 1.0f, scratchAlloc);

        bool logicErrorCaught     = false;
        bool wrongExceptionCaught = false;
        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::min());
        }
        catch (const std::logic_error &) {
            logicErrorCaught = true;
        }
        catch (...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == logicErrorCaught    );
        ASSERT(false == wrongExceptionCaught);

        logicErrorCaught     = false;
        wrongExceptionCaught = false;
        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::denorm_min());
        }
        catch (const std::logic_error &) {
            logicErrorCaught = true;
        }
        catch (...) {
            wrongExceptionCaught = true;
        }
        ASSERT( true == logicErrorCaught    );
        ASSERT(false == wrongExceptionCaught);
    }
#endif  // BULD_TARGET_EXC

    if (veryVerbose) puts("Negative testing");
    {
        bsls::AssertTestHandlerGuard hG;

        static const float FLT_TINY = std::numeric_limits<float>::denorm_min();
        static const float FLT_NAN  = std::numeric_limits<float>::quiet_NaN();
        static const float FLT_INF  = std::numeric_limits<float>::infinity();

        (void)FLT_TINY;
        (void)FLT_NAN;

        Obj mX(HASHF, COMPAREF, 0, 1.0f, scratchAlloc);

//        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(  FLT_TINY));
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(      0.5f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(      0.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(     -0.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor( -FLT_TINY));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(     -1.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(  -FLT_INF));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(   FLT_NAN));
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(   FLT_INF));
    }

}
//@bdetdsplit FOR 16 END

//@bdetdsplit FOR 17 BEGIN
template <class CONFIGURATION_PARAMETERS>
void TestCases<CONFIGURATION_PARAMETERS>::testCase17()
{
    // ------------------------------------------------------------------------
    // DEFAULT CONSTRUCTOR:
    //   The default constructor creates an empty container having one bucket.
    //   The state of a default constructed HashTable is that of a
    //   value-constructed HashTable requesting zero initial buckets, a maximum
    //   load factor of `1.0`, and default values for the functors.  This state
    //   is extensively tested as a starting point for most other test cases.
    //   However, the default constructor also allows for a different type of
    //   hasher/comparator functor that is DefaultConstructible, but is not
    //   CopyConstructible.  Such functors produce a HashTable that, in turn,
    //   is not CopyConstructible - but may be Swappable if the functor, in
    //   turn, are swappable.  It is important to test all the other methods of
    //   this class work with such a type, although we are not really adding
    //   anything new other than constructing into this state - therefore, the
    //   default constructor should be the last method tested, so that it can
    //   check every other method being called for a type that is only default
    //   constructible, relying on their otherwise validated behavior from
    //   known states.
    //
    // Concerns:
    // 1. An object created with the default constructor (with or without a
    //    supplied allocator) has the contractually specified default value.
    //
    // 2. If an allocator is NOT supplied to the default constructor, the
    //    default allocator in effect at the time of construction becomes the
    //    object allocator for the resulting object.
    //
    // 3. If an allocator IS supplied to the default constructor, that
    //    allocator becomes the object allocator for the resulting object.
    //
    // 4. Supplying a null allocator address has the same effect as not
    //    supplying an allocator.
    //
    // 5. Supplying an allocator to the default constructor has no effect on
    //    subsequent object values.
    //
    // 6. Any memory allocation is from the object allocator.
    //
    // 7. There is no temporary allocation from any allocator.
    //
    // 8. Every object releases any allocated memory at destruction.
    //
    // 9. QoI: The default constructor allocates no memory.
    //
    // Plan:
    // 1. For each value of increasing length, `L`:
    //
    //   2. Using a loop-based approach, default-construct three distinct
    //      objects, in turn, but configured differently: (a) without passing
    //      an allocator, (b) passing a null allocator address explicitly,
    //      and (c) passing the address of a test allocator distinct from the
    //      default.  For each of these three iterations:  (C-1..14)
    //
    //     1. Create three `bslma::TestAllocator` objects, and install one as
    //        the current default allocator (note that a ubiquitous test
    //        allocator is already installed as the global allocator).
    //
    //     2. Use the default constructor to dynamically create an object
    //        `X`, with its object allocator configured appropriately (see
    //        P-2); use a distinct test allocator for the object's footprint.
    //
    //     3. Use the (as yet unproven) `allocator` to ensure that its
    //        object allocator is properly installed.  (C-2..4)
    //
    //     4. Use the appropriate test allocators to verify that no memory is
    //        allocated by the default constructor.  (C-9)
    //
    //     5. Use the individual (as yet unproven) salient attribute accessors
    //        to verify the default-constructed value.  (C-1)
    //
    //     6. Verify that no temporary memory is allocated from the object
    //        allocator.  (C-7)
    //
    //     7. Verify that all object memory is released when the object is
    //        destroyed.  (C-8)
    //
    // Testing:
    //   HashTable(const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);  // Contains 52 distinct increasing values.

    const char *ALLOC_SPEC = ObjMaker::specForDefaultTests();

    for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
        const char CONFIG = *cfg;  // how we specify the allocator

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        // ----------------------------------------------------------------

        {
        Obj       *objPtr;
        ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                   CONFIG,
                                                   &fa,
                                                   &sa);
        bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                   &fa);
        Obj& mX = *objPtr;  const Obj& X = mX;

        // Verify any attribute allocators are installed properly.

        ASSERTV(CONFIG, expAlloc == X.allocator());

        const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
        const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

        // It is important that these allocators are found, or else the
        // following tests will break severely, dereferencing null pointer.

        BSLS_ASSERT_OPT(oa);
        BSLS_ASSERT_OPT(noa);

        // Confirm the expected state.

        ASSERTV(CONFIG, 0 == X.size());
        ASSERTV(CONFIG, 1 == X.numBuckets());
        ASSERTV(CONFIG, 0 == X.elementListRoot());
        ASSERTV(CONFIG, 1.0f == X.maxLoadFactor());
        ASSERTV(CONFIG, 0.0f == X.loadFactor());
        ASSERTV(CONFIG, 0 == X.countElementsInBucket(0));

        const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
        ASSERTV(CONFIG, 0 == bucket.first());
        ASSERTV(CONFIG, 0 == bucket.last());

//        ASSERTV(CONFIG, areEqualComparators(COMPARATOR(), X.comparator()));
//        ASSERTV(CONFIG, areEqualHashers(HASHER(), X.hasher()));

        // Add any additional fine-grained tests that might be interesting.

        // Verify no allocation from the object/non-object allocators.

        ASSERTV(CONFIG,  oa->numBlocksTotal(), 0 ==  oa->numBlocksTotal());
        ASSERTV(CONFIG, noa->numBlocksTotal(), 0 == noa->numBlocksTotal());

        // -----------------------------------------------------------------

        // check all other methods of the class continue to work as expected
        // for a default constructed object.

        ASSERTV(CONFIG,   X == X);
        ASSERTV(CONFIG, !(X != X));

        // an infinite load factor suggests we should never rehash.  However,
        // in setting the load factor we will allocate the initial bucket
        // array.

        mX.setMaxLoadFactor(std::numeric_limits<float>::infinity());
        ASSERTV(CONFIG, X.maxLoadFactor(),
                X.maxLoadFactor() == std::numeric_limits<float>::infinity());

        size_t INITIAL_BUCKETS = X.numBuckets();
        ASSERTV(CONFIG, 0 == X.size());
        ASSERTV(CONFIG, 1  < INITIAL_BUCKETS);
        ASSERTV(CONFIG, 0 == X.elementListRoot());
        ASSERTV(CONFIG, 0.0f == X.loadFactor());

        mX.insert(VALUES[0]);

        ASSERTV(CONFIG, 1 == X.size());
        ASSERTV(CONFIG, INITIAL_BUCKETS == X.numBuckets());
        ASSERTV(CONFIG, 0 != X.elementListRoot());
        ASSERTV(CONFIG, 0.0f < X.loadFactor());

        // -----------------------------------------------------------------

        // Reclaim dynamically allocated object under test.

        //fa.deleteObject(objPtr);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
    }
}
//@bdetdsplit FOR 17 END

//@bdetdsplit FOR 14..17, 25 BEGIN
//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

namespace UsageExamples {

// BDE_VERIFY pragma: push    // Suppress idiomatic issues with usage examples
// BDE_VERIFY pragma: -CC01   // C-style casts are used for readability
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose
// BDE_VERIFY pragma: -FD01   // Function contract may be documented implicitly
// BDE_VERIFY pragma: set ok_unquoted allocator hash value

///Usage
///-----
// This section illustrates intended use of this component.  The
// `bslstl::HashTable` class template provides a common foundation for
// implementing the four standard unordered containers:
//  - `bsl::unordered_map`
//  - `bsl::unordered_multiset`
//  - `bsl::unordered_multimap`
//  - `bsl::unordered_set`
// This and the subsequent examples in this component use the
// `bslstl::HashTable` class to implement several model container classes, each
// providing a small but representative sub-set of the functionality of one of
// the standard unordered containers.
//
///Example 1: Implementing a Hashed Set Container
///----------------------------------------------
// Suppose we wish to implement, `MyHashedSet`, a greatly abbreviated version
// of `bsl::unordered_set`.  The `bslstl::HashTable` class template can be used
// as the basis of that implementation.
//
// First, we define `UseEntireValueAsKey`, a class template we can use to
// configure `bslstl::HashTable` to use its entire elements as keys for its
// hasher, a policy suitable for a set container.  (Later, in {Example 2}, we
// will define `UseFirstValueOfPairAsKey` for use in a map container.  Note
// that, in practice, developers can use the existing classes in
// {`bslstl_unorderedmapkeyconfiguration`} and
// {`bslstl_unorderedsetkeyconfiguration`}.)
// ```
                            // ==========================
                            // struct UseEntireValueAsKey
                            // ==========================

    /// This `struct` provides a namespace for types and methods that define
    /// the policy by which the key value of a hashed container (i.e., the
    /// value passed to the hasher) is extracted from the objects stored in
    /// the hashed container (the `value` type).
    template <class VALUE_TYPE>
    struct UseEntireValueAsKey {

        /// Alias for `VALUE_TYPE`, the type stored in the hashed container.
        typedef VALUE_TYPE ValueType;

        /// Alias for the type passed to the hasher by the hashed container.
        /// In this policy, that type is `ValueType`.
        typedef ValueType KeyType;

        /// Return the key value for the specified `value`.  In this policy,
        /// that is `value` itself.
        static const KeyType& extractKey(const ValueType& value);
    };

                            // --------------------------
                            // struct UseEntireValueAsKey
                            // --------------------------

    template <class VALUE_TYPE>
    inline
    const typename UseEntireValueAsKey<VALUE_TYPE>::KeyType&
                   UseEntireValueAsKey<VALUE_TYPE>::extractKey(
                                                        const ValueType& value)
    {
        return value;
    }
// ```
// Next, we define our `MyHashedSet` class template with an instance of
// `bslstl::HashTable` (configured using `UseEntireValueAsKey`) as its sole
// data member.  We provide `insert` method, to allow us to populate these
// sets, and the `find` method to allow us to examine those elements.  We also
// provide `size` and `bucket_count` accessor methods to let us check the inner
// workings of our class.
//
// Note that the standard classes define aliases for the templated parameters
// and other types.  In the interest of brevity, this model class (and the
// classes in the subsequent examples) do not define such aliases except where
// strictly needed for the example.
// ```
                            // =================
                            // class MyHashedSet
                            // =================

    template <class KEY,
              class HASHF     = bsl::hash<     KEY>,
              class EQUAL     = bsl::equal_to< KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedSet
    {
      private:
        // PRIVATE TYPES
        typedef bsl::allocator_traits<ALLOCATOR>               AllocatorTraits;
        typedef typename AllocatorTraits::difference_type      difference_type;
        typedef BloombergLP::bslstl::HashTableIterator<
                                          const KEY, difference_type> iterator;

        typedef UseEntireValueAsKey<KEY>                               HashKey;
        typedef BloombergLP::bslstl::HashTable<HashKey,
                                               HASHF,
                                               EQUAL,
                                               ALLOCATOR>         ImpHashTable;

        // DATA
        ImpHashTable d_impl;

      public:
        // TYPES
        typedef typename AllocatorTraits::size_type size_type;
        typedef iterator                            const_iterator;

        // CREATORS

        /// Create an empty `MyHashedSet` object having a maximum load
        /// factor of 1.  Optionally specify at least `initialNumBuckets` in
        /// this container's initial array of buckets.  If
        /// `initialNumBuckets` is not supplied, an implementation defined
        /// value is used.  Optionally specify a `hash` used to generate the
        /// hash values associated to the keys extracted from the values
        /// contained in this object.  If `hash` is not supplied, a
        /// default-constructed object of type `HASH` is used.  Optionally
        /// specify a key-equality functor `keyEqual` used to verify that
        /// two key values are the same.  If `keyEqual` is not supplied, a
        /// default-constructed object of type `EQUAL` is used.  Optionally
        /// specify an `allocator` used to supply memory.  If `allocator` is
        /// not supplied, a default-constructed object of the (template
        /// parameter) type `ALLOCATOR` is used.  If the `ALLOCATOR` is
        /// `bsl::allocator` (the default), then `allocator` shall be
        /// convertible to `bslma::Allocator *`.  If the `ALLOCATOR` is
        /// `bsl::allocator` and `allocator` is not supplied, the currently
        /// installed default allocator will be used to supply memory.
        explicit MyHashedSet(size_type        initialNumBuckets = 0,
                             const HASHF&     hash              = HASHF(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());

        //! ~MyHashedSet() = default;
            // Destroy this object.

        // MANIPULATORS

        /// Insert the specified `value` into this set if the `value` does
        /// not already exist in this set; otherwise, this method has no
        /// effect.  Return a pair whose `first` member is an iterator
        /// providing non-modifiable access to the (possibly newly inserted)
        /// `KEY` object having `value` (according to `EQUAL`) and whose
        /// `second` member is `true` if a new element was inserted, and
        /// `false` if `value` was already present.
        bsl::pair<const_iterator, bool> insert(const KEY& value);

        // ACCESSORS

        /// Return the number of buckets in this set.
        size_type bucket_count() const;

        /// Return an iterator providing non-modifiable access to the
        /// past-the-end element (in the sequence of `KEY` objects)
        /// maintained by this set.
        const_iterator cend() const;

        /// Return an iterator providing non-modifiable access to the `KEY`
        /// object in this set having the specified `key`, if such an entry
        /// exists, and the iterator returned by the `cend` method
        /// otherwise.
        const_iterator find(const KEY& key) const;

        /// Return the number of elements in this set.
        size_type size() const;
    };
// ```
// Next, we implement the methods of `MyHashedSet`.  In many cases, the
// implementations consist mainly in forwarding arguments to and returning
// values from the underlying `bslstl::HashTable`.
// ```
                            // =================
                            // class MyHashedSet
                            // =================

    // CREATORS
    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::MyHashedSet(
                                            size_type        initialNumBuckets,
                                            const HASHF&     hash,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
// ```
// Note that the `insertIfMissing` method of `bslstl::HashTable` provides the
// semantics needed for adding values (unique values only) to sets.
// ```
    // MANIPULATORS
    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    bsl::pair<typename MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::iterator,
              bool>    MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::insert(
                                                              const KEY& value)
    {
        typedef bsl::pair<iterator, bool> ResultType;

        bool                       isInsertedFlag = false;
        bslalg::BidirectionalLink *result         = d_impl.insertIfMissing(
                                                               &isInsertedFlag,
                                                               value);
        return ResultType(iterator(result), isInsertedFlag);
    }

    // ACCESSORS
    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::size_type
             MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::bucket_count() const
    {
        return d_impl.numBuckets();
    }

    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::const_iterator
             MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::cend() const
    {
        return const_iterator();
    }

    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::const_iterator
             MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::find(const KEY& key)
                                                                          const
    {
        return const_iterator(d_impl.find(key));
    }

    template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::size_type
             MyHashedSet<KEY, HASHF, EQUAL, ALLOCATOR>::size() const
    {
        return d_impl.size();
    }

    void main1()
    {
if (veryVerbose) puts("Usage Example1");
// ```
// Finally, we create `mhs`, an instance of `MyHashedSet`, exercise it, and
// confirm that it behaves as expected.
// ```
    MyHashedSet<int> mhs;
    ASSERT( 0    == mhs.size());
    ASSERT( 1    == mhs.bucket_count());
// ```
// Notice that the newly created set is empty and has a single bucket.
//
// Inserting a value (10) succeeds the first time but correctly fails on the
// second attempt.
// ```
    bsl::pair<MyHashedSet<int>::const_iterator, bool> status;

    status = mhs.insert(10);
    ASSERT( 1    ==  mhs.size());
    ASSERT(10    == *status.first);
    ASSERT(true  ==  status.second);

    status = mhs.insert(10);
    ASSERT( 1    ==  mhs.size());
    ASSERT(10    == *status.first);
    ASSERT(false ==  status.second);
// ```
// We can insert a different value (20) and thereby increase the set size to 2.
// ```
    status = mhs.insert(20);
    ASSERT( 2    ==  mhs.size());
    ASSERT(20    == *status.first);
    ASSERT(true  ==  status.second);
// ```
// Each of the inserted values (10, 20) can be found in the set.
// ```
    MyHashedSet<int>::const_iterator itr, end = mhs.cend();

    itr = mhs.find(10);
    ASSERT(end !=  itr);
    ASSERT(10  == *itr);

    itr = mhs.find(20);
    ASSERT(end !=  itr);
    ASSERT(20  == *itr);
// ```
// However, a value known to absent from the set (0), is correctly reported as
// not there.
// ```
    itr = mhs.find(0);
    ASSERT(end ==  itr);
// ```
    }

///Example 2: Implementing a Hashed Map Container
///----------------------------------------------
// Suppose we wish to implement, `MyHashedMap`, a greatly abbreviated version
// of `bsl::unordered_map`.  As with `MyHashedSet` (see {Example 1}), the
// `bslstl::HashTable` class template can be used as the basis of our
// implementation.
//
// First, we define `UseFirstValueOfPairAsKey`, a class template we can use to
// configure `bslstl::HashTable` to use the `first` member of each element,
// each a `bsl::pair`, as the key-value for hashing.  Note that, in practice,
// developers can use class defined in {`bslstl_unorderedmapkeyconfiguration`}.
// ```
                            // ===============================
                            // struct UseFirstValueOfPairAsKey
                            // ===============================

    /// This `struct` provides a namespace for types and methods that define
    /// the policy by which the key value of a hashed container (i.e., the
    /// value passed to the hasher) is extracted from the objects stored in
    /// the hashed container (the `value` type).
    template <class VALUE_TYPE>
    struct UseFirstValueOfPairAsKey {

        /// Alias for `VALUE_TYPE`, the type stored in the hashed container.
        /// For this policy `ValueType` must define a public member named
        /// `first` of type `first_type`.
        typedef VALUE_TYPE ValueType;

        /// Alias for the type passed to the hasher by the hashed container.
        /// In this policy, that type is the type of the `first` element of
        /// `ValueType`.
        typedef typename ValueType::first_type KeyType;

        /// Return the key value for the specified `value`.  In this policy,
        /// that is the value of the `first` member of `value`.
        static const KeyType& extractKey(const ValueType& value);
    };

                            // -------------------------------
                            // struct UseFirstValueOfPairAsKey
                            // -------------------------------

    template <class VALUE_TYPE>
    inline
    const typename UseFirstValueOfPairAsKey<VALUE_TYPE>::KeyType&
                   UseFirstValueOfPairAsKey<VALUE_TYPE>::extractKey(
                                                        const ValueType& value)
    {
        return value.first;
    }
// ```
// Next, we define our `MyHashedMap` class template with an instance of
// `bslstl::HashTable` (configured using `UseFirstValueOfPairAsKey`) as its
// sole data member.  In this example, we choose to implement `operator[]`
// (corresponding to the signature method of `bsl::unordered_map`) to allow us
// to populate our maps and to examine their elements.
// ```
                            // =================
                            // class MyHashedMap
                            // =================

    template <class KEY,
              class VALUE,
              class HASHF     = bsl::hash<     KEY>,
              class EQUAL     = bsl::equal_to< KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedMap
    {
      private:
        // PRIVATE TYPES
        typedef bsl::allocator_traits<ALLOCATOR>               AllocatorTraits;

        typedef UseFirstValueOfPairAsKey<bsl::pair<const KEY, VALUE> > HashKey;
        typedef BloombergLP::bslstl::HashTable<HashKey,
                                               HASHF,
                                               EQUAL,
                                               ALLOCATOR>         ImpHashTable;
        // DATA
        ImpHashTable d_impl;

      public:
        // TYPES
        typedef typename AllocatorTraits::size_type size_type;

        // CREATORS

        /// Create an empty `MyHashedMap` object having a maximum load
        /// factor of 1.  Optionally specify at least `initialNumBuckets` in
        /// this container's initial array of buckets.  If
        /// `initialNumBuckets` is not supplied, one empty bucket shall be
        /// used and no memory allocated.  Optionally specify `hash` to
        /// generate the hash values associated with the key-value pairs
        /// contained in this unordered map.  If `hash` is not supplied, a
        /// default-constructed object of (template parameter) `HASH` is
        /// used.  Optionally specify a key-equality functor `keyEqual` used
        /// to determine whether two keys have the same value.  If
        /// `keyEqual` is not supplied, a default-constructed object of
        /// (template parameter) `EQUAL` is used.  Optionally specify an
        /// `allocator` used to supply memory.  If `allocator` is not
        /// supplied, a default-constructed object of the (template
        /// parameter) type `ALLOCATOR` is used.  If `ALLOCATOR` is
        /// `bsl::allocator` (the default), then `allocator` shall be
        /// convertible to `bslma::Allocator *`.  If `ALLOCATOR` is
        /// `bsl::allocator` and `allocator` is not supplied, the currently
        /// installed default allocator will be used to supply memory.  Note
        /// that more than `initialNumBuckets` buckets may be created in
        /// order to preserve the bucket allocation strategy of the
        /// hash-table (but never fewer).
        explicit MyHashedMap(size_type        initialNumBuckets = 0,
                             const HASHF&     hash              = HASHF(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());

        //! ~MyHashedMap() = default;
            // Destroy this object.

        // MANIPULATORS

        /// Return a reference providing modifiable access to the
        /// mapped-value associated with the specified `key` in this
        /// unordered map; if this unordered map does not already contain a
        /// `value_type` object with `key`, first insert a new `value_type`
        /// object having `key` and a default-constructed `VALUE` object.
        /// This method requires that the (template parameter) type `KEY` is
        /// "copy-constructible" and the (template parameter) `VALUE` is
        /// "default-constructible".
        VALUE& operator[](const KEY& key);
    };
// ```
// Then, we implement the methods `MyHashedMap`.  The construct need merely
// forward its arguments to the constructor of `d_impl`,
// ```
                            // =================
                            // class MyHashedMap
                            // =================

    // CREATORS
    template <class KEY,
              class VALUE,
              class HASHF,
              class EQUAL,
              class ALLOCATOR>
    inline
    MyHashedMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::MyHashedMap(
                                            size_type        initialNumBuckets,
                                            const HASHF&     hash,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
// ```
// As with `MyHashedSet`, the `insertIfMissing` method of `bslstl::HashTable`
// provides the semantics we need: an element is inserted only if no such
// element (no element with the same key) in the container, and a reference to
// that element (`node`) is returned.  Here, we use `node` to obtain and return
// a reference offering modifiable access to the `second` member of the
// (possibly newly added) element.  Note that the `static_cast` from
// `HashTableLink *` to `HashTableNode *` is valid because the nodes derive
// from the link type (see `bslalg_bidirectionallink` and
// `bslalg_hashtableimputil`).
// ```
    // MANIPULATORS
    template <class KEY,
              class VALUE,
              class HASHF,
              class EQUAL,
              class ALLOCATOR>
    VALUE& MyHashedMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::operator[](
                                                                const KEY& key)
    {
        typedef typename ImpHashTable::NodeType        HashTableNode;
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *node = d_impl.insertIfMissing(key);
        return static_cast<HashTableNode *>(node)->value().second;
    }

    void main2()
    {
if (veryVerbose) puts("Usage Example2");
// ```
// Finally, we create `mhm`, an instance of `MyHashedMap`, exercise it, and
// confirm that it behaves as expected.  We can add an element (with key value
// of 0).
// ```
    MyHashedMap<int, double> mhm;

    mhm[0] = 1.234;
    ASSERT(1.234 == mhm[0]);
// ```
// We can change the value of the element with key value 0.
// ```
    mhm[0] = 4.321;
    ASSERT(4.321 == mhm[0]);
// ```
// We can add a new element (key value 1), without changing the previously
// existing element (key value 0).
// ```
    mhm[1] = 5.768;
    ASSERT(5.768 == mhm[1]);
    ASSERT(4.321 == mhm[0]);
// ```
// Accessing a non-existing element (key value 2) creates that element and
// populates it with the default value of the mapped value (0.0).
// ```
    ASSERT(0.000 == mhm[2]);
// ```
    }
//
///Example 3: Implementing a Hashed Multi-Map Container
///----------------------------------------------------
// Suppose we wish to implement, `MyHashedMultiMap`, a greatly abbreviated
// version of `bsl::unordered_multimap`.  As with `MyHashedSet` and
// `MyHashedMap` (see {Example 1}, and {Example 2}, respectively), the
// `bslstl::HashTable` class template can be used as the basis of our
// implementation.
//
// First, we need a class template to configure `bslstl::HashTable` to extract
// key values in manner appropriate for maps.  The previously defined
// `UseFirstValueOfPairAsKey` class template (see {Example 2}) suits perfectly.
//
// Next, we define our `MyHashedMultiMap` class template with an instance of
// `bslstl::HashTable` (configured using `UseFirstValueOfPairAsKey`) as its
// sole data member.  In this example, we choose to implement an `insert`
// method to populate our container, and an `equal_range` method (a signature
// method of the multi containers) to provide access to those elements.
// ```
                            // ======================
                            // class MyHashedMultiMap
                            // ======================

    template <class KEY,
              class VALUE,
              class HASHF     = bsl::hash<     KEY>,
              class EQUAL     = bsl::equal_to< KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedMultiMap
    {
      private:
        // PRIVATE TYPES
        typedef bsl::pair<const KEY, VALUE>                    value_type;
        typedef bsl::allocator_traits<ALLOCATOR>               AllocatorTraits;
        typedef typename AllocatorTraits::difference_type      difference_type;

        typedef UseFirstValueOfPairAsKey<bsl::pair<const KEY, VALUE> > HashKey;
        typedef BloombergLP::bslstl::HashTable<HashKey,
                                               HASHF,
                                               EQUAL,
                                               ALLOCATOR>         ImpHashTable;
        // DATA
        ImpHashTable d_impl;

      public:
        // TYPES
        typedef typename AllocatorTraits::size_type                  size_type;
        typedef BloombergLP::bslstl::HashTableIterator<value_type,
                                                       difference_type>
                                                                      iterator;
        typedef BloombergLP::bslstl::HashTableIterator<const value_type,
                                                       difference_type>
                                                                const_iterator;

        // CREATORS

        /// Create an empty `MyHashedMultiMap` object having a maximum load
        /// factor of 1.  Optionally specify at least `initialNumBuckets` in
        /// this container's initial array of buckets.  If
        /// `initialNumBuckets` is not supplied, an implementation defined
        /// value is used.  Optionally specify a `hash`, a hash-functor used
        /// to generate the hash values associated to the key-value pairs
        /// contained in this object.  If `hash` is not supplied, a
        /// default-constructed object of (template parameter) `HASH` type
        /// is used.  Optionally specify a key-equality functor `keyEqual`
        /// used to verify that two key values are the same.  If `keyEqual`
        /// is not supplied, a default-constructed object of (template
        /// parameter) `EQUAL` type is used.  Optionally specify an
        /// `allocator` used to supply memory.  If `allocator` is not
        /// supplied, a default-constructed object of the (template
        /// parameter) `ALLOCATOR` type is used.  If `ALLOCATOR` is
        /// `bsl::allocator` (the default), then `allocator` shall be
        /// convertible to `bslma::Allocator *`.  If the `ALLOCATOR` is
        /// `bsl::allocator` and `allocator` is not supplied, the currently
        /// installed default allocator will be used to supply memory.
        explicit MyHashedMultiMap(
                             size_type        initialNumBuckets = 0,
                             const HASHF&     hash              = HASHF(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());

        //! ~MyHashedMultiMap() = default;
            // Destroy this object.

        // MANIPULATORS

        /// Insert the specified `value` into this multi-map, and return an
        /// iterator to the newly inserted element.  This method requires
        /// that the (class template parameter) types `KEY` and `VALUE`
        /// types both be "copy-constructible", and that the (function
        /// template parameter) `SOURCE_TYPE` be convertible to the (class
        /// template parameter) `VALUE` type.
        template <class SOURCE_TYPE>
        iterator insert(const SOURCE_TYPE& value);

        // ACCESSORS

        /// Return a pair of iterators providing non-modifiable access to
        /// the sequence of `value_type` objects in this container matching
        /// the specified `key`, where the first iterator is positioned at
        /// the start of the sequence and the second iterator is positioned
        /// one past the end of the sequence.  If this container contains no
        /// `value_type` objects matching `key` then the two returned
        /// iterators will have the same value.
        bsl::pair<const_iterator, const_iterator> equal_range(const KEY& key)
                                                                         const;
    };
// ```
// Then, we implement the methods `MyHashedMultiMap`.  The construct need
// merely forward its arguments to the constructor of `d_impl`,
// ```
                            // ======================
                            // class MyHashedMultiMap
                            // ======================

    // CREATORS
    template <class KEY,
              class VALUE,
              class HASHF,
              class EQUAL,
              class ALLOCATOR>
    inline
    MyHashedMultiMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::MyHashedMultiMap(
                                            size_type        initialNumBuckets,
                                            const HASHF&     hash,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
// ```
// Note that here we forgo use of the `insertIfMissing` method and use the
// `insert` method of `bslstl::HashTable`.  This method supports the semantics
// of the multi containers: there can be more than one element with the same
// key value.
// ```
    // MANIPULATORS
    template <class KEY,
              class VALUE,
              class HASHF,
              class EQUAL,
              class ALLOCATOR>
    template <class SOURCE_TYPE>
    inline
    typename MyHashedMultiMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::iterator
             MyHashedMultiMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
    {
        return iterator(d_impl.insert(value));
    }
// ```
// The `equal_range` method need only convert the values returned by the
// `findRange` method to the types expected by the caller.
// ```
    // ACCESSORS
    template <class KEY,
              class VALUE,
              class HASHF,
              class EQUAL,
              class ALLOCATOR>
    bsl::pair<typename MyHashedMultiMap<KEY,
                                        VALUE,
                                        HASHF,
                                        EQUAL,
                                        ALLOCATOR>::const_iterator,
              typename MyHashedMultiMap<KEY,
                                        VALUE,
                                        HASHF,
                                        EQUAL, ALLOCATOR>::const_iterator>
    MyHashedMultiMap<KEY, VALUE, HASHF, EQUAL, ALLOCATOR>::equal_range(
                                                          const KEY& key) const
    {
        typedef bsl::pair<const_iterator, const_iterator>  ResultType;
        typedef BloombergLP::bslalg::BidirectionalLink     HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_impl.findRange(&first, &last, key);
        return ResultType(const_iterator(first), const_iterator(last));
    }

    void main3()
    {
if (veryVerbose) puts("Usage Example3");
// ```
// Finally, we create `mhmm`, an instance of `MyHashedMultiMap`, exercise it,
// and confirm that it behaves as expected.
//
// We define several aliases to make our code more concise.
// ```
    typedef MyHashedMultiMap<int, double>::iterator        Iterator;
    typedef MyHashedMultiMap<int, double>::const_iterator  ConstIterator;
    typedef bsl::pair<ConstIterator, ConstIterator>        ConstRange;
// ```
// Searching for an element (key value 10) in a newly created, empty container
// correctly shows the absence of any such element.
// ```
    MyHashedMultiMap<int, double> mhmm;

    ConstRange range;
    range = mhmm.equal_range(10);
    ASSERT(range.first == range.second);
// ```
// We can insert a value (the pair 10, 100.00) into the container...
// ```
    bsl::pair<const int, double> value(10, 100.00);

    Iterator itr;

    itr = mhmm.insert(value);
    ASSERT(value == *itr);
// ```
// ... and we can do so again.
// ```
    itr = mhmm.insert(value);
    ASSERT(value == *itr);
// ```
// We can now find elements with the key value of 10.
// ```
    range = mhmm.equal_range(10);
    ASSERT(range.first != range.second);
// ```
// As expected, there are two such elements, and both are identical in key
// value (10) and mapped value (100.00).
// ```
    int count = 0;
    for (ConstIterator cur  = range.first,
                       end  = range.second;
                       end != cur; ++cur, ++count) {
        ASSERT(value == *cur);
    }
    ASSERT(2 == count);
// ```
    }
//
///Example 4: Implementing a Custom Container
///------------------------------------------
// Although the `bslstl::HashTable` class was created to be a common
// implementation for the standard unordered classes, this class can also be
// used in its own right to address other user problems.
//
// Suppose that we wish to retain a record of sales orders, that each record is
// characterized by several integer attributes, and that we must be able to
// find records based on *any* of those attributes.  We can use
// `bslstl::HashTable` to implement a custom container supporting multiple
// key-values.
//
// First, we define `MySalesRecord`, our record class:
// ```
    enum { MAX_DESCRIPTION_SIZE = 16 };

    struct MySalesRecord {
        int  orderNumber;                        // unique
        int  customerId;                         // no constraint
        int  vendorId;                           // no constraint
        char description[MAX_DESCRIPTION_SIZE];  // ascii string
    };
// ```
// Notice that only each `orderNumber` is unique.  We expect multiple sales to
// any given customer (`customerId`) and multiple sales by any given vendor
// (`vendorId`).
//
// We will use a `bslstl::HashTable` object (a hashtable) to save record values
// based on the unique `orderNumber`, and two auxiliary hashtables to provide
// map `customerId` and `vendorId` values to the addresses of the records in
// the first `bslstl::HashTable` object.  Note that this implementation relies
// on the fact that nodes in our hashtables remain stable until they are
// removed and that in this application we do *not* allow the removal (or
// modification) of records once they are inserted.
//
// To configure these hashtables, we will need several policy objects to
// extract relevant portions the `MySalesRecord` objects for hashing.
//
// Next, define `UseOrderNumberAsKey`, a policy class for the hashtable holding
// the sales record objects.  Note that the `ValueType` is `MySalesRecord` and
// that the `extractKey` method selects the `orderNumber` attribute:
// ```
                            // ==========================
                            // struct UseOrderNumberAsKey
                            // ==========================

    /// This `struct` provides a namespace for types and methods that define
    /// the policy by which the key value of a hashed container (i.e., the
    /// value passed to the hasher) is extracted from the objects stored in
    /// the hashed container (the `value` type).
    struct UseOrderNumberAsKey {

        /// Alias for `MySalesRecord`, the type stored in the first
        /// hashtable.
        typedef MySalesRecord ValueType;

        /// Alias for the type passed to the hasher by the hashed container.
        /// In this policy, the value passed to the hasher is the
        /// `orderNumber` attribute, an `int` type.
        typedef int KeyType;

        /// Return the key value for the specified `value`.  In this policy,
        /// that is the `orderNumber` attribute of `value`.
        static const KeyType& extractKey(const ValueType& value);
    };

                            // --------------------------
                            // struct UseOrderNumberAsKey
                            // --------------------------

    inline
    const UseOrderNumberAsKey::KeyType&
          UseOrderNumberAsKey::extractKey(const ValueType& value)
    {
        return value.orderNumber;
    }
// ```
// Then, we define `UseCustomerIdAsKey`, the policy class for the hashtable
// that will multiply map `customerId` to the addresses of records in the first
// hashtable.  Note that in this policy class the `ValueType` is
// `const MySalesRecord *`.
// ```
                            // =========================
                            // struct UseCustomerIdAsKey
                            // =========================

    /// This `struct` provides a namespace for types and methods that define
    /// the policy by which the key value of a hashed container (i.e., the
    /// value passed to the hasher) is extracted from the objects stored in
    /// the hashed container (the `value` type).
    struct UseCustomerIdAsKey {

        /// Alias for `const MySalesRecord *`, the type stored in second
        /// hashtable, a pointer to the record stored in the first
        /// hashtable.
        typedef const MySalesRecord *ValueType;

        /// Alias for the type passed to the hasher by the hashed container.
        /// In this policy, the value passed to the hasher is the
        /// `orderNumber` attribute, an `int` type.
        typedef int KeyType;

        /// Return the key value for the specified `value`.  In this policy,
        /// that is the `customerId` attribute of `value`.
        static const KeyType& extractKey(const ValueType& value);
    };

                            // -------------------------
                            // struct UseCustomerIdAsKey
                            // -------------------------

    inline
    const UseCustomerIdAsKey::KeyType&
          UseCustomerIdAsKey::extractKey(const ValueType& value)
    {
        return value->customerId;
    }
// ```
// Notice that, since the values in the second hashtable are addresses, the
// key-value is extracted by reference.  This second hashtable allows what
// map-like semantics, *without* having to store key-values; those reside in
// the records in the first hashtable.
//
// The `UseVendorIdAsKey` class, the policy class for the hashtable providing
// an index by `vendorId`, is almost a near clone of `UseCustomerIdAsKey`.  It
// is shown for completeness:
// ```
                            // =======================
                            // struct UseVendorIdAsKey
                            // ========================

    /// This `struct` provides a namespace for types and methods that define
    /// the policy by which the key value of a hashed container (i.e., the
    /// value passed to the hasher) is extracted from the objects stored in
    /// the hashed container (the `value` type).
    struct UseVendorIdAsKey {

        /// Alias for `const MySalesRecord *`, the type stored in second
        /// hashtable, a pointer to the record stored in the first
        /// hashtable.
        typedef const MySalesRecord *ValueType;

        /// Alias for the type passed to the hasher by the hashed container.
        /// In this policy, the value passed to the hasher is the `vendorId`
        /// attribute, an `int` type.
        typedef int KeyType;

        /// Return the key value for the specified `value`.  In this policy,
        /// that is the `vendorId` attribute of `value`.
        static const KeyType& extractKey(const ValueType& value);
    };

                            // -----------------------
                            // struct UseVendorIdAsKey
                            // -----------------------

    inline
    const UseVendorIdAsKey::KeyType&
          UseVendorIdAsKey::extractKey(const ValueType& value)
    {
        return value->vendorId;
    }
// ```
// Next, we define `MySalesRecordContainer`, our customized container:
// ```
                            // ----------------------------
                            // class MySalesRecordContainer
                            // ----------------------------

    class MySalesRecordContainer
    {
      private:
        // PRIVATE TYPES
        typedef BloombergLP::bslstl::HashTable<
                      UseOrderNumberAsKey,
                      bsl::hash<    UseOrderNumberAsKey::KeyType>,
                      bsl::equal_to<UseOrderNumberAsKey::KeyType> >
                                                          RecordsByOrderNumber;
        typedef bsl::allocator_traits<
              bsl::allocator<UseOrderNumberAsKey::ValueType> > AllocatorTraits;
        typedef AllocatorTraits::difference_type               difference_type;
// ```
// The `ItrByOrderNumber` type is used to provide access to the elements of the
// first hash table, the one that stores the records.
// ```

        typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord,
                                                       difference_type>
                                                              ItrByOrderNumber;
// ```
// The `ItrPtrById` type is used to provide access to the elements of the other
// hashtables, the ones that store pointers into the first hashtable.
// ```
        typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord *,
                                                       difference_type>
                                                                    ItrPtrById;
// ```
// If we were to provide iterators of type `ItrPtrById` to our users,
// dereferencing the iterator would provide a `MySalesRecord` pointer, which
// would then have to be dereferences.  Instead, we use `ItrPtrById` to define
// `ItrById` in which accessors have been overriden to provide that extra
// dereference implicitly.
// ```
        class ItrById : public ItrPtrById
        {
          public:
            // CREATORS
            explicit ItrById(bslalg::BidirectionalLink *node)
            : ItrPtrById(node)
            {
            }

            // ACCESSORS
            const MySalesRecord& operator*() const
            {
                return *ItrPtrById::operator*();
            }

            const MySalesRecord *operator->() const
            {
                return &(*ItrPtrById::operator*());
            }
        };

        typedef BloombergLP::bslstl::HashTable<
                      UseCustomerIdAsKey,
                      bsl::hash<    UseCustomerIdAsKey::KeyType>,
                      bsl::equal_to<UseCustomerIdAsKey::KeyType> >
                                                       RecordsPtrsByCustomerId;
        typedef BloombergLP::bslstl::HashTable<
                      UseVendorIdAsKey,
                      bsl::hash<    UseVendorIdAsKey::KeyType>,
                      bsl::equal_to<UseVendorIdAsKey::KeyType> >
                                                         RecordsPtrsByVendorId;
        // DATA
        RecordsByOrderNumber    d_recordsByOrderNumber;
        RecordsPtrsByCustomerId d_recordptrsByCustomerId;
        RecordsPtrsByVendorId   d_recordptrsByVendorId;

      public:
        // PUBLIC TYPES
        typedef ItrByOrderNumber  ConstItrByOrderNumber;
        typedef ItrById           ConstItrById;

        // CREATORS

        /// Create an empty `MySalesRecordContainer` object.  Optionally
        /// specify a `basicAllocator` to supply memory.  If
        /// `basicAllocator` is 0, the currently installed default allocator
        /// is used.
        explicit MySalesRecordContainer(bslma::Allocator *basicAllocator = 0);

        //! ~MySalesRecordContainer() = default;
            // Destroy this object.

        // MANIPULATORS

        /// Insert the specified `value` into this set if the `value` does
        /// not already exist in this set; otherwise, this method has no
        /// effect.  Return a pair whose `first` member is an iterator
        /// providing non-modifiable access to the (possibly newly inserted)
        /// `MySalesRecord` object having `value` and whose `second` member
        /// is `true` if a new element was inserted, and `false` if `value`
        /// was already present.
        bsl::pair<ConstItrByOrderNumber, bool> insert(
                                                   const MySalesRecord& value);

        // ACCESSORS

        /// Return an iterator providing non-modifiable access to the
        /// past-the-end element (in the sequence of `MySalesRecord`
        /// objects) maintained by this set.
        ConstItrByOrderNumber cend() const;

        /// Return an iterator providing non-modifiable access to the
        /// `MySalesRecord` object in this set having the specified `value`,
        /// if such an entry exists, and the iterator returned by the `cend`
        /// method otherwise.
        ConstItrByOrderNumber findByOrderNumber(int value) const;
// ```
// Notice that this interface provides map-like semantics for finding records.
// We need only specify the `orderNumber` attribute of the record of interest;
// however, the return value is set-like: we get access to the record, not the
// more complicated key-value/record pair that a map would have provided.
//
// Internally, the hash table need only store the records themselves.  A map
// would have had to manage key-value/record pairs, where the key-value would
// be a copy of part of the record.
// ```

        /// Return a pair of iterators providing non-modifiable access to
        /// the sequence of `MySalesRecord` objects in this container having
        /// a `customerId` attribute equal to the specified `value` where
        /// the first iterator is positioned at the start of the sequence
        /// and the second iterator is positioned one past the end of the
        /// sequence.  If this container has no such objects, then the two
        /// iterators will be equal.
        bsl::pair<ConstItrById, ConstItrById> findByCustomerId(int value)
                                                                         const;

        /// Return a pair of iterators providing non-modifiable access to
        /// the sequence of `MySalesRecord` objects in this container having
        /// a `vendorId` attribute equal to the specified `value` where the
        /// first iterator is positioned at the start of the sequence and
        /// the second iterator is positioned one past the end of the
        /// sequence.  If this container has no such objects, then the two
        /// iterators will be equal.
        bsl::pair<ConstItrById, ConstItrById> findByVendorId(int value) const;
    };
// ```
// Then, we implement the methods of `MySalesRecordContainer`, our customized
// container:
// ```
                            // ----------------------------
                            // class MySalesRecordContainer
                            // ----------------------------

    // CREATORS
    inline
    MySalesRecordContainer::MySalesRecordContainer(
                                              bslma::Allocator *basicAllocator)
    : d_recordsByOrderNumber(basicAllocator)
    , d_recordptrsByCustomerId(basicAllocator)
    , d_recordptrsByVendorId(basicAllocator)
    {
    }

    // MANIPULATORS
    inline
    bsl::pair<MySalesRecordContainer::ConstItrByOrderNumber, bool>
    MySalesRecordContainer::insert(const MySalesRecord& value)
    {
        // Insert into internal container that will own the record.

        bool                                    isInsertedFlag = false;
        BloombergLP::bslalg::BidirectionalLink *result         =
                d_recordsByOrderNumber.insertIfMissing(&isInsertedFlag, value);

        // Index by other record attributes

        RecordsByOrderNumber::NodeType *nodePtr =
                         static_cast<RecordsByOrderNumber::NodeType *>(result);

        d_recordptrsByCustomerId.insert(&nodePtr->value());
          d_recordptrsByVendorId.insert(&nodePtr->value());

        // Return of insertion.

        return bsl::pair<ConstItrByOrderNumber, bool>(
                                                 ConstItrByOrderNumber(result),
                                                 isInsertedFlag);
    }

    // ACCESSORS
    inline
    MySalesRecordContainer::ConstItrByOrderNumber
    MySalesRecordContainer::cend() const
    {
        return ConstItrByOrderNumber();
    }

    inline
    MySalesRecordContainer::ConstItrByOrderNumber
    MySalesRecordContainer::findByOrderNumber(int value) const
    {
        return ConstItrByOrderNumber(d_recordsByOrderNumber.find(value));
    }

    inline
    bsl::pair<MySalesRecordContainer::ConstItrById,
              MySalesRecordContainer::ConstItrById>
    MySalesRecordContainer::findByCustomerId(int value) const
    {
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_recordptrsByCustomerId.findRange(&first, &last, value);

        return bsl::pair<ConstItrById, ConstItrById>(ConstItrById(first),
                                                     ConstItrById(last));
    }

    inline
    bsl::pair<MySalesRecordContainer::ConstItrById,
              MySalesRecordContainer::ConstItrById>
    MySalesRecordContainer::findByVendorId(int value) const
    {
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_recordptrsByVendorId.findRange(&first, &last, value);

        return bsl::pair<ConstItrById, ConstItrById>(ConstItrById(first),
                                                     ConstItrById(last));
    }

    void main4()
    {
if (veryVerbose) puts("Usage Example4");
// ```
// Now, create an empty container and load it with some sample data.
// ```
        MySalesRecordContainer msrc;

        const MySalesRecord DATA[] = {
            { 1000, 100, 10, "hello" },
            { 1001, 100, 20, "world" },
            { 1002, 200, 10, "how" },
            { 1003, 200, 20, "are" },
            { 1004, 100, 10, "you" },
            { 1005, 100, 20, "today" }
        };
        const int numDATA = sizeof DATA / sizeof *DATA;

if (veryVerbose) puts("\nInsert sales records into container.");

        for (int i = 0; i < numDATA; ++i) {
            const int orderNumber   = DATA[i].orderNumber;
            const int customerId    = DATA[i].customerId;
            const int vendorId      = DATA[i].vendorId;
            const char *description = DATA[i].description;

if (veryVerbose) {
    printf("%d: %d %d %s\n", orderNumber, customerId, vendorId, description);
}

            typedef MySalesRecordContainer::ConstItrByOrderNumber MyConstItr;
            typedef bsl::pair<MyConstItr, bool>                   InsertResult;

            const InsertResult status = msrc.insert(DATA[i]);
            ASSERT(msrc.cend() != status.first);
            ASSERT(true        == status.second);
        }
// ```
// We find on standard output:
// ```
//  Insert sales records into container.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
// ```
// We can search our container by order number and find the expected records.
// ```
if (veryVerbose) puts("\nFind sales records by order number.");
        for (int i = 0; i < numDATA; ++i) {
            const int orderNumber   = DATA[i].orderNumber;
            const int customerId    = DATA[i].customerId;
            const int vendorId      = DATA[i].vendorId;
            const char *description = DATA[i].description;

if (veryVerbose) {
     printf("%d: %d %d %s\n", orderNumber, customerId, vendorId, description);
}
            MySalesRecordContainer::ConstItrByOrderNumber itr =
                                           msrc.findByOrderNumber(orderNumber);
            ASSERT(msrc.cend() != itr);
            ASSERT(orderNumber == itr->orderNumber);
            ASSERT(customerId  == itr->customerId);
            ASSERT(vendorId    == itr->vendorId);
            ASSERT(0 == strcmp(description, itr->description));
        }
// ```
// We find on standard output:
// ```
//  Find sales records by order number.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
// ```
// We can search our container by customer identifier and find the expected
// records.
// ```
if (veryVerbose) puts("\nFind sales records by customer identifier.");

        typedef MySalesRecordContainer::ConstItrById MyConstItrById;

        for (int customerId = 100; customerId <= 200; customerId += 100) {
            bsl::pair<MyConstItrById, MyConstItrById> result =
                                             msrc.findByCustomerId(customerId);
            typedef bsl::iterator_traits<
                                    MyConstItrById>::difference_type CountType;
            const CountType count = bsl::distance(result.first, result.second);

if (veryVerbose) printf("customerId %d, count " TD "\n", customerId, count);

            for (MySalesRecordContainer::ConstItrById itr  = result.first,
                                                      end  = result.second;
                                                      end != itr; ++itr) {
if (veryVerbose) {
    printf("\t\t%d %d %d %s\n",
           itr->orderNumber,
           itr->customerId,
           itr->vendorId,
           itr->description);
}
            }
        }
// ```
// We find on standard output:
// ```
//  Find sales records by customer identifier.
//  customerId 100, count 4
//              1005 100 20 today
//              1004 100 10 you
//              1001 100 20 world
//              1000 100 10 hello
//  customerId 200, count 2
//              1003 200 20 are
//              1002 200 10 how
// ```
// Lastly, we can search our container by vendor identifier and find the
// expected records.
// ```
if (veryVerbose) puts("\nFind sales records by vendor identifier.");

        typedef MySalesRecordContainer::ConstItrById MyConstItrById;

        for (int vendorId = 10; vendorId <= 20; vendorId += 10) {
            bsl::pair<MyConstItrById, MyConstItrById> result =
                                                 msrc.findByVendorId(vendorId);
            typedef bsl::iterator_traits<
                                    MyConstItrById>::difference_type CountType;
            const CountType count = bsl::distance(result.first, result.second);

if (veryVerbose) printf("vendorId %d, count " TD "\n", vendorId, count);

            for (MySalesRecordContainer::ConstItrById itr  = result.first,
                                                      end  = result.second;
                                                      end != itr; ++itr) {
if (veryVerbose) {
    printf("\t\t%d %d %d %s\n",
            (*itr).orderNumber,
            (*itr).customerId,
            (*itr).vendorId,
            (*itr).description);
}
            }
        }
// ```
// We find on standard output:
// ```
//  Find sales records by vendor identifier.
//  vendorId 10, count 3
//              1004 100 10 you
//              1002 200 10 how
//              1000 100 10 hello
//  vendorId 20, count 3
//              1005 100 20 today
//              1003 200 20 are
//              1001 100 20 world
// ```
    }
}  // close namespace UsageExamples

// Type trait specializations to suppress warnings on allocator usage.  We omit
// these details from the actual usage examples as the necessary opening and
// closing of multiple namespaces would distract from the narrative.

namespace BloombergLP {
namespace bslma {

template <class KEY, class VALUE, class HASHF, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedMap<KEY,
                                                     VALUE,
                                                     HASHF,
                                                     EQUAL,
                                                     ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type {};

template <class KEY, class VALUE, class HASHF, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedMultiMap<KEY,
                                                          VALUE,
                                                          HASHF,
                                                          EQUAL,
                                                          ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type {};

template <class KEY, class HASHF, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedSet<KEY,
                                                     HASHF,
                                                     EQUAL,
                                                     ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type {};

template <>
struct UsesBslmaAllocator<UsageExamples::MySalesRecordContainer>
: bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace
//@bdetdsplit FOR 14..17, 25 END

// BDE_VERIFY pragma: pop  // Suppress idiomatic issues with usage examples

//=============================================================================
//                            STATIC ASSERTIONS
//-----------------------------------------------------------------------------

//@bdetdsplit FOR 24 BEGIN
namespace static_assertions {

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                         bsl::hash<int>,
                                         bsl::equal_to<int>,
                                         bsl::allocator<int> > TestAsTrue;

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                         bsl::hash<int>,
                                         bsl::equal_to<int>,
                                         bsltf::StdTestAllocator<int> >
                                                                   TaseAsFalse;

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                         bsl::hash<int>,
                                         bsl::equal_to<int>,
                                         bsltf::StdStatefulAllocator<int,
                                                                     true,
                                                                     false,
                                                                     false,
                                                                     false> >
                                                                  TestAsFalse2;

BSLMF_ASSERT( bslma::UsesBslmaAllocator<TestAsTrue>::value);
BSLMF_ASSERT(!bslma::UsesBslmaAllocator<TaseAsFalse>::value);
BSLMF_ASSERT(!bslma::UsesBslmaAllocator<TestAsFalse2>::value);

}  // close namespace static_assertions
//@bdetdsplit FOR 24 END

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Set this test allocator globally, up front, to avoid multiple static
    // allocator references on AIX.  This allocator should always be swapped
    // out by an allocator guard during any specific test case.

    bslma::TestAllocator bsltfAllocator("bsltf-default", veryVeryVeryVerbose);
    g_bsltfAllocator_p = &bsltfAllocator;
    bsltf::StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                                           g_bsltfAllocator_p);

    ASSERT(bsltf::StdTestAllocatorConfiguration::delegateAllocator() ==
                                                           g_bsltfAllocator_p);

    switch (test) { case 0:
      case 26: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        UsageExamples::main1();
        UsageExamples::main2();
        UsageExamples::main3();
        UsageExamples::main4();
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        // 1. `unordered_map` does not have a transparent set of lookup
        //    functions if the comparator is not transparent.
        //
        // 2. `unordered_map` has a transparent set of lookup functions if the
        //    comparator is transparent.
        //
        // Plan:
        // 1. Construct a non-transparent map and call the lookup functions
        //    with a type that is convertible to the `value_type`.  There
        //    should be exactly one conversion per call to a lookup function.
        //    (C-1)
        //
        // 2. Construct a transparent map and call the lookup functions with a
        //    type that is convertible to the `value_type`.  There should be no
        //    conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: `find`              handles transparent comparators.
        //   CONCERN: `findRance`         handles transparent comparators.
        //   CONCERN: `bucketIndexForKey` handles transparent comparators.
        //   CONCERN: `operator []`       handles transparent comparators.
        //   CONCERN: `tryEmplace`        handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        if (veryVerbose) printf("Testing set-like containers\n");
        {
            typedef SetKeyConfiguration<int> SetConfig;
            typedef bslstl::HashTable<SetConfig,
                        TransparentHasher,
                        TransparentComparator
            >    TransparentSet;

            typedef bslstl::HashTable<SetConfig,
                        bsl::hash<int>,
                        TransparentComparator
            >    NonTransparentSet;

            const int DATA[] = { 0, 1, 2, 3, 4 };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            NonTransparentSet        mSet;
            const NonTransparentSet &cSet = mSet;
            TransparentSet           mXSet;
            const TransparentSet    &cXSet = mXSet;

            for (int i = 0; i < NUM_DATA; ++i) {
                if (veryVeryVeryVerbose) {
                    printf("Constructing test data\n");
                }
                const TransparentSet::ValueType VALUE(DATA[i]);
                mSet.insert(VALUE);
                mXSet.insert(VALUE);
            }

            ASSERT(NUM_DATA == cSet.size());
            ASSERT(NUM_DATA == cXSet.size());

            for (int i = 0; i < NUM_DATA; ++i) {
                const int KEY = DATA[i];
                if (veryVerbose) {
                    printf("Testing transparent comparators with a key of %d\n",
                           KEY);
                }

                if (veryVerbose) {
                    printf("\tTesting const non-transparent map.\n");
                }
                testSetTransparentComparator( cSet, false, KEY);

                if (veryVerbose) {
                    printf("\tTesting mutable non-transparent map.\n");
                }
                testSetTransparentComparator       (mSet, false, KEY);
                testSetTransparentComparatorMutable(mSet, false, KEY);

                if (veryVerbose) {
                    printf("\tTesting const transparent map.\n");
                }
                testSetTransparentComparator(cXSet,  true,  KEY);

                if (veryVerbose) {
                    printf("\tTesting mutable transparent map.\n");
                }
                testSetTransparentComparator       (mXSet, true, KEY);
                testSetTransparentComparatorMutable(mXSet, true, KEY);
            }
        }

        if (veryVerbose) printf("Testing map-like containers\n");
        {
            typedef MapKeyConfiguration<const int, bsl::pair<const int, int> > MapConfig;
            typedef bslstl::HashTable<MapConfig,
                        TransparentHasher,
                        TransparentComparator
            >    TransparentMap;

            typedef bslstl::HashTable<MapConfig,
                        bsl::hash<int>,
                        TransparentComparator
            >    NonTransparentMap;

            const int DATA[] = { 0, 1, 2, 3, 4 };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            NonTransparentMap        mMap;
            const NonTransparentMap &cMap = mMap;
            TransparentMap           mXMap;
            const TransparentMap    &cXMap = mXMap;

            for (int i = 0; i < NUM_DATA; ++i) {
                if (veryVeryVeryVerbose) {
                    printf("Constructing test data\n");
                }
                const TransparentMap::ValueType VALUE(DATA[i], DATA[i]);
                mMap.insert(VALUE);
                mXMap.insert(VALUE);
            }

            ASSERT(NUM_DATA == cMap.size());
            ASSERT(NUM_DATA == cXMap.size());

            for (int i = 0; i < NUM_DATA; ++i) {
                const int KEY = DATA[i];
                if (veryVerbose) {
                    printf("Testing transparent comparators with a key of %d\n",
                           KEY);
                }

                if (veryVerbose) {
                    printf("\tTesting const non-transparent map.\n");
                }
                testMapTransparentComparator( cMap, false, KEY);

                if (veryVerbose) {
                    printf("\tTesting mutable non-transparent map.\n");
                }
                testMapTransparentComparator       (mMap, false, KEY);
                testMapTransparentComparatorMutable(mMap, false, KEY);

                if (veryVerbose) {
                    printf("\tTesting const transparent map.\n");
                }
                testMapTransparentComparator(cXMap,  true,  KEY);

                if (veryVerbose) {
                    printf("\tTesting mutable transparent map.\n");
                }
                testMapTransparentComparator       (mXMap, true, KEY);
                testMapTransparentComparatorMutable(mXMap, true, KEY);
            }
        }

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TRAITS AND OTHER COMPILE-TIME PROPERTIES
        // --------------------------------------------------------------------
        // For this test, the standard "minimal" set of test types provides
        // adequate coverage for the different properties dependent on the
        // value type.  We must simply test sufficient pre-packaged
        // configurations to cover the possibilities of the hash functor,
        // comparator and allocator too.  As of this writing, there is no
        // package that separately isolates bitwise copyable hasher and
        // comparator, to test as separate dimensions.
        // --------------------------------------------------------------------
        if (verbose) puts("\nTRAITS AND OTHER COMPILE-TIME PROPERTIES"
                          "\n========================================");

        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(traitsAndCompileTime);

#define u_RUN_HARNESS(HarnessTemplateName)                     \
            u_RUN_HARNESS_(HarnessTemplateName,                 \
                           traitsAndCompileTime,                 \
                           u_TEST_TYPES_ALL)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);

#ifdef BSLS_PLATFORM_CMP_SUN
        // Compiling these with SUN Studio slows down compilation significantly
        // (60+ seconds), but other compilers do it all under 15 seconds.
        //
        // TBD: We should go over which types and configuration combinations
        //      are necessary to cover the traits and the other compile-time
        //      concerns tested here, so we can perhaps not have this Solaris
        //      exception and test the same (fewer) types on all platforms.
    #define u_TESTED_TYPES_MINIMAL signed char,                               \
                                   bsltf::TemplateTestFacility::MethodPtr,    \
                                   bsltf::AllocBitwiseMoveableTestType,       \
                                   bsltf::MovableAllocTestType,               \
                                   TestTypes::MostEvilTestType
    #undef  u_RUN_HARNESS
    #define u_RUN_HARNESS(HarnessTemplateName)                     \
                u_RUN_HARNESS_(HarnessTemplateName,                 \
                               traitsAndCompileTime,                 \
                               u_TESTED_TYPES_MINIMAL)
#endif  // SUN Studio Compiler

        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);
#undef  u_RUN_HARNESS
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // PUBLIC TYPES
        // --------------------------------------------------------------------
        if (verbose) puts("\nPUBLIC TYPES"
                          "\n============");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // `maxSize`, `maxNumBuckets` METHODS
        // --------------------------------------------------------------------
        if (verbose) puts("\n'maxSize', `maxNumBuckets` METHODS"
                          "\n==================================");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // `findRange` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'findRange' METHOD"
                          "\n==================");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // `find` MENTHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'find' METHOD"
                          "\n=============");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // `insertIfMissing(const KeyType& key)` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'insertIfMissing(const KeyType& key)' METHOD"
                          "\n============================================");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // `insertIfMissing(bool *, VALUE)` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'insertIfMissing(bool *, VALUE)' METHOD"
                          "\n=======================================");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        // --------------------------------------------------------------------
        if (verbose) puts("\nDEFAULT CONSTRUCTOR"
                          "\n===================");

    //@bdetdsplit INTO FIRST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase17);
        TestCases<AwkwardMaplikeForDefaultParams>::testCase17();
    //@bdetdsplit INTO FIRST SLICE END

    // Note that `NonEqualComparableTestType` is not applicable in this test
    // case because the equality operator is used in test assertions.

    //@bdetdsplit SLICING TYPELIST / 2
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType
    // The above is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but with the
    // type `bsltf::NonEqualComparableTestType` removed.

#define u_RUN_HARNESS(HarnessTemplateName)                       \
    u_RUN_HARNESS_(HarnessTemplateName, testCase17, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);

        // This is the use case we are most concerned about.
        // TBD: We probably want to test with a smattering of the following
        //      concerns as well, notably with the different allocator
        //      patterns.
        u_RUN_HARNESS(TestCases_DefaultOnlyFunctors);

        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
    #if 0
        // These configurations are not available, as these functors are
        // not default constructible.
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);

        // Function pointers do not give usable defaults for functors.
        u_RUN_HARNESS(TestCases_FunctionPointers);
    #endif  // if 0
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
    #if 0
        // The stateless allocator flags issues installing the chosen allocator
        // when it is not the default.
        // TBD: What does this mean?  Why does the reader have to know that
        //      while look at the code here?
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
    #endif  // if 0
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);

#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // `setMaxLoadFactor` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'setMaxLoadFactor' METHOD"
                          "\n=========================");

    //@bdetdsplit INTO LAST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase16);
    //@bdetdsplit INTO LAST SLICE END

    // Note that `NonEqualComparableTestType` is not applicable in this test
    // case because the equality operator is used in test assertions.

    //@bdetdsplit SLICING TYPELIST / 2
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType
    // The above is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but with the
    // type `bsltf::NonEqualComparableTestType` removed.

#define u_RUN_HARNESS(HarnessTemplateName)                       \
    u_RUN_HARNESS_(HarnessTemplateName, testCase16, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);

        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
    #if 0
        // Fails test with all input types, 36 memory leaks (6 per type) and 6
        // other test failures saying:
        // ```
        //      currentTestAllocator:   <addr>,
        //      parameterTestAllocator: <addr>,
        //      currentTestAllocator->name():   "bsltf-default",
        //      parameterTestAllocator->name(): "scratch"
        //  Error: currentTestAllocator == parameterTestAllocator (failed)
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
    #endif  // if 0
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);

#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // `insert` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'insert' METHOD"
                          "\n===============");

    //@bdetdsplit INTO LAST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase15);
    //@bdetdsplit INTO LAST SLICE END

#define u_RUN_HARNESS_WITH(HarnessTemplateName, ...)           \
    u_RUN_HARNESS_(HarnessTemplateName, testCase15, __VA_ARGS__)

    // Note that `NonEqualComparableTestType` is not applicable in this test
    // case because the equality operator is used in test assertions.

//@bdetdsplit CODE SLICING BEGIN
    //@bdetdsplit SLICING TYPELIST / 4
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType,                               \
        TestTypes::MostEvilTestType
    // The above is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL`, with the type
    // `bsltf::NonEqualComparableTestType` removed, and the type
    // `TestTypes::MostEvilTestType` added.

#define u_RUN_HARNESS(HarnessTemplateName)                       \
            u_RUN_HARNESS_WITH(HarnessTemplateName, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
    #if 0
        // Test failure `ITER[tj] == RESULT->nextLink()` with all tested types.
        // TBD: Is this expected?
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
    #endif
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);

#if 0   // Does not compile due to test data being `const`
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
#endif
#if 0  // TBD: Why is this test config commented out?
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
#endif
#if 0   // Does not compile
        // TBD: Is this a problem with the test, or is this expected?
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#endif
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES

//@bdetdsplit CODE SLICING BREAK
    // Note that `u_TESTED_TYPES_NOALLOC` is a sub-set of tests that do not yet
    // want to support testing the allocatable types, as non-BDE allocators do
    // not scope the object allocator to the elements, and so use the default
    // allocator instead, which confuses the math of the test case.
    #define u_TESTED_TYPES_NOALLOC                                            \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,                  \
            bsltf::EnumeratedTestType::Enum,                                  \
            bsltf::UnionTestType,                                             \
            bsltf::SimpleTestType,                                            \
            bsltf::BitwiseMoveableTestType,                                   \
            bsltf::NonTypicalOverloadsTestType,                               \
            bsltf::NonTypicalOverloadsTestType,                               \
            bsltf::NonDefaultConstructibleTestType
    // `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL`, but with the types
    // `bsltf::AllocTestType`, `bsltf::BitwiseCopyableTestType`,
    // `bsltf::AllocBitwiseMoveableTestType`, `bsltf::MovableTestType`,
    // `bsltf::MovableAllocTestType`, `bsltf::NonEqualComparableTestType`
    // removed.

        // This has an assertion failure with types that allocate:
        // bslma_exceptionguard.h:142 Assertion failed: d_copy == *d_object_p
        // TBD: Is this expected?  The above long explanation seems to address
        //      something completely different.
        u_RUN_HARNESS_WITH(TestCases_ConvertibleValueConfiguration,
                           u_TESTED_TYPES_NOALLOC);
#undef u_TESTED_TYPES_NOALLOC
//@bdetdsplit CODE SLICING END

#undef u_RUN_HARNESS_WITH
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // `reserveForNumElements` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'reserveForNumElements' METHOD"
                          "\n==============================");

        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase14);

#define u_RUN_HARNESS(HarnessTemplateName)                         \
    u_RUN_HARNESS_(HarnessTemplateName, testCase14, u_TEST_TYPES_ALL)

        // These probably should be RUN separately for run-time, but compile
        // all-together fast so we do not split them
        u_RUN_HARNESS(TestCases_BasicConfiguration);
#undef u_RUN_HARNESS
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // `rehashForNumBuckets` METHOD
        // --------------------------------------------------------------------
        if (verbose) puts("\n'rehashForNumBuckets' METHOD"
                          "\n============================");

    //@bdetdsplit INTO FIRST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase13);
    //@bdetdsplit INTO FIRST SLICE END

        // Note that `NonEqualComparableTestType` is not applicable to this
        // test case.

    //@bdetdsplit SLICING TYPELIST / 3
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType
    // The above is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but with the
    // type `bsltf::NonEqualComparableTestType` removed.

#define u_RUN_HARNESS(HarnessTemplateName)                       \
    u_RUN_HARNESS_(HarnessTemplateName, testCase13, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);

        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_FunctionPointers);

        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);

#if 0  // TBD: Why is this test config commented out?
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
#endif
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        // --------------------------------------------------------------------
        if (verbose) puts("\nBSLX STREAMING"
                          "\n==============");

        if (verbose) puts("\nThe component doesn't support `bslx` streaming.");
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------
        if (verbose) puts("\nMOVE ASSIGNMENT OPERATOR"
                          "\n========================");

        if (verbose) puts("\nTest case not yet implemented -- TBD");
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------
        if (verbose) puts("\nCOPY ASSIGNMENT OPERATOR"
                          "\n========================");

    //@bdetdsplit INTO FIRST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase10);
    //@bdetdsplit INTO FIRST SLICE END

        // Note that `NonEqualComparableTestType` is not applicable in this
        // test case because the equality operator is used in test assertions.

#define u_RUN_HARNESS_WITH(HarnessTemplateName, ...)          \
    u_RUN_HARNESS_(HarnessTemplateName, testCase10, __VA_ARGS__)

//@bdetdsplit CODE SLICING BEGIN
    //@bdetdsplit SLICING TYPELIST / 5

    /// The type-list belowis `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL`, but
    /// with the type `bsltf::NonEqualComparableTestType` removed, and the
    /// type `TestTypes::MostEvilTestType` added.
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType,                               \
        TestTypes::MostEvilTestType

#define u_RUN_HARNESS(HarnessTemplateName)               \
    u_RUN_HARNESS_WITH(HarnessTemplateName, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
    #if 0
        u_RUN_HARNESS_WITH(TestCases_DegenerateConfigurationWithNoSwap,
                           BSLSTL_HASHTABLE_TESTCASE_COPY_ASSIGN_TYPES,
                           bsltf::AllocTestType,
                           bsltf::AllocBitwiseMoveableTestType);
        // Degenerate functors are not CopyAssignable, and rely on the
        // copy/swap idiom for the copy-assignment operator to function.
    #endif
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);

        // The non-BDE allocators do not propagate the container allocator to
        // their elements, and so will make use of the default allocator when
        // making copies.  Therefore, we use a slightly different list of types
        // when testing with these allocators.  This leaves unaddressed the
        // issue of testing `HashTable` with these allocator types, and
        // elements that in turn allocate their own memory, using their own
        // allocator.
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES

//@bdetdsplit CODE SLICING BREAK
        // Note that `u_TESTED_TYPES_NOALLOC` is a sub-set of tests that do not
        // yet want to support testing the allocatable types, as non-BDE
        // allocators do not scope the object allocator to the elements, and so
        // use the default allocator instead, which confuses the math of the
        // test case.

/// `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL`, but with the types
/// `bsltf::AllocTestType`, `bsltf::BitwiseCopyableTestType`,
/// `bsltf::AllocBitwiseMoveableTestType`, `bsltf::MovableTestType`,
/// `bsltf::MovableAllocTestType`, `bsltf::NonEqualComparableTestType`
/// removed.
#define u_TESTED_TYPES_NOALLOC                                                \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,                  \
            bsltf::EnumeratedTestType::Enum,                                  \
            bsltf::UnionTestType,                                             \
            bsltf::SimpleTestType,                                            \
            bsltf::BitwiseMoveableTestType,                                   \
            bsltf::NonTypicalOverloadsTestType,                               \
            bsltf::NonTypicalOverloadsTestType,                               \
            bsltf::NonDefaultConstructibleTestType
#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying to
        // separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        u_RUN_HARNESS_WITH(TestCases_StdAllocatorConfiguration,
                           u_TESTED_TYPES_NOALLOC);
#endif
        u_RUN_HARNESS_WITH(TestCases_StatefulAllocatorConfiguration,
                           u_TESTED_TYPES_NOALLOC);
#undef u_TESTED_TYPES_NOALLOC
//@bdetdsplit CODE SLICING END

    //@bdetdsplit INTO LAST SLICE BEGIN
        u_RUN_HARNESS_WITH(TestCases_ConvertibleValueConfiguration,
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
            bsltf::NonAssignableTestType,
            bsltf::NonDefaultConstructibleTestType);
        // The above must be non-allocating types.
        // TBD: Why must be non-allocating? Why these types and not the types
        //      from `u_TESTED_TYPES_NOALLOC`?
    //@bdetdsplit INTO LAST SLICE END
#undef u_RUN_HARNESS_WITH
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION `swap`
        // --------------------------------------------------------------------
        if (verbose) puts("\nMANIPULATOR AND FREE FUNCTION `swap`"
                          "\n====================================");

    //@bdetdsplit INTO FIRST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase9);
    //@bdetdsplit INTO FIRST SLICE END

        // Note that `NonEqualComparableTestType` is not applicable in this
        // test case because the equality operator is used in test assertions.

    //@bdetdsplit SLICING TYPELIST / 4

    /// The type-list below is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but
    /// with the type `bsltf::NonEqualComparableTestType` removed.
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType

#define u_RUN_HARNESS(HarnessTemplateName)                      \
    u_RUN_HARNESS_(HarnessTemplateName, testCase9, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);

#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying to
        // separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
#endif  // if 0
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        // --------------------------------------------------------------------
        if (verbose) puts("\nMOVE CONSTRUCTORS"
                          "\n=================");

        if (verbose) puts("\nTBD - Test case not yet implemented");
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------
        if (verbose) puts("\nCOPY CONSTRUCTORS"
                          "\n=================");

    //@bdetdsplit INTO LAST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase7);
    //@bdetdsplit INTO LAST SLICE END

        // Note that `NonEqualComparableTestType` is not applicable in this
        // test case because the equality operator is used in test assertions.

    //@bdetdsplit SLICING TYPELIST / 4

    /// The type-list below is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but
    /// with the type `bsltf::NonEqualComparableTestType` removed.
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType

#define u_RUN_HARNESS(HarnessTemplateName)                      \
    u_RUN_HARNESS_(HarnessTemplateName, testCase7, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);

#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying to
        // separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
#endif  // if 0
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);

#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        // --------------------------------------------------------------------
        if (verbose) puts("\nEQUALITY-COMPARISON OPERATORS"
                          "\n=============================");

    //@bdetdsplit INTO LAST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase6);
    //@bdetdsplit INTO LAST SLICE END

        // Note that `NonEqualComparableTestType` is not applicable in this
        // test case.

    //@bdetdsplit SLICING TYPELIST / 3

    /// The type-list below is `BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL` but
    /// with the type `bsltf::NonEqualComparableTestType` removed.
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,                        \
        bsltf::NonAssignableTestType,                                         \
        bsltf::NonDefaultConstructibleTestType

#define u_RUN_HARNESS(HarnessTemplateName)                      \
    u_RUN_HARNESS_(HarnessTemplateName, testCase6, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
  #if 0  // This always causes trouble
       // TBD: What trouble?  On what platforms?  Is that expected?  A bug?
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
  #endif
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------
        if (verbose) puts("\nOUTPUT (<<) OPERATOR"
                          "\n====================");

        if (verbose) puts("\nThere is no output operator for this component");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------
        if (verbose) puts("\nBASIC ACCESSORS"
                          "\n===============");

    //@bdetdsplit INTO FIRST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase4);
    //@bdetdsplit INTO FIRST SLICE END

#define u_RUN_HARNESS_WITH(HarnessTemplateName, ...)         \
    u_RUN_HARNESS_(HarnessTemplateName, testCase4, __VA_ARGS__)

//@bdetdsplit CODE SLICING BEGIN
    //@bdetdsplit SLICING TYPELIST / 3
    #define u_TESTED_TYPES BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

#define u_RUN_HARNESS(HarnessTemplateName) \
        u_RUN_HARNESS_WITH(HarnessTemplateName, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
  #if 0
    // This test mostly seems to work, but the public interface for the
    // accessor `bucketForKey` is not compatible with these functors, and it
    // would take non-trivial amount of effort to factor out a test that does
    // not compile that signature for just this configuration.  Alternatively,
    // we might want to redesign the public API, but there is no real desire to
    // support these functions/functors with awkward signatures any more than
    // is strictly necessary for standard conformance.
    //
    // TBD: Should this comment be in `TestCases_ModifiableFunctors`, and we
    //      just refer to it here?  Without seeing the actual implementation of
    //      `TestCases_ModifiableFunctors<...>::testCase4(...)` the above text
    //      has not enough context to understand anything.
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
  #endif  // if 0
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES

//@bdetdsplit CODE SLICING BREAK
    #define u_TESTED_TYPES_NOALLOC                                            \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD,                    \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE

        // Types with BDE allocators make temporaries with the default
        // allocator, and that is not yet accounted for in the arithmetic of
        // this test case.  So we use only those types that do not allocate.
        u_RUN_HARNESS_WITH(TestCases_ConvertibleValueConfiguration,
                           u_TESTED_TYPES_NOALLOC);
#undef u_TESTED_TYPES_NOALLOC
//@bdetdsplit CODE SLICING END

#undef u_RUN_HARNESS_WITH
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //
        //   This test case verifies `gg` and `ggg` and other test machinery.
        // --------------------------------------------------------------------
        if (verbose) puts("\nTEST MACHINERY"
                          "\n==============");

    //@bdetdsplit INTO LAST SLICE BEGIN
        // Non-template test configuration
        u_RUN_AWKWARD_MAP_LIKE(testCase3);
    //@bdetdsplit INTO LAST SLICE END

#define u_RUN_HARNESS_WITH(HarnessTemplateName, ...)         \
    u_RUN_HARNESS_(HarnessTemplateName, testCase3, __VA_ARGS__)

//@bdetdsplit CODE SLICING BEGIN
    //@bdetdsplit SLICING TYPELIST / 3
    #define u_TESTED_TYPES BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

#define u_RUN_HARNESS(HarnessTemplateName)               \
    u_RUN_HARNESS_WITH(HarnessTemplateName, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#undef u_RUN_HARNESS
#undef u_TESTED_TYPES

//@bdetdsplit CODE SLICING BREAK
    #define u_TESTED_TYPES_NOALLOC                                            \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,                  \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD

        u_RUN_HARNESS_WITH(TestCases_ConvertibleValueConfiguration,
                           u_TESTED_TYPES_NOALLOC);
        // Types with BDE allocators make temporaries with the default
        // allocator, and that is not yet accounted for in the arithmetic of
        // this test case.  So we use only those types that do not allocate.
#undef u_TESTED_TYPES_NOALLOC
//@bdetdsplit CODE SLICING END

#undef u_RUN_HARNESS_WITH
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMITIVE CONSTRUCTORS AND BASIC ACCESSORS
        //
        //   This case is implemented as the `testCase2` method of several
        //   templated test harnesses (that are reused in other test cases)
        //   that address our high level concerns, listed below, about the
        //   different choices of template parameters `bslstl::HashTable` is
        //   supposed to support.  The runtime concerns and the details of the
        //   test plan are documented in those templated test harnesses.
        //
        //   Note that while later test cases may be able to place additional
        //   requirements on the template parameters they operate with, this
        //   bootstrap case has to cover the widest variety of parameterized
        //   concerns because to validate bringing any valid container into any
        //   valid state for any of the later cases.
        //
        //   Also note that testing of the last two, allocator specific
        //   concerns are deferred for full `allocator_traits` support test
        //   case as they are orthogonal.
        //
        // Concerns:
        //  1. The class bootstraps with the default template arguments for the
        //     unordered containers as policy parameters
        //
        //  2. The class supports a wide variety of troublesome element types,
        //     as covered extensively in the template test facility, `bsltf`.
        //
        //  3. STL allocators that are not (BDE) polymorphic, and that never
        //     propagate on any operations, just like BDE
        //
        //  4. STL allocators that are not (BDE) polymorphic, and propagate on
        //     all possible operations.
        //
        //  5. functors that do not const-qualify `operator()`
        //
        //  6. stateful functors
        //
        //  7. function pointers as functors
        //
        //  8. non-default-constructible functors
        //
        //  9. functors overloading `operator&`
        //
        // 10. functors overloading `operator,`
        //
        // 11. functors that cannot be swapped (`swap` is required to support
        //     assignment, not bootstrap, default constructor, or most methods)
        //
        // 12. functors whose argument(s) are convertible-form the key-type
        //
        // 13. functors with templated function-call operators
        //
        // 14. support for simple set-like policy
        //
        // 15. support for basic map-like policy, where key is a sub-state of
        //     the element's value
        //
        // 16. support for a minimal key type, with equality-comparable element
        //     type
        //
        // 17. support for comparison functors returning an evil boolean-like
        //      type
        //
        // 18. support for STL allocators returning smart pointers
        //
        // 19. that the STL allocator functions are called if the STL allocator
        //     supplies them (rather than always using a default incantation in
        //     allocator_traits, for example).
        //
        // Plan:
        // 1. Run the test harnesses in a variety of configurations that each,
        //    in turn, address the concerns and verify that the behavior is as
        //    expected.
        //
        // Testing:
        //   BOOTSTRAP
        //   HashTable(HASHER, COMPARATOR, SizeType, float, ALLOC)
        //   ~HashTable();
        //   void removeAll();
        //   insertElement  (test driver function, proxy for basic manipulator)
        // --------------------------------------------------------------------
        if (verbose) puts("\nBOOTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS"
                          "\n==============================================");

        u_RUN_AWKWARD_MAP_LIKE(testCase2);  //@bdetdsplit INTO FIRST SLICE

    //@bdetdsplit SLICING TYPELIST / 7
    #define u_TESTED_TYPES                                                    \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

#define u_RUN_HARNESS(HarnessTemplateName) \
                 u_RUN_HARNESS_(HarnessTemplateName, testCase2, u_TESTED_TYPES)

        u_RUN_HARNESS(TestCases_BasicConfiguration);
        u_RUN_HARNESS(TestCases_BsltfConfiguration);
        u_RUN_HARNESS(TestCases_StatefulConfiguration);
        u_RUN_HARNESS(TestCases_GroupedUniqueKeys);
        u_RUN_HARNESS(TestCases_GroupedSharedKeys);
        u_RUN_HARNESS(TestCases_DegenerateConfiguration);
        u_RUN_HARNESS(TestCases_DegenerateConfigurationWithNoSwap);
        u_RUN_HARNESS(TestCases_ConstFunctors);
        u_RUN_HARNESS(TestCases_GenericFunctors);
        u_RUN_HARNESS(TestCases_ConvertibleValueConfiguration);
        u_RUN_HARNESS(TestCases_ModifiableFunctors);
#ifndef u_NO_REFERENCE_COLLAPSING
        u_RUN_HARNESS(TestCases_FunctorReferences);
        u_RUN_HARNESS(TestCases_FunctionReferences);
#endif
        u_RUN_HARNESS(TestCases_FunctionTypes);
        u_RUN_HARNESS(TestCases_FunctionPointers);
        u_RUN_HARNESS(TestCases_StdAllocatorConfiguration);
        u_RUN_HARNESS(TestCases_StatefulAllocatorConfiguration);
#undef  u_RUN_HARNESS
#undef  u_TESTED_TYPES
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Execute each method to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        typedef BasicKeyConfig<int> KeyConfig;

        typedef bslstl::HashTable<BasicKeyConfig<int>,
                                  bsl::hash<int>,
                                  bsl::equal_to<int> > Obj;

        // `intValues` is non-`const` so we can permute it later
        int values[] = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
        const size_t NUM_VALUES = sizeof values / sizeof *values;

        const bsl::hash<int> HASHER = bsl::hash<int>();
        const bsl::equal_to<int> COMPARATOR = bsl::equal_to<int>();

        bslma::TestAllocator defaultAllocator("defaultAllocator",
                                              veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

        bslma::TestAllocator objectAllocator("objectAllocator",
                                             veryVeryVeryVerbose);

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) puts("Construct an empty HashTable");
        {
            bslma::TestAllocator dummyAllocator("dummyAllocator",
                                                veryVeryVeryVerbose);

            // Note that `HashTable` does not have a default constructor, so we
            // must explicitly supply a default for each attribute.
            Obj mX(HASHER, COMPARATOR, 0, 1.0f, &dummyAllocator);
            const Obj& X = mX;
            ASSERTV(0    == X.size());
            ASSERTV(0    <  X.maxSize());
            ASSERTV(0    == defaultAllocator.numBytesInUse());
            ASSERTV(0    == dummyAllocator.numBytesInUse());

            if (veryVeryVerbose) puts("Call *all* `const` functions.");
            // As a simple compile-check of the template, call every `const`
            // function member, unless they require a non-empty container.
            bsl::hash<int>     h = X.hasher();      (void)h;
            bsl::equal_to<int> c = X.comparator();  (void)c;
            bsl::allocator<int>  a = X.allocator();
            ASSERTV(&dummyAllocator == a.mechanism());

            ASSERTV(0 == X.loadFactor());
            ASSERTV(1.0f == X.maxLoadFactor());

            (void)X.bucketIndexForKey(42);

            bslalg::BidirectionalLink *first, *last;
            X.findRange(&first, &last, 42);
            ASSERTV(0 == first);
            ASSERTV(0 == last);

            first = X.elementListRoot();
            last  = X.find(42);
            ASSERTV(0 == first);
            ASSERTV(0 == last);

            ASSERTV(X.numBuckets() < X.maxNumBuckets());

            bslalg::HashTableBucket bkt = X.bucketAtIndex(0); (void)bkt;
            ASSERTV(0 == X.countElementsInBucket(0));

            ASSERTV(X == X);
            ASSERTV(!(X != X));

            if (veryVeryVerbose) puts("Call *all* modifier functions");
            // As a simple compile-check of the template, call every remaining
            // function member.
            swap(mX, mX);
            ASSERTV(mX == X);
            ASSERTV(!(X != mX));

            bslalg::BidirectionalLink *newLink = mX.insert(values[0]);
            newLink = X.findEndOfRange(newLink);// last `const` method to check
            // TBD: What does this mean?  What's checked here exactly?  There
            //      is no `ASSERT`, and `newLink` appears to be overwritten
            //      immediately on the next line w/o its value ever used.
            newLink = mX.insert(values[0], X.elementListRoot());
            mX = X;
            mX.swap(mX);
            ASSERTV(mX == X);
            ASSERTV(!(X != mX));

            bool missing;
            newLink = mX.insertIfMissing(&missing, values[0]);
            ASSERTV(!missing);
            ASSERTV(X.elementListRoot() == newLink);

            const bsltf::ConvertibleValueWrapper<int> val(values[0]);
            newLink = mX.insertIfMissing(&missing, val);
            ASSERTV(!missing);
            ASSERTV(X.elementListRoot() == newLink);

           // This makes sense only if `Value` is a `pair`.
           // TBD: Why is it so?  If so, why do we have it here in a BREATHING
           //      test (and then commented out) when a BREATHING test is not
            //      supposed to be a complete test?
           // (void)mX.insertIfMissing(K);

            newLink = mX.remove(newLink);
            mX.removeAll();

            mX.rehashForNumBuckets(0);
            mX.reserveForNumElements(0);

#ifdef BDE_BUILD_TARGET_EXC
            // The call to `setMaxLoadFactor` may try to allocate a lot of
            // memory and is known to throw `bad_alloc` exceptions on AIX test
            // runners.
            //
            // TBD: If only throws on AIX, why do we "swallow" the exception
            //      without reporting a failure on other operating systems
            //      where we expect the test to work?  Also, *what* does this
            //      test?  There is no `ASSERT` of any kind.
            try {
                mX.setMaxLoadFactor(9e-9f);
            }
            catch(const std::exception& e) {
                if (veryVeryVerbose) {
                    printf("exception: `%s`\n", e.what());
                }
            }
#endif  // BDE_BUILD_TARGET_EXC
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) puts("Test use of allocators");
        {
            bslma::TestAllocator objectAllocator1("objectAllocator1",
                                                  veryVeryVeryVerbose);
            bslma::TestAllocator objectAllocator2("objectAllocator2",
                                                  veryVeryVeryVerbose);

            Obj o1(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
            const Obj& O1 = o1;
            ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

            for (size_t i = 0; i != NUM_VALUES; ++i) {
                o1.insert(values[i]);
            }
            ASSERTV(NUM_VALUES == O1.size());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());
            ASSERTV(0 == objectAllocator2.numBytesInUse());
        }

        {
            bslma::TestAllocator objectAllocator1("objectAllocator1",
                                                  veryVeryVeryVerbose);
            bslma::TestAllocator objectAllocator2("objectAllocator2",
                                                  veryVeryVeryVerbose);

            Obj o1(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
            const Obj& O1 = o1;
            ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

            for (size_t i = 0; i != NUM_VALUES; ++i) {
                bool isInsertedFlag = false;
                o1.insertIfMissing(&isInsertedFlag, values[i]);
                ASSERTV(isInsertedFlag, true == isInsertedFlag);
            }
            ASSERTV(NUM_VALUES == O1.size());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());
            ASSERTV(0 == objectAllocator2.numBytesInUse());

            // Copied code from below, under evaluation
                // TBD: what does the above comment mean?
            if (veryVerbose) puts("Use a different allocator");
                // TBD: what does the above mean? Different from what? Use the
                //      allocator to do what?
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

            if (veryVerbose) puts("Copy construct O2(O1)");

            Obj o2(O1, &objectAllocator1); const Obj& O2 = o2;

            ASSERTV(&objectAllocator1 == O2.allocator().mechanism());

            ASSERTV(NUM_VALUES == O1.size());
            ASSERTV(NUM_VALUES == O2.size());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());

            if (veryVerbose) puts("Default construct O3 and swap with O1");
                // TBD: This seems to me at this point to be quite a lot,
                //      quite comprehensive, and quite cryptic for a breathing
                //      test.
            Obj o3(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
            const Obj& O3 = o3;
            ASSERTV(&objectAllocator1 == O3.allocator().mechanism());

            ASSERTV(NUM_VALUES == O1.size());
            ASSERTV(NUM_VALUES == O2.size());
            ASSERTV(0         == O3.size());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());

            bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
            o1.swap(o3);
            ASSERTV(0         == O1.size());
            ASSERTV(NUM_VALUES == O2.size());
            ASSERTV(NUM_VALUES == O3.size());
            ASSERTV(monitor1.isInUseSame());
            ASSERTV(monitor1.isTotalSame());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());

            if (veryVerbose) puts("swap O3 with O2");
            o3.swap(o2);
            ASSERTV(0         == O1.size());
            ASSERTV(NUM_VALUES == O2.size());
            ASSERTV(NUM_VALUES == O3.size());
            ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down,
                                             // depending on the implementation
            ASSERTV(!monitor1.isTotalUp());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());

            ASSERTV(&objectAllocator1 == O1.allocator().mechanism());
            ASSERTV(&objectAllocator1 == O2.allocator().mechanism());
            ASSERTV(&objectAllocator1 == O3.allocator().mechanism());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
             puts("Test primary manipulators/accessors on every permutation.");
        // TBD: Do we really need to do "every permutation" in BREATHING test?

        // Assume that the array `intValues` is initially sorted.  That is true
        // when this test case was initially written, and should be maintained
        // through any future edits.  Otherwise, sort the array at this point.
        do {
            // For each possible permutation of values, insert values iterate
            // over the resulting container, find values, and then erase values

            Obj x(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator);
            const Obj& X = x;
            for (size_t i = 0; i != NUM_VALUES; ++i) {
                Obj y(X, &objectAllocator); const Obj& Y = y;
                ASSERTV(X == Y);
                ASSERTV(!(X != Y));

                ASSERTV(i, 0 == X.find(KeyConfig::extractKey(values[i])));

                // Test `insert`.
                const int value = values[i];
                bool wasInserted = false;
                Link * const link = x.insertIfMissing(&wasInserted, value);
                ASSERTV(   0 != link);
                ASSERTV(true == wasInserted);
                ASSERTV(KeyConfig::extractKey(values[i] ==
                                        ImpUtil::extractKey<KeyConfig>(link)));
                ASSERTV(values[i] == ImpUtil::extractValue<KeyConfig>(link));

                // Test size, empty.
                ASSERTV(i + 1 == X.size());
                ASSERTV(    0 != X.size());

                // Test insert duplicate key
                ASSERTV(link  == x.insertIfMissing(&wasInserted, value));
                ASSERTV(false == wasInserted);
                ASSERTV(i + 1 == X.size());

                // Test find
                Link *it = X.find(KeyConfig::extractKey(values[i]));
                ASSERTV(ImpUtil::extractKey<KeyConfig>(link) ==
                                           ImpUtil::extractKey<KeyConfig>(it));

                ASSERTV(  X != Y );
                ASSERTV(!(X == Y));

                y = x;
                ASSERTV(  X == Y );
                ASSERTV(!(X != Y));
            }

            ASSERTV(0 != objectAllocator.numBytesInUse());
            ASSERTV(0 == defaultAllocator.numBytesInUse());

            // Use remove(link) on all the elements.
            for (size_t i = 0; i != NUM_VALUES; ++i) {
                Link * const it     = x.find(KeyConfig::extractKey(values[i]));
                Link * const nextIt = it->nextLink();

                ASSERTV(0 != it);
                ASSERTV(KeyConfig::extractKey(values[i] ==
                                          ImpUtil::extractKey<KeyConfig>(it)));
                ASSERTV(values[i] == ImpUtil::extractValue<KeyConfig>(it));
                Link * const rvIt = x.remove(it);
                ASSERTV(rvIt == nextIt);

                Link * const rvFind = x.find(KeyConfig::extractKey(values[i]));
                ASSERTV(0 == rvFind);

                ASSERTV(NUM_VALUES - i - 1 == X.size());
            }
        } while (std::next_permutation(values, values + NUM_VALUES));

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose) puts("Test `remove(bslalg::BidirectionalLink *)`.");
        {
        #ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            std::shuffle(values,
                         values + NUM_VALUES,
                         std::default_random_engine());
        #else   // end - use the available better `shuffle` on modern compilers
            std::random_shuffle(values,  values + NUM_VALUES);
        #endif  // end - use C++03 algorithm (may be unsupported in C++17)

            Obj x(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator);
            const Obj& X = x;
            for (size_t i = 0; i != NUM_VALUES; ++i) {
                const int value = values[i];
                Link * const result1 = x.insert(value);
                ASSERTV(0 != result1);
                Link * const result2 = x.insert(value);
                ASSERTV(0 != result2);
                ASSERTV(result1 != result2);
                ASSERTV(2 * (i + 1) == X.size());

                const int key = KeyConfig::extractKey(values[i]);

                Link *start;
                Link *end;
                x.findRange(&start, &end, key);
                ASSERTV(key == ImpUtil::extractKey<KeyConfig>(start));
                ASSERTV(key ==
                            ImpUtil::extractKey<KeyConfig>(start->nextLink()));
                ASSERTV(start->nextLink()->nextLink() == end);
            }

            for (size_t i = 0; i != NUM_VALUES; ++i) {
                Link * const initialRoot = X.elementListRoot();
                const int key = ImpUtil::extractKey<KeyConfig>(initialRoot);

                Link * const rvIt1 = x.remove(X.elementListRoot());
                ASSERTV(initialRoot != rvIt1);
                ASSERTV(initialRoot != X.elementListRoot());
                ASSERTV(X.elementListRoot() == rvIt1);
                ASSERTV(X.find(key) == rvIt1);

                ASSERTV((2 * (NUM_VALUES - i) - 1),   X.size(),
                        (2 * (NUM_VALUES - i) - 1) == X.size());

                const Link * const rvIt2 = x.remove(x.elementListRoot());
                ASSERTV(x.elementListRoot() == rvIt2);
                ASSERTV(rvIt2 != rvIt1);
                ASSERTV(X.find(key) == 0);
                ASSERTV((2 * (NUM_VALUES - i - 1)),   X.size(),
                        (2 * (NUM_VALUES - i - 1)) == X.size());
            }
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // There should be no allocations for the "default STL-test allocator"
    ASSERTV(bsltfAllocator.numBlocksTotal(),
            0 == bsltfAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
