// bdlcc_stripedunorderedmap.t.cpp                                    -*-C++-*-

#include <bdlcc_stripedunorderedmap.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bdlb_random.h>
#include <bdlb_randomdevice.h>

#include <bslim_testutil.h>
#include <bslmt_threadutil.h>
#include <bslmt_semaphore.h>
#include <bslmt_throughputbenchmark.h>
#include <bslmt_throughputbenchmarkresult.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsltf_allocbitwisemoveabletesttype.h>
#include <bsltf_alloctesttype.h>
#include <bsltf_bitwisecopyabletesttype.h>
#include <bsltf_bitwisemoveabletesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_nontypicaloverloadstesttype.h>
#include <bsltf_simpletesttype.h>
#include <bsltf_streamutil.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_uniontesttype.h>

#include <bslmf_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_nameof.h>
#include <bsls_performancehint.h>
#include <bsls_timeutil.h>  // 'HashPerformance'
#include <bsls_types.h>     // 'BloombergLP::bsls::Types::Int64'

#include <bsl_algorithm.h>  // sort
#include <bsl_climits.h>    // INT_MAX
#include <bsl_cstdlib.h>    // 'atoi', 'rand'
#include <bsl_cmath.h>      // 'sqrt'
#include <bsl_cstdio.h>     // 'sprintf'
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>     // allocate_shared
#include <bsl_ostream.h>    // operator<<
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_unordered_map.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a fully thread safe container template,
// 'bdlcc::StripedUnorderedMap', that provides a concurrent striped hash map.
// By design this container does not implement equality comparison, an
// assignment operator, copy constructor, or even a 'print' method.  Thus, it
// is classified as an *irregular* value-semantic type, even if its 'KEY' and
// 'VALUE' types are are VSTs and, test drivers of other (value-semantic)
// containers are of limited use here.  This test driver will *not* be provide
// the canonical (first) ten test cases typically used for VSTs.  Still, the
// fundamental principles of test case ordering will, of course, be followed.
//
// This fully thread-safe container template should be able to handle any type
// that is acceptable to the standard containers 'bsl::unorderedmap'.
// Consequently, our tests be run for the same set of types used in our test of
// those standard templates: primarily the types defined in the
// 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' macro and other types in
// special cases.
//
// Single-threaded behavior is tested in test cases [1 .. 17].  Multi-threaded
// issues are addressed in test cases 18.
//
// As this component simply forwards its methods to
// 'bdlcc:StripedUnorderedImpl', we simply need to test that the various
// methods are performing the same as the corresponding ones in
// 'bdlcc:StripedUnorderedImpl'.  Hence, for each method we simply perform the
// relevant portion of the test in 'bdlcc:StripedUnorderedImpl'.  Specifically,
// the portions pertaining to duplicate keys are omitted.  The portions
// including parameters 'e_ALL' and 'e_INSERT_ALWAYS' are removed.
//
// Negatively numbered (manually run) performance tests are also implemented,
// as well as the usage examples.
//
// Global Concerns:
//: o All allocations from the intended allocator.
//: o The global allocator is never used.
//: o There are no temporary allocations.
//: o All memory is returned on destruction.
//: o Rehash is started whenever necessary.
//: o Critical sections are appropriately locked (i.e., read vs. write lock).
//: o Exceptions leave the hash map in an unlocked state.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] StripedUnorderedMap(numInitialBuckets, numStripes, *basicAllocator);
// [ 2] ~StripedUnorderedMap();
//
// MANIPULATORS
// [ 8] void clear();
// [14] void disableRehash();
// [14] void enableRehash();
// [ 6] bsl::size_t erase(const KEY& key);
// [ 7] bsl::size_t eraseBulk(RANDOMIT first, last);
// [ 3] bsl::size_t insert(const KEY& key, const VALUE& value);
// [17] bsl::size_t insert(const KEY& key, VALUE&& value);
// [ 9] bsl::size_t insertBulk(RANDOMIT first, last);
// [14] void maxLoadFactor(float newMaxLoadFactor);
// [14] void rehash(bsl::size_t numBuckets);
// [11] int setComputedValue(const KEY& key, visitor);
// [10] bsl::size_t setValue(const KEY& key, const VALUE& value);
// [16] bsl::size_t setValue(const KEY& key, VALUE&& value);
// [12] int update(const KEY& key, const VisitorFunction& visitor);
// [13] int visit(const VisitorFunction& visitor);
//
// ACCESSORS
// [ 4] bsl::size_t bucketIndex(const KEY& key) const;
// [ 4] bsl::size_t bucketCount() const;
// [ 4] bsl::size_t bucketSize(bsl::size_t index) const;
// [ 4] bool empty() const;
// [ 4] EQUAL equalFunction() const;
// [ 5] bsl::size_t getValue(VALUE *value, const KEY& key) const;
// [ 4] HASH hashFunction() const;
// [14] bool isRehashEnabled() const;
// [14] float loadFactor() const;
// [14] float maxLoadFactor() const;
// [ 4] bsl::size_t numStripes() const;
// [ 4] bsl::size_t size() const;
//
// [ 4] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] TYPE TRAITS
// [18] MULTI-THREADED STRESS TEST
// [19] USAGE EXAMPLE
// [-1] PERFORMANCE TEST INT->STRING
// [-2] PERFORMANCE TEST STRING->INT64
// [-4] READ WRITE PERFORMANCE
// [-8] READ/WRITE PERFORMANCE TEST WITH LONG KEY

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE      BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// This macro is similar to BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED,
// except that bsltf::NonTypicalOverloadsTestType is removed, as we do want to
// use operator &.
#define TEST_TYPES_USER_DEFINED                    \
    bsltf::EnumeratedTestType::Enum,                                          \
    bsltf::UnionTestType,                                                     \
    bsltf::SimpleTestType,                                                    \
    bsltf::AllocTestType,                                                     \
    bsltf::BitwiseCopyableTestType,                                           \
    bsltf::BitwiseMoveableTestType,                                           \
    bsltf::AllocBitwiseMoveableTestType,                                      \
    bsltf::MovableTestType,                                                   \
    bsltf::MovableAllocTestType

    // This macro is equivalent to
    // BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, except that
    // bsltf::NonTypicalOverloadsTestType is removed.
#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,   \
        TEST_TYPES_USER_DEFINED

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

bool             verbose;
bool         veryVerbose;
bool     veryVeryVerbose;
bool veryVeryVeryVerbose;

namespace usage {

void example1()
    // Usage example 1.
{
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example shows some basic usage of 'bdlcc::StripedUnorderedMap'.
//
// First, we define a 'bdlcc::StripedUnorderedMap' object, 'myFriends', that
// maps 'int' to 'bsl::string':
//..
    bdlcc::StripedUnorderedMap<int, bsl::string> myFriends;
//..
// Notice that we are using the default value number of buckets, number of
// stripes, and allocator.
//
// Then, we insert three elements into the map and verify that the size is the
// expected value:
//..
    ASSERT(0 == myFriends.size());
    myFriends.insert(0, "Alex");
    myFriends.insert(1, "John");
    myFriends.insert(2, "Rob");
    ASSERT(3 == myFriends.size());
//..
// Next, we demonstrate 'insertBulk' by creating a vector of three key-value
// pairs and add them to the map using a single method call:
//..
    typedef bsl::pair<int, bsl::string> PairType;
    bsl::vector<PairType> insertData;
    insertData.push_back(PairType(3, "Jim"));
    insertData.push_back(PairType(4, "Jeff"));
    insertData.push_back(PairType(5, "Ian" ));
    ASSERT(3 == insertData.size())

    ASSERT(3 == myFriends.size());
    myFriends.insertBulk(insertData.begin(), insertData.end());
    ASSERT(6 == myFriends.size());
//..
// Then, we 'getValue' method to retrieve the previously inserted string
// associated with the value 1:
//..
    bsl::string value;
    bsl::size_t rc = myFriends.getValue(&value, 1);
    ASSERT(1      == rc);
    ASSERT("John" == value);
//..
// Now, we change the value associated with 1 from "John" to "Jack" and confirm
// that the size of the map has not changed:
//..
    rc = myFriends.setValue(1, "Jack");
    ASSERT(1 == rc);
    ASSERT(6 == myFriends.size());

    rc = myFriends.getValue(&value, 1);
    ASSERT(1      == rc);
    ASSERT("Jack" == value);
//..
// Finally, we erase the element '(3, "Jim")' from the map, confirm that the
// map size is decremented, and that element can no longer be found in the map:
//..
    rc = myFriends.erase(3);
    ASSERT(1 == rc);
    ASSERT(5 == myFriends.size());

    rc = myFriends.getValue(&value, 3);
    ASSERT(0 == rc);
//..
//
}

///Example 2: Track Stats
/// - - - - - - - - - - -
// This example uses the 'setComputedValue' and 'update' methods to keep track
// of user ID usage counts (stats).  A striped unordered map has the user ID as
// the key, and the count as the value.  There are 2 functors, one used to
// increase the count (and set it to '1' if the user ID has not been referenced
// yet), and the other is used to decrease the count.
//..
    typedef bdlcc::StripedUnorderedMap<int, int> StatsMap;
//..
//
// First, define a functor, 'IncFunctor', that has parameters corresponding to
// the 'KEY' and 'VALUE' types of 'StatsMap' and, when invoked, adds 1 to
// whatever existing value is associated with the given 'KEY' value, or sets a
// newly added element to 1:
//..
    struct IncFunctor {
        bool operator()(int        *value,  // 'VALUE *'
                        const int&)         // 'const KEY&'
        {
            *value += 1;
            return true;
        }
    };
//..
//
// Next, define a functor, 'DecFunctor', that has parameters corresponding to
// the 'KEY' and 'VALUE' types of 'StatsMap' and, when invoked, subtracts 1
// from whatever existing value is associated with the given 'KEY' value:
//..
        struct DecFunctor {
            bool operator()(int        *value,  // 'VALUE *'
                            const int&  )       // 'const KEY&'
            {
                *value -= 1;
                return true;
            }
        };
//..

void example2()
    // Usage example 2 main function.
{

// Then, create 'myStats', a 'StatsMap' object with (as we did in {Example 1}
// default number of buckets, number of stripes, and allocator:
//..
    StatsMap myStats;
//..
// Next, instantiate 'myIncFunctor' and 'myDecFunctor' from 'IncFunctor' and
// 'DecFunctor', respectively:
//..
    IncFunctor myIncFunctor;
    DecFunctor myDecFunctor;
//..
// Next, increase count for three user IDs:
//..
    ASSERT(0 == myStats.size());
    int rc1 = myStats.setComputedValue(1001, myIncFunctor);
    ASSERT(0 == rc1);
    rc1 = myStats.setComputedValue(1002, myIncFunctor);
    ASSERT(0 == rc1);
    rc1 = myStats.setComputedValue(1003, myIncFunctor);
    ASSERT(0 == rc1);
    ASSERT(3 == myStats.size());
    int         value = 0;
    bsl::size_t rc = myStats.getValue(&value, 1001);
    ASSERT(1 == rc);
    ASSERT(1 == value);
//..
// Now, increase count for existing user IDs.  Confirm that the values have
// been updated as expected.
//..
    rc1 = myStats.setComputedValue(1001, myIncFunctor);
    ASSERT(1 == rc1);
    rc1 = myStats.setComputedValue(1002, myIncFunctor);
    ASSERT(1 == rc1);
    rc1 = myStats.setComputedValue(1001, myIncFunctor);
    ASSERT(1 == rc1);
    ASSERT(3 == myStats.size());
    rc = myStats.getValue(&value, 1001);
    ASSERT(1 == rc);
    ASSERT(3 == value);
    rc = myStats.getValue(&value, 1002);
    ASSERT(1 == rc);
    ASSERT(2 == value);
//..
// Finally decrease count for existing user IDs.  Confirm that the values have
//  been updated as expected.
//..
    int ret = myStats.update(1001, myDecFunctor);
    ASSERT(1 == ret);
    ret = myStats.update(1003, myDecFunctor);
    ASSERT(1 == ret);
    ASSERT(3 == myStats.size());
    rc = myStats.getValue(&value, 1001);
    ASSERT(1 == rc);
    ASSERT(2 == value);
    rc = myStats.getValue(&value, 1003);
    ASSERT(1 == rc);
    ASSERT(0 == value);
//..
//
}

///Example 3: Visiting all the Container Elements
/// - - - - - - - - - - - - - - - - - - - - - - -
// This example uses the 'visit' method to apply a transformation (as defined
// by a functor) to the value of every key-value pair in the map.  This example
// will construct a map from names (type 'bsl::string') to some (arbitrary)
// measure of salary (type 'int'):
//..
    typedef bdlcc::StripedUnorderedMap<bsl::string, int> SalaryMap;
//..
// First, define a functor, 'mySalaryAdjustmentVisitor', that increases values
// above 1000 by 3% and lower values by 5%.  The fractional part of increases
// are truncated to 'int' values:
//..
    struct mySalaryAdjustmentVisitor {
        bool operator()(int                *value,  // 'VALUE *'
                        const bsl::string&)         // 'const KEY&'
        {
            if (*value <= 1000) {
                *value = static_cast<int>(*value * 1.05);
            } else {
                *value = static_cast<int>(*value * 1.03);
            }
            return true;
        }
    };
//..

void example3()
    // Usage example 3 main function.
{
// Then, default create 'mySalaries', a 'SalaryMap' object:
//..
    SalaryMap mySalaries;
//..
// Next, load 'mySalaries' with some representative elements:
//..
    mySalaries.insert("Alex", 1000);
    mySalaries.insert("John",  800);
    mySalaries.insert("Rob",  1100);
    ASSERT(3 == mySalaries.size());
//..
// Now, apply 'mySalaryAdjustmentVisitor' to every element in the map:
//..
    mySalaryAdjustmentVisitor func;
    mySalaries.visit(func);
    ASSERT(3 == mySalaries.size());
//..
// Finally, confirm that the values have been adjusted as expected:
//..
//
    int         value = 0;
    bsl::size_t rc;

    rc = mySalaries.getValue(&value, "Alex");
    ASSERT(1    == rc);
    ASSERT(1050 == value);

    rc = mySalaries.getValue(&value, "John");
    ASSERT(1    == rc);
    ASSERT( 840 == value);

    rc = mySalaries.getValue(&value, "Rob");
    ASSERT(1    == rc);
    ASSERT(1133 == value);
//..
}

}  // close namespace usage

// Utilities
namespace {
typedef bsltf::TemplateTestFacility TstFacility;

}  // close unnamed namespace


namespace glbh {
// Wrap bsl::unordered_map with a single RW mutex.

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class GlobalHashMap {
    // This class represents a global hash map.

  public:
    // PUBLIC TYPES
    typedef bsl::pair<KEY, VALUE> KVType;
        // Value type of a bulk insert entry.

  private:
    // PRIVATE TYPES
    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL> MapType;
        // Hash map type.

    typedef bslmt::ReaderWriterMutex LockType;

    // DATA
    bslma::Allocator      *d_allocator_p;  // memory allocator (held, not
                                           // owned).

    MapType                d_map;          // hash table storing key-value
                                           // pairs.

    mutable LockType       d_lock;         // Lock for concurrency.

    // PRIVATE MANIPULATORS
    void lockRead();
        // Lock for read.

    void lockWrite();
        // Lock for write.

    void unlock();
        // Unlock.

    // NOT IMPLEMENTED
    GlobalHashMap(const GlobalHashMap<KEY, VALUE, HASH, EQUAL>&); // = delete
    GlobalHashMap<KEY, VALUE, HASH, EQUAL>&
                operator =(const GlobalHashMap<KEY, VALUE, HASH>&); // = delete

  public:
    // CREATORS
    explicit GlobalHashMap(int               numBuckets,
                           bslma::Allocator *basicAllocator = 0);
        // Create a 'GlobalHashMap' object have the specified 'numBuckets'
        // that are synchronized by a single lock.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '2 <= numBuckets', and 'numBuckets' is a power of
        // 2.

    // ~GlobalHashMap() = default;
        // Destroy this object.

    // MANIPULATORS
    void clear();
        // Remove all items from this global hash map.

    bsl::size_t erase(const KEY& key);
        // Remove the item having the specified 'key' from this hash.  Return 1
        // on success and 0 if 'key' does not exist.

    bsl::size_t eraseBulk(const bsl::vector<KEY>& keys);
        // Remove the items having the specified 'keys' from this hash.  Return
        // the number of items successfully removed.

    void insert(const KEY& key, const VALUE& value);
        // Insert the specified 'key' and its associated 'value' into this
        // hash.  If 'key' already exists, then its value will be replaced with
        // 'value'.

    bsl::size_t insertBulk(const bsl::vector<KVType>& data);
        // Insert the specified 'data' (composed of Key-Value pairs) into this
        // hash.  If a key already exists, then its value will be replaced with
        // the value.  Return the number of items successfully inserted.

    template <class FUNCTOR>
    int updateValue(FUNCTOR& functor, const KEY& key);
        // Given the specified 'key', check for its existence in this hash.
        // Apply the specified 'functor' with the given parameters of 'value',
        // 'key', and 'found' (true if found, false otherwise) to the value.
        // 'value' is a pointer to VALUE, and 'key' is a const reference to
        // KEY.  Return 0 on successful insert, 1 on successful update, and a
        // negative value on error.

    // ACCESSORS
    EQUAL equalFunction() const;
        // Return (a copy of) the key-equality functor used by this hash that
        // returns 'true' if two 'KEY' objects have the same value, and 'false'
        // otherwise.

    bsl::size_t getValue(VALUE *value, const KEY& key) const;
        // Load, into the specified 'value', the value associated with the
        // specified 'key' in this cache.  Return 1 on success, and 0 if 'key'
        // does not exist in this hash.

    HASH hashFunction() const;
        // Return (a copy of) the unary hash functor used by this hash to
        // generate a hash value (of type 'std::size_t') for a 'KEY' object.

    bsl::size_t bucketCount() const;
        // Return the size (number of buckets) in the hash.

    bsl::size_t size() const;
        // Return the current size of this hash.
};

             // ==================================================
             //                        INLINE FUNCTION DEFINITIONS
             // ==================================================

                            // -------------------
                            // class GlobalHashMap
                            // -------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
GlobalHashMap<KEY, VALUE, HASH, EQUAL>::GlobalHashMap(
                                              int               numBuckets,
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_map(numBuckets, d_allocator_p)
{
    BSLS_ASSERT(2 <= numBuckets);
    BSLS_ASSERT(0 == (numBuckets & (numBuckets - 1)));
    //d_map.max_load_factor(1e6); // Set high enough to prevent resize.
}

// PRIVATE MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void GlobalHashMap<KEY, VALUE, HASH, EQUAL>::lockRead()
{
    d_lock.lockRead();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void GlobalHashMap<KEY, VALUE, HASH, EQUAL>::lockWrite()
{
    d_lock.lockWrite();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void GlobalHashMap<KEY, VALUE, HASH, EQUAL>::unlock()
{
    d_lock.unlock();
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
void GlobalHashMap<KEY, VALUE, HASH, EQUAL>::clear()
{
    d_lock.lockWrite();
    d_map.clear();
    d_lock.unlock();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::erase(const KEY& key)
{
    bslmt::WriteLockGuard<LockType> guard(&d_lock);
    const bsl::size_t               res = d_map.erase(key);
    return res;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::eraseBulk(
                                                  const bsl::vector<KEY>& keys)
{
    bsl::size_t count = 0;

    bslmt::WriteLockGuard<LockType> guard(&d_lock);
    for (bsl::size_t i = 0; i < keys.size(); ++i) {
        const bsl::size_t res = d_map.erase(keys[i]);
        count += static_cast<int>(res);
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void GlobalHashMap<KEY, VALUE, HASH, EQUAL>::insert(const KEY&   key,
                                                    const VALUE& value)
{
    bslmt::WriteLockGuard<LockType> guard(&d_lock);
    d_map[key] = value;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::insertBulk(
                                               const bsl::vector<KVType>& data)
{
    bsl::size_t                     count = 0;
    bslmt::WriteLockGuard<LockType> guard(&d_lock);

    for (bsl::size_t i = 0; i < data.size(); ++i) {
        typename MapType::iterator mapIt = d_map.find(data[i].first);
        if (mapIt != d_map.end()) {
            mapIt->second = data[i].second;
        }
        else {
            d_map[data[i].first] = data[i].second;
            ++count;
        }
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class FUNCTOR> inline int
GlobalHashMap<KEY, VALUE, HASH, EQUAL>::updateValue(FUNCTOR&   functor,
                                                    const KEY& key)
{
    // Given the specified 'key', check for its existence in this hash.  Apply
    // the specified 'functor' with the given parameters of 'key' and 'found'
    // (true if found, false otherwise) to the value.  Return 0 on successful
    // insert, 1 on successful update, and a negative value on error.
    bslmt::WriteLockGuard<LockType> guard(&d_lock);

    typename MapType::iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        VALUE value;
        functor(&value, key, false, d_allocator_p);
        d_map[key] = value;
        //DBG_LOG1("UPDI_KEY=" << key << ",V=" << value)
        return 0;                                                     // RETURN
    }

    //DBG_LOG1("UPDU_KEY=" << key << ",V=" << mapIt->second )
    functor(&mapIt->second, key, true, d_allocator_p);
    return 1;
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL GlobalHashMap<KEY, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_map.key_eq();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::getValue(
                                                         VALUE      *value,
                                                         const KEY&  key) const
{
    bslmt::ReadLockGuard<LockType> guard(&d_lock);

    typename MapType::const_iterator mapIt = d_map.find(key);
    if (mapIt == d_map.end()) {
        return 0;                                                     // RETURN
    }

    *value = mapIt->second;

    return 1;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH GlobalHashMap<KEY, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_map.hash_function();
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::bucketCount() const
{
    return d_map.bucket_count();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t GlobalHashMap<KEY, VALUE, HASH, EQUAL>::size() const
{
    bslmt::ReadLockGuard<LockType> guard(&d_lock);
    return d_map.size();
}

}  // close namespace glbh

namespace hashPerf {
class HashPerformance {
    // This class performs the various performance tests.

  public:
    typedef bdlcc::StripedUnorderedMap<int, bsl::string>          HashType;
    typedef bdlcc::StripedUnorderedMap<bsl::string, bsl::vector<int> >
                                                                  HashType2;
    typedef glbh::GlobalHashMap<int, bsl::string>                 MapType;
    typedef glbh::GlobalHashMap<bsl::string, bsl::vector<int> >   MapType2;
    typedef bsl::pair<int, bsl::string>                           PairType;
    typedef bsl::pair<bsl::string, bsl::vector<int> >             PairType2;
    typedef bsls::Types::Int64                                    TimeType;
    typedef bsl::vector<bsls::Types::Int64>                       VecTimeType;
    typedef bsl::vector<int>                                      VecIntType;

    typedef int (*InitFunc)(HashPerformance*, VecIntType&);
        // Initialization function takes a vector of 'int', and returns 0 on
        // success, and negative value on failure.
    typedef int (*RunFunc)(HashPerformance*, VecIntType&);
        // A run function takes a pointer to 'HashPerformance', and a vector
        // of 'int'.  Returns a return code of 0 on success, and negative value
        // on failure.  The last entry in the vector of 'int' is the thread
        // number.  Writer threads are first, readers after.

    typedef struct WorkData {
        RunFunc          d_func;
        HashPerformance *d_hashPerf_p;
        VecIntType       d_data;
    } WorkData;

  private:
    // DATA
    bslma::Allocator *d_allocator_p; // memory allocator
    bsl::string       d_title;       // Title for printing purposes
    int               d_numStripes;  // # of stripes in input.  0 - use the
                                     // global unordered map
    int               d_numBuckets;  // # of buckets
    bool              d_enblRehash;  // Enable / disable rehash
    int               d_numRThreads; // # of reader threads
    int               d_numWThreads; // # of writer threads
    int               d_numThreads;  // d_numRThreads + d_numWThreads
    int               d_numCalcs;    // # of calculations, say 1,000,000
    int               d_numRepeats;  // # of repetitions of the calculation
                                     // (used to calculate variance), say 10.

    VecTimeType d_vecWTime;
    VecTimeType d_vecUTime;
    VecTimeType d_vecSTime;
        // Wall time, user time and system time in nanos, collected from the
        // various threads, and the various repetitions.  If we have 6
        // threads, and 10 repetitions, we will have 60 entries.

    VecTimeType d_avgWTime;
    VecTimeType d_avgUTime;
    VecTimeType d_avgSTime;
        // Averages of wall time, user time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.
        // There are 3 values for each here: 0-all threads, 1-WThreads,
        // 2-RThreads.  If no RThreads or WThreads, the respective value is 0.
    VecTimeType d_seWTime;
    VecTimeType d_seUTime;
    VecTimeType d_seSTime;
        // Standard errors of wall time, user time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.
        // They are set to 0 if d_numRepeat = 1.  There are 3 values for each
        // here: 0-all threads, 1-WThreads, 2-RThreads.  If no RThreads or
        // WThreads, the respective value is 0.
    VecTimeType d_medWTime;
    VecTimeType d_medUTime;
    VecTimeType d_medSTime;
        // Medians of wall time, user time and system time in nanos, calculated
        // from d_vecWTime, d_vecUTime, d_vecCTime respectively.  There are 3
        // values for each here: 0-all threads, 1-WThreads, 2-RThreads.  If no
        // RThreads or WThreads, the respective value is 0.
    VecTimeType d_avgMaxWTime;
    VecTimeType d_avgMaxUTime;
    VecTimeType d_avgMaxSTime;
        // Averages of wall time, user time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively, of
        // the longest running thread in each repetition.  There are 3 values
        // for each here: 0-all threads, 1-WThreads, 2-RThreads.  If no
        // RThreads or WThreads, the respective value is 0.
    VecTimeType d_medMaxWTime;
    VecTimeType d_medMaxUTime;
    VecTimeType d_medMaxSTime;
        // Medians of wall time, user time and system time in nanos, calculated
        // from d_vecWTime, d_vecUTime, d_vecCTime respectively, of the longest
        // running thread in each repetition.  There are 3 values for each
        // here: 0-all threads, 1-WThreads, 2-RThreads.  If no RThreads or
        // WThreads, the respective value is 0.
    HashType  d_hash;  // The real hash, int to string
    HashType2 d_hash2; // The real hash, string to int
    MapType   d_map;   // Unordered map with a single RW mutex, int to string
    MapType2  d_map2;  // Unordered map with a single RW mutex, int to string

    void runTest(VecTimeType *times, VecIntType& args, RunFunc func);
        // Run a single repetition of the calculation.  The calculation is
        // defined in the specified 'func' function, and takes as input the
        // specified 'args' vector.  For each thread a triad of elapsed wall
        // time, user time, and system time is returned in the specified
        // 'times'.

    // NOT IMPLEMENTED
    HashPerformance(const HashPerformance&);
    HashPerformance& operator=(const HashPerformance&);

  public:
    HashPerformance(const char       *title,
                    bsl::size_t       numStripes,
                    bsl::size_t       numBuckets,
                    bool              enblRehash,
                    int               numRThreads,
                    int               numWThreads,
                    int               numCalcs,
                    int               numRepeats,
                    bslma::Allocator *basicAllocator = 0);
        // Create a 'HashPerformance' object with the specified 'title',
        // 'numStripes', 'numBuckets', 'enblRehash', 'numRThreads',
        // 'numWThreads', 'numCalcs', and 'numRepeats'.  Optionally specify
        // 'basicAllocator'.

    int initialize(VecIntType& args, InitFunc func);
        // Run the initialization function.  The initialization is defined in
        // the specified 'func' function, and takes as input the specified
        // 'args' vector.  Returns the return code from 'func'.
    void runTests(VecTimeType *times, VecIntType& args, RunFunc func);
        // Run the tests by running 'runTest' numRepeats times.  The test being
        // run is the defined in the specified 'func' function, and takes as
        // input the specified 'args' vector.  A triad of elapsed wall time,
        // user time, and system time is returned in the specified 'times'.
    void printResult();
        // Print a standard time values output from the calculation.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the memory allocator

    HashType&  hash();
        // Return a reference providing modifiable access to the int->string
        // striped hash map.
    HashType2& hash2();
        // Return a reference providing modifiable access to the string->int
        // striped hash map.
    MapType&   map();
        // Return a reference providing modifiable access to the int->string
        // unordered map.
    MapType2&  map2();
        // Return a reference providing modifiable access to the string->int
        // unordered map.

    bsl::size_t numStripes() const;
        // Return the number of stripes in this hash.

    bsl::size_t numBuckets() const;
        // Return the number of buckets in this hash.

    bool enableRehash() const;
        // Return 'true' if rehash is enabled, and 'false' otherwise.

    int numCalcs() const;
        // Return the number of calculations.

    int numRepeats() const;
        // Return the number of test repetitions.

    int numRThreads() const;
        // Return the number of reader threads.

    int numWThreads() const;
        // Return the number of writer threads.

    int numThreads() const;
        // Return the total number of threads.  It is equal to numRThreads() +
        // numWThreads().

    bsl::size_t size() const;
        // Return the current size of this hash.

    const char* title() const;
        // Return the test title.

    // TEST FUNCTIONS
    static int testInsert(HashPerformance *hashPerf_p, VecIntType& args);
        // Insert rows, one at a time, into the specified 'hashPerf_p' using
        // the specified 'args' vector.  Only value in 'args' is the current
        // thread index, 0 to numThreads - 1.

    static int testInsertBulk(HashPerformance *hashPerf_p, VecIntType& args);
        // Insert rows, in bulk, into the specified 'hashPerf_p' using the
        // specified 'args' vector.  'args[0]' is the number of batches to use.
        // 'args[1]' (last parameter) is the current thread index, 0 to
        // numThreads - 1.

    static int initRead(HashPerformance *hashPerf_p, VecIntType& args);
        // Preload the specified hash in 'hashPerf_p' for subsequent tests
        // using the specified 'args' vector.  'args[0]' is the number of
        // values to load.  'arg[1]' is the sparsity of the data, that is, the
        // distance between consecutive values.  I.e: sparsity of 1 will
        // preload 0, 1, 2, ...  Sparsity of 2 will preload 0, 2, 4, ..., etc.
        // The hash is int to string.

    static int initRead2(HashPerformance *hashPerf_p, VecIntType& args);
        // Preload the specified hash in 'hashPerf_p' for subsequent tests
        // using the specified 'args' vector.  'args[0]' is the number of
        // values to load.  'arg[1]' is the sparsity of the data, that is, the
        // distance between consecutive values.  I.e: sparsity of 1 will
        // preload 0, 1, 2, ...  Sparsity of 2 will preload 0, 2, 4, ..., etc.
        // The hash is string to int.

    static int testRead(HashPerformance *hashPerf_p, VecIntType& args);
        // Run tryGetValue multiple times against the specified 'hashPerf_p',
        // using the specified 'args' vector.  '0 to arg[0] - 1' is the range
        // to read from.  'args[1]' is the thread index, and is used as the
        // seed for the random number generating the key values.

    static int testReadWrite(HashPerformance *hashPerf_p, VecIntType& args);
        // Run either 'insert' or 'tryGetValue' multiple times against the
        // specified 'hashPerf_p' hash, with the specified 'args' vector.
        // 'args[0]' is the number of values to load.  'arg[1]' is the
        // sparsity: 1 - 0, 1, 2, ...; 2 - 0, 2, 4, ..., etc.
        // '0 to arg[2] - 1' is the range to read from.  The first numWThreads
        // are write threads.  The rest are read threads.  The hash is int to
        // string.

    static int testReadWrite2(HashPerformance *hashPerf_p, VecIntType& args);
        // Run either 'insert' or 'tryGetValue' multiple times against the
        // specified 'hashPerf_p' hash, with the specified 'args' vector.
        // 'args[0]' is the number of values to load.  'arg[1]' is the
        // sparsity: 1 - 0, 1, 2, ...; 2 - 0, 2, 4, ..., etc.
        // '0 to arg[2] - 1' is the range to read from.  The first numWThreads
        // are write threads.  The rest are read threads.  The hash is string
        // to int.

};  // END class HashPerformance

// FREE FUNCTIONS
extern "C" void *workFunc(void *arg);
    // The thread main function.

HashPerformance::HashPerformance(const char       *title,
                                 bsl::size_t       numStripes,
                                 bsl::size_t       numBuckets,
                                 bool              enblRehash,
                                 int               numRThreads,
                                 int               numWThreads,
                                 int               numCalcs,
                                 int               numRepeats,
                                 bslma::Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
, d_title(title, basicAllocator)
, d_numStripes(static_cast<int>(numStripes))
, d_numBuckets(static_cast<int>(numBuckets))
, d_enblRehash(enblRehash)
, d_numRThreads(numRThreads)
, d_numWThreads(numWThreads)
, d_numThreads(numRThreads + numWThreads)
, d_numCalcs(numCalcs)
, d_numRepeats(numRepeats)
, d_vecWTime(basicAllocator)
, d_vecUTime(basicAllocator)
, d_vecSTime(basicAllocator)
, d_avgWTime(3, basicAllocator)
, d_avgUTime(3, basicAllocator)
, d_avgSTime(3, basicAllocator)
, d_seWTime (3, basicAllocator)
, d_seUTime (3, basicAllocator)
, d_seSTime (3, basicAllocator)
, d_medWTime(3, basicAllocator)
, d_medUTime(3, basicAllocator)
, d_medSTime(3, basicAllocator)
, d_avgMaxWTime(3, basicAllocator)
, d_avgMaxUTime(3, basicAllocator)
, d_avgMaxSTime(3, basicAllocator)
, d_medMaxWTime(3, basicAllocator)
, d_medMaxUTime(3, basicAllocator)
, d_medMaxSTime(3, basicAllocator)
, d_hash(d_numBuckets, d_numStripes == 0 ? 1 : d_numStripes, basicAllocator)
, d_hash2(d_numBuckets, d_numStripes == 0 ? 1 : d_numStripes, basicAllocator)
, d_map(d_numBuckets, basicAllocator)
, d_map2(d_numBuckets, basicAllocator)
{
}

int HashPerformance::initialize(VecIntType&               args,
                                HashPerformance::InitFunc func)
{
    // Call initializing function
    int rc = func(this, args);
    return rc;
}

void HashPerformance::runTests(VecTimeType              *times,
                               VecIntType&               args,
                               HashPerformance::RunFunc  func)
{
    for (int i = 0; i < 3; ++i) {
        d_seWTime [i] = 0;
        d_seUTime [i] = 0;
        d_seSTime [i] = 0;
        d_avgWTime[i] = 0;
        d_avgUTime[i] = 0;
        d_avgSTime[i] = 0;
    }

    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedWTimes(3);
    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedUTimes(3);
    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedSTimes(3);

    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedMaxWTimes(3);
    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedMaxUTimes(3);
    bsl::vector<bsl::vector<bsls::Types::Int64> > sortedMaxSTimes(3);

    for (int j = 0; j < d_numRepeats; ++j) {
        //DBG_LOG1("RUNTESTS=" << j)
        VecTimeType times(d_allocator_p);
        runTest(&times, args, func);

        bsl::vector<bsls::Types::Int64> maxWTime(3, 0);
        bsl::vector<bsls::Types::Int64> maxUTime(3, 0);
        bsl::vector<bsls::Types::Int64> maxSTime(3, 0);

        // WThreads come first.
        for (int i = 0; i < d_numThreads; ++i) {
            int rwIdx = i < d_numWThreads ? 1 : 2;

            TimeType curWTime = times[i * 3    ];
            TimeType curUTime = times[i * 3 + 1];
            TimeType curSTime = times[i * 3 + 2];
            d_vecWTime.push_back(curWTime);
            d_vecUTime.push_back(curUTime);
            d_vecSTime.push_back(curSTime);
            d_avgWTime[0    ] += curWTime;
            d_avgUTime[0    ] += curUTime;
            d_avgSTime[0    ] += curSTime;
            d_avgWTime[rwIdx] += curWTime;
            d_avgUTime[rwIdx] += curUTime;
            d_avgSTime[rwIdx] += curSTime;
            sortedWTimes[0    ].push_back(curWTime);
            sortedUTimes[0    ].push_back(curUTime);
            sortedSTimes[0    ].push_back(curSTime);
            sortedWTimes[rwIdx].push_back(curWTime);
            sortedUTimes[rwIdx].push_back(curUTime);
            sortedSTimes[rwIdx].push_back(curSTime);
            d_seWTime[0    ] += curWTime * curWTime;
            d_seUTime[0    ] += curUTime * curUTime;
            d_seSTime[0    ] += curSTime * curSTime;
            d_seWTime[rwIdx] += curWTime * curWTime;
            d_seUTime[rwIdx] += curUTime * curUTime;
            d_seSTime[rwIdx] += curSTime * curSTime;
            if (i == 0) {
                maxWTime[0    ] = curWTime;
                maxUTime[0    ] = curUTime;
                maxSTime[0    ] = curSTime;
                maxWTime[rwIdx] = curWTime;
                maxUTime[rwIdx] = curUTime;
                maxSTime[rwIdx] = curSTime;
            }
            else {
                if (maxWTime[0    ] < curWTime) maxWTime[0    ] = curWTime;
                if (maxUTime[0    ] < curUTime) maxUTime[0    ] = curUTime;
                if (maxSTime[0    ] < curSTime) maxSTime[0    ] = curSTime;
                if (maxWTime[rwIdx] < curWTime) maxWTime[rwIdx] = curWTime;
                if (maxUTime[rwIdx] < curUTime) maxUTime[rwIdx] = curUTime;
                if (maxSTime[rwIdx] < curSTime) maxSTime[rwIdx] = curSTime;
            }
        } // END loop on numThreads
        for (int i = 0; i < 3; ++i) {
            sortedMaxWTimes[i].push_back(maxWTime[i]);
            sortedMaxUTimes[i].push_back(maxUTime[i]);
            sortedMaxSTimes[i].push_back(maxSTime[i]);
            d_avgMaxWTime[i] += maxWTime[i];
            d_avgMaxUTime[i] += maxUTime[i];
            d_avgMaxSTime[i] += maxSTime[i];
        }
    } // END loop on numRepeats

    // Finalize the calculation
    bsl::vector<int> numSamples(3);
    numSamples[0] = d_numRepeats * d_numThreads;
    numSamples[1] = d_numRepeats * d_numWThreads;
    numSamples[2] = d_numRepeats * d_numRThreads;
    bsls::Types::Int64 temp;
    for (int i = 0; i < 3; ++i) {
        if (numSamples[i] > 0) {
            d_avgWTime[i] /= numSamples[i];
            d_avgUTime[i] /= numSamples[i];
            d_avgSTime[i] /= numSamples[i];
            // SD = SQRT(E(X^2) - E(X)^2)
            d_seWTime [i] /= numSamples[i];
            d_seUTime [i] /= numSamples[i];
            d_seSTime [i] /= numSamples[i];
            temp = d_seWTime[i] - d_avgWTime[i] * d_avgWTime[i];
            d_seWTime[i] = temp <= 0 ? 0 : static_cast<TimeType>(
                                         bsl::sqrt(static_cast<double>(temp)));
            temp = d_seUTime[i] - d_avgUTime[i] * d_avgUTime[i];
            d_seUTime[i] = temp <= 0 ? 0 : static_cast<TimeType>(
                                         bsl::sqrt(static_cast<double>(temp)));
            temp = d_seSTime[i] - d_avgSTime[i] * d_avgSTime[i];
            d_seSTime[i] = temp <= 0 ? 0 : static_cast<TimeType>(
                                         bsl::sqrt(static_cast<double>(temp)));
            d_avgWTime[i] /= 1000;
            d_avgUTime[i] /= 1000;
            d_avgSTime[i] /= 1000;
            d_seWTime [i] /= 1000;
            d_seUTime [i] /= 1000;
            d_seSTime [i] /= 1000;

            int medianIdx = numSamples[i] / 2;
            bsl::sort(sortedWTimes[i].begin(), sortedWTimes[i].end());
            bsl::sort(sortedUTimes[i].begin(), sortedUTimes[i].end());
            bsl::sort(sortedSTimes[i].begin(), sortedSTimes[i].end());
            d_medWTime[i] = sortedWTimes[i][medianIdx] / 1000;
            d_medUTime[i] = sortedUTimes[i][medianIdx] / 1000;
            d_medSTime[i] = sortedSTimes[i][medianIdx] / 1000;
        }
        d_avgMaxWTime[i] /= d_numRepeats;
        d_avgMaxUTime[i] /= d_numRepeats;
        d_avgMaxSTime[i] /= d_numRepeats;
        d_avgMaxWTime[i] /= 1000;
        d_avgMaxUTime[i] /= 1000;
        d_avgMaxSTime[i] /= 1000;
        bsl::sort(sortedMaxWTimes[i].begin(), sortedMaxWTimes[i].end());
        bsl::sort(sortedMaxUTimes[i].begin(), sortedMaxUTimes[i].end());
        bsl::sort(sortedMaxSTimes[i].begin(), sortedMaxSTimes[i].end());
        d_medMaxWTime[i] = sortedMaxWTimes[i][d_numRepeats / 2] / 1000;
        d_medMaxUTime[i] = sortedMaxUTimes[i][d_numRepeats / 2] / 1000;
        d_medMaxSTime[i] = sortedMaxSTimes[i][d_numRepeats / 2] / 1000;
        times->push_back(d_avgWTime[i]);
        times->push_back(d_avgUTime[i]);
        times->push_back(d_avgSTime[i]);
    }
}

void HashPerformance::runTest(VecTimeType              *times,
                              VecIntType&               args,
                              HashPerformance::RunFunc  func)
{
    if (d_numThreads == 1) {
        // For a single thread, run calculation on the caller
        TimeType startWTime = bsls::TimeUtil::getTimer();
        TimeType startUTime, startSTime;
        bsls::TimeUtil::getProcessTimers(&startSTime, &startUTime);
        args.push_back(-1);
        int      rc = func(this, args);
        (void) rc;
        TimeType endWTime = bsls::TimeUtil::getTimer();
        TimeType endUTime, endSTime;
        bsls::TimeUtil::getProcessTimers(&endSTime, &endUTime);
        times->push_back((endWTime - startWTime) / 1000);
        times->push_back((endUTime - startUTime) / 1000);
        times->push_back((endSTime - startSTime) / 1000);
    }

    bsl::vector<bslmt::ThreadUtil::Handle> handles(d_numThreads);
    bsl::vector<WorkData>                  todos(d_numThreads);

    // Spawn work threads
    for (int i = 0; i < d_numThreads; ++i) {
        todos[i].d_func = func;
        todos[i].d_hashPerf_p = this;
        todos[i].d_data = args;
        todos[i].d_data.push_back(i);

        bslmt::ThreadUtil::create(&handles[i], workFunc, &todos[i]);
    }
    // Collect results
    bsl::vector<void*> results(d_numThreads);
    for (int i = 0; i < d_numThreads; ++i) {
        bslmt::ThreadUtil::join(handles[i], &results[i]);
        VecTimeType *pRes = reinterpret_cast<VecTimeType*>(results[i]);
        for (bsl::size_t j = 0; j < pRes->size(); ++j) {
            times->push_back((*pRes)[j]);
        }
        delete pRes;
    }
}

void HashPerformance::printResult()
{
    // print the output from the calculation
    bsl::cout << "Title=" << d_title << "\n";
    bsl::cout << bsl::fixed << bsl::setprecision(1);
    bsl::cout << "Calcs="     << d_numCalcs
              << ",Stripes="  << d_numStripes
              << ",enRehash=" << d_enblRehash
              << ",Buckets="  << d_numBuckets
              << ",Threads="  << d_numThreads
              << ",RThreads=" << d_numRThreads
              << ",WThreads=" << d_numWThreads
              << ",loadFactor=" << d_hash.loadFactor() << "\n";

    // Time is printed in milliseconds
    const char *subTitles[]  = {"All ", "WThreads ", "RThreads "};
    int         numSamples[] = {d_numThreads, d_numWThreads, d_numRThreads};
    double      temp;
    for (int i = 0; i < 3; ++i) {
        if (numSamples[i] > 0 && (i == 0 || numSamples[i] != numSamples[0])) {
            temp = d_avgWTime[i] == 0 ? 0.0 :
                     100.0 * static_cast<double>(d_seWTime[i]) /
                             static_cast<double>(d_avgWTime[i]);
            bsl::cout << subTitles[i] << "WTime=" << d_avgWTime[i] << "+/-"
                      << temp << "%,";
            temp = d_avgUTime[i] == 0 ? 0.0 :
                     100.0 * static_cast<double>(d_seUTime[i]) /
                             static_cast<double>(d_avgUTime[i]);
            bsl::cout << "UTime=" << d_avgUTime[i] << "+/-" << temp << "%,";
            temp = d_avgSTime[i] == 0 ? 0.0 :
                     100.0 * static_cast<double>(d_seSTime[i]) /
                             static_cast<double>(d_avgSTime[i]);
            bsl::cout << "STime=" << d_avgSTime[i] << "+/-" << temp << "%\n";
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (numSamples[i] > 0 && (i == 0 || numSamples[i] != numSamples[0])) {
            bsl::cout << subTitles[i] << "WMed=" << d_medWTime[i] << ",";
            bsl::cout << "UMed=" << d_medUTime[i] << ",";
            bsl::cout << "SMed=" << d_medSTime[i] << "\n";
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (numSamples[i] > 0 && (i == 0 || numSamples[i] != numSamples[0])) {
            bsl::cout << subTitles[i] << "WMAvg=" << d_avgMaxWTime[i] << ",";
            bsl::cout << "UMAvg=" << d_avgMaxUTime[i] << ",";
            bsl::cout << "SMAvg=" << d_avgMaxSTime[i] << "\n";
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (numSamples[i] > 0 && (i == 0 || numSamples[i] != numSamples[0])) {
            bsl::cout << subTitles[i] << "WMMed=" << d_medMaxWTime[i] << ",";
            bsl::cout << "UMMed=" << d_medMaxUTime[i] << ",";
            bsl::cout << "SMMed=" << d_medMaxSTime[i] << "\n";
        }
    }
}

                 // Expose members
inline
HashPerformance::HashType& HashPerformance::hash()
{
    return d_hash;
}

inline
HashPerformance::HashType2& HashPerformance::hash2()
{
    return d_hash2;
}

inline
HashPerformance::MapType& HashPerformance::map()
{
    return d_map;
}

// ACCESSORS
inline
bslma::Allocator* HashPerformance::allocator() const
{
    return d_allocator_p;
}

inline
bool HashPerformance::enableRehash() const
{
    return d_enblRehash;
}

inline
HashPerformance::MapType2& HashPerformance::map2()
{
    return d_map2;
}

inline
bsl::size_t HashPerformance::numBuckets() const
{
    return d_numBuckets;
}

inline
int HashPerformance::numCalcs() const
{
    return d_numCalcs;
}

inline
int HashPerformance::numRThreads() const
{
    return d_numRThreads;
}

inline
int HashPerformance::numRepeats() const
{
    return d_numRepeats;
}

inline
bsl::size_t HashPerformance::numStripes() const
{
    return d_numStripes;
}

inline
int HashPerformance::numThreads() const
{
    return d_numThreads;
}

inline
int HashPerformance::numWThreads() const
{
    return d_numWThreads;
}

inline
bsl::size_t HashPerformance::size() const
{
    return d_hash.size();
}

inline
const char* HashPerformance::title() const
{
    return d_title.c_str();
}


// TEST FUNCTIONS
int HashPerformance::testInsert(HashPerformance *hashPerf_p,
                                VecIntType&      args)
{
    // Insert rows one at a time into the specified 'hashPerf_p' using the
    // specified 'args' vector.  Only value in 'args' is the current thread
    // index, 0 to 'numThreads - 1'.

    int size = hashPerf_p->numCalcs();

    // Current thread number is the last (and only) argument

    int  threadIdx = args[0];
    int  offset = size * threadIdx;

    if (hashPerf_p->numStripes() > 0) {
        if (false == hashPerf_p->enableRehash()) {
            hashPerf_p->hash().disableRehash();
        }
        for (int i = 0; i < size; ++i) {
            int    key = offset++;
            string val("ABCDEFG");
            hashPerf_p->hash().insert(key, val);
        }
    } else {
        for (int i = 0; i < size; ++i) {
            int    key = offset++;
            string val("ABCDEFG");
            hashPerf_p->map().insert(key, val);
        }
    }

    return 0;
}

int HashPerformance::testInsertBulk(HashPerformance *hashPerf_p,
                                    VecIntType&      args)
{
    // Insert rows in bulk into the specified 'hashPerf_p' using the specified
    // 'args' vector.  'args[0]' is the number of batches to use.  'args[1]'
    // (last parameter) is the current thread index,
    // 0 to 'numThreads - 1'.

    int  numBatches = args[0];
    int  threadIdx = args[1];
    int  size = hashPerf_p->numCalcs() / numBatches;
    int  offset = size * numBatches * threadIdx;

    bsl::string value;

    if (hashPerf_p->numStripes() > 0) {
        if (false == hashPerf_p->enableRehash()) {
            hashPerf_p->hash().disableRehash();
        }
        for (int j = 0; j < numBatches; ++j) {
            bsl::vector<PairType> insertData(size, hashPerf_p->d_allocator_p);
            for (int i = 0; i < size; ++i) {
                int    key = offset++;
                string val("ABCDEFG");
                insertData[i] = PairType(key, val);
            }
            bsl::size_t count =
                              hashPerf_p->hash().insertBulk(insertData.begin(),
                                                            insertData.end());
            (void)count;
        }
    } else {
        for (int j = 0; j < numBatches; ++j) {
            bsl::vector<PairType> insertData(size, hashPerf_p->d_allocator_p);
            for (int i = 0; i < size; ++i) {
                int    key = offset++;
                string val("ABCDEFG");
                insertData[i] = PairType(key, val);
            }
            hashPerf_p->map().insertBulk(insertData);
        }
    }
    return 0;
}

int HashPerformance::initRead(HashPerformance *hashPerf_p, VecIntType& args)
{
    // Preload the specified hash in 'hashPerf_p' for subsequent tests using
    // the specified 'args' vector.  'args[0]' is the number of values to load.
    // 'arg[1]' is the sparsity of the data, that is, the distance between
    // consecutive values.  I.e: sparsity of 1 will preload 0, 1, 2, ...
    // Sparsity of 2 will preload 0, 2, 4, ..., etc.

    int  size = args[0];
    int  sparsity = args[1];
    int  key = 0;
    char buf[12];

    bsl::vector<PairType> insertData(size, hashPerf_p->d_allocator_p);
    for (int i = 0; i < size; ++i) {
        bsl::sprintf(buf, "V%d", key);
        insertData[i] = PairType(key, buf);
        key += sparsity;
    }
    bsl::size_t count = 0;
    if (hashPerf_p->numStripes() > 0) {
        if (false == hashPerf_p->enableRehash()) {
            hashPerf_p->hash().disableRehash();
        }
        count = static_cast<int>(
          hashPerf_p->hash().insertBulk(insertData.begin(), insertData.end()));
    } else {
        count = hashPerf_p->map().insertBulk(insertData);
    }
    return static_cast<int>(count);
}

int HashPerformance::initRead2(HashPerformance *hashPerf_p, VecIntType& args)
{
    // Preload the specified hash in 'hashPerf_p' for subsequent tests using
    // the specified 'args' vector.  'args[0]' is the number of values to load.
    // 'arg[1]' is the sparsity of the data, that is, the distance between
    // consecutive values.  I.e: sparsity of 1 will preload 0, 1, 2, ...
    // Sparsity of 2 will preload 0, 2, 4, ..., etc.

    int  size = args[0];
    int  sparsity = args[1];
    int  keyGen = 0;
    char buf[85];

    bsl::vector<PairType2> insertData(size, hashPerf_p->d_allocator_p);
    for (int i = 0; i < size; ++i) {
        bsl::sprintf(buf, "VABCDEFGHIJKLMNOABCDEFGHIJKLMNO%d%10d%10d%10d%10d",
                                       keyGen, keyGen, keyGen, keyGen, keyGen);
        bsl::string      key(buf);
        bsl::vector<int> value(10);
        for (int j = 0; j < 10; ++j) value[j] = i + j;
        insertData[i] = PairType2(key, value);
        keyGen += sparsity;
    }
    bsl::size_t count = 0;
    if (hashPerf_p->numStripes() > 0) {
        if (false == hashPerf_p->enableRehash()) {
            hashPerf_p->hash2().disableRehash();
        }
        count = hashPerf_p->hash2().insertBulk(insertData.begin(),
                                               insertData.end());
    } else {
        count = hashPerf_p->map2().insertBulk(insertData);
    }
    return static_cast<int>(count);
}

int HashPerformance::testRead(HashPerformance *hashPerf_p, VecIntType& args)
{
    // Run getValue multiple times against the specified 'hashPerf_p', using
    // the specified 'args' vector.  '0 to arg[0] - 1' is the range to read
    // from.  'args[1]' is the thread index, and is used as the seed for the
    // random number generating the key values.

    int numThreads = hashPerf_p->numThreads();
    int numCalcs   = hashPerf_p->numCalcs();
    int size = numCalcs / numThreads;
    int range = args[0];
    int threadIdx = args[1];
    int countErr = 0;
    srand(threadIdx);
    bsl::string value;
    if (hashPerf_p->numStripes() > 0) {
        for (int i = 0; i < size; ++i) {
            int         key = rand() % range;
            bsl::size_t rc = hashPerf_p->hash().getValue(&value, key);
            if (rc != 1 && numCalcs == range) {
                cout << "rc=" << rc << ",key=" << key << ",i=" << i << "\n";
                ++countErr;
            }
        }
    } else {
        for (int i = 0; i < size; ++i) {
            int         key = rand() % range;
            bsl::size_t rc = hashPerf_p->map().getValue(&value, key);
            if (rc != 1 && numCalcs == range) {
                cout << "rc=" << rc << ",key=" << key << ",i=" << i << "\n";
                ++countErr;
            }
        }
    }
    if (countErr > 0)
        bsl::cout << "countErr=" << countErr << "\n";
    return 0;
}

int HashPerformance::testReadWrite(HashPerformance *hashPerf_p,
                                   VecIntType&      args)
{
    // Run either insert of getValue multiple times.  'args[0]' is the number
    // of values to load.  'arg[1]' is the sparsity: 1 - 0, 1, 2, ...;
    // 2 - 0, 2, 4, ..., etc.  '0 to arg[2] - 1' is the range to read from.
    // The first numWThreads are write threads.  The rest are read threads.

    int countErr = 0;
    int numWThreads = hashPerf_p->numWThreads();
    int numCalcs    = hashPerf_p->numCalcs();
    int size = numCalcs;
    int range = args[0];
    int threadIdx = args[1];
    int offset = size * threadIdx;

    bool        isWThread = threadIdx < numWThreads ? true : false;
    bsl::string value;
    if (isWThread) {
        if (hashPerf_p->numStripes() > 0) {
            if (false == hashPerf_p->enableRehash()) {
                hashPerf_p->hash().disableRehash();
            }
            for (int i = 0; i < size; ++i) {
                int    key = offset++;
                string val("ABCDEFG");
                hashPerf_p->hash().insert(key, val);
            }
        } else {
            for (int i = 0; i < size; ++i) {
                int    key = offset++;
                string val("ABCDEFG");
                hashPerf_p->map().insert(key, val);
            }
        }
    } else { // R thread
        if (hashPerf_p->numStripes() > 0) {
            for (int i = 0; i < size; ++i) {
                int         key = offset++ % range;
                bsl::size_t rc = hashPerf_p->hash().getValue(&value, key);
                if (rc != 1 && numCalcs == range) {
                    cout << "rc=" << rc << ",key=" << key << ",i=" << i
                         << "\n";
                    ++countErr;
                }
            }
        } else {
            for (int i = 0; i < size; ++i) {
                int         key = offset++ % range;
                bsl::size_t rc = hashPerf_p->map().getValue(&value, key);
                if (rc != 1 && numCalcs == range) {
                    cout << "rc=" << rc << ",key=" << key << ",i=" << i
                         << "\n";
                    ++countErr;
                }
            }
        }
    }
    if (countErr > 0)
        bsl::cout << "countErr=" << countErr << "\n";
    return 0;
}

int HashPerformance::testReadWrite2(HashPerformance *hashPerf_p,
                                    VecIntType&      args)
{
    // Run either insert of getValue multiple times.  'args[0]' is the number
    // of values to load.  'arg[1]' is the sparsity: 1 - 0, 1, 2, ...;
    // 2 - 0, 2, 4, ..., etc.  '0 to arg[2] - 1' is the range to read from.
    // The first numWThreads are write threads.  The rest are read threads.

    int countErr = 0;
    int numWThreads = hashPerf_p->numWThreads();
    int numCalcs    = hashPerf_p->numCalcs();
    int size = numCalcs;
    int range = args[0];
    int threadIdx = args[1];

    bool isWThread = threadIdx < numWThreads ? true : false;
    char buf[85];
    srand(threadIdx);

    for (int i = 0; i < size; ++i) {
        int keyGen = rand() % range;
        bsl::sprintf(buf, "VABCDEFGHIJKLMNOABCDEFGHIJKLMNO%d%10d%10d%10d%10d",
                                       keyGen, keyGen, keyGen, keyGen, keyGen);
        bsl::string key(buf);
        if (isWThread) {
            bsl::vector<int> value(10);
            for (int j = 0; j < 10; ++j) value[j] = i + j;
            if (hashPerf_p->numStripes() > 0) {
                hashPerf_p->hash2().insert(key, value);
            } else {
                hashPerf_p->map2().insert(key, value);
            }
        }
        else {
            bsl::vector<int> value;
            bsl::size_t      rc = INT_MAX;
            if (hashPerf_p->numStripes() > 0) {
                rc = hashPerf_p->hash2().getValue(&value, key);
            } else {
                rc = hashPerf_p->map2().getValue(&value, key);
            }
            if (rc != 1 && numCalcs == range) {
                cout << "rc=" << rc << ",key=" << key << ",i=" << i << "\n";
                ++countErr;
            }
        }
    }
    if (countErr > 0)
        bsl::cout << "countErr=" << countErr << "\n";
    return 0;
}

// FREE FUNCTIONS
extern "C" void *workFunc(void *arg)
{
    HashPerformance::WorkData    *wdp =
                           reinterpret_cast<HashPerformance::WorkData *>(arg);
    HashPerformance::VecTimeType *pTimes =
          new HashPerformance::VecTimeType(wdp->d_hashPerf_p->allocator());

    HashPerformance::TimeType startWTime = bsls::TimeUtil::getTimer();

    HashPerformance::TimeType startUTime, startSTime;
    bsls::TimeUtil::getProcessTimers(&startSTime, &startUTime);

    int rc = wdp->d_func(wdp->d_hashPerf_p, wdp->d_data);
    (void) rc;

    HashPerformance::TimeType endWTime = bsls::TimeUtil::getTimer();

    HashPerformance::TimeType endUTime, endSTime;
    bsls::TimeUtil::getProcessTimers(&endSTime, &endUTime);

    pTimes->push_back((endWTime - startWTime) / 1000);
    pTimes->push_back((endUTime - startUTime) / 1000);
    pTimes->push_back((endSTime - startSTime) / 1000);
    return pTimes;
}

}  // close namespace hashPerf

namespace threaded {


struct ThreadArg {
    typedef bdlcc::StripedUnorderedMap<int, int> StripType;

    StripType       *d_strip_p;
    bsls::AtomicInt *d_stop_p;
    bsls::AtomicInt *d_writeCounts_p;
    int              d_numItems;
    int              d_workerId;
    int              d_numWorkers;
};

void worker(ThreadArg *arg)
    // Worker thread function for multi threaded stress test with the specified
    // 'arg'.
{
    int seed;
    int rc = bdlb::RandomDevice::getRandomBytesNonBlocking(
        reinterpret_cast<unsigned char*>(&seed), sizeof(int));

    ASSERTV(rc == 0);
    while (0 == *arg->d_stop_p) {
        int key = bdlb::Random::generate15(&seed) % arg->d_numItems;
        if (key % arg->d_numWorkers != arg->d_workerId) {
            continue;
        }
        if (veryVeryVerbose) {
            bsl::stringstream ss;
            ss << arg->d_workerId << "/" << arg->d_numWorkers << ": " << key
               << "\n";
            bsl::cout << ss.str();
        }
        int value;

        bsl::size_t rc = arg->d_strip_p->getValue(&value, key);

        if (veryVeryVerbose) {
            const bsl::size_t bucketCount = arg->d_strip_p->bucketCount();
            bsl::stringstream ss;
            ss << "AfterGetValue, rc=" << rc << "," << arg->d_workerId << "/"
               << arg->d_numWorkers << ": " << key << ", bucketCount="
               << bucketCount << "\n";
            bsl::cout << ss.str();
        }

        arg->d_strip_p->insert(key, 0);

        if (veryVeryVerbose) {
            const bsl::size_t bucketCount = arg->d_strip_p->bucketCount();
            bsl::stringstream ss;
            ss << "AfterInsert, rc=" << rc << "," << arg->d_workerId << "/"
               << arg->d_numWorkers << ": " << key << ", bucketCount="
               << bucketCount << "\n";
            bsl::cout << ss.str();
        }

        if (1 != rc) {
            arg->d_strip_p->insert(key, 0);
            arg->d_writeCounts_p[key] = 0;
            if (veryVeryVerbose) {
                const bsl::size_t bucketCount = arg->d_strip_p->bucketCount();
                bsl::stringstream ss;
                ss << "Insert2, rc=" << rc << "," << arg->d_workerId << "/"
                   << arg->d_numWorkers << ": " << key << ", bucketCount="
                   << bucketCount << "\n";
                bsl::cout << ss.str();
            }
        }
        else {
            arg->d_strip_p->insert(key, value + 1);
            ++arg->d_writeCounts_p[key];
            if (veryVeryVerbose) {
                const bsl::size_t bucketCount = arg->d_strip_p->bucketCount();
                bsl::stringstream ss;
                ss << "Insert3, rc=" << rc << "," << arg->d_workerId << "/"
                   << arg->d_numWorkers << ": " << key << ", bucketCount="
                   << bucketCount << "\n";
                bsl::cout << ss.str();
            }
        }
    }
}

extern "C" void *workerThread(void *v_arg)
{
    ThreadArg *arg = (ThreadArg *) v_arg;
    worker(arg);
    return (void*) v_arg;
}

void threadedTest1()
    // Multi threaded stress test.
{
    // ------------------------------------------------------------------------
    // MULTI-THREADED STRESS TEST
    //    Other tests have shown that the locking methods of this class invoke
    //    invoke locks of the expected types (i.e., the lock types dictated by
    //    our design); however, questions remain.
    //
    // Concerns:
    //: 1 Each method invokes an appropriate lock (i.e., "read" or "write" as
    //:   needed by the context) and that lock protects all of the critical
    //:   code sections that require protection.
    //
    // Plan:
    //: 1 Create a hash map and a set of threads that for a specified period
    //:   perform several actions at random intervals.  Those actions are:
    //:   o the insertion of elements: 'insert' and 'setValue'
    //:   o fetching the value (attribute) of elements: 'getValue'
    //:   o the increment of element values (attributes): 'insert' and
    //:     'setValue'
    //:
    //: 2 At the end of the test period, confirm that value (attribute) each
    //:   element matches the expected value (according to thread activity).
    //:
    //: 3 Confirm that hash map's number of buckets has increased during the
    //:   test run: an indication that rehash ran concurrently with the other
    //:   activity at some point.
    //:
    //: 4 This test is basic sanity test of what are deemed to be the most
    //:   heavily used methods, and *not* a comprehensive test that all
    //:   multi-threaded scenarios, involving all methods, will run correctly.
    //
    // Testing:
    //   MULTI-THREADED STRESS TEST
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "MULTI-THREADED STRESS TEST" << endl
                      << "--------------------------" << endl;

    const int k_NUM_WORKERS = 10;
    const int k_NUM_ITEMS = 128;

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
    ThreadArg::StripType strip(1024, 1, &supplied);
    bsls::AtomicInt      writeCounts[k_NUM_ITEMS];  // default 0
    bsls::AtomicInt      stop(0);

    bslmt::ThreadUtil::Handle queryHandles[k_NUM_WORKERS];
    ThreadArg                 args[k_NUM_WORKERS];

    for (int i = 0; i < k_NUM_WORKERS; ++i) {
        ThreadArg arg = {&strip, &stop, writeCounts,
                   k_NUM_ITEMS, i, k_NUM_WORKERS};
        args[i] = arg;

        bslmt::ThreadUtil::create(&queryHandles[i],
                                  workerThread, &args[i]);
    }

    bslmt::ThreadUtil::microSleep(0, 3);

    stop = 1;

    for (int i = 0; i < k_NUM_WORKERS; ++i) {
        bslmt::ThreadUtil::join(queryHandles[i]);
    }

    for (int i = 0; i < k_NUM_ITEMS; ++i) {
        int value;

        bsl::size_t rc = strip.getValue(&value, i);
        int         count = rc != 1 ? 0 : value;

        ASSERTV(i, count, writeCounts[i], count == writeCounts[i]);

        if (veryVerbose) {
            bsl::cout << i << ": " << writeCounts[i] << bsl::endl;
        }
    }
}

}  // close namespace threaded

// TestDriver template
namespace {

bslma::TestAllocator defaultAllocator("default-static", false);

template <class KEY, class VALUE, class ALLOC>
struct IntToPairConverter {
    // Convert an 'int' value to a 'bsl::pair' of the template parameter 'KEY'
    // and 'VALUE' types.

    // CLASS METHODS
    static void
    createInplace(bsl::pair<KEY, VALUE> *address,
                  int                    value,
                  ALLOC                  allocator)
        // Create a new 'pair<KEY, VALUE>' object at the specified 'address',
        // passing the specified 'value' to the 'KEY' and 'VALUE' constructors
        // and using the specified 'allocator' to supply memory.  The behavior
        // is undefined unless '0 < value < 128'.
    {
        BSLS_ASSERT(address);
        BSLS_ASSERT(    0 < value);
        BSLS_ASSERT(value < 128);

        // If creating the 'key' and 'value' temporary objects requires an
        // allocator, it should not be the default allocator as that will
        // confuse the arithmetic of our test machinery.  Therefore, we will
        // use the 'bslma::MallocFreeAllocator', as being the simplest, least
        // obtrusive allocator that is also unlikely to be employed by an end
        // user.

        bslma::Allocator *privateAllocator =
                                      &bslma::MallocFreeAllocator::singleton();

        bsls::ObjectBuffer<typename bsl::remove_const<KEY>::type> tempKey;
        bsltf::TemplateTestFacility::emplace(tempKey.address(),
                                             value,
                                             privateAllocator);

        bsls::ObjectBuffer<VALUE> tempValue;
        bsltf::TemplateTestFacility::emplace(tempValue.address(),
                                             value - 'A' + '0',
                                             privateAllocator);

        bsl::allocator_traits<ALLOC>::construct(
                              allocator,
                              address,
                              bslmf::MovableRefUtil::move(tempKey.object()),
                              bslmf::MovableRefUtil::move(tempValue.object()));
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.

    // DATA
    int d_id;

  public:
    explicit TestHashFunctor(int id = 0)
        // Create 'TestHashFunctor' object with the optionally specified 'id'.
    : d_id(id)
    {}

    // ACCESSORS
    bsl::size_t operator() (const TYPE& obj) const
        // Return the has value of the specified 'obj'.
    {
        return  bsltf::TemplateTestFacility::getIdentifier(obj);
    }

    bool operator==(const TestHashFunctor& rhs) const
        // Return 'true' if the specified 'rhs' equals this 'TestHashFunctor'.
    {
        return d_id == rhs.d_id;
    }

    int id() const
        // Return the 'id' attribute of this 'TestHashFunctor'.
    {
        return d_id;
    }
};

template <class TYPE>
bool areEqual(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the ID of the specified 'lhs' and 'rhs' is equal, and
    // 'false' otherwise.
{
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
        == bsltf::TemplateTestFacility::getIdentifier(rhs);
}

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.

    // DATA
    int d_id;

  public:
    explicit TestEqualityComparator(int id = 0)
        // Create 'TestEqualityComparator' object with the optionally specified
        // 'id'.
    : d_id(id)
    {}

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        return areEqual(lhs, rhs);
    }

    bool operator==(const TestEqualityComparator& rhs) const
        // Return 'true' if the specified 'rhs' equals this 'TestHashFunctor'.
    {
        return d_id == rhs.d_id;
    }

    int id() const
        // Return the 'id' attribute of this 'TestHashFunctor'.
    {
        return d_id;
    }
};

template <class KEY,
          class VALUE = KEY,
          class HASH  = TestHashFunctor<KEY>,
          class EQUAL = TestEqualityComparator<KEY> >
class TestDriver {
    // This templatized struct provide a namespace for testing 'bdlcc::Hash'.
    // The parameterized 'KEY', 'VALUE', 'HASH', 'EQUAL' specifies the key
    // type, the mapped type, the hash functor, the equality comparator type
    // respectively.  Each "testCase*" method test a specific aspect of
    // 'bdlcc:Hash<KEY, VALUE, HASH, EQUAL>'.  Every test cases should be
    // invoked with various parameterized type to fully test the hash.

  private:
    // TYPES
    typedef bdlcc::StripedUnorderedMap<KEY, VALUE, HASH, EQUAL> Obj;

    // Shorthands
    typedef bsl::pair<KEY, VALUE>    PairType;
    typedef bsl::allocator<PairType> PairStdAllocator;

    typedef bsltf::TestValuesArray<PairType,
                                   PairStdAllocator,
                                   IntToPairConverter<
                                                KEY,
                                                VALUE,
                                                PairStdAllocator> > TestValues;

    struct TestCase15Updater {
        // Functor for testCase15 - setComputedValue in rehash.

        // DATA
        VALUE d_value; // Value to assign if 'found' is 'false'.

        // CREATORS
        explicit TestCase15Updater(const VALUE &value);
            // Create a 'TestCase15Updater' with the specified 'value'.

        // MANIPULATORS
        bool operator()(VALUE *value, const KEY& key);
            // If the specified 'value' equals VALUE(), set 'value' to the
            // constructor provided 'value'.  Ignore the specified 'key'.
    };

    struct TestCase14Updater {
        // Functor for testCase14 - visit.

        // DATA
        int         d_numSuccess; // Functor will succeed for the first
                                  // 'd_numSuccess' times, and fail after.
        const TestValues &d_values;     // Test values array, size 52

        // CREATORS
        explicit TestCase14Updater(int numSuccess, const TestValues& values);
            // Create a 'TestCase14Updater' with the specified 'numSuccess' and
            // a reference to the specified 'values'.  Functor calls will
            // succeed the first 'numSuccess' times and fail afterwards.

        // MANIPULATORS
        bool operator()(VALUE      *value,
                        const KEY&  key);
            // Update the specified 'value' with the value related to the key
            // after the specified 'key'.  Return 'true' for the first
            // 'd_numSuccess' calls, and 'false' afterwards.
    };

    struct TestCase13Updater {
        // Functor for testCase13 - update.

        // DATA
        bool d_success; // Set to 'false' if functor is to fail, or 'true'
                        // otherwise;
        // CREATORS
        explicit TestCase13Updater(bool success);
            // Create a 'TestCase13Updater' with the specified 'success'.
            // Functor calls will return the same value as 'success'.

        // MANIPULATORS
        bool operator()(VALUE *value, const KEY& key);
            // Set the specified 'key' to the static s_testCase13_key.  Set the
            // identifier in the value pointed by the specified 'value' to the
            // static s_testCase13_valueId.  Copy the static s_testCase13_value
            // to 'value'.  Return the boolean value in 'd_success'.
    };

    struct TestCase12Updater {
        // Functor for 'testCase12' - 'setComputedValue'.

        // MANIPULATORS
        bool operator()(VALUE *value, const KEY& key);
            // Set the specified 'key' to the static 's_testCase12_key'.  If
            // the specified 'value' does not equal VALUE(), set the identifier
            // in the value pointed by 'value' to the static
            // 's_testCase12_valueId' and 's_testCase12_found' to 'true',
            // otherwise set 's_testCase12_found' to 'true'.  Copy the static
            // 's_testCase12_value' to 'value'.
    };

    // PUBLIC DATA
    static bool  s_testCase12_found;
    static KEY   s_testCase12_key;
    static int   s_testCase12_valueId;
    static VALUE s_testCase12_value;
        // Places for the test case 12 functor to put a copy of its 'found'
        // input, 'key' input, the identifier of its '*value' input, and to
        // obtain a value to be set to '*value', respectively.

    static KEY   s_testCase13_key;
    static int   s_testCase13_valueId;
    static VALUE s_testCase13_value;
    static int   s_testCase14_count;
        // Places for the test case 13 functor to put a copy of its 'key'
        // input. to put a copy of the identifier of its '*value' input, to
        // obtain a value to be set to its '*value', and to keep a counter of
        // calls, respectively.

  public:
    static void testCase1();
    static void testCase2();
    static void testCase3();
    static void testCase4();
    static void testCase5();
    static void testCase6();
    static void testCase7();
    static void testCase8();
    static void testCase9();
    static void testCase10();
    static void testCase11();
    static void testCase12();
    static void testCase13();
    static void testCase14();
    static void testCase15();
    static void testCase16();
    static void testCase17();
        // Code for test cases 1 to 17.
};

template <class KEY, class VALUE, class HASH, class EQUAL>
TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase15Updater::TestCase15Updater(
                                                            const VALUE& value)
: d_value(value)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase15Updater::operator()(
                                                             VALUE      *value,
                                                             const KEY&  key)
{
    (void)key;
    if (*value == VALUE()) {
        *value = d_value;
    }
    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase14Updater::TestCase14Updater(
                                                  int               numSuccess,
                                                  const TestValues& values)
: d_numSuccess(numSuccess)
, d_values(values)
{
    s_testCase14_count = 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase14Updater::operator()(
                                                  VALUE *value, const KEY& key)
{
    // Search for key within d_values.  As we need to copy from next value,
    // assume that it must be within the first 51 members.
    int keyIdx;
    for (keyIdx = 0; keyIdx < 51; ++keyIdx) {
        if (key == d_values[keyIdx].first) {
            break;
        }
    }
    if (keyIdx == 51) { // Should not happen!
        return false;                                                 // RETURN
    }
    *value = d_values[keyIdx+1].second;

    // Increase count, and return true / false based on count.
    ++s_testCase14_count;
    return s_testCase14_count <= d_numSuccess;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase13Updater::TestCase13Updater(
                                                                  bool success)
: d_success(success)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase13Updater::operator()(
                                                             VALUE      *value,
                                                             const KEY&  key)
{
    s_testCase13_key     = key;
    s_testCase13_valueId = TstFacility::getIdentifier(*value);
    *value            = s_testCase13_value;
    return d_success;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_found;

template <class KEY, class VALUE, class HASH, class EQUAL>
KEY TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_key = KEY();

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_valueId;

template <class KEY, class VALUE, class HASH, class EQUAL>
VALUE TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_value = VALUE();

template <class KEY, class VALUE, class HASH, class EQUAL>
KEY TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_key = KEY();

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_valueId;

template <class KEY, class VALUE, class HASH, class EQUAL>
VALUE TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_value = VALUE();

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase14_count;

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase12Updater::operator()(
                                                             VALUE      *value,
                                                             const KEY&  key)
{
    s_testCase12_found = *value != VALUE();
    s_testCase12_key   = key;
    if (s_testCase12_found) {
        s_testCase12_valueId = TstFacility::getIdentifier(*value);
    }
    else {
        ASSERTV(*value == VALUE());
        s_testCase12_valueId = 0;
    }
    *value = s_testCase12_value;
    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase17()
{
    // ------------------------------------------------------------------------
    // TEST MOVABLE 'insert'
    //
    // Concerns:
    //: 1 When 'insert' adds an element, the element has the expected key
    //:   and value, and returns 1.
    //:
    //: 2 When a hash map has one or more elements having the given key,
    //:   exactly one element is updated.  Elements having other keys and other
    //:   elements having the same key are not changed.  The return value is 0.
    //:
    //: 3 Elements having the specified 'key' are found irrespective of the
    //:   bucket in which they reside.
    //:
    //: 4 When an element is added, the memory allocated increases, the
    //:   number of elements increases by 1, and the number of elements in the
    //:   target bucket increases by 1.
    //:
    //: 5 Any memory allocation is exception neutral.
    //:
    //: 6 The method works as expected when the hash map is empty.
    //:
    //: 7 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 8 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Update one existing key.
    //:   2 Confirm that the return value is 0.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the value(s) associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //:   8 Insert one missing key.
    //:   9 Confirm that the return value is 1.
    //:  10 The size of the container increases by 1.
    //:  11 The size of the bucket increases by 1.
    //:  12 Memory in use has increased.
    //:  13 Confirm that the new element has the given key and value.
    //:  14 Confirm that no other elements have changed.
    //:
    //: 3 Each 'insert' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //   bsl::size_t insert(const KEY& key, VALUE&& value);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase18' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST MOVABLE 'insert'" << endl
                      << "=====================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    typedef bslmf::MovableRefUtil MoveUtil;

    // Test empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        VALUE val(VALUES[0].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedInto());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check missing value
        bsl::size_t rc = mX.insert(VALUES[0].first, MoveUtil::move(val));

        ASSERTV(rc, 1 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        ASSERTV(1 == X.size());

        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }

    // Test updating unique keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check existing value
        bsl::size_t rc = mX.insert(VALUES[LENGTH - 1].first,
                                   MoveUtil::move(val));
        ASSERTV(rc, 0 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        // Update did not change size.
        ASSERTV(LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the value has changed to the new value.
        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[LENGTH - 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH].second));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        // Check missing value
        rc = mX.insert(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        ASSERTV(rc, 1 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1
        ASSERTV(LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&value, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value,
                             tj == LENGTH - 1 ? VALUES[LENGTH].second :
                                                VALUES[tj].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val1(VALUES[LENGTH + 2].second, &scratch);
            mX.insert(VALUES[LENGTH + 2].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    } // END Test updating unique keys

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase16()
{
    // ------------------------------------------------------------------------
    // TEST MOVABLE 'setValue'
    //
    // Concerns:
    //: 1 When 'setValue' adds an element, the element has the expected key
    //:   and value, and returns 0.
    //:
    //: 2 When a hash map has one or more elements having the given key,
    //:   exactly one element is updated.  Elements having other keys and other
    //:   elements having the same key are not changed.  The return value is 1.
    //:
    //: 3 Elements having the specified 'key' are found irrespective of the
    //:   bucket in which they reside.
    //:
    //: 4 When an element is added, the memory allocated increases, the
    //:   number of elements increases by 1, and the number of elements in the
    //:   target bucket increases by 1.
    //:
    //: 5 Any memory allocation is exception neutral.
    //:
    //: 6 The method works as expected when the hash map is empty.
    //:
    //: 7 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 8 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Update one existing key.
    //:   2 Confirm that the return value is 1.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the value(s) associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //:   8 Insert one missing key.
    //:   9 Confirm that the return value is correct.
    //:  10 The size of the container increases by 1.
    //:  11 The size of the bucket increases by 1.
    //:  12 Memory in use has increased.
    //:  13 Confirm that the new element has the given key and value.
    //:  14 Confirm that no other elements have changed.
    //:
    //: 3 Each 'setValue' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //   bsl::size_t setValue(const KEY& key, VALUE&& value);
    // ------------------------------------------------------------------------

    // This is equivalent to 'testCase17' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST MOVABLE 'setValue'" << endl
                      << "=======================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    typedef bslmf::MovableRefUtil MoveUtil;

    // Test empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        VALUE val(VALUES[0].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedInto());
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check missing value.
        bsl::size_t rc = mX.setValue(VALUES[0].first, MoveUtil::move(val));
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());
        ASSERTV(rc, 0 == rc);
        ASSERTV(1 == X.size());

        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }

    // Test updating unique keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check existing value.
        bsl::size_t rc = mX.setValue(VALUES[LENGTH - 1].first,
                                     MoveUtil::move(val));
        ASSERTV(rc, 1 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        // Update did not change size.
        ASSERTV(LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the value has changed to the new value.
        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[LENGTH - 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH].second));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        // Check missing value
        rc = mX.setValue(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        ASSERTV(rc, ti, 0 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1
        ASSERTV(LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&value, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value,
                             tj == LENGTH - 1 ? VALUES[LENGTH].second :
                                                VALUES[tj].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val1(VALUES[LENGTH + 2].second, &scratch);
            mX.setValue(VALUES[LENGTH + 2].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}


template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase15()
{
    // --------------------------------------------------------------------
    // TYPE TRAITS
    //
    // Concerns:
    //: 1 The object has the 'bslma::UsesBslmaAllocator' trait.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.
    //
    // Testing:
    //   TYPE TRAITS
    // --------------------------------------------------------------------

    // This is equivalent to 'testCase16' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TYPE TRAITS: Compile Time Test" << endl
                      << "==============================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase14()
{
    // ------------------------------------------------------------------------
    // TEST REHASHING FUNCTIONS
    //
    // Concerns:
    //: 1 Adding elements increases the load factor irrespective of the method
    //:   used (e.g., 'insert', 'setValue').
    //:
    //: 2 After rehashing, the hash map shows the expected values for size (the
    //:   same) number of buckets, and placement of elements within buckets.
    //:
    //: 2 Pushing the load factor above the maximum load factor triggers a
    //:   rehash.
    //:
    //: 4 Lowering the maximum load factor below the current load factor
    //:   triggers a rehash.
    //:
    //: 5 The 'disableRehash' method prevents prevents rehash in any of
    //:   scenario where rehash would otherwise occur.
    //:
    //: 6 The 'enableRehash' method triggers a rehash is warranted when called.
    //:   That is, rehash events are not latched while rehashing is disabled.
    //:
    //: 7 The accessor functions:
    //:   1 Provide values that are predictive of rehash events.
    //:   2 Do not allocate memory.
    //:   3 Are 'const' qualified.
    //:
    //: 8 The rehash process is exception safe.
    //:
    //: 9 The initial value of 'maxLoadFactor' is 1.0.
    //:
    //:10 Initially 'isRehashingEnabled()' returns 'true'.
    //
    // Plan:
    //: 1 For different initial bucket sizes and insertion methods ('insert',
    //:   'setValue', and 'setComputedValue'), create a hash map and insert
    //:   elements:
    //:
    //:   1 When the hash map is created, confirm that 'maxLoadFactor' is 1.0,
    //:     and the 'isRehashingEnabled() is 'true'.
    //:
    //:   2 Confirm that loadFactor increases when elements are added, if the
    //:     number of buckets did not change.
    //:
    //:   3 The number of buckets doubles when we reach number of elements
    //:     higher by one than the initial number of buckets.  Confirm the
    //:     size.  Confirm that the number of elements in each of the two
    //:     buckets that an old bucket was split into are equal to the number
    //:     of elements in the old bucket, except for the bucket where the last
    //:     element was added.
    //:
    //: 2 For different initial bucket sizes, create a hash map and insert
    //:   elements:
    //:
    //:   1 Disable rehashing, and check that the number of buckets does not
    //:     change when it should (i.e., a rehash does not happen).  Enable
    //:     rehashing, and verify that rehashing does not spontaneously happen.
    //:
    //:   2 Verify that 'loadFactor' is bigger than the current
    //:     'maxLoadFactor.'
    //:
    //:   3 Set 'maxLoadFactor' to a large number and confirm that the number
    //:     of buckets does not change.
    //:
    //:   4 Set 'maxLoadFactor' to its previous value (1.0), which is smaller
    //:     than the current loadFactor, and confirm that the number of buckets
    //:     does change.
    //:
    //:   5 Directly call 'rehash' with double the number of the existing
    //:     buckets, and confirm that the number of buckets doubled.  Wrap the
    //:     call with 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST' macro.
    //
    // Testing:
    //   void rehash(bsl::size_t numBuckets);
    //   void disableRehash();
    //   void enableRehash();
    //   void maxLoadFactor(float newMaxLoadFactor);
    //   bool isRehashEnabled() const;
    //   float maxLoadFactor() const;
    //   float loadFactor() const;
    // ------------------------------------------------------------------------

    // This is equivalent to 'testCase15' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST REHASHING FUNCTIONS" << endl
                      << "========================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    const TestValues VALUES;  // contains 52 distinct increasing values
    const int        LENG = 50;

    const bsl::size_t numBucketsArray[] = {2, 4, 8, 16, 32, 64};

    // Regular rehash by adding elements.
    for (bsl::size_t ti = 0; ti < 6; ++ti) {
        const bsl::size_t initialNumBuckets = numBucketsArray[ti];
        const bsl::size_t initialNumStripes = initialNumBuckets / 2;

        for (int i = 0; i <= 5; ++i) { // Insert types

            Obj        mX(initialNumBuckets, initialNumStripes, &supplied);
            const Obj& X = mX;

            bslma::TestAllocatorMonitor sam(&supplied);

            float oldLoadFactor = X.loadFactor();
            ASSERTV(LENG, oldLoadFactor,     0.0 == oldLoadFactor)
            ASSERTV(LENG, X.maxLoadFactor(), 1.0 == X.maxLoadFactor())
            ASSERTV(LENG, X.isRehashEnabled())

            // Confirm no change in memory as result of accessors.
            ASSERTV(sam.isTotalSame());

            // Bucket sizes before rehash
            bsl::vector<bsl::size_t> bucketSizes(initialNumBuckets, &scratch);

            // Loop to insert elements.
            for (bsl::size_t tj = 0; tj < LENG; ++tj) {

                // Confirm the size.
                ASSERTV(tj == X.size());

                // If we are just before rehash, read bucket sizes
                if (tj == initialNumBuckets) {
                    for (bsl::size_t j = 0; j < initialNumBuckets; ++j) {
                        bucketSizes[j] = X.bucketSize(j);
                    }
                }

                switch (i) {
                  case 0: { // insert
                    bsl::size_t rc = mX.insert(VALUES[tj].first,
                                               VALUES[tj].second);
                    ASSERTV(1 == rc)
                  } break;
                  case 1: { // insert move
                    bsl::size_t rc = mX.insert(
                               VALUES[tj].first,
                               bslmf::MovableRefUtil::move(VALUES[tj].second));
                    ASSERTV(1 == rc)
                  } break;
                  case 2: { // setValue
                    bsl::size_t rc = mX.setValue(VALUES[tj].first,
                                                 VALUES[tj].second);
                    ASSERTV(0 == rc)
                  } break;
                  case 3: { // setValue move
                    bsl::size_t rc = mX.setValue(
                               VALUES[tj].first,
                               bslmf::MovableRefUtil::move(VALUES[tj].second));
                    ASSERTV(0 == rc)
                  } break;
                  case 4: { // insertBulk
                    bsl::vector<typename Obj::KVType> insertVec(&scratch);

                    typename Obj::KVType item(VALUES[tj].first,
                                              VALUES[tj].second,
                                              &supplied);
                    insertVec.push_back(item);
                    bsl::size_t rc = mX.insertBulk(
                                                insertVec.begin(),
                                                insertVec.end());
                    ASSERTV(1 == rc)
                  } break;
                  case 5: { // setComputedValue
                    TestCase15Updater testCase15Updater(VALUES[tj].second);

                    typename Obj::VisitorFunction func(testCase15Updater);
                    dam.reset(); // 'bsl::function' uses the default
                                 //  alllocator.

                    int rc = mX.setComputedValue(VALUES[tj].first, func);
                    ASSERTV(0 == rc)
                  } break;
                } // END 'switch'

                // Insert bulk does allocate on the default allocator.
                //
                // Set computed value functor copies around values.
                if (i == 4 ||
                         (i == 5 && bslma::UsesBslmaAllocator<VALUE>::value)) {
                    dam.reset();
                }
                ASSERTV(i, dam.isTotalSame());

                if (tj < initialNumBuckets) {

                    // Verify that the number of buckets has not changed.
                    ASSERTV(initialNumBuckets == X.bucketCount());

                    // Verify that 'loadFactor' increased.
                    float newLoadFactor = X.loadFactor();
                    ASSERTV(newLoadFactor > oldLoadFactor);

                    oldLoadFactor = newLoadFactor;

                } else if (tj < 2 * initialNumBuckets) {

                    // Verify that the number of buckets doubled.
                    ASSERTV(ti, i, tj, initialNumBuckets, X.bucketCount(),
                            2 * initialNumBuckets == X.bucketCount());

                    // If we are just after rehash, check bucket sizes, except
                    // the bucket of the last key added.
                    if (tj == initialNumBuckets) {
                        bsl::size_t lastKeyBucketIdx =
                                               X.bucketIndex(VALUES[tj].first);
                        for (bsl::size_t j = 0; j < initialNumBuckets; ++j) {

                            if (lastKeyBucketIdx != j &&
                                lastKeyBucketIdx != j + initialNumBuckets) {

                                ASSERTV(ti, tj, j,
                                        bucketSizes[j],
                                        X.bucketSize(j),
                                        X.bucketSize(j + initialNumBuckets),
                                        bucketSizes[j] == X.bucketSize(j)
                                                        + X.bucketSize(
                                                       j + initialNumBuckets));
                            }
                        }
                    }
                }
            } // END Loop to insert elements
        } // END Loop on insert types
    } // END Loop on bucket sizes

    // Test 'disable', 'enable', 'maxLoadFactor', and explicit 'rehash'.
    //
    // Note that we skip initial bucket count of 64, as it will not rehash for
    // the test values we use here.
    for (bsl::size_t ti = 0; ti < 5; ++ti) {
        const bsl::size_t initialNumBuckets = numBucketsArray[ti];
        const bsl::size_t initialNumStripes = initialNumBuckets / 2;

        Obj        mX(initialNumBuckets, initialNumStripes, &supplied);
        const Obj& X = mX;

        bslma::TestAllocatorMonitor sam(&supplied);

        // Disable rehash, confirm that it does not allocate memory.
        mX.disableRehash();
        ASSERTV(sam.isTotalSame());

        // Insert values.
        for (bsl::size_t tj = 0; tj < LENG; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENG, X.size(), LENG == X.size())

        // Confirm that no rehash happened.
        ASSERTV(LENG,
                X.bucketCount(),
                initialNumBuckets == X.bucketCount());

        // Enable rehash, confirm no rehash happened.
        sam.reset();
        mX.enableRehash();
        ASSERTV(LENG,
                X.bucketCount(),
                initialNumBuckets == X.bucketCount());

        // Confirm that 'loadFactor' is bigger than 'maxLoadFactor'.
        ASSERTV(LENG, X.loadFactor() > X.maxLoadFactor());

        // Confirm that no memory was allocated in 'enableRehash',
        // 'loadFactor', and 'maxLoadFactor'.
        ASSERTV(sam.isTotalSame());

        // Set 'maxLoadFactor' to a large value, and confirm no rehash
        // happened.
        float curMaxLoadFactor = X.maxLoadFactor();
        mX.maxLoadFactor(1e6);
        ASSERTV(LENG, X.maxLoadFactor(), 1e6 == X.maxLoadFactor());
        ASSERTV(LENG,
                X.bucketCount(),
                initialNumBuckets == X.bucketCount());

        // Set 'maxLoadFactor' to its original value, and confirm rehash
        // happened.
        mX.maxLoadFactor(curMaxLoadFactor);
        ASSERTV(LENG,
                X.bucketCount(),
                initialNumBuckets < X.bucketCount());

        // Directly call 'rehash', doubling the number of buckets, within
        // exception macros, and confirm that the number of buckets doubled.
        bsl::size_t curBucketCount = X.bucketCount();
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            mX.rehash(2 * curBucketCount);
            ASSERTV(X.bucketCount() == curBucketCount * 2 ||
                    X.bucketCount() == curBucketCount);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase13()
{
    // ------------------------------------------------------------------------
    // TEST 'visit'
    //
    // Concerns:
    //: 1 The given functor is executed and is called with the expected
    //:   arguments:
    //:
    //:   1 The value at 'value' is the value attribute associated with
    //:     'key'.
    //:
    //:   2 A change at 'value' changes the element associated with 'key'.
    //:
    //: 2 The functor is called for every element in the hash map irrespective
    //:   of the bucket in which it resides.
    //:
    //: 3 The 'visit' method returns the number of keys visited.
    //:
    //: 4 The "tour" is halted prematurely when the functor returns 'false'.
    //:
    //:   1 The number of elements visited is less than the size of the hash
    //:     map.
    //:
    //:   2 The return value is negative.
    //:
    //: 5 The method works if the hash map is empty.
    //:
    //: 6 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 7 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths with unique keys, we:
    //:   1 Run a visitor that always succeeds.
    //:   2 Update the value of a key with the value of the next key.
    //:   3 Confirm that the return value is correct.
    //:   4 Confirm that the functor was called the correct number of times.
    //:   5 The size of the container does not change.
    //:   6 The sizes of all buckets do not change.
    //:   7 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   8 Confirm that the value associated with each key were updated to
    //:     the new value.
    //:
    //: 3 For hash maps of varying lengths with unique keys, we:
    //:   1 Run a visitor that fails after half the elements.
    //:   2 Update the value of a key with the value of the next key.
    //:   3 Confirm that the return value is correct.
    //:   4 Confirm that the functor was called the correct number of times.
    //:   5 The size of the container does not change.
    //:   6 The sizes of all buckets do not change.
    //:   7 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   8 Confirm that the value associated with each key were updated to
    //:     the new value.
    //:   9 Confirm that the second half of the elements have not changed.
    //
    // Testing:
    //   int visit(const VisitorFunction& visitor);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase14' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'visit'" << endl
                      << "============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // Test empty hash map.
    {
        Obj mX(128, 8, &supplied);  const Obj& X = mX;

        TestCase14Updater testCase14UpdaterSuccess(10, VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' uses the default allocator.

        int rc = mX.visit(visitor);
        ASSERTV(rc, 0 == rc);
        ASSERTV(0 == X.size());
    }

    // Testing with unique values - success
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;
        const int         intLength = static_cast<int>(LENGTH);

        Obj mX(128, 8, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bsl::size_t              bucketCount = X.bucketCount();
        bsl::vector<bsl::size_t> bucketSizes(bucketCount, &supplied);

        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            bucketSizes[i] = X.bucketSize(i);
        }

        TestCase14Updater testCase14UpdaterSuccess(INT_MAX, VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' uses the default allocator.

        bslma::TestAllocatorMonitor sam(&supplied);

        int rc = mX.visit(visitor);

        // Confirm return value and number of functor calls.
        ASSERTV(rc,                     intLength == rc);
        ASSERTV(rc, s_testCase14_count, intLength == s_testCase14_count);

        // Size unchanged.
        ASSERTV(LENGTH == X.size());

        // Bucket count unchanged.
        ASSERTV(bucketCount == X.bucketCount());

        // Bucket sizes did not change.
        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            ASSERTV(bucketSizes[i] == X.bucketSize(i));
        }

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that values changed to the correct value.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            bsl::size_t rc1 = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc1);
            ASSERTV(areEqual(value, VALUES[tj+1].second));
        }

    } // END Testing with unique values - success

    // Testing with unique values - fail
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;
        const int         intLength = static_cast<int>(LENGTH);

        Obj mX(128, 8, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bsl::size_t              bucketCount = X.bucketCount();
        bsl::vector<bsl::size_t> bucketSizes(bucketCount, &supplied);

        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            bucketSizes[i] = X.bucketSize(i);
        }

        TestCase14Updater
                testCase14UpdaterSuccess(static_cast<int>(LENGTH / 2), VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' uses the default allocator.

        bslma::TestAllocatorMonitor sam(&supplied);

        int rc = mX.visit(visitor);

        // Confirm return value and number of functor calls.
        ASSERTV(rc, intLength, -intLength / 2, -(intLength / 2 + 1) == rc);
        ASSERTV((intLength / 2 + 1) == s_testCase14_count);

        // Size unchanged.
        ASSERTV(LENGTH == X.size());

        // Bucket count unchanged.
        ASSERTV(bucketCount == X.bucketCount());

        // Bucket sizes did not change.
        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            ASSERTV(bucketSizes[i] == X.bucketSize(i));
        }

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that values changed to the correct value or stayed the
        // previous value.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            bsl::size_t rc1 = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc1);
            ASSERTV(areEqual(value, VALUES[tj].second) ||
                    areEqual(value, VALUES[tj+1].second));
        }

    } // END Testing with unique values - fail

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase12()
{
    // ------------------------------------------------------------------------
    // TEST 'update'
    //
    // Concerns:
    //: 1 The given functor is executed and is called with the expected
    //:   arguments:
    //:
    //:   1 The value at 'value' is the value attribute associated with
    //:     'key'.
    //:
    //:   2 A change at 'value' changes the element associated with 'key'.
    //:
    //: 2 The functor is called for every element in the hash map having the
    //:   given 'key'.
    //:
    //: 3 The 'update' method returns the number of keys visited if the functor
    //:   returns 'true' in all invocations.
    //:
    //: 4 The "tour" is halted prematurely when the functor returns 'false'.
    //:
    //:   1 The number of elements visited is less than the size of the hash
    //:     map.
    //:
    //:   2 The return value is negative.
    //:
    //: 5 The method works if 'key' is not in the hash map.
    //:
    //: 6 The method works if the hash map is empty.
    //:
    //: 7 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 8 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths with unique keys, we:
    //:   1 Update one existing key.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the value(s) associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //
    // Testing:
    //   int update(const KEY& key, const VisitorFunction& functor);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase13' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'update'" << endl
                      << "=============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
    TestCase13Updater    testCase13UpdaterSuccess(true );
    TestCase13Updater    testCase13UpdaterFail   (false);

    // Test empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Check missing value
        bsl::size_t rc = mX.update(VALUES[0].first, testCase13UpdaterSuccess);
        ASSERTV(rc, 0 == rc);
        ASSERTV(0 == X.size());
    }

    // Test updating unique keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj  mX(128, 8, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        s_testCase13_value = VALUES[LENGTH].second;

        // Check existing value.
        int rc = mX.update(VALUES[LENGTH - 1].first, testCase13UpdaterSuccess);
        ASSERTV(rc, 1 == rc);
        ASSERTV(VALUES[LENGTH - 1].first, s_testCase13_key,
                VALUES[LENGTH - 1].first  == s_testCase13_key);
        ASSERTV(TstFacility::getIdentifier(VALUES[LENGTH - 1].second),
                s_testCase13_valueId,
                TstFacility::getIdentifier(VALUES[LENGTH - 1].second)
                                          == s_testCase13_valueId);
        // Update did not change size.
        ASSERTV(LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the value has changed to the new value.
        bsl::size_t rc1 = X.getValue(&value, VALUES[LENGTH - 1].first);
        ASSERTV(1 == rc1);
        ASSERTV(areEqual(value, VALUES[LENGTH].second));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc1 = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc1);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        bucketIdx      = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        s_testCase13_value = VALUES[LENGTH + 1].second;

        // Check missing value
        rc = mX.update(VALUES[LENGTH + 1].first, testCase13UpdaterSuccess);
        ASSERTV(rc, 0 == rc);

        // Confirm that values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            rc1 = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc1);
            ASSERTV(areEqual(value,
                             tj == LENGTH - 1 ? VALUES[LENGTH].second :
                                                VALUES[tj].second));
        }
    } // END Testing with unique values

    // Test coverage with a small number of buckets.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        // Check existing value
        int rc = mX.update(VALUES[LENGTH - 1].first,
                                        testCase13UpdaterSuccess);
        ASSERTV(rc, 1 == rc);
        ASSERTV(VALUES[LENGTH - 1].first, s_testCase13_key,
                VALUES[LENGTH - 1].first  == s_testCase13_key);
        ASSERTV(TstFacility::getIdentifier(VALUES[LENGTH - 1].second),
                s_testCase13_valueId,
                TstFacility::getIdentifier(VALUES[LENGTH - 1].second)
                                          == s_testCase13_valueId);

        rc = mX.update(VALUES[LENGTH - 1].first,
                                        testCase13UpdaterFail);
        ASSERTV(rc, -1 == rc);

        // Check missing value
        rc = mX.update(VALUES[LENGTH + 1].first, testCase13UpdaterSuccess);
        ASSERTV(rc, 0 == rc);
    }

    // CONCERN: In no case does memory come from the default allocator.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase11()
{
    // ------------------------------------------------------------------------
    // TEST 'setComputedValue'
    //
    // Concerns:
    //: 1 When 'setComputedValue' adds an element, the element has the expected
    //:   key and value, and returns 0.
    //:
    //: 2 When 'setComputedValue' updates element(s), all elements with that
    //:   given key, and no others, are updated to the given value, and the
    //:   number of elements updated is returned.
    //:
    //: 3 Elements having the specified 'key' are found irrespective of the
    //:   bucket in which they reside.
    //:
    //: 4 When an element is added, the memory allocated increases, the number
    //:   of elements increases by 1, and the number of elements in the target
    //:   bucket increases by 1.
    //:
    //: 5 The expected 'functor' is called with the expected arguments.
    //:   1 If 'found' is 'true', the value of the existing element is found at
    //:     the provided address.
    //:   2 If 'found' is 'false', 'VALUE()' is found
    //:
    //: 6 When the 'functor' changes the value of the provide value address,
    //:   the element has its value changed.
    //:
    //: 7 Any memory allocation is exception neutral.
    //:
    //: 8 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 9 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Update one existing key.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the value(s) associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //:   8 Insert one missing key.
    //:   9 Confirm that the return value is correct.
    //:  10 The size of the container increases by 1.
    //:  11 The size of the bucket increases by 1.
    //:  12 Memory in use has increased.
    //:  13 Confirm that the new element has the given key and value.
    //:  14 Confirm that no other elements have changed.
    //
    // Testing:
    //   int setComputedValue(const KEY& key, visitor);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase12' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'setComputedValue'" << endl
                      << "=======================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
    TestCase12Updater    testCase12Updater;

    // Test empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Check missing value
        int rc1 = mX.setComputedValue(VALUES[0].first, testCase12Updater);
        ASSERTV(rc1, 0 == rc1);
        ASSERTV(1 == X.size());

        bsl::size_t rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUE()));
    }

    // Testing with unique values.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        s_testCase12_value = VALUES[LENGTH].second;

        // Check existing value
        int rc1 = mX.setComputedValue(VALUES[LENGTH - 1].first,
                                      testCase12Updater);
        ASSERTV(rc1, 1 == rc1);
        ASSERTV(true                      == s_testCase12_found);
        ASSERTV(VALUES[LENGTH - 1].first, s_testCase12_key,
                VALUES[LENGTH - 1].first  == s_testCase12_key);
        ASSERTV(TstFacility::getIdentifier(VALUES[LENGTH - 1].second),
                s_testCase12_valueId,
                TstFacility::getIdentifier(VALUES[LENGTH - 1].second)
                                          == s_testCase12_valueId);

        // Update did not change size.
        ASSERTV(LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the value has changed to the new value.
        bsl::size_t rc = X.getValue(&value, VALUES[LENGTH - 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH].second));

        // No change in memory allocation if VALUE is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }
        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        bucketIdx          = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize      = X.bucketSize(bucketIdx);
        s_testCase12_value = VALUES[LENGTH + 1].second;

        // Check missing value.
        rc1 = mX.setComputedValue(VALUES[LENGTH + 1].first, testCase12Updater);
        ASSERTV(rc1, 0 == rc1);

        ASSERTV(false                     == s_testCase12_found);
        ASSERTV(VALUES[LENGTH + 1].first, s_testCase12_key,
                VALUES[LENGTH + 1].first  == s_testCase12_key);

        // Insert increased size by 1.
        ASSERTV(LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&value, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value,
                             tj == LENGTH - 1 ? VALUES[LENGTH].second :
                                                VALUES[tj].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {

            s_testCase12_value = VALUES[LENGTH + 2].second;

            mX.setComputedValue(VALUES[LENGTH + 2].first, testCase12Updater);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    } // END Testing with unique values

    // CONCERN: In no case does memory come from the default allocator.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase10()
{
    // ------------------------------------------------------------------------
    // TEST 'setValue'
    //
    // Concerns:
    //: 1 When 'setValue' adds an element, the element has the expected key
    //:   and value, and returns 0.
    //:
    //: 2 When 'setValue' updates element(s), all elements with that given
    //:   key, and no others, are updated to the given value, and the number
    //:   of elements updated is returned.
    //:
    //: 3 Elements having the specified 'key' are found irrespective of the
    //:   bucket in which they reside.
    //:
    //: 4 When an element is added, the memory allocated increases, the
    //:   number of elements increases by 1, and the number of elements in the
    //:   target bucket increases by 1.
    //:
    //: 5 Any memory allocation is exception neutral.
    //:
    //: 6 The method works as expected when the hash map is empty.
    //:
    //: 7 QoI: When element(s) are updated, and the 'VALUE' type is
    //:   non-allocating, no memory is allocated or deallocated.
    //:
    //: 8 QoI: There is no temporary memory allocation from any allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Update one existing key.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the value(s) associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //:   8 Insert one missing key.
    //:   9 Confirm that the return value is correct.
    //:  10 The size of the container increases by 1.
    //:  11 The size of the bucket increases by 1.
    //:  12 Memory in use has increased.
    //:  13 Confirm that the new element has the given key and value.
    //:  14 Confirm that no other elements have changed.
    //:
    //: 3 Each 'setValue' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //   bsl::size_t setValue(const KEY& key, value);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST 'setValue'" << endl
                      << "===============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];
    bsl::vector<VALUE>   values(&scratch);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // Test empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Check missing value
        bsl::size_t rc = mX.setValue(VALUES[0].first, VALUES[0].second);
        ASSERTV(rc, 0 == rc);
        ASSERTV(1 == X.size());

        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }

    // Test updating unique keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        // Check existing value.
        bsl::size_t rc = mX.setValue(VALUES[LENGTH - 1].first,
                                     VALUES[LENGTH].second);
        ASSERTV(rc, 1 == rc);

        // Update did not change size.
        ASSERTV(LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the value has changed to the new value.
        rc = X.getValue(&value, VALUES[LENGTH - 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH].second));

        // No change in memory allocation if VALUE is non-allocating
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        // Check missing value.
        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        rc = mX.setValue(VALUES[LENGTH + 1].first, VALUES[LENGTH + 1].second);
        ASSERTV(rc, 0 == rc);

        // Insert increased size by 1.
        ASSERTV(LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&value, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, tj == LENGTH - 1 ? VALUES[LENGTH].second
                                                     : VALUES[tj].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            mX.setValue(VALUES[LENGTH + 1].first, VALUES[LENGTH + 1].second);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase9()
{
    // ------------------------------------------------------------------------
    // TEST 'insertBulk'
    //   Note that 'insertBulk' is implemented independently of 'insert' (for
    //   performance), hence all the concerns of 'insert' have to be retested
    //   independently.
    //
    // Concerns:
    //: 1 Each element between 'first' (inclusive) and 'last' (exclusive) is
    //:   inserted, irrespective of the distance between 'first' and 'last'.
    //:   In particular:
    //:   o 'last - first == 0'
    //:   o 'last - first == 1'
    //:   o 'last - first == 2'
    //:   o 'last - first >  2'
    //:
    //: 2 Insertion updates the values of existing elements having 'key'.
    //:
    //: 3 Updates of existing elements occurs irrespective of their bucket.
    //:
    //: 4 The addition of new elements is reflected in the size of the
    //:   element's bucket and the overall size of the container.
    //:
    //: 5 The "insertion" of elements (addition of, or update of elements) has
    //:   do not change elements that do not have the specified 'key' value.
    //:
    //: 6 Elements can be inserted into an empty hash map.
    //:
    //: 7 The update of existing elements and addition of new elements are each
    //:   exception safe.
    //:
    //: 8 The iterator arguments need provide read-only access.
    //:
    //: 9 QoI: Asserted precondition violations are detected when enabled.
    //:
    //:10 QoI: Insert allocates memory from the allocator.
    //:
    //:11 QoI: Temporary allocation are taken from the default allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Bulk insert:
    //:     1 Using different range lengths: i.e., one a key one at a time, two
    //:       at a time, and three at a time.  Also try to insert 0 keys.
    //:     2 Show that elements can be added/updated in every bucket of the
    //:       hash map.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container increases by the expected amount.
    //:   4 The size of the associated buckets increase by the expected amount.
    //:   5 Memory in use has increased.
    //:   6 Trying to insert an existing key results in an update.
    //:   7 Existing keys having other keys are not changed.
    //:   8 Existing keys having with inserted or updated keys show the correct
    //:     value(s).
    //:   9 For a non empty range of keys, temporary memory is allocated from
    //:     the default allocator.
    //:
    //: 3 In each instance, 'const'-interators are provided to the method.
    //:
    //: 4 Verify that, within 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros
    //:   'insertBulk' does not leak memory, and that if it completed, and
    //:   the number of elements of the hash map increased, some memory has
    //:   been allocated.
    //
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid indexes, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).
    //
    // Testing:
    //   bsl::size_t insertBulk(RANDOMIT first, last);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase10' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'insertBulk'" << endl
                      << "=================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    const TestValues VALUES;  // contains 52 distinct increasing values

    const bsl::size_t MAX_LENGTH = 15;

    bsl::size_t rc;

    // Try to insert into an empty hash map.
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Keys to be inserted with 'insertBulk'
        bsl::vector<typename Obj::KVType> insertVec(&supplied);
        for (bsl::size_t tj = 0; tj < MAX_LENGTH; ++tj) {
            typename Obj::KVType item(VALUES[tj].first,
                                      VALUES[tj].second,
                                      &supplied);
            insertVec.push_back(item);
        }
        rc = mX.insertBulk(insertVec.cbegin(), insertVec.cend());

        ASSERTV(MAX_LENGTH, rc, MAX_LENGTH == rc);
        ASSERTV(MAX_LENGTH, X.size(), MAX_LENGTH == X.size());
        // Confirm that the keys inserted have the correct values.
        for (bsl::size_t tj = 0; tj < MAX_LENGTH; ++tj) {
            VALUE value;
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }
    }

    // Test unique keys
    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

            // Small number of buckets and stripes
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t v = 0; v < LENGTH; ++v) {
                mX.insert(VALUES[v].first, VALUES[v].second);
            }
            ASSERTV(LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            // Loop on the length of the key vector passed to 'insertBulk'.
            bsl::size_t tkOffset = 0;
            for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                // special case: empty vector of keys
                if (tk == 0) {
                    bsl::vector<typename Obj::KVType> insertVec(&scratch);
                    sam.reset();
                    rc = mX.insertBulk(insertVec.cbegin(), insertVec.cbegin());

                    ASSERTV(LENGTH == X.size());
                    ASSERTV(sam.isInUseSame());
                    continue;
                }

                // Here: '1 <= tk <= 3'
                //
                // Insert a group of 'tk' elements with the same key as
                // existing ones but different values, and check:
                //: 1 The 'insertBulk' returned the correct return code.
                //: 2 No elements were added.
                //: 3 Bucket sizes have not changed.
                //: 4 Some temporary memory was allocated.
                //: 5 If the value type is not allocating, the allocator
                //    did not allocate.

                ASSERTV(ti, tj, tk, tkOffset, X.size(),
                        LENGTH + tkOffset == X.size());
                if (tkOffset + tk > LENGTH) {
                    // Skip sizes that do not properly fit.
                    continue;
                }
                bsl::size_t              bucketCount = X.bucketCount();
                bsl::vector<bsl::size_t> bucketSizes(bucketCount,
                                                     &scratch);

                bsl::vector<typename Obj::KVType> insertVec(&scratch);
                for (bsl::size_t i = 0; i < tk; ++i) {
                    typename Obj::KVType item(
                                           VALUES[i + tkOffset].first,
                                           VALUES[i + tkOffset + 1].second,
                                           &scratch);
                    insertVec.push_back(item);
                }
                for (bsl::size_t j = 0; j < bucketCount; ++j) {
                    bucketSizes[j] = X.bucketSize(j);
                }

                sam.reset();
                dam.reset();

                rc = mX.insertBulk(insertVec.cbegin(), insertVec.cend());

                // Confirm no new records.
                ASSERTV(0 == rc);

                // Confirm temporary memory usage of default allocator
                ASSERTV(dam.isTotalUp());

                // No change in memory allocation if 'VALUE' is
                // non-allocating.
                if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                    ASSERTV(sam.isInUseSame());
                }

                // Confirm size did not change.
                ASSERTV(LENGTH + tkOffset == X.size());

                // Confirm bucket sizes did not change.
                for (bsl::size_t j = 0; j < bucketCount; ++j) {
                    ASSERTV(bucketSizes[j] == X.bucketSize(j));
                }

                // Here: '1 <= tk <= 3'
                //
                // Insert a group of 'tk' elements with a different key from
                // existing ones, and check:
                //: 1 The 'insertBulk' returned the correct return code.
                //: 2 'tk' elements were added.
                //: 3 The total size of buckets increased by 'tk'.
                //: 4 Some temporary memory was allocated.
                //: 5 Some memory was allocated.

                bsl::vector<typename Obj::KVType> insertVec1(&scratch);
                for (bsl::size_t i = 0; i < tk; ++i) {
                    typename Obj::KVType item(
                                          VALUES[i + tkOffset + LENGTH].first,
                                          VALUES[i + tkOffset + LENGTH].second,
                                          &scratch);
                    insertVec1.push_back(item);
                }

                sam.reset();
                dam.reset();

                rc = mX.insertBulk(insertVec1.cbegin(), insertVec1.cend());

                // Confirm 'tk' new records.
                ASSERTV(tk == rc);

                // Confirm temporary memory usage of default allocator.
                ASSERTV(dam.isTotalUp());

                // Memory allocation increased.
                ASSERTV(sam.isInUseUp());

                // Confirm size increased.
                ASSERTV(LENGTH + tkOffset + tk == X.size());

                // Confirm total bucket sizes increased.
                if (bucketCount == X.bucketCount()) {
                    bsl::size_t newSumBucketSize = 0;
                    bsl::size_t oldSumBucketSize = 0;
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        newSumBucketSize += X.bucketSize(j);
                        oldSumBucketSize += bucketSizes[j];
                    }
                    ASSERTV(oldSumBucketSize + tk == newSumBucketSize);
                }

                // Confirm that the inserted keys have the appropriate values.
                for (bsl::size_t i = 0; i < tk; ++i) {
                    bsl::size_t j = i + tkOffset + LENGTH;
                    VALUE       value = VALUE();
                    rc = X.getValue(&value, VALUES[j].first);
                    ASSERTV(1 == rc);
                    ASSERTV(areEqual(value, VALUES[j].second));
                }

                // Confirm that the updated keys have the appropriate values.
                for (bsl::size_t i = 0; i < tk; ++i) {
                    bsl::size_t j = i + tkOffset;
                    VALUE       value = VALUE();
                    rc = X.getValue(&value, VALUES[j].first);
                    ASSERTV(1 == rc);
                    ASSERTV(areEqual(value, VALUES[j + 1].second));
                }

                tkOffset += tk;
            } // END Loop on 'tk'

            // Confirm that the untouched keys have the same values.
            for (bsl::size_t i = tkOffset; i < LENGTH; ++i) {
                VALUE value = VALUE();
                rc = X.getValue(&value, VALUES[i].first);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value, VALUES[i].second));
            }
        } // END Loop on 'tj'
    } // END test for unique keys

    if (verbose) cout << "Exception Testing" << endl;
    {
        bslma::TestAllocator        scratch("scratch",   veryVeryVeryVerbose);
        bslma::TestAllocator        supplied("supplied", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor sam(&supplied);

        const TestValues VALUES;  // contains 52 distinct increasing values
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            sam.reset();

            Obj mX(8, 4, &supplied);

            // Keys to be inserted with 'insertBulk'
            bsl::vector<typename Obj::KVType> insertVec(&supplied);
            for (bsl::size_t tj = 0; tj < 2; ++tj) {
                typename Obj::KVType item(VALUES[tj].first,
                                          VALUES[tj].second,
                                          &scratch);
                insertVec.push_back(item);
            }
            // Empty
            mX.insertBulk(insertVec.cbegin(), insertVec.cend());

            // Existing
            mX.insertBulk(insertVec.cbegin(), insertVec.cend());

            // New
            insertVec.clear();
            for (bsl::size_t tj = 2; tj < 4; ++tj) {
                typename Obj::KVType item(VALUES[tj].first,
                                          VALUES[tj].second,
                                          &scratch);
                insertVec.push_back(item);
            }
            mX.insertBulk(insertVec.cbegin(), insertVec.cend());

            ASSERTV(sam.isInUseUp());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
        ASSERTV(sam.isInUseSame());
    }

    if (verbose) cout << "Negative Testing" << endl;
    {
        if (veryVerbose) cout << "\t'insertBulk' Method" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX;

        bsl::vector<typename Obj::KVType> insertVec(&scratch);
        typename Obj::KVType              item(VALUES[0].first,
                                               VALUES[0].second,
                                               &scratch);
        insertVec.push_back(item);

        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL(mX.insertBulk(insertVec.cend()  , insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulk(insertVec.cbegin(), insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulk(insertVec.cbegin(), insertVec.cend()));
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase8()
{
    // ------------------------------------------------------------------------
    // TEST 'clear'
    //
    // Concerns:
    //: 1 The 'clear' removes all items from the hash map irrespective of
    //:   bucket.
    //:
    //: 2 QoI: 'clear' bring the memory allocated to its initial state, if no
    //:   rehash.
    //:
    //: 3 The 'clear' method is idempotent.
    //
    // Plan:
    //: 1 Using the loop based approach, call 'clear' for objects having
    //:   different lengths and verify that size dropped to 0, and memory was
    //:   returned.
    //:
    //: 2 Verify that a second call to 'clear' does not change the state.
    //:
    //: 3 Some of our test objects have elements in every bucket.
    //
    // Testing:
    //   void clear();
    // ------------------------------------------------------------------------

    // This is equivalent to 'testCase9' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'clear'" << endl
                      << "============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        bslma::TestAllocatorMonitor sam(&supplied);

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insert(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());
        bslma::TestAllocatorMonitor sam1(&supplied);

        mX.clear();
        ASSERTV(0 == X.size());
        // If anything was allocated, 'clear' releases some memory.  Otherwise,
        // no change.
        if (LENGTH > 0) {
            ASSERTV(sam1.isInUseDown());
        } else {
            ASSERTV(sam1.isInUseSame());
        }

        // Check that if no rehash, we get to the initial state.
        if (8 == X.bucketCount()) {
            ASSERTV(sam.isInUseSame());
        }

        // Check idempotence.
        mX.clear();
        ASSERTV(0 == X.size());
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase7()
{
    // ------------------------------------------------------------------------
    // TEST 'eraseBulk'
    //   Note that 'eraseBulk' is implemented independently of 'erase' (for
    //   performance), hence all the concerns of 'erase' have to be retested
    //   independently.
    //
    // Concerns:
    //: 1 The 'eraseBulk' method removes the specified keys from the hash.
    //:   The method returns the number of items removed.
    //:
    //: 2 Erasure occurs for every key between 'first' (inclusive) and 'last'
    //:   (exclusive), and no other.
    //:
    //: 3 Erasure occurs irrespective of the bucket in which an target element
    //:   resides.
    //:
    //: 4 Erasure is reflected in the size of the element's bucket and the size
    //:   of the container.
    //:
    //: 5 There is no error if there are no elements having any of the key
    //:   values.  Including an empty hash map, and an empty range.
    //:
    //: 6 The iterator arguments need provide read-only access.
    //:
    //: 7 QoI: Asserted precondition violations are detected when enabled.
    //:
    //: 8 QoI: Erasure returns memory to the allocator.
    //:
    //: 9 QoI: Temporary allocation are taken from the default allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Erase each key one at a time, two at a time, and three at a time.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container decreases by the expected amount.
    //:   4 The size of the associated buckets decrease by the expected amount.
    //:   5 Memory in use has decreased.
    //:   6 Erasing a non-existent key fails, and does not change memory in
    //:     use.
    //:   7 We iterate until the hash map is empty.
    //:   8 For a non empty range of keys, temporary memory is allocated from
    //:     the default allocator.
    //:
    //: 3 In each instance, 'const'-interators are provided to the method.
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid indexes, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).
    //
    // Testing:
    //   bsl::size_t eraseBulk(RANDOMIT first, last);
    // ------------------------------------------------------------------------

    // This is equivalent to portion of 'testCase8' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'eraseBulk'" << endl
                      << "================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // Try to erase from an empty hash map.

    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Keys to be erased with 'eraseBulk'.
        bsl::vector<KEY> eraseKeys(&supplied);
        for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
            eraseKeys.push_back(VALUES[v].first);
        }
        bsl::size_t count = mX.eraseBulk(eraseKeys.cbegin(), eraseKeys.cend());
        ASSERTV(0 == count);
        ASSERTV(0 == X.size());
    }

    // Test unique keys

    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

            // Loop on the length of the key vector passed to 'eraseBulk'.
            for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                // arbitrary number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insert(VALUES[v].first, VALUES[v].second);
                }
                ASSERTV(LENGTH == X.size());

                bslma::TestAllocatorMonitor sam(&supplied);

                // special case: empty vector of keys
                if (tk == 0) {
                    bsl::vector<KEY> eraseKeys(&supplied);

                    bsl::size_t count = mX.eraseBulk(eraseKeys.cbegin(),
                                                     eraseKeys.cbegin());
                    ASSERTV(0 == count);
                    ASSERTV(LENGTH == X.size());
                    ASSERTV(sam.isInUseSame());
                    continue;
                }

                // Here: '1 <= tk <= 3'
                //
                // In a loop, erase all elements in groups of 'tk' elements,
                // check that each 'eraseBulk' is successful, and that the
                // final number of elements is 0.  The last step may contain
                // keys that were already removed.

                ASSERTV(LENGTH == X.size());
                for (bsl::size_t v = 0; v < LENGTH; v += tk) {
                    bsl::vector<KEY> eraseKeys(tk, &supplied);
                    // Index and size of buckets related to the keys in the
                    // current 'eraseKeys' vector.
                    bsl::vector<bsl::size_t> bucketIdxs (tk, &supplied);
                    bsl::vector<bsl::size_t> bucketSizes(tk, &supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t idx = (v + i) % LENGTH;
                        eraseKeys  [i] = VALUES[idx].first;
                        bucketIdxs [i] = X.bucketIndex(VALUES[idx].first);
                        bucketSizes[i] = X.bucketSize(bucketIdxs[i]);
                    }
                    sam.reset();

                    bsl::size_t rc = mX.eraseBulk(eraseKeys.cbegin(),
                                                  eraseKeys.cend());
                    bsl::size_t expNumErased =
                                        LENGTH - v >= tk ? tk : LENGTH - v;
                    bsl::size_t expSize =
                                    LENGTH - v >= tk ? LENGTH - v - tk : 0;
                    ASSERTV(ti, tj, tk, v, rc, expNumErased,
                            rc == expNumErased);
                    ASSERTV(ti, tj, tk, v, rc, expSize,
                            expSize == X.size());

                    // Confirm reduction in size for each of the buckets.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t newBucketSize =
                                               X.bucketSize(bucketIdxs[i]);
                        if (expNumErased == tk) {
                            ASSERTV(bucketSizes[i] > newBucketSize);
                        } else {
                            ASSERTV(bucketSizes[i] >= newBucketSize);
                        }
                    }

                    // Confirm reduction in memory use.
                    ASSERTV(sam.isInUseDown());

                    // Confirm temporary memory usage of default allocator.
                    ASSERTV(ti, tj, tk, v, rc, expNumErased,
                            dam.isTotalUp());
                }
                ASSERTV(0 == X.size());
            } // END Loop on 'tk'
        } // END Loop on 'tj'
    } // END test for unique keys

    if (verbose) cout << "Negative Testing" << endl;
    {
        if (veryVerbose) cout << "\t'eraseBulk' Method" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        Obj mX;

        bsl::vector<KEY> eraseKeys(1, &supplied);
        eraseKeys[0] = VALUES[0].first;

        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL(mX.eraseBulk(eraseKeys.cend()  , eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulk(eraseKeys.cbegin(), eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulk(eraseKeys.cbegin(), eraseKeys.cend()));
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase6()
{
    // ------------------------------------------------------------------------
    // TEST 'erase'
    //
    // Concerns:
    //: 1 Erased elements are no longer found in the container.
    //:
    //: 2 Elements having keys other than the specified 'key' are not changed.
    //:
    //: 3 Erased elements are reflected by a reduction in the 'size()' of the
    //:   container.
    //:
    //: 4 Erased elements reflected by a reduction in the size of the bucket
    //:   where they resided.
    //:
    //: 5 The return value equals the number of elements erased.
    //:
    //: 6 Elements can be erased from any bucket.
    //:
    //: 7 Erase fails (returns 0) on an empty container.
    //:
    //: 8 QoI: Erasing an element returns memory to the allocator.
    //
    // Plan:
    //: 1 Test the empty hash map in a standalone case.
    //:
    //: 2 For hash maps of varying lengths, we:
    //:   1 Erase each key one at a time.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container decreases by the expected amount.
    //:   4 The size of the bucket decreases by the expected amount.
    //:   5 Memory in use has decreased.
    //:   6 Erasing a non-existent key fails, and does not change memory in
    //:     use.
    //:   7 We iterate until the hash map is empty.
    //
    // Testing:
    //   bsl::size_t erase(const KEY& key);
    // ------------------------------------------------------------------------

    // This is equivalent to portion 'testCase7' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'erase'" << endl
                      << "============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // Try to erase from an empty hash map.
    {
        Obj mX(8, 4, &supplied);

        bsl::size_t rc = mX.erase(VALUES[0].first);
        ASSERTV(rc == 0);
    }

    // This is for unique keys.
    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

            // arbitrary number of buckets and stripes
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t v = 0; v < LENGTH; ++v) {
                mX.insert(VALUES[v].first, VALUES[v].second);
            }

            bslma::TestAllocatorMonitor sam(&supplied);

            // In a loop, erase all elements and check that each erase is
            // successful, and that the final number of elements is 0.

            ASSERT(LENGTH == X.size());
            for (bsl::size_t v = 0; v < LENGTH; ++v) {
                sam.reset();
                // Find the bucket where the key resides.
                bsl::size_t bucketIdx     = X.bucketIndex(VALUES[v].first);
                bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

                bsl::size_t rc = mX.erase(VALUES[v].first);
                ASSERT(rc == 1);
                ASSERT(LENGTH - v - 1 == X.size());

                bsl::size_t newBucketSize = X.bucketSize(bucketIdx);
                ASSERT(newBucketSize == oldBucketSize - 1);

                ASSERT(sam.isInUseDown());
                sam.reset();

                // Try to erase again the already erased key.
                rc = mX.erase(VALUES[v].first);
                ASSERT(rc == 0);
                ASSERT(LENGTH - v - 1 == X.size());
                ASSERT(sam.isInUseSame());
            }
            ASSERT(0 == X.size());
        }
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase5()
{
    // ------------------------------------------------------------------------
    // TEST 'getValue'
    //
    // Concerns:
    //: 1 'getValue' succeeds only if the key is in hash map, and then returns
    //:   the correct value.
    //:
    //: 2 'getValue' fails if the key is not in the hash map.
    //:
    //: 3 'getValue' allocates no memory.
    //:
    //: 4 'getValue' is 'const' qualified.
    //:
    //: 5 'getValue' works irrespective of which bucket/stripe the element
    //:   resides.
    //:
    //: 6 'getValue' works as expected when there are multiple instances of the
    //:   'key' in the hash map.
    //
    // Plan:
    //: 1 Test that calling 'getValue' on an empty object fails.
    //:
    //: 2 Create objects of different lengths having unique key values.
    //:   1 Confirm that all inserted keys are found by 'getValue'.
    //:   2 Confirm that 'getValue' fails to find keys that were not inserted.
    //:   3 Insert the keys that were not found, and confirm that they are now
    //:     found.
    //:
    //: 3 In some cases we have an element in every bucket and confirm that all
    //:   buckets are searched.
    //:
    //: 4 Create objects of different lengths having duplicate keys.  In some
    //:   cases the associated values are the same, and in some are different.
    //:   1 Confirm that all inserted keys are found by 'getValue'.
    //:   2 Confirm that the value returned is the expected value (one of the
    //:     expected values).
    //:
    //: 5 'getValue' is called using a 'const' alias to the object under test.
    //
    // Testing:
    //   bsl::size_t getValue(VALUE *value, key) const;
    // ------------------------------------------------------------------------

    // This is similar to portion of 'testCase5' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "TEST 'getValue'" << endl
                      << "===============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    const TestValues     VALUES;  // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 15;
    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // Test for an empty hash map.
    {
        // arbitrary number of stripes and buckets
        Obj mX(64, 4, &supplied);  const Obj& X = mX;

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t rc = X.getValue(&value, VALUES[0].first);
        ASSERT(0 == rc);
        ASSERT(sam.isTotalSame());
    }

    // Test with unique keys
    {
        // Test with one key missing and then inserted.  This also tests that
        // all buckets are covered, as the number of buckets is small compared
        // to the number of values.

        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
            // Small number of buckets
            Obj mX(8, 2, &supplied);  const Obj& X = mX;

            mX.insert(VALUES[ti].first, VALUES[ti].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&value, VALUES[ti].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(value, VALUES[ti].second));

            rc = X.getValue(&value, VALUES[ti + 1].first);
            ASSERT(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);

            sam.reset();

            rc = X.getValue(&value, VALUES[ti + 1].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(value, VALUES[ti + 1].second));
            ASSERT(sam.isTotalSame());
        }

        // Test with two keys missing and then inserted.

        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 2; ++ti) {

            // arbitrary number of stripes and buckets
            Obj mX(64, 4, &supplied);  const Obj& X = mX;

            mX.insert(VALUES[ti].first, VALUES[ti].second);
            mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&value, VALUES[ti].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[ti].second));

            rc = X.getValue(&value, VALUES[ti + 2].first);
            ASSERTV(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);

            sam.reset();

            rc = X.getValue(&value, VALUES[ti + 2].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[ti + 2].second));
            ASSERT(sam.isTotalSame());
        }
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //
    // Concerns:
    //: 1 Each accessor method is declared 'const'.
    //:
    //: 2 Accessors that take arguments can take 'const'-qualified arguments.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 Each accessor returns the expected value.
    //:
    //: 5 QoI: 'bucketSize()': Asserted precondition violations are detected
    //:   when enabled.
    //
    // Plan:
    //: 1 Except for 'hashFunction' and 'equalFunction', the BOOTSTRAP tests in
    //:   case 3 have demonstrated that all basic accessors return values that
    //:   are consistent with the operations on the hash map.  What remains:
    //:
    //:   1 Demonstrate that these methods are 'const'-qualified by invoking
    //:     them on a 'const' alias to a hash map.
    //:
    //:   2 Demonstrate that accessors that take arguments can take
    //:     'const'-qualified arguments, but remains is to confirm that these
    //:
    //: 2 Demonstrate that the 'hashFunction' and 'equalFunction' methods
    //:   return the expected results by confirming that the objects they
    //:   return have the expected 'id' attribute.
    //:
    //: 3 Confirm that no memory has been allocated by any allocator.
    //:
    //: 5 Accessors:
    //:
    //:   1 The accessors listed below are checked for consistency with the
    //:     expected results after each operation.  They are considered
    //:     fully-tested after this test case.
    //:
    //:   2 Accessors always are invoked via 'const' qualified aliases to the
    //:     objects under test.
    //:
    //:   3 'bucket_size': Verify that, in appropriate build modes, defensive
    //:     checks are triggered for invalid indexes, but not triggered for
    //:     adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
    //
    // Testing:
    //   HASH hashFunction() const;
    //   EQUAL equalFunction() const;
    //   bsl::size_t bucketIndex(const KEY& key) const;
    //   bsl::size_t bucketCount() const;
    //   bsl::size_t bucketSize(bsl::size_t index) const;
    //   bool empty() const;
    //   bsl::size_t numStripes() const;
    //   bsl::size_t size() const;
    //   bslma::Allocator *allocator() const;
    // ------------------------------------------------------------------------

    // This is identical to 'testCase4' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "BASIC ACCESSORS" << endl
                      << "===============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    const KEY   cKEY   = TstFacility::create<KEY  >(0);
    const VALUE cVALUE = TstFacility::create<VALUE>(0);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // arbitrary number of stripes and buckets
    Obj mX(128, 32, &supplied);  const Obj& X = mX;

    bsl::size_t rc = mX.insert(cKEY, cVALUE);
    ASSERT(1 == rc);

    bslma::TestAllocatorMonitor sam(&supplied);

    ASSERT(0 == X.hashFunction().id());
    ASSERT(0 == X.equalFunction().id());

    ASSERT(       0 == X.bucketIndex(cKEY));
    ASSERT(     128 == X.bucketCount());
    ASSERT(       1 == X.bucketSize(0));
    ASSERT(   false == X.empty());
    ASSERT(      32 == X.numStripes());
    ASSERT(       1 == X.size());
    ASSERT(&supplied == X.allocator());

    ASSERT(sam.isTotalSame());

    if (verbose) cout << "Negative Testing" << endl;
    {
        if (veryVerbose) cout << "\t'bucketSize' Method" << endl;

        Obj mX(8, 4, &supplied); const Obj& X = mX;

        mX.insert(cKEY, cVALUE);

        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL(X.bucketSize(-1));
        ASSERT_PASS(X.bucketSize( 0));
        ASSERT_PASS(X.bucketSize(X.bucketCount() - 1));
        ASSERT_FAIL(X.bucketSize(X.bucketCount()));
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}


template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase3()
{
    // ------------------------------------------------------------------------
    // INSERT (SINGLE KEY/VALUE)
    //
    // Concerns:
    //: 1 The 'insert' method works as expected irrespective of the parameters
    //:   specified on construction of the hash map.
    //:
    //: 2 Allocations by the 'insert' method are taken from the allocator
    //:   specified on construction of the hash map.
    //
    //: 3 QoI: There is no temporary memory allocation from any allocator.
    //:
    //: 4 Any memory allocation is exception neutral.
    //:
    //: 5 Every hash map releases any allocated memory at destruction.
    //:
    //: 6 Expectations of 'insert' operations consist of:
    //:   1 The return value.
    //:   2 The creation of new element or update of existing according to
    //:     existing elements of the hash map.
    //:   3 The placement of any new element in the expected bucket.
    //:   4 Appropriate increases in size of hash map and size of bucket.
    //:   5 The lack of unintended changes to other elements in the hash map,
    //:     if any.
    //
    // Plan:
    //: 1 Each test is repeated for several hash maps, each constructed with
    //:   different, representative arguments.  In different iterations each
    //:   construction parameter is set to its minimal allowed value and to an
    //:   incremented (i.e., non-minimal) value.
    //:
    //: 2 This test follows the depth-ordered enumeration test pattern.  We
    //:   fully test each level before proceeding to the next, where each
    //:   level corresponds to the number of elements held by the hash map
    //:   before invocation of 'insert'.  Testing to level 2 (i.e., up to three
    //:   elements after 'insert' was deemed sufficient.  That allows test
    //:   cases where one or two elements may be effected while there is
    //:   an uninvolved element that is shown to be unchanged.
    //:
    //: 3 Each 'insert' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.  The 'loopCount' is
    //:   checked for multiple iterations and the hash map allocator is checked
    //:   for the allocation of memory and the return of all allocated memory.
    //:
    //: 4 Each 'insert' test call has an associated test allocator monitor that
    //:   is wired to the default allocator.  That monitor is checked for zero
    //:   allocations after the test call.
    //:
    //: 5 BOOTSTRAP: Basic accessors (see case 4) are checked for consistency
    //:   with expect results for each operation.
    //
    // Testing:
    //   bsl::size_t insert(const KEY& key, const VALUE& value);
    // ------------------------------------------------------------------------

    // This is similar to a portion ('e_INSERT_UNIQUE') of 'testCase3' in
    // StripedUnorderedImpl'.

    if (verbose) cout
                    << endl
                    << "INSERT (SINGLE KEY/VALUE)" << endl
                    << "=========================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    struct {
        int         d_line;
        bsl::size_t d_numBuckets;
        bsl::size_t d_numStripes;
    } CONFIGS[] = {
        //LINE NUM_BUCKETS  NUM_STRIPES
        //---- -----------  -----------
        { L_,            2,           1 } // minimum arguments
      , { L_,            2,           2 } // non-minimum number stripes
      , { L_,            4,           1 } // non-minimum number buckets
    };
    bsl::size_t NUM_CONFIGS = sizeof CONFIGS / sizeof *CONFIGS;

    for (bsl::size_t ti = 0; ti < NUM_CONFIGS; ++ti) {
        const int         LINE        = CONFIGS[ti].d_line;  (void)LINE;
        const bsl::size_t NUM_BUCKETS = CONFIGS[ti].d_numBuckets;
        const bsl::size_t NUM_STRIPES = CONFIGS[ti].d_numStripes;

        if (veryVeryVerbose) { P_(NUM_BUCKETS) P(NUM_STRIPES) }

        if (verbose) cout << "Depth 0" << endl;
        {
            const bsls::Types::Int64 EXPECTED_NUM_ALLOCATIONS
                   = 1 // for container node
                   + (true == bslma::UsesBslmaAllocator<KEY>  ::value ? 1 : 0)
                   + (true == bslma::UsesBslmaAllocator<VALUE>::value ? 1 : 0);

            if (veryVeryVerbose) { P(EXPECTED_NUM_ALLOCATIONS) }

            const struct {
                int         d_line;
                int         d_key;
                int         d_value;

                bsl::size_t d_expRc;
                bsl::size_t d_expSize;
                bsl::size_t d_expBucketIndex;
                bsl::size_t d_expBucketSize;
            } INSERTS0[] = {
                //LINE KEY VAL EXP_RC EXP_SIZE EXP_BIDX EXP_BSIZE
                //---- --- --- ------ -------- -------- ---------
                { L_,    0, 10,    1,        1,       0,        1 }
              , { L_,    1, 11,    1,        1,       1,        1 }
            };
            const bsl::size_t NUM_INSERTS0 = sizeof  INSERTS0
                                           / sizeof *INSERTS0;

            for (bsl::size_t tj = 0; tj < NUM_INSERTS0; ++tj) {
                const int   LINE      = INSERTS0[tj].d_line;
                const int   keyCode   = INSERTS0[tj].d_key;
                const int   valueCode = INSERTS0[tj].d_value;
                const KEY   cKEY      = TstFacility::create<KEY  >(  keyCode);
                const VALUE cVALUE    = TstFacility::create<VALUE>(valueCode);
                dam.reset(); // 'TstFacility::create' allocates from default
                             // allocator.

                (void)LINE;

                const bsl::size_t  EXP_RC     = INSERTS0[tj].d_expRc;
                const bsl::size_t  EXP_SIZE   = INSERTS0[tj].d_expSize;
                const bsl::size_t  EXP_BSIZE  = INSERTS0[tj].d_expBucketSize;

                const bsl::size_t  EXP_BIDX   = INSERTS0[tj].d_key
                                              % NUM_BUCKETS;


                if (veryVeryVerbose) {

                    P_(keyCode) P_(valueCode)
                }

                const bool *const vvvV = &veryVeryVeryVerbose;

                bslma::TestAllocator         sa("supplied", *vvvV);
                bslma::TestAllocatorMonitor  sam(&sa);
                bslma::TestAllocator         da("default" , *vvvV);
                bslma::DefaultAllocatorGuard dag(&da);

                int loopCount = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++loopCount;

                    // TEST OBJECT
                    Obj mX(NUM_BUCKETS, NUM_STRIPES, &sa);  const Obj& X = mX;
                    ASSERT(true        == X.empty());
                    ASSERT(0           == X.size());
                    ASSERT(NUM_BUCKETS == X.bucketCount());
                    ASSERT(NUM_STRIPES == X.numStripes());
                    ASSERT(&sa         == X.allocator());

                    bsls::Types::Int64          numBlocksTotalAfore
                                                         = sa.numBlocksTotal();
                    bslma::TestAllocatorMonitor dam1(&da);

                    bsl::size_t rc = mX.insert(cKEY, cVALUE); // *ACTION*
                    ASSERT(EXP_RC == rc);

                    bsls::Types::Int64 numBlocksTotalAfter
                                                         = sa.numBlocksTotal();
                    ASSERT(dam1.isTotalSame());
                    ASSERT(numBlocksTotalAfter
                         - numBlocksTotalAfore == EXPECTED_NUM_ALLOCATIONS);

                    ASSERTV(2                         // CTOR   allocations
                          + EXPECTED_NUM_ALLOCATIONS  // insert allocations
                          + 1 == loopCount);

                    const bsl::size_t numElements = X.size();
                    const bsl::size_t bucketIndex = X.bucketIndex(cKEY);
                    const bsl::size_t bucketSize  = X.bucketSize(bucketIndex);

                    if (veryVeryVerbose) {
                        P_(numElements) P_(bucketIndex) P(bucketSize)
                    }

                    ASSERTV(numElements, EXP_SIZE  == numElements);
                    ASSERTV(bucketIndex, EXP_BIDX  == bucketIndex);
                    ASSERTV(bucketSize,  EXP_BSIZE == bucketSize);

                    rc = X.getValue(&value, cKEY);
                    ASSERT(numElements == rc);
                    ASSERT(value       == cVALUE);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
                ASSERTV(sam.isTotalUp());    // Memory was allocated.
                ASSERTV(sam.isInUseSame());  // All memory recovered.
            }  // INSERTS0
        }  // Depth 0
        ASSERT(dam.isTotalSame());

        if (verbose) cout << "Depth 1" << endl;
        {
            const int KA = 0; const int VA1 = 10;  const int VA2 = 12;
            const int KB = 1; const int VB1 = 11;  const int VB2 = 13;

            const struct {
                int         d_line;
                const char  d_object;
                int         d_key;
                int         d_value;

                bsl::size_t d_expRc;
                bsl::size_t d_expSize;
            } INSERTS1[] = {
                //LINE OBJ  KEY  VAL  EXP_RC EXP_SIZE
                //---- ---  ---  ---  ------ --------
                { L_,  'A',  KA, VA1,      0,       1 }
              , { L_,  'A',  KA, VA2,      0,       1 }
              , { L_,  'B',  KB, VB1,      0,       1 }
              , { L_,  'B',  KB, VB2,      0,       1 }
            };
            const bsl::size_t NUM_INSERTS1 = sizeof  INSERTS1
                                           / sizeof *INSERTS1;

            for (bsl::size_t tj = 0; tj < NUM_INSERTS1; ++tj) {
                const int   LINE      = INSERTS1[tj].d_line;
                const char  OBJ       = INSERTS1[tj].d_object;
                const int   keyCode   = INSERTS1[tj].d_key;
                const int   valueCode = INSERTS1[tj].d_value;
                const KEY   cKEY      = TstFacility::create<KEY  >(  keyCode);
                const VALUE cVALUE    = TstFacility::create<VALUE>(valueCode);
                dam.reset(); // 'TstFacility::create' allocates from default
                             // allocator.

                const bsl::size_t  EXP_RC    = INSERTS1[tj].d_expRc;
                const bsl::size_t  EXP_SIZE  = INSERTS1[tj].d_expSize;

                const bsl::size_t  EXP_BIDX  = INSERTS1[tj].d_key
                                             % NUM_BUCKETS;
                const bsl::size_t  EXP_BSIZE = EXP_RC + 1;

                if (veryVeryVerbose) {
                    P_(keyCode) P_(valueCode)
                }

                const bool *const vvvV = &veryVeryVeryVerbose;

                bslma::TestAllocator         sa("supplied", *vvvV);
                bslma::TestAllocatorMonitor  sam(&sa);
                bslma::TestAllocator         da("da",  *vvvV);
                bslma::DefaultAllocatorGuard dag(&da);

                int loopCount = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++loopCount;

                    ///CREATE TEST OBJECT
                    ///------------------
                    // Depth 1 test objects: 'A' and 'B', each have one element
                    // that reside in different buckets.

                    Obj mA(NUM_BUCKETS, NUM_STRIPES, &sa);
                    Obj mB(NUM_BUCKETS, NUM_STRIPES, &sa);

                    const KEY   cKEY_A   = TstFacility::create<KEY  >(KA);
                    const KEY   cKEY_B   = TstFacility::create<KEY  >(KB);
                    const VALUE cVALUE1A = TstFacility::create<VALUE>(VA1);
                    const VALUE cVALUE1B = TstFacility::create<VALUE>(VB1);

                    mA.insert(cKEY_A, cVALUE1A);
                    mB.insert(cKEY_B, cVALUE1B);

                    Obj  *objPtr = OBJ == 'A' ? &mA : &mB;
                    Obj&  mX     = *objPtr;  const Obj& X = mX;

                    // INSERT TEST
                    bslma::TestAllocatorMonitor dam1(&da);

                    bsl::size_t rc = mX.insert(cKEY, cVALUE); // *ACTION*

                    // ACCESS RESULTS
                    ASSERT(EXP_RC == rc);
                    ASSERT(dam1.isTotalSame());

                    const bsl::size_t numElements = X.size();
                    const bsl::size_t bucketIndex = X.bucketIndex(cKEY);
                    const bsl::size_t bucketSize  = X.bucketSize(bucketIndex);

                    if (veryVeryVerbose) {
                        P_(numElements) P_(bucketIndex) P(bucketSize)
                    }

                    ASSERTV(numElements, EXP_SIZE  == numElements);
                    ASSERTV(bucketIndex, EXP_BIDX  == bucketIndex);
                    ASSERTV(bucketSize,  EXP_BSIZE == bucketSize);

                    VALUE value;
                    rc = X.getValue(&value, cKEY);

                    ASSERT(numElements == rc);

                    ASSERTV(LINE, value == cVALUE);

                    ASSERT(1 < loopCount);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
                ASSERTV(sam.isTotalUp());    // Memory was allocated.
                ASSERTV(sam.isInUseSame());  // All memory recovered.

            }  // INSERTS1
        }  // Depth 1
        ASSERT(dam.isTotalSame());

        if (verbose) cout << "Depth 2" << endl;
        {
            const int K1A = 0, K2A = 2, K3A = 4, V1A = 10,  V2A = 12, V3A = 14;
            const int K1B = 1, K2B = 3,          V1B = 11,  V2B = 13;
            const int nnn = -1;

            typedef struct KeyValuesArray {
                int d_key;
                int d_values[3];
            } KeyValuesArray;

            const struct {
                int  d_line;
                int  d_element1key;
                int  d_element1value;
                int  d_element2key;
                int  d_element2value;

                int  d_key;
                int  d_value;

                bsl::size_t    d_expRc;
                bsl::size_t    d_expSize;
                KeyValuesArray d_expKeyValuesArray[3];
            } INSERTS2[] = {

            ///Recurring Pattern of Inserts:
            ///-----------------------------
            //: o Insert    matches existing key  and       value.
            //: o Insert    matches existing key  but not   value.
            //: o Insert mismatches existing keys but same  bucket
            //: o Insert mismatches existing keys and other bucket.

            //LINE E1         E2       KEY  VAL RC SZ "Sorted" Results
            //---- --------  --------  ---  --- -- -- ----------------
            // Initially, two identical elements in same bucket.
            { L_,  K1A, V1A, K1A, V1A, K1A, V1A, 0, 2,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1A, V2A, 0, 2,
                                                 {
                                                     { K1A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }

                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K2A, V2A, 1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1B, V1B, 1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            // Initially two different elements in same bucket.
            { L_,  K1A, V1A, K2A, V2A, K1A, V1A, 0, 2,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1A, V2A, 0, 2,
                                                 {
                                                     { K1A, { V2A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K3A, V3A, 1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K3A, { V3A, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1B, V1B, 1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            // Initially two different elements in different buckets.
            { L_,  K1A, V1A, K1B, V1B, K1A, V1A, 0, 2,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K1A, V2A, 0, 3,
                                                 {
                                                     { K1A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2A, V2A, 1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2B, V2B, 1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { K2B, { V2B, nnn, nnn } }
                                                 }},
            };

            const bsl::size_t NUM_INSERTS2 = sizeof  INSERTS2
                                           / sizeof *INSERTS2;

            for (bsl::size_t tj = 0; tj < NUM_INSERTS2; ++ tj) {
                const int         LINE        = INSERTS2[tj].d_line;
                const int         E1KEY       = INSERTS2[tj].d_element1key;
                const int         E1VALUE     = INSERTS2[tj].d_element1value;
                const int         E2KEY       = INSERTS2[tj].d_element2key;
                const int         E2VALUE     = INSERTS2[tj].d_element2value;
                const int         IKEY        = INSERTS2[tj].d_key;
                const int         IVALUE      = INSERTS2[tj].d_value;

                if (veryVerbose) {
                    T_ P(LINE)
                    T_ P_(E1KEY) P(E1VALUE)
                    T_ P_(E2KEY) P(E2VALUE)
                    T_ P_(IKEY)  P_(IVALUE)
                }

                const bsl::size_t EXP_RC       = INSERTS2[tj].d_expRc;
                const bsl::size_t EXP_SIZE     = INSERTS2[tj].d_expSize;

                if (veryVeryVerbose) {
                    T_ P_(EXP_RC) P(EXP_SIZE)
                }

                const KeyValuesArray * const
                                  EXP_ELEMENTS = INSERTS2[tj].
                                                           d_expKeyValuesArray;

                const bool *const vvvV = &veryVeryVeryVerbose;

                bslma::TestAllocator         sa("supplied", *vvvV);
                bslma::TestAllocatorMonitor  sam(&sa);
                bslma::TestAllocator         da("da",  *vvvV);
                bslma::DefaultAllocatorGuard dag(&da);

                int loopCount = 0;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++loopCount;

                    // Create Test Object
                    Obj mX(NUM_BUCKETS, NUM_STRIPES, &sa); const Obj& X = mX;

                    const KEY   E1_KEY   = TstFacility::create<KEY  >(E1KEY);
                    const KEY   E2_KEY   = TstFacility::create<KEY  >(E2KEY);
                    const VALUE E1_VALUE = TstFacility::create<VALUE>(E1VALUE);
                    const VALUE E2_VALUE = TstFacility::create<VALUE>(E2VALUE);

                    // Create Arguments for Insertion Test
                    const KEY   I_KEY    = TstFacility::create<KEY  >(IKEY);
                    const VALUE I_VALUE  = TstFacility::create<VALUE>(IVALUE);
                    dam.reset(); // 'TstFacility::create' allocates from
                                 // default allocator.

                    mX.insert(E1_KEY, E1_VALUE);
                    mX.insert(E2_KEY, E2_VALUE);

                    bsl::size_t expSize = E1_KEY == E2_KEY ? 1 : 2;
                    ASSERT(expSize == X.size());

                    // Test 'insert'
                    bslma::TestAllocatorMonitor dam1(&da);

                    bsl::size_t rc = mX.insert(I_KEY, I_VALUE); // <==

                    // Assess Results
                    ASSERTV(EXP_RC, rc, EXP_RC == rc);
                    ASSERT(dam1.isTotalSame());

                    for (bsl::size_t i = 0; i < 3; ++i) {
                        const KeyValuesArray *keyValuesArray = EXP_ELEMENTS;

                        int        key    = keyValuesArray->d_key;
                        const int *values = keyValuesArray->d_values;
                        if (nnn == key) {
                            break;
                        }
                        bsl::vector<int>        exp_values(&scratch);
                        const bsl::vector<int>& EXP_VALUES = exp_values;

                        for (bsl::size_t j = 0; j < 3; ++j) {
                            int value = values[j];
                            if (nnn == value) {
                                break;
                            }
                            exp_values.push_back(value);

                            if (veryVeryVerbose) {
                                cout << "Expected: " << value << " ";
                            }
                        }
                        if (veryVeryVerbose)  cout << endl;

                        const KEY cKEY = TstFacility::create<KEY>(key);
                        VALUE     foundValue = VALUE();
                        dam.reset(); // 'TstFacility::create' allocates from
                                     // default allocator.

                        bsl::size_t rc = X.getValue(&foundValue, cKEY);
                        ASSERT(rc == 1);

                        // Convert returned value to 'int' representation.

                        int foundValueAsInt =
                                        TstFacility::getIdentifier(foundValue);

                        // Compare found value to expected value.

                        ASSERTV(EXP_VALUES[0] == foundValueAsInt);
                    }

                    ASSERT(1 < loopCount);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

                ASSERTV(sam.isTotalUp());    // Memory was allocated.
                ASSERTV(sam.isInUseSame());  // All memory recovered.

            }  // INSERTS2
        }  // Depth 2
        ASSERT(dam.isTotalSame());

    }  // CONFIGS

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}


template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase2()
{
    // ------------------------------------------------------------------------
    // CONSTRUCTOR AND DESTRUCTOR
    //
    // Concerns:
    //: 1 The constructed container has the expected attributes.
    //:
    //:   o Allocator:
    //:     If an allocator is not specified (i.e., 'basicAllocator' set to 0
    //:     or not supplied) the currently installed default allocator is used.
    //:
    //:   o Number of buckets and stripes:
    //:     o Expected default values assigned when the arguments are
    //:       unspecified.
    //:     o Arguments that are not powers of two are rounded up.
    //:     o Arguments that are too small are rounded up.
    //:
    //: 2 A newly constructed hash map is empty.
    //:
    //: 3 QoI: There is no temporary memory allocation from any allocator.
    //:
    //: 4 Any memory allocation is exception neutral.
    //:
    //: 5 Every hash map releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 The "footprint" idiom is used to create container objects using a
    //:   variety of different "configurations" of arguments: arguments
    //:   specified and arguments allowed to default; arguments that are valid
    //:   and arguments that must be adjusted (internally) before use.
    //:
    //: 2 Several accessors (as yet untested) are used.  This usage is
    //:   considered "BOOTSTRAP" tests of those methods.
    //:
    //: 3 Exception tests follow the conventional BDE idiom using the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //:
    // Testing:
    //   StripedUnorderedMap(numInitialBuckets, numStripes, *basicAllocator);
    //   ~StripedUnorderedMap();
    // ------------------------------------------------------------------------

    // This is identical to 'testCase2' in 'StripedUnorderedImpl'.

    if (verbose) cout << endl
                      << "CONSTRUCTOR AND DESTRUCTOR" << endl
                      << "==========================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    if (veryVerbose) cout << endl << "Configuration Tests" << endl;
    {
        for (char cfg = 'a'; cfg <= 'g'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            if (veryVerbose) {
                P(CONFIG);
            }

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            bsl::size_t exp_num_buckets;
            bsl::size_t exp_num_stripes;

            const bsl::size_t NON_DEFAULT_NUM_BUCKETS = 256;  // 2^n
            const bsl::size_t NON_DEFAULT_NUM_STRIPES =  16;  // 2^n

            BSLMF_ASSERT(Obj::k_DEFAULT_NUM_BUCKETS
                         != NON_DEFAULT_NUM_BUCKETS);

            BSLMF_ASSERT(Obj::k_DEFAULT_NUM_STRIPES
                         != NON_DEFAULT_NUM_STRIPES);

            switch (CONFIG) {
              case 'a': {
                if (veryVeryVerbose) {
                    cout << "No allocator specified" << endl;
                }

                objPtr = new (fa) Obj(NON_DEFAULT_NUM_BUCKETS,
                                      NON_DEFAULT_NUM_STRIPES);
                objAllocatorPtr = &da;
                exp_num_buckets = NON_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = NON_DEFAULT_NUM_STRIPES;
              } break;

              case 'b': {
                if (veryVeryVerbose) {
                    cout << "Default allocator explicitly (0) specified"
                         << endl;
                }

                objPtr = new (fa) Obj(NON_DEFAULT_NUM_BUCKETS,
                                      NON_DEFAULT_NUM_STRIPES,
                                      0);
                objAllocatorPtr = &da;
                exp_num_buckets = NON_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = NON_DEFAULT_NUM_STRIPES;
              } break;

              case 'c': {
                if (veryVeryVerbose) {
                    cout << "Supplied allocator specified (&sa)" << endl;
                }

                objPtr = new (fa) Obj(NON_DEFAULT_NUM_BUCKETS,
                                      NON_DEFAULT_NUM_STRIPES,
                                      &sa);
                objAllocatorPtr = &sa;
                exp_num_buckets = NON_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = NON_DEFAULT_NUM_STRIPES;
              } break;

              case 'd': {
                if (veryVeryVerbose) {
                    cout << "Number buckets and stripes not 2^n" << endl;
                }

                objPtr = new (fa) Obj(NON_DEFAULT_NUM_BUCKETS - 1,  // not 2^n
                                      NON_DEFAULT_NUM_STRIPES - 1,  // not 2^n
                                      &sa);
                objAllocatorPtr = &sa;
                exp_num_buckets = NON_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = NON_DEFAULT_NUM_STRIPES;
              } break;

              case 'e': {
                if (veryVeryVerbose) {
                    cout << "Check default number of stripes." << endl;
                }

                objPtr = new (fa) Obj(NON_DEFAULT_NUM_BUCKETS);
                objAllocatorPtr = &da;
                exp_num_buckets =    NON_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = Obj::k_DEFAULT_NUM_STRIPES;
              } break;

              case 'f': {
                if (veryVeryVerbose) {
                    cout << "Default number of buckets (and stripes)." << endl;
                }

                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
                exp_num_buckets = Obj::k_DEFAULT_NUM_BUCKETS;
                exp_num_stripes = Obj::k_DEFAULT_NUM_STRIPES;
              } break;

              case 'g': {
                if (veryVeryVerbose) {
                    cout << "Lower limit: number of buckets and stripes"
                         << endl;
                }

                objPtr = new (fa) Obj(1, // number of buckets (initially)
                                      0, // number of stripes
                                      &sa);
                objAllocatorPtr = &sa;
                exp_num_buckets = 2;
                exp_num_stripes = 1;
              } break;

              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = ('c' != CONFIG &&
                                         'd' != CONFIG &&
                                         'g' != CONFIG)
                                      ? sa
                                      : da;
            if (veryVeryVerbose) {
                cout << " oa" << ": "
                     << ( &oa == &sa ? "supplied" : "default") << endl;
                cout << "noa" << ": "
                     << (&noa == &sa ? "supplied" : "default") << endl;
            }

            const bsl::size_t EXP_NUM_BUCKETS = exp_num_buckets;
            const bsl::size_t EXP_NUM_STRIPES = exp_num_stripes;

            // Verify allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 2 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, &oa  == X.allocator());
            ASSERTV(CONFIG, 0    == X.size());
            ASSERTV(CONFIG, true == X.empty());

            ASSERTV(CONFIG, EXP_NUM_BUCKETS == X.bucketCount());
            ASSERTV(CONFIG, EXP_NUM_STRIPES == X.numStripes());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }

    if (veryVerbose) cout << endl << "Exception Test" << endl;
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        const bsl::size_t ARBITRARY_NUM_BUCKETS = 512;
        const bsl::size_t ARBITRARY_NUM_STRIPES =  64;

        int loopCount = 0;

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
            ++loopCount;
            if (veryVeryVerbose) {
                P(loopCount);
            }

            Obj mX(ARBITRARY_NUM_BUCKETS, ARBITRARY_NUM_STRIPES, &sa);

            ASSERTV(sa.numBlocksTotal(), 2 == sa.numBlocksInUse());

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(loopCount,           3 == loopCount);
        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(sa.numBlocksInUse(), 3 == sa.numBlocksTotal()); // 0 + 1 + 2
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}


template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive testing
    //:   in subsequent test cases.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "BREATHING TEST" << endl
                      << "==============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    {
        Obj mX(1024, 16);  const Obj& X = mX;

        mX.insert(TstFacility::create<KEY>(1),
                  TstFacility::create<VALUE>(11));
        mX.insert(TstFacility::create<KEY>(2),
                  TstFacility::create<VALUE>(22));
        mX.insert(TstFacility::create<KEY>(3),
                  TstFacility::create<VALUE>(33));
        mX.insert(TstFacility::create<KEY>(4),
                  TstFacility::create<VALUE>(44));
        mX.insert(TstFacility::create<KEY>(5),
                  TstFacility::create<VALUE>(55));

        ASSERTV(X.size(), X.size() == 5);

        {
            VALUE       value;
            bsl::size_t rc = mX.getValue(&value, TstFacility::create<KEY>(1));
            ASSERTV(rc, 1 == rc);
            ASSERTV(value, 11 == TstFacility::getIdentifier(value));
        };

        {
            VALUE       value;
            bsl::size_t rc = mX.getValue(&value, TstFacility::create<KEY>(3));
            ASSERTV(rc, 1 == rc);
            ASSERTV(value, 33 == TstFacility::getIdentifier(value));
        };
    }


    const TestValues VALUES;  // contains 52 distinct increasing values

    {
        Obj        mX(1024, 16);
        const Obj& X = mX;

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);
        mX.insert(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insert(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 6);
    }

    {
        Obj        mX(1024, 16);
        const Obj& X = mX;

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);

        {
            VALUE       value;
            bsl::size_t rc = mX.getValue(&value, VALUES[1].first);
            ASSERTV(rc, 1 == rc);
            ASSERTV(value, areEqual(VALUES[1].second, value));
        };

        mX.insert(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insert(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 6);
    }

}


}  // close unnamed namespace

bsl::vector<int> stringSplit(bsl::string csString)
    // Return a vector of the integer split from the specified 'csString'
    //comma separated string of integers.  Assume only digits and ','.
{
    bsl::vector<int> ret;
    bsl::stringstream ss(csString);
    for (int i; ss >> i;) {
        ret.push_back(i);
        if (ss.peek() == ',') {
            ss.ignore();
        }
    }
    return ret;
}

// ============================================================================
//                                PERFORMANCE TEST
// ----------------------------------------------------------------------------

namespace hPerf {
template <class KEY, class VAL>
class HBenchmark {
    // This class provides the performance test functions for the various
    // performance tests on 'bdlcc::StripedUnorderedMap'.

  public:
    typedef bdlcc::StripedUnorderedMap<KEY, VAL> MapType;

  private:
    // DATA
    int               d_numStripes;  // # of stripes in input.  0 - use the
                                     // global unordered map
    int               d_numBuckets;  // # of buckets
    int               d_maxSize;     // Maximal number of elements in the map
    bool              d_enblRehash;  // Enable / disable rehash

    MapType          *d_map_p;       // bdlcc::StripedUnorderedMultiMap

    int               d_curValue;    // Internal counter for the pushed value
    int               d_countErr;    // Internal counter for the number of
                                     // tryPopFront errors
    bslma::Allocator *d_allocator_p; // memory allocator

    // PRIVATE ACCESSORS
    KEY makeKey(int key) const
        // Return a KEY type from the specified 'key'.
    {
        return makeKey(key, bsl::is_same<bsl::string, KEY>());
    }

    KEY makeKey(int key, bsl::true_type) const
        // Return a string built from the specified 'key'.
    {
        return bsl::to_string(key);
    }

    KEY makeKey(int key, bsl::false_type) const
        // Return a KEY type from the specified 'key', cast from 'key'.
    {
        return static_cast<KEY>(key);
    }

    VAL makeValue(int key) const
        // Return a VAL type from the specified 'key'.
    {
        return makeValue(key, bsl::is_same<bsl::string, VAL>());
    }

    VAL makeValue(int key, bsl::true_type) const
        // Return a string built from the specified 'key'.
    {
        return bsl::to_string(key);
    }

    VAL makeValue(int key, bsl::false_type) const
        // Return a VAL type from the specified 'key', cast from 'key'.
    {
        return static_cast<VAL>(key);
    }

    // NOT IMPLEMENTED
    HBenchmark(const HBenchmark&);
    HBenchmark& operator=(const HBenchmark&);

  public:
    HBenchmark(int               numStripes,
               int               numBuckets,
               int               maxSize,
               bool              enblRehash,
               bslma::Allocator *basicAllocator = 0);
        // Create a 'HashPerformance' object with the specified 'numStripes',
        // 'numBuckets', 'maxSize', and 'enblRehash'  Optionally specify
        // 'basicAllocator'.

    // MANIPULATORS
    void initializeSample(bool);
        // Initialize 'HashPerformance' object before a sample for tests
        // requiring initial data.

    void initializeSampleNoInsert(bool);
        // Initialize 'HashPerformance' object before a sample for tests not
        // requiring initial data.

    void cleanupSample(bool);
        // Run after a sample.

    // ACCESSORS
    int countErr() const;
        // Return the error count accumulated through the run.

    // TEST FUNCTIONS
    void insert(int);
        // Insert a single element into the hash map. Test type 1

    void findExist(int);
        // Find a single element that exists in the hash map. Test type 2

    void findMiss(int);
        // Find a single element that is missing in the hash map. Test type 3

    void erase(int);
        // Erase a single element from the hash map. Test type 4

};  // END class HBenchmark

// CREATORS
template <class KEY, class VAL>
HBenchmark<KEY, VAL>::HBenchmark(int               numStripes,
                                 int               numBuckets,
                                 int               maxSize,
                                 bool              enblRehash,
                                 bslma::Allocator *basicAllocator)
: d_numStripes(numStripes)
, d_numBuckets(numBuckets)
, d_maxSize(maxSize)
, d_enblRehash(enblRehash)
, d_map_p(0)
, d_curValue(0)
, d_countErr(0)
, d_allocator_p(basicAllocator)
{
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::initializeSample(bool)
{
    initializeSampleNoInsert(false); // Parameter is ignored
    // For test types 2, 3, 4 - do the inserts
    for (int j = 0; j < d_maxSize; ++j) {
        int key   = d_curValue++;
        KEY ky    = makeKey(key);
        VAL value = makeValue(key);
        d_map_p->insert(ky, value);
    }
    d_curValue = 0;
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::initializeSampleNoInsert(bool)
{
    // For test types 0, 1
    d_map_p = new (*d_allocator_p) MapType(
                                        static_cast<bsl::size_t>(d_numBuckets),
                                        static_cast<bsl::size_t>(d_numStripes),
                                        d_allocator_p);
    d_curValue = 0;
    d_countErr = 0;
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::cleanupSample(bool)
{
    d_allocator_p->deleteObject(d_map_p);
}

// ACCESSORS
template <class KEY, class VAL>
inline
int HBenchmark<KEY, VAL>::countErr() const
{
    return d_countErr;
}

// TEST FUNCTIONS
template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::insert(int)
{
    // insert a single element into the hash map. Test type 1
    int key   = d_curValue++;
    KEY ky    = makeKey(key);
    VAL value = makeValue(key);
    d_map_p->setValue(ky, value);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxSize ==
                                          static_cast<int>(d_map_p->size()))) {
        d_map_p->clear();
        d_curValue = 0;
    }
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::findExist(int)
{
    // find a single element that exists in the hash map. Test type 2
    int         key = d_curValue++;
    KEY         ky  = makeKey(key);
    VAL         value;
    bsl::size_t num = d_map_p->getValue(&value, ky);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == num)) ++d_countErr;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_curValue >= d_maxSize))
        d_curValue = 0;
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::findMiss(int)
{
    // find a single element that is missing in the hash map. Test type 3
    int key = d_curValue++;
    key += static_cast<int>(d_numBuckets);
    KEY         ky  = makeKey(key);
    VAL         value;
    bsl::size_t num = d_map_p->getValue(&value, ky);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != num)) ++d_countErr;
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_curValue >= d_maxSize))
        d_curValue = 0;
}

template <class KEY, class VAL>
void HBenchmark<KEY, VAL>::erase(int)
{
    // erase a single element from the hash map. Test type 4
    int key = d_curValue++;
    KEY ky  = makeKey(key);
    d_map_p->erase(ky);
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_curValue >= d_maxSize))
        d_curValue = 0;
}

}  // close namespace hPerf

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5 && test > 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    defaultAllocator.setVerbose(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocatorMonitor gam(&globalAllocator);

    // BDE_VERIFY pragma: -TP17 These are defined in the various test functions
    switch (test) { case 0:
      case 19: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage examples from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        usage::example1();
        usage::example2();
        usage::example3();

      } break;
      // BDE_VERIFY pragma: -TP05 Defined in the various test functions
      case 18: {
        threaded::threadedTest1();
      } break;
      case 17: {
        RUN_EACH_TYPE(TestDriver, testCase17, bsltf::MovableAllocTestType);
      } break;
      case 16: {
        RUN_EACH_TYPE(TestDriver, testCase16, bsltf::MovableAllocTestType);
      } break;
      case 15: {
        RUN_EACH_TYPE(TestDriver, testCase15, TEST_TYPES_REGULAR);
      } break;
      case 14: {
        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES_REGULAR);
      } break;
      case 13: {
        RUN_EACH_TYPE(TestDriver, testCase13, TEST_TYPES_REGULAR);
      } break;
      case 12: {
        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES_REGULAR);
      } break;
      case 11: {
        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR);
      } break;
      case 10: {
        RUN_EACH_TYPE(TestDriver, testCase10, TEST_TYPES_REGULAR);
      } break;
      case 9: {
        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES_REGULAR);
      } break;
      case 8: {
        RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES_REGULAR);
      } break;
      case 7: {
        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES_REGULAR);
      } break;
      case 6: {
        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR);
      } break;
      case 5: {
        RUN_EACH_TYPE(TestDriver, testCase5, TEST_TYPES_REGULAR);
      } break;
      case 4: {
        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES_REGULAR);
      } break;
      case 3: {
        RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES_REGULAR);
      } break;
      case 2: {
        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR);
      } break;
      case 1: {
        RUN_EACH_TYPE(TestDriver, testCase1, TEST_TYPES_REGULAR);
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST INT->STRING
        //   Tests performance of running insert, getValue and erase against
        //   the hash map, which is int to string.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: test type (defaults to 2):
        //       1-insert up to given size, then setValue
        //       2-find (element exists)
        //       3-find (element is missing)
        //       4-erase
        //       5-insert+find
        //   3rd parameter: number of threads in first group (defaults to 2).
        //   4th parameter: number of threads in second group (defaults to 0;
        //       used for for test type 5).
        //   5th parameter: number of stripes (defaults to 4).
        //   6th parameter: number of buckets (defaults to 16)
        //   7th parameter: work load for for threads (defaults to 0 - no load)
        //   8th parameter: percentage until which we fill the buckets
        //       (defaults to 70%)
        //   9th parameter: 1-enable rehash; 0-disable rehash
        //  10th parameter: number of milliseconds each sample runs (defaults
        //       to 2000)
        //  11th parameter: number of samples to run (defaults to 10)
        //
        // Note that parameters 2,3,4,5,6,7,8 can be provided as a comma
        // separated list of values.  Also note that test type 5 will produce
        // two sets of percentiles.
        //
        // Concerns:
        //: 1 Calculates throughput percentiles (0%-min, 25%, 50%-median, 75%,
        //    and 100%-max) for the thread groups based on the given
        //    parameters.  Note that the various test types cover all the basic
        //    operations, and required combinations).
        //
        // Plan:
        //: 1 Create a HashPerformance object, and use initRead to pre-load
        //:   it.  High mark is set above eviction.  Then run testReadWrite.
        //:   Note that testReadWrite run inserts for the writer threads and
        //:   tryGetValue for the reader threads.  Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   PERFORMANCE TEST INT->STRING
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST INT->STRING" << endl
                 << "============================" << endl;

        bslma::NewDeleteAllocator nalloc;

        if (argc < 3) {
            bsl::cout
        << "Usage: bdlcc_stripedunorderedmultimap.t -1 <testType> "
        << "[numThread1] [numThread2] [numStripe] [numBucket] [load] "
        << "[percentage] [enableRehash] [numMillis] [numSamples]\n"
        << "\n"
        << "All parameters except the first (testType) are optional.\n"
        << "testType, numThread1, numThread2, numStripe, numBucket, load, "
        << "percentage can be provided as comma separated list of "
        << "integers with no spaces.\n"
        << "\n"
        << "Test types: 1-insert, 2-find, 3-not find, 4-erase, 5-insert+find\n"
        << "numThread1: number of threads in first group. Default 2\n"
        << "numThread2: number of threads in second group. Default 0\n"
        << "numStripe: Default  4\n"
        << "numBucket: Default 16\n"
        << "load: from bslmt_throughputbenchmark. Default 0\n"
        << "percentage: fill level of container. Default 70%\n"
        << "enableRehash: if percentage > 100%, enable rehash. Default 1\n"
        << "numMillis: number of milliseconds in a sample. Default 2000\n"
        << "numSamples: Default 10\n";
            return -1;                                                // RETURN
        }
        // Test types: 1-insert, 2-find, 3-not find, 4-erase, 5-insert+find
        // Note that numThread, numWThread, testType, numStripe, numBucket,
        // load, and percentage accept comma separated values.
        bsl::string testType   = argv[2];
        bsl::string numThread1 = argc >  3 ? argv[3] :  "2";
        bsl::string numThread2 = argc >  4 ? argv[4] :  "0";
        bsl::string numStripe  = argc >  5 ? argv[5] :  "4";
        bsl::string numBucket  = argc >  6 ? argv[6] : "16";
        bsl::string load       = argc >  7 ? argv[7] :  "0";
        bsl::string percentage = argc >  8 ? argv[8] : "70";
        // 1-enable rehash, 0-disable rehash
        int         enblRehash = argc >  9 ? atoi(argv[ 9]) :    1;
        int         numMillis  = argc > 10 ? atoi(argv[10]) : 2000;
        int         numSamples = argc > 11 ? atoi(argv[11]) :   10;

        vector<int> numThreads1 = stringSplit(numThread1);
        vector<int> numThreads2 = stringSplit(numThread2);
        vector<int> testTypes   = stringSplit(testType);
        vector<int> numStripes  = stringSplit(numStripe);
        vector<int> numBuckets  = stringSplit(numBucket);
        vector<int> loads       = stringSplit(load);
        vector<int> percentages = stringSplit(percentage);

        bsl::cout << "Test,NT1,NT2,NS,NB,Load,Pct,ER,0%,25%,50%,75%,100%,"
                     "ErrCount,0%,25%,50%,75%,100%\n";
            // Loops on the various ranges, but to avoid 7-level indentation,
            // collapse it to one complicated loop.
        int numThread1Idx = 0;
        int numThread2Idx = 0;
        int testTypeIdx   = 0;
        int numStripeIdx  = 0;
        int numBucketIdx  = 0;
        int loadIdx       = 0;
        int pctIdx        = 0;
        for(;;) {
            // Set the looping values
            int numThread1 = numThreads1[numThread1Idx];
            int numThread2 = numThreads2[numThread2Idx];
            int testType   = testTypes[testTypeIdx];
            int numStripe  = numStripes[numStripeIdx];
            int numBucket  = numBuckets[numBucketIdx];
            int load       = loads[loadIdx];
            int percentage = percentages[pctIdx];

            int maxSize    = (percentage * numBucket) / 100;

            // Create the performance, benchmark and result objects
            typedef hPerf::HBenchmark<int,bsl::string> Bench;
            Bench hb(numStripe, numBucket, maxSize, enblRehash, &nalloc);

            bslmt::ThroughputBenchmark       tb(&nalloc);
            bslmt::ThroughputBenchmarkResult res(&nalloc);

            bsl::function<void(int)>  runFunc1;
            bsl::function<void(bool)> initFunc = (testType % 4) == 1 ?
                    bdlf::BindUtil::bind(&Bench::initializeSampleNoInsert,
                                         &hb,
                                         bdlf::PlaceHolders::_1) :
                               bdlf::BindUtil::bind(&Bench::initializeSample,
                                                    &hb,
                                                    bdlf::PlaceHolders::_1);
            bsl::function<void(bool)> cleanFunc = bdlf::BindUtil::bind(
                                                       &Bench::cleanupSample,
                                                       &hb,
                                                       bdlf::PlaceHolders::_1);

            switch (testType) {
              case 1: { // insert
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::insert, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 2: { // find exist
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::findExist, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 3: { // find miss
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::findMiss, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 4: { // erase
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::erase, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 5: { // insert + read
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::insert, &hb, bdlf::PlaceHolders::_1);
              } break;
            } // END switch
            int tGId1 = tb.addThreadGroup(runFunc1, numThread1, load);

            int tGId2 = -1;
            if (testType == 5) {
                bsl::function<void(int)> runFunc2 = bdlf::BindUtil::bind(
                               &Bench::findExist, &hb, bdlf::PlaceHolders::_1);

                tGId2 = tb.addThreadGroup(runFunc2, numThread2, load);
            }
            tb.execute(&res,
                       numMillis,
                       numSamples,
                       initFunc,
                       bslmt::ThroughputBenchmark::ShutdownSampleFunction(),
                       cleanFunc);
            
            int countErr = hb.countErr();

            // Calculate percentiles.
            vector<double> percentiles(5);
            res.getPercentiles(&percentiles, tGId1);
            bsl::cout << bsl::fixed << bsl::setprecision(0) << testType << ","
                      << numThread1 << "," << numThread2 << ","
                      << numStripe << "," << numBucket << "," << load << ","
                      << percentage << "," << enblRehash << ","
                      << percentiles[0] << ","
                      << percentiles[1] << ","
                      << percentiles[2] << ","
                      << percentiles[3] << ","
                      << percentiles[4] << ","
                      << countErr;
            if (tGId2 >= 0) {
                res.getPercentiles(&percentiles, tGId2);
                bsl::cout << bsl::fixed << bsl::setprecision(0) << ","
                          << percentiles[0] << ","
                          << percentiles[1] << ","
                          << percentiles[2] << ","
                          << percentiles[3] << ","
                          << percentiles[4];
            }
            bsl::cout << "\n";

            // Update to the next index, and exit loop if end
            ++pctIdx;
            if (pctIdx < static_cast<int>(percentages.size())) continue;
            pctIdx = 0;
            ++loadIdx;
            if (loadIdx < static_cast<int>(loads.size())) continue;
            loadIdx = 0;
            ++numBucketIdx;
            if (numBucketIdx < static_cast<int>(numBuckets.size())) continue;
            numBucketIdx = 0;
            ++numStripeIdx;
            if (numStripeIdx < static_cast<int>(numStripes.size())) continue;
            numStripeIdx = 0;
            ++testTypeIdx;
            if (testTypeIdx < static_cast<int>(testTypes.size())) continue;
            testTypeIdx = 0;
            ++numThread2Idx;
            if (numThread2Idx < static_cast<int>(numThreads2.size())) continue;
            numThread2Idx = 0;
            ++numThread1Idx;
            if (numThread1Idx >= static_cast<int>(numThreads1.size())) break;
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST STRING->INT64
        //   Tests performance of running insert, getValue and erase against
        //   the hash map, which is string to int64.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: test type (defaults to 2):
        //       1-insert up to given size, then setValue
        //       2-find (element exists)
        //       3-find (element is missing)
        //       4-erase
        //       5-insert+find
        //   3rd parameter: number of threads in first group (defaults to 2).
        //   4th parameter: number of threads in second group (defaults to 0;
        //       used for for test type 5).
        //   5th parameter: number of stripes (defaults to 4).
        //   6th parameter: number of buckets (defaults to 16)
        //   7th parameter: work load for for threads (defaults to 0 - no load)
        //   8th parameter: percentage until which we fill the buckets
        //       (defaults to 70%)
        //   9th parameter: 1-enable rehash; 0-disable rehash
        //  10th parameter: number of milliseconds each sample runs (defaults
        //       to 2000)
        //  11th parameter: number of samples to run (defaults to 10)
        //
        // Note that parameters 2,3,4,5,6,7,8 can be provided as a comma
        // separated list of values.  Also note that test type 5 will produce
        // two sets of percentiles.
        //
        // Concerns:
        //: 1 Calculates throughput percentiles (0%-min, 25%, 50%-median, 75%,
        //    and 100%-max) for the thread groups based on the given
        //    parameters.  Note that the various test types cover all the basic
        //    operations, and required combinations).
        //
        // Plan:
        //: 1 Create a HashPerformance object, and use initRead to pre-load
        //:   it.  High mark is set above eviction.  Then run testReadWrite.
        //:   Note that testReadWrite run inserts for the writer threads and
        //:   tryGetValue for the reader threads.  Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   PERFORMANCE TEST STRING->INT64
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "PERFORMANCE TEST STRING->INT64" << endl
                 << "==============================" << endl;

        bslma::NewDeleteAllocator nalloc;
        typedef bsls::Types::Int64 Int64;

        if (argc < 3) {
            bsl::cout
        << "Usage: bdlcc_stripedunorderedmultimap.t -2 <testType> "
        << "[numThread1] [numThread2] [numStripe] [numBucket] [load] "
        << "[percentage] [enableRehash] [numMillis] [numSamples]\n"
        << "\n"
        << "All parameters except the first (testType) are optional.\n"
        << "testType, numThread1, numThread2, numStripe, numBucket, load, "
        << "percentage can be provided as comma separated list of "
        << "integers with no spaces.\n"
        << "\n"
        << "Test types: 1-insert, 2-find, 3-not find, 4-erase, 5-insert+find\n"
        << "numThread1: number of threads in first group. Default 2\n"
        << "numThread2: number of threads in second group. Default 0\n"
        << "numStripe: Default  4\n"
        << "numBucket: Default 16\n"
        << "load: from bslmt_throughputbenchmark. Default 0\n"
        << "percentage: fill level of container. Default 70%\n"
        << "enableRehash: if percentage > 100%, enable rehash. Default 1\n"
        << "numMillis: number of milliseconds in a sample. Default 2000\n"
        << "numSamples: Default 10\n";
            return -1;                                                // RETURN
        }
        // Test types: 1-insert, 2-find, 3-not find, 4-erase, 5-insert+find
        // Note that numThread, numWThread, testType, numStripe, numBucket,
        // load, and percentage accept comma separated values.
        bsl::string testType   = argv[2];
        bsl::string numThread1 = argc >  3 ? argv[3] :  "2";
        bsl::string numThread2 = argc >  4 ? argv[4] :  "0";
        bsl::string numStripe  = argc >  5 ? argv[5] :  "4";
        bsl::string numBucket  = argc >  6 ? argv[6] : "16";
        bsl::string load       = argc >  7 ? argv[7] :  "0";
        bsl::string percentage = argc >  8 ? argv[8] : "70";
        // 1-enable rehash, 0-disable rehash
        int         enblRehash = argc >  9 ? atoi(argv[ 9]) :    1;
        int         numMillis  = argc > 10 ? atoi(argv[10]) : 2000;
        int         numSamples = argc > 11 ? atoi(argv[11]) :   10;

        vector<int> numThreads1 = stringSplit(numThread1);
        vector<int> numThreads2 = stringSplit(numThread2);
        vector<int> testTypes   = stringSplit(testType);
        vector<int> numStripes  = stringSplit(numStripe);
        vector<int> numBuckets  = stringSplit(numBucket);
        vector<int> loads       = stringSplit(load);
        vector<int> percentages = stringSplit(percentage);

        bsl::cout << "Test,NT1,NT2,NS,NB,Load,Pct,ER,0%,25%,50%,75%,100%,"
                     "ErrCount,0%,25%,50%,75%,100%\n";
            // Loops on the various ranges, but to avoid 7-level indentation,
            // collapse it to one complicated loop.
        int numThread1Idx = 0;
        int numThread2Idx = 0;
        int testTypeIdx   = 0;
        int numStripeIdx  = 0;
        int numBucketIdx  = 0;
        int loadIdx       = 0;
        int pctIdx        = 0;
        for(;;) {
            // Set the looping values
            int numThread1 = numThreads1[numThread1Idx];
            int numThread2 = numThreads2[numThread2Idx];
            int testType   = testTypes[testTypeIdx];
            int numStripe  = numStripes[numStripeIdx];
            int numBucket  = numBuckets[numBucketIdx];
            int load       = loads[loadIdx];
            int percentage = percentages[pctIdx];

            int maxSize    = (percentage * numBucket) / 100;

            // Create the performance, benchmark and result objects
            typedef hPerf::HBenchmark<bsl::string,Int64> Bench;
            Bench hb(numStripe, numBucket, maxSize, enblRehash, &nalloc);

            bslmt::ThroughputBenchmark       tb(&nalloc);
            bslmt::ThroughputBenchmarkResult res(&nalloc);

            bsl::function<void(int)>  runFunc1;
            bsl::function<void(bool)> initFunc = (testType % 4) == 1 ?
                    bdlf::BindUtil::bind(&Bench::initializeSampleNoInsert,
                                         &hb,
                                         bdlf::PlaceHolders::_1) :
                               bdlf::BindUtil::bind(&Bench::initializeSample,
                                                    &hb,
                                                    bdlf::PlaceHolders::_1);
            bsl::function<void(bool)> cleanFunc = bdlf::BindUtil::bind(
                                &Bench::cleanupSample,
                                &hb,
                                bdlf::PlaceHolders::_1);

            switch (testType) {
              case 1: { // insert
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::insert, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 2: { // find exist
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::findExist, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 3: { // find miss
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::findMiss, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 4: { // erase
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::erase, &hb, bdlf::PlaceHolders::_1);
              } break;
              case 5: { // insert + read
                runFunc1 = bdlf::BindUtil::bind(
                     &Bench::insert, &hb, bdlf::PlaceHolders::_1);
              } break;
            } // END switch
            int tGId1 = tb.addThreadGroup(runFunc1, numThread1, load);

            int tGId2 = -1;
            if (testType == 5) {
                bsl::function<void(int)> runFunc2 = bdlf::BindUtil::bind(
                     &Bench::findExist, &hb, bdlf::PlaceHolders::_1);

                tGId2 = tb.addThreadGroup(runFunc2, numThread2, load);
            }
            tb.execute(&res,
                       numMillis,
                       numSamples,
                       initFunc,
                       bslmt::ThroughputBenchmark::ShutdownSampleFunction(),
                       cleanFunc);
            int countErr = hb.countErr();

            // Calculate percentiles.
            vector<double> percentiles(5);
            res.getPercentiles(&percentiles, tGId1);
            bsl::cout << bsl::fixed << bsl::setprecision(0) << testType << ","
                      << numThread1 << "," << numThread2 << ","
                      << numStripe << "," << numBucket << "," << load << ","
                      << percentage << "," << enblRehash << ","
                      << percentiles[0] << ","
                      << percentiles[1] << ","
                      << percentiles[2] << ","
                      << percentiles[3] << ","
                      << percentiles[4] << ","
                      << countErr;
            if (tGId2 >= 0) {
                res.getPercentiles(&percentiles, tGId2);
                bsl::cout << bsl::fixed << bsl::setprecision(0) << ","
                          << percentiles[0] << ","
                          << percentiles[1] << ","
                          << percentiles[2] << ","
                          << percentiles[3] << ","
                          << percentiles[4];
            }
            bsl::cout << "\n";

            // Update to the next index, and exit loop if end
            ++pctIdx;
            if (pctIdx < static_cast<int>(percentages.size())) continue;
            pctIdx = 0;
            ++loadIdx;
            if (loadIdx < static_cast<int>(loads.size())) continue;
            loadIdx = 0;
            ++numBucketIdx;
            if (numBucketIdx < static_cast<int>(numBuckets.size())) continue;
            numBucketIdx = 0;
            ++numStripeIdx;
            if (numStripeIdx < static_cast<int>(numStripes.size())) continue;
            numStripeIdx = 0;
            ++testTypeIdx;
            if (testTypeIdx < static_cast<int>(testTypes.size())) continue;
            testTypeIdx = 0;
            ++numThread2Idx;
            if (numThread2Idx < static_cast<int>(numThreads2.size())) continue;
            numThread2Idx = 0;
            ++numThread1Idx;
            if (numThread1Idx >= static_cast<int>(numThreads1.size())) break;
        }
      } break;
      // BDE_VERIFY pragma: +TP05
      case -4: {
        // --------------------------------------------------------------------
        // READ/WRITE PERFORMANCE TEST
        //   Tests performance of running both insert and tryGetValue against
        //   the hash, which is int to string.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to read or write.
        //   4th parameter: number of writer threads.
        //   5th parameter: number of stripes; if 0, use unordered_map and a
        //   global R/W mutex.
        //   6th parameter: number of buckets; ignore if number of stripes is
        //   0.
        //   7th parameter: 1-enable rehash; 0-disable rehash
        //   8th parameter: sparsity of values loaded.  Sparsity is the
        //   distance between consecutive values inserted, and represents how
        //   likely is a read to find the key given. A value of 1 means
        //   consecutive values inserted during initialization, and 100% chance
        //   of finding the key given.  A value of 2 means 50% chance of
        //   finding the key given.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for a
        //:   combination of reading a random generated key from the pre-loaded
        //:   hash, and inserting random values into the hash.
        //
        // Plan:
        //: 1 Create a HashPerformance object, and use initRead to pre-load
        //:   it.  High mark is set above eviction.  Then run testReadWrite.
        //:   Note that testReadWrite run inserts for the writer threads and
        //:   tryGetValue for the reader threads.  Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   READ WRITE PERFORMANCE
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "READ/WRITE PERFORMANCE TEST" << endl
                 << "===========================" << endl;

        //bslma::TestAllocator talloc("ptm4", veryVeryVeryVerbose);
        bslma::Allocator& talloc(*(bslma::Default::defaultAllocator()));

        int  numThreads  = argc > 2 ? atoi(argv[2]) : 1;
        int  numCalcs    = argc > 3 ? atoi(argv[3]) : 200000;
        int  numWThreads = argc > 4 ? atoi(argv[4]) : numThreads / 2;
        int  numStripes  = argc > 5 ? atoi(argv[5]) : 1;
        int  numBuckets  = argc > 6 ? atoi(argv[6]) : 1024;
        bool enblRehash  = argc > 7 ? atoi(argv[7]) : 1;
        int  sparsity    = argc > 8 ? atoi(argv[8]) : 1;

        hashPerf::HashPerformance hp("testRW1",
                                     numStripes,
                                     numBuckets,
                                     enblRehash,
                                     numThreads - numWThreads,
                                     numWThreads,
                                     numCalcs,
                                     10,
                                     &talloc);

        hashPerf::HashPerformance::VecIntType args(&talloc);  // args vector
        args.push_back(numCalcs);
        args.push_back(sparsity);
        int countIns = hp.initialize(args,
                                          hashPerf::HashPerformance::initRead);
        cout << "Inserted " << countIns << "\n";

        args.clear(); // Now, load args for testReadWrite
        args.push_back(numCalcs * sparsity);
        hashPerf::HashPerformance::VecTimeType times(&talloc);
        hp.runTests(&times, args, hashPerf::HashPerformance::testReadWrite);
        hp.printResult();
      } break;
      case -8: {
        // --------------------------------------------------------------------
        // READ/WRITE PERFORMANCE TEST WITH LONG KEY
        //   Tests performance of running both insert and getValue against
        //   the hash, which is string to int.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to read or write.
        //   4th parameter: number of writer threads.
        //   5th parameter: number of stripes; if 0, use unordered_map and a
        //   global R/W mutex.
        //   6th parameter: number of buckets; ignore if number of stripes is
        //   0.
        //   7th parameter: 1-enable rehash; 0-disable rehash
        //   8th parameter: sparsity of values loaded.  Sparsity is the
        //   distance between consecutive values inserted, and represents how
        //   likely is a read to find the key given. A value of 1 means
        //   consecutive values inserted during initialization, and 100% chance
        //   of finding the key given.  A value of 2 means 50% chance of
        //   finding the key given.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for a
        //:   combination of reading a random generated key from the pre-loaded
        //:   hash, and inserting random values into the hash.
        //
        // Plan:
        //: 1 Create a HashPerformance object, and use initRead2 to pre-load
        //:   it.  Then run testReadWrite.  Note that testReadWrite run inserts
        //:   for the writer threads and getValue for the reader threads.  Run
        //:   10 repetitions.  (C-1)
        //
        // Testing:
        //   READ/WRITE PERFORMANCE TEST WITH LONG KEY
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "READ/WRITE PERFORMANCE TEST WITH LONG KEY" << endl
                 << "=========================================" << endl;

        //bslma::TestAllocator talloc("ptm8", veryVeryVeryVerbose);
        bslma::Allocator& talloc(*(bslma::Default::defaultAllocator()));

        int  numThreads  = argc > 2 ? atoi(argv[2]) : 1;
        int  numCalcs    = argc > 3 ? atoi(argv[3]) : 200000;
        int  numWThreads = argc > 4 ? atoi(argv[4]) : numThreads / 2;
        int  numStripes  = argc > 5 ? atoi(argv[5]) : 1;
        int  numBuckets  = argc > 6 ? atoi(argv[6]) : 1024;
        bool enblRehash  = argc > 7 ? atoi(argv[7]) : 1;
        int  sparsity    = argc > 8 ? atoi(argv[8]) : 1;

        hashPerf::HashPerformance hp("testRW2", numStripes, numBuckets,
                enblRehash, numThreads - numWThreads, numWThreads, numCalcs,
                10, &talloc);

        hashPerf::HashPerformance::VecIntType args(&talloc);  // args vector
        args.push_back(numCalcs);
        args.push_back(sparsity);
        int countIns = hp.initialize(args,
                                        hashPerf::HashPerformance::initRead2);
        cout << "Inserted " << countIns << "\n";

        args.clear(); // Now, load args for testReadWrite
        args.push_back(numCalcs * sparsity);
        hashPerf::HashPerformance::VecTimeType times(&talloc);
        hp.runTests(&times, args, hashPerf::HashPerformance::testReadWrite2);
        hp.printResult();
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    // BDE_VERIFY pragma: +TP17

    if (test >= 0) {
        // CONCERN: In no case does memory come from the global allocator.

        ASSERT(gam.isTotalSame());
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
