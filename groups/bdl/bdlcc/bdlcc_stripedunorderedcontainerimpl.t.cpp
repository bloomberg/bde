// bdlcc_stripedunorderedcontainerimpl.t.cpp                          -*-C++-*-

#include <bdlcc_stripedunorderedcontainerimpl.h>

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
#include <bslma_usesbslmaallocator.h>

#include <bsltf_allocbitwisemoveabletesttype.h>
#include <bsltf_alloctesttype.h>
#include <bsltf_bitwisecopyabletesttype.h>
#include <bsltf_bitwisemoveabletesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_nonequalcomparabletesttype.h>
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
#include <bsls_timeutil.h>
#include <bsls_types.h>     // 'BloombergLP::bsls::Types::Int64'

#include <bsl_algorithm.h>  // 'count'
#include <bsl_climits.h>    // 'INT_MAX'
#include <bsl_cstdlib.h>    // 'atoi'
#include <bsl_cmath.h>      // 'sqrt'
#include <bsl_cstdio.h>     // 'sprintf'
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_memory.h>     // 'allocate_shared'
#include <bsl_ostream.h>    // 'operator<<'
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
// The component under test provides a fully thread safe container template,
// 'bdlcc::StripedUnorderedContainerImpl', that provides a common
// implementation for 'bdlcc::StripedUnorderedMap' and
// 'bdlcc::StripedUnorderedMultiMap' container templates.  By design these
// containers do not implement equality comparison, an assignment operator,
// copy constructor, or even a 'print' method.  Thus, they are classified as
// *irregular* value-semantic types, even if their 'KEY' and 'VALUE' types are
// are VSTs and, test drivers of other (value-semantic) containers are of
// limited use here.  This test driver will *not* be provide the canonical
// (first) ten test cases typically used for VSTs.  Still, the fundamental
// principles of test case ordering will, of course, be followed.
//
// This fully thread-safe container template should be able to handle any type
// that is acceptable to the standard containers 'bsl::unorderedmap' and
// 'bsl::unorderedmultimap'.  Consequently, our tests be run for the same set
// of types used in our test of those standard templates: primarily the types
// defined in the 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR' macro and
// other types in special cases.
//
// Single-threaded behavior is tested in test cases [1 .. 18].  Multi-threaded
// issues are addressed in test cases [19 .. 21].  Two techniques are used:
//
//: 1 The component defines a component "private" class, a 'friend' of the hash
//:   map, that allows users to explicitly lock and unlock specified stripes.
//:   This helper class is used (after testing) to confirm that each locking
//:   method of the hash map sets a lock of the flavor dictated by the design.
//:
//: 2 Stress testing by multiple threads operating at random intervals,
//:   followed by consistency tests.
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
// [ 2] StripedUnorderedContainerImpl(numInitialBuckets, numStripes, *ba);
// [ 2] ~StripedUnorderedContainerImpl();
//
// MANIPULATORS
// [ 9] void clear();
// [15] void disableRehash();
// [15] void enableRehash();
// [ 7] bsl::size_t eraseAll(const KEY& key);
// [ 7] bsl::size_t eraseFirst(const KEY& key);
// [ 8] bsl::size_t eraseBulkAll(RANDOMIT first, last);
// [ 8] bsl::size_t eraseBulkFirst(RANDOMIT first, last);
// [ 3] void insertAlways(const KEY& key, const VALUE& value);
// [ 3] bsl::size_t insertUnique(const KEY& key, const VALUE& value);
// [18] void insertAlways(const KEY& key, VALUE&& value);
// [18] bsl::size_t insertUnique(const KEY& key, VALUE&& value);
// [10] void insertBulkAlways(RANDOMIT first, last);
// [10] bsl::size_t insertBulkUnique(RANDOMIT first, last);
// [15] void maxLoadFactor(float newMaxLoadFactor);
// [15] void rehash(bsl::size_t numBuckets);
// [12] int setComputedValueAll(const KEY& key, functor);
// [12] int setComputedValueFirst(const KEY& key, functor);
// [11] bsl::size_t setValueAll(const KEY& key, const VALUE& value);
// [11] bsl::size_t setValueFirst(const KEY& key, const VALUE& value);
// [17] bsl::size_t setValue(const KEY& key, VALUE&& value);
// [13] int update(const KEY& key, const VisitorFunction& functor);
// [14] int visit(const VisitorFunction& visitor);
//
// ACCESSORS
// [ 4] bsl::size_t bucketIndex(const KEY& key) const;
// [ 4] bsl::size_t bucketCount() const;
// [ 4] bsl::size_t bucketSize(bsl::size_t index) const;
// [15] bool canRehash() const;
// [ 4] bool empty() const;
// [ 4] EQUAL equalFunction() const;
// [ 5] bsl::size_t getValue(VALUE *value, const KEY& key) const;
// [ 6] bsl::size_t getValue(*valuesVector, const KEY& key) const;
// [ 4] HASH hashFunction() const;
// [15] bool isRehashEnabled() const;
// [15] float loadFactor() const;
// [15] float maxLoadFactor() const;
// [ 4] bsl::size_t numStripes() const;
// [ 4] bsl::size_t size() const;
//
// [ 4] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] TYPE TRAITS
// [19] LOCKING TEST UTIL
// [20] LOCKING
// [21] MULTI-THREADED STRESS TEST

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
// use operator &.  It also includes bsltf::NonEqualComparableTestType to
// verify that the equality comparator is being used.
#define TEST_TYPES_USER_DEFINED                    \
    bsltf::EnumeratedTestType::Enum,                                          \
    bsltf::UnionTestType,                                                     \
    bsltf::SimpleTestType,                                                    \
    bsltf::AllocTestType,                                                     \
    bsltf::BitwiseCopyableTestType,                                           \
    bsltf::BitwiseMoveableTestType,                                           \
    bsltf::AllocBitwiseMoveableTestType,                                      \
    bsltf::MovableTestType,                                                   \
    bsltf::MovableAllocTestType,                                              \
    bsltf::NonEqualComparableTestType

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


// Utilities
namespace {
typedef bsltf::TemplateTestFacility TstFacility;

}  // close unnamed namespace


namespace testLock {

bslma::TestAllocator talloc("tl", veryVeryVeryVerbose);

bslmt::Semaphore smp; // Used to sync worker thread and main thread

typedef bdlcc::StripedUnorderedContainerImpl<int, bsl::string> StripType;
typedef bdlcc::StripedUnorderedContainerImpl_TestUtil<int, bsl::string>
                                                      Strip_TestUtilType;
typedef bsls::Types::Int64                            TimeType;
typedef bsl::pair<int, bsl::string>                   PairType;

typedef struct ThreadData {
    Strip_TestUtilType *d_stripTestUtil_p;
        // Pointer to StripedUnorderedContainerImpl_TestUtil object
    int                 d_sleep;
        // sleep time, microsec
    char                d_lockType;
        // 'R' - Read, 'W' - Write
    int                 d_key;
        // Key to lock

    ThreadData(Strip_TestUtilType *stripTestUtil_p,
               int                 sleep,
               char                lockType,
               int                 key)
        // Create a 'ThreadData' object with the specified 'stripTestUtil_p',
        // 'sleep', 'lockType', and 'key'.
    : d_stripTestUtil_p(stripTestUtil_p)
    , d_sleep(sleep)
    , d_lockType(lockType)
    , d_key(key)
    {}
} ThreadData;

const int k_SLEEP_PERIOD = 100*1000; // Sleep period of 100 millisec.

extern "C" void *workThread(void *arg)
{
    // Work function for the spawned thread: lockRead or LockWrite, sleep,
    // unlock.
    ThreadData *tdp = reinterpret_cast<ThreadData *>(arg);
    if (tdp->d_lockType == 'R') {
        tdp->d_stripTestUtil_p->lockRead(tdp->d_key);
    }
    else { // 'W'
        tdp->d_stripTestUtil_p->lockWrite(tdp->d_key);
    }

    smp.post();

    bslmt::ThreadUtil::microSleep(tdp->d_sleep, 0);

    if (tdp->d_lockType == 'R') {
        tdp->d_stripTestUtil_p->unlockRead(tdp->d_key);
    }
    else { // 'W'
        tdp->d_stripTestUtil_p->unlockWrite(tdp->d_key);
    }
    return 0;
}

void testTestingUtil()
    // Test the locking methods in
    // 'bdlcc::StripedUnorderedContainerImpl_TestUtil'.
{
    // ------------------------------------------------------------------------
    // TEST HELPER CLASS
    //   The 'bdlcc::StripedUnorderedContainerImpl_TestUtil' class is a
    //   'friend' of 'bdlcc:StripedUnorderedContainerImpl' that allows users to
    //   explicitly lock and unlock stripes of a hash map.  This helper class
    //   is tested here and used in later tests.
    //
    // Concerns:
    //: 1 The four methods 'lockRead', 'lockWrite', 'unlockRead', and
    //:   'unLockWrite' each acquire (or release) the intended lock on the
    //:   intended stripe.
    //:
    //: 2 Other threads are blocked (or not) according to the flavor of lock
    //:   (i.e., "read" or "write").
    //:
    //: Plan:
    //: 1 The interactions of these four methods will be explored by two
    //:   threads:' the "main" thread and the "spawned" thread.
    //:
    //:   1 One thread ("spawned") will acquire a lock, sleep for a short
    //:     period, release the lock.
    //:   2 The other thread ("main") will measure the time it takes to acquire
    //:     that same lock.
    //:
    //: 2 Repeat P-1 for both flavors of lock ("read" and "write") invoked by
    //:   the main and spawned thread; four combinations in all.
    //:
    //:   1 The time to acquire a read lock when a reach lock is in place
    //:     should be very small.  Concurrent readers are allowed, so no block.
    //:
    //:   2 In all other combinations, the thread trying to acquire an already
    //:     locked stripe must wait until that lock is released.  The wait time
    //:     should be the same (or somewhat larger) than the sleep time of the
    //:     thread holding the lock.
    //:
    //: 3 Empirical tests will be run manually to determine effective sleep
    //:   times and margins of error to effectively distinguish successful
    //:   locking scenarios from lock failures.
    //
    // Testing:
    //   void bdlcc::StripedUnorderedContainerImpl_TestUtil::lockRead();
    //   void bdlcc::StripedUnorderedContainerImpl_TestUtil::lockWrite();
    //   void bdlcc::StripedUnorderedContainerImpl_TestUtil::unlockRead();
    //   void bdlcc::StripedUnorderedContainerImpl_TestUtil::unlockWrite();
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST HELPER CLASS" << endl
                      << "-----------------" << endl;

    StripType           strip(16, 4, &talloc);
    Strip_TestUtilType  strip_TestUtil(strip);
    bslmt::ThreadUtil::Handle
                        handle;
    void               *result;
    int                 key = 1001;
    ThreadData          tdWrite(&strip_TestUtil, k_SLEEP_PERIOD, 'W', key);

    if (veryVerbose) cout << "LockWrite / LockWrite" << endl;
    {
        // Time the duration how long it took to lock write.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        strip_TestUtil.lockWrite(key);

        TimeType endTime    = bsls::TimeUtil::getTimer();
        int      durationWW = static_cast<int>((endTime - startTime) / 1000);
        strip_TestUtil.unlockWrite(key);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationWW > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockWrite / LockRead" << endl;
    {
        // Time the duration how long it took to lock read
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        strip_TestUtil.lockRead(key);

        TimeType endTime    = bsls::TimeUtil::getTimer();
        int      durationWR = static_cast<int>((endTime - startTime) / 1000);
        strip_TestUtil.unlockRead(key);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationWR > k_SLEEP_PERIOD / 2);
    }

    ThreadData          tdRead(&strip_TestUtil, k_SLEEP_PERIOD, 'R', key);

    if (veryVerbose) cout << "LockRead  / LockWrite" << endl;
    {
        // Time the duration how long it took to lock write.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip_TestUtil.lockWrite(key);

        TimeType endTime    = bsls::TimeUtil::getTimer();
        int      durationRW = static_cast<int>((endTime - startTime) / 1000);
        strip_TestUtil.unlockWrite(key);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationRW > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / LockRead" << endl;
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        // Time the duration how long it took to lock read.
        TimeType startTime = bsls::TimeUtil::getTimer();

        strip_TestUtil.lockRead(key);

        TimeType endTime    = bsls::TimeUtil::getTimer();
        int      durationRR = static_cast<int>((endTime - startTime) / 1000);
        strip_TestUtil.unlockRead(key);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(durationRR < k_SLEEP_PERIOD / 2);
    }
} // END 'testTestingUtil'

struct TestUpdater {
    // Updater to the test 'bdlcc:StripedUnorderedContainerImpl' in
    // 'testLocking'.

    bool operator() (bsl::string* value, int key)
        // 'VisitorFunction' function for testing 'update' using the specified
        // 'value' and 'key'.  Always returns 'true'.
    {
        // Print out key and value
        if (veryVerbose) {
            bsl::cout << "Updating " << key
                      << ", value: "
                      << *value
                      << bsl::endl;
        }

        return true;
    }
};

struct TestComputedValueUpdater {
    // Functor to the test 'bdlcc:StripedUnorderedContainerImpl' in
    // 'testLocking'.

    bool operator() (bsl::string* value, int key)
        // 'VisitorFunction' functor for testing 'setComputedValue' using the
        // specified 'value' and 'key'.
    {
        // Print out key and value
        if (veryVerbose) {
            bsl::cout << "Set computed value " << key
                      << ", value: "
                      << *value
                      << bsl::endl;
        }
        return true;
    }
};

void testLocking()
    // Test locking for the various 'bdlcc:StripedUnorderedContainerImpl'
    //methods.
{
    // ------------------------------------------------------------------------
    // TEST LOCKING OF 'bdlcc:StripedUnorderedContainerImpl' METHODS
    //
    // Concerns:
    //: 1 Each of the following methods of 'StripedUnorderedContainerImpl'
    //:   acquires the expected flavor of lock:
    //..
    //  Method            Lock Type
    //  ----------------  ---------
    //  clear             write
    //  erase             write
    //  eraseBulk         write
    //  getValue (plain)  READ
    //  getValue (vector) READ
    //  insert (movable)  write
    //  insert (plain)    write
    //  insertBulk        write
    //  rehash            write
    //  setComputedValue  write
    //  setValue          write
    //  update            write
    //  visit             write
    //..
    //
    //: 2 The lock is applied for any arbitrary stripe.
    //:
    //: 3 The lock is released by the method.
    //
    // Plan:
    //: 1 For each method, spawn a thread that uses the
    //:   'bdlcc::StripedUnorderedContainerImpl_TestUtil' helper class to
    //:   acquire a lock on an arbitrary stripe of a hash map and then sleep
    //:   for a short period.  The flavor of lock set is chosen so that the
    //:   type of lock acquired by the main thread (see P-2) can be inferred by
    //:   the time it must wait to acquire its lock on that same stripe.  That
    //:   time is approximated by the time needed complete the method.
    //
    //: 2 When signaled, the main thread invokes the method on an element in
    //:   the stripe that was locked by the spawned thread (see P-1).  The
    //:   duration of wait time informs us if a lock of the expected flavor was
    //:   acquired by the method.
    //:
    //:   o If the main thread waits a period comparable to the sleep time of
    //:     the spawned thread, then the main thread was blocked waiting for
    //:     exclusive access to the stripe, i.e., a "write" lock.
    //:
    //: 3 Confirming that a method is acquiring a "read" lock requests two
    //:   stages:
    //:
    //:   1 The spawned thread stages a "read" lock and the main thread
    //:     completes almost immediately.  This is expected because the "read"
    //:     lock allows concurrent access; however, it is possible that the
    //:     method did not apply any lock -- the observed behavior (immediate
    //:     return) would be the same.
    //:
    //:   2 The spawned thread stages an "write" (exclusive) lock.  If the
    //:     method execution approximates spawned thread's sleep time, then we
    //:     know that the method is acquiring a lock.  This fact, combined with
    //:     P-3-1 informs that lock is a "read" lock.
    //:
    //: 4 We know that each method releases its lock because subsequent locking
    //:   tests work as expected.
    //:
    //: 5 We know that locks are acquired once because these locks are not
    //:   recursive and attempting require a lock already held would deadlock.
    //
    // Testing:
    //   TEST LOCKING OF 'bdlcc:StripedUnorderedContainerImpl' METHODS
    // ------------------------------------------------------------------------

    if (verbose) cout
             << endl
             << "TEST LOCKING OF 'bdlcc:StripedUnorderedContainerImpl' METHODS"
             << endl
             << "----------------------------------------------------" << endl;

    StripType           strip(16, 4, &talloc);
    Strip_TestUtilType  strip_TestUtil(strip);
    bslmt::ThreadUtil::Handle
                        handle;
    void               *result;
    int                 key = 1001;
    ThreadData          tdWrite(&strip_TestUtil, k_SLEEP_PERIOD, 'W', key);
    ThreadData          tdRead( &strip_TestUtil, k_SLEEP_PERIOD, 'R', key);

    if (veryVerbose) cout <<
        "LockRead / 'insertUnique(const KEY& key, const VALUE& value)'"
                          << endl;
    {
        // Time the duration how long it took to run 'insert'.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip.insertUnique(key, "John");

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout <<
          "LockRead / 'insertUnique(const KEY& key, VALUE&& value)'"
                          << endl;
    {
        // Time the duration how long it took to run 'insert'.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        bsl::string val("John");
        strip.insertUnique(key, bslmf::MovableRefUtil::move(val));

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'insertBulk'" << endl;
    {
        // Time the duration how long it took to run 'insertBulk'.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        bsl::vector<PairType>    insertData(&talloc);
        insertData.push_back(PairType(key, "V1001"));
        insertData.push_back(PairType(key+1, "V1002"));
        strip.insertBulkUnique(insertData.begin(), insertData.end());

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'erase'" << endl;
    {
        // Time the duration how long it took to run 'erase'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip.eraseFirst(key);

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'eraseBulk'" << endl;
    {
        // Time the duration how long it took to run 'eraseBulk'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        bsl::vector<int> eraseKeys(&talloc);
        eraseKeys.push_back(key);
        eraseKeys.push_back(key+1);
        strip.eraseBulkFirst(eraseKeys.begin(), eraseKeys.end());

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'clear'" << endl;
    {
        // Time the duration how long it took to run 'clear'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip.clear();

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout <<
          "LockRead  / 'setValueFirst(const KEY& key, const VALUE& value)'"
                          << endl;
    {
        // Time the duration how long it took to run 'setValue'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip.setValueFirst(key, "John");

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout <<
             "LockRead  / 'setComputedValueFirst(const KEY& key, visitor)'"
                          << endl;
    {
        // Time the duration how long it took to run 'setValue'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        TestComputedValueUpdater func;
        strip.setComputedValueFirst(key, func);

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'update(const KEY& key, functor)'"
                          << endl;
    {
        // Time the duration how long it took to run 'update'.
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        TestUpdater func;
        strip.update(key, func);

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'visit(functor)'" << endl;
    {
        // Time the duration how long it took to run 'visit'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        TestUpdater func;
        strip.visit(func);

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'rehash(numBuckets)'" << endl;
    {
        // Time the duration how long it took to run 'setValue'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();

        strip.rehash(32);

        TimeType endTime  = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockWrite / 'getValue'" << endl;
    {
        // Time the duration how long it took to run 'getValue'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        bsl::string value;
        bsl::size_t rc = strip.getValue(&value, key);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'getValue'" << endl;
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();
        // Time the duration how long it took to run 'getValue'
        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::string value;
        bsl::size_t rc = strip.getValue(&value, key);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration < k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockWrite / 'getValue' vector" << endl;
    {
        // Time the duration how long it took to run 'getValue'
        TimeType startTime = bsls::TimeUtil::getTimer();
        bslmt::ThreadUtil::create(&handle, workThread, &tdWrite);
        smp.wait();

        bsl::vector<bsl::string> values;
        bsl::size_t              rc = strip.getValue(&values, key);
        (void) rc;

        TimeType endTime = bsls::TimeUtil::getTimer();
        int      duration = static_cast<int>((endTime - startTime) / 1000);
        bslmt::ThreadUtil::join(handle, &result);

        ASSERT(duration > k_SLEEP_PERIOD / 2);
    }

    if (veryVerbose) cout << "LockRead  / 'getValue' vector" << endl;
    {
        bslmt::ThreadUtil::create(&handle, workThread, &tdRead);
        smp.wait();
        // Time the duration how long it took to run 'getValue'
        TimeType startTime = bsls::TimeUtil::getTimer();

        bsl::vector<bsl::string> values;
        bsl::size_t              rc = strip.getValue(&values, key);
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
    typedef bdlcc::StripedUnorderedContainerImpl<int, int> StripType;

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

        if (veryVeryVerbose) {
            const bsl::size_t bucketCount = arg->d_strip_p->bucketCount();
            bsl::stringstream ss;
            ss << "AfterInsert, rc=" << rc << "," << arg->d_workerId << "/"
               << arg->d_numWorkers << ": " << key << ", bucketCount="
               << bucketCount << "\n";
            bsl::cout << ss.str();
        }

        if (1 != rc) {
            arg->d_strip_p->insertUnique(key, 0);
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
            arg->d_strip_p->setValueFirst(key, value + 1);
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

    const int k_NUM_WORKERS =  10;
    const int k_NUM_ITEMS   = 128;

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
    ThreadArg::StripType strip(16, 8, &supplied);
    bsls::AtomicInt      writeCounts[k_NUM_ITEMS];  // default 0
    bsls::AtomicInt      stop(0);

    bslmt::ThreadUtil::Handle queryHandles[k_NUM_WORKERS];
    ThreadArg                 args[k_NUM_WORKERS];

    bsl::size_t initialNumBuckets = strip.bucketCount();

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

    bsl::size_t finalNumBuckets = strip.bucketCount();
    ASSERTV(initialNumBuckets,
            finalNumBuckets,
            initialNumBuckets < finalNumBuckets);
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
    typedef bdlcc::StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL> Obj;

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
    static bool              s_testCase12_found;
    static KEY               s_testCase12_key;
    static int               s_testCase12_valueId;
    static VALUE             s_testCase12_value;
    static bslma::Allocator *s_testCase12_alloc;
        // Places for the test case 12 functor to put a copy of its 'found'
        // input, 'key' input, the identifier of its '*value' input, and to
        // obtain a value to be set to '*value', respectively.  'alloc' - if
        // non-null - is used by the test case 12 functor to allocate and
        // deallocate a single byte, which acts as a hook for testing exception
        // safety using 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST'.

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
    static void testCase12_noncopyable();
    static void testCase13();
    static void testCase14();
    static void testCase15();
    static void testCase16();
    static void testCase17();
    static void testCase18();
        // Code for test cases 1 to 18.
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
    if (areEqual(*value, VALUE())) {
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
        if (areEqual(key, d_values[keyIdx].first)) {
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
    *value               = s_testCase13_value;
    return d_success;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_found;

template <class KEY, class VALUE, class HASH, class EQUAL>
KEY TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_key;

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_valueId;

template <class KEY, class VALUE, class HASH, class EQUAL>
VALUE TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_value;

template <class KEY, class VALUE, class HASH, class EQUAL>
bslma::Allocator *TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase12_alloc;

template <class KEY, class VALUE, class HASH, class EQUAL>
KEY TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_key;

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_valueId;

template <class KEY, class VALUE, class HASH, class EQUAL>
VALUE TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase13_value;

template <class KEY, class VALUE, class HASH, class EQUAL>
int TestDriver<KEY, VALUE, HASH, EQUAL>::s_testCase14_count;

template <class KEY, class VALUE, class HASH, class EQUAL>
bool TestDriver<KEY, VALUE, HASH, EQUAL>::TestCase12Updater::operator()(
                                                             VALUE      *value,
                                                             const KEY&  key)
{
    s_testCase12_found = !areEqual(*value, VALUE());
    s_testCase12_key   = key;
    if (s_testCase12_found) {
        s_testCase12_valueId = TstFacility::getIdentifier(*value);
    }
    else {
        ASSERTV(areEqual(*value, VALUE()));
        s_testCase12_valueId = 0;
    }
    *value = s_testCase12_value;

    if (s_testCase12_alloc) {
        // Allocate and deallocate a single byte.  This operation acts as a
        // hook for testing exception safety using
        // 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST' where 'allocate' would throw a
        // 'bad_alloc'.
        s_testCase12_alloc->deallocate(s_testCase12_alloc->allocate(1));
    }
    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase18()
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
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'insertUnique'
    //:   2 Duplicate keys, 'insertUnique'
    //:   3 Unique keys, 'insertAlways'
    //:   4 Duplicate keys, 'insertAlways'
    //:
    //: 4 Each 'insert' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //  void insertAlways(const KEY& key, VALUE&& value);
    //   bsl::size_t insertUnique(const KEY& key, VALUE&& value);
    //   bsl::size_t insert(const KEY& key, VALUE&& value, multiplicity);
    // ------------------------------------------------------------------------

    // Testing of 'insertUnique' is equivalent to 'testCase17' in
    // 'StripedUnorderedMap', and 'insertAlways' to 'testCase21' in
    // 'StripedUnorderedMultiMap'.

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
        bsl::size_t rc = mX.insertUnique(VALUES[0].first,
                                         MoveUtil::move(val));

        ASSERTV(rc, 1 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        ASSERTV(1 == X.size());

        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }

    // Test updating unique keys, 'insertUnique'.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check existing value
        bsl::size_t rc = mX.insertUnique(VALUES[LENGTH - 1].first,
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
        rc = mX.insertUnique(VALUES[LENGTH + 1].first,
                             MoveUtil::move(val1));
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
            mX.insertUnique(VALUES[LENGTH + 2].first,
                            MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    } // END Test updating unique keys, 'e_INSERT_UNIQUE'

    // Test updating duplicate keys, 'e_INSERT_UNIQUE'.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first,
                            VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first,
                            VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);

        // Check existing value
        bsl::size_t rc = mX.insertUnique(VALUES[LENGTH - 1].first,
                                         MoveUtil::move(val));
        ASSERTV(rc, 0 == rc);

        // Update did not change size
        ASSERTV(2 * LENGTH == X.size());

        // Bucket size did not change
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm that one of the values have changed to the new value.
        bsl::vector<VALUE> values(&scratch);
        rc = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(2 == rc);
        bool val0Eq = areEqual(values[0], VALUES[LENGTH].second);
        bool val1Eq = areEqual(values[1], VALUES[LENGTH].second);
        ASSERTV((val0Eq && !val1Eq) || (!val0Eq && val1Eq));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        sam.reset();

        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        // Check missing value.
        rc = mX.insertUnique(VALUES[LENGTH + 1].first,
                             MoveUtil::move(val1));
        ASSERTV(rc, 1 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1
        ASSERTV(2 * LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&values, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val1(VALUES[LENGTH + 2].second, &scratch);
            mX.insertUnique(VALUES[LENGTH + 2].first,
                            MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    } // END Test updating duplicate keys, 'insertUnique'

    // Test updating unique keys, 'insertAlways'.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check existing value
        mX.insertAlways(VALUES[LENGTH - 1].first, MoveUtil::move(val));
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        // Insert increased size by 1
        ASSERTV(LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm a duplicate key was added, and we now have both the old and
        // the new values.
        bsl::vector<VALUE> values(&scratch);
        bsl::size_t        rc = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(2 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH - 1].second) ||
                areEqual(values[0], VALUES[LENGTH].second));
        ASSERTV(areEqual(values[1], VALUES[LENGTH - 1].second) ||
                areEqual(values[1], VALUES[LENGTH].second));

        // Memory was allocated for the node.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        VALUE value(&scratch);
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        sam.reset();

        // Check missing value
        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        mX.insertAlways(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1
        ASSERTV(LENGTH + 2 == X.size());

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
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&value, VALUES[tj].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[tj].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val1(VALUES[LENGTH + 2].second, &scratch);
            mX.insertAlways(VALUES[LENGTH + 2].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    } // END Test updating unique keys, 'e_INSERT_ALWAYS'

    // Test updating duplicate keys, 'e_INSERT_ALWAYS'.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first,
                            VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        // Check existing value
        mX.insertAlways(VALUES[LENGTH - 1].first, MoveUtil::move(val));
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());

        // Insert increased size by 1.
        ASSERTV(2 * LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm that we now have 3 values, and one of them is the value that
        // set set.
        bsl::vector<VALUE> values(&scratch);
        bsl::size_t        rc = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(3 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH].second) ||
                areEqual(values[1], VALUES[LENGTH].second) ||
                areEqual(values[2], VALUES[LENGTH].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        sam.reset();

        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        // Check missing value.
        mX.insertAlways(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1.
        ASSERTV(2 * LENGTH + 2 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {

            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }
        // Confirm the new element key and value.
        rc = X.getValue(&values, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

        // Memory was allocated.
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        // Exception testing - check missing and existing values.
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val(VALUES[LENGTH].second, &scratch);
            mX.insertAlways(VALUES[LENGTH - 1].first, MoveUtil::move(val));

            VALUE val1(VALUES[LENGTH + 1].second, &scratch);
            mX.insertAlways(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    } // END Test updating duplicate keys, 'insertAlways'

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase17()
{
    // ------------------------------------------------------------------------
    // TEST MOVABLE 'setValue'
    //   Note that movable 'setValue' testing is equivalent to 'setValueFirst'.
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
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys
    //:   2 Duplicate keys
    //:
    //: 4 Each 'setValue' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //   bsl::size_t setValueFirst(const KEY& key, VALUE&& value);
    // ------------------------------------------------------------------------

    // Test is equivalent to 'testCase16' in 'StripedUnorderedMap' and to
    // 'testCase20' in 'StripedUnorderedMultiMap'.

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
        bsl::size_t rc = mX.setValueFirst(VALUES[0].first,
                                          MoveUtil::move(val));
        ASSERTV(bsltf::MoveState::e_MOVED == val.movedFrom());
        ASSERTV(rc, 0 == rc);
        ASSERTV(1 == X.size());

        VALUE value(&scratch);
        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }
    ASSERT(dam.isTotalSame());

    // Test updating unique keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);
        ASSERTV(bsltf::MoveState::e_NOT_MOVED == val.movedFrom());

        // Check existing value.
        bsl::size_t rc = mX.setValueFirst(VALUES[LENGTH - 1].first,
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
        rc = mX.setValueFirst(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
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
            mX.setValueFirst(VALUES[LENGTH + 2].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    }
    ASSERT(dam.isTotalSame());

    // Test updating duplicate keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first, VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        // Check existing value
        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val(VALUES[LENGTH].second, &scratch);

        bsl::size_t rc = mX.setValueFirst(VALUES[LENGTH - 1].first,
                                          MoveUtil::move(val));
        ASSERTV(rc, 1 == rc);

        // Update did not change size.
        ASSERTV(2 * LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm that one of the values have changed to the new value
        bsl::vector<VALUE> values(&scratch);
        rc = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(2 == rc);
        bool val0Eq = areEqual(values[0], VALUES[LENGTH].second);
        bool val1Eq = areEqual(values[1], VALUES[LENGTH].second);
        ASSERTV((val0Eq && !val1Eq) || (!val0Eq && val1Eq));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        sam.reset();

        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        VALUE val1(VALUES[LENGTH + 1].second, &scratch);

        // Check missing value
        rc = mX.setValueFirst(VALUES[LENGTH + 1].first, MoveUtil::move(val1));
        ASSERTV(rc, 0 == rc);
        ASSERTV(bsltf::MoveState::e_MOVED == val1.movedFrom());

        // Insert increased size by 1
        ASSERTV(2 * LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }
        // Confirm the new element key and value.
        rc = X.getValue(&values, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

        // Memory was allocated
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            VALUE val1(VALUES[LENGTH + 2].second, &scratch);
            mX.setValueFirst(VALUES[LENGTH + 2].first, MoveUtil::move(val1));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;

    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase16()
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

    // Test is equivalent to 'testCase15' in 'StripedUnorderedMap' and to
    // 'testCase19' in 'StripedUnorderedMultiMap'.

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
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase15()
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
    //   bool canRehash() const;
    //   float maxLoadFactor() const;
    //   float loadFactor() const;
    // ------------------------------------------------------------------------

    // Test is equivalent to 'testCase14' in 'StripedUnorderedMap' and to
    // 'testCase18' in 'StripedUnorderedMultiMap'.

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
                    bsl::size_t rc = mX.insertUnique(VALUES[tj].first,
                                                     VALUES[tj].second);
                    ASSERTV(1 == rc)
                  } break;
                  case 1: { // insert move
                    bsl::size_t rc = mX.insertUnique(
                               VALUES[tj].first,
                               bslmf::MovableRefUtil::move(VALUES[tj].second));
                    ASSERTV(1 == rc)
                  } break;
                  case 2: { // setValue
                    bsl::size_t rc = mX.setValueFirst(VALUES[tj].first,
                                                      VALUES[tj].second);
                    ASSERTV(0 == rc)
                  } break;
                  case 3: { // setValue move
                    bsl::size_t rc = mX.setValueFirst(
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
                    bsl::size_t rc = mX.insertBulkUnique(insertVec.begin(),
                                                         insertVec.end());
                    ASSERTV(1 == rc)
                  } break;
                  case 5: { // setComputedValue
                    TestCase15Updater testCase15Updater(VALUES[tj].second);

                    typename Obj::VisitorFunction func(testCase15Updater);
                    dam.reset(); // 'bsl::function' constructor allocates from
                                 // the default allocator.

                    int rc = mX.setComputedValueFirst(VALUES[tj].first, func);
                    ASSERTV(0 == rc)
                  } break;
                } // END 'switch'

                // 'insertBulk' (i.e., 'i == 4') intentionally uses the default
                // allocator.
                //
                // 'setComputedValue' (i.e., 'i == 5) copies values and some
                // may use the default allocator.

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
    ASSERT(dam.isTotalSame());

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
        ASSERTV(false == X.canRehash());

        // Insert values.
        for (bsl::size_t tj = 0; tj < LENG; ++tj) {
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENG, X.size(), LENG == X.size())

        // Confirm that no rehash happened.
        ASSERTV(LENG,
                X.bucketCount(),
                initialNumBuckets == X.bucketCount());

        // Enable rehash, confirm no rehash happened.
        sam.reset();
        mX.enableRehash();
        ASSERTV(true == X.canRehash());
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
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase14()
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
    //:
    //: 3 For hash maps of varying lengths with duplicate keys, we:
    //:   1 Run a visitor that always succeeds.
    //:   2 Update both values of a key with values of the next key.
    //:   3 Confirm that the return value is correct.
    //:   4 Confirm that the functor was called the correct number of times.
    //:   5 The size of the container does not change.
    //:   6 The sizes of all buckets do not change.
    //:   7 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   8 Confirm that the values associated with the key were updated to
    //:     the new values.
    //
    // Testing:
    //   int visit(const VisitorFunction& visitor);
    // ------------------------------------------------------------------------

    // Test is equivalent to 'testCase13' in 'StripedUnorderedMap' and to
    // 'testCase17' in 'StripedUnorderedMultiMap'.

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
        dam.reset(); // 'bsl::function' constructor allocates from the default
                     // allocator.

        int rc = mX.visit(visitor);
        ASSERTV(rc, 0 == rc);
        ASSERTV(0 == X.size());
    }
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // Testing with unique values - success
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;
        const int         intLength = static_cast<int>(LENGTH);

        Obj mX(128, 8, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bsl::size_t              bucketCount = X.bucketCount();
        bsl::vector<bsl::size_t> bucketSizes(bucketCount, &scratch);

        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            bucketSizes[i] = X.bucketSize(i);
        }

        TestCase14Updater testCase14UpdaterSuccess(INT_MAX, VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' constructor allocates from the default
                     // allocator.

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
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bsl::size_t              bucketCount = X.bucketCount();
        bsl::vector<bsl::size_t> bucketSizes(bucketCount, &scratch);

        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            bucketSizes[i] = X.bucketSize(i);
        }

        TestCase14Updater
                testCase14UpdaterSuccess(static_cast<int>(LENGTH / 2), VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' constructor allocates from the default
                     // allocator.

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

    // Testing with duplicate values - success
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;
        const int         intLength = static_cast<int>(LENGTH);

        Obj mX(128, 8, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first,
                            VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bsl::size_t              bucketCount = X.bucketCount();
        bsl::vector<bsl::size_t> bucketSizes(bucketCount, &supplied);

        for (bsl::size_t i = 0; i < bucketCount; ++i) {
            bucketSizes[i] = X.bucketSize(i);
        }

        TestCase14Updater testCase14UpdaterSuccess(INT_MAX, VALUES);

        typename Obj::VisitorFunction visitor(testCase14UpdaterSuccess);
        dam.reset(); // 'bsl::function' constructor allocates from the default
                     // allocator.

        bslma::TestAllocatorMonitor sam(&supplied);

        int rc = mX.visit(visitor);

        // Confirm return value, number of functor calls
        ASSERTV(rc,                     intLength * 2 == rc);
        ASSERTV(rc, s_testCase14_count, intLength * 2 == s_testCase14_count);

        // Size unchanged.
        ASSERTV(LENGTH * 2 == X.size());

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
        bsl::vector<VALUE> values(&scratch);
        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            bsl::size_t rc1 = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc1);
            ASSERTV(areEqual(values[0], VALUES[tj+1].second));
            ASSERTV(areEqual(values[1], VALUES[tj+1].second));
        }

    } // END Testing with duplicate values - success

    // CONCERN: In no case does memory come from the default allocator.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase13()
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
    //:
    //: 3 For hash maps of varying lengths with duplicate keys, we:
    //:   1 Update one existing key with a functor that return always 'true'.
    //:   2 Confirm that the return value is correct.
    //:   3 The size of the container does not change.
    //:   4 The size of the bucket does not change.
    //:   5 Memory in use does not change if 'VALUE' type is non-allocating.
    //:   6 Confirm that the values associated with the key were updated to
    //:     the new value.
    //:   7 Confirm that no other elements have changed.
    //:   8 Update one existing key with a functor that return always 'false'.
    //:   9 Confirm that the return value is correct.
    //:  10 The size of the container does not change.
    //:  11 The size of the bucket does not change.
    //:  12 Memory in use does not change if 'VALUE' type is non-allocating.
    //:  13 Confirm that one of the values associated with the key was updated
    //:     to the new value.
    //:  14 Confirm that no other elements have changed.
    //
    // Testing:
    //   int update(const KEY& key, const VisitorFunction& functor);
    // ------------------------------------------------------------------------

    // Test is equivalent to 'testCase12' in 'StripedUnorderedMap' and to
    // 'testCase16' in 'StripedUnorderedMultiMap'.

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
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
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
                areEqual(VALUES[LENGTH - 1].first, s_testCase13_key));
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
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // Test updating duplicate keys.
    for (bsl::size_t ti = 2; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first,
                            VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        // Existing value - update with successful functor
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        s_testCase13_value = VALUES[LENGTH].second;

        int rc = mX.update(VALUES[LENGTH - 1].first, testCase13UpdaterSuccess);
        ASSERTV(rc, 2 == rc);
        ASSERTV(VALUES[LENGTH - 1].first, s_testCase13_key,
                areEqual(VALUES[LENGTH - 1].first, s_testCase13_key));

        // Update did not change size.
        ASSERTV(2 * LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the values have changed to the new value.
        bsl::vector<VALUE> values(&scratch);

        bsl::size_t rc1 = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(2 == rc1);
        ASSERTV(areEqual(values[0], VALUES[LENGTH].second));
        ASSERTV(areEqual(values[1], VALUES[LENGTH].second));

        // No change in memory allocation if VALUE is non-allocating
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc1 = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc1);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        sam.reset();

        // Existing value - update with failed functor
        bucketIdx     = X.bucketIndex(VALUES[LENGTH - 2].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        s_testCase13_value = VALUES[LENGTH].second;

        rc = mX.update(VALUES[LENGTH - 2].first, testCase13UpdaterFail);
        ASSERTV(rc, -1 == rc);
        ASSERTV(VALUES[LENGTH - 2].first, s_testCase13_key,
                areEqual(VALUES[LENGTH - 2].first, s_testCase13_key));

        // Update did not change size.
        ASSERTV(2 * LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm that one of the values have changed to the new value.
        rc1 = X.getValue(&values, VALUES[LENGTH - 2].first);
        ASSERTV(2 == rc1);
        ASSERTV(areEqual(values[0], VALUES[LENGTH].second) ||
                areEqual(values[0], VALUES[LENGTH - 2].second) ||
                areEqual(values[0], VALUES[1].second));
        ASSERTV(areEqual(values[1], VALUES[LENGTH].second) ||
                areEqual(values[1], VALUES[LENGTH - 2].second) ||
                areEqual(values[1], VALUES[1].second));

        // No change in memory allocation if 'VALUE' is non-allocating.
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }
        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 2; ++tj) {
            rc1 = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc1);
            ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                    areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                    areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }
    } // END Test updating duplicate keys.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // Test coverage with a small number of buckets.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        // Check existing value
        int rc = mX.update(VALUES[LENGTH - 1].first,
                                        testCase13UpdaterSuccess);
        ASSERTV(rc, 1 == rc);
        ASSERTV(VALUES[LENGTH - 1].first, s_testCase13_key,
                areEqual(VALUES[LENGTH - 1].first, s_testCase13_key));
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
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase12()
{
    // ------------------------------------------------------------------------
    // TEST 'setComputedValue'
    //
    // Concerns:
    //: 1 When 'setComputedValue' adds an element, the element has the expected
    //:   key and value, and returns 0 for the number of updated elements.
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
    //:
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'setComputedValueFirst'
    //:   2 Unique keys, 'setComputedValueAll'
    //:   3 Duplicate keys, 'setComputedValueAll'
    //:   4 Duplicate keys, 'setComputedValueFirst'
    //
    // Testing:
    //   int setComputedValueAll(const KEY& key, functor);
    //   int setComputedValueFirst(const KEY& key, functor);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST 'setComputedValue'" << endl
                      << "=======================" << endl;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // This is equivalent to 'testCase11' in 'StripedUnorderedMap' and
    // 'testCase14' in 'StripedUnorderedMultiMap.
    {
        if (veryVerbose) cout << endl
                          << "Test 'setComputedValueFirst'" << endl;

        if (veryVeryVerbose) {
            cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
                 << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
                 << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
                 << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
        }

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 15;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        TestCase12Updater    testCase12Updater;

        // Test empty hash map.
        {
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            // Check missing value for 'setComputedValueFirst'.
            int rc1 = mX.setComputedValueFirst(VALUES[0].first,
                                               testCase12Updater);
            ASSERTV(rc1, 0 == rc1);
            ASSERTV(1 == X.size());

            bsl::size_t rc = X.getValue(&value, VALUES[0].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUE()));

            // Check missing value for 'setComputedValueAll'.
            rc1 = mX.setComputedValueAll(VALUES[1].first,
                                         testCase12Updater);
            ASSERTV(rc1, 0 == rc1);
            ASSERTV(2 == X.size());

            rc = X.getValue(&value, VALUES[1].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUE()));
        }
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERT(dam.isTotalSame());
        }

        // Testing 'e_FIRST' with unique values.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
            }
            ASSERTV(LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount   = X.bucketCount();
            bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH-1].first);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH].second;

            // Check existing value
            int rc1 = mX.setComputedValueFirst(VALUES[LENGTH - 1].first,
                                               testCase12Updater);
            ASSERTV(rc1, 1 == rc1);
            ASSERTV(true                      == s_testCase12_found);
            ASSERTV(VALUES[LENGTH - 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH - 1].first, s_testCase12_key));
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
            rc1 = mX.setComputedValueFirst(VALUES[LENGTH + 1].first,
                                           testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false                     == s_testCase12_found);
            ASSERTV(VALUES[LENGTH + 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH + 1].first, s_testCase12_key));

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
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {

                s_testCase12_value = VALUES[LENGTH + 2].second;

                mX.setComputedValueFirst(VALUES[LENGTH + 2].first,
                                         testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Testing 'setComputedValueFirst' with unique values
    } // END 'setComputedValueFirst'
    // No change in default memory allocation if 'VALUE' is non-allocating.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // This is equivalent to 'testCase15' in 'StripedUnorderedMultiMap'.
    {
        if (veryVerbose) cout << endl
                          << "Test 'setComputedValueAll'" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 9;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        TestCase12Updater    testCase12Updater;

        // Test updating unique keys.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj  mX(128, 8, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
            }
            ASSERTV(LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount   = X.bucketCount();
            bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH-1].first);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH].second;

            // Check existing value.
            int rc1 = mX.setComputedValueAll(VALUES[LENGTH - 1].first,
                                             testCase12Updater);
            ASSERTV(rc1, 1 == rc1);
            ASSERTV(true                      == s_testCase12_found);
            ASSERTV(VALUES[LENGTH - 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH - 1].first, s_testCase12_key));

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

            bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
            oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH + 1].second;

            // Check missing value.
            rc1 = mX.setComputedValueAll(VALUES[LENGTH + 1].first,
                                         testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false                     == s_testCase12_found);
            ASSERTV(VALUES[LENGTH + 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH + 1].first, s_testCase12_key));

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
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {

                s_testCase12_value = VALUES[LENGTH + 2].second;

                mX.setComputedValueAll(VALUES[LENGTH + 2].first,
                                       testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Testing 'e_ALL' with unique values
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERT(dam.isTotalSame());
        }

        // Test updating duplicate keys.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
                mX.insertAlways(VALUES[tj].first,
                                VALUES[LENGTH - tj - 1].second);
            }
            ASSERTV(2 * LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount   = X.bucketCount();
            bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH-1].first);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH].second;

            // Check existing value
            int rc1 = mX.setComputedValueAll(VALUES[LENGTH - 1].first,
                                             testCase12Updater);
            ASSERTV(rc1, 2 == rc1);
            ASSERTV(true                      == s_testCase12_found);
            ASSERTV(VALUES[LENGTH - 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH - 1].first, s_testCase12_key));

            // Update did not change size.
            ASSERTV(2 * LENGTH == X.size());

            // Bucket size did not change.
            ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

            // Confirm the values have changed to the new value.
            bsl::vector<VALUE> values(&scratch);

            bsl::size_t rc = X.getValue(&values, VALUES[LENGTH - 1].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[LENGTH].second));
            ASSERTV(areEqual(values[1], VALUES[LENGTH].second));

            // No change in memory allocation if 'VALUE' is non-allocating.
            if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                ASSERTV(sam.isInUseSame());
            }

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&values, VALUES[tj].first);
                ASSERTV(2 == rc);
                ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                        areEqual(values[0], VALUES[LENGTH - tj - 1].second));
                ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                        areEqual(values[1], VALUES[LENGTH - tj - 1].second));
            }

            sam.reset();

            bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
            oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH + 1].second;

            // Check missing value.
            rc1 = mX.setComputedValueAll(VALUES[LENGTH + 1].first,
                                         testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false                     == s_testCase12_found);
            ASSERTV(VALUES[LENGTH + 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH + 1].first, s_testCase12_key));

            // Insert increased size by 1
            ASSERTV(2 * LENGTH + 1 == X.size());

            // Bucket size increased by 1 if number of buckets did not change
            if (bucketCount == X.bucketCount()) {
                ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
            }

            // Confirm the new element key and value.
            rc = X.getValue(&values, VALUES[LENGTH + 1].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

            // Memory was allocated.
            ASSERTV(sam.isInUseUp());
            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&values, VALUES[tj].first);
                ASSERTV(2 == rc);
                ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                        areEqual(values[0], VALUES[LENGTH - tj - 1].second));
                ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                        areEqual(values[1], VALUES[LENGTH - tj - 1].second));
            }

            // Exception testing - check missing value
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
                s_testCase12_value = VALUES[LENGTH + 2].second;
                mX.setComputedValueAll(VALUES[LENGTH + 2].first,
                                       testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Test updating duplicate keys.
    } // END 'setComputedValueAll'
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // This can happen in "multimap", as we test duplicate keys ("multimap")
    // with 'setComputedValueFirst'.
    {
        if (veryVerbose) cout << endl
                          << "Test 'setComputedValueFirst'" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 9;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        TestCase12Updater    testCase12Updater;

        // Test updating duplicate keys.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
                mX.insertAlways(VALUES[tj].first,
                                VALUES[LENGTH - tj - 1].second);
            }
            ASSERTV(2 * LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount   = X.bucketCount();
            bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH-1].first);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH].second;

            // Check existing value.
            int rc1 = mX.setComputedValueFirst(VALUES[LENGTH - 1].first,
                                               testCase12Updater);
            ASSERTV(rc1, 1 == rc1);

            ASSERTV(true                      == s_testCase12_found);
            ASSERTV(VALUES[LENGTH - 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH - 1].first, s_testCase12_key));

            // Update did not change size.
            ASSERTV(2 * LENGTH == X.size());

            // Bucket size did not change.
            ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

            // Confirm that one of the values have changed to the new value.
            bsl::vector<VALUE> values(&scratch);

            bsl::size_t rc = X.getValue(&values, VALUES[LENGTH - 1].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[LENGTH].second) ||
                    areEqual(values[1], VALUES[LENGTH].second));

            // No change in memory allocation if 'VALUE' is non-allocating.
            if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                ASSERTV(sam.isInUseSame());
            }

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&values, VALUES[tj].first);
                ASSERTV(2 == rc);
                ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                        areEqual(values[0], VALUES[LENGTH - tj - 1].second));
                ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                        areEqual(values[1], VALUES[LENGTH - tj - 1].second));
            }

            sam.reset();

            bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
            oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH + 1].second;

            // Check missing value.
            rc1 = mX.setComputedValueFirst(VALUES[LENGTH + 1].first,
                                           testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false                     == s_testCase12_found);
            ASSERTV(VALUES[LENGTH + 1].first, s_testCase12_key,
                    areEqual(VALUES[LENGTH + 1].first, s_testCase12_key));

            // Insert increased size by 1
            ASSERTV(2 * LENGTH + 1 == X.size());

            // Bucket size increased by 1 if no rehash.
            if (bucketCount == X.bucketCount()) {
                ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
            }

            // Confirm the new element key and value.
            rc = X.getValue(&values, VALUES[LENGTH + 1].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

            // Memory was allocated.
            ASSERTV(sam.isInUseUp());

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&values, VALUES[tj].first);
                ASSERTV(2 == rc);
                ASSERTV(areEqual(values[0], VALUES[tj].second) ||
                        areEqual(values[0], VALUES[LENGTH - tj - 1].second));
                ASSERTV(areEqual(values[1], VALUES[tj].second) ||
                        areEqual(values[1], VALUES[LENGTH - tj - 1].second));
            }

            // Exception testing - check missing value
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
                s_testCase12_value = VALUES[LENGTH + 2].second;
                mX.setComputedValueFirst(VALUES[LENGTH + 2].first,
                                         testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Test updating duplicate keys.
    } // END 'setComputedValueFirst' with duplicate keys

    // CONCERN: In no case does memory come from the default allocator.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase12_noncopyable()
{
    // ------------------------------------------------------------------------
    // TEST 'setComputedValue' for noncopyable values
    //
    // Concerns:
    //: 1 When 'setComputedValue' adds an element, the element has the expected
    //:   key and value, and returns 0 for the number of updated elements.
    //:
    //: 2 When 'setComputedValue' updates an element, all elements with that
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
    //:
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'setComputedValueFirst'
    //:   2 Unique keys, 'setComputedValueAll'
    //
    // Testing:
    //   int setComputedValueAll(const KEY& key, functor);
    //   int setComputedValueFirst(const KEY& key, functor);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
        << "TEST 'setComputedValue'" << endl
        << "=======================" << endl;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bsl::vector<VALUE>   valueVec(1, &scratch);
    VALUE&               value = valueVec[0];

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // This is equivalent to 'testCase11' in 'StripedUnorderedMap' and
    // 'testCase14' in 'StripedUnorderedMultiMap.
    {
        if (veryVerbose) cout << endl
            << "Test 'setComputedValueFirst'" << endl;

        if (veryVeryVerbose) {
            cout << "KEY  " << ": " << bsls::NameOf<KEY>() << "\n"
                << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
                << "HASH " << ": " << bsls::NameOf<HASH>() << "\n"
                << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
        }

        // contains 52 distinct increasing values
        bsltf::TestValuesArray<KEY>   KEYS;
        bsltf::TestValuesArray<VALUE> VALUES;

        const bsl::size_t    MAX_LENGTH = 15;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        TestCase12Updater    testCase12Updater;

        // Test empty hash map.
        {
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            // Check missing value for 'setComputedValueFirst'.
            int rc1 = mX.setComputedValueFirst(KEYS[0],
                testCase12Updater);
            ASSERTV(rc1, 0 == rc1);
            ASSERTV(1 == X.size());

            bsl::size_t rc = X.getValue(&value, KEYS[0]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUE()));

            // Check missing value for 'setComputedValueAll'.
            rc1 = mX.setComputedValueAll(KEYS[1],
                testCase12Updater);
            ASSERTV(rc1, 0 == rc1);
            ASSERTV(2 == X.size());

            rc = X.getValue(&value, KEYS[1]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUE()));
        }
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERT(dam.isTotalSame());
        }

        // Testing 'e_FIRST' with unique values.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                // Do not use 'insert' for non-copyable value types
                s_testCase12_value = VALUES[tj];
                mX.setComputedValueFirst(KEYS[tj], testCase12Updater);
            }
            ASSERTV(LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount = X.bucketCount();
            bsl::size_t bucketIdx = X.bucketIndex(KEYS[LENGTH - 1]);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH];

            // Check existing value
            int rc1 = mX.setComputedValueFirst(KEYS[LENGTH - 1],
                testCase12Updater);
            ASSERTV(rc1, 1 == rc1);
            ASSERTV(true == s_testCase12_found);
            ASSERTV(KEYS[LENGTH - 1], s_testCase12_key,
                areEqual(KEYS[LENGTH - 1], s_testCase12_key));
            ASSERTV(TstFacility::getIdentifier(VALUES[LENGTH - 1]),
                s_testCase12_valueId,
                TstFacility::getIdentifier(VALUES[LENGTH - 1])
                == s_testCase12_valueId);

            // Update did not change size.
            ASSERTV(LENGTH == X.size());

            // Bucket size did not change.
            ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

            // Confirm the value has changed to the new value.
            bsl::size_t rc = X.getValue(&value, KEYS[LENGTH - 1]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[LENGTH]));

            // No change in memory allocation if VALUE is non-allocating.
            if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                ASSERTV(sam.isInUseSame());
            }
            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&value, KEYS[tj]);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value, VALUES[tj]));
            }

            sam.reset();

            bucketIdx = X.bucketIndex(KEYS[LENGTH + 1]);
            oldBucketSize = X.bucketSize(bucketIdx);
            s_testCase12_value = VALUES[LENGTH + 1];

            // Check missing value.
            rc1 = mX.setComputedValueFirst(KEYS[LENGTH + 1],
                testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false == s_testCase12_found);
            ASSERTV(KEYS[LENGTH + 1], s_testCase12_key,
                areEqual(KEYS[LENGTH + 1], s_testCase12_key));

            // Insert increased size by 1.
            ASSERTV(LENGTH + 1 == X.size());

            // Bucket size increased by 1 if no rehash.
            if (bucketCount == X.bucketCount()) {
                ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
            }

            // Confirm the new element key and value.
            rc = X.getValue(&value, KEYS[LENGTH + 1]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[LENGTH + 1]));

            // Memory was allocated.
            ASSERTV(sam.isInUseUp());

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                rc = X.getValue(&value, KEYS[tj]);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value,
                    tj == LENGTH - 1 ? VALUES[LENGTH] :
                    VALUES[tj]));
            }

            // Exception testing - check missing value
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {

                s_testCase12_value = VALUES[LENGTH + 2];

                mX.setComputedValueFirst(KEYS[LENGTH + 2],
                    testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Testing 'setComputedValueFirst' with unique values
    } // END 'setComputedValueFirst'
    // No change in default memory allocation if 'VALUE' is non-allocating.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }

    // This is equivalent to 'testCase15' in 'StripedUnorderedMultiMap'.
    {
        if (veryVerbose) cout << endl
            << "Test 'setComputedValueAll'" << endl;

        // contains 52 distinct increasing values
        bsltf::TestValuesArray<KEY>   KEYS;
        bsltf::TestValuesArray<VALUE> VALUES;

        const bsl::size_t    MAX_LENGTH = 9;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);
        TestCase12Updater    testCase12Updater;

        // Test updating unique keys.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            Obj  mX(128, 8, &supplied);  const Obj& X = mX;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                // Do not use 'insert' for non-copyable value types
                s_testCase12_value = VALUES[tj];
                mX.setComputedValueFirst(KEYS[tj], testCase12Updater);
            }
            ASSERTV(LENGTH == X.size());

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t bucketCount = X.bucketCount();
            bsl::size_t bucketIdx = X.bucketIndex(KEYS[LENGTH - 1]);
            bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH];

            // Check existing value.
            int rc1 = mX.setComputedValueAll(KEYS[LENGTH - 1],
                testCase12Updater);
            ASSERTV(rc1, 1 == rc1);
            ASSERTV(true == s_testCase12_found);
            ASSERTV(KEYS[LENGTH - 1], s_testCase12_key,
                areEqual(KEYS[LENGTH - 1], s_testCase12_key));

            ASSERTV(TstFacility::getIdentifier(VALUES[LENGTH - 1]),
                s_testCase12_valueId,
                TstFacility::getIdentifier(VALUES[LENGTH - 1])
                == s_testCase12_valueId);
            // Update did not change size.
            ASSERTV(LENGTH == X.size());

            // Bucket size did not change.
            ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

            // Confirm the value has changed to the new value.
            bsl::size_t rc = X.getValue(&value, KEYS[LENGTH - 1]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[LENGTH]));

            // No change in memory allocation if VALUE is non-allocating.
            if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                ASSERTV(sam.isInUseSame());
            }

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
                rc = X.getValue(&value, KEYS[tj]);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value, VALUES[tj]));
            }

            sam.reset();

            bucketIdx = X.bucketIndex(KEYS[LENGTH + 1]);
            oldBucketSize = X.bucketSize(bucketIdx);

            s_testCase12_value = VALUES[LENGTH + 1];

            // Check missing value.
            rc1 = mX.setComputedValueAll(KEYS[LENGTH + 1],
                testCase12Updater);
            ASSERTV(rc1, 0 == rc1);

            ASSERTV(false == s_testCase12_found);
            ASSERTV(KEYS[LENGTH + 1], s_testCase12_key,
                areEqual(KEYS[LENGTH + 1], s_testCase12_key));

            // Insert increased size by 1.
            ASSERTV(LENGTH + 1 == X.size());

            // Bucket size increased by 1 if no rehash.
            if (bucketCount == X.bucketCount()) {
                ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
            }

            // Confirm the new element key and value.
            rc = X.getValue(&value, KEYS[LENGTH + 1]);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[LENGTH + 1]));

            // Memory was allocated.
            ASSERTV(sam.isInUseUp());

            // Confirm that other values have not changed.
            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
                rc = X.getValue(&value, KEYS[tj]);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value,
                    tj == LENGTH - 1 ? VALUES[LENGTH] :
                    VALUES[tj]));
            }

            // Exception testing - check missing value
            s_testCase12_alloc = &supplied;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {

                s_testCase12_value = VALUES[LENGTH + 2];

                mX.setComputedValueAll(KEYS[LENGTH + 2],
                    testCase12Updater);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
            s_testCase12_alloc = 0;
        } // END Testing 'e_ALL' with unique values
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERT(dam.isTotalSame());
        }
    } // END 'setComputedValueAll'

    // CONCERN: In no case does memory come from the default allocator.
    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
        ASSERT(dam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase11()
{
    // ------------------------------------------------------------------------
    // TEST 'setValue'
    //   Note that 'setValueFirst' was effectively tested in test
    //   case 2, as it mainly consists of a call to the bucket 'setValue'
    //   method, and is the same as 'insertUnique'.
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
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys
    //:   2 Duplicate keys
    //:
    //: 4 Each 'setValue' test call is wrapped with the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END' macros.
    //
    // Testing:
    //   bsl::size_t setValueAll(const KEY& key, const VALUE& value);
    //   bsl::size_t setValueFirst(const KEY& key, const VALUE& value);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST 'setValue'" << endl
                      << "===============" << endl;

    // This is equivalent to 'testCase13' in 'StripedUnorderedMultiMap'.

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
        bsl::size_t rc = mX.setValueAll(VALUES[0].first, VALUES[0].second);
        ASSERTV(rc, 0 == rc);
        ASSERTV(1 == X.size());

        rc = X.getValue(&value, VALUES[0].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[0].second));
    }
    {
        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        // Check missing value
        bsl::size_t rc = mX.setValueFirst(VALUES[0].first, VALUES[0].second);
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
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
        }
        ASSERTV(LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        // Check existing value.
        bsl::size_t rc = mX.setValueAll(VALUES[LENGTH - 1].first,
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

        rc = mX.setValueAll(VALUES[LENGTH + 1].first,
                            VALUES[LENGTH + 1].second);
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
            mX.setValueAll(VALUES[LENGTH + 1].first,
                           VALUES[LENGTH + 1].second);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }
    ASSERT(dam.isTotalSame());

    // Test updating duplicate keys.
    for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertAlways(VALUES[tj].first, VALUES[tj].second);
            mX.insertAlways(VALUES[tj].first, VALUES[LENGTH - tj - 1].second);
        }
        ASSERTV(2 * LENGTH == X.size());

        bslma::TestAllocatorMonitor sam(&supplied);

        // Check existing value
        bsl::size_t bucketCount   = X.bucketCount();
        bsl::size_t bucketIdx     = X.bucketIndex(VALUES[LENGTH - 1].first);
        bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

        bsl::size_t rc = mX.setValueAll(VALUES[LENGTH - 1].first,
                                        VALUES[LENGTH].second);
        ASSERTV(rc, 2 == rc);

        // Update did not change size
        ASSERTV(2 * LENGTH == X.size());

        // Bucket size did not change.
        ASSERTV(oldBucketSize == X.bucketSize(bucketIdx));

        // Confirm the values have changed to the new value
        rc = X.getValue(&values, VALUES[LENGTH - 1].first);
        ASSERTV(2 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH].second));
        ASSERTV(areEqual(values[1], VALUES[LENGTH].second));

        // No change in memory allocation if VALUE is non-allocating
        if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
            ASSERTV(sam.isInUseSame());
        }

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj]             .second)
                 || areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj]             .second)
                 || areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        sam.reset();

        // Check missing value.
        bucketIdx     = X.bucketIndex(VALUES[LENGTH + 1].first);
        oldBucketSize = X.bucketSize(bucketIdx);

        rc = mX.setValueAll(VALUES[LENGTH + 1].first,
                            VALUES[LENGTH + 1].second);
        ASSERTV(rc, 0 == rc);

        // Insert increased size by 1.
        ASSERTV(2 * LENGTH + 1 == X.size());

        // Bucket size increased by 1 if no rehash.
        if (bucketCount == X.bucketCount()) {
            ASSERTV(oldBucketSize + 1 == X.bucketSize(bucketIdx));
        }

        // Confirm the new element key and value.
        rc = X.getValue(&values, VALUES[LENGTH + 1].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(values[0], VALUES[LENGTH + 1].second));

        // Memory was allocated
        ASSERTV(sam.isInUseUp());

        // Confirm that other values have not changed.
        for (bsl::size_t tj = 0; tj < LENGTH - 1; ++tj) {
            rc = X.getValue(&values, VALUES[tj].first);
            ASSERTV(2 == rc);
            ASSERTV(areEqual(values[0], VALUES[tj].second)
                 || areEqual(values[0], VALUES[LENGTH - tj - 1].second));
            ASSERTV(areEqual(values[1], VALUES[tj].second)
                 || areEqual(values[1], VALUES[LENGTH - tj - 1].second));
        }

        // Exception testing - check missing value
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(supplied) {
            mX.setValueAll(VALUES[LENGTH + 1].first,
                           VALUES[LENGTH + 1].second);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase10()
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
    //: 2 Insertion using 'insertBulkUnique' updates the values of existing
    //:   elements having 'key'.
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
    //:   6 Trying to insert an existing key when in 'e_INSERT_UNIQUE' results
    //:     in an update.
    //:   7 Existing keys having other keys are not changed.
    //:   8 Existing keys having with inserted or updated keys show the correct
    //:     value(s).
    //:   9 For a non empty range of keys, temporary memory is allocated from
    //:     the default allocator.
    //:
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'insertBulkUnique'
    //:   2 Unique keys, 'insertBulkAlways'
    //:   3 Duplicate keys, 'insertBulkUnique'
    //:   4 Duplicate keys, 'insertBulkAlways'
    //:
    //: 4 In each instance, 'const'-interators are provided to the method.
    //:
    //: 5 Verify that, within 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros
    //:   'insertBulk' does not leak memory, and that if it completed, and
    //:   the number of elements of the hash map increased, some memory has
    //:   been allocated.
    //:
    //: 6 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid indexes, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).
    //
    // Testing:
    //   void insertBulkAlways(RANDOMIT first, last);
    //   bsl::size_t insertBulkUnique(RANDOMIT first, last);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST 'insertBulk'" << endl
                      << "=================" << endl;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    // This is equivalent to 'testCase9' in 'StripedUnorderedMap'.
    {
        if (veryVerbose) cout << endl
                              << "Test 'insertBulkUnique'" << endl;

        if (veryVeryVerbose) {
            cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
                 << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
                 << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
                 << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
        }

        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        const TestValues VALUES;  // contains 52 distinct increasing values

        const bsl::size_t MAX_LENGTH = 15;

        bsl::size_t rc;

        // Try to insert into an empty hash map, 'einsertBulkUnique'.
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
            rc = mX.insertBulkUnique(insertVec.cbegin(), insertVec.cend());

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

        // Test unique keys, 'insertBulkUnique'.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Small number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }
                ASSERTV(LENGTH == X.size());

                bslma::TestAllocatorMonitor sam(&supplied);

                // Loop on the length of the key vector passed to 'insertBulk'.
                bsl::size_t tkOffset = 0;
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<typename Obj::KVType> insertVec(&supplied);
                        sam.reset();
                        rc = mX.insertBulkUnique(insertVec.cbegin(),
                                                 insertVec.cbegin());

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
                                                         &supplied);

                    bsl::vector<typename Obj::KVType> insertVec(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                               VALUES[i + tkOffset].first,
                                               VALUES[i + tkOffset + 1].second,
                                               &supplied);
                        insertVec.push_back(item);
                    }
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        bucketSizes[j] = X.bucketSize(j);
                    }
                    sam.reset();
                    dam.reset();

                    rc = mX.insertBulkUnique(insertVec.cbegin(),
                                             insertVec.cend());

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
                    // Insert a group of 'tk' elements with a different key
                    // from existing ones, and check:
                    //: 1 The 'insertBulk' returned the correct return code.
                    //: 2 'tk' elements were added.
                    //: 3 The total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    bsl::vector<typename Obj::KVType> insertVec1(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                          VALUES[i + tkOffset + LENGTH].first,
                                          VALUES[i + tkOffset + LENGTH].second,
                                          &supplied);
                        insertVec1.push_back(item);
                    }
                    sam.reset();
                    dam.reset();

                    rc = mX.insertBulkUnique(insertVec1.cbegin(),
                                             insertVec1.cend());

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

                    // Confirm that the inserted keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t j = i + tkOffset + LENGTH;
                        VALUE       value;
                        rc = X.getValue(&value, VALUES[j].first);
                        ASSERTV(1 == rc);
                        ASSERTV(areEqual(value, VALUES[j].second));
                    }

                    // Confirm that the updated keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t j = i + tkOffset;
                        VALUE       value;
                        rc = X.getValue(&value, VALUES[j].first);
                        ASSERTV(1 == rc);
                        ASSERTV(areEqual(value, VALUES[j + 1].second));
                    }

                    tkOffset += tk;
                } // END Loop on 'tk'

                // Confirm that the untouched keys have the same values.
                for (bsl::size_t i = tkOffset; i < LENGTH; ++i) {
                    VALUE value;
                    rc = X.getValue(&value, VALUES[i].first);
                    ASSERTV(1 == rc);
                    ASSERTV(areEqual(value, VALUES[i].second));
                }
            } // END Loop on 'tj'
        } // END test for unique keys, 'insertBulkUnique'

        // Test duplicate keys, 'insertBulkUnique'.  This cannot happen in
        // either "map" or "multimap".

        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Small number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first,
                                    VALUES[LENGTH - v - 1].second);
                }
                ASSERTV(2 * LENGTH == X.size());

                bslma::TestAllocatorMonitor sam(&supplied);

                // Loop on the length of the key vector passed to 'insertBulk'.
                bsl::size_t tkOffset = 0;
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<typename Obj::KVType> insertVec(&supplied);
                        sam.reset();
                        rc = mX.insertBulkUnique(insertVec.cbegin(),
                                                 insertVec.cbegin());

                        ASSERTV(2 * LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with the same key as
                    // existing ones but different values, and check:
                    //: 1 The 'insertBulk' returned the correct return code.
                    //: 2 No elements were added.
                    //: 3 The bucket sizes have not changed.
                    //: 4 Some temporary memory was allocated.
                    //: 5 If the value type is not allocating, the allocator
                    //    did not allocate.

                    ASSERTV(ti, tj, tk, tkOffset, X.size(),
                            2 * LENGTH + tkOffset == X.size());
                    if (tkOffset + tk > LENGTH) {
                        // Skip sizes that do not properly fit.
                        continue;
                    }
                    bsl::size_t              bucketCount = X.bucketCount();
                    bsl::vector<bsl::size_t> bucketSizes(bucketCount,
                                                         &supplied);

                    bsl::vector<typename Obj::KVType> insertVec(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                               VALUES[i + tkOffset].first,
                                               VALUES[i + tkOffset + 1].second,
                                               &supplied);
                        insertVec.push_back(item);
                    }
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        bucketSizes[j] = X.bucketSize(j);
                    }
                    sam.reset();
                    dam.reset();

                    rc = mX.insertBulkUnique(insertVec.cbegin(),
                                             insertVec.cend());

                    // Confirm no new records
                    ASSERTV(0 == rc);

                    // Confirm temporary memory usage of default allocator.
                    ASSERTV(dam.isTotalUp());

                    // No change in memory allocation if 'VALUE' is
                    // non-allocating type.
                    if (false == bslma::UsesBslmaAllocator<VALUE>::value) {
                        ASSERTV(sam.isInUseSame());
                    }

                    // Confirm size did not change.
                    ASSERTV(2 * LENGTH + tkOffset == X.size());

                    // Confirm bucket sizes did not change.
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        ASSERTV(bucketSizes[j] == X.bucketSize(j));
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with a different key
                    // from existing ones, and check:
                    //: 1 The 'insertBulk' call returned the correct value.
                    //: 2 'tk' elements were added.
                    //: 3 The total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    bsl::vector<typename Obj::KVType> insertVec1(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                          VALUES[i + tkOffset + LENGTH].first,
                                          VALUES[i + tkOffset + LENGTH].second,
                                          &supplied);
                        insertVec1.push_back(item);
                    }
                    sam.reset();
                    dam.reset();

                    rc = mX.insertBulkUnique(insertVec1.cbegin(),
                                             insertVec1.cend());
                    ASSERTV(tk == rc); // Confirm tk new records
                    // Confirm temporary memory usage of default allocator
                    ASSERTV(dam.isTotalUp());
                    // memory allocation increased
                    ASSERTV(sam.isInUseUp());
                    // Confirm size increased
                    ASSERTV(2 * LENGTH + tkOffset + tk == X.size());
                    // Confirm total bucket sizes increased
                    if (bucketCount == X.bucketCount()) {
                        bsl::size_t newSumBucketSize = 0;
                        bsl::size_t oldSumBucketSize = 0;
                        for (bsl::size_t j = 0; j < bucketCount; ++j) {
                            newSumBucketSize += X.bucketSize(j);
                            oldSumBucketSize += bucketSizes[j];
                        }
                        ASSERTV(oldSumBucketSize + tk == newSumBucketSize);
                    }

                    // Confirm that the inserted keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t j = i + tkOffset + LENGTH;
                        VALUE       value;
                        rc = X.getValue(&value, VALUES[j].first);
                        ASSERTV(1 == rc);
                        ASSERTV(areEqual(value, VALUES[j].second));
                    }

                    // Confirm that the updated keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t        j = i + tkOffset;
                        bsl::vector<VALUE> values;
                        rc = X.getValue(&values, VALUES[j].first);
                        ASSERTV(2 == rc);
                        ASSERT(areEqual(values[0], VALUES[j+1]       .second)
                            || areEqual(values[0], VALUES[j]         .second)
                            || areEqual(values[0], VALUES[LENGTH-j-1].second));
                        ASSERT(areEqual(values[1], VALUES[j + 1]     .second)
                            || areEqual(values[1], VALUES[j]         .second)
                            || areEqual(values[1], VALUES[LENGTH-j-1].second));
                    }

                    tkOffset += tk;
                } // END Loop on 'tk'

                // Confirm that the untouched keys have the same values
                for (bsl::size_t i = tkOffset; i < LENGTH; ++i) {
                    bsl::vector<VALUE> values;
                    rc = X.getValue(&values, VALUES[i].first);
                    ASSERTV(2 == rc);
                    ASSERT(areEqual(values[0], VALUES[i]             .second)
                        || areEqual(values[0], VALUES[LENGTH - i - 1].second));
                    ASSERT(areEqual(values[1], VALUES[i]             .second)
                        || areEqual(values[1], VALUES[LENGTH - i - 1].second));
                }
            } // END Loop on 'tj'
        } // END test for duplicate keys, 'insertBulkUnique'

    } // END 'insertBulkUnique' testing

    // This is equivalent to 'testCase11' in 'StripedUnorderedMultiMap'.
    {
        if (veryVerbose) cout << endl
                          << "Test 'insertBulkAlways'" << endl;

        if (veryVeryVerbose) {
            cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
                 << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
                 << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
                 << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
        }

        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        const TestValues VALUES;  // contains 52 distinct increasing values

        const bsl::size_t MAX_LENGTH = 15;

        bsl::size_t rc;

        // Try to insert into an empty hash map, 'e_INSERT_ALWAYS'.
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
            mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

            ASSERTV(MAX_LENGTH, X.size(), MAX_LENGTH == X.size());
            // Confirm that the keys inserted have the correct values.
            for (bsl::size_t tj = 0; tj < MAX_LENGTH; ++tj) {
                VALUE value;
                rc = X.getValue(&value, VALUES[tj].first);
                ASSERTV(1 == rc);
                ASSERTV(areEqual(value, VALUES[tj].second));
            }
        }

        // Test unique keys, 'insertBulkAlways'.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Small number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }
                ASSERTV(LENGTH == X.size());

                bslma::TestAllocatorMonitor sam(&supplied);

                // Loop on the length of the key vector passed to 'insertBulk'.
                bsl::size_t tkOffset = 0;
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<typename Obj::KVType> insertVec(&supplied);
                        sam.reset();
                        mX.insertBulkAlways(insertVec.cbegin(),
                                            insertVec.cbegin());

                        ASSERTV(LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }
                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with the same key as
                    // existing ones but different values, and check:
                    //: 1 The 'insertBulk' returned the correct value.
                    //: 2 'tk' elements were added.
                    //: 3 The total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    ASSERTV(ti, tj, tk, tkOffset, X.size(),
                            LENGTH + 2 * tkOffset == X.size());

                    if (tkOffset + tk > LENGTH) {
                        // Skip sizes that do not properly fit.
                        continue;
                    }
                    bsl::size_t              bucketCount = X.bucketCount();
                    bsl::vector<bsl::size_t> bucketSizes(bucketCount,
                                                         &supplied);

                    bsl::vector<typename Obj::KVType> insertVec(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                               VALUES[i + tkOffset].first,
                                               VALUES[i + tkOffset + 1].second,
                                               &supplied);
                        insertVec.push_back(item);
                    }
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        bucketSizes[j] = X.bucketSize(j);
                    }
                    sam.reset();
                    dam.reset();

                    mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

                    // Confirm temporary memory usage of default allocator
                    ASSERTV(dam.isTotalUp());

                    // memory allocation increased
                    ASSERTV(sam.isInUseUp());

                    // Confirm size increased
                    ASSERTV(LENGTH + 2 * tkOffset + tk == X.size());

                    // Confirm total bucket sizes increased
                    if (bucketCount == X.bucketCount()) {
                        bsl::size_t newSumBucketSize = 0;
                        bsl::size_t oldSumBucketSize = 0;
                        for (bsl::size_t j = 0; j < bucketCount; ++j) {
                            newSumBucketSize += X.bucketSize(j);
                            oldSumBucketSize += bucketSizes[j];
                        }
                        ASSERTV(oldSumBucketSize + tk == newSumBucketSize);
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with a different key
                    // from existing ones, and check:
                    //: 1 That 'insertBulk' returned the correct value.
                    //: 2 'tk' elements were added.
                    //: 3 The total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    bsl::vector<typename Obj::KVType> insertVec1(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                          VALUES[i + tkOffset + LENGTH].first,
                                          VALUES[i + tkOffset + LENGTH].second,
                                          &supplied);
                        insertVec1.push_back(item);
                    }
                    sam.reset();
                    dam.reset();

                    mX.insertBulkAlways(insertVec1.cbegin(),
                                        insertVec1.cend());

                    // Confirm temporary memory usage of default allocator.
                    ASSERTV(dam.isTotalUp());

                    // Memory allocation increased.
                    ASSERTV(sam.isInUseUp());

                    // Confirm size increased.
                    ASSERTV(LENGTH, tkOffset, tk, X.size(),
                            LENGTH + 2 * tkOffset + 2 * tk == X.size());

                    // Confirm total bucket sizes increased.
                    if (bucketCount == X.bucketCount()) {
                        bsl::size_t newSumBucketSize = 0;
                        bsl::size_t oldSumBucketSize = 0;
                        for (bsl::size_t j = 0; j < bucketCount; ++j) {
                            newSumBucketSize += X.bucketSize(j);
                            oldSumBucketSize += bucketSizes[j];
                        }
                        ASSERTV(oldSumBucketSize, tk, newSumBucketSize,
                                oldSumBucketSize + 2 * tk == newSumBucketSize);
                    }

                    // Confirm that the inserted new keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t j = i + tkOffset + LENGTH;
                        VALUE       value;
                        rc = X.getValue(&value, VALUES[j].first);
                        ASSERTV(1 == rc);
                        ASSERTV(areEqual(value, VALUES[j].second));
                    }

                    // Confirm that the inserted existing keys have the
                    // appropriate values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t        j = i + tkOffset;
                        bsl::vector<VALUE> values;
                        rc = X.getValue(&values, VALUES[j].first);
                        ASSERTV(2 == rc);
                        ASSERTV(areEqual(values[0], VALUES[j + 1].second)
                             || areEqual(values[0], VALUES[j]    .second));
                        ASSERTV(areEqual(values[1], VALUES[j + 1].second)
                             || areEqual(values[1], VALUES[j]    .second));
                    }

                    tkOffset += tk;
                } // END Loop on 'tk'

                // Confirm that the untouched keys have the same values.
                for (bsl::size_t i = tkOffset; i < LENGTH; ++i) {
                    VALUE value;
                    rc = X.getValue(&value, VALUES[i].first);
                    ASSERTV(1 == rc);
                    ASSERTV(areEqual(value, VALUES[i].second));
                }
            } // END Loop on 'tj'
        } // END test for unique keys, 'insertBulkAlways'

        // Test duplicate keys, 'insertBulkAlways'.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // small number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first,
                                    VALUES[LENGTH - v - 1].second);
                }
                ASSERTV(2 * LENGTH == X.size());

                bslma::TestAllocatorMonitor sam(&supplied);

                // Loop on the length of the key vector passed to 'insertBulk'.
                bsl::size_t tkOffset = 0;
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<typename Obj::KVType> insertVec(&supplied);
                        sam.reset();
                        mX.insertBulkAlways(insertVec.cbegin(),
                                            insertVec.cbegin());

                        ASSERTV(2 * LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with the same key as
                    // existing ones but different values, and check:
                    //: 1 The 'insertBulk' returned the correct value;
                    //: 2 'tk' elements were added.
                    //: 3 The Total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    ASSERTV(ti, tj, tk, tkOffset, X.size(),
                            2 * LENGTH + 2 * tkOffset == X.size());

                    if (tkOffset + tk > LENGTH) {
                        // Skip sizes that do not properly fit.
                        continue;
                    }
                    bsl::size_t              bucketCount = X.bucketCount();
                    bsl::vector<bsl::size_t> bucketSizes(bucketCount,
                                                         &supplied);

                    bsl::vector<typename Obj::KVType> insertVec(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                               VALUES[i + tkOffset].first,
                                               VALUES[i + tkOffset + 1].second,
                                               &supplied);
                        insertVec.push_back(item);
                    }
                    for (bsl::size_t j = 0; j < bucketCount; ++j) {
                        bucketSizes[j] = X.bucketSize(j);
                    }
                    sam.reset();
                    dam.reset();

                    mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

                    // Confirm temporary memory use from the default allocator.
                    ASSERTV(dam.isTotalUp());

                    // Memory allocation increased.
                    ASSERTV(sam.isInUseUp());

                    // Confirm size increased.
                    ASSERTV(2 * LENGTH + 2 * tkOffset + tk == X.size());

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

                    // Here: '1 <= tk <= 3'
                    //
                    // Insert a group of 'tk' elements with a different key
                    // from existing ones, and check:
                    //: 1 The 'insertBulk' returned the correct value.
                    //: 2 'tk' elements were added.
                    //: 3 The total size of buckets increased by 'tk'.
                    //: 4 Some temporary memory was allocated.
                    //: 5 Some memory was allocated.

                    bsl::vector<typename Obj::KVType> insertVec1(&supplied);
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        typename Obj::KVType item(
                                          VALUES[i + tkOffset + LENGTH].first,
                                          VALUES[i + tkOffset + LENGTH].second,
                                          &supplied);
                        insertVec1.push_back(item);
                    }
                    sam.reset();
                    dam.reset();

                    mX.insertBulkAlways(insertVec1.cbegin(),
                                        insertVec1.cend());

                    // Confirm temporary memory usage of default allocator.
                    ASSERTV(dam.isTotalUp());

                    // Memory allocation increased.
                    ASSERTV(sam.isInUseUp());

                    // Confirm size increased.
                    ASSERTV(2 * LENGTH + 2 * tkOffset + 2 * tk == X.size());

                    // Confirm total bucket sizes increased.
                    if (bucketCount == X.bucketCount()) {
                        bsl::size_t newSumBucketSize = 0;
                        bsl::size_t oldSumBucketSize = 0;
                        for (bsl::size_t j = 0; j < bucketCount; ++j) {
                            newSumBucketSize += X.bucketSize(j);
                            oldSumBucketSize += bucketSizes[j];
                        }
                        ASSERTV(oldSumBucketSize + 2 * tk == newSumBucketSize);
                    }

                    // Confirm that the inserted new keys have the appropriate
                    // values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t j = i + tkOffset + LENGTH;
                        VALUE       value;
                        rc = X.getValue(&value, VALUES[j].first);
                        ASSERTV(1 == rc);
                        ASSERTV(areEqual(value, VALUES[j].second));
                    }

                    // Confirm that the inserted existing keys have the
                    // appropriate values.
                    for (bsl::size_t i = 0; i < tk; ++i) {
                        bsl::size_t        j = i + tkOffset;
                        bsl::vector<VALUE> values;
                        rc = X.getValue(&values, VALUES[j].first);
                        ASSERTV(3 == rc);
                        ASSERT(areEqual(values[0], VALUES[j+1]       .second)
                            || areEqual(values[0], VALUES[j]         .second)
                            || areEqual(values[0], VALUES[LENGTH-j-1].second));
                        ASSERT(areEqual(values[1], VALUES[j+1]       .second)
                            || areEqual(values[1], VALUES[j]         .second)
                            || areEqual(values[1], VALUES[LENGTH-j-1].second));
                        ASSERT(areEqual(values[2], VALUES[j+1]       .second)
                            || areEqual(values[2], VALUES[j]         .second)
                            || areEqual(values[2], VALUES[LENGTH-j-1].second));
                    }

                    tkOffset += tk;
                } // END Loop on 'tk'

                // Confirm that the untouched keys have the same values.
                for (bsl::size_t i = tkOffset; i < LENGTH; ++i) {
                    bsl::vector<VALUE> values;
                    rc = X.getValue(&values, VALUES[i].first);
                    ASSERTV(2 == rc);
                    ASSERTV(
                           areEqual(values[0], VALUES[i].second) ||
                           areEqual(values[0], VALUES[LENGTH - i - 1].second));
                    ASSERTV(
                           areEqual(values[1], VALUES[i].second) ||
                           areEqual(values[1], VALUES[LENGTH - i - 1].second));
                }
            } // END Loop on 'tj'
        } // END test for duplicate keys, 'insertBulkAlways'
    } // END 'insertBulkAlways' testing

    if (verbose) cout << "Exception Testing - 'insertBulkUnique'" << endl;
    {
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
                                          &supplied);
                insertVec.push_back(item);
            }
            // Empty
            mX.insertBulkUnique(insertVec.cbegin(), insertVec.cend());

            // Existing
            mX.insertBulkUnique(insertVec.cbegin(), insertVec.cend());

            // New
            insertVec.clear();
            for (bsl::size_t tj = 2; tj < 4; ++tj) {
                typename Obj::KVType item(VALUES[tj].first,
                                          VALUES[tj].second,
                                          &supplied);
                insertVec.push_back(item);
            }
            mX.insertBulkUnique(insertVec.cbegin(), insertVec.cend());

            ASSERTV(sam.isInUseUp());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }

    if (verbose) cout << "Exception Testing - 'insertBulkAlways'" << endl;
    {
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
                                          &supplied);
                insertVec.push_back(item);
            }
            // Empty
            mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

            // Existing
            mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

            // New
            insertVec.clear();
            for (bsl::size_t tj = 2; tj < 4; ++tj) {
                typename Obj::KVType item(VALUES[tj].first,
                                          VALUES[tj].second,
                                          &supplied);
                insertVec.push_back(item);
            }
            mX.insertBulkAlways(insertVec.cbegin(), insertVec.cend());

            ASSERTV(sam.isInUseUp());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END;
    }

    if (verbose) cout << "Negative Testing" << endl;
    {
        if (veryVerbose) cout << "\t'insertBulk' Method" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        Obj mX(8, 4, &supplied);

        bsl::vector<typename Obj::KVType> insertVec(&scratch);
        typename Obj::KVType              item(VALUES[0].first,
                                               VALUES[0].second,
                                               &scratch);
        insertVec.push_back(item);

        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL(mX.insertBulkUnique(insertVec.cend(),
                                        insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulkUnique(insertVec.cbegin(),
                                        insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulkUnique(insertVec.cbegin(),
                                        insertVec.cend()));

        ASSERT_FAIL(mX.insertBulkAlways(insertVec.cend(),
                                        insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulkAlways(insertVec.cbegin(),
                                        insertVec.cbegin()));
        ASSERT_PASS(mX.insertBulkAlways(insertVec.cbegin(),
                                        insertVec.cend()));
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase9()
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

    // This is equivalent to 'testCase8' in 'StripedUnorderedMap' and to
    // 'testCase10' in 'StripedUnorderedMultiMap'.

    if (verbose) cout << endl
                      << "TEST 'clear'" << endl
                      << "============" << endl;

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

    for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const bsl::size_t LENGTH = ti;

        Obj mX(8, 4, &supplied);  const Obj& X = mX;

        bslma::TestAllocatorMonitor sam(&supplied);

        for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {
            mX.insertUnique(VALUES[tj].first, VALUES[tj].second);
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
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase8()
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
    //: 9 QoI: Erasure returns memory to the allocator.
    //:
    //:10 QoI: Temporary allocation are taken from the default allocator.
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
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'eraseBulkFirst'
    //:   2 Unique keys, 'eraseBulkAll'
    //:   3 Duplicate keys, 'eraseBulkAll'
    //:   4 Duplicate keys, 'eraseBulkFirst'
    //:
    //: 4 In each instance, 'const'-interators are provided to the method.
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid indexes, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).
    //
    // Testing:
    //   bsl::size_t eraseBulkAll(RANDOMIT first, last);
    //   bsl::size_t eraseBulkFirst(RANDOMIT first, last);
    // ------------------------------------------------------------------------

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

    // This is equivalent to 'testCase7' in 'StripedUnorderedMap'.
    {
        if (veryVerbose) cout << endl
                              << "Test 'eraseBulk' First" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 15;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        // Try to erase from an empty hash map, 'e_FIRST'.

        {
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            // Keys to be erased with 'eraseBulk'.
            bsl::vector<KEY> eraseKeys(&scratch);
            for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                eraseKeys.push_back(VALUES[v].first);
            }
            bsl::size_t count = mX.eraseBulkFirst(eraseKeys.cbegin(),
                                                  eraseKeys.cend());
            ASSERTV(0 == count);
            ASSERTV(0 == X.size());
        }

        // Verify the tail pointer is maintained correctly.
        {
            bslma::TestAllocatorMonitor sam(&supplied);

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }

                bsl::vector<KEY> eraseKeys;
                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    eraseKeys.push_back(VALUES[v].first);
                }
                ASSERT(eraseKeys.size() == mX.eraseBulkFirst(
                                                            eraseKeys.cbegin(),
                                                            eraseKeys.cend()));

                mX.insertUnique(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }

                bsl::vector<KEY> eraseKeys;
                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    eraseKeys.push_back(VALUES[v].first);
                }
                ASSERT(eraseKeys.size() == mX.eraseBulkAll(eraseKeys.cbegin(),
                                                           eraseKeys.cend()));

                mX.insertUnique(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                }

                bsl::vector<KEY> eraseKeys;
                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    eraseKeys.push_back(VALUES[v].first);
                }
                ASSERT(3 * eraseKeys.size() == mX.eraseBulkAll(
                                                            eraseKeys.cbegin(),
                                                            eraseKeys.cend()));

                mX.insertAlways(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());
        }

        // Test unique keys, 'eraseBulkFirst'.

        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Loop on the length of the key vector passed to 'eraseBulk'.
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // arbitrary number of buckets and stripes
                    Obj mX(8, 4, &supplied);  const Obj& X = mX;

                    for (bsl::size_t v = 0; v < LENGTH; ++v) {
                        mX.insertUnique(VALUES[v].first, VALUES[v].second);
                    }
                    ASSERTV(LENGTH == X.size());

                    bslma::TestAllocatorMonitor sam(&supplied);

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<KEY> eraseKeys(&scratch);

                        bsl::size_t count =
                            mX.eraseBulkFirst(eraseKeys.cbegin(),
                                              eraseKeys.cbegin());
                        ASSERTV(0 == count);
                        ASSERTV(LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // In a loop, erase all elements in groups of 'tk'
                    // elements, check that each 'eraseBulk' is successful, and
                    // that the final number of elements is 0.  The last step
                    // may contain keys that were already removed.

                    ASSERTV(LENGTH == X.size());
                    for (bsl::size_t v = 0; v < LENGTH; v += tk) {
                        bsl::vector<KEY> eraseKeys(tk, &scratch);
                        // Index and size of buckets related to the keys in the
                        // current 'eraseKeys' vector.
                        bsl::vector<bsl::size_t> bucketIdxs (tk, &scratch);
                        bsl::vector<bsl::size_t> bucketSizes(tk, &scratch);
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t idx = (v + i) % LENGTH;
                            eraseKeys  [i] = VALUES[idx].first;
                            bucketIdxs [i] = X.bucketIndex(VALUES[idx].first);
                            bucketSizes[i] = X.bucketSize(bucketIdxs[i]);
                        }
                        sam.reset();

                        bsl::size_t rc = mX.eraseBulkFirst(eraseKeys.cbegin(),
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
        } // END test for unique keys, 'eraseBulkFirst'

        // Test duplicate keys, 'eraseBulkFirst'; this cannot happen in either
        // "map" or "multimap".
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Loop on the length of the key vector passed to 'eraseBulk'.
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // arbitrary number of buckets and stripes
                    Obj mX(8, 4, &supplied);  const Obj& X = mX;

                    for (bsl::size_t v = 0; v < LENGTH; ++v) {
                        mX.insertUnique(VALUES[v].first, VALUES[v].second);
                        mX.insertAlways(VALUES[v].first,
                                        VALUES[LENGTH - v - 1].second);
                    }

                    bslma::TestAllocatorMonitor sam(&supplied);

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<KEY> eraseKeys(&scratch);

                        bsl::size_t count =
                                         mX.eraseBulkFirst(eraseKeys.cbegin(),
                                                           eraseKeys.cbegin());
                        ASSERTV(0 == count);
                        ASSERTV(2 * LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Erase all elements in a loop in groups of tk elements,
                    // and check that each eraseBulk is successful, and that
                    // the final number of elements is 0.  The last step may
                    // contain keys that were already removed.

                    ASSERTV(2 * LENGTH == X.size());
                    for (bsl::size_t v = 0; v < LENGTH; v += tk) {
                        bsl::vector<KEY> eraseKeys(tk, &scratch);
                        // Index and size of buckets related to the keys in the
                        // current 'eraseKeys' vector.
                        bsl::vector<bsl::size_t> bucketIdxs (tk, &scratch);
                        bsl::vector<bsl::size_t> bucketSizes(tk, &scratch);
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t idx = (v + i) % LENGTH;
                            eraseKeys  [i] = VALUES[idx].first;
                            bucketIdxs [i] = X.bucketIndex(VALUES[idx].first);
                            bucketSizes[i] = X.bucketSize(bucketIdxs[i]);
                        }
                        sam.reset();
                        dam.reset();

                        bsl::size_t rc = mX.eraseBulkFirst(eraseKeys.cbegin(),
                                                           eraseKeys.cend());
                        bsl::size_t expNumErased = tk;
                        bsl::size_t expSize      = 2 * LENGTH - v - tk;

                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                rc == expNumErased);
                        ASSERTV(ti, tj, tk, v, rc, expSize,
                                expSize == X.size());

                        // Confirm reduction in size for each of the buckets
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t newBucketSize =
                                                   X.bucketSize(bucketIdxs[i]);
                            if (expNumErased == tk) {
                                if (bucketSizes[i] > 0) {
                                    ASSERTV(ti, tj, tk, v, i, bucketSizes[i],
                                            bucketSizes[i] > newBucketSize);
                                }
                            } else {
                                ASSERTV(bucketSizes[i] >= newBucketSize);
                            }
                        }

                        // Confirm reducation in memory usage
                        ASSERTV(sam.isInUseDown());

                        // Confirm temporary memory usage of default allocator
                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                dam.isTotalUp());
                    }
                } // END Loop on 'tk'
            } // END Loop on 'tj'
        } // END test for duplicate keys, 'eraseBulkFirst'
    } // END 'eraseBulkFirst' testing

    // This is equivalent to 'testCase9' in 'StripedUnorderedMultiMap'.
    {
        if (veryVerbose) cout << endl
                              << "Test 'eraseBulkAll'" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 15;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        // Try to erase from an empty hash map, 'eraseBulkAll'.
        {
            Obj mX(8, 4, &supplied);  const Obj& X = mX;

            // Keys to be erased with 'eraseBulk'
            bsl::vector<KEY> eraseKeys(&scratch);
            for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                eraseKeys.push_back(VALUES[v].first);
            }
            bsl::size_t count = mX.eraseBulkAll(eraseKeys.cbegin(),
                                                eraseKeys.cend());
            ASSERTV(0 == count);
            ASSERTV(0 == X.size());
        }

        // Test unique keys, 'eraseBulkAll'.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Loop on the length of the key vector passed to 'eraseBulk'.
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // arbitrary number of buckets and stripes
                    Obj mX(8, 4, &supplied);  const Obj& X = mX;

                    for (bsl::size_t v = 0; v < LENGTH; ++v) {
                        mX.insertUnique(VALUES[v].first, VALUES[v].second);
                    }
                    ASSERTV(LENGTH == X.size());

                    bslma::TestAllocatorMonitor sam(&supplied);

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<KEY> eraseKeys(&scratch);

                        bsl::size_t count =
                                           mX.eraseBulkAll(eraseKeys.cbegin(),
                                                           eraseKeys.cbegin());
                        ASSERTV(0 == count);
                        ASSERTV(LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // Erase all elements in a loop in groups of tk elements,
                    // and check that each eraseBulk is successful, and that
                    // the final number of elements is 0.  The last step may
                    // contain keys that were already removed.
                    ASSERTV(LENGTH == X.size());
                    for (bsl::size_t v = 0; v < LENGTH; v += tk) {
                        bsl::vector<KEY> eraseKeys(tk, &scratch);
                        // Index and size of buckets related to the keys in the
                        // current eraseKeys.
                        bsl::vector<bsl::size_t> bucketIdxs (tk, &scratch);
                        bsl::vector<bsl::size_t> bucketSizes(tk, &scratch);
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t idx = (v + i) % LENGTH;
                            eraseKeys  [i] = VALUES[idx].first;
                            bucketIdxs [i] = X.bucketIndex(VALUES[idx].first);
                            bucketSizes[i] = X.bucketSize(bucketIdxs[i]);
                        }
                        sam.reset();
                        dam.reset();

                        bsl::size_t rc = mX.eraseBulkAll(eraseKeys.cbegin(),
                                                         eraseKeys.cend());
                        bsl::size_t expNumErased =
                                            LENGTH - v >= tk ? tk : LENGTH - v;
                        bsl::size_t expSize =
                                        LENGTH - v >= tk ? LENGTH - v - tk : 0;

                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                rc == expNumErased);
                        ASSERTV(ti, tj, tk, v, rc, expSize,
                                expSize == X.size());

                        // Confirm reduction in size for each of the buckets
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t newBucketSize =
                                                   X.bucketSize(bucketIdxs[i]);
                            if (expNumErased == tk) {
                                ASSERTV(bucketSizes[i] > newBucketSize);
                            } else {
                                ASSERTV(bucketSizes[i] >= newBucketSize);
                            }
                        }

                        // Confirm reducation in memory usage
                        ASSERTV(sam.isInUseDown());

                        // Confirm temporary memory usage of default allocator
                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                dam.isTotalUp());
                    }
                    ASSERTV(0 == X.size());
                } // END Loop on 'tk'
            } // END Loop on 'tj'
        } // END test for unique keys, 'eraseBulkAll'

        // Test duplicate keys, 'eraseBulkAll'.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // Loop on the length of the key vector passed to 'eraseBulk'.
                for (bsl::size_t tk = 0; tk <= 3 && tk <= LENGTH; ++tk) {

                    // arbitrary number of buckets and stripes
                    Obj mX(8, 4, &supplied);  const Obj& X = mX;

                    for (bsl::size_t v = 0; v < LENGTH; ++v) {
                        mX.insertUnique(VALUES[v].first, VALUES[v].second);
                        mX.insertAlways(VALUES[v].first,
                                        VALUES[LENGTH - v - 1].second);
                    }

                    bslma::TestAllocatorMonitor sam(&supplied);

                    // special case: empty vector of keys
                    if (tk == 0) {
                        bsl::vector<KEY> eraseKeys(&scratch);

                        bsl::size_t count =
                                           mX.eraseBulkAll(eraseKeys.cbegin(),
                                                           eraseKeys.cbegin());
                        ASSERTV(0 == count);
                        ASSERTV(2 * LENGTH == X.size());
                        ASSERTV(sam.isInUseSame());
                        continue;
                    }

                    // Here: '1 <= tk <= 3'
                    //
                    // In a loop, erase all elements in groups of 'tk'
                    // elements, check that each 'eraseBulk' is successful, and
                    // that the final number of elements is 0.  The last step
                    // may contain keys that were already removed.

                    ASSERTV(2 * LENGTH == X.size());
                    for (bsl::size_t v = 0; v < LENGTH; v += tk) {
                        bsl::vector<KEY> eraseKeys(tk, &scratch);
                        // Index and size of buckets related to the keys in the
                        // current 'eraseKeys' vector.
                        bsl::vector<bsl::size_t> bucketIdxs (tk, &scratch);
                        bsl::vector<bsl::size_t> bucketSizes(tk, &scratch);
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t idx = (v + i) % LENGTH;
                            eraseKeys  [i] = VALUES[idx].first;
                            bucketIdxs [i] = X.bucketIndex(VALUES[idx].first);
                            bucketSizes[i] = X.bucketSize(bucketIdxs[i]);
                        }
                        sam.reset();
                        dam.reset();

                        bsl::size_t rc = mX.eraseBulkAll(eraseKeys.cbegin(),
                                                         eraseKeys.cend());
                        bsl::size_t expNumErased =
                                  LENGTH - v >= tk ? 2 * tk : 2 * (LENGTH - v);
                        bsl::size_t expSize =
                                  LENGTH - v >= tk ? 2 * (LENGTH - v - tk) : 0;

                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                rc == expNumErased);
                        ASSERTV(ti, tj, tk, v, rc, expSize,
                                expSize == X.size());

                        // Confirm reduction in size for each of the buckets.
                        for (bsl::size_t i = 0; i < tk; ++i) {
                            bsl::size_t newBucketSize =
                                                   X.bucketSize(bucketIdxs[i]);
                            if (expNumErased == tk) {
                                if (bucketSizes[i] > 0) {
                                    ASSERTV(ti, tj, tk, v, i, bucketSizes[i],
                                            bucketSizes[i] > newBucketSize);
                                }
                            } else {
                                ASSERTV(bucketSizes[i] >= newBucketSize);
                            }
                        }

                        // Confirm reducation in memory usage.
                        ASSERTV(sam.isInUseDown());

                        // Confirm temporary memory usage of default allocator.
                        ASSERTV(ti, tj, tk, v, rc, expNumErased,
                                dam.isTotalUp());
                    }
                    ASSERTV(0 == X.size());
                } // END Loop on 'tk'
            } // END Loop on 'tj'
        } // END test for duplicate keys, 'eraseBulkAll'
    } // END 'eraseBulkAll' testing

    if (verbose) cout << "Negative Testing" << endl;
    {
        if (veryVerbose) cout << "\t'eraseBulk' Method" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        Obj mX(8, 4, &supplied);

        bsl::vector<KEY> eraseKeys(1, &scratch);
        eraseKeys[0] = VALUES[0].first;

        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL(mX.eraseBulkFirst(eraseKeys.cend(),
                                      eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulkFirst(eraseKeys.cbegin(),
                                      eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulkFirst(eraseKeys.cbegin(),
                                      eraseKeys.cend()));

        ASSERT_FAIL(mX.eraseBulkAll(eraseKeys.cend(),
                                    eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulkAll(eraseKeys.cbegin(),
                                    eraseKeys.cbegin()));
        ASSERT_PASS(mX.eraseBulkAll(eraseKeys.cbegin(),
                                    eraseKeys.cend()));
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase7()
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
    //:
    //: 3 P-2 is repeated for containers having different configurations:
    //:   1 Unique keys, 'eraseFirst'
    //:   2 Unique keys, 'eraseAll'
    //:   3 Duplicate keys, 'eraseAll'
    //:   4 Duplicate keys, 'eraseFirst'
    //
    // Testing:
    //   bsl::size_t eraseAll(const KEY& key);
    //   bsl::size_t eraseFirst(const KEY& key);
    // ------------------------------------------------------------------------

    if (verbose) cout << endl
                      << "TEST 'erase'" << endl
                      << "============" << endl;

    // This is equivalent to 'testCase6' in 'StripedUnorderedMap', and to
    // 'testCase7' in 'StripedUnorderedMultiMap'.

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    {
        if (veryVerbose) cout << endl
                                  << "Test 'eraseFirst'" << endl;

        if (veryVeryVerbose) {
            cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
                 << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
                 << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
                 << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
        }

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 15;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        // Try to erase from an empty hash map.
        {
            Obj mX(8, 4, &supplied);

            bsl::size_t rc = mX.eraseFirst(VALUES[0].first);
            ASSERTV(rc == 0);

            rc = mX.eraseAll(VALUES[0].first);
            ASSERTV(rc == 0);
        }

        // Verify the tail pointer is maintained correctly.
        {
            bslma::TestAllocatorMonitor sam(&supplied);

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }

                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    ASSERT(1 == mX.eraseFirst(VALUES[v].first));
                }

                mX.insertUnique(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
                }

                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    ASSERT(1 == mX.eraseAll(VALUES[v].first));
                }

                mX.insertUnique(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());

            sam.reset();
            {
                Obj mX(1, 1, &supplied);

                mX.disableRehash();

                for (bsl::size_t v = 0; v < MAX_LENGTH; ++v) {
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                }

                for (bsl::size_t v = MAX_LENGTH / 2; v < MAX_LENGTH; ++v) {
                    ASSERT(3 == mX.eraseAll(VALUES[v].first));
                }

                mX.insertAlways(VALUES[MAX_LENGTH - 1].first,
                                VALUES[MAX_LENGTH - 1].second);
            }
            ASSERT(sam.isInUseSame());
        }

        // This is for unique keys.
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // arbitrary number of buckets and stripes
                Obj mX(8, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertUnique(VALUES[v].first, VALUES[v].second);
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

                    bsl::size_t rc = mX.eraseFirst(VALUES[v].first);
                    ASSERT(rc == 1);
                    ASSERT(LENGTH - v - 1 == X.size());

                    bsl::size_t newBucketSize = X.bucketSize(bucketIdx);
                    ASSERT(newBucketSize == oldBucketSize - 1);

                    ASSERT(sam.isInUseDown());
                    sam.reset();

                    // Try to erase again the already erased key.
                    rc = mX.eraseFirst(VALUES[v].first);
                    ASSERT(rc == 0);
                    ASSERT(LENGTH - v - 1 == X.size());
                    ASSERT(sam.isInUseSame());
                }
                ASSERT(0 == X.size());
            }
        }
    }

    // This is equivalent to 'testCase7' in 'StripedUnorderedMultiMap'.

    {
        if (veryVerbose) cout << endl
                          << "Test 'eraseAll'" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 9;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        // This is for unique keys
        for (bsl::size_t ti = 0; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // arbitrary number of buckets and stripes
                Obj mX(32, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                }

                bslma::TestAllocatorMonitor sam(&supplied);

                // In a loop, erase all elements, check that each erase is
                // successful, and that the final number of elements is 0.

                ASSERTV(LENGTH == X.size());
                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    sam.reset();
                    // Find the bucket where the key resides.
                    bsl::size_t bucketIdx     = X.bucketIndex(VALUES[v].first);
                    bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

                    bsl::size_t rc = mX.eraseAll(VALUES[v].first);
                    ASSERTV(rc == 1);
                    ASSERTV(LENGTH - v - 1 == X.size());

                    bsl::size_t newBucketSize = X.bucketSize(bucketIdx);
                    ASSERTV(newBucketSize == oldBucketSize - 1);

                    ASSERTV(sam.isInUseDown());
                    sam.reset();

                    // Try to erase again the already erased key.
                    rc = mX.eraseAll(VALUES[v].first);
                    ASSERTV(rc == 0);
                    ASSERTV(LENGTH - v - 1 == X.size());
                    ASSERTV(sam.isInUseSame());
                }
                ASSERTV(0 == X.size());
            }
        }

        // Test erasing duplicate keys.

        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // arbitrary number of buckets and stripes
                Obj mX(32, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first,
                                    VALUES[LENGTH - v - 1].second);
                }

                bslma::TestAllocatorMonitor sam(&supplied);

                // In a loop, erase all elements, check that each erase is
                // successful, and that the final number of elements is 0.

                ASSERTV(2 * LENGTH == X.size());
                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    sam.reset();
                    // Check what bucket is the key to be erased in.
                    bsl::size_t bucketIdx     = X.bucketIndex(VALUES[v].first);
                    bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

                    bsl::size_t rc = mX.eraseAll(VALUES[v].first);
                    ASSERTV(rc == 2);
                    ASSERTV(2 * (LENGTH - v - 1) == X.size());

                    bsl::size_t newBucketSize = X.bucketSize(bucketIdx);
                    ASSERTV(newBucketSize == oldBucketSize - 2);

                    ASSERTV(sam.isInUseDown());
                    sam.reset();

                    // Try to erase again the already erased key.
                    rc = mX.eraseAll(VALUES[v].first);
                    ASSERTV(rc == 0);
                    ASSERTV(2 * (LENGTH - v - 1) == X.size());
                    ASSERTV(sam.isInUseSame());
                }
                ASSERTV(0 == X.size());
            }
        }
    }

    // This can happen only in "multimap", as we test duplicate keys
    // ("multimap") with eraseFirst.
    {
        if (veryVerbose) cout << endl
                          << "Test 'eraseFirst' against duplicates" << endl;

        const TestValues     VALUES; // contains 52 distinct increasing values
        const bsl::size_t    MAX_LENGTH = 9;
        bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

        // Test erasing duplicate keys.
        for (bsl::size_t ti = 1; ti < MAX_LENGTH; ++ti) {
            const bsl::size_t LENGTH = ti;

            for (bsl::size_t tj = 0; tj < LENGTH; ++tj) {

                // arbitrary number of buckets and stripes
                Obj mX(32, 4, &supplied);  const Obj& X = mX;

                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    mX.insertAlways(VALUES[v].first, VALUES[v].second);
                    mX.insertAlways(VALUES[v].first,
                                    VALUES[LENGTH - v - 1].second);
                }

                bslma::TestAllocatorMonitor sam(&supplied);

                // Erase all elements in a loop, and check that each erase is
                // successful, and that the final number of elements is 0.

                ASSERTV(2 * LENGTH == X.size());
                for (bsl::size_t v = 0; v < LENGTH; ++v) {
                    sam.reset();
                    // Check what bucket is the key to be erased in.
                    bsl::size_t bucketIdx     = X.bucketIndex(VALUES[v].first);
                    bsl::size_t oldBucketSize = X.bucketSize(bucketIdx);

                    // Erase the first copy.
                    bsl::size_t rc = mX.eraseFirst(VALUES[v].first);
                    ASSERTV(rc == 1);
                    ASSERTV(2 * (LENGTH - v - 1) + 1 == X.size());

                    bsl::size_t newBucketSize = X.bucketSize(bucketIdx);
                    ASSERTV(newBucketSize == oldBucketSize - 1);

                    ASSERTV(sam.isInUseDown());
                    sam.reset();

                    // Erase the second copy.
                    rc = mX.eraseFirst(VALUES[v].first);
                    ASSERTV(rc == 1);
                    ASSERTV(2 * (LENGTH - v - 1) == X.size());

                    bsl::size_t veryNewBucketSize = X.bucketSize(bucketIdx);
                    ASSERTV(veryNewBucketSize == oldBucketSize - 2);

                    ASSERTV(sam.isInUseDown());
                    sam.reset();

                    // Try again to erase the previously erased key.
                    rc = mX.eraseFirst(VALUES[v].first);
                    ASSERTV(rc == 0);
                    ASSERTV(2 * (LENGTH - v - 1) == X.size());
                    ASSERTV(sam.isInUseSame());
                }
                ASSERTV(0 == X.size());
            }
        }
    }

    // CONCERN: In no case does memory come from the default allocator.
    ASSERT(dam.isTotalSame());
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void TestDriver<KEY, VALUE, HASH, EQUAL>::testCase6()
{
    // ------------------------------------------------------------------------
    // TEST 'getValue' VECTOR
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
    //:   2 Confirm that all values for the searched key are returned.
    //:
    //: 5 'getValue' is call against a 'const' alias to the object under test.
    //
    // Testing:
    //   bsl::size_t getValue(*valuesVector, key) const;
    // ------------------------------------------------------------------------

    // This is equivalent to 'testCase6' in StripedUnorderedMultiMap'.

    if (verbose) cout << endl
                      << "TEST 'getValue' VECTOR" << endl
                      << "======================" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator da("da", veryVeryVeryVerbose);
    bsl::vector<VALUE>   values(&scratch);

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

        bsl::size_t rc = X.getValue(&values, VALUES[0].first);
        ASSERT(0 == rc);
        ASSERT(sam.isTotalSame());
    }

    // Test retrieving unique keys.
    {
        // Test with one key missing and then inserted.  This also tests that
        // all buckets are covered, as the number of buckets is small compared
        // to the number of values.

        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
            // Small number of buckets
            Obj mX(8, 2, &supplied);  const Obj& X = mX;

            mX.insertUnique(VALUES[ti].first, VALUES[ti].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&values, VALUES[ti].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(values[0], VALUES[ti].second));

            rc = X.getValue(&values, VALUES[ti + 1].first);
            ASSERT(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insertUnique(VALUES[ti + 1].first, VALUES[ti + 1].second);

            sam.reset();

            rc = X.getValue(&values, VALUES[ti + 1].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(values[0], VALUES[ti + 1].second));
            ASSERT(sam.isTotalSame());
        }

        // Test with two keys missing and then inserted.

        for (bsl::size_t ti = 0; ti < MAX_LENGTH - 2; ++ti) {
            // arbitrary number of stripes and buckets
            Obj mX(64, 4, &supplied);  const Obj& X = mX;

            mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
            mX.insertUnique(VALUES[ti + 1].first, VALUES[ti + 1].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&values, VALUES[ti].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(values[0], VALUES[ti].second));

            rc = X.getValue(&values, VALUES[ti + 2].first);
            ASSERT(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insertUnique(VALUES[ti + 2].first, VALUES[ti + 2].second);

            sam.reset();

            rc = X.getValue(&values, VALUES[ti + 2].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(values[0], VALUES[ti + 2].second));
            ASSERT(sam.isTotalSame());
        }
    }

    // Test with duplicate keys, matching value attribute.

    for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
        // arbitrary number of stripes and buckets
        Obj mX(64, 4, &supplied);  const Obj& X = mX;

        mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
        mX.insertAlways(VALUES[ti].first, VALUES[ti].second);

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t rc = X.getValue(&values, VALUES[ti].first);
        ASSERT(2 == rc);
        ASSERT(areEqual(values[0], VALUES[ti].second));
        ASSERT(areEqual(values[1], VALUES[ti].second));

        ASSERT(sam.isTotalSame());
    }

    // Test with duplicate keys, different value attributes

    for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
        // arbitrary number of stripes and buckets
        Obj mX(64, 4, &supplied);  const Obj& X = mX;

        mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
        mX.insertAlways(VALUES[ti].first, VALUES[ti+1].second);

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t rc = X.getValue(&values, VALUES[ti].first);
        ASSERT(2 == rc);
        ASSERT(areEqual(values[0], VALUES[ti].second) ||
               areEqual(values[0], VALUES[ti+1].second));
        ASSERT(areEqual(values[1], VALUES[ti].second) ||
               areEqual(values[1], VALUES[ti+1].second));

        ASSERT(sam.isTotalSame());
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

    // This is similar to 'testCase5' in 'StripedUnordered*Map'.

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

            mX.insertUnique(VALUES[ti].first, VALUES[ti].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&value, VALUES[ti].first);
            ASSERT(1 == rc);
            ASSERT(areEqual(value, VALUES[ti].second));

            rc = X.getValue(&value, VALUES[ti + 1].first);
            ASSERT(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insertUnique(VALUES[ti + 1].first, VALUES[ti + 1].second);

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

            mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
            mX.insertUnique(VALUES[ti + 1].first, VALUES[ti + 1].second);

            bslma::TestAllocatorMonitor sam(&supplied);

            bsl::size_t rc = X.getValue(&value, VALUES[ti].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[ti].second));

            rc = X.getValue(&value, VALUES[ti + 2].first);
            ASSERTV(0 == rc);
            ASSERT(sam.isTotalSame());

            mX.insertUnique(VALUES[ti + 2].first, VALUES[ti + 2].second);

            sam.reset();

            rc = X.getValue(&value, VALUES[ti + 2].first);
            ASSERTV(1 == rc);
            ASSERTV(areEqual(value, VALUES[ti + 2].second));
            ASSERT(sam.isTotalSame());
        }
    }

    // Test with duplicate keys.
    //
    // This test will be replicated for the "multimap" component, but not for
    // the "map" component.

    for (bsl::size_t ti = 0; ti < MAX_LENGTH - 1; ++ti) {
        // arbitrary number of stripes and buckets
        Obj mX(64, 4, &supplied);  const Obj& X = mX;

        // Duplicate key, same value
        mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
        mX.insertAlways(VALUES[ti].first, VALUES[ti].second);

        bslma::TestAllocatorMonitor sam(&supplied);

        bsl::size_t rc = X.getValue(&value, VALUES[ti].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[ti].second));

        ASSERT(sam.isTotalSame());

        // Duplicate key, different value
        mX.insertUnique(VALUES[ti].first, VALUES[ti].second);
        mX.insertAlways(VALUES[ti].first, VALUES[ti+1].second);

        sam.reset();

        rc = X.getValue(&value, VALUES[ti].first);
        ASSERTV(1 == rc);
        ASSERTV(areEqual(value, VALUES[ti].second) ||
                areEqual(value, VALUES[ti+1].second));

        ASSERT(sam.isTotalSame());
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
    //: 4 'bucket_size': Verify that, in appropriate build modes, defensive
    //:    checks are triggered for invalid indexes, but not triggered for
    //:    adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
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

    // This is identical to 'testCase4' in 'StripedUnordered*Map'.

    if (verbose) cout << endl
                      << "BASIC ACCESSORS" << endl
                      << "===============" << endl;

    if (veryVeryVerbose) {
        cout << "KEY  " << ": " << bsls::NameOf<KEY>()   << "\n"
             << "VALUE" << ": " << bsls::NameOf<VALUE>() << "\n"
             << "HASH " << ": " << bsls::NameOf<HASH>()  << "\n"
             << "EQUAL" << ": " << bsls::NameOf<EQUAL>() << endl;
    }

    // Note that the allocating types do allocate from the default allocator.
    const KEY   cKEY   = TstFacility::create<KEY  >(0);
    const VALUE cVALUE = TstFacility::create<VALUE>(0);

    // CONCERN: In no case does memory come from the default allocator.
    bslma::TestAllocatorMonitor dam(&defaultAllocator);

    bslma::TestAllocator supplied("supplied", veryVeryVeryVerbose);

    // arbitrary number of stripes and buckets
    Obj mX(128, 32, &supplied);  const Obj& X = mX;

    bsl::size_t rc = mX.insertUnique(cKEY, cVALUE);
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

        mX.insertUnique(cKEY, cVALUE);

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
    //:     existing elements of the hash map and the 'multiplicity' argument.
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
    //   void insertAlways(const KEY& key, const VALUE& value);
    //   bsl::size_t insertUnique(const KEY& key, const VALUE& value);
    // ------------------------------------------------------------------------

    // This is similar to 'testCase3' in 'StripedUnordered*Map'.

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
                char        d_multiplicity; // 'U' (unique) or 'A' (always)

                bsl::size_t d_expRc;
                bsl::size_t d_expSize;
                bsl::size_t d_expBucketIndex;
                bsl::size_t d_expBucketSize;
            } INSERTS0[] = {
                //LINE KEY VAL MULT EXP_RC EXP_SIZE EXP_BIDX EXP_BSIZE
                //---- --- --- ---- ------ -------- -------- ---------
                { L_,    0, 10, 'U',    1,        1,       0,        1 }
              , { L_,    0, 10, 'A',    1,        1,       0,        1 }
              , { L_,    1, 11, 'U',    1,        1,       1,        1 }
              , { L_,    1, 11, 'A',    1,        1,       1,        1 }
            };
            const bsl::size_t NUM_INSERTS0 = sizeof  INSERTS0
                                           / sizeof *INSERTS0;

            for (bsl::size_t tj = 0; tj < NUM_INSERTS0; ++tj) {
                const int   LINE      = INSERTS0[tj].d_line;
                const int   keyCode   = INSERTS0[tj].d_key;
                const int   valueCode = INSERTS0[tj].d_value;
                const KEY   cKEY      = TstFacility::create<KEY  >(  keyCode);
                const VALUE cVALUE    = TstFacility::create<VALUE>(valueCode);
                dam.reset(); // 'TstFacility::create' allocates from the
                             // default allocator.

                (void)LINE;

                bool isUnique = INSERTS0[tj].d_multiplicity == 'U';

                const bsl::size_t  EXP_RC     = INSERTS0[tj].d_expRc;
                const bsl::size_t  EXP_SIZE   = INSERTS0[tj].d_expSize;
                const bsl::size_t  EXP_BSIZE  = INSERTS0[tj].d_expBucketSize;

                const bsl::size_t  EXP_BIDX   = INSERTS0[tj].d_key
                                              % NUM_BUCKETS;


                if (veryVeryVerbose) {

                    const char *MULT_LABEL = isUnique ? "UNIQUE" : "ALWAYS";
                    P_(keyCode) P_(valueCode) P(MULT_LABEL)
                }

                const bool *const vvvV = &veryVeryVeryVerbose;

                bslma::TestAllocator         sa("supplied", *vvvV);
                bslma::TestAllocatorMonitor  sam(&sa);
                bslma::TestAllocator         da("default",  *vvvV);
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

                    if (isUnique) {
                        bsl::size_t rc =  mX.insertUnique(cKEY, cVALUE);
                        ASSERT(EXP_RC == rc);
                    } else {
                         mX.insertAlways(cKEY, cVALUE);
                    }

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

                    bsl::vector<VALUE> values(&scratch);
                    bsl::size_t        rc = X.getValue(&values, cKEY);
                    ASSERT(numElements == rc);
                    ASSERT(areEqual(values[0] , cVALUE));

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
                char        d_multiplicity; // 'U' (unique) or 'A' (always)

                bsl::size_t d_expRc;
                bsl::size_t d_expSize;
                bsl::size_t d_expCountValue1;
                bsl::size_t d_expCountValue2;
            } INSERTS1[] = {
                //LINE OBJ  KEY  VAL  MULT EXP_RC EXP_SIZE #V1 #V2
                //---- ---  ---  ---  ---- ------ -------- --- ---
                { L_,  'A',  KA, VA1,  'U',     0,       1,  1,  0 }
              , { L_,  'A',  KA, VA2,  'U',     0,       1,  0,  1 }
              , { L_,  'B',  KB, VB1,  'U',     0,       1,  1,  0 }
              , { L_,  'B',  KB, VB2,  'U',     0,       1,  0,  1 }
              , { L_,  'A',  KA, VA1,  'A',     1,       2,  2,  0 }
              , { L_,  'A',  KA, VA2,  'A',     1,       2,  1,  1 }
              , { L_,  'B',  KB, VB1,  'A',     1,       2,  2,  0 }
              , { L_,  'B',  KB, VB2,  'A',     1,       2,  1,  1 }
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
                dam.reset(); // 'TstFacility::create' allocates from the
                             // default allocator.

                bool isUnique = INSERTS1[tj].d_multiplicity == 'U';

                const bsl::size_t  EXP_RC    = INSERTS1[tj].d_expRc;
                const bsl::size_t  EXP_SIZE  = INSERTS1[tj].d_expSize;

                const bsl::size_t  EXP_V1COUNT = INSERTS1[tj].d_expCountValue1;
                const bsl::size_t  EXP_V2COUNT = INSERTS1[tj].d_expCountValue2;

                const bsl::size_t  EXP_BIDX  = INSERTS1[tj].d_key
                                             % NUM_BUCKETS;
                const bsl::size_t  EXP_BSIZE = EXP_RC + 1;

                if (veryVeryVerbose) {
                    const char *MULT_LABEL = isUnique ? "UNIQUE" : "ALWAYS";
                    P_(keyCode) P_(valueCode) P(MULT_LABEL)
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
                    const VALUE cVALUE2A = TstFacility::create<VALUE>(VA2);
                    const VALUE cVALUE2B = TstFacility::create<VALUE>(VB2);

                    mA.insertUnique(cKEY_A, cVALUE1A);
                    mB.insertUnique(cKEY_B, cVALUE1B);

                    Obj  *objPtr = OBJ == 'A' ? &mA : &mB;
                    Obj&  mX     = *objPtr;  const Obj& X = mX;

                    // CREATE ARGUMENTS;
                    const VALUE  VALUE1 = OBJ == 'A' ? cVALUE1A : cVALUE1B;
                    const VALUE  VALUE2 = OBJ == 'A' ? cVALUE2A : cVALUE2B;

                    // INSERT TEST
                    bslma::TestAllocatorMonitor dam1(&da);

                    if (isUnique) {
                        bsl::size_t rc =  mX.insertUnique(cKEY, cVALUE);
                        ASSERT(EXP_RC == rc);
                    } else {
                         mX.insertAlways(cKEY, cVALUE);
                    }

                    // ACCESS RESULTS
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

                    bsl::vector<VALUE> values(&scratch);
                    bsl::size_t        rc = X.getValue(&values, cKEY);

                    ASSERT(numElements == rc);

                    bsl::size_t V1COUNT = 0;
                    bsl::size_t V2COUNT = 0;
                    for (typename bsl::vector<VALUE>::const_iterator
                                                    itr  = values.begin(),
                                                    end  = values.end();
                                                    end != itr; ++itr) {
                        if (areEqual(*itr, VALUE1)) {
                            ++V1COUNT;
                        }
                        if (areEqual(*itr, VALUE2)) {
                            ++V2COUNT;
                        }
                    }

                    if (veryVeryVerbose) {
                        P(LINE)
                        P_(VALUE1) P_(EXP_V1COUNT) P(V1COUNT)
                        P_(VALUE2) P_(EXP_V2COUNT) P(V2COUNT)
                    }

                    ASSERT(EXP_V1COUNT == V1COUNT);
                    ASSERT(EXP_V2COUNT == V2COUNT);

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
                char d_multiplicity; // 'U' (unique) or 'A' (always)

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

            // UNIQUE INSERTS

            //LINE E1         E2       KEY  VAL MULT  RC SZ "Sorted" Results
            //---- --------  --------  ---  --- ----  -- -- ----------------
            // Initially, two identical elements in same bucket.
            { L_,  K1A, V1A, K1A, V1A, K1A, V1A, 'U',  0, 2,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1A, V2A, 'U',  0, 2,
                                                 {
                                                     { K1A, { V1A, V2A, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }

                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K2A, V2A, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1B, V1B, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            // Initially two different elements in same bucket.
            { L_,  K1A, V1A, K2A, V2A, K1A, V1A, 'U',  0, 2,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1A, V2A, 'U',  0, 2,
                                                 {
                                                     { K1A, { V2A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K3A, V3A, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K3A, { V3A, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1B, V1B, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            // Initially two different elements in different buckets.
            { L_,  K1A, V1A, K1B, V1B, K1A, V1A, 'U',  0, 2,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K1A, V2A, 'U',  0, 3,
                                                 {
                                                     { K1A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2A, V2A, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2B, V2B, 'U',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { K2B, { V2B, nnn, nnn } }
                                                 }},

            // INSERT ALWAYS

            // Initially, two identical elements in same bucket.
            { L_,  K1A, V1A, K1A, V1A, K1A, V1A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, V1A } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1A, V2A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, V2A } }
                                                   , { nnn, { nnn, nnn, nnn } }

                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K2A, V2A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1A, V1A, K1B, V1B, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            // Initially two different elements in same bucket.
            { L_,  K1A, V1A, K2A, V2A, K1A, V1A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1A, V2A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V2A, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K3A, V3A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K3A, { V3A, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K2A, V2A, K1B, V1B, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            // Initially two different elements in different buckets.
            { L_,  K1A, V1A, K1B, V1B, K1A, V1A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V1A, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K1A, V2A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, V2A, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                   , { nnn, { nnn, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2A, V2A, 'A',  1, 3,
                                                 {
                                                     { K1A, { V1A, nnn, nnn } }
                                                   , { K2A, { V2A, nnn, nnn } }
                                                   , { K1B, { V1B, nnn, nnn } }
                                                 }},
            { L_,  K1A, V1A, K1B, V1B, K2B, V2B, 'A',  1, 3,
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
                const char        IMULT       = INSERTS2[tj].d_multiplicity;

                if (veryVerbose) {
                    T_ P(LINE)
                    T_ P_(E1KEY) P(E1VALUE)
                    T_ P_(E2KEY) P(E2VALUE)
                    T_ P_(IKEY)  P_(IVALUE) P(IMULT)
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
                    dam.reset(); // 'TstFacility::create' allocates from the
                                 // default allocator.

                    mX.insertAlways(E1_KEY, E1_VALUE);
                    mX.insertAlways(E2_KEY, E2_VALUE);

                    ASSERT(2 == X.size());

                    bool isUnique = IMULT == 'U';

                    // Test 'insert'
                    bslma::TestAllocatorMonitor dam1(&da);

                    if (isUnique) {
                        bsl::size_t rc =  mX.insertUnique(I_KEY, I_VALUE);
                        ASSERT(EXP_RC == rc);
                    } else {
                         mX.insertAlways(I_KEY, I_VALUE);
                    }

                    // Assess Results
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
                        dam.reset(); // 'TstFacility::create' allocates from
                                     // the default allocator.

                        bsl::vector<VALUE> foundValues(&scratch);
                        bsl::size_t        rc = X.getValue(&foundValues, cKEY);
                        ASSERT(rc == foundValues.size());

                        // Convert returned values to 'int' representation.

                        bsl::vector<int> foundValuesAsInt(&scratch);

                        for (typename bsl::vector<VALUE>::const_iterator
                                                    itr  = foundValues.begin(),
                                                    end  = foundValues.end();
                                                    end != itr; ++itr) {
                            int asInt = TstFacility::getIdentifier(*itr);
                            foundValuesAsInt.push_back(asInt);

                            if (veryVeryVerbose) {
                                cout << "Found   : " << asInt << " ";
                            }
                        }
                        if (veryVeryVerbose)  cout << endl;

                        bsl::sort(foundValuesAsInt.begin(),
                                  foundValuesAsInt.end());

                        // Compare found values to expected values.

                        ASSERTV(key,
                                EXP_VALUES.size(),
                                foundValuesAsInt.size(),
                                EXP_VALUES.size() == foundValuesAsInt.size());
                        ASSERT(EXP_VALUES == foundValuesAsInt);
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
    //   StripedUnorderedContainerImpl(numInitialBuckets, numStripes, *ba);
    //   ~StripedUnorderedContainerImpl();
    // ------------------------------------------------------------------------

    // This is identical to 'testCase2' in 'StripedUnordered*Map'.

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

        mX.insertUnique(TstFacility::create<KEY>(1),
                        TstFacility::create<VALUE>(11));
        mX.insertUnique(TstFacility::create<KEY>(2),
                        TstFacility::create<VALUE>(22));
        mX.insertUnique(TstFacility::create<KEY>(3),
                        TstFacility::create<VALUE>(33));
        mX.insertUnique(TstFacility::create<KEY>(4),
                        TstFacility::create<VALUE>(44));
        mX.insertUnique(TstFacility::create<KEY>(5),
                        TstFacility::create<VALUE>(55));

        ASSERTV(X.size(), X.size() == 5);

        {
            VALUE       value;
            bsl::size_t rc = X.getValue(&value, TstFacility::create<KEY>(1));
            ASSERTV(rc,     1 == rc);
            ASSERTV(value, 11 == TstFacility::getIdentifier(value));
        };

        {
            VALUE       value;
            bsl::size_t rc = X.getValue(&value, TstFacility::create<KEY>(3));
            ASSERTV(rc,     1 == rc);
            ASSERTV(value, 33 == TstFacility::getIdentifier(value));
        };
    }

    const TestValues VALUES;  // contains 52 distinct increasing values

    {
        Obj mX(1024, 16);  const Obj& X = mX;

        mX.insertUnique(VALUES[0].first, VALUES[0].second);
        mX.insertUnique(VALUES[1].first, VALUES[1].second);
        mX.insertUnique(VALUES[2].first, VALUES[2].second);
        mX.insertUnique(VALUES[3].first, VALUES[3].second);
        mX.insertUnique(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insertUnique(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 6);
    }

    {
        Obj mX(1024, 16);  const Obj& X = mX;

        mX.insertUnique(VALUES[0].first, VALUES[0].second);
        mX.insertUnique(VALUES[1].first, VALUES[1].second);
        mX.insertUnique(VALUES[2].first, VALUES[2].second);
        mX.insertUnique(VALUES[3].first, VALUES[3].second);

        {
            VALUE       value;
            bsl::size_t rc = X.getValue(&value, VALUES[1].first);
            ASSERTV(rc,    1 == rc);
            ASSERTV(value, areEqual(VALUES[1].second, value));
        };

        mX.insertUnique(VALUES[4].first, VALUES[4].second);

        ASSERTV(X.size(), X.size() == 5);

        mX.insertUnique(VALUES[5].first, VALUES[5].second);

        ASSERTV(X.size(), X.size() == 6);
    }
}

}  // close unnamed namespace

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
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
      // BDE_VERIFY pragma: -TP05 Defined in the various test functions
      case 21: {
        threaded::threadedTest1();
      } break;
      case 20: {
        testLock::testLocking();
      } break;
      case 19: {
        testLock::testTestingUtil();
      } break;
      case 18: {
        RUN_EACH_TYPE(TestDriver, testCase18, bsltf::MovableAllocTestType);
      } break;
      case 17: {
        RUN_EACH_TYPE(TestDriver, testCase17, bsltf::MovableAllocTestType);
      } break;
      case 16: {
        RUN_EACH_TYPE(TestDriver, testCase16, TEST_TYPES_REGULAR);
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
        TestDriver<
            bsltf::AllocTestType,
            bsltf::NonCopyConstructibleTestType>::testCase12_noncopyable();
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
      // BDE_VERIFY pragma: +TP05
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    // BDE_VERIFY pragma: +TP17

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
