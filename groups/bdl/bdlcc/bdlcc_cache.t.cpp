// bdlcc_cache.t.cpp                                                  -*-C++-*-

#include <bdlcc_cache.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>

#include <bdlb_random.h>
#include <bdlb_randomdevice.h>

#include <bslim_testutil.h>
#include <bslmt_threadutil.h>
#include <bslmt_semaphore.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslmf_assert.h>
#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_timeutil.h>  // 'CachePerformance'
#include <bsls_types.h>     // 'BloombergLP::bsls::Types::Int64'

#include <bsl_iostream.h>
#include <bsl_vector.h>
#include <bsl_string.h>
#include <bsl_iomanip.h>
#include <bsl_cstdlib.h>    // 'atoi', 'rand'
#include <bsl_cmath.h>      // 'sqrt'
#include <bsl_cstdio.h>     // 'sprintf'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a mechanism, 'bdlcc::Cache', that provides
// an in-memory key-value cache with a configurable eviction policy.  Because
// of the need to provide a thread-safe interface, 'bdlcc::Cache' is not a
// value-semantic type; therefore, it does not provide a copy constructor, the
// copy assignment operator, nor the equality comparison operators.
//
// Due to the similarity of 'bdlcc::Cache' with other standard containers, we
// will utilize some of the test cases strategy for VST to which 'bdlcc::Cache'
// applies.  Specifically, we will define the primary manipulators and the
// basic accessors, and we will substitute the equality-comparison operator
// with a custom function defined in this test driver.
//
// Two relatively unique aspects of this test driver are the need to ensure
// template type correctness and thread safety.  Since almost all of the
// template functionality of 'bdlcc::Cache' is delegated to implementation
// types, only the "pass-through" of this functionality needs to be tested.  As
// such, we will test the template with a few common types, including a type
// that allocates memory.  Thread-safety is achieved using a synchronization
// object and, as such, only verification that the lock is accessed correctly
// is needed.
//
// Primary Manipulators:
//: o 'insert'
//: o 'insertBulk'
//: o 'popFront'
//: o 'erase'
//: o 'eraseBulk'
//: o 'clear'
//: o 'tryGetValue'
//: o 'setPostEvictionCallback'
//
// Basic Accessors:
//: o 'equalFunction'
//: o 'evictionPolicy'
//: o 'hashFunction'
//: o 'highWatermark'
//: o 'lowWatermark'
//: o 'size'
//: o 'visit'
//
// Global Concerns:
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit Cache(bslma::Allocator *basicAllocator);
// [ 2] Cache(evictionPolicy, lowWatermark, highWatermark, basicAllocator);
// [ 2] Cache(evictionPolicy, lowWat, highWat, hashFunction, equal, alloc);
//
// MANIPULATORS
// [ 2] void insert(const KEYTYPE& key, const VALUETYPE& value);
// [10] void insert(const KEYTYPE& key, const ValuePtrType& valuePtr);
// [11] int insertBulk(const bsl::vector<KVType>& data);
// [ 5] int tryGetValue(value, KEYTYPE& key, bool modifyEvictionQueue);
// [ 9] int popFront();
// [ 6] int erase(const KEYTYPE& key);
// [ 7] int eraseBulk(const bsl::vector<KEYTYPE>& keys);
// [ 5] void setPostEvictionCallback(postEvictionCallback);
// [ 8] void clear();
//
// ACCESSORS
// [ 4] void visit(VISITOR& visitor) const;
// [ 4] CacheEvictionPolicy::Enum evictionPolicy() const;
// [ 4] bsl::size_t highWatermark() const;
// [ 4] bsl::size_t lowWatermark() const;
// [ 4] bsl::size_t size() const;
// [ 4] HASH hashFunction() const;
// [ 4] EQUAL equalFunction() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TEST APPARATUS
// [12] TYPE TRAITS
// [13] THREAD SAFETY
// [14] LOCKING TEST UTIL
// [15] LOCKING
// [16] USAGE EXAMPLE
// [-1] INSERT PERFORMANCE
// [-2] INSERT BULK PERFORMANCE
// [-3] READ PERFORMANCE
// [-4] READ WRITE PERFORMANCE

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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
#define TEST_TYPES                                                            \
    signed char,                                                              \
    size_t,                                                                   \
    bsltf::TemplateTestFacility::ObjectPtr,                                   \
    bsltf::AllocTestType

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


namespace usageExample1 {

void myPostEvictionCallback(bsl::shared_ptr<bsl::string> value)
{
    bsl::cout << "Evicted: " << *value << bsl::endl;
}

bslma::TestAllocator talloc("ue2", veryVeryVeryVerbose);

void example1()
{
    // This examples shows some basic usage of the cache.  First, we define a
    // custom post-eviction callback function, 'myPostEvictionCallback' that
    // simply prints the evicted item to stdout:
    //..
    //..
    // Then, we define a 'bdlcc::Cache' object, 'myCache', that maps 'int' to
    // 'bsl::string' and uses the LRU eviction policy:
    //..
    bdlcc::Cache<int, bsl::string> myCache(bdlcc::CacheEvictionPolicy::e_LRU,
                                          6, 7, &talloc);

    //..
    // Next, we insert 3 items into the cache and verify that the size of the
    // cache has been updated correctly:
    //..
    myCache.insert(0, "Alex");
    myCache.insert(1, "John");
    myCache.insert(2, "Rob");
    ASSERT(myCache.size() == 3);

    //..
    // Then, we bulk insert 3 additional items into the cache and verify that
    // the size of the cache has been updated correctly:
    //..
    typedef bsl::pair<int, bsl::shared_ptr<bsl::string> > PairType;
    bsl::vector<PairType> insertData(&talloc);
    insertData.push_back(PairType(3,
                          bsl::allocate_shared<bsl::string>(&talloc, "Jim" )));
    insertData.push_back(PairType(4,
                          bsl::allocate_shared<bsl::string>(&talloc, "Jeff")));
    insertData.push_back(PairType(5,
                          bsl::allocate_shared<bsl::string>(&talloc, "Ian" )));
    myCache.insertBulk(insertData);
    ASSERT(myCache.size() == 6);

    //..
    // Next, we retrieve the second value of the second item stored in the
    // cache using the 'tryGetValue' method:
    //..
    bsl::shared_ptr<bsl::string> value;
    int                          rc = myCache.tryGetValue(&value, 1);
    ASSERT(rc == 0);
    ASSERT(*value == "John");
    //..
    // Then, we set the cache's post-eviction callback to
    // 'myPostEvictionCallback':
    //..
    myCache.setPostEvictionCallback(myPostEvictionCallback);
    //..
    // Now, we insert two more items into the cache to trigger the eviction
    // behavior:
    //..
    myCache.insert(6, "Steve");
    ASSERT(myCache.size() == 7);
    myCache.insert(7, "Tim");
    ASSERT(myCache.size() == 6);
    //..
    // Notice that after we insert "Steve", the size of the cache is at the
    // high watermark.  After the following item, "Tim", is inserted, the size
    // of the cache goes back down to 6, the low watermark.
    //
    // Finally, we observe the following output to stdout:
    //..
    //  Evicted: Alex
    //  Evicted: Rob
    //..
    // Notice that the item "John" was not evicted even though it was inserted
    // before "Rob", because "John" was accessed after "Rob" was inserted.
}

}  // close namespace usageExample1

namespace usageExample2 {

bslma::TestAllocator talloc("ue2", veryVeryVeryVerbose);

// Suppose that a service needs to retrieve some values that are relatively
// expensive to compute.  Clients of the service cannot wait for computing the
// values, so the service should pre-compute and cache them.  In addition, the
// values are only valid for around one hour, so older items must be
// periodically updated in the cache.  This problem can be solved using
// 'bdlcc::Cache' with a background updater thread.
//
// First, we define the types representing the cached values and the cache
// itself:
//..
struct MyValue {
    int            d_data;       // data
    bdlt::Datetime d_timestamp;  // last update time stamp
};

typedef bdlcc::Cache<int, MyValue> MyCache;
//..
// Then, suppose that we have access to a function 'retrieveValue' that returns
// a 'MyValue' object given a 'int' key:
//..
MyValue retrieveValue(int key)
{
    MyValue ret = {key, bdlt::CurrentTime::utc()};
    return ret;
}
//..
// Next, we define a visitor type to aggregate keys of the out-of-date values
// in the cache:
//..
struct MyVisitor {
    // Visitor to 'MyCache'.

    bsl::vector<int>  d_oldKeys;  // list of out-of-date keys

    MyVisitor()
    : d_oldKeys(&talloc)
    {}

    bool operator() (int key, const MyValue& value)
        // Check if the specified 'value' is older than 1 hour.  If so, insert
        // the specified 'key' into 'd_oldKeys'.
    {
        if (veryVerbose) {
            bsl::cout << "Visiting " << key
                      << ", age: "
                      << bdlt::CurrentTime::utc() - value.d_timestamp
                      << bsl::endl;
        }

        if (bdlt::CurrentTime::utc() - value.d_timestamp <
            // bdlt::DatetimeInterval(0, 60)) {
            bdlt::DatetimeInterval(0, 0, 0, 3)) {
            return false;                                             // RETURN
        }

        d_oldKeys.push_back(key);
        return true;
    }
};
//..
// Then, we define the background thread function to find and update the
// out-of-date values:
//..
void myWorker(MyCache *cache)
{
    while (true) {
        if (cache->size() == 0) {
            break;
        }

        // Find and update the old values once per minute.
        // bslmt::ThreadUtil::microSleep(0, 60);
        bslmt::ThreadUtil::microSleep(0, 5);
        MyVisitor visitor;
        cache->visit(visitor);
        for (bsl::vector<int>::const_iterator itr =
             visitor.d_oldKeys.begin();
             itr != visitor.d_oldKeys.end(); ++itr) {
            if (veryVerbose) bsl::cout << "Updating " << *itr << bsl::endl;
            cache->insert(*itr, retrieveValue(*itr));
        }
    }
}

extern "C" void *myWorkerThread(void *v_cache)
{
    MyCache *cache = (MyCache *) v_cache;
    myWorker(cache);
    return 0;
}
//..
// Finally, we define the entry point of the application:
//..
void example2()
{
    MyCache myCache(bdlcc::CacheEvictionPolicy::e_FIFO, 100, 120, &talloc);

    // Pre-populate the cache.

    myCache.insert(0, retrieveValue(0));
    myCache.insert(1, retrieveValue(1));
    myCache.insert(2, retrieveValue(2));
    ASSERT(myCache.size() == 3);

    bslmt::ThreadUtil::Handle myWorkerHandle;

    int rc = bslmt::ThreadUtil::create(&myWorkerHandle, myWorkerThread,
                                       &myCache);
    ASSERT(rc == 0);

    // Do some work.

    bslmt::ThreadUtil::microSleep(0, 7);
    ASSERT(myCache.size() == 3);

    // Clean up.

    myCache.clear();
    ASSERT(myCache.size() == 0);
    bslmt::ThreadUtil::join(myWorkerHandle);
}
//..

}  // close namespace usageExample2

// Utilities
namespace {
typedef bsltf::TemplateTestFacility TstFacility;

template <class KEYTYPE, class VALUETYPE>
class PrintVisitor
{
    bsl::ostream *d_stream_p;

  public:
    explicit PrintVisitor(bsl::ostream *stream)
    : d_stream_p(stream)
    {}

    bool operator() (const KEYTYPE& key, const VALUETYPE& value)
    {
        *d_stream_p << TstFacility::getIdentifier(key)   << " => "
                    << TstFacility::getIdentifier(value) << ", ";
        return true;
    }
};

template <class                                       KEYTYPE,
          class                                       VALUETYPE,
          class                                       HASH,
          class                                       EQUAL,
          template <class, class, class, class> class CACHE>
static bsl::ostream& printCache(
                          bsl::ostream&                                 stream,
                          const CACHE<KEYTYPE, VALUETYPE, HASH, EQUAL>& obj)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    PrintVisitor<KEYTYPE, VALUETYPE> visitor(&stream);
    obj.visit(visitor);
    stream << bsl::endl;
    return stream;
}

template <class VALUE>
struct InplaceUtil {
    // The class is a wrapper to create a shared pointer in place, with and
    // without an allocator.  This struct implements 'SpCreateInplace'.
  private:
    // PRIVATE MANIPULATORS
    static void SpCreateInplaceImp(bsl::shared_ptr<VALUE> *ptr,
                                   const VALUE&            v,
                                   bslma::Allocator       *allocator,
                                   bsl::true_type);
    static void SpCreateInplaceImp(bsl::shared_ptr<VALUE> *ptr,
                                   const VALUE&            v,
                                   bslma::Allocator       *allocator,
                                   bsl::false_type);
        // Create a shared_ptr object in the specified 'ptr' with the specified
        // 'v' value, using the specified 'allocator'.  The last parameter
        // should be 'true_type' if 'VALUE' uses a 'bslma::Allocator', and
        // 'false_type' if it does not.  Note that both functions are just a
        // way to decide, at compile time, if an allocator is to be passed to
        // the function.
  public:
    // MANIPULATORS
    static void SpCreateInplace(bsl::shared_ptr<VALUE> *ptr,
                                const VALUE&            v,
                                bslma::Allocator       *allocator);
        // Create a shared_ptr object in the specified 'ptr' with the specified
        // 'v' value, using the specified 'allocator'.
};

// PRIVATE MANIPULATORS
template <class VALUE>
void InplaceUtil<VALUE>::SpCreateInplaceImp(bsl::shared_ptr<VALUE> *ptr,
                                            const VALUE&            v,
                                            bslma::Allocator       *allocator,
                                            bsl::true_type)
{
    ptr->createInplace(allocator, v, allocator);
}

template <class VALUE>
void InplaceUtil<VALUE>::SpCreateInplaceImp(bsl::shared_ptr<VALUE> *ptr,
                                            const VALUE&            v,
                                            bslma::Allocator       *allocator,
                                            bsl::false_type)
{
    ptr->createInplace(allocator, v);
}

// MANIPULATORS
template <class VALUE>
void InplaceUtil<VALUE>::SpCreateInplace(bsl::shared_ptr<VALUE> *ptr,
                                         const VALUE&            v,
                                         bslma::Allocator       *allocator)
{
    SpCreateInplaceImp(ptr, v, allocator, bslma::UsesBslmaAllocator<VALUE>());
}

}  // close unnamed namespace

namespace cacheperf {
class CachePerformance {
    // This class performs the various performance tests.

  public:
    typedef bdlcc::Cache<int, bsl::string>                CacheType;
    typedef bsl::pair<int, bsl::string>                   PairType;
    typedef bsl::pair<int, bsl::shared_ptr<bsl::string> > PairBulkType;
    typedef bsls::Types::Int64                            TimeType;
    typedef bsl::vector<bsls::Types::Int64>               VecTimeType;
    typedef bsl::vector<int>                              VecIntType;

    typedef int (*InitFunc)(CachePerformance*, VecIntType&);
        // Initialization function takes a vector of 'int', and returns 0 on
        // success, and negative value on failure.
    typedef int (*RunFunc)(CachePerformance*, VecIntType&);
        // A run function takes a pointer to 'CachePerformance', and a vector
        // of 'int'.  Returns a return code of 0 on success, and negative value
        // on failure.  The last entry in the vector of 'int' is the thread
        // number.  Writer threads are first, readers after.

    typedef struct WorkData {
        RunFunc           d_func;
        CachePerformance *d_cacheperf_p;
        VecIntType        d_data;
    } WorkData;

  private:
    // DATA
    bslma::Allocator *d_allocator_p; // memory allocator
    bsl::string       d_title;       // Title for printing purposes
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

    TimeType d_avgWTime;
    TimeType d_avgUTime;
    TimeType d_avgSTime;
        // Averages of wall time, user time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.
    TimeType d_seWTime;
    TimeType d_seUTime;
    TimeType d_seSTime;
        // Standard errors of wall time, user time and system time in nanos,
        // calculated from d_vecWTime, d_vecUTime, d_vecCTime respectively.
        // They are set to 0 if d_numRepeat = 1.
    CacheType d_cache;
    VecTimeType runTest(VecIntType& args, RunFunc func);
        // Run a single repetition of the calculation.  The calculation is
        // defined in the specified 'func' function, and takes as input the
        // specified 'args' vector.  Return for each thread a triad of elapsed
        // wall time, user time, and system time.

    // NOT IMPLEMENTED
    CachePerformance(const CachePerformance&);
    CachePerformance& operator=(const CachePerformance&);

  public:
    CachePerformance(const char                       *title,
                     bdlcc::CacheEvictionPolicy::Enum  evictionPolicy,
                     bsl::size_t                       lowWatermark,
                     bsl::size_t                       highWatermark,
                     int                               numRThreads,
                     int                               numWThreads,
                     int                               numCalcs,
                     int                               numRepeats,
                     bslma::Allocator                 *basicAllocator = 0);

    int initialize(VecIntType& args, InitFunc func);
        // Run the initialization function.  The initialization is defined in
        // the specified 'func' function, and takes as input the specified
        // 'args' vector.  Returns the return code from 'func'.
    VecTimeType runTests(VecIntType& args, RunFunc func);
        // Run the tests by running 'runTest' numRepeats times.  The test being
        // run is the defined in the specified 'func' function, and takes as
        // input the specified 'args' vector.  Return a triad of elapsed wall
        // time, user time, and system time.
    void printResult();
        // Print a standard time values output from the calculation.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the memory allocator

    CacheType& cache();
    bdlcc::CacheEvictionPolicy::Enum evictionPolicy() const;
        // Return the eviction policy used by this cache.

    bsl::size_t highWatermark() const;
        // Return the high watermark of this cache, which is the size at which
        // eviction of existing items begins.

    bsl::size_t lowWatermark() const;
        // Return the low watermark of this cache, which is the size at which
        // eviction of existing items ends.

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
        // Return the current size of this cache.

    const char* title() const;
        // Return the test title.

    // TEST FUNCTIONS
    static int testInsert(CachePerformance *cacheperf_p, VecIntType& args);
        // Insert rows, one at a time, into the specified 'cacheperf_p' using
        // the specified 'args' vector.  Only value in 'args' is the current
        // thread index, 0 to numThreads - 1.

    static int testInsertBulk(CachePerformance *cacheperf_p, VecIntType& args);
        // Insert rows, in bulk, into the specified 'cacheperf_p' using the
        // specified 'args' vector.  'args[0]' is the number of batches to use.
        // 'args[1]' (last parameter) is the current thread index, 0 to
        // numThreads - 1.

    static int initRead(CachePerformance *cacheperf_p, VecIntType& args);
        // Preload the specified cache in 'cacheperf_p' for subsequent tests
        // using the specified 'args' vector.  'args[0]' is the number of
        // values to load.  'arg[1]' is the sparsity of the data, that is, the
        // distance between consecutive values.  I.e: sparsity of 1 will
        // preload 0, 1, 2, ...  Sparsity of 2 will preload 0, 2, 4, ..., etc.

    static int testRead(CachePerformance *cacheperf_p, VecIntType& args);
        // Run tryGetValue multiple times against the specified 'cacheperf_p',
        // using the specified 'args' vector.  '0 to arg[0] - 1' is the range
        // to read from.  'args[1]' is the thread index, and is used as the
        // seed for the random number generating the key values.

    static int testReadWrite(CachePerformance *cacheperf_p, VecIntType& args);
        // Run either 'insert' or 'tryGetValue' multiple times against the
        // specified 'cacheperf_p' cache, with the specified 'args' vector.
        // 'args[0]' is the number of values to load.  'arg[1]' is the
        // sparsity: 1 - 0, 1, 2, ...; 2 - 0, 2, 4, ..., etc.
        // '0 to arg[2] - 1' is the range to read from.  The first numWThreads
        // are write threads.  The rest are read threads.

};  // END class CachePerformance

// FREE FUNCTIONS
extern "C" void *workFunc(void *arg);
    // The thread main function.

CachePerformance::CachePerformance(
                              const char                       *title,
                              bdlcc::CacheEvictionPolicy::Enum  evictionPolicy,
                              bsl::size_t                       lowWatermark,
                              bsl::size_t                       highWatermark,
                              int                               numRThreads,
                              int                               numWThreads,
                              int                               numCalcs,
                              int                               numRepeats,
                              bslma::Allocator                 *basicAllocator)
: d_allocator_p(basicAllocator)
, d_title(title, basicAllocator)
, d_numRThreads(numRThreads)
, d_numWThreads(numWThreads)
, d_numThreads(numRThreads + numWThreads)
, d_numCalcs(numCalcs)
, d_numRepeats(numRepeats)
, d_vecWTime(basicAllocator)
, d_vecUTime(basicAllocator)
, d_vecSTime(basicAllocator)
, d_cache(evictionPolicy, lowWatermark, highWatermark, basicAllocator)
{
}

int CachePerformance::initialize(VecIntType&                args,
                                 CachePerformance::InitFunc func)
{
    // Call initializing function
    int rc = func(this, args);
    return rc;
}

CachePerformance::VecTimeType CachePerformance::runTests(VecIntType&      args,
                                                CachePerformance::RunFunc func)
{
    d_seWTime = d_seUTime = d_seSTime = 0;
    d_avgWTime = d_avgUTime = d_avgSTime = 0;

    for(int j = 0; j < d_numRepeats; ++j) {
        VecTimeType times(d_allocator_p);
        times = runTest(args, func);
        TimeType curWTime = 0, curUTime = 0, curSTime = 0;
        for(int i = 0; i < d_numThreads; ++i) {
            d_vecWTime.push_back(times[i * 3    ]);
            d_vecUTime.push_back(times[i * 3 + 1]);
            d_vecSTime.push_back(times[i * 3 + 2]);
            curWTime += times[i * 3    ];
            curUTime += times[i * 3 + 1];
            curSTime += times[i * 3 + 2];
        }
        d_avgWTime += curWTime;
        d_avgUTime += curUTime;
        d_avgSTime += curSTime;
        d_seWTime += curWTime * curWTime;
        d_seUTime += curUTime * curUTime;
        d_seSTime += curSTime * curSTime;
    }
    d_avgWTime /= d_numRepeats;
    d_avgUTime /= d_numRepeats;
    d_avgSTime /= d_numRepeats;
    if (d_numRepeats == 1) {
        d_seWTime = d_seUTime = d_seSTime = 0;
    }
    else {
        // SD = SQRT(E(X^2) - E(X)^2)
        d_seWTime = static_cast<TimeType>(bsl::sqrt(static_cast<double>(
                  d_seWTime / d_numRepeats - d_avgWTime * d_avgWTime)));
        d_seUTime = static_cast<TimeType>(bsl::sqrt(static_cast<double>(
                  d_seUTime / d_numRepeats - d_avgUTime * d_avgUTime)));
        d_seSTime = static_cast<TimeType>(bsl::sqrt(static_cast<double>(
                  d_seSTime / d_numRepeats - d_avgSTime * d_avgSTime)));
    }
    VecTimeType ret(d_allocator_p);
    ret.push_back(d_avgWTime);
    ret.push_back(d_avgUTime);
    ret.push_back(d_avgSTime);
    return ret;
}

CachePerformance::VecTimeType CachePerformance::runTest(VecIntType&       args,
                                                CachePerformance::RunFunc func)
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
        VecTimeType times(d_allocator_p);
        times.push_back((endWTime - startWTime) / 1000);
        times.push_back((endUTime - startUTime) / 1000);
        times.push_back((endSTime - startSTime) / 1000);
        return times;                                                 // RETURN
    }

    bsl::vector<bslmt::ThreadUtil::Handle> handles(d_numThreads);
    bsl::vector<WorkData>                  todos(d_numThreads);

    // Spawn work threads
    for(int i = 0; i < d_numThreads; ++i) {
        todos[i].d_func = func;
        todos[i].d_cacheperf_p = this;
        todos[i].d_data = args;
        todos[i].d_data.push_back(i);

        bslmt::ThreadUtil::create(&handles[i], workFunc, &todos[i]);
    }
    // Collect results
    VecTimeType        times(d_allocator_p);
    bsl::vector<void*> results(d_numThreads);
    for(int i = 0; i < d_numThreads; ++i) {
        bslmt::ThreadUtil::join(handles[i], &results[i]);
        VecTimeType *pRes = reinterpret_cast<VecTimeType*>(results[i]);
        for(size_t j = 0; j < pRes->size(); ++j) times.push_back((*pRes)[j]);
        delete pRes;
    }

    return times;
}

void CachePerformance::printResult()
{
    // print the output from the calculation
    bsl::cout << "Title=" << d_title << "\n";
    bsl::cout << bsl::fixed << bsl::setprecision(1);
    bsl::cout << "Threads="   << d_numThreads
              << ",RThreads=" << d_numRThreads
              << ",WThreads=" << d_numWThreads << "\n";

    // Time is printed in milliseconds
    bsl::cout << "Wall Time="   << d_avgWTime / 1000 << "+/-"
            << static_cast<double>(d_seWTime) / static_cast<double>(d_avgWTime)
               * 100.0 << "%\n";
    bsl::cout << "User Time="   << d_avgUTime / 1000 << "+/-"
            << static_cast<double>(d_seUTime) / static_cast<double>(d_avgUTime)
               * 100.0 << "%\n";
    bsl::cout << "System Time="   << d_avgSTime / 1000 << "+/-"
            << static_cast<double>(d_seSTime) / static_cast<double>(d_avgSTime)
               * 100.0 << "%\n";
}

// ACCESSORS
inline
bslma::Allocator* CachePerformance::allocator() const
{
    return d_allocator_p;
}

inline
CachePerformance::CacheType& CachePerformance::cache()
{
    return d_cache;
}

inline
bdlcc::CacheEvictionPolicy::Enum
CachePerformance::evictionPolicy() const
{
    return d_cache.evictionPolicy();
}

inline
bsl::size_t CachePerformance::highWatermark() const
{
    return d_cache.highWatermark();
}

inline
bsl::size_t CachePerformance::lowWatermark() const
{
    return d_cache.lowWatermark();
}

inline
int CachePerformance::numCalcs() const
{
    return d_numCalcs;
}

inline
int CachePerformance::numRThreads() const
{
    return d_numRThreads;
}

inline
int CachePerformance::numRepeats() const
{
    return d_numRepeats;
}

inline
int CachePerformance::numThreads() const
{
    return d_numThreads;
}

inline
int CachePerformance::numWThreads() const
{
    return d_numWThreads;
}

inline
bsl::size_t CachePerformance::size() const
{
    return d_cache.size();
}

inline
const char* CachePerformance::title() const
{
    return d_title.c_str();
}


// TEST FUNCTIONS
int CachePerformance::testInsert(CachePerformance *cacheperf_p,
                                 VecIntType&       args)
{
    // Insert rows one at a time into the specified 'cacheperf_p' using the
    // specified 'args' vector.  Only value in 'args' is the current thread
    // index, 0 to 'numThreads - 1'.

    int numThreads = cacheperf_p->numThreads();
    int size = cacheperf_p->numCalcs() / numThreads;

    // Current thread number is the last (and only) argument

    int  threadIdx = args[0];
    int  offset = size * threadIdx;
    char buf[12];

    for(int i = 0; i < size; ++i) {
        int key = offset++;
        bsl::sprintf(buf, "V%d", key);
        string val(buf);
        cacheperf_p->cache().insert(key, val);
    }

    return 0;
}

int CachePerformance::testInsertBulk(CachePerformance *cacheperf_p,
                                     VecIntType&       args)
{
    // Insert rows in bulk into the specified 'cacheperf_p' using the
    // specified 'args' vector.  'args[0]' is the number of batches to use.
    // 'args[1]' (last parameter) is the current thread index,
    // 0 to 'numThreads - 1'.

    int  numThreads = cacheperf_p->numThreads();
    int  numBatches = args[0];
    int  threadIdx = args[1];
    int  size = cacheperf_p->numCalcs() / numThreads / numBatches;
    int  offset = size * numBatches * threadIdx;
    char buf[12];

    bsl::shared_ptr<bsl::string> valuePtr;
    for(int j = 0; j < numBatches; ++j) {
        bsl::vector<PairBulkType> insertData(size, cacheperf_p->d_allocator_p);
        for(int i = 0; i < size; ++i) {
            int key = offset++;
            bsl::sprintf(buf, "V%d", key);
            valuePtr.createInplace(cacheperf_p->d_allocator_p, buf,
                                                   cacheperf_p->d_allocator_p);
            insertData[i] = PairBulkType(key, valuePtr);
        }
        cacheperf_p->cache().insertBulk(insertData);
    }
    return 0;
}

int CachePerformance::initRead(CachePerformance *cacheperf_p, VecIntType& args)
{
    // Preload the specified cache in 'cacheperf_p' for subsequent tests using
    // the specified 'args' vector.  'args[0]' is the number of values to load.
    // 'arg[1]' is the sparsity of the data, that is, the distance between
    // consecutive values.  I.e: sparsity of 1 will preload 0, 1, 2, ...
    // Sparsity of 2 will preload 0, 2, 4, ..., etc.

    int  size = args[0];
    int  sparsity = args[1];
    int  key = 0;
    char buf[12];

    bsl::shared_ptr<bsl::string> valuePtr;

    bsl::vector<PairBulkType> insertData(size, cacheperf_p->d_allocator_p);
    for(int i = 0; i < size; ++i) {
        bsl::sprintf(buf, "V%d", key);
        valuePtr.createInplace(cacheperf_p->d_allocator_p, buf,
                                                 cacheperf_p->d_allocator_p);
        insertData[i] = PairBulkType(key, valuePtr);
        key += sparsity;
    }
    return cacheperf_p->cache().insertBulk(insertData);
}

int CachePerformance::testRead(CachePerformance *cacheperf_p, VecIntType& args)
{
    // Run tryGetValue multiple times against the specified 'cacheperf_p',
    // using the specified 'args' vector.  '0 to arg[0] - 1' is the range to
    // read from.  'args[1]' is the thread index, and is used as the seed for
    // the random number generating the key values.

    int numThreads = cacheperf_p->numThreads();
    int numCalcs = cacheperf_p->numCalcs();
    int numCalls = numCalcs / numThreads;
    int range = args[0];
    int threadIdx = args[1];
    int countErr = 0;
    srand(threadIdx);
    bsl::shared_ptr<bsl::string> valuePtr;
    for(int i = 0; i < numCalls; ++i) {
        int key = rand() % range;
        int rc = cacheperf_p->cache().tryGetValue(&valuePtr, key);
        if (rc != 0 && numCalcs == range) {
            cout << "rc=" << rc << ",key=" << key << ",i=" << i << "\n";
            ++countErr;
        }
    }
    if (countErr > 0)
        bsl::cout << "countErr=" << countErr << "\n";
    return 0;
}

int CachePerformance::testReadWrite(CachePerformance *cacheperf_p,
                                    VecIntType&       args)
{
    // Run either insert of tryGetValue multiple times.  'args[0]' is the
    // number of values to load.  'arg[1]' is the sparsity: 1 - 0, 1, 2, ...;
    // 2 - 0, 2, 4, ..., etc.  '0 to arg[2] - 1' is the range to read from.
    // The first numWThreads are write threads.  The rest are read threads.

    int countErr = 0;
    int numWThreads = cacheperf_p->numWThreads();
    int numCalcs = cacheperf_p->numCalcs();
    int range = args[0];
    int threadIdx = args[1];

    bool isWThread = threadIdx < numWThreads ? true : false;
    char buf[12];
    srand(threadIdx);
    bsl::shared_ptr<bsl::string> valuePtr;
    for(int i = 0; i < numCalcs; ++i) {
        int key = rand() % range;
        if (isWThread) {
            bsl::sprintf(buf, "V%d", key);
            string val(buf);
            cacheperf_p->cache().insert(key, val);
        }
        else {
            int rc = cacheperf_p->cache().tryGetValue(&valuePtr, key);
            if (rc != 0 && numCalcs == range) {
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
    CachePerformance::WorkData    *wdp =
                           reinterpret_cast<CachePerformance::WorkData *>(arg);
    CachePerformance::VecTimeType *pTimes =
          new CachePerformance::VecTimeType(wdp->d_cacheperf_p->allocator());

    CachePerformance::TimeType startWTime = bsls::TimeUtil::getTimer();

    CachePerformance::TimeType startUTime, startSTime;
    bsls::TimeUtil::getProcessTimers(&startSTime, &startUTime);

    int rc = wdp->d_func(wdp->d_cacheperf_p, wdp->d_data);
    (void) rc;

    CachePerformance::TimeType endWTime = bsls::TimeUtil::getTimer();

    CachePerformance::TimeType endUTime, endSTime;
    bsls::TimeUtil::getProcessTimers(&endSTime, &endUTime);

    pTimes->push_back((endWTime - startWTime) / 1000);
    pTimes->push_back((endUTime - startUTime) / 1000);
    pTimes->push_back((endSTime - startSTime) / 1000);
    return pTimes;
}

}  // close namespace cacheperf

namespace testLock {

bslma::TestAllocator talloc("tl", veryVeryVeryVerbose);

bslmt::Semaphore smp; // Used to sync worker thread and main thread

typedef bdlcc::Cache<int, bsl::string>          CacheType;
typedef bdlcc::Cache_TestUtil<int, bsl::string> Cache_TestUtilType;
typedef bsls::Types::Int64                      TimeType;
typedef bsl::pair<int, bsl::shared_ptr<bsl::string> > PairBulkType;

typedef struct ThreadData {
    Cache_TestUtilType *d_cacheTestUtil_p; // Pointer to bdlcc::Cache object
    unsigned short      d_sleep;           // sleep time, millisec
    char                d_lockType;        // 'R' - Read, 'W' - Write

    ThreadData(Cache_TestUtilType *cacheTestUtil_p,
               unsigned short      sleep,
               char                lockType)
    : d_cacheTestUtil_p(cacheTestUtil_p)
    , d_sleep(sleep)
    , d_lockType(lockType)
    {}
} ThreadData;

const unsigned short k_SLEEP_PERIOD = 100; // Sleep period of 100 millisec.

extern "C" void *workThread(void *arg)
{
    // Work function for the spawned thread: lockRead or LockWrite, sleep,
    // unlock.
    ThreadData *tdp = reinterpret_cast<ThreadData *>(arg);
    if (tdp->d_lockType == 'R') {
        tdp->d_cacheTestUtil_p->lockRead();
    }
    else { // 'W'
        tdp->d_cacheTestUtil_p->lockWrite();
    }
    smp.post();
    bslmt::ThreadUtil::microSleep(tdp->d_sleep * 1000, 0);
    tdp->d_cacheTestUtil_p->unlock();
    return 0;
}

void testTestingUtil()
{
    // ------------------------------------------------------------------------
    // Testing utility
    //
    // Concerns:
    //: 1 'bdlcc::Cache_TestUtil' methods of 'lockRead', 'lockWrite', and
    //:   'unlock' actually lock and unlock a reader writer lock.
    //
    // Plan:
    //: 1 Spawn a thread that calls 'lockWrite', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'lockWrite' and measure how long
    //:   it took to acquire the lock.  It should be around 0.1 sec.
    //:
    //: 2 Spawn a thread that calls 'lockWrite', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'lockRead' and measure how long
    //:   it took to acquire the lock.  It should be around 0.1 sec.
    //:
    //: 3 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'lockWrite' and measure how long
    //:   it took to acquire the lock.  It should be around 0.1 sec.
    //:
    //: 4 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'lockRead' and measure how long
    //:   it took to acquire the lock.  It should be much less than 0.1 sec.
    //:
    // Testing:
    //   void Cache_TestUtil::lockRead();
    //   void Cache_TestUtil::lockWrite();
    //   void Cache_TestUtil::unlock();
    // ------------------------------------------------------------------------

    CacheType                 cache(&talloc);
    Cache_TestUtilType        cache_TestUtil(cache);
    bslmt::ThreadUtil::Handle handle;
    void                     *result;
    ThreadData                tdWrite(&cache_TestUtil, k_SLEEP_PERIOD, 'W');

    // LockWrite / LockWrite test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        // Time the duration how long it took to lock write

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache_TestUtil.lockWrite();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      durationWW = static_cast<int>((endTime - startTime) / 1000);
        cache_TestUtil.unlock();
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationWW > k_SLEEP_PERIOD / 2);
    }

    // LockWrite / LockRead test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        // Time the duration how long it took to lock read

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache_TestUtil.lockRead();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      durationWR = static_cast<int>((endTime - startTime) / 1000);
        cache_TestUtil.unlock();
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationWR > k_SLEEP_PERIOD / 2);
    }

    ThreadData                tdRead(&cache_TestUtil, k_SLEEP_PERIOD, 'R');

    // LockRead / LockWrite test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to lock write

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache_TestUtil.lockWrite();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      durationRW = static_cast<int>((endTime - startTime) / 1000);
        cache_TestUtil.unlock();
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationRW > k_SLEEP_PERIOD / 2);
    }

    // LockRead / LockRead test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to lock read

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache_TestUtil.lockRead();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      durationRR = static_cast<int>((endTime - startTime) / 1000);
        cache_TestUtil.unlock();
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationRR < k_SLEEP_PERIOD / 2);
    }
} // END testTestingUtil

struct TestVisitor {
    // Visitor to the test 'bdlcc:Cache' in 'testLocking'.

    bool operator() (int key, const bsl::string& value)
    {
        // Print out key and value
        if (veryVerbose) {
            bsl::cout << "Visiting " << key
                      << ", value: "
                      << value
                      << bsl::endl;
        }

        return true;
    }
};

void testLocking()
{
    // ------------------------------------------------------------------------
    // Testing locking in 'bdlcc:Cache' methods
    //
    // Concerns:
    //: 1 'bdlcc::Cache_TestUtil' methods of 'insert', 'insertBulk', 'erase',
    //:   'eraseBulk', 'clear', and 'popFront' actually write lock and unlock a
    //:   reader writer lock.
    //
    //: 2 'bdlcc::Cache_TestUtil' methods of 'size', and 'visit' actually read
    //:   lock and unlock a reader writer lock.
    //
    //: 3 'bdlcc::Cache_TestUtil' method of 'tryGetValue' actually read lock
    //:   and unlock a reader writer lock if eviction policy is FIFO, and write
    //:   lock and unlock a reader writer lock if eviction policy is LRU.
    //
    // Plan:
    //: 1 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'insert' and measure how long it
    //:   took to complete.  It should be around 0.1 sec.
    //:
    //: 2 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'insertBulk' and measure how long
    //:   it took to complete.  It should be around 0.1 sec.
    //:
    //: 3 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'erase' and measure how long it
    //:   took to complete.  It should be around 0.1 sec.
    //:
    //: 4 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'eraseBulk' and measure how long
    //:   it took to complete.  It should be around 0.1 sec.
    //:
    //: 5 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'clear' and measure how long it
    //:   took to complete.  It should be around 0.1 sec.
    //:
    //: 6 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'popFront' and measure how long
    //:   it took to complete.  It should be around 0.1 sec.
    //:
    //: 7 Spawn a thread that calls 'lockWrite', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'size' and measure how long it
    //:   took to complete.  It should be around 0.1 sec.
    //:
    //: 8 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'size' and measure how long it
    //:   took to complete.  It should be less than 0.1 sec.
    //:
    //: 9 Spawn a thread that calls 'lockWrite', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'visit' and measure how long it
    //:   took to complete.  It should be around 0.1 sec.
    //:
    //:10 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, call 'visit' and measure how long it
    //:   took to complete.  It should be less than 0.1 sec.
    //:
    //:11 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, use a 'bdlcc:Cache' object with LRU
    //:   eviction policy, run 'tryGetValue' and measure how long it took to
    //:   complete.  It should be around 0.1 sec.
    //:
    //:12 Spawn a thread that calls 'lockWrite', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, use a 'bdlcc:Cache' object with FIFO
    //:   eviction policy, run 'tryGetValue' and measure how long it took to
    //:   complete.  It should be around 0.1 sec.
    //:
    //:13 Spawn a thread that calls 'lockRead', sleep for 0.1sec, and calls
    //:   'unlock'.  On the main thread, use a 'bdlcc:Cache' object with FIFO
    //:   eviction policy, run 'tryGetValue' and measure how long it took to
    //:   complete.  It should be less than sec.
    //:
    // Testing:
    //   void insert(const KEYTYPE& key, const VALUETYPE& value);
    //   void insert(const KEYTYPE& key, const ValuePtrType& valuePtr);
    //   int insertBulk(const bsl::vector<KVType>& data);
    //   int popFront();
    //   int erase(const KEYTYPE& key);
    //   int eraseBulk(const bsl::vector<KEYTYPE>& keys);
    //   void clear();
    //   int tryGetValue(value, KEYTYPE& key, bool modifyEvictionQueue);
    //   bsl::size_t size() const;
    // ------------------------------------------------------------------------

    CacheType                 cache(&talloc);
    Cache_TestUtilType        cache_TestUtil(cache);
    bslmt::ThreadUtil::Handle handle;
    void                     *result;
    ThreadData                tdWrite(&cache_TestUtil, k_SLEEP_PERIOD, 'W');
    ThreadData                tdRead( &cache_TestUtil, k_SLEEP_PERIOD, 'R');

    // LockRead / insert(const KEYTYPE& key, const VALUETYPE& value) test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'insert'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.insert(1, "John");
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / insert(const KEYTYPE& key, const ValuePtrType& valuePtr) test
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'insert'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::shared_ptr<bsl::string> valuePtr;
        valuePtr.createInplace(&talloc, "V2", &talloc);
        cache.insert(2, valuePtr);

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / insertBulk
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'insertBulk'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::shared_ptr<bsl::string> valuePtr;
        bsl::vector<PairBulkType>    insertData(2, &talloc);
        valuePtr.createInplace(&talloc, "V3", &talloc);
        insertData[0] = PairBulkType(3, valuePtr);
        valuePtr.createInplace(&talloc, "V4", &talloc);
        insertData[1] = PairBulkType(4, valuePtr);
        cache.insertBulk(insertData);

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / erase
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'erase'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.erase(1);
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / eraseBulk
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'eraseBulk'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::vector<int> eraseKeys(&talloc);
        eraseKeys.push_back(3);
        eraseKeys.push_back(4);
        cache.eraseBulk(eraseKeys);

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / clear
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'clear'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.clear();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / popFront
    {
        cache.insert(6, "Jack");
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'popFront'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.popFront();
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockWrite / size
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        // Time the duration how long it took to run 'size'

        TimeType startTime = bsls::TimeUtil::getTimer();

        std::size_t size = cache.size();
        (void) size;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / size
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'size'

        TimeType startTime = bsls::TimeUtil::getTimer();

        std::size_t size = cache.size();
        (void) size;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration < k_SLEEP_PERIOD / 2);
    }

    // LockWrite / visit
    {
        cache.insert(7, "Steve");
        cache.insert(8, "Ofer");
        TestVisitor visitor;

        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        // Time the duration how long it took to run 'visit'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.visit(visitor);
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / visit
    {
        TestVisitor visitor;

        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'visit'

        TimeType startTime = bsls::TimeUtil::getTimer();
        cache.visit(visitor);
        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration < k_SLEEP_PERIOD / 2);
    }

    // LockRead / tryGetValue, LRU
    {
        // The default cache eviction policy is LRU.
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to run 'tryGetValue'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::shared_ptr<bsl::string> valuePtr;

        int rc = cache.tryGetValue(&valuePtr, 8);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    CacheType          fifoCache(bdlcc::CacheEvictionPolicy::e_FIFO, 10, 20,
                                                                      &talloc);
    Cache_TestUtilType fifoCache_TestUtil(fifoCache);
    ThreadData         tdFifoWrite(&fifoCache_TestUtil, k_SLEEP_PERIOD, 'W');
    ThreadData         tdFifoRead( &fifoCache_TestUtil, k_SLEEP_PERIOD, 'R');
    // LockWrite / tryGetValue, FIFO
    {
        // The default cache eviction policy is LRU - need to create a FIFO one
        bslmt::ThreadUtil::create(&handle, workThread, &tdFifoWrite);
        smp.wait();

        // Time the duration how long it took to run 'tryGetValue'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::shared_ptr<bsl::string> valuePtr;

        int rc = fifoCache.tryGetValue(&valuePtr, 8);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    // LockRead / tryGetValue, FIFO
    {
        // The default cache eviction policy is LRU - need to create a FIFO one
        bslmt::ThreadUtil::create(&handle, workThread, &tdFifoRead);
        smp.wait();

        // Time the duration how long it took to run 'tryGetValue'

        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::shared_ptr<bsl::string> valuePtr;

        int rc = fifoCache.tryGetValue(&valuePtr, 8);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration < k_SLEEP_PERIOD / 2);
    }

}
}  // close namespace testLock

namespace threaded {


struct ThreadArg {
    typedef bdlcc::Cache<int, int> CacheType;

    CacheType       *d_cache_p;
    bsls::AtomicInt *d_stop_p;
    bsls::AtomicInt *d_writeCounts_p;
    int              d_numItems;
    int              d_workerId;
    int              d_numWorkers;
};

void worker(ThreadArg *arg)
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
            bsl::cout << arg->d_workerId << ": " << key << bsl::endl;
        }
        ThreadArg::CacheType::ValuePtrType valuePtr;

        int rc = arg->d_cache_p->tryGetValue(&valuePtr, key, true);

        arg->d_cache_p->insert(key, 0);
        if (0 != rc) {
            arg->d_cache_p->insert(key, 0);
            arg->d_writeCounts_p[key] = 0;
        }
        else {
            arg->d_cache_p->insert(key, (*valuePtr) + 1);
            ++arg->d_writeCounts_p[key];
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
{
    // ------------------------------------------------------------------------
    // STRESS MULTITHREADED TEST
    //
    // Concerns:
    //: 1 The code contains no deadlocks or race conditions.
    //
    // Plan:
    //   Test for thread-safety using many simultaneous threads inserting and
    //   reading items from a shared cache.  Each worker thread gets assigned a
    //   unique id that determines the keys for which it is responsible.  Each
    //   time a key is read, it is incremented by 1 using the 'insert' method.
    //   This is done for a fixed period of time before all worker threads are
    //   joined with the main thread.  The value stored for each key is then
    //   verified with the counted number of inserts stored in an atomic
    //   integral value.
    //
    // Testing:
    //   Threaded test
    // ------------------------------------------------------------------------

    const int k_NUM_WORKERS = 10;
    const int k_NUM_ITEMS = 128;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ThreadArg::CacheType cache(&scratch);
    bsls::AtomicInt      writeCounts[k_NUM_ITEMS];  // default 0
    bsls::AtomicInt      stop(0);

    bslmt::ThreadUtil::Handle queryHandles[k_NUM_WORKERS];
    ThreadArg                 args[k_NUM_WORKERS];

    for (int i = 0; i < k_NUM_WORKERS; ++i) {
        ThreadArg arg = {&cache, &stop, writeCounts,
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

    if (veryVerbose) {
        printCache(bsl::cout, cache);
    }

    for (int i = 0; i < k_NUM_ITEMS; ++i) {
        ThreadArg::CacheType::ValuePtrType valuePtr;

        int rc = cache.tryGetValue(&valuePtr, i, false);
        int count = rc != 0 ? 0 : *valuePtr;

        ASSERTV(i, count, writeCounts[i], count == writeCounts[i]);

        if (veryVerbose) {
            bsl::cout << i << ": " << writeCounts[i] << bsl::endl;
        }
    }
}

}  // close namespace threaded

// TestDriver template
namespace {

template <class KEYTYPE, class VALUETYPE, class ALLOC>
struct IntToPairConverter {
    // Convert an 'int' value to a 'bsl::pair' of the template parameter 'KEY'
    // and 'VALUE' types.

    // CLASS METHODS
    static void
    createInplace(bsl::pair<KEYTYPE, VALUETYPE> *address,
                  int                            value,
                  ALLOC                          allocator)
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

        bsls::ObjectBuffer<typename bsl::remove_const<KEYTYPE>::type> tempKey;
        bsltf::TemplateTestFacility::emplace(tempKey.address(),
                                             value,
                                             privateAllocator);

        bsls::ObjectBuffer<VALUETYPE> tempValue;
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
    : d_id(id)
    {}

    // ACCESSORS
    bsl::size_t operator() (const TYPE& obj) const
        // Return the has value of the specified 'obj'.
    {
        return  bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator==(const TestHashFunctor& rhs) const
    {
        return d_id == rhs.d_id;
    }

    int id() const
    {
        return d_id;
    }
};

template <class TYPE>
bool areEqual(const TYPE& lhs, const TYPE& rhs)
{
    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
        == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
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
    {
        return d_id == rhs.d_id;
    }

    int id() const
    {
        return d_id;
    }
};


template <class KEYTYPE,
          class VALUETYPE = KEYTYPE,
          class HASH  = TestHashFunctor<KEYTYPE>,
          class EQUAL = TestEqualityComparator<KEYTYPE> >
class TestDriver {
    // This templatized struct provide a namespace for testing 'bdlcc::Cache'.
    // The parameterized 'KEYTYPE', 'VALUETYPE', 'HASH', 'EQUAL' specifies the
    // key type, the mapped type, the hash functor, the equality comparator
    // type respectively.  Each "testCase*" method test a specific aspect of
    // 'bdlcc:Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>'.  Every test cases should
    // be invoked with various parameterized type to fully test the cache.

  private:
    // TYPES
    typedef bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL> Obj;

    // Shorthands
    typedef bsl::pair<KEYTYPE, VALUETYPE> PairType;
    typedef bsl::allocator<PairType>      PairStdAllocator;

    typedef bsltf::TestValuesArray<PairType,
                                   PairStdAllocator,
                                   IntToPairConverter<
                                                KEYTYPE,
                                                VALUETYPE,
                                                PairStdAllocator> > TestValues;

    class TestPostEvictionCallback {
        // This class implements a callback functor for testing purposes that
        // can used as the argument to the 'setPostEvictionCallback' method.
        // The arguments passed to the value constructor is used to verify the
        // values received by function-call operator.  After the operations
        // being tested have been ran, the 'assertEnd' method should be called
        // to ensure that all of the expected values have been received.

      private:
        // DATA
        const TestValues *d_values_p;  // array values to compare with

        bsl::size_t      *d_pos_p;     // current position from 0.  If 'd_idx'
                                       // is not 0, then this value corresponds
                                       // to the position in the 'd_idx'.  This
                                       // needs to be a pointer so that copy
                                       // constructed objects will modify the
                                       // position in the original value, which
                                       // is required for 'assertEnd' to work
                                       // correctly.

        bsl::size_t       d_size;      // expected size of evicted values

        bsl::size_t       d_start;     // start position.  The actual position
                                       // in 'd_values_p' is either
                                       // '*d_pos_p + d_start' or
                                       // 'd_idx[*d_pos_p]' if 'd_idx' is not
                                       // 0.

        bsl::size_t      *d_idx_p;       // array of indexes in 'd_values_p'

      public:
        TestPostEvictionCallback(const TestValues *values,
                                 bsl::size_t      *pos_p,
                                 bsl::size_t       size,
                                 bsl::size_t       start = 0,
                                 bsl::size_t      *idx = 0)
        : d_values_p(values)
        , d_pos_p(pos_p)
        , d_size(size)
        , d_start(start)
        , d_idx_p(idx)
        {
            *d_pos_p = 0;
        }

        void operator() (const bsl::shared_ptr<VALUETYPE>& value)
        {
            if (veryVerbose) {
                bsl::cout << "Post-eviction callback: ["
                          << TstFacility::getIdentifier(*value)
                          << "]" << bsl::endl;
            }

            bsl::size_t idx;
            bool isExpected;
            callOpImp(&idx, &isExpected, value);
            const PairType& pair = (*d_values_p)[idx];

            ASSERTV(TstFacility::getIdentifier(*value),
                    TstFacility::getIdentifier(pair.second),
                    isExpected);
        }

        void assertEnd() {
            ASSERTV(*d_pos_p, d_size, assertEndImp());
        }

        bool assertEndImp() {
            return *d_pos_p == d_size;
        }

        void callOpImp(bsl::size_t                      *idx,
                       bool                             *isExpected,
                       const bsl::shared_ptr<VALUETYPE>& value)
        {
            if (d_idx_p) {
                *idx = d_idx_p[*d_pos_p];
            }
            else {
                *idx = *d_pos_p + d_start;
            }
            ++(*d_pos_p);
            const PairType& pair = (*d_values_p)[*idx];
            *isExpected = areEqual(*value, pair.second);
        }
    };

    class TestVisitor {
        // This class implements a visitor functor for testing purposes that
        // can be used as the argument to the 'visit' method.  The arguments
        //  passed to the value constructor is used to verify the values
        // received by function-call operator.  Finally, the 'assertEnd' method
        //  should be called to ensure that all of the expected values have
        // been received.

      public:
        struct Args {
            bsl::size_t d_idx;
            bool        d_ret;
            Args(){}
            Args(bsl::size_t idx, bool ret ) : d_idx(idx), d_ret(ret) {}
        };

      private:
        // DATA
        const TestValues *d_values_p; // array of expected values to visit

        bsl::size_t       d_size;     // expected size of values to visit

        const Args       *d_args_p;   // indices pointing to 'd_values_p' array
                                      // and values to return from the
                                      // function-call operator

        bsl::size_t       d_pos;      // current position from 0.  If
                                      // 'd_args_p' is not 0, then this value
                                      // corresponds to the position in the
                                      // 'd_args_p'.  Otherwise, it indicates
                                      // the position directly

      public:
        TestVisitor(const TestValues *values,
                    bsl::size_t       size,
                    const Args       *args = 0)
        : d_values_p(values)
        , d_size(size)
        , d_args_p(args)
        , d_pos(0)
        {}

        bool operator() (const KEYTYPE& key, const VALUETYPE& value)
        {
            if (veryVerbose) {
                bsl::cout << "Visitor: ["
                          << TstFacility::getIdentifier(key) << ", "
                          << TstFacility::getIdentifier(value)
                          << "]" << bsl::endl;
            }

            bsl::size_t idx;
            bool        isKeyExpected;
            bool        isValueExpected;

            bool ret = callOpImp(&idx, &isKeyExpected, &isValueExpected,
                                 key, value);

            const PairType& pair = (*d_values_p)[idx];
            ASSERTV(TstFacility::getIdentifier(key),
                    TstFacility::getIdentifier(pair.first),
                    isKeyExpected);
            ASSERTV(TstFacility::getIdentifier(value),
                    TstFacility::getIdentifier(pair.second),
                    isValueExpected);

            return ret;
        }

        void assertEnd() {
            ASSERTV(assertEndImp());
        }


        bool callOpImp(bsl::size_t      *idx,
                       bool             *isKeyExpected,
                       bool             *isValueExpected,
                       const KEYTYPE&    key,
                       const VALUETYPE&  value)
        {
            bool ret;
            if (d_args_p) {
                *idx = d_args_p[d_pos].d_idx;
                ret = d_args_p[d_pos].d_ret;
            }
            else {
                *idx = d_pos;
                ret = true;
            }

            ++d_pos;

            const PairType& pair = (*d_values_p)[*idx];
            *isKeyExpected = areEqual(key, pair.first);
            *isValueExpected = areEqual(value, pair.second);

            return ret;
        }

        bool assertEndImp() {
            return d_pos == d_size;
        }
    };

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
};

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase12()
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

    BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase11()
{
    // ------------------------------------------------------------------------
    // 'insertBulk'
    //
    // Concerns:
    //: 1 'insertBulk' provides exception neutrality guarantee.
    //
    // Plan:
    //: 1 Using the loop-based approach, insert items using the 'insert' method
    //:   taking a shared pointer and verify that the resulting object.  Test
    //:   for exception safety guarantee using the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST' macros.
    //
    // Testing:
    //   int insertBulk(const bsl::vector<KVType>& data);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    const TestValues VALUES;  // contains 52 distinct increasing values

    const bsl::size_t MAX_LENGTH = 9;

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 100, 100, &oa);
            const Obj& X = mX;

            bsl::vector<typename Obj::KVType> insertVec(&oa);
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                typename Obj::ValuePtrType value;
                InplaceUtil<VALUETYPE>::SpCreateInplace(&value,
                                                       VALUES[tj].second, &oa);
                typename Obj::KVType item(VALUES[tj].first, value, &oa);
                insertVec.push_back(item);
            }
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                mX.insertBulk(insertVec);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            TestVisitor visitor(&VALUES, LENGTH);
            X.visit(visitor);
            visitor.assertEnd();
            ASSERTV(LENGTH, X.size(), LENGTH == X.size())
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase10()
{
    // ------------------------------------------------------------------------
    // 'insert' TAKING A SHARED POINTER
    //   'insert' taking a value as a const reference has already been tested
    //   in test case 2.
    //
    // Concerns:
    //: 1 'insert' provides exception neutrality guarantee.
    //
    // Plan:
    //: 1 Using the loop-based approach, insert items using the 'insert' method
    //:   taking a shared pointer and verify that the resulting object.  Test
    //:   for exception safety guarantee using the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST' macros.
    //
    // Testing:
    //   void insert(const KEYTYPE& key, const ValuePtrType& valuePtr);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    const TestValues VALUES;  // contains 52 distinct increasing values

    const bsl::size_t MAX_LENGTH = 9;

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 100, 100, &oa);
            const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                typename Obj::ValuePtrType value;
                InplaceUtil<VALUETYPE>::SpCreateInplace(&value,
                                                       VALUES[tj].second, &oa);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    mX.insert(VALUES[tj].first, value);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            }

            TestVisitor visitor(&VALUES, LENGTH);
            X.visit(visitor);
            visitor.assertEnd();
            ASSERTV(LENGTH, X.size(), LENGTH == X.size())
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase9()
{
    // ------------------------------------------------------------------------
    // 'popFront'
    //
    // Concerns:
    //: 1 'popFront' removes the first item in the cache's queue.
    //:
    //: 2 'popFront' returns 0 on success and non-zero if the cache is empty.
    //:
    //: 3 'popFront' evokes the post-eviction callback on success.
    //
    // Plan:
    //: 1 Using the loop based approach, call 'popFront' for objects having
    //:   different lengths and verify the method behaves correctly.
    //
    // Testing:
    //   int popFront();
    // ------------------------------------------------------------------------

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 90, 100, &scratch);
            const Obj& X = mX;
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insert(VALUES[tj].first, VALUES[tj].second);
            }
            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }

            bsl::vector<typename TestVisitor::Args> expected(&scratch);
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                typename TestVisitor::Args args(tj, true);
                expected.push_back(args);
            }

            bsl::size_t pos;
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                TestPostEvictionCallback callback(&VALUES, &pos, 1, tj);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                int rc = mX.popFront();
                ASSERTV(rc == 0);
                const bsl::size_t EXPECTED_LENGTH = LENGTH - tj - 1;
                callback.assertEnd();
                ASSERTV(EXPECTED_LENGTH == X.size());

                expected.erase(expected.begin());
                {
                    TestVisitor visitor(&VALUES, EXPECTED_LENGTH,
                                        expected.data());
                    X.visit(visitor);
                    visitor.assertEnd();
                }
            }
            TestPostEvictionCallback callback(&VALUES, &pos, 0, 0);

            typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                PostEvictionCallback
                callbackFunc (bsl::allocator_arg, &scratch, callback);
            mX.setPostEvictionCallback(callbackFunc);
            int rc = mX.popFront();
            ASSERTV(0 != rc);
            ASSERTV(0 == mX.size());
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase8()
{
    // ------------------------------------------------------------------------
    // 'clear'
    //
    // Concerns:
    //: 1 The 'clear' removes all items from the cache.
    //:
    //: 2 The post-eviction callback is not invoked.
    //
    // Plan:
    //: 1 Using the loop based approach, call 'clear' for objects having
    //:   different lengths and verify the method behaves correctly.
    //
    // Testing:
    //   void clear();
    // ------------------------------------------------------------------------

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 90, 100, &scratch);
            const Obj& X = mX;
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insert(VALUES[tj].first, VALUES[tj].second);
            }
            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }

            bsl::size_t              pos;
            TestPostEvictionCallback callback(&VALUES, &pos, 0, 0);

            typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                PostEvictionCallback
                callbackFunc (bsl::allocator_arg, &scratch, callback);
            mX.setPostEvictionCallback(callbackFunc);
            mX.clear();
            callback.assertEnd();
            {
                TestVisitor visitor(&VALUES, 0);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(0 == X.size());
            }
        }
    }

}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase7()
{
    // ------------------------------------------------------------------------
    // 'eraseBulk'
    //
    // Concerns:
    //: 1 The 'eraseBulk' method removes the specified keys from the cache. The
    //:   method returns the number of items successfully removed.
    //:
    //: 2 The 'erase' method invokes the post-eviction callback on success,
    //:   passing the value of the evicted item.
    //
    // Plan:
    //: 1 Using the loop-based approach, make sure that erasing a valid key
    //:   successfully removes the item with that key and calls the
    //:   post-eviction callback.
    //:
    //: 2 Using the loop-based approach, make sure that erasing an invalid key
    //:   returns a non-zero value.
    //
    // Testing:
    //   int eraseBulk(const bsl::vector<KEYTYPE>& keys);
    // ------------------------------------------------------------------------

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            bsl::vector<typename TestVisitor::Args> allValues(&scratch);
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                typename TestVisitor::Args args(tj, true);
                allValues.push_back(args);
            }

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                Obj        mX(policy, 100, 100, &scratch);
                const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insert(VALUES[v].first, VALUES[v].second);
                }

                {
                    TestVisitor visitor(&VALUES, LENGTH);
                    X.visit(visitor);
                    visitor.assertEnd();
                }
                bsl::size_t              pos;
                TestPostEvictionCallback callback(&VALUES, &pos, tj, 0);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                ASSERTV(LENGTH == X.size());

                // Keys to be erased with 'eraseBulk'
                bsl::vector<KEYTYPE> eraseKeys(&scratch);
                for (bsl::size_t v = 0; v < tj; ++v) {
                    eraseKeys.push_back(VALUES[v].first);
                }
                bsl::size_t count = mX.eraseBulk(eraseKeys);
                ASSERTV(tj == count);
                ASSERTV(LENGTH - count == X.size());
                callback.assertEnd();
            }
        }
    }

    // Test erasing non-existent keys.

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 90, 100, &scratch);
            const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insert(VALUES[tj].first, VALUES[tj].second);
            }
            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }

            bsl::vector<KEYTYPE> eraseKeys(&scratch);
            for (bsl::size_t v = LENGTH; v < 2*LENGTH; ++v) {
                eraseKeys.push_back(VALUES[v].first);
            }
            int count = mX.eraseBulk(eraseKeys);
            ASSERTV(0 == count);

            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase6()
{
    // ------------------------------------------------------------------------
    // 'erase'
    //
    // Concerns:
    //: 1 The 'erase' method removes the specified key from the cache. The
    //:   method returns 0 on success and non-zero if the key doesn't exist in
    //:   the cache.
    //:
    //: 2 The 'erase' method invokes the post-eviction callback on success,
    //:   passing the value of the evicted item.
    //
    // Plan:
    //: 1 Using the loop-based approach, make sure that erasing a valid key
    //:   successfully removes the item with that key and calls the
    //:   post-eviction callback.
    //:
    //: 2 Using the loop-based approach, make sure that erasing an invalid key
    //:   returns a non-zero value.
    //
    // Testing:
    //   int erase(const KEYTYPE& key);
    // ------------------------------------------------------------------------

    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            bsl::vector<typename TestVisitor::Args> allValues(&scratch);
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                typename TestVisitor::Args args(tj, true);
                allValues.push_back(args);
            }

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                Obj        mX(policy, 100, 100, &scratch);
                const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insert(VALUES[v].first, VALUES[v].second);
                }

                {
                    TestVisitor visitor(&VALUES, LENGTH);
                    X.visit(visitor);
                    visitor.assertEnd();
                }
                bsl::size_t              pos;
                TestPostEvictionCallback callback(&VALUES, &pos, 1, tj);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                ASSERTV(LENGTH == X.size());
                int rc = mX.erase(VALUES[tj].first);
                ASSERTV(rc == 0);
                ASSERTV(LENGTH - 1 == X.size());
                callback.assertEnd();

                bsl::vector<typename TestVisitor::Args> expected(allValues,
                                                                 &scratch);
                expected.erase(expected.begin() + tj);
                {
                    TestVisitor visitor(&VALUES, LENGTH - 1, expected.data());
                    X.visit(visitor);
                    visitor.assertEnd();
                }
            }
        }
    }

    // Test erasing non-existent keys.

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 90, 100, &scratch);
            const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insert(VALUES[tj].first, VALUES[tj].second);
            }
            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }

            int rc = mX.erase(VALUES[LENGTH].first);
            ASSERTV(0 != rc);

            {
                TestVisitor visitor(&VALUES, LENGTH);
                X.visit(visitor);
                visitor.assertEnd();
                ASSERTV(LENGTH == X.size());
            }
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase5()
{
    // ------------------------------------------------------------------------
    // Eviction Policies
    //
    // Concerns:
    //: 1 Eviction starts when size of the cache is at its high watermark.  For
    //:   example, if a cache with a high watermark of 3 has a size of 3, then
    //:   inserting an item into the cache will result in the eviction of least
    //:   one item.
    //:
    //: 2 Low watermark is the size of the cache at which eviction stops.  For
    //:   example, if a cache with a high watermark of 3 and low watermark of
    //:   2, then inserting an item into the cache will result in eviction of
    //:   exactly 2 items.  In otherwords, when an item is inserted into the
    //:   cache at the high watermark, the final size of the cache after this
    //:   insertion operation is the low watermark.
    //:
    //: 3 For FIFO, the items are evicted in the order of insertion, with the
    //:   elements first inserted being evicted first.
    //:
    //: 4 For LRU, the items are evicted in the order of access, with the
    //:   elements least-recently accessed evicted first.  If no elements has
    //:   been accessed, this policy is equivalent to FIFO.
    //:
    //: 5 'tryGetValue' only changes the eviction order if its argument
    //:   'modifyEvictionQueue' is true, which is the default value.
    //
    // Plan:
    //: 1 Use the loop-based approach to test 'insert' without any item access
    //:   for objects with LRU or FIFO eviction policies.
    //:
    //: 2 Using the loop-based approach, make sure that calling 'tryGetValue'
    //:   for a FIFO cache, or calling 'tryGetValue' with 'modifyEvictionQueue'
    //:   set to 'false' for a LRU cache does not modify the eviction order.
    //:
    //: 3 Using the loop-based approach, verify that calling 'tryGetValue' for
    //:   a LRU queue with 'modifyEvictionQueue' set to 'true' does modify the
    //:   eviction order appropriately.
    //
    // Testing:
    //   void setPostEvictionCallback(postEvictionCallback);
    //   int tryGetValue(value, KEYTYPE& key, bool modifyEvictionQueue);
    // ------------------------------------------------------------------------

    const TestValues     VALUES;  // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // Testing FIFO and LRU without any item access.
    {
        const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
            bdlcc::CacheEvictionPolicy::e_LRU,
            bdlcc::CacheEvictionPolicy::e_FIFO
        };

        const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

        for (int tp = 0; tp < NUM_POLICIES; ++tp) {
            bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
                {
                    Obj                      mX(policy, 1, 1, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 1, 1, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 1, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    callback.assertEnd();
                }
            }
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 2; ++ti) {
                {
                    Obj                      mX(policy, 1, 2, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 1, 2, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 2, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    callback.assertEnd();
                }
            }
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 3; ++ti) {
                {
                    Obj                      mX(policy, 1, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 1, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 3, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    mX.insert(VALUES[ti + 3].first, VALUES[ti + 3].second);
                    callback.assertEnd();
                }
            }
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 2; ++ti) {
                {
                    Obj                      mX(policy, 2, 2, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 2, 2, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 1, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    callback.assertEnd();
                }
            }
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 3; ++ti) {
                {
                    Obj                      mX(policy, 2, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 2, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 2, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 3].second);
                    callback.assertEnd();
                }
            }
            for (bsl::size_t ti = 0; ti < MAX_LENGTH - 3; ++ti) {
                {
                    Obj                      mX(policy, 3, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 0, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    callback.assertEnd();
                }
                {
                    Obj                      mX(policy, 3, 3, &scratch);
                    bsl::size_t              pos;
                    TestPostEvictionCallback callback(&VALUES, &pos, 1, ti);

                    typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                        PostEvictionCallback
                        callbackFunc (bsl::allocator_arg, &scratch, callback);
                    mX.setPostEvictionCallback(callbackFunc);

                    mX.insert(VALUES[ti].first, VALUES[ti].second);
                    mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                    mX.insert(VALUES[ti + 2].first, VALUES[ti + 3].second);
                    callback.assertEnd();
                }
            }
        }
    }

    // Testing LRU with access.
    {
        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 1, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti };
                TestPostEvictionCallback callback(&VALUES, &pos, 1, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);
                mX.insert(VALUES[ti].first, VALUES[ti].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti].first);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                callback.assertEnd();
            }
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 1, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti };
                TestPostEvictionCallback callback(&VALUES, &pos, 1, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                mX.insert(VALUES[ti].first, VALUES[ti].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti].first, false);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                callback.assertEnd();
            }
        }
        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 2; ++ti) {
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 2, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti + 1, ti };
                TestPostEvictionCallback callback(&VALUES, &pos, 2, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                mX.insert(VALUES[ti].first, VALUES[ti].second);
                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti].first);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                callback.assertEnd();
            }
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 2, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti, ti + 1 };
                TestPostEvictionCallback callback(&VALUES, &pos, 2, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                mX.insert(VALUES[ti].first, VALUES[ti].second);
                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti].first, false);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);
                callback.assertEnd();
            }
        }
        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 3; ++ti) {
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 3, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti + 2,  ti + 1, ti };
                TestPostEvictionCallback callback(&VALUES, &pos, 3, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                mX.insert(VALUES[ti].first, VALUES[ti].second);
                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti + 1].first);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti + 1].second));

                rc = mX.tryGetValue(&ptr, VALUES[ti].first);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 3].first, VALUES[ti + 3].second);
                callback.assertEnd();
            }
            {
                Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 3, &scratch);

                bsl::size_t              pos;
                bsl::size_t              indexes[] = { ti, ti + 1, ti + 2 };
                TestPostEvictionCallback callback(&VALUES, &pos, 3, 0,
                                                  indexes);

                typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
                    PostEvictionCallback
                    callbackFunc (bsl::allocator_arg, &scratch, callback);
                mX.setPostEvictionCallback(callbackFunc);

                mX.insert(VALUES[ti].first, VALUES[ti].second);
                mX.insert(VALUES[ti + 1].first, VALUES[ti + 1].second);
                mX.insert(VALUES[ti + 2].first, VALUES[ti + 2].second);

                typename Obj::ValuePtrType ptr;

                int rc = mX.tryGetValue(&ptr, VALUES[ti + 1].first, false);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti + 1].second));

                rc = mX.tryGetValue(&ptr, VALUES[ti].first, false);
                ASSERTV(0 == rc);
                ASSERTV(areEqual(*ptr, VALUES[ti].second));

                mX.insert(VALUES[ti + 3].first, VALUES[ti + 3].second);
                callback.assertEnd();
            }
        }
    }
}

template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //
    // Plan:
    //: 1 Using the loop-based approach, create objects having different
    //:   eviction policies and items.  Verify that the accessor methods
    //:   behave correctly.
    //:
    //: 2 Use the brute-force approach to create and verify objects created
    //:   with different high watermark, low watermark, hash function, and
    //:   equal function.
    //
    // Testing:
    //   void visit(VISITOR& visitor) const;
    //   CacheEvictionPolicy::Enum evictionPolicy() const;
    //   bsl::size_t highWatermark() const;
    //   bsl::size_t lowWatermark() const;
    //   bsl::size_t size() const;
    //   HASH hashFunction() const;
    //   EQUAL equalFunction() const;
    // ------------------------------------------------------------------------


    const TestValues     VALUES; // contains 52 distinct increasing values
    const bsl::size_t    MAX_LENGTH = 9;
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const bdlcc::CacheEvictionPolicy::Enum POLICIES[] = {
        bdlcc::CacheEvictionPolicy::e_LRU,
        bdlcc::CacheEvictionPolicy::e_FIFO
    };

    const int NUM_POLICIES = sizeof(POLICIES) / sizeof(*POLICIES);

    for (int tp = 0; tp < NUM_POLICIES; ++tp) {
        bdlcc::CacheEvictionPolicy::Enum policy = POLICIES[tp];
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj        mX(policy, 90, 100, &scratch);
            const Obj& X = mX;

            ASSERTV(policy == X.evictionPolicy());
            ASSERTV(    90 == X.lowWatermark());
            ASSERTV(   100 == X.highWatermark());

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insert(VALUES[tj].first, VALUES[tj].second);
            }

            TestVisitor visitor(&VALUES, LENGTH);
            X.visit(visitor);
            visitor.assertEnd();
            ASSERTV(LENGTH == X.size());
        }
    }

    {
        Obj        mX(&scratch);
        const Obj& X = mX;
        ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
        ASSERTV(bsl::numeric_limits<bsl::size_t>::max() == X.lowWatermark());
        ASSERTV(bsl::numeric_limits<bsl::size_t>::max() == X.highWatermark());
        ASSERTV(0 == X.hashFunction().id());
        ASSERTV(0 == X.equalFunction().id());
    }
    {
        Obj        mX(bdlcc::CacheEvictionPolicy::e_LRU, 123, 456, &scratch);
        const Obj& X = mX;
        ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
        ASSERTV(123 == X.lowWatermark());
        ASSERTV(456 == X.highWatermark());
        ASSERTV(0   == X.hashFunction().id());
        ASSERTV(0   == X.equalFunction().id());
    }
    {
        Obj mX(bdlcc::CacheEvictionPolicy::e_FIFO, 123, 456,
               TestHashFunctor<KEYTYPE>(300),
               TestEqualityComparator<KEYTYPE>(400),
               &scratch);

        const Obj& X = mX;
        ASSERTV(bdlcc::CacheEvictionPolicy::e_FIFO == X.evictionPolicy());
        ASSERTV(123 == X.lowWatermark());
        ASSERTV(456 == X.highWatermark());
        ASSERTV(300 == X.hashFunction().id());
        ASSERTV(400 == X.equalFunction().id());
    }
}


template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase3()
{
    // ------------------------------------------------------------------------
    // TEST APPARATUS
    //
    // Concerns:
    //: 1 'TestPostEvictionCallback' correctly determines the received values
    //:   are as expected.
    //:
    //: 2 'TestVisitor' correctly determines the visited values are as
    //:   expected.
    //
    // Plan:
    //: 1 Use the brute-force approach to test the test apparatus.  We don't
    //:   need to do an exhaustive test because the test apparatus are further
    //:   corroborated throughout the rest of the test driver.
    //
    // Testing:
    //   TEST APPARATUS
    // ------------------------------------------------------------------------

    const TestValues VALUES;  // contains 52 distinct increasing values

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // Testing 'TestPostEvictionCallback'.
    {
        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 0, 0);
        ASSERTV(callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 0, 0);
        {
            bsl::size_t                idx;
            bool                       isExpected;
            typename Obj::ValuePtrType value;
            InplaceUtil<VALUETYPE>::SpCreateInplace(&value,
                                                   VALUES[0].second, &scratch);
            callback.callOpImp(&idx, &isExpected, value);
            ASSERTV(0 == idx);
            ASSERTV(isExpected);
        }
        ASSERTV(!callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 1, 2);
        ASSERTV(!callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 1, 2);
        ASSERTV(!callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 2, 1);
        {
            bsl::size_t                idx;
            bool                       isExpected;
            typename Obj::ValuePtrType value;
            InplaceUtil<VALUETYPE>::SpCreateInplace(&value,
                                                   VALUES[1].second, &scratch);
            callback.callOpImp(&idx, &isExpected, value);
            ASSERTV(1 == idx);
            ASSERTV(isExpected);
        }
        ASSERTV(!callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        bsl::size_t              indexes[] = { 1, 2 };
        TestPostEvictionCallback callback(&VALUES, &pos, 2, 0, indexes);
        {
            bsl::size_t                idx;
            bool                       isExpected;
            typename Obj::ValuePtrType value;
            InplaceUtil<VALUETYPE>::SpCreateInplace(&value,
                                                   VALUES[1].second, &scratch);
            callback.callOpImp(&idx, &isExpected, value);
            ASSERTV(1 == idx);
            ASSERTV(isExpected);
        }
        {
            bsl::size_t                idx;
            bool                       isExpected;
            typename Obj::ValuePtrType value;
            InplaceUtil<VALUETYPE>::SpCreateInplace(&value, VALUES[2].second,
                                                                     &scratch);
            callback.callOpImp(&idx, &isExpected, value);
            ASSERTV(2 == idx);
            ASSERTV(isExpected);
        }
        ASSERTV(callback.assertEndImp())
    }
    {
        bsl::size_t              pos;
        bsl::size_t              indexes[] = { 1, 2 };
        TestPostEvictionCallback callback(&VALUES, &pos, 2, 0, indexes);
        {
            bsl::size_t                idx;
            bool                       isExpected;
            typename Obj::ValuePtrType value;
            InplaceUtil<VALUETYPE>::SpCreateInplace(&value, VALUES[5].second,
                                                                     &scratch);
            callback.callOpImp(&idx, &isExpected, value);
            ASSERTV(1 == idx);
            ASSERTV(!isExpected);
        }
        ASSERTV(!callback.assertEndImp())
    }

    // Testing 'TestVisitor'
    {
        TestVisitor visitor(&VALUES, 0);
        ASSERTV(visitor.assertEndImp());
    }
    {
        TestVisitor visitor(&VALUES, 0);
        {
            bsl::size_t idx;

            bool isKeyExpected;
            bool isValueExpected;
            bool ret = visitor.callOpImp(&idx, &isKeyExpected,
                          &isValueExpected, VALUES[0].first, VALUES[0].second);
            ASSERTV(0 == idx);
            ASSERTV(isKeyExpected);
            ASSERTV(isValueExpected);
            ASSERTV(ret);
        }
        ASSERTV(!visitor.assertEndImp());
    }
    {
        TestVisitor visitor(&VALUES, 1);
        ASSERTV(!visitor.assertEndImp());
    }
    {
        TestVisitor visitor(&VALUES, 1);
        {
            bsl::size_t idx;

            bool isKeyExpected;
            bool isValueExpected;
            bool ret = visitor.callOpImp(
                                        &idx, &isKeyExpected, &isValueExpected,
                                        VALUES[0].first, VALUES[0].second);
            ASSERTV(0 == idx);
            ASSERTV(isKeyExpected);
            ASSERTV(isValueExpected);
            ASSERTV(ret);
        }
        ASSERTV(visitor.assertEndImp());
    }

    {
        typename TestVisitor::Args args[] = {
            typename TestVisitor::Args(1, true),
            typename TestVisitor::Args(6, true),
            typename TestVisitor::Args(3, false)
        };

        TestVisitor visitor(&VALUES, 3, args);
        {
            bsl::size_t idx;

            bool isKeyExpected;
            bool isValueExpected;
            bool ret = visitor.callOpImp(&idx, &isKeyExpected,
                          &isValueExpected, VALUES[1].first, VALUES[1].second);
            ASSERTV(1 == idx);
            ASSERTV(isKeyExpected);
            ASSERTV(isValueExpected);
            ASSERTV(ret);
        }
        {
            bsl::size_t idx;

            bool isKeyExpected;
            bool isValueExpected;
            bool ret = visitor.callOpImp(&idx, &isKeyExpected,
                          &isValueExpected, VALUES[6].first, VALUES[6].second);
            ASSERTV(6 == idx);
            ASSERTV(isKeyExpected);
            ASSERTV(isValueExpected);
            ASSERTV(ret);
        }
        {
            bsl::size_t idx;

            bool isKeyExpected;
            bool isValueExpected;
            bool ret = visitor.callOpImp(&idx, &isKeyExpected,
                          &isValueExpected, VALUES[3].first, VALUES[3].second);
            ASSERTV(3 == idx);
            ASSERTV(isKeyExpected);
            ASSERTV(isValueExpected);
            ASSERTV(!ret);
        }
        ASSERTV(visitor.assertEndImp());
    }
}


template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase2()
{
    // ------------------------------------------------------------------------
    // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
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
    //:10 The 'insert' method provides exception neutrality guarantee.
    //:
    //:11 Ensure that hash function and allocators are passed in correctly to
    //:   the underlying unordered map's constructor.
    //:
    //:12 QoI: The default constructor allocates 1 block of memory as required
    //:   by the default construction of 'bsl::list'.
    //:
    //:13 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use the loop-based approach to construct three different objects
    //:   differently: without passing an allocator, passing a null allocator
    //:   address explicitly, and passing the address of a test allocator
    //:   explicitly.  For each iteration, insert elements into the constructed
    //:   object and verify that the object has the correct value and memory
    //:   has been allocated properly.
    //:
    //: 2 Use the brute-force approach to test the value constructor by
    //:   verifying that all of the values have been correctly passed to the
    //:   objects used in the implementation.
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values.
    //
    // Testing:
    //   explicit Cache(bslma::Allocator *basicAllocator);
    //   Cache(evictionPolicy, lowWat, highWat, alloc);
    //   Cache(evictionPolicy, lowWat, highWat, hashFunction, equal, alloc);
    //   void insert(const KEYTYPE& key, const VALUETYPE& value);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEYTYPE>::value +
                           bslma::UsesBslmaAllocator<VALUETYPE>::value;

    if (verbose) { P(TYPE_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const bsl::size_t MAX_LENGTH = 9;

    // Testing insert
    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        if (verbose) {
            cout << "\nTesting with various allocator configurations.\n";
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                cout << "\n\tTesting default constructor.\n";
            }

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    1 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0          == X.size());

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                    mX.insert(VALUES[tj].first, VALUES[tj].second);
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

            TestVisitor visitor(&VALUES, LENGTH);
            X.visit(visitor);
            visitor.assertEnd();
            ASSERTV(LENGTH, X.size(), LENGTH == X.size())

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
    // Testing value constructor with 4 arguments
    {
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        {
            Obj        mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 1, &scratch);
            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
            ASSERTV(1 == X.lowWatermark());
            ASSERTV(1 == X.highWatermark());
        }
        {
            Obj        mX(bdlcc::CacheEvictionPolicy::e_LRU, 2, 3, &scratch);
            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
            ASSERTV(2 == X.lowWatermark());
            ASSERTV(3 == X.highWatermark());
        }
        {
            Obj        mX(bdlcc::CacheEvictionPolicy::e_FIFO, 2, 3, &scratch);
            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_FIFO == X.evictionPolicy());
            ASSERTV(2 == X.lowWatermark());
            ASSERTV(3 == X.highWatermark());
        }

        // Negative Testing
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_FIFO,
                                    0, 0, &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    0, 0, &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    1, 0, &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    0, 1, &scratch));
            ASSERT_SAFE_PASS(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    1, 1, &scratch));
        }
    }

    // Testing value constructor with 6 arguments
    {
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        {
            Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 1, 1,
                   TestHashFunctor<KEYTYPE>(100),
                   TestEqualityComparator<KEYTYPE>(200),
                   &scratch);

            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
            ASSERTV(1   == X.lowWatermark());
            ASSERTV(1   == X.highWatermark());
            ASSERTV(100 == X.hashFunction().id());
            ASSERTV(200 == X.equalFunction().id());
        }
        {
            Obj mX(bdlcc::CacheEvictionPolicy::e_LRU, 2, 3,
                   TestHashFunctor<KEYTYPE>(300),
                   TestEqualityComparator<KEYTYPE>(400),
                   &scratch);

            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_LRU == X.evictionPolicy());
            ASSERTV(2   == X.lowWatermark());
            ASSERTV(3   == X.highWatermark());
            ASSERTV(300 == X.hashFunction().id());
            ASSERTV(400 == X.equalFunction().id());
        }
        {
            Obj mX(bdlcc::CacheEvictionPolicy::e_FIFO, 2, 3,
                   TestHashFunctor<KEYTYPE>(500),
                   TestEqualityComparator<KEYTYPE>(600),
                   &scratch);

            const Obj& X = mX;
            ASSERTV(bdlcc::CacheEvictionPolicy::e_FIFO == X.evictionPolicy());
            ASSERTV(2   == X.lowWatermark());
            ASSERTV(3   == X.highWatermark());
            ASSERTV(500 == X.hashFunction().id());
            ASSERTV(600 == X.equalFunction().id());
        }

        // Negative Testing
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_FIFO,
                                    0, 0, TestHashFunctor<KEYTYPE>(),
                                    TestEqualityComparator<KEYTYPE>(),
                                    &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    0, 0, TestHashFunctor<KEYTYPE>(),
                                    TestEqualityComparator<KEYTYPE>(),
                                    &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    1, 0, TestHashFunctor<KEYTYPE>(),
                                    TestEqualityComparator<KEYTYPE>(),
                                    &scratch));
            ASSERT_SAFE_FAIL(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    0, 1, TestHashFunctor<KEYTYPE>(),
                                    TestEqualityComparator<KEYTYPE>(),
                                    &scratch));
            ASSERT_SAFE_PASS(Obj mX(bdlcc::CacheEvictionPolicy::e_LRU,
                                    1, 1, TestHashFunctor<KEYTYPE>(),
                                    TestEqualityComparator<KEYTYPE>(),
                                    &scratch));
        }
    }
}


template <class KEYTYPE, class VALUETYPE, class HASH, class EQUAL>
void TestDriver<KEYTYPE, VALUETYPE, HASH, EQUAL>::testCase1()
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

    bslma::TestAllocator         scratch("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    {
        Obj mX(&scratch);  const Obj& X = mX;

        mX.insert(TstFacility::create<KEYTYPE>(1),
                  TstFacility::create<VALUETYPE>(11));
        mX.insert(TstFacility::create<KEYTYPE>(2),
                  TstFacility::create<VALUETYPE>(22));
        mX.insert(TstFacility::create<KEYTYPE>(3),
                  TstFacility::create<VALUETYPE>(33));
        mX.insert(TstFacility::create<KEYTYPE>(4),
                  TstFacility::create<VALUETYPE>(44));
        mX.insert(TstFacility::create<KEYTYPE>(5),
                  TstFacility::create<VALUETYPE>(55));

        ASSERTV(X.size(), X.size() == 5);

        {
            typename Obj::ValuePtrType val;

            int rc = mX.tryGetValue(&val, TstFacility::create<KEYTYPE>(1));
            ASSERTV(rc, 0 == rc);
            ASSERTV(*val, 11 == TstFacility::getIdentifier(*val));
        };

        {
            typename Obj::ValuePtrType val;

            int rc = mX.tryGetValue(&val, TstFacility::create<KEYTYPE>(3));
            ASSERTV(rc, 0 == rc);
            ASSERTV(*val, 33 == TstFacility::getIdentifier(*val));
        };
    }


    const TestValues VALUES;  // contains 52 distinct increasing values

    // LRU
    {
        Obj        mX(bdlcc::CacheEvictionPolicy::e_LRU, 3, 5, &scratch);
        const Obj& X = mX;

        bsl::size_t              pos;
        TestPostEvictionCallback callback(&VALUES, &pos, 3, 0);

        typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
            PostEvictionCallback
            callbackFunc (bsl::allocator_arg, &scratch, callback);
        mX.setPostEvictionCallback(callbackFunc);

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);
        mX.insert(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insert(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 3);
        callback.assertEnd();
    }

    {
        Obj        mX(bdlcc::CacheEvictionPolicy::e_LRU, 3, 5, &scratch);
        const Obj& X = mX;

        bsl::size_t              pos;
        bsl::size_t              indexes[] = { 0, 2, 3 };
        TestPostEvictionCallback callback(&VALUES, &pos, 3, 0, indexes);

        typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
            PostEvictionCallback
            callbackFunc (bsl::allocator_arg, &scratch, callback);
        mX.setPostEvictionCallback(callbackFunc);

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);

        {
            typename Obj::ValuePtrType val;

            int rc = mX.tryGetValue(&val, VALUES[1].first);
            ASSERTV(rc, 0 == rc);
            ASSERTV(*val, areEqual(VALUES[1].second, *val));
        };

        mX.insert(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insert(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 3);
        callback.assertEnd();
    }

    // FIFO
    {
        Obj        mX(bdlcc::CacheEvictionPolicy::e_FIFO, 3, 5, &scratch);
        const Obj& X = mX;

        bsl::size_t              pos;
        bsl::size_t              indexes[] = { 0, 1, 2 };
        TestPostEvictionCallback callback(&VALUES, &pos, 3, 0, indexes);

        typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
            PostEvictionCallback
            callbackFunc (bsl::allocator_arg, &scratch, callback);
        mX.setPostEvictionCallback(callbackFunc);

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);
        mX.insert(VALUES[4].first, VALUES[4].second);
        ASSERTV(X.size(), X.size() == 5);

        mX.insert(VALUES[5].first, VALUES[5].second);
        ASSERTV(X.size(), X.size() == 3);

        callback.assertEnd();
    }
    {

        Obj        mX(bdlcc::CacheEvictionPolicy::e_FIFO, 3, 5, &scratch);
        const Obj& X = mX;

        bsl::size_t              pos;
        bsl::size_t              indexes[] = { 0, 1, 2 };
        TestPostEvictionCallback callback(&VALUES, &pos, 3, 0, indexes);

        typename bdlcc::Cache<KEYTYPE, VALUETYPE, HASH, EQUAL>::
            PostEvictionCallback
            callbackFunc (bsl::allocator_arg, &scratch, callback);
        mX.setPostEvictionCallback(callbackFunc);

        mX.insert(VALUES[0].first, VALUES[0].second);
        mX.insert(VALUES[1].first, VALUES[1].second);
        mX.insert(VALUES[2].first, VALUES[2].second);
        mX.insert(VALUES[3].first, VALUES[3].second);

        ASSERTV(X.size(), X.size() == 4);
        mX.insert(VALUES[4].first, VALUES[4].second);
        ASSERTV(X.size(), X.size() == 5);

        {
            typename Obj::ValuePtrType val;

            int rc = mX.tryGetValue(&val, VALUES[1].first);
            ASSERTV(rc, 0 == rc);
            ASSERTV(*val, areEqual(VALUES[1].second, *val));
        };

        mX.insert(VALUES[5].first, VALUES[5].second);
        callback.assertEnd();
    }
}


}  // close unnamed namespace

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);
    bslma::TestAllocatorMonitor gam(&globalAllocator);

    // BDE_VERIFY pragma: -TP17 These are defined in the various test functions
    switch (test) { case 0:
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        usageExample1::example1();
        usageExample2::example2();
      } break;
      // BDE_VERIFY pragma: -TP05 Defined in the various test functions
      case 15: {
        testLock::testLocking();
      } break;
      case 14: {
        testLock::testTestingUtil();
      } break;
      case 13: {
        threaded::threadedTest1();
      } break;
      case 12: {
        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES);
      } break;
      case 11: {
        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES);
      } break;
      case 10: {
        RUN_EACH_TYPE(TestDriver, testCase10, TEST_TYPES);
      } break;
      case 9: {
        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES);
      } break;
      case 8: {
        RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES);
      } break;
      case 7: {
        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES);
      } break;
      case 6: {
        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES);
      } break;
      case 5: {
        RUN_EACH_TYPE(TestDriver, testCase5, TEST_TYPES);
      } break;
      case 4: {
        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES);
      } break;
      case 3: {
        RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES);
      } break;
      case 2: {
        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES);
      } break;
      case 1: {
        RUN_EACH_TYPE(TestDriver, testCase1, TEST_TYPES);
      } break;
      // BDE_VERIFY pragma: +TP05
      case -1: {
        // --------------------------------------------------------------------
        // INSERT PERFORMANCE TEST
        //   Tests performance of single inserts into the cache.  To provide
        //   control over the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to insert.
        //   4th parameter: if F, use FIFO for eviction policy; LRU othrwise.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for inserting
        //:   the given rows.
        //
        // Plan:
        //: 1 Create a CachePerformance object, and use testInsert.  High water
        //:   mark is set above eviction. Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   INSERT PERFORMANCE
        // --------------------------------------------------------------------
        bslma::TestAllocator talloc("ptm1", veryVeryVeryVerbose);

        int numThreads = argc > 2 ? atoi(argv[2]) : 1;
        int numCalcs   = argc > 3 ? atoi(argv[3]) : 200000;

        bdlcc::CacheEvictionPolicy::Enum  evictionPolicy =
            (argc > 4 && argv[4][0] == 'F' ?
            bdlcc::CacheEvictionPolicy::e_FIFO :
            bdlcc::CacheEvictionPolicy::e_LRU);

        cacheperf::CachePerformance cp("testInsert1", evictionPolicy,
                1e7, 2e7, 0, numThreads, numCalcs, 10, &talloc);
        // Empty args vector
        cacheperf::CachePerformance::VecIntType  args(&talloc);
        cacheperf::CachePerformance::VecTimeType times(&talloc);
        times = cp.runTests(args, cacheperf::CachePerformance::testInsert);
        cp.printResult();
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // INSERT BULK PERFORMANCE TEST
        //   Tests performance of bulk inserts into the cache.  To provide
        //   control over the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to insert.
        //   4th parameter: if F, use FIFO for eviction policy; LRU othrwise.
        //   5th parameter: number of batches to divide the number of rows
        //   into.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for inserting
        //:   the given rows in bulk.
        //
        // Plan:
        //: 1 Create a CachePerformance object, and use testInsertBulk.  High
        //:   mark is set above eviction. Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   INSERT BULK PERFORMANCE
        // --------------------------------------------------------------------
        bslma::TestAllocator talloc("ptm2", veryVeryVeryVerbose);

        int numThreads = argc > 2 ? atoi(argv[2]) : 1;
        int numCalcs   = argc > 3 ? atoi(argv[3]) : 200000;

        bdlcc::CacheEvictionPolicy::Enum  evictionPolicy =
            (argc > 4 && argv[4][0] == 'F' ?
            bdlcc::CacheEvictionPolicy::e_FIFO :
            bdlcc::CacheEvictionPolicy::e_LRU);

        int numBatches = argc > 5 ? atoi(argv[5]) : 1;

        cacheperf::CachePerformance cp("testInsertBulk1", evictionPolicy,
                1e7, 2e7, 0, numThreads, numCalcs, 10, &talloc);

        cacheperf::CachePerformance::VecIntType args(&talloc);  // args vector
        args.push_back(numBatches);
        cacheperf::CachePerformance::VecTimeType times(&talloc);
        times = cp.runTests(args, cacheperf::CachePerformance::testInsertBulk);
        cp.printResult();
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // READ PERFORMANCE TEST
        //   Tests performance of tryGetValue into the cache.  Note that LRU
        //   eviction policy requires writes to the eviction queue.  To provide
        //   control over the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to read.
        //   4th parameter: if F, use FIFO for eviction policy; LRU othrwise.
        //   5th parameter: sparsity of values loaded.  Sparsity is the
        //   distance between consecutive values inserted, and represents how
        //   likely is a read to find the key given. A value of 1 means
        //   consecutive values inserted during initialization, and 100% chance
        //   of finding the key given.  A value of 2 means 50% chance of
        //   finding the key given.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for reading a
        //:   random generated key from the pre-loaded cache.
        //
        // Plan:
        //: 1 Create a CachePerformance object, and use initRead to pre-load
        //:   it.  High mark is set above eviction.  Then run testRead.  Run 10
        //:   repetitions.
        //:   (C-1)
        //
        // Testing:
        //   READ PERFORMANCE
        // --------------------------------------------------------------------
        bslma::TestAllocator talloc("ptm3", false);
        //bslma::TestAllocator talloc("ptm3", veryVeryVeryVerbose);

        int numThreads = argc > 2 ? atoi(argv[2]) : 1;
        int numCalcs   = argc > 3 ? atoi(argv[3]) : 200000;

        bdlcc::CacheEvictionPolicy::Enum  evictionPolicy =
            (argc > 4 && argv[4][0] == 'F' ?
            bdlcc::CacheEvictionPolicy::e_FIFO :
            bdlcc::CacheEvictionPolicy::e_LRU);

        int sparsity = argc > 5 ? atoi(argv[5]) : 1;

        cacheperf::CachePerformance cp("testRead1", evictionPolicy,
                1e7, 2e7, numThreads, 0, numCalcs, 10, &talloc);

        cacheperf::CachePerformance::VecIntType args(&talloc);  // args vector
        args.push_back(numCalcs);
        args.push_back(sparsity);
        int countIns = cp.initialize(args,
                                        cacheperf::CachePerformance::initRead);
        cout << "Inserted " << countIns << "\n";

        args.clear(); // Now, load args for testRead
        args.push_back(numCalcs * sparsity);
        cacheperf::CachePerformance::VecTimeType times(&talloc);
        times = cp.runTests(args, cacheperf::CachePerformance::testRead);
        cp.printResult();
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // READ/WRITE PERFORMANCE TEST
        //   Tests performance of running both insert and tryGetValue against
        //   the cache.  Note that LRU eviction policy requires writes to the
        //   eviction queue by the "reader" threads.  To provide control over
        //   the test, command line parameters are used.
        //   2nd parameter: number of threads.
        //   3rd parameter: number of rows to read.
        //   4th parameter: number of writer threads.
        //   5th parameter: if F, use FIFO for eviction policy; LRU othrwise.
        //   6th parameter: sparsity of values loaded.  Sparsity is the
        //   distance between consecutive values inserted, and represents how
        //   likely is a read to find the key given. A value of 1 means
        //   consecutive values inserted during initialization, and 100% chance
        //   of finding the key given.  A value of 2 means 50% chance of
        //   finding the key given.
        //
        // Concerns:
        //: 1 Calculates wall time, user time, and system time for a
        //:   combination of reading a random generated key from the pre-loaded
        //:   cache, and inserting random values into the cache.
        //
        // Plan:
        //: 1 Create a CachePerformance object, and use initRead to pre-load
        //:   it.  High mark is set above eviction.  Then run testReadWrite.
        //:   Note that testReadWrite run inserts for the writer threads and
        //:   tryGetValue for the reader threads.  Run 10 repetitions.
        //:   (C-1)
        //
        // Testing:
        //   READ WRITE PERFORMANCE
        // --------------------------------------------------------------------
        bslma::TestAllocator talloc("ptm4", false);
        //bslma::TestAllocator talloc("ptm4", veryVeryVeryVerbose);

        int numThreads  = argc > 2 ? atoi(argv[2]) : 1;
        int numCalcs    = argc > 3 ? atoi(argv[3]) : 200000;
        int numWThreads = argc > 4 ? atoi(argv[4]) : numThreads / 2;

        bdlcc::CacheEvictionPolicy::Enum  evictionPolicy =
            (argc > 5 && argv[5][0] == 'F' ?
            bdlcc::CacheEvictionPolicy::e_FIFO :
            bdlcc::CacheEvictionPolicy::e_LRU);

        int sparsity = argc > 6 ? atoi(argv[6]) : 1;

        cacheperf::CachePerformance cp("testRW1", evictionPolicy,
                1e7, 2e7, numThreads - numWThreads, numWThreads, numCalcs,
                10, &talloc);

        cacheperf::CachePerformance::VecIntType args(&talloc);  // args vector
        args.push_back(numCalcs);
        args.push_back(sparsity);
        int countIns = cp.initialize(args,
                                        cacheperf::CachePerformance::initRead);
        cout << "Inserted " << countIns << "\n";

        args.clear(); // Now, load args for testReadWrite
        args.push_back(numCalcs * sparsity);
        cacheperf::CachePerformance::VecTimeType times(&talloc);
        times = cp.runTests(args, cacheperf::CachePerformance::testReadWrite);
        cp.printResult();
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    // BDE_VERIFY pragma: +TP17

    if (test >= 0) {
        // CONCERN: In no case does memory come from the default allocator.

        ASSERT(dam.isTotalSame());

        // CONCERN: In no case does memory come from the global allocator.

        ASSERT(gam.isTotalSame());
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
