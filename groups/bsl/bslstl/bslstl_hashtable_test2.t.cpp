// bslstl_hashtable_test2.t.cpp                                       -*-C++-*-
#include <bslstl_hashtable_test2.h>

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
                       // ====================
                       // class IsBslAllocator
                       // ====================

template <class TYPE>
struct IsBslAllocator : bsl::false_type {};
template <class TYPE>
struct IsBslAllocator<bsl::allocator<TYPE> > : bsl::true_type {};
    // This simple traits class reports 'true' if the (template parameter)
    // 'TYPE' is an instantiation of the 'bsl::allocator' template, and 'false'
    // otherwise.

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//       test support functions dealing with hash and comparator functors

template <class COMPARATOR>
bool isEqualComparator(const COMPARATOR&, const COMPARATOR&);
    // Return 'true'.  This function template provides a common signature that
    // may be overloaded for specific hasher types that can support the idea of
    // setting a state value.  It is assumed that any comparator that does not
    // overload this functor is stateless, and so equivalent to any other
    // comparator.

template <class RESULT, class ARG1, class ARG2>
bool isEqualComparator(RESULT (*const &a)(ARG1, ARG2),
                       RESULT (*const &b)(ARG1, ARG2));
    // Return 'true' if the specified addresses 'a' and 'b' are the same, and
    // 'false' otherwise.  Note that we pass pointers by 'const &' in order to
    // avoid ambiguities on the IBM XLC compiler.

template <class KEY>
bool isEqualComparator(const ThrowingEqualityComparator<KEY>& lhs,
                       const ThrowingEqualityComparator<KEY>& rhs);
    // Provide an overloaded function to compare comparators.  Return 'true' if
    // the specified 'lhs' compares equal to the specified 'rhs' under the
    // expression 'lhs == rhs'.  This template will not instantiate if the
    // expression does not compile.

template <class HASHER>
bool isEqualHasher(const HASHER&, const HASHER&);
    // Return 'true'.  This function template provides a common signature that
    // may be overloaded for specific hasher types that can support the idea of
    // setting a state value.

template <class RESULT, class ARGUMENT>
bool isEqualHasher(RESULT (*const &a)(ARGUMENT),
                   RESULT (*const &b)(ARGUMENT));
    // Return 'true' if the specified addresses 'a' and 'b' are the same, and
    // 'false' otherwise.  Note that we pass pointers by 'const &' in order to
    // avoid ambiguities on the IBM XLC compiler.


bool isEqualHasher(const ThrowingHashFunctor<int>& lhs,
                   const ThrowingHashFunctor<int>& rhs);
    // Provide an overloaded function to compare hash functors.  Return 'true'
    // if the specified 'lhs' compares equal to the specified 'rhs' under the
    // expression 'lhs == rhs'.  This template will not instantiate if the
    // expression does not compile.

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class COMPARATOR>
inline
bool isEqualComparator(const COMPARATOR&, const COMPARATOR&)
{
    return true;
}

template <class RESULT, class ARG1, class ARG2>
inline
bool isEqualComparator(RESULT (*const &a)(ARG1, ARG2),
                       RESULT (*const &b)(ARG1, ARG2))
{
    return a == b;
}

template <class KEY>
inline
bool isEqualComparator(const ThrowingEqualityComparator<KEY>& lhs,
                       const ThrowingEqualityComparator<KEY>& rhs)
{
    return lhs == rhs;
}

template <class HASHER>
inline
bool isEqualHasher(const HASHER&, const HASHER&)
{
    return true;
}

template <class RESULT, class ARGUMENT>
inline
bool isEqualHasher(RESULT (*const &a)(ARGUMENT),
                   RESULT (*const &b)(ARGUMENT))
{
    return a == b;
}

inline
bool isEqualHasher(const ThrowingHashFunctor<int>& lhs,
                   const ThrowingHashFunctor<int>& rhs)
{
    return lhs == rhs;
}

#if !defined(BSLS_PLATFORM_CMP_CLANG)
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

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
    static void testCase14() { CONFIGURED_DRIVER::testCase14(); }
    static void testCase15() { CONFIGURED_DRIVER::testCase15(); }
    static void testCase16() { CONFIGURED_DRIVER::testCase16(); }
    static void testCase17() { CONFIGURED_DRIVER::testCase17(); }
    static void testCase18() { CONFIGURED_DRIVER::testCase18(); }
        // Run the test case with the matching number from the supplied
        // (template parameter) type 'CONFIGURED_DRIVER'.
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
    // their argument by a reference to non-const, although behavior will be
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

// - - - - - Special configurations for testing default constructor - - - - - -

// These configurations are required only for a special set of tests using the
// default constructor for HashTable, which can be instantiated for hasher and
// comparator types that are default constructible, but awkward in every other
// way.  This will be the only test driver to examine the limits of default
// constructible (only) types.

template <class ELEMENT>
struct TestDriver_DefaultOnlyFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , DefaultOnlyHasher<ELEMENT>
                     , DefaultOnlyComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

struct TestDriver_AwkwardMaplikeForDefault
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , DefaultOnlyHasher<TrickyConfig::KeyType>
                     , DefaultOnlyComparator<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};

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

  public:
    // TEST CASES
    static void testCase14();
    static void testCase15();
    static void testCase16();
        // Run the test case with the corresponding case number for
        // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.
};

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING setMaxLoadFactor METHOD
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
    //*  setMaxLoadFactor
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting 'setMaxLoadFactor'.\n");
    }

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    if (verbose) printf("Testing exteme values.\n");
    {

        Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);  const Obj& X = mX;

        mX.setMaxLoadFactor(std::numeric_limits<float>::max());
        ASSERT(std::numeric_limits<float>::max() == X.maxLoadFactor());

        mX.setMaxLoadFactor(std::numeric_limits<float>::infinity());
        ASSERT(std::numeric_limits<float>::infinity() == X.maxLoadFactor());
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose) printf("Testing exceptional trigger values.\n");
    {
        Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);
        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::min());
            ASSERT(!"setMaxLoadFactor(min) should throw a 'logic_error'");
        }
        catch (const std::logic_error &) {
            // This is the expected code path
        }
        catch (...) {
            ASSERT(!"rehash(max size_t) threw the wrong exception type");
        }

        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::denorm_min());
            ASSERT(!"setMaxLoadFactor(denorm) should throw a 'logic_error'");
        }
        catch (const std::logic_error &) {
            // This is the expected code path
        }
        catch (...) {
            ASSERT(!"rehash(max size_t) threw the wrong exception type");
        }
    }
#endif

    if (verbose) printf("Negative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        static const float FLT_TINY = std::numeric_limits<float>::denorm_min();
        static const float FLT_NAN  = std::numeric_limits<float>::quiet_NaN();
        static const float FLT_INF  = std::numeric_limits<float>::infinity();

        (void) FLT_TINY;
        (void) FLT_NAN;

        Obj mX(HASH, COMPARE, 0, 1.0f, scratchAlloc);

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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING DEFAULT CONSTRUCTOR:
    //   The default constructor creates an empty container having one bucket.
    //   The state of a default constructed HashTable is that of a
    //   value-constructed HashTable requesting zero initial buckets, a maximum
    //   load factor of '1.0', and default values for the functors.  This state
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
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   HashTable(const A& allocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting with various allocator configurations.\n");

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

        if (veryVerbose) {
            printf("\n\tTesting default constructor.\n");
        }

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

//        ASSERTV(CONFIG, isEqualComparator(COMPARATOR(), X.comparator()));
//        ASSERTV(CONFIG, isEqualHasher(HASHER(), X.hasher()));

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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING traits and other compile-time properties
    //
    // Concerns:
    //: 1 HashTable supports the expected traits
    //:
    //: 2 HashTable has the expected size
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //*  traits
    //*  sizeof(HashTable)
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting bsl traits.\n");
    }

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;


    ASSERT((bsl::is_same<typename Obj::KeyType,   KeyType>::value));
    ASSERT((bsl::is_same<typename Obj::ValueType, ValueType>::value));

    // These traits should never be true

    ASSERT(!bsl::is_trivially_copyable<             Obj>::value);
    ASSERT(!bsl::is_trivially_default_constructible<Obj>::value);
    ASSERT(!bslmf::IsBitwiseEqualityComparable<     Obj>::value);

    // The uses-allocator trait should depend only on the allocator type.

    bool usesBslAllocator        = IsBslAllocator<ALLOCATOR>::value;
    bool objUsesBslmaAllocator   = bslma::UsesBslmaAllocator<Obj>::value;

    ASSERTV(usesBslAllocator, objUsesBslmaAllocator,
            usesBslAllocator == objUsesBslmaAllocator);

    // The stored type has no bearing on whether a node-based container is
    // bitwise-moveable.  However, the allocator and two custom function-like
    // types must all be bitwise-moveable for the whole container to be
    // bitwise-moveable.

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

//=============================================================================
//                      TEST CASE DISPATCH FUNCTIONS
//-----------------------------------------------------------------------------
// In order to reduce the complexity of the 'switch' statement in 'main' that
// was getting to large for some compilers to handle, we have migrated the
// invocation of each test case into its own function.
//-----------------------------------------------------------------------------

static
void mainTestCase14()
    // --------------------------------------------------------------------
    //  TESTING 'setMaxLoadFactor'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE14_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting 'setMaxLoadFactor'"
                        "\n==========================\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE14_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE14_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    // We need to limit the test coverage on IBM as the compiler cannot cope
    // with so many template instantiations.

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE14_TYPES

    // Remaining special cases
    TestDriver_AwkwardMaplike::testCase14();
}

static
void mainTestCase15()
    // --------------------------------------------------------------------
    // DEFAULT CONSTRUCTOR
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE15_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting default constructor"
                        "\n===========================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting non-copyable functors"
                        "\n-----------------------------\n");
    // This is the use case we are most concerned about.

    // We probably want to test with a smattering of the following concerns as
    // well, notably with the different allocator patterns.
    RUN_EACH_TYPE(TestDriver_DefaultOnlyFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE15_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE15_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if 0
    // These configurations not available, as functors are not default
    // constructible.

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

#if 0
    // Function pointers do not give usable defaults for functors.

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if 0
    // The stateless allocator flags issues installing the chosen allocator
    // when it is not the default.

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE15_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase15();
    TestDriver_AwkwardMaplikeForDefault::testCase15();
}

static
void mainTestCase16()
    // --------------------------------------------------------------------
    // TESTING traits and other compile-time properties
    // --------------------------------------------------------------------
    // For this test, the standard "minimal" set of test types provides
    // adequate coverage for the different properties dependent on the value
    // type.  We must simply test sufficient pre-packaged configurations to
    // cover the possibilities of the hash functor, comparator and allocator
    // too.  As of this writing, there is no package that separately isolates
    // bitwise copyable hasher and comparator, to test as separate dimensions.
{
    if (verbose) printf("\nTesting basic configuration"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING)
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);
#endif

    if (verbose) printf("\nTesting function pointers"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting function references"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);
#endif

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase16();
}

#if 0  // Planned test cases, not yet implemented
static
void mainTestCase16()
    // --------------------------------------------------------------------
    // TESTING 'setMaxLoadFactor'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase16,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase16,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase17()
    // --------------------------------------------------------------------
    // TESTING 'insert'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase17,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase17,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase18()
    // --------------------------------------------------------------------
    // TESTING 'insertIfMissing(bool *, VALUE)'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase18,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase18,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase19()
    // --------------------------------------------------------------------
    // TESTING 'insertIfMissing(const KeyType& key);
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase19,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase19,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase20()
    // --------------------------------------------------------------------
    // TESTING 'find'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase20,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase20,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase21()
    // --------------------------------------------------------------------
    // TESTING 'findRange'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase21,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase21,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase22()
    // --------------------------------------------------------------------
    // TESTING 'countElementsInBucket'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase22,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase22,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase23()
    // --------------------------------------------------------------------
    // TESTING "max" FUNCTIONS
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase23,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase23,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase24()
    // --------------------------------------------------------------------
    // TESTING PUBLIC TYPEDEFS
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase24,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase24,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

#endif

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
      case 16: { mainTestCase16(); } break;
      case 15: { mainTestCase15(); } break;
      case 14: { mainTestCase14(); } break;
      case 13:  // falls through
      case 12:  // falls through
      case 11: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY 'bslstl_hashtable_test1'"
                   "\n===================================================\n",
                   test);
      } break;
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
