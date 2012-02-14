// bteso_ipresolutioncache.t.cpp                                      -*-C++-*-
#include <bteso_ipresolutioncache.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>
#include <bsls_stopwatch.h>

#include <bdef_bind.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>

#include <bcep_fixedthreadpool.h>

// #include <bces_atomictypes.h>

#include <bcemt_lockguard.h>
#include <bcemt_threadattributes.h>
#include <bcemt_threadutil.h>
#include <bcemt_once.h>
#include <bcemt_barrier.h>

#include <bcema_testallocator.h>

#include <bsl_climits.h>     // 'INT_MIN', 'INT_MAX'
#include <bsls_asserttest.h>
#include <bsls_byteorder.h>

#include <bsls_platform.h>                    // for testing only

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This componenet implements a mechanism for storing resolved IP
// address.  The test driver will test the component in both a single threaded
// adn multithreaded environmenet.
//
// ----------------------------------------------------------------------------
// class bteso_IpResolutionCache
//
// CREATORS
// [ 6] bteso_IpResolutionCache(bslma_Allocator *bA);
// [ 7] bteso_IpResolutionCache(Callback cb, bslma_Allocator *bA);
// [ 6] ~bteso_IpResolutionCache();
//
// MANIPULATORS
// [10] void removeAll();
// [ 8] int resolveAddress(vector *r, const char *h, int n, int *e);
// [ 6] void setTimeToLive(const bdet_DatetimeInterval& value);
//
// ACCESSORS
// [ 7] bslma_Allocator *allocator() const;
// [ 8] int lookupAddressRaw(vector *res, const char *host, int num);
// [ 7] ResolveByNameCallback resolverCallback();
// [ 7] const bdet_DatetimeInterval& timeToLive();
// ----------------------------------------------------------------------------
// class bteso_IpResolutionCache_Entry
//
// CREATORS
// [ 3] bteso_IpResolutionCache_Entry();
// [ 4] bteso_IpResolutionCache_Entry(const IpResolutionCache_Entry& orig);
// [ 3] ~bteso_IpResolutionCache_Entry();
//
// MANIPULATORS
// [ 3] void setData(DataPtr value);
// [ 3] void reset();
//
// ACCESSORS
// [ 4] DataPtr data();
// [ 5] bcemt_Mutex& updatingLock();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ 2] CONCERN: Test apparatus is working as expected.
// [ 9] CONCERN: bteso_IpResolutionCache works with multiple threads
// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {

class bteso_IpResolutionCache_Data {
    // Duplicate of the 'bteso_IpResolutionCache_Data' class declaration
    // defined in 'bteso_ipresolutioncache.cpp'.
    //
    // This class provides storage for a set of IP addresses and a
    // 'bdet_Datetime' to indicate the time these addresses were populated.

    // DATA
    bsl::vector<bteso_IPv4Address> d_addresses;     // set of IP addresses

    bdet_Datetime                  d_creationTime;  // time at which this
                                                    // object is created
  private:
    // NOT IMPLEMENTED
    bteso_IpResolutionCache_Data(const bteso_IpResolutionCache_Data&);
    bteso_IpResolutionCache_Data& operator=(
                                          const bteso_IpResolutionCache_Data&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bteso_IpResolutionCache,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATOR
    bteso_IpResolutionCache_Data(
                    const bsl::vector<bteso_IPv4Address>&  ipAddresses,
                    const bdet_Datetime&                   creationTime,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create an object storing the specified 'ipAddresses', and having the
        // specified 'creationTime'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // ACCESSORS
    const bsl::vector<bteso_IPv4Address>& addresses() const;
        // Return a reference providing non-modifiable access to the IP
        // addresses stored in this object.

    const bdet_Datetime& creationTime() const;
        // Return a reference providing non-modifiable access to the time this
        // object was created.
};

}

typedef bteso_IpResolutionCache                Obj;
typedef bteso_IpResolutionCache_Entry          Entry;
typedef bteso_IpResolutionCache_Data           Data;
typedef bteso_IPv4Address                      IPv4;
typedef bsl::vector<bteso_IPv4Address>         Vec;
typedef bteso_IpResolutionCache_Entry::DataPtr DataPtr;

struct ThreadInfo {
    bcemt_Mutex    *d_lock;
    bces_AtomicInt  d_retval;
    bces_AtomicInt  d_retvalSet;
};

enum {
    // Options for performance test.
    DEFAULT,
    CACHE
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Example 2: Using Address Cache with 'bteso_ResolveUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to configure the 'bteso_ResolverUtil'
// component to use a 'bteso_IpResolutionCache' object for resolving IP
// addresses.
//
// In order to use a 'bteso_IpResolutionCache' as the resolution callback in
// 'bteso_ResolveUtil', we must wrap the call to
// 'bteso_IpResolutionCache::resolveAddress' in a free function.
//
// When configuring 'bteso_ResolveUtil', a singleton cache should be created to
// ensure the cache exist for all calls to 'bteso_ResolveUtil::getAddress'.
// First, we create a function that initializes the singleton cache on the
// first execution and returns the address of the cache:
//..
    static
    bteso_IpResolutionCache *ipCacheInstance()
    {
        static bteso_IpResolutionCache *singletonCachePtr = 0;
        BCEMT_ONCE_DO {
            if (0 == singletonCachePtr) {
                bslma_Allocator *allocator = bslma_Default::globalAllocator();
                static bteso_IpResolutionCache cache(allocator);
                singletonCachePtr = &cache;
            }
        }
        return singletonCachePtr;
    }
//..
// Then, we create a free function to wrap the
// 'bteso_IpResolutionCache::resolveAddress' method:
//..
    static
    int resolverCallback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                         const char                     *hostName,
                         int                             numAddresses,
                         int                            *errorCode)
    {
        return ipCacheInstance()->resolveAddress(hostAddresses,
                                                 hostName,
                                                 numAddresses,
                                                 errorCode);
    }
//..

// ============================================================================
//                     GLOBAL CLASSES USED FOR TESTING
// ----------------------------------------------------------------------------

                         // ============
                         // TestResolver
                         // ============

class TestResolver {
    // This class provide a mechanism to resolve IP addresses given a host name
    // for testing purposes.  The returned IP addresses depends on the number
    // of times the callback is invoked.

    // DATA
    static int s_count;
    static int s_errorCode;
    static int s_numAddresses;

  private:
    // NOT IMPLEMENTED
    TestResolver();
    TestResolver(const TestResolver&);
    const TestResolver& operator=(const TestResolver&);

  public:
    // CLASS METHOD
    static int callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                        const char                     *hostName,
                        int                             maxNumAddresses,
                        int                            *errorCode);
        // Load, into the specified 'hostAddresses', either the specified
        // 'maxNumAddresses' or 's_numAddresses' (whichever is less) number of
        // IP addresses based on 's_count', regardless of the specified
        // 'hostname'.  Load into the specified 'errorCode', 's_errorCode' if
        // 's_errorCode' is non-zero.  Return 's_errorCode'.

    static int count();
        // Return the number of times 'callback' has been invoked since the
        // start of the program or the last time 'reset' was invoked.

    static int errorCode();
        // Return the error code that will be returned by the 'callback'
        // method.

    static bteso_IPv4Address lastIPv4Added();
        // Return the last IP addresses that was returned by the 'callback'
        // method.

    static int numAddresses();
        // Return the number of IP addresses the 'callback' method will attempt
        // to return.

    static void reset();
        // Reset 'TestResolver' to its default state ('s_count == 0' and
        // 's_numAddresses == 0')

    static void setCount(int value);
        // Set the value of 's_count' to the specified value.

    static void setErrorCode(int value);
        // Set the error code to return by the 'callback' method to the
        // specified value.

    static void setNumAddresses(int value);
        // Set the number of IP addresses the 'callback' method will attempt
        // to return to the specified value.

};

                         // ------------
                         // TestResolver
                         // ------------


int TestResolver::s_count        = 0;
int TestResolver::s_errorCode    = 0;
int TestResolver::s_numAddresses = 1;

int TestResolver::callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                           const char                     *hostName,
                           int                             maxNumAddresses,
                           int                            *errorCode)
{
    if (s_errorCode) {
        // Simulate an error.

        if (errorCode) {
            *errorCode = s_errorCode;
        }
        return s_errorCode;                                           // RETURN
    }
    ++s_count;

    hostAddresses->clear();

    int size = bsl::min(s_numAddresses, maxNumAddresses);
    for (int i = 0; i < size; ++i) {
        hostAddresses->push_back(
                          bteso_IPv4Address(BSLS_BYTEORDER_HTONL(s_count), 0));
    }

    (void) hostName;  // quash potential compiler warning
    return 0;
}

int TestResolver::count()
{
    return s_count;
}

int TestResolver::errorCode()
{
    return s_errorCode;
}

bteso_IPv4Address TestResolver::lastIPv4Added()
{
    return bteso_IPv4Address(BSLS_BYTEORDER_HTONL(s_count), 0);
}

int TestResolver::numAddresses()
{
    return s_numAddresses;
}

void TestResolver::reset()
{
    s_count        = 0;
    s_numAddresses = 1;
}

void TestResolver::setCount(int value)
{
    s_count = value;
}

void TestResolver::setErrorCode(int value)
{
    s_errorCode = value;
}

void TestResolver::setNumAddresses(int value)
{
    s_numAddresses = value;
}

// ============================================================================
//                     GLOBAL FUNCTIONS USED FOR TESTING
// ----------------------------------------------------------------------------
static
void performanceTest(bcemt_Mutex    *updateMutex,
                     double         *totalSystemTime,
                     double         *totalUserTime,
                     double         *totalWallTime,
                     vector<string> *hostname,
                     int            *option,
                     int             numCalls)
    // Function for testing performance 'bteso_IpResolutionCache'.
{
    bteso_IPv4Address address;

    bsls_Stopwatch timer;
    timer.start();

    for (vector<string>::const_iterator it = hostname->begin();
                                        it != hostname->end();
                                        ++it) {
        switch (*option) {
          case DEFAULT: {
            for (int i = 0; i < numCalls; ++i) {
                int rc = bteso_ResolveUtil::getAddressDefault(&address,
                                                              it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
          case CACHE: {
            for (int i = 0; i < numCalls; ++i) {
                int rc = bteso_ResolveUtil::getAddress(&address,
                                                       it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
        }
    }
    timer.stop();

    bcemt_LockGuard<bcemt_Mutex> guard(updateMutex);

    *totalSystemTime += timer.accumulatedSystemTime();
    *totalUserTime   += timer.accumulatedUserTime();
    *totalWallTime   += timer.accumulatedWallTime();
}

extern "C" void* MyThread(void* arg_p)
    // Function for testing lock mechanism.
{
    ThreadInfo* arg = (ThreadInfo*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

//=============================================================================
//                      CONCURRENCY CONCERNS RELATED ENTRIES
//-----------------------------------------------------------------------------

void executeInParallel(int                               numThreads,
                       bcemt_ThreadUtil::ThreadFunction  func,
                       void                             *threadArgs)
   // Create the specified 'numThreads', each executing the specified 'func'
   // on the specified 'threadArgs'.
{
    bcemt_ThreadUtil::Handle *threads =
                                      new bcemt_ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    int rc;
    for (int i = 0; i < numThreads; ++i) {
        rc = bcemt_ThreadUtil::create(&threads[i], func, threadArgs);
        LOOP_ASSERT(i, !rc);
    }
    for (int i = 0; i < numThreads; ++i) {
        rc = bcemt_ThreadUtil::join(threads[i]);
        LOOP_ASSERT(i, !rc);
    }

    delete [] threads;
}

namespace BTESO_IPRESOLUTIONCACHE_CONCURRENCY {

struct IpAddressData {
        int         d_line;       // line number
        const char *d_hostname;   // hostname
        int         d_ipAddress;  // integer value of IP address
} DATA[] = {

    //LINE HOST     IP

    { L_,  "ID_A",   1 },
    { L_,  "ID_B",   2 },
    { L_,  "ID_C",   3 },
    { L_,  "ID_D",   4 },
    { L_,  "ID_E",   5 },
    { L_,  "ID_F",   6 },
    { L_,  "ID_G",   7 },
    { L_,  "ID_H",   8 },
    { L_,  "ID_I",   9 },
    { L_,  "ID_J",  10 },
    { L_,  "ID_K",  11 },
    { L_,  "ID_L",  12 },
    { L_,  "ID_M",  13 },
    { L_,  "ID_N",  14 }

};
const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

struct ThreadData {
    Obj           *d_cache_p;    // cache under test
    bcemt_Barrier *d_barrier_p;  // testing barrier
};

int testConcurrencyCallback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                            const char                     *hostName,
                            int                             maxNumAddresses,
                            int                            *errorCode)
    // Load, into the specified 'hostAddresses', the IP address based on the
    // specified 'hostName'.  Load into the specified 'errorCode', a value of 0
    // if 'errorCode' is not null.  The behavior is undefined unless
    // 'maxNumAddresses >= 1'.
{
    BSLS_ASSERT(hostAddresses);
    BSLS_ASSERT(hostName);
    BSLS_ASSERT(1 <= maxNumAddresses);

    hostAddresses->clear();

    for (int i = 0; i < NUM_DATA; ++i) {
        if (0 == strcmp(DATA[i].d_hostname, hostName)) {
            hostAddresses->push_back(
              bteso_IPv4Address(BSLS_BYTEORDER_HTONL(DATA[i].d_ipAddress), 0));
            break;
        }
    }

    if (errorCode) {
        *errorCode = 0;
    }

    return 0;
}

extern "C" void *workerThread(void *arg)
{
    ThreadData *p = (ThreadData*)arg;
    bcemt_Barrier& barrier = *p->d_barrier_p;

    Obj &mX = *p->d_cache_p;

    // Begin the test.

    barrier.wait();

    for (int testRun = 0; testRun < 50; ++testRun) {
        for (int i = 0; i < NUM_DATA; ++i) {
            const char              *ID = DATA[i].d_hostname;
            const bteso_IPv4Address  IP = bteso_IPv4Address(
                                     BSLS_BYTEORDER_HTONL(DATA[i].d_ipAddress),
                                     0);
            bsl::vector<bteso_IPv4Address> result;
            mX.resolveAddress(&result, ID, 1);
            ASSERT(1  == result.size());
            ASSERT(IP == result[0]);
        }
    }

    bcemt_ThreadUtil::microSleep(100000);

    for (int testRun = 0; testRun < 50; ++testRun) {
        for (int i = 0; i < NUM_DATA; ++i) {
            const char              *ID = DATA[i].d_hostname;
            const bteso_IPv4Address  IP = bteso_IPv4Address(
                                     BSLS_BYTEORDER_HTONL(DATA[i].d_ipAddress),
                                     0);
            bsl::vector<bteso_IPv4Address> result;
            mX.resolveAddress(&result, ID, 1);
            ASSERT(IP == result[0]);
        }
    }

    mX.removeAll();

    for (int testRun = 0; testRun < 50; ++testRun) {
        for (int i = 0; i < NUM_DATA; ++i) {
            const char              *ID = DATA[i].d_hostname;
            const bteso_IPv4Address  IP = bteso_IPv4Address(
                                     BSLS_BYTEORDER_HTONL(DATA[i].d_ipAddress),
                                     0);
            bsl::vector<bteso_IPv4Address> result;
            mX.resolveAddress(&result, ID, 1);
            ASSERT(IP == result[0]);
        }
    }
    barrier.wait();
    return 0;
}

}

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------

class TestAllocatorMonitor {
    // TBD

    // DATA
    int                              d_lastInUse;
    int                              d_lastMax;
    int                              d_lastTotal;
    const bslma_TestAllocator *const d_allocator_p;

  public:
    // CREATORS
    TestAllocatorMonitor(const bslma_TestAllocator& basicAllocator);
        // TBD

    ~TestAllocatorMonitor();
        // TBD

    // ACCESSORS
    bool isInUseSame() const;
        // TBD

    bool isInUseUp() const;
        // TBD

    bool isMaxSame() const;
        // TBD

    bool isMaxUp() const;
        // TBD

    bool isTotalSame() const;
        // TBD

    bool isTotalUp() const;
        // TBD
};

// CREATORS
inline
TestAllocatorMonitor::TestAllocatorMonitor(
                                     const bslma_TestAllocator& basicAllocator)
: d_lastInUse(basicAllocator.numBlocksInUse())
, d_lastMax(basicAllocator.numBlocksMax())
, d_lastTotal(basicAllocator.numBlocksTotal())
, d_allocator_p(&basicAllocator)
{
}

inline
TestAllocatorMonitor::~TestAllocatorMonitor()
{
}

// ACCESSORS
inline
bool TestAllocatorMonitor::isInUseSame() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() == d_lastInUse;
}

inline
bool TestAllocatorMonitor::isInUseUp() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() != d_lastInUse;
}

inline
bool TestAllocatorMonitor::isMaxSame() const
{
    return d_allocator_p->numBlocksMax() == d_lastMax;
}

inline
bool TestAllocatorMonitor::isMaxUp() const
{
    return d_allocator_p->numBlocksMax() != d_lastMax;
}

inline
bool TestAllocatorMonitor::isTotalSame() const
{
    return d_allocator_p->numBlocksTotal() == d_lastTotal;
}

inline
bool TestAllocatorMonitor::isTotalUp() const
{
    return d_allocator_p->numBlocksTotal() != d_lastTotal;
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bteso_IpResolutionCache IpResolutionCache(&scratch);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
{
///Usage
///-----
// In this section, we show intended usage of this component.
//
///Example 1: Retrieving the IPv4 Addresses of a Given Host
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'bteso_IpResolutionCache', and use it to
// retrieve the IPv4 addresses of several host machines.
//
// First, we create a 'bteso_IpResolutionCache' object.  By default, it will
// use 'bteso_ResolveUtil::defaultResolveByNameCallback' to retrieve addresses
// that are not in the cache:
//..
    bteso_IpResolutionCache cache;
//..
// Then, we verify the newly constructed cache does not contain the addresses
// of either "www.bloomberg.com" and "www.businessweek.com":
//..
    bsl::vector<bteso_IPv4Address> ipAddresses;
    ASSERT(0 != cache.lookupAddressRaw(&ipAddresses, "www.bloomberg.com", 1));
    ASSERT(0 != cache.lookupAddressRaw(&ipAddresses,
                                       "www.businessweek.com",
                                       1));
//..
// Next, we call the 'resolveAddress' method to retrieve one of the IPv4
// address for "www.bloomberg.com" and print it out.  Since this is the first
// call to 'resolveAddress', 'resolverCallback' will be invoked to retrieve the
// addresses:
//..
    int rc = cache.resolveAddress(&ipAddresses, "www.bloomberg.com", 1);
#ifndef BSLS_PLATFORM__OS_WINDOWS
    // The Windows test machine is not configured resolve external domain name
    // properly.  Disable the ASSERT test to prevent test failure.

    ASSERT(0 == rc);
    ASSERT(1 == ipAddresses.size());
    if (verbose) bsl::cout << "IP Address: " << ipAddresses[0] << std::endl;
#endif
//..
//  Finally, we verify that subsequent call to 'lookupAddressRaw' return 0 to
//  indicate "www.bloomberg.com" is stored in the cache, but not
//  "www.businessweek.com":
//..
#ifndef BSLS_PLATFORM__OS_WINDOWS
    ASSERT(0 == cache.lookupAddressRaw(&ipAddresses, "www.bloomberg.com", 1));
    ASSERT(0 != cache.lookupAddressRaw(&ipAddresses,
                                       "www.businessweek.com",
                                       1));
#endif
//..
}

{
// Now, we set the callback for 'bteso_ResolveUtil' to the free function we
// just created:
//..
    bteso_ResolveUtil::setResolveByNameCallback(&resolverCallback);
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'www.bloomberg.com':
//..
    bteso_IPv4Address ipAddress;
    bteso_ResolveUtil::getAddress(&ipAddress, "www.bloomberg.com");
//..
// Now, we write the address to stdout:
//..
#ifndef BSLS_PLATFORM__OS_WINDOWS
    if (verbose) bsl::cout << "IP Address: " << ipAddress << std::endl;
#endif
//..
// Finally, we observe the output to be in the form:
//..
//  IP Address: 63.85.36.34:0
//..
}
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // MANIPULATORS 'removeAll'
        //
        // Concerns:
        //: 1 'removeAll' removes the data in the cache such that they are
        //:   inaccessible with the 'lookupAddressRaw' method
        //
        // Plan:
        //: 1 Insert a set of data into the cache.
        //:
        //: 2 Call 'removeAll' and verify that the data has been removed.
        //:   (C-1)
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) { cout << "Testing 'resolveAddress'" << endl; }
        {
            static const struct {
                int         d_line;
                const char *d_name;
                const char *d_ip;
            } DATA[] = {
                { L_,  "A", "0.0.0.1" },
                { L_,  "B", "0.0.0.2" },
                { L_,  "C", "0.0.0.3" },
                { L_,  "D", "0.0.0.4" },
                { L_,  "E", "0.0.0.5" },
                { L_,  "F", "0.0.0.6" },
                { L_,  "G", "0.0.0.7" },
                { L_,  "H", "0.0.0.8" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int count = 0;
            ASSERT(TestResolver::count() == count);

            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj mX(&TestResolver::callback, &oa); Obj &X = mX;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const NAME     = DATA[ti].d_name;
                const IPv4        ADDR(DATA[ti].d_ip, 0);

                Vec mV;
                int err;

                if (verbose) { P_(LINE) P_(NAME) P(ADDR); }

                LOOP2_ASSERT(LINE, NAME,
                             0 == X.resolveAddress(&mV, NAME, 1, &err));

                LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));
            }

            mX.removeAll();

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const char *const NAME     = DATA[ti].d_name;
                const IPv4        ADDR(DATA[ti].d_ip, 0);

                Vec mV;
                int err;

                if (verbose) { P_(LINE) P_(NAME) P(ADDR); }

                LOOP_ASSERT(LINE, 0 != X.lookupAddressRaw(&mV, NAME, 1));

                LOOP2_ASSERT(LINE, NAME,
                             0 == X.resolveAddress(&mV, NAME, 1, &err));

                LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache
        //
        // Concerns:
        //: 1 'bteso_IpResolutionCache' works properly in a multithreaded
        //:   environment.
        //
        // Plan:
        //: 1 Configure cache to expire in 100 ms.
        //:
        //: 2 Spawn multiple threads, for each thread (C-1):
        //:
        //:   1 Call 'resolveAddress' multiple times.  Sleep for 100ms such
        //:     that the data in the cache expires and call 'resolveAddress'
        //:     multiple times again.  Verify the expected address is returned
        //:     each time.
        //:
        //:   2 Call 'reset' then call 'resolveAddress' multiple time.  Verify
        //:     the expected address is returned each time.
        //
        // Testing:
        //   CONCERN: bteso_IpResolutionCache works with multiple threads
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING CONCURRENCY" << endl
                                  << "===================" << endl;

        using namespace BTESO_IPRESOLUTIONCACHE_CONCURRENCY;

        // A 'bcema_TestAllocator' is required for thread safe allocations.
        bcema_TestAllocator testAllocator;

        enum {
#ifdef BSLS_PLATFORM__OS_LINUX
            NUM_THREADS = 8    // linux can't do a lot of threads
#else
            NUM_THREADS = 50
#endif
        };

        bcemt_Barrier barrier(NUM_THREADS);
        Obj mX(&testConcurrencyCallback, &testAllocator);
        mX.setTimeToLive(bdet_DatetimeInterval(0, 0, 0, 0, 100));

        ThreadData args = { &mX, &barrier };
        executeInParallel(NUM_THREADS, workerThread, &args);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATORS 'resolveAddress'
        //
        // Concerns:
        //: 1 Data is cached on the first call.
        //:
        //: 2 The cache data is used if available.
        //:
        //: 3 Resolver callback is invoked when data expires.
        //:
        //: 4 New data replaces the expired data.
        //:
        //: 5 Exact number of address are returns if available.
        //:
        //: 6 Returns non-zero on error.
        //:
        //: 7 Value is not cached on error.
        //:
        //: 8 'lookupAddressRaw' returns 0 if the data is cached, and non-zero
        //:   if data has not been cached.
        //:
        //: 9 Memory allocation is from the object's allocator.
        //:
        //:10 Any memory allocation is exception neutral.
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a table of distinct test parameters.
        //:
        //: 2 Call 'resolveAddress' for each value and verify that the return
        //:   value is as expected.  Use 'lookupAddressRaw' to verify the data
        //:   is stored in the cache.  (C-1..2, 5, 8..10)
        //:
        //: 3 Set expiration time to a small interval and call 'resolveAddress'
        //:   to retrieve a value.  Wait until the data becomes stale, call
        //:   'resolveAddress' and verify that new data replaces the old one.
        //:   (C-3..4)
        //:
        //: 4 Set expiration time to 0 and verify that data does not expires
        //:   immediately.
        //:
        //: 5 Configure the test resovler to return error.  Verify error code
        //:   is returned correctly and address is nto cached  (C-6..7)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-11)
        //
        // Testing:
        //   int resolveAddress(vector *r, const char *h, int n, int *e);
        //   int lookupAddressRaw(vector *res, const char *host, int num);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'resolveAddress'" << endl
                          << "========================" << endl;

        bslma_TestAllocator         da("default", veryVeryVeryVerbose);
        bslma_DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "Create table of distinct values" << endl;

        static const struct {
            int         d_line;
            const char *d_name;
            bool        d_cachedFlag;
            const char *d_ip;
            int         d_numAddress;
        } DATA[] = {
            { L_,  "",        false,  "0.0.0.1",        1 },
            { L_,  "A",       false,  "0.0.0.2",        1 },
            { L_,  "A",        true,  "0.0.0.2",        1 },
            { L_,  "AB",      false,  "0.0.0.3",        2 },
            { L_,  "AB",       true,  "0.0.0.3",  INT_MAX },
            { L_,  "ABCDEF",  false,  "0.0.0.4",  INT_MAX },
            { L_,  "ABCDEF",   true,  "0.0.0.4",        1 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int count = 0;
        ASSERT(TestResolver::count() == count);

        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
        Obj X(&TestResolver::callback, &oa);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const NAME     = DATA[ti].d_name;
            const bool        CACHED   = DATA[ti].d_cachedFlag;
            const bool        NUM_ADDR = DATA[ti].d_numAddress;
            const IPv4        ADDR(DATA[ti].d_ip, 0);

            Vec mV(&scratch);
            int err;

            if (verbose) {
                P_(LINE) P_(NAME) P_(CACHED) P_(NUM_ADDR) P(ADDR);
            }

            if (CACHED) {
                LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));
            }
            else {
                LOOP_ASSERT(LINE, 0 != X.lookupAddressRaw(&mV, NAME, 1));
            }

            int resolverCount = TestResolver::count();

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                TestResolver::setCount(resolverCount);
                LOOP2_ASSERT(LINE, NAME,
                         0 == X.resolveAddress(&mV, NAME, NUM_ADDR, &err));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (!CACHED) {
                ++count;
            }

            LOOP3_ASSERT(LINE, TestResolver::count(), count,
                         TestResolver::count() == count);

            LOOP3_ASSERT(LINE, ADDR, mV[0], ADDR == mV[0]);

            LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));

            LOOP2_ASSERT(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }

        TestResolver::reset();

        if (verbose) cout << "Test expiration time" << endl;
        {
            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj X(&TestResolver::callback, &oa);
            X.setTimeToLive(bdet_DatetimeInterval(0, 0, 0, 2));

            Vec mV;
            ASSERT(0 == X.resolveAddress(&mV, "A", 1));
            ASSERT(1 == TestResolver::count());
            ASSERT(IPv4("0.0.0.1", 0) == mV[0]);

            ASSERT(0 == X.resolveAddress(&mV, "A", 1));
            ASSERT(1 == TestResolver::count());
            ASSERT(IPv4("0.0.0.1", 0) == mV[0]);

            bcemt_ThreadUtil::microSleep(0, 2);

            ASSERT(0 == X.resolveAddress(&mV, "A", 1));
            ASSERT(2 == TestResolver::count());
            ASSERT(IPv4("0.0.0.2", 0) == mV[0]);

            // Verify callback is not invoked again before data expires.

            ASSERT(0 == X.resolveAddress(&mV, "A", 1));
            ASSERT(2 == TestResolver::count());
            ASSERT(IPv4("0.0.0.2", 0) == mV[0]);

            X.setTimeToLive(bdet_DatetimeInterval(0, 0, 0, 0));
            bcemt_ThreadUtil::microSleep(0, 2);

            ASSERT(0 == X.resolveAddress(&mV, "A", 1));
            ASSERT(2 == TestResolver::count());
            ASSERT(IPv4("0.0.0.2", 0) == mV[0]);
        }

        TestResolver::reset();

        if (verbose) cout << "Error on callback" << endl;
        {
            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj X(&TestResolver::callback, &oa);
            TestResolver::setErrorCode(1);

            Vec mV;
            int err;
            ASSERT(1 == X.resolveAddress(&mV, "A", 1, &err));
            ASSERT(1 == err);
            ASSERT(0 == TestResolver::count());

            TestResolver::setErrorCode(0);
            ASSERT(0 == X.resolveAddress(&mV, "A", 1, &err));
            ASSERT(1 == err);  // err is unchanged.
            ASSERT(1 == TestResolver::count());
            ASSERT(IPv4("0.0.0.1", 0) == mV[0]);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj X(&TestResolver::callback, &oa);

            if (veryVerbose) cout << "\t'resolveAddress'" << endl;
            {
                Vec mV;

                ASSERT_PASS(X.resolveAddress(&mV, "A", 1));
                ASSERT_FAIL(X.resolveAddress(0,   "A", 1));
                ASSERT_FAIL(X.resolveAddress(&mV,   0, 1));
                ASSERT_FAIL(X.resolveAddress(&mV, "A", 0));
            }

        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //
        // Plan:
        //: 1 Create two 'bslma_TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 3 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3)
        //
        // Testing:
        //   bslma_Allocator *allocator() const;
        //   ResolveByNameCallback resolverCallback();
        //   const bdet_DatetimeInterval& timeToLive();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                        << "'bteso_IpResolutionCache' BASIC ACCESSORS" << endl
                        << "=========================================" << endl;

        // Attribute Types

        typedef Obj::ResolveByNameCallback T1;     // 'resolverCallback'
        typedef bdet_DatetimeInterval      T2;     // 'timeToLive'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1(bteso_ResolveUtil::defaultResolveByNameCallback());
                            // 'resolverCallback'
        const T2 D2(0, 1);  // 'timeToLive'

        // -------------------------------------------------------
        // 'A' values
        // -------------------------------------------------------

        const T1 A1(&TestResolver::callback);
        const T2 A2(1);

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            Obj mX(&oa);  const Obj& X = mX;

            const T1& resolverCallback = X.resolverCallback();
            LOOP2_ASSERT(D1, resolverCallback, D1 == resolverCallback);

            const T2& timeToLive = X.timeToLive();
            LOOP2_ASSERT(D2, timeToLive, D2 == timeToLive);

            ASSERT(&oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(timeToLive) }
        {
            Obj mX(A1, &oa);  const Obj& X = mX;

            mX.setTimeToLive(A2);

            TestAllocatorMonitor oam(oa), dam(da);

            const T1& resolverCallback = X.resolverCallback();
            LOOP2_ASSERT(A1, resolverCallback, A1 == resolverCallback);

            const T2& timeToLive = X.timeToLive();
            LOOP2_ASSERT(A2, timeToLive, A2 == timeToLive);

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // Note that memory should be independently allocated for each
        // attribute capable of allocating memory.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..14)
        //:
        //:   1 Create three 'bslma_TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values, then
        //:     setting all of the attributes to their 'B' values.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   bteso_IpResolutionCache(bslma_Allocator *bA);
        //   ~bteso_IpResolutionCache();
        //   void setTimeToLive(const bdet_DatetimeInterval& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        // Attribute Types

        typedef Obj::ResolveByNameCallback T1;     // 'resolverCallback'
        typedef bdet_DatetimeInterval      T2;     // 'timeToLive'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const T1 D1 = bteso_ResolveUtil::defaultResolveByNameCallback();
                               // 'resolverCallback'
        const T2 D2(0, 1);     // 'timeToLive'

        // 'A' values

        const T2 A2(1);

        // 'B' values

        const T2 B2(INT_MAX);

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma_TestAllocator da("default",   veryVeryVeryVerbose);
            bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma_Default::setDefaultAllocatorRaw(&da);

            Obj                 *objPtr;
            bslma_TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj((bslma_Allocator *)0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma_TestAllocator&  oa = *objAllocatorPtr;
            bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            // Also invoke the object's 'allocator' accessor.

            LOOP3_ASSERT(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the non-object allocators.

            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP3_ASSERT(CONFIG, D1, X.resolverCallback(),
                         D1 == X.resolverCallback());
            LOOP3_ASSERT(CONFIG, D2, X.timeToLive(),
                         D2 == X.timeToLive());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'timeToLive'
            {
                TestAllocatorMonitor tam(oa);

                mX.setTimeToLive(A2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, A2 == X.timeToLive());

                mX.setTimeToLive(B2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, B2 == X.timeToLive());

                mX.setTimeToLive(D2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, D2 == X.timeToLive());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            LOOP3_ASSERT(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                         oa.numBlocksTotal() == oa.numBlocksInUse());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            bslma_TestAllocator da("default", veryVeryVeryVerbose);
            bslma_Default::setDefaultAllocatorRaw(&da);

            Obj mX;

            if (veryVerbose) cout << "\tdescription" << endl;
            {
                ASSERT_SAFE_FAIL(mX.setTimeToLive(
                                          bdet_DatetimeInterval(0, 0, 0, -1)));
                ASSERT_SAFE_PASS(mX.setTimeToLive(bdet_DatetimeInterval(0)));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'bteso_IpResolutionCacheEntry' LOCK
        //
        // Concern:
        //: 1 'updatingLock' successfully retrieve the mutex in a
        //:   'bteso_IpResolutionCacheEntry' object.
        //
        // Plan:
        //: 1 Retrieve lock with the 'updatingLock' method.  Lock and verify
        //:   that tryLock fails; unlock and verify that tryLock succeeds.
        //:   (C-1)
        //
        // Testing:
        //   bcemt_Mutex& updatingLock() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "'bteso_IpResolutionCacheEntry::updateLock()'" << endl
                     << "============================================" << endl;

        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        if (veryVerbose) cout << "" << endl;
        {
            Entry mX;
            mX.updatingLock().lock();

            ThreadInfo args;
            args.d_lock = &mX.updatingLock();

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bcemt_ThreadAttributes attr;
            attr.setDetachedState(
                                bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
            bcemt_ThreadUtil::Handle dum;
            bcemt_ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0;
                 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                bcemt_ThreadUtil::microSleep(1000 * SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            mX.updatingLock().unlock();

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bcemt_ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                bcemt_ThreadUtil::microSleep(1000 * SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 == args.d_retval); // should succeed
            if (verbose) {
                P(args.d_retval);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'bteso_IpResolutionCacheEntry' BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Accessors for attributes that can allocate memory (i.e., those
        //:   that take an allocator in their constructor) return a reference
        //:   providing only non-modifiable access.
        //
        // Plan:
        //: 1 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.
        //:
        //: 3 Use the corresponding primary manipulator to set the attribute to
        //:   a unique value, and use the corresponding basic accessor to
        //:   verify the new expected value.  (C-1..4)
        //
        // Testing:
        //   int data() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                   << "'bteso_IpResolutionCacheEntry' BASIC ACCESSORS" << endl
                   << "==============================================" << endl;

        // Attribute Types

        typedef Entry::DataPtr         T1;     // 'data'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1  D1;                          // 'data'

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const T1  A1 = Entry::DataPtr(new Data(Vec(), bdet_Datetime(1, 1, 1)));

        Entry mX;  const Entry& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& data = X.data();
            LOOP2_ASSERT(D1, data, D1 == data);
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(data) }
        {
            mX.setData(A1);

            const T1& data = X.data();
            LOOP2_ASSERT(A1, data, A1 == data);
        }

        if (verbose) cout << "\nCopy Constructor." << endl;
        {
            const Entry Y(X);

            const T1& data = X.data();
            LOOP2_ASSERT(A1, data, A1 == data);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'bteso_IpResolutionCacheEntry' CTOR, PRIMARY MANIPULATORS, & DTOR
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 3 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Create three sets of distant attribute values, ('D'), ('A'), and
        //:   ('B'), with ('D') corresponding to the default-constructed
        //:   object.
        //:
        //: 2 Set each attribute with each of the values and use the individual
        //:   accessors to verify the attributes are set correctly.  (C-1..3)
        //
        // Testing:
        //   bteso_IpResolutionCache_Entry();
        //   ~bteso_IpResolutionCache_Entry();
        //   void setData(DataPtr value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        bslma_TestAllocator ta("object",  veryVeryVeryVerbose);

        // 'D' values: These are the default-constructed values.

        const Entry::DataPtr  D1;

        // 'A' values

        const Entry::DataPtr  A1(new (ta) Data(vector<bteso_IPv4Address>(),
                                               bdet_Datetime(1, 1, 1),
                                               &scratch),
                                 &ta);

        // 'B' values

        Entry::DataPtr b1;
        b1.createInplace(&ta,
                         vector<bteso_IPv4Address>(),
                         bdet_Datetime(1, 1, 1),
                         &scratch);
        const Entry::DataPtr& B1 = b1;

        Entry mX;  const Entry& X = mX;

        if (verbose) cout << "\nTesting primary manipulators."
                          << endl;
        {
            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP2_ASSERT(D1, X.data(), D1 == X.data());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'data'
            {
                mX.setData(A1);
                LOOP2_ASSERT(A1, X.data(), A1 == X.data());

                mX.setData(B1);
                LOOP2_ASSERT(B1, X.data(), B1 == X.data());
            }
        }

        if (verbose) cout << "\nTest 'reset'" << endl;
        {
            mX.reset();
            LOOP2_ASSERT(D1, X.data(), D1 == X.data());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 'TestResolver' correctly returns a different IP addresses each
        //:   time it is called.
        //:
        //: 2 The number of addresses returned can be controlled by the
        //:   'setNumAddresses' method.
        //:
        //: 3 The number of addresses returned are as expected.
        //:
        //: 4 'reset' puts the 'TestResolver' back to its default state.
        //
        // Plan:
        //: 1 Create a table of distinct configurations for 'TestResolver'
        //:
        //: 2 For each entry in the table, invoke the callback of the
        //:   'TestResolver' and verify that the expected result is returned.
        //:   (C-1..3)
        //:
        //: 3 Call 'reset' and verify 'TestResolver' is returned its initial
        //:   state.  (C-4)
        //
        // Testing:
        //   CONCERN: Test apparatus is working as expected.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Test apparatus" << endl
                          << "==============" << endl;

        if (veryVerbose) cout <<
                             "Create table of distinct configurations" << endl;

        struct {
            int d_line;
            int d_numReturned;
            int d_maxNumAddresses;
        } DATA[] = {

        //LINE  RETURN      MAX

        { L_,        0,       1 },
        { L_,        1,       1 },
        { L_,        0,       2 },
        { L_,        1,       2 },
        { L_,  INT_MAX,       2 },
        { L_,        0, INT_MAX },
        { L_,        1, INT_MAX },
        { L_,        2, INT_MAX },

        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE = DATA[ti].d_line;
            const int NUM  = DATA[ti].d_numReturned;
            const int MAX  = DATA[ti].d_maxNumAddresses;
            const int CNT  = ti + 1;

            const bteso_IPv4Address EXP_IP   = bteso_IPv4Address(
                                                     BSLS_BYTEORDER_HTONL(CNT),
                                                     0);
            const int               EXP_SIZE = bsl::min(NUM, MAX);

            TestResolver::setNumAddresses(NUM);
            LOOP3_ASSERT(LINE, NUM, TestResolver::numAddresses(),
                         NUM == TestResolver::numAddresses());

            bsl::vector<bteso_IPv4Address> addr;
            int                            error = -1;

            const bsl::vector<bteso_IPv4Address> &ADDR = addr;
            const int                            &ERR = error;

            ASSERT(0 == TestResolver::callback(&addr, "host", MAX, &error));

            const bteso_IPv4Address IP   = (0 < EXP_SIZE)
                                         ? ADDR[0]
                                         : bteso_IPv4Address();
            const int               SIZE = ADDR.size();

            LOOP3_ASSERT(LINE, EXP_SIZE, SIZE, EXP_SIZE == SIZE);
            if (0 < SIZE) {
                LOOP3_ASSERT(LINE, EXP_IP, IP, EXP_IP == IP);
            }

            LOOP2_ASSERT(LINE, ERR, -1 == ERR);  // ERR should be unchanged

            LOOP3_ASSERT(LINE, EXP_IP, TestResolver::lastIPv4Added(),
                         EXP_IP == TestResolver::lastIPv4Added());
            LOOP3_ASSERT(LINE, CNT, TestResolver::count(),
                         CNT == TestResolver::count());
        }

        if (verbose) cout << "reset" << endl;
        {
            TestResolver::reset();
            const int CNT = TestResolver::count();
            const int NUM = TestResolver::numAddresses();
            LOOP_ASSERT(CNT, 0 == CNT);
            LOOP_ASSERT(NUM, 1 == NUM);
        }

        if (verbose) cout << "Test 'errorCode'" << endl;
        {
            bsl::vector<bteso_IPv4Address> addr;
            int                            err;

            TestResolver::reset();

            TestResolver::setErrorCode(INT_MIN);
            ASSERT(INT_MIN == TestResolver::errorCode());
            ASSERT(INT_MIN == TestResolver::callback(&addr, "host", 1, &err));
            ASSERT(INT_MIN == err);
            ASSERT(0 == TestResolver::count());

            TestResolver::setErrorCode(-1);
            ASSERT(-1 == TestResolver::errorCode());
            ASSERT(-1 == TestResolver::callback(&addr, "host", 1, &err));
            ASSERT(-1 == err);
            ASSERT(0 == TestResolver::count());

            TestResolver::setErrorCode(1);
            ASSERT(1 == TestResolver::errorCode());
            ASSERT(1 == TestResolver::callback(&addr, "host", 1, &err));
            ASSERT(1 == err);
            ASSERT(0 == TestResolver::count());

            TestResolver::setErrorCode(INT_MAX);
            ASSERT(INT_MAX == TestResolver::errorCode());
            ASSERT(INT_MAX == TestResolver::callback(&addr, "host", 1, &err));
            ASSERT(INT_MAX == err);
            ASSERT(0 == TestResolver::count());
        }

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        bsl::vector<bteso_IPv4Address> addresses;

        {
            bteso_IPv4Address ipv4;
            int err;

            bsls_Stopwatch timer;
            timer.start();
            for (int i = 0; i < 10; ++i) {
                bteso_ResolveUtil::getAddress(&ipv4, "nylxdev1", &err);
            }
            timer.stop();
            bsl::cout << "ip: " << ipv4 << bsl::endl;
            bsl::cout << "Time: " << timer.accumulatedWallTime() << bsl::endl;
        }

        {
            bteso_ResolveUtil::setResolveByNameCallback(&resolverCallback);

            bteso_IPv4Address ipv4;
            int err;

            bsls_Stopwatch timer;
            timer.start();

            for (int i = 0; i < 10; ++i) {
                bteso_ResolveUtil::getAddress(&ipv4, "nylxdev1", &err);
            }
            timer.stop();
            bsl::cout << "ip: " << ipv4 << bsl::endl;
            bsl::cout << "Time: " << timer.accumulatedWallTime() << bsl::endl;
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        //  MULTITHREADED PERFORMANCE TEST

        //  Concern:
        //: 1 The cache provides performance improvement
        //
        //  Plan:
        //: 1 Register the cache callback to 'bteso_ResolveUtil' and compare
        //:   the performance between the cache and the default resolver.
        //:   (C-1)
        // --------------------------------------------------------------------

        if (argc <= 2) {
            cerr << "Usage:" << endl
                 << "cmd -1 <option>" << endl
                 << "\toption" << endl
                 << "\t\t0 - default" << endl
                 << "\t\t1 - cached" << endl;

            return 1;                                                 // RETURN
        }
        int option   = atoi(argv[2]);
        int numCalls = argc > 3 ? atoi(argv[3]) : 20;  // default number of
                                                       // calls to 20

        bcemt_Mutex updateMutex;

        const int THREAD_CNT[] = {5};
        const int NUM_THREAD_CNT = sizeof THREAD_CNT / sizeof *THREAD_CNT;

        bsl::vector<bsl::string> hostnames;
        hostnames.push_back("sundev1");
        hostnames.push_back("nylxdev1");
        hostnames.push_back("ibm1");
        hostnames.push_back("hp1");
        hostnames.push_back("nysbldo1");

        ipCacheInstance()->setTimeToLive(
                                       bdet_DatetimeInterval(0, 0, 0, 0, 100));
        bteso_ResolveUtil::setResolveByNameCallback(&resolverCallback);

        for (int tj = 0; tj < NUM_THREAD_CNT; ++tj) {
            const int THREAD = THREAD_CNT[tj];

            int NUM_CALLS = numCalls;
            int OPT       = option;

            double totalSystemTime = 0;
            double totalUserTime   = 0;
            double totalWallTime   = 0;

#ifdef BSLS_PLATFORM__OS_AIX
            setenv("NSORDER", "bind,local", 1);
#endif

            int NUM_TRIALS = 1000;
            for (int i = 0; i < NUM_TRIALS; ++i) {
                bcep_FixedThreadPool::Job job = bdef_BindUtil::bind(
                                                          &performanceTest,
                                                          &updateMutex,
                                                          &totalSystemTime,
                                                          &totalUserTime,
                                                          &totalWallTime,
                                                          &hostnames,
                                                          &OPT,
                                                          NUM_CALLS);

                bcep_FixedThreadPool pool(THREAD, 2000);
                pool.start();

                for (int i = 0; i < THREAD; ++i) {
                    pool.enqueueJob(job);
                }

                pool.drain();
                ipCacheInstance()->removeAll();
            }

            double rate = (THREAD
                           * NUM_CALLS
                           * hostnames.size()
                           * NUM_TRIALS)
                        / (totalWallTime / THREAD);

            P(totalWallTime);
            P(rate);

            cout << endl;
        }

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
