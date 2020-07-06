// bslstl_hashtable_test1.t.cpp                                       -*-C++-*-
#include <bslstl_hashtable_test1.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtable.h>
#include <bslstl_hashtableiterator.h>  // usage example
#include <bslstl_iterator.h>           // 'distance', in usage example

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
#include <bslmf_isfunction.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_removeconst.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
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

#include <stdexcept>  // to verify correct exceptions are thrown

#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for 'strcmp'

using namespace BloombergLP;
using bslstl::CallableVariable;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 3 parts, 'bslstl_hashtable.t.cpp' (cases 1-10, plus the usage example)
// 'bslstl_hashtable_test1.cpp' (cases 11-13), and 'bslstl_hashtable_test2.cpp'
// (cases 14 and higher).
//
//                                  Overview
//                                  --------
// The component under test is a value-semantic container class template, whose
// elements are indexed by key, and duplicate key values are permitted.  The
// purpose of the component is to support implementing the four standard
// unordered containers, so it may be customized at compile time with a C++11
// standard conforming allocator, the most important of which is the 'bsl'
// allocator that supports runtime customization of the object allocator.
//
// The basic test plan is to employ the standard 10 test cases for a value-
// semantic type, before incrementally validating the remaining methods.  One
// important issue is the selection of primary manipulators.  The 'insert'
// operations of the container may trigger a 'rehash' operation, which is
// unrelated to value.  As this is an operation we prefer to defer testing
// until after the initial ten value-semantic test cases, we create an
// 'insertElement' function that checks if an insert would trigger a rehash
// before proceeding.  This becomes our primary manipulator, coupled with a
// strategy to always reserve enough space at construction to accommodate all
// the values inserted for that test scenario.  This is then sufficient to
// bring the object to any valid state.
//
// The Primary Manipulators and Basic Accessors are decided to be:
//
// Primary Manipulators:
//: o 'insertElement' free function in this test driver
//: o 'removeAll'
//
// Basic Accessors:
//: o 'allocator'
//: o 'comparator'
//: o 'hasher'
//: o 'size'
//: o 'numBuckets'
//: o 'maxLoadFactor'
//: o 'rehashThreshold'
//: o 'elementListRoot'
//: o 'bucketAtIndex'
//: o 'bucketIndexForKey'
//
// We note that of the basic accessors, only 'size' and 'elementListRoot'
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
// As we are testing a template, we will make heavy use of the 'bsltf' template
// test facility.  Our primary means of implementing each test case will be to
// write a single function template for the case, that can test and satisfy all
// the concerns of each of the eventual instantiations.  For example, while
// testing the range of potential allocator types, and the way memory is
// consumed by the object under test, we will arrange for all allocators to
// wrap a 'bslma::TestAllocator', which can separately be queried for the
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
// ----------------------------------------------------------------------------
// TYPES
//*[20] typedef ALLOCATOR                              AllocatorType;
//*[20] typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[20] typedef typename KEY_CONFIG::KeyType           KeyType;
//*[20] typedef typename KEY_CONFIG::ValueType         ValueType;
//*[20] typedef bslalg::BidirectionalNode<ValueType>   NodeType;
//*[20] typedef typename AllocatorTraits::size_type    SizeType;
//
// CREATORS
// [  ] HashTable(const ALLOCATOR& allocator = ALLOCATOR());
// [ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
// [ 7] HashTable(const HashTable& original);
// [ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
// [  ] HashTable(MovableRef<HashTable> original);
// [  ] HashTable(MovableRef<HashTable> original, const ALLOCATOR& allocator);
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
// [ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define BSL_TF_EQ      BSLTF_TEMPLATETESTFACILITY_COMPARE_EQUAL
#define BSL_TF_NOT_EQ  BSLTF_TEMPLATETESTFACILITY_COMPARE_NOT_EQUAL

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// As this is a break-out testing component for another component (without the
// '_test' suffix) all negative testing must use the '_RAW' macros, or fail on
// validating the component name.

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU // An alias for a string that can be
                                      // treated as the "%zu" format

#define TD BSLS_BSLTESTUTIL_FORMAT_TD // An alias for a string that can be
                                      // treated as the "%td" format


// ============================================================================
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

// We note that certain test cases rely on the reference collapsing rules that
// were adopted shortly after C++03, and so are not a feature of many older
// compilers, or perhaps compilers in strictly conforming modes.

#if __cplusplus <= 199711L
#  define BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
// The IBM implementation of 'abort' does not have a no-return annotation, so
// may yield warnings about falling off the end of a function without returning
// a value, where 'abort' is being used as the fail-safe in some functions.
# define BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT
#endif

// Note that some compilers struggle with the number of template instantiations
// in this test driver.  We define this macro to simplify the test driver for
// them, until such time as we can provide a more specific review of the type
// based concerns, and narrow the range of tests needed for confirmed coverage.
//
// Currently we are enabling the minimal set of test types on:
// XLC                    (CMP_IBM)
// Sun Studio             (CMP_SUN)

#define BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS
// Undefine the macro 'BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS' to reduce the set
// of types tested by this test driver.  This is sometimes necessary for
// resource constrained environments, or compilers struggling under the load of
// too many template instantiations.

#define BSLSTL_HASHTABLE_MINIMALTEST_TYPES      \
        signed char,                            \
        bsltf::TemplateTestFacility::MethodPtr, \
        bsltf::AllocBitwiseMoveableTestType,    \
        bsltf::MovableAllocTestType,            \
        TestTypes::MostEvilTestType

// The following macro can be enabled to provide a truly minimal test driver
// (fast enough to be suitable for testing during development):
//
//  #define BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK

#if defined(BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK)
# undef BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS
#
# undef  BSLSTL_HASHTABLE_MINIMALTEST_TYPES
# define BSLSTL_HASHTABLE_MINIMALTEST_TYPES  TestTypes::MostEvilTestType
#endif

//=============================================================================
//                              TEST SUPPORT
//-----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

bslma::TestAllocator *g_bsltfAllocator_p = 0;

// Peeking into the "white box" of our standard table of prime numbers in
// 'HashTable_Util', we can see critical sequence lengths around 2, 5 and 13.
// That suggests we will want to pay attention to length 0, 1, 2, 3, 4, 5, 6,
// 12, 13 and 14.  We will pay special attention to all permutations around the
// smaller lengths, and test a representative range of concerns of distinct/
// duplicate values, in and out of order, for the longer sequences.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
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

static
const float DEFAULT_MAX_LOAD_FACTOR[] = {
#if defined(BSLSTL_HASHTABLE_TEST_EXTREME_CASES)
    // Note that using tiny values for 'maxLoadFactor' can consume a huge
    // amount of memory for no real purpose, and greatly slows down the
    // running of the test case.  These test cases are disabled by default
    // as a practical matter, but can be run manually by defining the macro
    // 'BSLSTL_HASHTABLE_TEST_EXTREME_CASES' when building the test driver.
            1e-5,    // This is small enough to test extremely low value
#endif
            0.125f,  // Low value used throughout the test driver
            1.0f,    // Default and most common value
            8.0f,    // High value used throughout the test driver
            native_std::numeric_limits<float>::infinity()  // edge case
};
static const int DEFAULT_MAX_LOAD_FACTOR_SIZE =
              sizeof DEFAULT_MAX_LOAD_FACTOR / sizeof *DEFAULT_MAX_LOAD_FACTOR;

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;

//=============================================================================
//                      HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl
{
                    // ============================================
                    // class template bsl::equal_to specializations
                    // ============================================

template <>
struct equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    // This class template specialization provides a functor to determine when
    // two '::BloombergLP::bsltf::NonEqualComparableTestType' objects have the
    // same value.  Usually, the primary template calls 'operator==' to
    // determine the result, but that cannot work for the test type that is
    // specifically designed to not have an 'operator=='.  In such cases, we
    // must provide an explicit specialization of this template if we expect
    // the test type to work in our test cases along side other types.

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          first_argument_type;
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          second_argument_type;
    typedef bool                                          result_type;

    // ACCESSORS
    bool operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                    const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                         const;
        // Return 'true' if the specified 'a' and 'b' objects have the same
        // value, and 'false' otherwise.  Two 'NonEqualComparableTestType'
        // objects have the same value if ... (TBD)
};

                    // ========================================
                    // class template bsl::hash specializations
                    // ========================================

template <>
struct hash< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    // This class template specialization provides a functor to determine a
    // hash value for '::BloombergLP::bsltf::NonEqualComparableTestType'
    // objects.

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType argument_type;
    typedef size_t                                           result_type;

    // ACCESSORS
    size_t operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType&
                                                                  value) const;
        // Return a hash for the specified 'value' such that this functor
        // satisfies the Hash requirements of the C++ Standard,
        // [hash.requirements].
};

}  // close namespace bsl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes
{

                       // ===========================
                       // class AwkwardMaplikeElement
                       // ===========================

class AwkwardMaplikeElement {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while the
    // class itself *is* equality-comparable (as required of a value-semantic
    // type) so that a HashTable of these objects should have a well-defined
    // 'operator=='.  Note that this class is a specific example for a specific
    // problem, rather than a template providing the general test type for keys
    // distinct from values, as the template test facility requires an explicit
    // specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  public:
    // CREATORS
    AwkwardMaplikeElement();
        // Create an 'AwkwardMaplikeElement' element having '0' as its 'data'.

    explicit
    AwkwardMaplikeElement(int value);
        // Create an 'AwkwardMaplikeElement' element having the specified
        // 'value' as its 'data'.

    explicit
    AwkwardMaplikeElement(const bsltf::NonEqualComparableTestType& value);
        // Create an 'AwkwardMaplikeElement' element having the specified
        // 'value' as its 'data'.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    const bsltf::NonEqualComparableTestType& key() const;
        // Return a reference offering non-modifiable access to the 'key' of
        // this object.
};

bool operator==(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AwkwardMaplikeElement' objects have
    // the same value if ... (TBD)

bool operator!=(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AwkwardMaplikeElement' objects
    // do not have the same value if ... (TBD)

void debugprint(const AwkwardMaplikeElement& value);
    // Print to the console a textual representation of the specified 'value'.
    // Note that this representation is intended only to support error reports
    // and not as a portable format.

                       // ======================
                       // class MostEvilTestType
                       // ======================

class MostEvilTestType {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while the
    // class itself *is* equality-comparable (as required of a value-semantic
    // type) so that a HashTable of these objects should have a well-defined
    // 'operator=='.  Note that this class is a specific example for a specific
    // problem, rather than a template providing the general test type for keys
    // distinct from values, as the template test facility requires an explicit
    // specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  private:
    // NOT IMPLEMENTED
    void operator=(MostEvilTestType&); // = delete

    void operator&();  // = delete;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&); // = delete;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&); // = delete;

    static void* operator new(std::size_t size); // = delete
    static void* operator new(std::size_t size, void *ptr); // = delete
    static void operator delete(void *ptr); // = delete

  public:
    // CREATORS
    explicit MostEvilTestType(int value);
        // Create an 'AwkwardMaplikeElement' object having the specified
        // 'value' as its 'data'.

    // explicit MostEvilTestType(const MostEvilTestType& original) = default;
        // Create a 'MostEvilTestType' object having the same 'data' value as
        // the specified 'original'.

    // ~MostEvilTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

bool operator==(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MostEvilTestType' objects have the
    // same value if ... (TBD)

bool operator!=(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MostEvilTestType' objects do
    // not have the same value if ... (TBD)

void debugprint(const MostEvilTestType& value);
    // Print to the console a textual representation of the specified 'value'.
    // Note that this representation is intended only to support error reports
    // and not as a portable format.

}  // close namespace TestTypes

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace BloombergLP {
namespace bslstl {
// HashTable-specific print function, which must be provided in the same
// namespace as the class do be printed.

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
        const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t);
    // Print the value of the specified 'HashTable' 't' in a format suitable
    // for reading for debug purposes.
}  // close package namespace

namespace bsltf {

template <>
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                               const TestTypes::AwkwardMaplikeElement& object);

template <>
int TemplateTestFacility::getIdentifier<TestTypes::MostEvilTestType>(
                                    const TestTypes::MostEvilTestType& object);

}  // close namespace bsltf
}  // close enterprise namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef BSLS_PLATFORM_CMP_CLANG
namespace TestMachinery
#else
namespace
#endif
{
                       // ===============
                       // class BoolArray
                       // ===============

class BoolArray {
    // This class holds a set of boolean flags, the number of which is
    // specified when such an object is constructed.  The number of flags is
    // fixed at the time of construction, and the object is neither copy-
    // constructible nor copy-assignable.  The flags can be set and tested
    // using 'operator[]'.  This class is a lightweight alternative to
    // 'vector<bool>' as there is no need to introduce such a component
    // dependency for a simple test facility (in the same package).
    // TBD The constructor should support a 'bslma::Allocator *' argument to
    // supply memory for the dynamically allocated array.

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
    explicit BoolArray(size_t n);
        // Create a 'BoolArray' object holding the specified 'n' boolean flags.

    ~BoolArray();
        // Destroy this object, reclaiming any allocated memory.

    // MANIPULATORS
    bool& operator[](size_t index);
        // Return a reference offering modifiable access to the boolean flag at
        // the specified 'index'.  The behavior is undefined unless
        // 'index < size()'.

    // ACCESSORS
    bool operator[](size_t index) const;
        // Return the value of the boolean flag at the specified 'index'.  The
        // behavior is undefined unless 'index < size()'.

    size_t size() const;
        // Return the number of boolean flags held by this object.
};

struct TestException : native_std::exception{};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===============================
                       // class GroupedEqualityComparator
                       // ===============================

template <class TYPE, int GROUP_SIZE>
class GroupedEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE', but in a
    // way that multiple distinct value will compare equal with each other.
    // The function-call operator compares the integer returned by the class
    // method 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const;
        // Return 'true' if the integer representation of the specified 'lhs'
        // divided by 'GROUP_SIZE' (rounded down) is equal to than integer
        // representation of the specified 'rhs' divided by 'GROUP_SIZE'.
};

                       // ===================
                       // class GroupedHasher
                       // ===================

template <class TYPE, class HASHER, int GROUP_SIZE>
class GroupedHasher : private HASHER {
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
    size_t operator() (const TYPE& value);
        // Return the hash value of the integer representation of the specified
        // 'value' divided by 'GROUP_SIZE' (rounded down) is equal to than
        // integer representation of the specified 'rhs' divided by
        // 'GROUP_SIZE'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ================================
                       // class ThrowingEqualityComparator
                       // ================================

template <class TYPE>
class ThrowingEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times 'operator()' is called
    size_t         d_throwInterval; // throw after this many 'operator()' calls

  public:
    // CREATORS
    //! ThrowingEqualityComparator(const ThrowingEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    explicit ThrowingEqualityComparator(size_t id = 0);
        // Create a 'ThrowingEqualityComparator'.  Optionally, specify 'id'
        // that can be used to identify the object.

    // MANIPULATORS
    void setId(size_t value);
        // Set the 'id' of this object to the specified 'value'.

    void setThrowInterval(size_t value);
        // Set to the specified 'value' the number of times 'operator()' may be
        // called after throwing an exception before another such call would
        // throw.  If '0 == value' then disable throwing of exceptions by
        // 'operator()'.
        //
        // TBD: Document behavior if 'value' is less than the number of times
        // 'operator()' has already been called.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const;
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.

    size_t count() const;
        // Return the number of times 'operator()' is called.

    size_t id() const;
        // Return the 'id' of this object.

    size_t throwInterval() const;
        // Return the number of times 'operator()' may be called after throwing
        // an exception before another such call would throw, or '0' if no
        // exceptions are ever thrown.
};

template <class TYPE>
bool operator==(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ThrowingEqualityComparator' functors have the
    // same value if they have the same 'id'.

template <class TYPE>
bool operator!=(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'ThrowingEqualityComparator' functors
    // do not have the same value if they do not have the same 'id'.

                       // =========================
                       // class ThrowingHashFunctor
                       // =========================

template <class TYPE>
class ThrowingHashFunctor {
    // This value-semantic class meets the C++11 'Hash' requirements (C++11
    // [hash.requirements], 17.6.3.4) with an overload for 'operator()' that
    // can be configured at runtime to throw exceptions after a user-supplied
    // number of function calls.  It can also be configured to never throw, and
    // merely count the number of invocations of the function call operator.
    // Note that this functor relies on "logical" rather than "physical"
    // 'const' behavior, as it must count the number of times a
    // 'const'-qualified function is invoked.

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times 'operator()' is called
    size_t         d_throwInterval; // throw after this many 'operator()' calls

  public:
    // CREATORS
    //! ThrowingHashFunctor(const ThrowingHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit ThrowingHashFunctor(size_t id = 0);
        // Create a 'ThrowingHashFunctor'.  Optionally, specify 'id' that can
        // be used to identify the object.

    // MANIPULATORS
    void setId(size_t value);
        // Set the 'id' of this object to the specified value.  Note that the
        // 'id' contributes to the value produced by the 'operator()' method,
        // so the 'id' of a 'ThrowingHashFunctor' should not be changed for
        // functors that are installed in 'HashTable' objects.

    void setThrowInterval(size_t value);
        // Set to the specified 'value' the number of times 'operator()' may be
        // called after throwing an exception before another such call would
        // throw.  If '0 == value' then disable throwing of exceptions by
        // 'operator()'.
        //
        // TBD: Document behavior if 'value' is less than the number of times
        // 'operator()' has already been called.

    // ACCESSORS
    native_std::size_t operator() (const TYPE& obj) const;
        // Increment a counter that records the number of times this method is
        // called.   Return a hash value for the specified 'obj'.  The behavior
        // is undefined unless 'obj' is a value supplied by the BSL template
        // test facility.

    size_t count() const;
        // Return the number of times 'operator()' is called.

    size_t id() const;
        // Return the 'id' of this object.

    size_t throwInterval() const;
        // Return the number of times 'operator()' may be called after throwing
        // an exception before another such call would throw, or '0' if no
        // exceptions are ever thrown.
};

template <class TYPE>
bool operator==(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ThrowingHashFunctor' functors have the same
    // value if they have the same 'id'.

template <class TYPE>
bool operator!=(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'ThrowingHashFunctor' functors do not
    // have the same value if they do not have the same 'id'.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ========================
                       // class TestFacilityHasher
                       // ========================

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestFacilityHasher : private HASHER { // exploit empty base
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.  The function-call operator is implemented by calling the wrapper
    // functor, 'HASHER', with integers converted from objects of 'KEY' by the
    // class method 'TemplateTestFacility::getIdentifier'.    Note that this
    // class privately inherits from the specified 'HASHER' class in order to
    // exploit any compiler optimizations for empty base classes.

  public:
    TestFacilityHasher(const HASHER& hash = HASHER());              // IMPLICIT

    // ACCESSORS
    native_std::size_t operator() (const KEY& k) const;
        // Return a hash code for the specified 'k' using the wrapped functor
        // of (template parameter) type 'HASHER' supplied at construction.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ================================
                       // class TestConvertibleValueHasher
                       // ================================

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestConvertibleValueHasher : private TestFacilityHasher<KEY, HASHER> {
    // TBD This test class provides...

    typedef TestFacilityHasher<KEY, HASHER> Base;

  public:
    TestConvertibleValueHasher(const HASHER& hash = HASHER());      // IMPLICIT

    // ACCESSORS
    native_std::size_t operator()(const bsltf::ConvertibleValueWrapper<KEY>& k)
                                                                         const;
        // Return a hash code for the specified 'k' using the wrapped functor
        // of (template parameter) type 'HASHER' supplied at construction.
};

                       // ====================================
                       // class TestConvertibleValueComparator
                       // ====================================

template <class KEY>
class TestConvertibleValueComparator {
    // This test class provides...

  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator()(
                           const bsltf::ConvertibleValueWrapper<KEY>& a,
                           const bsltf::ConvertibleValueWrapper<KEY>& b) const;
        // Return a value convertible to 'true' if the specified 'a' has the
        // same value as the specified 'b', and a value convertible to 'false'
        // otherwise..
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =======================
                       // class GenericComparator
                       // =======================

class GenericComparator {
  public:
    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bsltf::EvilBooleanType operator() (ARG1_TYPE& arg1, ARG2_TYPE& arg2);
        // Return a value convertible to 'true' if the specified 'arg1' has the
        // same value as the specified 'arg2', for some unspecified definition
        // that defaults to 'operator==', but may use some other functionality.
};

                       // ===================
                       // class GenericHasher
                       // ===================

class GenericHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.

  public:
    // ACCESSORS
    template <class KEY>
    native_std::size_t operator() (KEY& k);
        // Return a hash code for the specified 'k'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ==========================
                       // class ModifiableComparator
                       // ==========================

template <class KEY>
class ModifiableComparator {
  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator() (KEY& arg1, KEY& arg2);
        // Return a value convertible to 'true' if the specified 'arg1' has the
        // same value as the specified 'arg2', for some unspecified definition
        // that defaults to 'operator==', but may use some other functionality
        // for 'KEY' types that do not support this operator.
};

                       // ======================
                       // class ModifiableHasher
                       // ======================

template <class KEY>
class ModifiableHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.

  public:
    // ACCESSORS
    native_std::size_t operator() (KEY& k);
        // Return a hash code for the specified 'k'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===========================
                       // class DefaultOnlyComparator
                       // ===========================

template <class TYPE>
class DefaultOnlyComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  However
    // it is a perverse type intended for testing templates against their
    // minimum requirements, and as such is neither copyable nor swappable, and
    // is only default constructible.  The function call operator will return a
    // perverse type that is convertible-to-bool.

  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator() (const TYPE& lhs, const TYPE& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' have the same value.
    {
        return BSL_TF_EQ(lhs, rhs);
    }
};

                       // =======================
                       // class DefaultOnlyHasher
                       // =======================

template <class TYPE>
class DefaultOnlyHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that returns the same hash for 'TYPE' values using the standard
    // 'bsl::hash' functor.  However, this class is a perverse type intended
    // for testing templates against their minimum requirements, and as such is
    // neither copyable nor swappable, and is only default constructible.

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
    size_t operator() (const TYPE& value)
        // Return the hash of the specified 'value'.
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

    static bool compare(const KEY& lhs, const KEY& rhs);
        // Return 'true' if the specified 'lhs' has the same value as the
        // specified 'rhs' when compared using the 'bsltf' test facility
        // 'BSLTF_EQ', and 'false' otherwise.

    static size_t hash(const KEY& k);
        // Return the hash value of the specified 'k' according to the 'bsltf'
        // hash functor, 'TestFacilityHasher<KEY>'.
};

                       // ========================
                       // class MakeCallableEntity
                       // ========================

template <class CALLABLE>
struct MakeCallableEntity {
    typedef CALLABLE CallableEntityType;

    static CallableEntityType make();
        // Return a default-constructed object of the (template parameter) type
        // 'CALLABLE'.
};

template <class CALLABLE>
struct MakeCallableEntity<CALLABLE&> {
    typedef CALLABLE& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a default-constructed object of the (template
        // parameter) type 'CALLABLE'.  Note that this is will be a reference
        // to a singleton object; any modifications made to that object will be
        // reflected in references to previous and subsequent calls to this
        // function.
};

template <class FUNCTOR, bool ENABLE_SWAP>
struct MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> > {
    typedef bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> CallableEntityType;

    static CallableEntityType make();
        // Return a functor wrapping a default-constructed object of the
        // (template parameter) type 'FUNCTOR'.  The adapter overloads all
        // operations other than the function-call operator in a most awkward
        // way, and is swappable if and only if (the template parameter)
        // 'ENABLE_SWAP' is 'true'.
};

template <class KEY>
struct MakeCallableEntity<size_t (*)(const KEY&)> {
    typedef size_t FunctionType(const KEY&);
    typedef FunctionType *CallableEntityType;

    static CallableEntityType make();
        // Return the address of a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class KEY>
struct MakeCallableEntity<bool (*)(const KEY&, const KEY&)> {
    typedef bool FunctionType(const KEY&, const KEY&);
    typedef FunctionType *CallableEntityType;

    static CallableEntityType make();
        // Return the address of a function that can compare two objects of the
        // (template parameter) type 'KEY' for equality.
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(&)(ARG)> {
    typedef RESULT FunctionType(ARG);
    typedef FunctionType& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(&)(ARG1, ARG2)> {
    typedef RESULT FunctionType(ARG1, ARG2);
    typedef FunctionType& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a function that can compare two objects of the
        // (template parameter) type 'KEY' for equality.
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(ARG)> {
    typedef RESULT CallableEntityType(ARG);

    static CallableEntityType& make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(ARG1, ARG2)> {
    typedef RESULT CallableEntityType(ARG1, ARG2);

    static CallableEntityType& make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===================
                       // class MakeAllocator
                       // ===================

template <class ALLOCATOR>
struct MakeAllocator {
    // TBD This utility class template ...

    // PUBLIC TYPES
    typedef ALLOCATOR AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *);
        // Return a default-constructed 'ALLOCATOR' object.
};

template <class TYPE>
struct MakeAllocator<bsl::allocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsl::allocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsl::allocator<TYPE>' object wrapping the specified
        // 'basicAllocator'.
};

template <class TYPE>
struct MakeAllocator<bsltf::StdTestAllocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdTestAllocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsltf::StdTestAllocator<TYPE>' object wrapping the
        // specified 'basicAllocator'.
};

template <class TYPE, bool A, bool B, bool C, bool D>
struct MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdStatefulAllocator<TYPE, A, B, C, D> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsltf::StdStatefulAllocator<TYPE, A, B, C, D>' object
        // wrapping the specified 'basicAllocator'.
};

                       // =================
                       // class ObjectMaker
                       // =================

// The 'ObjectMaker' template and its associated specializations customize the
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
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct ObjectMaker {
    // TBD This utility class template ...

    typedef          ALLOCATOR             AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<ALLOCATOR>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>   Obj;

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
struct ObjectMaker<KEY_CONFIG,
                   HASHER,
                   COMPARATOR,
                   bsl::allocator<typename KEY_CONFIG::ValueType> > {
    // TBD This utility class template specialization...

    typedef bsl::allocator<typename KEY_CONFIG::ValueType> AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
struct ObjectMaker<
                KEY_CONFIG,
                HASHER,
                COMPARATOR,
                bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                             A, B, C, D> > {
    // TBD This utility class template specialization...

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    typedef bsltf::StdStatefulAllocator<ValueType, A, B, C, D>   AllocatorType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type  SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "ab"; }
    static const char * specForCopyTests()      { return "ab"; }
    static const char * specForDefaultTests()   { return "ab"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//                         test support functions

template <class ALLOCATOR>
bslma::TestAllocator *extractTestAllocator(ALLOCATOR&);
    // Return a null pointer value, as there is no way to extract a test
    // allocator from an unknown allocator type.  This function should be
    // overloaded for allocator types for which it is possible to extract a
    // test allocator.

template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsl::allocator<TYPE>& alloc);
    // Return the address of the allocator 'mechanism' wrapped by the specified
    // 'alloc' if it is a test allocator (which can be found by 'dynamic_cast')
    // and a null pointer value otherwise.

template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsltf::StdTestAllocator<TYPE>&);
    // Return the address of the installed StdTestAllocator if it is a test
    // allocator, and a null pointer value otherwise.  Note that all
    // 'bsltf::StdTestAllocator's share the same allocator, which is set by the
    // 'bsltf::StdTestAllocatorConfiguration' utility.  We can determine if
    // this is wrapping a test allocator using 'dynamic_cast'.

template <class TYPE, bool A, bool B, bool C, bool D>
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc);
    // Return the address of the test allocator wrapped by the specified
    // 'alloc'.

#if !defined(BSLS_PLATFORM_CMP_CLANG)
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace bsl
{
                    // --------------------------------------------
                    // class template bsl::equal_to specializations
                    // --------------------------------------------

inline
bool equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
                    (const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                     const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                          const
{
    return BSL_TF_EQ(a, b);
}

                    // ----------------------------------------
                    // class template bsl::hash specializations
                    // ----------------------------------------

// not inlining initially due to static local data
size_t hash< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
          (const ::BloombergLP::bsltf::NonEqualComparableTestType& value) const
{
    static const bsl::hash<int> EVALUATE_HASH = bsl::hash<int>();
    return EVALUATE_HASH(value.data());
}

}  // close namespace bsl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes
{
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

                       // ----------------------
                       // class MostEvilTestType
                       // ----------------------

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace BloombergLP
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void bslstl::debugprint(
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

namespace bsltf {

template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                                const TestTypes::AwkwardMaplikeElement& object)
{
    return object.data();
}

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

#if !defined(BSLS_PLATFORM_CMP_CLANG)
namespace TestMachinery
#else
namespace
#endif
{
                       // -------------------------------
                       // class GroupedEqualityComparator
                       // -------------------------------

// ACCESSORS
template <class TYPE, int GROUP_SIZE>
bool GroupedEqualityComparator<TYPE, GROUP_SIZE>::
operator() (const TYPE& lhs, const TYPE& rhs) const
{
    int leftValue = bsltf::TemplateTestFacility::getIdentifier(lhs)
                  / GROUP_SIZE;
    int rightValue = bsltf::TemplateTestFacility::getIdentifier(rhs)
                   / GROUP_SIZE;

    return leftValue == rightValue;
}

                       // -------------------
                       // class GroupedHasher
                       // -------------------

// ACCESSORS
template <class TYPE, class HASHER, int GROUP_SIZE>
inline
size_t GroupedHasher<TYPE, HASHER, GROUP_SIZE>::operator() (const TYPE& value)
{
    int groupNum = bsltf::TemplateTestFacility::getIdentifier(value)
                 / GROUP_SIZE;

    return HASHER::operator()(groupNum);
}

                       // --------------------------------
                       // class ThrowingEqualityComparator
                       // --------------------------------

// CREATORS
template <class TYPE>
inline
ThrowingEqualityComparator<TYPE>::ThrowingEqualityComparator(size_t id)
: d_id(id)
, d_count(0)
, d_throwInterval(0)
{
}

// MANIPULATORS
template <class TYPE>
inline
void ThrowingEqualityComparator<TYPE>::setId(size_t value)
{
    d_id = value;
}

template <class TYPE>
inline
void ThrowingEqualityComparator<TYPE>::setThrowInterval(size_t value)
{
    d_throwInterval = value;
}

// ACCESSORS
template <class TYPE>
inline
bool ThrowingEqualityComparator<TYPE>::operator() (const TYPE& lhs,
                                                   const TYPE& rhs) const
{
    ++d_count;

    if (d_throwInterval && !(d_count % d_throwInterval)) {
        BSLS_THROW(TestException());
    }

    return bsltf::TemplateTestFacility::getIdentifier(lhs)
        == bsltf::TemplateTestFacility::getIdentifier(rhs);
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::throwInterval() const
{
    return d_throwInterval;
}

template <class TYPE>
inline
bool operator==(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() == rhs.id();
}

template <class TYPE>
inline
bool operator!=(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // -------------------------
                       // class ThrowingHashFunctor
                       // -------------------------

// CREATORS
template <class TYPE>
inline
ThrowingHashFunctor<TYPE>::ThrowingHashFunctor(size_t id)
: d_id(id)
, d_count(0)
, d_throwInterval()
{
}

// MANIPULATORS
template <class TYPE>
inline
void ThrowingHashFunctor<TYPE>::setId(size_t value)
{
    d_id = value;
}

template <class TYPE>
inline
void ThrowingHashFunctor<TYPE>::setThrowInterval(size_t value)
{
    d_throwInterval = value;
}

// ACCESSORS
template <class TYPE>
inline
native_std::size_t
ThrowingHashFunctor<TYPE>::operator() (const TYPE& obj) const
{
    ++d_count;

    if (d_throwInterval && !(d_count % d_throwInterval)) {
        BSLS_THROW(TestException());
    }

    return bsltf::TemplateTestFacility::getIdentifier(obj) + d_id;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::throwInterval() const
{
    return d_throwInterval;
}

template <class TYPE>
inline
bool operator==(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

template <class TYPE>
inline
bool operator!=(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // ------------------------
                       // class TestFacilityHasher
                       // ------------------------

// CREATORS
template <class KEY, class HASHER>
inline
TestFacilityHasher<KEY, HASHER>::TestFacilityHasher(const HASHER& hash)
: HASHER(hash)
{
}

    // ACCESSORS
template <class KEY, class HASHER>
inline
native_std::size_t
TestFacilityHasher<KEY, HASHER>::operator() (const KEY& k) const
{
    int temp =  bsltf::TemplateTestFacility::getIdentifier(k);
    return HASHER::operator()(temp);
}

                       // --------------------------------
                       // class TestConvertibleValueHasher
                       // --------------------------------

template <class KEY, class HASHER>
inline
TestConvertibleValueHasher<KEY, HASHER>::TestConvertibleValueHasher(
                                                            const HASHER& hash)
: Base(hash)
{
}

// ACCESSORS
template <class KEY, class HASHER>
inline
native_std::size_t
TestConvertibleValueHasher<KEY, HASHER>::operator()
                           (const bsltf::ConvertibleValueWrapper<KEY>& k) const
{
    return Base::operator()(k);
}

                       // ------------------------------------
                       // class TestConvertibleValueComparator
                       // ------------------------------------

// ACCESSORS
template <class KEY>
inline
bsltf::EvilBooleanType TestConvertibleValueComparator<KEY>::operator() (
                            const bsltf::ConvertibleValueWrapper<KEY>& a,
                            const bsltf::ConvertibleValueWrapper<KEY>& b) const
{
    return BSL_TF_EQ(static_cast<const KEY&>(a),
                     static_cast<const KEY&>(b));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------
                       // class GenericComparator
                       // -----------------------

// ACCESSORS
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bsltf::EvilBooleanType GenericComparator::operator() (ARG1_TYPE& arg1,
                                                      ARG2_TYPE& arg2)
{
    return BSL_TF_EQ(arg1, arg2);
}

                       // -------------------
                       // class GenericHasher
                       // -------------------

// ACCESSORS
template <class KEY>
native_std::size_t GenericHasher::operator() (KEY& k)
{
    // do not inline initially due to static local data

    typedef typename bsl::remove_const<KEY>::type KEY_TYPE;

    static const TestFacilityHasher<KEY_TYPE> HASHER;
    return HASHER(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // --------------------------
                       // class ModifiableComparator
                       // --------------------------

// ACCESSORS
template <class KEY>
inline
bsltf::EvilBooleanType ModifiableComparator<KEY>::operator() (KEY& arg1,
                                                              KEY& arg2)
{
    return BSL_TF_EQ(arg1, arg2);
}

                       // ----------------------
                       // class ModifiableHasher
                       // ----------------------

// ACCESSORS
template <class KEY>
native_std::size_t ModifiableHasher<KEY>::operator() (KEY& k)
{
    // do not inline initially due to static local data

    typedef typename bsl::remove_const<KEY>::type KEY_TYPE;

    static const TestFacilityHasher<KEY_TYPE> HASHER;
    return HASHER(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------------
                       // class FunctionPointerPolicies
                       // -----------------------------

// do not inline initially due to static local data
template <class KEY>
inline
bool FunctionPointerPolicies<KEY>::compare(const KEY& lhs, const KEY& rhs)
{
    return BSL_TF_EQ(lhs, rhs);
}

template <class KEY>
size_t FunctionPointerPolicies<KEY>::hash(const KEY& k)
{
    static const TestFacilityHasher<KEY> s_hasher = TestFacilityHasher<KEY>();
    return s_hasher(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
typename MakeCallableEntity<RESULT(ARG)>::CallableEntityType &
MakeCallableEntity<RESULT(ARG)>::make()
{
    return *MakeCallableEntity<CallableEntityType *>::make();
}

template <class RESULT, class ARG1, class ARG2>
inline
typename MakeCallableEntity<RESULT(ARG1, ARG2)>::CallableEntityType &
MakeCallableEntity<RESULT(ARG1, ARG2)>::make()
{
    return *MakeCallableEntity<CallableEntityType *>::make();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -------------------
                       // class MakeAllocator
                       // -------------------

template <class ALLOCATOR>
typename MakeAllocator<ALLOCATOR>::AllocatorType
MakeAllocator<ALLOCATOR>::make(bslma::Allocator *)
{
    return AllocatorType();
}

template <class TYPE>
typename MakeAllocator<bsl::allocator<TYPE> >::AllocatorType
MakeAllocator<bsl::allocator<TYPE> >::make(bslma::Allocator *basicAllocator)
{
    return AllocatorType(basicAllocator);
}

template <class TYPE>
typename MakeAllocator<bsltf::StdTestAllocator<TYPE> >::AllocatorType
MakeAllocator<bsltf::StdTestAllocator<TYPE> >::
make(bslma::Allocator *basicAllocator)
{
    // This method is a little bit of overkill (heavy on the assertions) as a
    // left-over from when we were trying hard to nail down a tricky bug that
    // manifests only on the IBM AIX compiler.  It should probably be cleaned
    // up for final release.

    typedef bsltf::StdTestAllocatorConfiguration BsltfAllocConfig;

    bslma::Allocator *installedAlloc = BsltfAllocConfig::delegateAllocator();

    bslma::TestAllocator *currentAllocator
                        = dynamic_cast<bslma::TestAllocator *>(installedAlloc);

    bslma::TestAllocator *newAllocator
                        = dynamic_cast<bslma::TestAllocator *>(basicAllocator);

    ASSERTV(g_bsltfAllocator_p, installedAlloc, currentAllocator, newAllocator,
            g_bsltfAllocator_p && installedAlloc &&
            currentAllocator   && newAllocator);

    ASSERTV(currentAllocator,   newAllocator,
            currentAllocator->name(), newAllocator->name(),
            currentAllocator == newAllocator);

    return AllocatorType();
}

template <class TYPE, bool A, bool B, bool C, bool D>
typename
MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> >::AllocatorType
MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> >::
make(bslma::Allocator *basicAllocator)
{
    bslma::TestAllocator *alloc = dynamic_cast<bslma::TestAllocator *>(
                                                               basicAllocator);
    ASSERT(alloc);
    return AllocatorType(alloc);
}

                       // -----------------
                       // class ObjectMaker
                       // -----------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
ALLOCATOR
ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    switch (config) {
      case 'a': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(objectAllocator);
        *objPtr = new (*fa) Obj(objAlloc);
        return objAlloc;                                              // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj();
        return ALLOCATOR();                                           // RETURN
      } break;
      case 'c': {
        ALLOCATOR result = ALLOCATOR();
        *objPtr = new (*fa) Obj(result);
        return result;                                                // RETURN
      } break;
      case 'd': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
        *objPtr = new (*fa) Obj(objAlloc);
        return objAlloc;                                              // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
ALLOCATOR
ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator,
           const HASHER&          hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    switch (config) {
      case 'a': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(objectAllocator);
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                objAlloc);
        return objAlloc;                                              // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor);
        return ALLOCATOR();                                           // RETURN
      } break;
      case 'c': {
        ALLOCATOR result = ALLOCATOR();
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                result);
        return result;                                                // RETURN
      } break;
      case 'd': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                objAlloc);
        return objAlloc;                                              // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    switch (config) {
      case 'a': {
        *objPtr = new (*fa) Obj(objectAllocator);
        return objectAllocator;                                       // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj();
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'c': {
        *objPtr = new (*fa) Obj(static_cast<bslma::Allocator *>(0));
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'd': {
        *objPtr = new (*fa) Obj(bslma::Default::defaultAllocator());
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
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
        return objectAllocator;                                       // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor);
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'c': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                static_cast<bslma::Allocator *>(0));
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'd': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                bslma::Default::defaultAllocator());
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                   A, B, C, D> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                                A, B, C, D> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    // 'bsltf::StdStatefulAllocator' objects are not DefaultConstructible.  We
    // know that the default allocator installed for this test driver will be a
    // test allocator, so we can safely expect a 'dynamic_cast' to not return a
    // null pointer.  Likewise, the 'objectAllocator' should not be a null
    // pointer either, so we can simply construct the desired allocator object
    // using the test allocator specified by the 'config' parameter, and use
    // that to explicitly construct the desired 'HashTable' object into
    // '*objPtr'.  Note that there is no distinction between config 'a' or 'b'
    // for this allocator, it would be useful if we could find some way to skip
    // config 'a' when running the various test cases.

    bslma::TestAllocator *alloc = 'a' == config
                                ? objectAllocator
                                : dynamic_cast<bslma::TestAllocator *>(
                                           bslma::Default::defaultAllocator());
    ASSERT(alloc);

    AllocatorType result = MakeAllocator<AllocatorType>::make(alloc);
    *objPtr = new (*fa) Obj(result);
    return result;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                   A, B, C, D> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                                A, B, C, D> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator,
           const HASHER&          hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    // 'bsltf::StdStatefulAllocator' objects are not DefaultConstructible.  We
    // know that the default allocator installed for this test driver will be a
    // test allocator, so we can safely expect a 'dynamic_cast' to not return a
    // null pointer.  Likewise, the 'objectAllocator' should not be a null
    // pointer either, so we can simply construct the desired allocator object
    // using the test allocator specified by the 'config' parameter, and use
    // that to explicitly construct the desired 'HashTable' object into
    // '*objPtr'.  Note that there is no distinction between config 'a' or 'b'
    // for this allocator, it would be useful if we could find some way to skip
    // config 'a' when running the various test cases.

    bslma::TestAllocator *alloc = 'a' == config
                                ? objectAllocator
                                : dynamic_cast<bslma::TestAllocator *>(
                                           bslma::Default::defaultAllocator());
    ASSERT(alloc);

    AllocatorType result = MakeAllocator<AllocatorType>::make(alloc);
    *objPtr = new (*fa) Obj(hash,
                            compare,
                            initialBuckets,
                            initialMaxLoadFactor,
                            result);
    return result;
}

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

//                         test support functions

template <class ALLOCATOR>
inline
bslma::TestAllocator *
extractTestAllocator(ALLOCATOR&)
    // Return a null pointer value.  Note that in general, there is no way to
    // extract a test allocator from a generic allocator type.
{
    return 0;
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsl::allocator<TYPE>& alloc)
    // Return the address of the test allocator wrapped by the specified
    // 'alloc', and a null pointer value if 'alloc' does not wrap a
    // 'bslma::TestAllocator'.
{
    return dynamic_cast<bslma::TestAllocator *>(alloc.mechanism());
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsltf::StdTestAllocator<TYPE>&)
    // Return the address of the test allocator wrapped by the standard test
    // allocator singleton, and a null pointer value if the standard test
    // allocator singleton 'alloc' does not currently wrap a
    // 'bslma::TestAllocator'.
{
    // All 'bsltf::StdTestAllocator's share the same allocator, which is set by
    // the 'bsltf::StdTestAllocatorConfiguration' utility.  We can determine if
    // this is wrapping a test allocator using 'dynamic_cast'.

    return dynamic_cast<bslma::TestAllocator *>(
                    bsltf::StdTestAllocatorConfiguration::delegateAllocator());
}

template <class TYPE, bool A, bool B, bool C, bool D>
inline
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc)
{
    return dynamic_cast<bslma::TestAllocator *>(alloc.allocator());
}

#if !defined(BSLS_PLATFORM_CMP_CLANG)
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace
{

bool expectPoolToAllocate(size_t n)
    // Return 'true' if the memory pool used by the container under test is
    // expected to allocate memory on the inserting the specified 'n'th
    // element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
                  bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
                  const typename KEY_CONFIG::ValueType&              value)
    // Insert an element having the specified 'value' into the specified
    // 'hashTable' and return the address of the new node, unless the insertion
    // would cause the hash table to exceed its 'maxLoadFactor' and rehash, in
    // which case return a null pointer value.  Return a null pointer value if
    // the 'hashTable' address is a null pointer value.
{
    if (!hashTable) {
        return 0;                                                     // RETURN
    }

    if (hashTable->size() == hashTable->rehashThreshold()) {
        return 0;                                                     // RETURN
    }

    return hashTable->insert(value);
}

template <class SIZE_TYPE>
SIZE_TYPE predictNumBuckets(SIZE_TYPE length, float maxLoadFactor)
    // Return the minimum number of buckets necessary to support the specified
    // 'length' array of elements in a 'HashTable' having the specified
    // 'maxLoadFactor' without rehashing.  Note that typically the result will
    // be passed to a 'HashTable' constructor or reserve call, which may in
    // turn choose to create even more buckets to preserve its growth strategy.
    // This function does not attempt to predict that growth strategy, but
    // merely predict the minimum number of buckets that strategy must
    // accommodate.
{
    if (!length) {
        return 0;                                                     // RETURN
    }

    if (1.0 / static_cast<double>(native_std::numeric_limits<SIZE_TYPE>::max())
                                                             > maxLoadFactor) {
        return native_std::numeric_limits<SIZE_TYPE>::max();          // RETURN
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
    // NOTE: THIS TEST IS EXPENSIVE, WITH QUADRATIC COMPLEXITY ON LIST LENGTH
    //                       DO NOT CALL IN A TIGHT LOOP
    // Verify the specified 'containerList' has the specified 'expectedSize'
    // number of elements, and contains the same values as the array in the
    // specified 'expectedValues', and that the elements in the list are
    // arranged so that elements whose keys compare equal using the specified
    // 'compareKeys' predicate are all arranged contiguously within the list.
    // Return 0 if 'container' has the expected values, and a non-zero value
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

#if !defined(BSLS_PLATFORM_CMP_CLANG)
    TestMachinery::BoolArray foundValues(expectedSize);
#else
    BoolArray foundValues(expectedSize);
#endif

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
    // that has a non-'const' qualified 'operator()'.  Note that we are now
    // requiring that the comparator be copy-constructible, which may be an
    // issue for testing a 'HashTable' default-constructed with functors that
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

#if !defined(BSLS_PLATFORM_CMP_CLANG)
using namespace TestMachinery;
#endif

// - - - - - Wrapper to forward test-class adapters to the main harness - - - -
// Note that this is a generic facility that should be exported to a component
// in its own right, in the 'bsltf' package.

template <class CONFIGURED_DRIVER>
struct TestDriver_ForwardTestCasesByConfiguation {
    // This utility class template provides forwarding utilities for invoking
    // test case functions of the (template type parameter) 'CONFIGURED_DRIVER'
    // class.  It is expected that a 'CONFIGURED_DRIVER' class is an
    // instantiation of the 'TestDriver' class template, with the template
    // arguments chosen to provide a specific portion of test coverage of the
    // whole type-space under test.
    //
    // This supports the following set of templates, implemented in terms of
    // this class template, that are parameterized on the single value type for
    // the container under test, which is the form required for support by the
    // 'bsltf' template testing facility.

    // TEST CASES

        // There is no testCase10.
    static void testCase11() { CONFIGURED_DRIVER::testCase11(); }
    static void testCaseReserveForNumElements() {
                          CONFIGURED_DRIVER::testCaseReserveForNumElements(); }
        // There is no testCase12
    static void testCase13() { CONFIGURED_DRIVER::testCase13(); }
};

//- - - - - - - - - - - - Classes to implement test cases - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class TestDriver;
    // Forward declaration of the main test driver.

// Concerns for template parameters
// --------------------------------
// The primary template supports four template parameters, each of which raises
// concerns that the template behaves correctly when instantiated with a
// variety of types that satisfy the requirements of the appropriate type
// parameter.  These concerns may affect all test cases, although a subset of
// concerns may not apply in specific cases where more precise requirements
// apply.  We enumerate the type-related concerns for each template parameter
// here in a common place that may be referenced in the testing concerns for
// each test case below.  There are also common concerns for the function-like
// types 'HASHER' and 'COMPARATOR' that we catalog separately.
//
// KEY_CONFIG
//- - - - - -
//
//Test Plan
//---------
// We will use a variety of test allocators from 'bsltf' to test the range of
// allocator-related concerns.
//
// Test with a minimal allocator relying on 'allocator_traits' for everything.
// Test with a complete allocator that supplies distinct, observable behavior
//   for each property supported by 'allocator_traits'
// Provide allocators supporting each combination of trait that triggers
//   different allocator behavior
//: o type alias 'propagate_on_container_copy_construction'  (true false)
//: o type alias 'propagate_on_container_copy_assignment'    (true false)
//: o type alias 'propagate_on_container_swap'               (true false)
//: o type alias 'pointer' is a native pointer, or a "smart" pointer
//
// Note that for this initial release we can use a greatly simplified plan, as
// our current implementation of 'allocator_traits' does not support any
// deduction of traits.  We will still use the minimal and maximal allocator
// interfaces supported by this implementation, and the range of customizable
// behaviors.
//
// Simplifying assumptions of our limited 'allocator_traits' implementation:
//: o no support for allocators returning smart pointers
//: o no support for any of the fine-grained propagation traits
//: o no deducing typedefs, everything must be supplied in the allocator
//: o copy constructing a container copies the allocator of the original
//    unless it is 'bsl::allocator', in which case the default is used.
//
// - - - - - - - - - - Pre-packaged test harness adapters - - - - - - - - - - -
// The template test facility, 'bsltf', requires class templates taking a
// single argument, that is the element type to vary in the test.  We desire a
// variety of configurations pushing the various policy parameters of the
// 'HashTable' class template, such as the type of functors, the key extraction
// policy, etc. so we write some simple adapters that will generate the
// appropriate instantiation of the test harness, from a template parameterized
// on only the element type (to be tested).  Note that in C++11 we would use
// the alias- template facility to define these templates, rather than using
// public inheritance through a dispatcher-shim.

// - - - - - - Configuration policies to instantiate HashTable with - - - - - -

template <class ELEMENT>
struct BasicKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static const KeyType& extractKey(const ValueType& value)
        // Return the specified 'value'.
    {
        return value;
    }
};

template <class ELEMENT>
struct ModifiableKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.  It also allows for functors that expect to take
    // their argument by a reference to non-'const', although behavior will be
    // undefined should any such functor actually modify such an argument.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static KeyType& extractKey(ValueType& value)
        // Return the specified 'value'.
    {
        return value;
    }
};

template <class ELEMENT>
struct BsltfConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef int     KeyType;
    typedef ELEMENT ValueType;

    static const int& extractKey(const ValueType& value);
        // Return a reference to the key corresponding to the specified 'value'
        // if it were to be inserted into a 'HashTable'.
};

template <class ELEMENT>
const int& BsltfConfig<ELEMENT>::extractKey(const ValueType& value)
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

struct TrickyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef bsltf::NonEqualComparableTestType KeyType;
    typedef TestTypes::AwkwardMaplikeElement  ValueType;

    static const KeyType& extractKey(const ValueType& value)
        // Return the result of calling 'key()' on the specified 'value'.
    {
        return value.key();
    }
};

//- - - - - - - - - - - Test Driver Configuration classes - - - - - - - - - - -
//- - - - - - - (providing coverage of test-parameter concerns) - - - - - - - -

// Configurations concerned with the 'KEY_CONFIG' template parameter

template <class ELEMENT>
struct TestDriver_BasicConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Basic configuration that acts like a multi-set of 'ELEMENT' values.
    // This is the most basic configuration that test the overwhelmingly common
    // case usage.
};

template <class ELEMENT>
struct TestDriver_BsltfConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BsltfConfig<ELEMENT>
                     , ::bsl::hash<int>
                     , ::bsl::equal_to<int>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Basic configuration to test a key-type different to the value-type.
    // This is a simple attempt to deliver something approximating a map with
    // 'int' as key, where the 'int' is computed for each element.  This is the
    // simplest way to generate a configuration compatible with the 'bsltf'
    // template testing framework used above, without rewriting each test case
    // to additional support for separate test tables of pairs to initialize a
    // more traditional-style map, with different math for computed values and
    // separate logic for duplicate elements and groups.
};

struct TestDriver_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , TestFacilityHasher<TrickyConfig::KeyType>
                     , ::bsl::equal_to<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
    // This configuration is especially tricky, as the element type itself is
    // not equality comparable, but the key-type of the element, which is the
    // sole constituent of the element, does support key-comparison (but not
    // equality comparison) through the supplied comparison functor.
};

// Configurations concerned with the 'HASHER' and 'COMPARATOR' parameters
//
// Function-like types
// - - - - - - - - - -
// A function-like type used to instantiate the 'HashTable' template may be:
//: o A function type
//: o A function pointer type
//: o An object type convertible to a function pointer
//: o An object type convertible to a function reference
//: o An object type that overloads 'operator()'
//: o A reference to a function
//: o A reference to a function pointer
//: o A reference to an object type convertible to a function pointer
//: o A reference to an object type convertible to a function reference
//: o A reference to an object type that overloads 'operator()'
//
// For each function-like type, the arguments to the function call me be:
//: o Passed by 'const &'
//: o Passed by 'const &' to something convertible from the desired argument
//: o Passed by const-unqualified '&' (subject to constraints below)
//: o Passed by value
//: o Passed by value convertible from the desired argument
//
// In each case, the callable signature may have a trailing ellipsis.
//
// For function, function-pointer, and reference-to-function types, the
// underlying function type may also have 'extern "C"' linkage.
//
// For an object-type overloading 'operator()', there are additional concerns:
//: o The operator might be 'const' qualified
//: o The operator might not be 'const' qualified
//: o The operator may be a virtual function
//: o The operator may be a function template
//: o The object-type may be a class template
//: o THe object-type may be a union (THIS IS NOT SUPPORTED)
//
// For an object-type overloading 'operator()'
//: o The type may be const-qualified
//: o The object may have internal state
//: o The object may allocate memory for state from its own allocator
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
//: o default constructor
//: o copy constructor
//: o operator=
//: o operator&
//: o operator,
//: o operator new
//: o operator delete
//
// QoI: Where the function-like entity is an empty class type, it should not
//      add to the overall size of the 'HashTable' object.
//
// We note, with mild relief, that the one thing we are not required to support
// is a pointer-to-member-function.
//
// HASHER
//- - - -
// 'HASHER' is a function-like type that takes a single argument convertible
// from 'typename KEY_CONFIG::KeyType' and returns a value of the type
// 'std::size_t'.
//
// COMPARATOR
//- - - - - -
// 'COMPARATOR' is a function-like type that takes two arguments, each
// convertible from 'typename KEY_CONFIG::KeyType', and returns a value of a
// type that is implicitly convertible to 'bool'.

template <class ELEMENT>
struct TestDriver_StatefulConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , ThrowingHashFunctor<ELEMENT>
                     , ThrowingEqualityComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Simple configuration to test simple stateful functors for the hasher and
    // comparator policies.  Both functors support an 'id' that provides state
    // that is not actually used when evaluating the function call operator,
    // and track the total number of calls to the function call operator.  An
    // additional member may be used to enable testing of exception safety, by
    // throwing exceptions from the function call operator every 'N'th time it
    // is called.  This extra state is declared as 'mutable' so that, as a
    // particularly unusual case, the function to change this state can be
    // declared 'const' to better facilitate testing.
};

template <class ELEMENT>
struct TestDriver_GroupedUniqueKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values of the 'ELEMENT' type to the same hash code, but maintaining a
    // unique key value for each of those cases.  This should lead to a higher
    // rate of collisions.
};

template <class ELEMENT>
struct TestDriver_GroupedSharedKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<ELEMENT, 5>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values of the 'ELEMENT' type to the same hash code, and similarly
    // arranging for those keys to compare equal to each other.  This should
    // lead to behavior similar to a 'multiset'.
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT> >
                     , bsltf::DegenerateFunctor<
                                          ThrowingEqualityComparator<ELEMENT> >
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration utilizes awkward functors, that abuse the implicitly
    // declared operations (such as the address-of and comma operators) and
    // return awkward types from predicates, rather than a simple 'bool'.
    // However, this configuration does support the 'swap' operation in order
    // to support a broad range of test cases.
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguationWithNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT>,
                                                false>
                     , bsltf::DegenerateFunctor<
                                           ThrowingEqualityComparator<ELEMENT>,
                                           false>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration utilizes awkward functors, that abuse the implicitly
    // declared operations (such as the address-of and comma operators) and
    // return awkward types from predicates, rather than a simple 'bool'.  As
    // this configuration does not support swapping of the functor types, it
    // does not support as broad a range of test cases as some of the other
    // configurations.
};

template <class ELEMENT>
struct TestDriver_ConstFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , const TestFacilityHasher<ELEMENT>
                     , const ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration employs const-qualified functors, and so cannot be
    // used to test behavior that may require modifying functors, such as the
    // assignment operator, or 'swap'.
};

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING)
template <class ELEMENT>
struct TestDriver_FunctorReferences
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT> &
                     , ::bsl::equal_to<ELEMENT> &
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with reference type for
    // the callable template parameters, which reference types that overload
    // the function call operator.  Note that this is more general than C++11
    // requires, as the hasher must be a hash function *object*.  Technically
    // we must support the comparator being a function-type or a function-
    // reference, so we test this implementation-detail component supports the
    // hasher as well.
};
#endif

template <class ELEMENT>
struct TestDriver_FunctionTypes
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(const ELEMENT&)
                     , bool(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-type
    // arguments for the hash type and key-comparison type.  Note that this is
    // more general than C++11 requires, as the hasher must be a hash function
    // *object*.  Technically we must support the comparator being a
    // function-type or a function-reference, so we test this implementation-
    // detail component supports the hasher as well.
};

template <class ELEMENT>
struct TestDriver_FunctionPointers
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(*)(const ELEMENT&)
                     , bool(*)(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-pointer type
    // arguments for the hash type and key-comparison type.  Note that this
    // corresponds to testing a stateful functor, with the awkward case that
    // the default value for the callable types is known to produce undefined
    // behavior (null pointer values for the function pointers).
};

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
template <class ELEMENT>
struct TestDriver_FunctionReferences
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(&)(const ELEMENT&)
                     , bool(&)(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-reference
    // arguments for the hash type and key-comparison type.  Note that this is
    // more general than C++11 requires, as the hasher must be a hash function
    // *object*.  Technically we must support the comparator being a
    // function-type or a function-reference, so we test this implementation-
    // detail component supports the hasher as well.
};
#endif

template <class ELEMENT>
struct TestDriver_ConvertibleValueConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestConvertibleValueHasher<ELEMENT>
                     , TestConvertibleValueComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration tests functors whose arguments are convertible from
    // the key-type of the configuration.
};

template <class ELEMENT>
struct TestDriver_GenericFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration tests functors whose function-call operator is a
    // function template that will deduce type as appropriate from the
    // arguments supplied to the function call.
};

template <class ELEMENT>
struct TestDriver_ModifiableFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< ModifiableKeyConfig<ELEMENT>
                     , ModifiableHasher<ELEMENT>
                     , ModifiableComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration tests functors whose arguments to the function-call
    // operator are passed by non-'const' reference, but honor the spirit of
    // the rule by not actually making any modifications to those arguments.
    // This is something we test only because standard conformance requires it.
};

// Configurations concerned with the 'ALLOCATOR' template parameter

// ALLOCATOR
// - - - - -
// The 'ALLOCATOR' argument must conform to the C++11 allocator requirements,
// and a minimal allocator would have (almost) all of its properties deduced by
// 'std::allocator_traits'.  Therefore, allocators that do or do not supply
// each of the following should be supported, and behave as specified by
// 'std::allocator_traits' (noting that 'allocator_traits' is a separately
// implemented and tested component that we depend on - this is not intended to
// test 'allocator_traits', but merely that this component correctly uses
// 'allocator_traits' rather than the object allocator directly):
//: o type alias 'pointer'
//: o type alias 'const_pointer'
//: o type alias 'void_pointer'
//: o type alias 'const_void_pointer'
//: o type alias 'size_type'
//: o type alias 'difference_type'
//: o type alias 'propagate_on_container_copy_construction'
//: o type alias 'propagate_on_container_copy_assignment'
//: o type alias 'propagate_on_container_swap'
//: o alias template 'rebind<OTHER>'
//: o 'construct(ANY*, ARGS...)'
//: o 'destroy(ANY*)'
//: o 'max_size()'
//
// While an allocator must be CopyConstructible, there is no requirement that
// it support any of the other implicitly declared operations:
//: o default constructor
//: o operator=
//: o operator&
//: o operator,
//: o operator new
//: o operator delete
// Note that some specific functions, such as the 'HashTable' default
// constructor, may require some of these extra functions, but there is no
// general requirement to supply these unless a specific function contract
// explicitly states that this operation is required, and that additional
// requirement applies to only that one function.
//
// When the allocator is 'bsl::allocator<TYPE>' then we have a number of
// additional concerns, that are mostly runtime issues rather than type-
// related issues.  They will be raised and addressed in the usual manner under
// the actual test case.

template <class ELEMENT>
struct TestDriver_StdAllocatorConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , bsl::equal_to<ELEMENT>
                     , bsltf::StdTestAllocator<ELEMENT>
                     >
       > {
    // This configuration uses a stateless std-conforming allocator, rather
    // than using the BDE allocator model.  To enhance test coverage of the
    // functor, we mix the generic hasher with 'bsl::equal_to', the default
    // comparator.  Note that all of these parameter types are empty, so we
    // expect to see the largest benefit from empty-base optimizations.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT,
                                                   true,
                                                   false,
                                                   false,
                                                   false>
                     >
       > {
    // This configuration uses a std-conforming allocator with per-object state
    // rather than using the BDE allocator model, and uses a different
    // allocator propagation strategy.  Allocators will propagate on copy
    // construction, but on no other operation.  We pick another mix of hasher
    // and comparator that were previously paired differently when testing
    // callable-parameter concerns.
};

#if defined(BSLSTL_ALLOCATOR_TRAITS_COMPUTES_ALLOCTOR_PROPAGATION)
// bsl::allocator_traits does not yet detect and support the allocator
// propagation predicate traits; these test configurations will be fully
// specced out once support is available

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation1
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT>
                     >
       > {
    // Propagate all allocator operations.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation2
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT, false>
                     >
       > {
    // Propagate all allocator operations but copy construction.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation3
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT, true, false>
                     >
       > {
    // Propagate all allocator operations but swap.
};
#endif

// - - - - - Main test driver harness, that implements the test cases - - - - -

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

    typedef bsltf::TestValuesArray<ValueType> TestValues;

    typedef ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR> ObjMaker;

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

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static int ggg(Obj *object, const char *spec, bool verbose = true);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify 'verbose' as false to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Returned
        // values lower than '-1' indicate some error other than parsing the
        // 'spec' string.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

  public:
    // TEST CASES

    static void testCase11();
        //      Test case 12 is not supported
    static void testCase13();
        // Run the test case with the corresponding case number for
        // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.

    static void testCaseReserveForNumElements();
        // Formerly 'testCase12'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::gg(Obj        *object,
                                                          const char *spec)
{
    int result = ggg(object, spec);
    ASSERTV(result, -1 == result);
    return *object;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
int TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ggg(
                                                           Obj        *object,
                                                           const char *spec,
                                                           bool        verbose)
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
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;   // Discontinue processing this spec.          // RETURN
        }

        if (!insertElement(object, VALUES[spec[i] - 'A'])) {
            if (verbose) {
                printf("Unexpected error inserting element no. %d from the "
                       "spec string ('%s').\n", i, spec);
            }
            return -100 -i;   // Discontinue processing this spec.    // RETURN
        }
   }
   return SUCCESS;
}

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -
bool exchange(bool &value, bool newValue)
    // Return the original specified 'value', and set 'value' to the specified
    // 'newValue'.  Note that this function is a specific implementation of a
    // standard C++11 template, intended to eliminate use of the deprecated
    // post-increment operator on 'bool'.
{
    bool result = value;
    value = newValue;
    return result;
}

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING 'rehashForNumBuckets'
    //
    // Concerns:
    //: 1 'rehashForNumBuckets' allocates at least the specified number of
    //:   buckets.
    //:
    //: 2 'rehashForNumBuckets' does not affect the value of the object.
    //:
    //: 3 'rehashForNumBuckets' does not affect the order of the inserted
    //:   elements with the same value.
    //:
    //: 4 'rehashForNumBuckets' is a no-op if the requested number of buckets
    //:   is less than the current 'numBuckets' in the object.
    //:
    //: 5 Any memory allocation is from the object allocator.
    //:
    //: 6 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 7 'rehashForNumBuckets' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //: 8 'rehashForNumBuckets' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   rehashForNumBuckets(SizeType newNumBuckets);
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

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

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

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
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_REHASH_SIZE; ++tk) {
                bslma::TestAllocator  oa("object",  veryVeryVeryVerbose);
                ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&oa);

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

#if defined BDE_BUILD_TARGET_EXC
        // The following set of tests are expected, at least in some test
        // configurations, to fail by throwing exceptions.
        if (verbose) printf("Testing behavior at extremities.\n");
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);
            try {
                mX.rehashForNumBuckets(std::numeric_limits<size_t>::max());
                ASSERT(!"rehash(max size_t) should throw a 'logic_error'");
            }
            catch (const std::logic_error &) {
                // This is the expected code path
            }
            catch (...) {
                ASSERT(!"rehash(max size_t) threw the wrong exception type");
            }
        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1e30f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1e-6f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1.0f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1.0f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
#endif
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
                                                testCaseReserveForNumElements()
{
    // ------------------------------------------------------------------------
    // TESTING 'reserveForNumElements'
    //
    // Concerns:
    //: 1 'reserveForNumElements' allocates sufficient buckets so that, after
    //:   the rehash, 'numBuckets() / maxLoadFactor()' >= the specified number
    //:   of elements.
    //:
    //: 2 'reserveForNumElements' does not affect the value of the object.
    //:
    //: 3 'reserveForNumElements' does not affect the order of the inserted
    //:   elements with the same value.
    //:
    //: 4 'reserveForNumElements' is a no-op if the requested number of
    //:   elements can already be accommodated without exceeding the
    //:   'maxLoadFactor' of the object.
    //:
    //: 5 Any memory allocation is from the object allocator.
    //:
    //: 6 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 7 'reserveForNumElements' provides the strong exception guarantee if
    //:   the hasher does not throw.
    //:
    //: 8 'reserveForNumElements' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   reserveForNumElements(SizeType numElements);
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

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

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

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
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_RESERVE_SIZE; ++tk) {
                bslma::TestAllocator reserve("reserveAlloc",
                                             veryVeryVeryVerbose);
                bslma::TestAllocator noReserve("noReserveAlloc",
                                               veryVeryVeryVerbose);

                ALLOCATOR reserveAlloc = MakeAllocator<ALLOCATOR>::make(
                                                                     &reserve);
                ALLOCATOR noReserveAlloc = MakeAllocator<ALLOCATOR>::make(
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
                // as inserting 'X' elements one at a time
                ASSERTV(LINE, tk, RESERVE_ELEMENTS, reserve.numBytesTotal() <=
                                                    noReserve.numBytesTotal());
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'insert' METHODS
    //
    // Concerns:
    //: 1 ...
    //:
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //   insert(const SOURCE_TYPE& obj);
    //   insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting is not yet implemented.\n");
    }


    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    const TestValues VALUES;  // contains 52 distinct increasing values


    // Probably want to pick these up as values from some injected policy, so
    // that we can test with stateful variants
    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    const size_t MAX_LENGTH = 9;

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            {
            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting bootstrap constructor.\n");
            }

            Obj                  *objPtr;

            //const size_t NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            ALLOCATOR expAllocator = ObjMaker::makeObject( &objPtr
                                                         , CONFIG
                                                         , &fa
                                                         , &sa
                                                         , HASH
                                                         , COMPARE
                                                         , 0
                                                         , MAX_LF);
            bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                       &fa);
            Obj&                   mX = *objPtr;  const Obj& X = mX;

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

            if (veryVerbose) {
                printf("\n\tTesting 'insert(element)'.\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

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
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch1("scratch1", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch1);

                    // This will fail on the initial insert as we must also
                    // create the bucket array, so there is an extra pass.  Not
                    // sure why that means the block counts get out of sync
                    // though, is this catching a real bug?
#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This does
                    // not allow for rehashes as the container grows.
#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    if (VALUE_TYPE_USES_ALLOCATOR  ||
                                                expectPoolToAllocate(LENGTH)) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }
#endif

                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    //
                    // !!! BROKEN TEST CONDITION !!!
                    //
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif

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
                                                           COMPARE,
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

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
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

            if (veryVerbose) { printf(
                "\n\tRepeat testing 'insert(element)', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

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
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
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
                    // as a memory use error.  'LENGTH' was the high-water mark
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

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
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

#if defined BDE_BUILD_TARGET_EXC
    {
        Obj mR(HASH,
               COMPARE,
               0,
               native_std::numeric_limits<float>::denorm_min());
        try {
            mR.insert(VALUES[0]);

            P(mR.numBuckets())
            P(mR.size());

            ASSERT(false);
        }
        catch(const native_std::length_error& e) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!"The wrong exception type was thrown.");
        }
    }
#endif  // BDE_BUILD_TARGET_EXC
}

//=============================================================================
//                      TEST CASE DISPATCH FUNCTIONS
//-----------------------------------------------------------------------------
// In order to reduce the complexity of the 'switch' statement in 'main' that
// was getting to large for some compilers to handle, we have migrated the
// invocation of each test case into its own function.
//-----------------------------------------------------------------------------

static
void mainTestCase11()
    // --------------------------------------------------------------------
    // TESTING 'rehashForNumBuckets'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE11_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTESTING 'rehashForNumBuckets'"
                        "\n=============================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE11_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE11_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#if 0
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE11_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase11();
}

static
void mainTestCaseReserveForNumElementsPart2()
{
    if (verbose) printf("\nTESTING 'reserveForNumElements' Part 2"
                        "\n===============================n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCaseReserveForNumElements,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED);
}

static
void mainTestCaseReserveForNumElementsPart1()
{
    if (verbose) printf("\nTESTING 'reserveForNumElements' Part 1"
                        "\n===============================n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCaseReserveForNumElements,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                  bsltf::NonAssignableTestType,
                  bsltf::NonDefaultConstructibleTestType);
}

static
void mainTestCase13()
    // --------------------------------------------------------------------
    // TESTING 'insert'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE13_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting 'insert'"
                        "\n================\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE13_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE13_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE13_TYPES
}

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

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TP05 // Test doc is in delegated functions
// BDE_VERIFY pragma: -TP17 // No test-banners in a delegating switch statement
    switch (test) { case 0:
      case 16:  // falls through
      case 15:  // falls through
      case 14: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY 'bslstl_hashtable_test2'"
                   "\n===================================================\n",
                   test);
      } break;
      case 13: { mainTestCase13(); } break;
      case 12: {
        mainTestCaseReserveForNumElementsPart2();
        mainTestCaseReserveForNumElementsPart1();
      } break;
      case 11: { mainTestCase11(); } break;
      case 10:  // falls through
      case  9:  // falls through
      case  8:  // falls through
      case  7:  // falls through
      case  6:  // falls through
      case  5:  // falls through
      case  4:  // falls through
      case  3:  // falls through
      case  2:  // falls through
      case  1: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY PRIMARY TEST DRIVER"
                   "\n==============================================\n",
                   test);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
// BDE_VERIFY pragma: pop

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
