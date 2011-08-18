// bteso_ipresolutioncache.t.cpp                                      -*-C++-*-
#include <bteso_ipresolutioncache.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>
#include <bsls_stopwatch.h>

#include <bdef_bind.h>

#include <bslma_default.h>
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
//
// Primary Manipulators:
//: o 'setAttribute'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'attribute'
//
// Global Concerns:
//: o
//
// Global Assumptions:
//: o
// ----------------------------------------------------------------------------
// CLASS METHODS
//
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//
// FREE FUNCTIONS
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {

class bteso_IpResolutionCache_Data {
    // This class provides storage for the a set of IP addresses and a
    // 'bdet_Datetime' to indicate the time these addresses expires.

    // DATA
    bsl::vector<bteso_IPv4Address> d_addresses;       // set of IP addresses

    bdet_Datetime                  d_creationTime;  // time from epoch until
                                                      // this data expires
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
        // Create an object storing the specified 'ipAddresses', which expires
        // at the specified 'creationTime' (expressed as the !ABSOLUTE! time
        // from 00:00:00 UTC, January 1, 1970).  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // ACCESSORS
    const bsl::vector<bteso_IPv4Address>& addresses() const;
        // Return a reference providing non-modifiable access to the IP
        // addresses stored in this object.

    const bdet_Datetime& creationTime() const;
        // Return a reference providing non-modifiable access to the time
        // (expressed as the !ABSOLUTE! time from 00:00:00 UTC, January 1,
        // 1970) addresses in the object expires.
};

}

typedef bteso_IpResolutionCache                Obj;
typedef bteso_IpResolutionCache_Entry          Entry;
typedef bteso_IpResolutionCache_Data           Data;
typedef bteso_IPv4Address                 IPv4;
typedef bsl::vector<bteso_IPv4Address>    Vec;
typedef bteso_IpResolutionCache_Entry::DataPtr DataPtr;

struct ThreadInfo {
    bcemt_Mutex    *d_lock;
    bces_AtomicInt  d_retval;
    bces_AtomicInt  d_retvalSet;
};

extern "C" void* MyThread(void* arg_p) {
    ThreadInfo* arg = (ThreadInfo*)arg_p;

    arg->d_retval = arg->d_lock->tryLock();
    if (0 == arg->d_retval) {
        arg->d_lock->unlock();
    }
    arg->d_retvalSet = 1;
    return arg_p;
}

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

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

                         // ============
                         // TestResolver
                         // ============

class TestResolver {
    // DATA
    static int  s_count;
    static int  s_numAddresses;

  public:
    // CLASS METHOD
    TestResolver();
    static int callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                        const char                     *hostName,
                        int                             maxNumAddresses,
                        int                            *errorCode);
    static int count();
    static bteso_IPv4Address lastIPv4Added();
    static void reset();
    static void setNumAddresses(int value);
    static int numAddresses();
};

                         // ------------
                         // TestResolver
                         // ------------


int TestResolver::s_count        = 0;
int TestResolver::s_numAddresses = 1;

int TestResolver::callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                           const char                     *hostName,
                           int                             maxNumAddresses,
                           int                            *errorCode)
{
    //cout << "callback" << endl;
    ++s_count;

    hostAddresses->clear();

    int size = bsl::min(s_numAddresses, maxNumAddresses);
    for (int i = 0; i < size; ++i) {
        hostAddresses->push_back(
                          bteso_IPv4Address(BSLS_BYTEORDER_HTONL(s_count), 0));
    }

    if (errorCode) {
        *errorCode = s_count;
    }

    return 0;
}

int TestResolver::count()
{
    return s_count;
}

bteso_IPv4Address TestResolver::lastIPv4Added()
{
    return bteso_IPv4Address(BSLS_BYTEORDER_HTONL(s_count), 0);
}

void TestResolver::reset()
{
    s_count        = 0;
    s_numAddresses = 1;
}

void TestResolver::setNumAddresses(int value)
{
    s_numAddresses = value;
}

int TestResolver::numAddresses()
{
    return s_numAddresses;
}

const int NUM_CALLS = 20;

enum { DEFAULT = 0, CACHE, PUTENV, CACHE_AND_PUTENV };

static
void resolve(bcemt_Mutex    *updateMutex,
             double         *totalSystemTime,
             double         *totalUserTime,
             double         *totalWallTime,
             vector<string> *hostname,
             int            *option)
{
    bteso_IPv4Address address;

    bsls_Stopwatch timer;
    timer.start();

    for (vector<string>::const_iterator it = hostname->begin();
                                        it != hostname->end();
                                        ++it) {
        switch (*option) {
          case DEFAULT: {
            for (int i = 0; i < NUM_CALLS; ++i) {
                int rc = bteso_ResolveUtil::getAddressDefault(&address,
                                                              it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
          case CACHE: {
            for (int i = 0; i < NUM_CALLS; ++i) {
                int rc = bteso_ResolveUtil::getAddress(&address,
                                                       it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
          case PUTENV: {
            for (int i = 0; i < NUM_CALLS; ++i) {
#ifdef BSLS_PLATFORM__OS_AIX
                setenv("NSORDER", "bind,local", 1);
#endif
                int rc = bteso_ResolveUtil::getAddressDefault(&address,
                                                              it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
          case CACHE_AND_PUTENV: {
            for (int i = 0; i < NUM_CALLS; ++i) {
#ifdef BSLS_PLATFORM__OS_AIX
                setenv("NSORDER", "bind,local", 1);
#endif
                int rc = bteso_ResolveUtil::getAddress(&address, it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
        }
    }
    timer.stop();

    bcemt_LockGuard<bcemt_Mutex> guard(updateMutex);

//     cout << "Wall Time: " << timer.accumulatedWallTime() << endl;

    *totalSystemTime += timer.accumulatedSystemTime();
    *totalUserTime   += timer.accumulatedUserTime();
    *totalWallTime   += timer.accumulatedWallTime();
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

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&threads[i], func, threadArgs);
    }
    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

namespace BTESO_IPRESOLUTIONCACHE_CONCURRENCY {

struct TimeZoneData {
        int         d_line;       // line number
        const char *d_hostname;   // time zone id
        int         d_ipAddress;  // utc offset in seconds for descriptor
} DATA[] = {
    { L_,  "ID_A",  1 },
    { L_,  "ID_B",  2 },
    { L_,  "ID_C",  3 },
    { L_,  "ID_D",  4 },
    { L_,  "ID_E",  5 },
    { L_,  "ID_F",  6 },
    { L_,  "ID_G",  7 },
    { L_,  "ID_H",  8 },
    { L_,  "ID_I",  9 },
    { L_,  "ID_J", 10 },
    { L_,  "ID_K", 11 },
    { L_,  "ID_L", 12 },
    { L_,  "ID_M", 13 },
    { L_,  "ID_N", 14 }
};
const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

struct ThreadData {
    Obj           *d_cache_p;    // cache under test
    bcemt_Barrier *d_barrier_p;  // testing barrier
};

                         // =======================
                         // testConcurrencyResolver
                         // =======================

int testConcurrencyCallback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                            const char                     *hostName,
                            int                             maxNumAddresses,
                            int                            *errorCode)
{
    hostAddresses->clear();

    for (int i = 0; i < NUM_DATA; ++i) {
        if (0 == strcmp(DATA[i].d_hostname, hostName)) {
            hostAddresses->push_back(
              bteso_IPv4Address(BSLS_BYTEORDER_HTONL(DATA[i].d_ipAddress), 0));
            break;
        }
    }

    //cout << hostName << ": " << (*hostAddresses)[0] << endl;

    if (errorCode) {
        *errorCode = 0;
    }

    return 0;
}

extern "C" void *workerThread(void *arg)
{
    ThreadData *p = (ThreadData*)arg;
    bcemt_Barrier& barrier = *p->d_barrier_p;

    Obj &mX = *p->d_cache_p; const Obj &X = mX;

    // Begin the test.

    barrier.wait();

    for (int testRun = 0; testRun < 20; ++testRun) {
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

    bcemt_ThreadUtil::microSleep(0, 1);

    for (int testRun = 0; testRun < 20; ++testRun) {
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

class bslma_TestAllocatorMonitor {
    // TBD

    // DATA
    int                              d_lastInUse;
    int                              d_lastMax;
    int                              d_lastTotal;
    const bslma_TestAllocator *const d_allocator_p;

  public:
    // CREATORS
    bslma_TestAllocatorMonitor(const bslma_TestAllocator& basicAllocator);
        // TBD

    ~bslma_TestAllocatorMonitor();
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
bslma_TestAllocatorMonitor::bslma_TestAllocatorMonitor(
                                     const bslma_TestAllocator& basicAllocator)
: d_lastInUse(basicAllocator.numBlocksInUse())
, d_lastMax(basicAllocator.numBlocksMax())
, d_lastTotal(basicAllocator.numBlocksTotal())
, d_allocator_p(&basicAllocator)
{
}

inline
bslma_TestAllocatorMonitor::~bslma_TestAllocatorMonitor()
{
}

// ACCESSORS
inline
bool bslma_TestAllocatorMonitor::isInUseSame() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() == d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isInUseUp() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() != d_lastInUse;
}

inline
bool bslma_TestAllocatorMonitor::isMaxSame() const
{
    return d_allocator_p->numBlocksMax() == d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isMaxUp() const
{
    return d_allocator_p->numBlocksMax() != d_lastMax;
}

inline
bool bslma_TestAllocatorMonitor::isTotalSame() const
{
    return d_allocator_p->numBlocksTotal() == d_lastTotal;
}

inline
bool bslma_TestAllocatorMonitor::isTotalUp() const
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
      case 12: {
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
    ASSERT(0 == rc);
    ASSERT(1 == ipAddresses.size());
    bsl::cout << "IP Address: " << ipAddresses[0] << std::endl;
//..
//  Finally, we verify that subsequent call to 'lookupAddressRaw' return 0 to
//  indicate "www.bloomberg.com" is stored in the cache, but not
//  "www.businessweek.com":
//..
    ASSERT(0 == cache.lookupAddressRaw(&ipAddresses, "www.bloomberg.com", 1));
    ASSERT(0 != cache.lookupAddressRaw(&ipAddresses,
                                       "www.businessweek.com",
                                       1));
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
    bteso_IPv4Address ipv4;
    bteso_ResolveUtil::getAddress(&ipv4, "www.bloomberg.com");
    bsl::cout << "IP Address: " << ipv4 << std::endl;
//..
// Now, we write the address to stdout:
//..
//  bsl::cout << "IP Address: " << ipAddress << std::endl;
//..
// Finally, we observe the output to be in the form:
//..
//  IP Address: 63.85.36.34:0
//..
}
      } break;
      case 11: {
        // --------------------------------------------------------------------
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache
        //
        // Concerns:
        //: 
        //
        // removeAll
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
                             0 == X.resolveAddress(&mV, NAME, 1));

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
                             0 == X.resolveAddress(&mV, NAME, 1));

                LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache
        //
        // resolveaddress multithread
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING CONCURRENCY" << endl
                                  << "===================" << endl;

        using namespace BTESO_IPRESOLUTIONCACHE_CONCURRENCY;

        // A 'bcema_TestAllocator' is required for thread safe allocations.
        bcema_TestAllocator testAllocator;

        enum {
            NUM_THREADS = 20
        };

        bcemt_Barrier barrier(NUM_THREADS);
        Obj mX(&testConcurrencyCallback, &testAllocator); const Obj& X = mX;
        mX.setTimeToLive(bdet_DatetimeInterval(0, 0, 0, 1));

        ThreadData args = { &mX, &barrier };
        executeInParallel(NUM_THREADS, workerThread, &args);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache
        //
        // Concerns:
        //: 1 Data is cached on the first call
        //:
        //: 2 The cache data is used if available
        //:
        //: 3 Resolver callback is invoked when data expires
        //:
        //: 4 New data replaces the expired data.
        //:
        //: 5 Exact number of address are returns if available
        //:
        //: 6 Returns non-zero on error
        //:
        //: 7 Value is not cached on error
        //:
        //: ...Allocators...
        //:
        //: 8 QoI: Negative testing
        //
        // Plan:
        //:
        //
        // --------------------------------------------------------------------

        if (verbose) { cout << "Testing 'resolveAddress'" << endl; }
        {
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

                Vec mV;
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

                LOOP2_ASSERT(LINE, NAME,
                             0 == X.resolveAddress(&mV, NAME, NUM_ADDR, &err));

                if (!CACHED) {
                    ++count;
                }

                LOOP3_ASSERT(LINE, TestResolver::count(), count,
                             TestResolver::count() == count);

                LOOP3_ASSERT(LINE, ADDR, mV[0], ADDR == mV[0]);

                LOOP_ASSERT(LINE, 0 == X.lookupAddressRaw(&mV, NAME, 1));
            }
        }

        TestResolver::reset();

        if (verbose) { cout << "Test expiration time" << endl; }
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

            bcemt_ThreadUtil::microSleep(0, 2.5);

            ASSERT(0 == X.resolveAddress(&mV, "A", 1));

            ASSERT(2 == TestResolver::count());
            ASSERT(IPv4("0.0.0.2", 0) == mV[0]);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj X(&TestResolver::callback, &oa);

            if (veryVerbose) cout << "\t'resolveAddress'" << endl;
            {
                Vec mV;
                int err;

                ASSERT_PASS(X.resolveAddress(&mV, "A", 1));
                ASSERT_FAIL(X.resolveAddress(0,   "A", 1));
                ASSERT_FAIL(X.resolveAddress(&mV,   0, 1));
                ASSERT_FAIL(X.resolveAddress(&mV, "A", 0));
            }

        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
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
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
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
        //   const string& description() const;
        //   bool timeToLive() const;
        //   int resolverCallback() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        // Attribute Types

        typedef Obj::ResolveByNameCallback T1;     // 'resolverCallback'
        typedef bdet_DatetimeInterval      T2;     // 'timeToLive'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = bteso_ResolveUtil::defaultResolveByNameCallback();
                            // 'resolverCallback'
        const T2 D2(0, 1);  // 'timeToLive'

        // -------------------------------------------------------
        // 'A' values
        // -------------------------------------------------------

        const T1 A1 = &TestResolver::callback;
        const T2 A2(1);

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma_TestAllocator da("default", veryVeryVeryVerbose);
        bslma_TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma_Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
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
            mX.setTimeToLive(A2);

            bslma_TestAllocatorMonitor oam(oa), dam(da);

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
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
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
        //   baetzo_LocalTimeDescriptor(bslma_Allocator *bA = 0);
        //   ~baetzo_LocalTimeDescriptor();
        //   setDescription(const StringRef& value);
        //   setTimeToLive(bool value);
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

        const T1 A1 = &TestResolver::callback;
        const T2 A2(1);

        // 'B' values

        const T1 B1 = 0;
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
                bslma_TestAllocatorMonitor tam(oa);

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
        // bteso_IpResolutionCacheEntry
        //
        // lock
        // --------------------------------------------------------------------
        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        {
            Entry mX;  const Entry &X = mX;
            X.updatingLock().lock();

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

            X.updatingLock().unlock();

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
        // BASIC ACCESSORS
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
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
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
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   bslma_Allocator *allocator() const;
        //   const string& description() const;
        //   bool dstInEffectFlag() const;
        //   int data() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

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

        const T1  A1   = Entry::DataPtr(new Data(Vec(),
                                        bdet_Datetime(1, 1, 1)));

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
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
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
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 3 Any argument can be 'const'.
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
        //   TBD: FIXME
        //   baetzo_LocalTimeDescriptor(bslma_Allocator *bA = 0);
        //   ~baetzo_LocalTimeDescriptor();
        //   setDescription(const StringRef& value);
        //   setTimeToLive(bool value);
        //   setResolverCallback(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const Entry::DataPtr  D1;

        // 'A' values

        const Entry::DataPtr  A1(new Data(vector<bteso_IPv4Address>(),
                                          bdet_Datetime(1, 1, 1),
                                          &scratch));


        // 'B' values

        Entry::DataPtr b1;
        b1.createInplace(&scratch,
                         vector<bteso_IPv4Address>(),
                         bdet_Datetime(1, 1, 1),
                         &scratch);
        const Entry::DataPtr& B1 = b1;

        if (verbose) cout << "\nTesting primary manipulators."
                          << endl;
        {
            Entry mX;  const Entry& X = mX;

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP2_ASSERT(D1, X.data(),
                         D1 == X.data());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'data'
            {
                mX.setData(A1);

                mX.setData(B1);
            }
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
        //:
        //
        // Plan:
        //
        //: 1 Create a tabel of distinct configurations for 'TestResolver'
        //:
        //: 2 
        //
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Test apparatus" << endl;

        if (veryVerbose) cout << "Create table of distinct configurations" << endl;

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
            int                            error;

            const bsl::vector<bteso_IPv4Address> &ADDR = addr;
            const int                            &ERR = error;

            TestResolver::callback(&addr, "host", MAX, &error);

            const bteso_IPv4Address IP   = (0 < EXP_SIZE)
                                         ? ADDR[0]
                                         : bteso_IPv4Address();
            const int               SIZE = ADDR.size();

            LOOP3_ASSERT(LINE, EXP_SIZE, SIZE, EXP_SIZE == SIZE);
            if (0 < SIZE) {
                LOOP3_ASSERT(LINE, EXP_IP, IP, EXP_IP == IP);
            }

            LOOP3_ASSERT(LINE, CNT, ERR, CNT == ERR);

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
        int errorCode = 0;

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
        // --------------------------------------------------------------------

        bcemt_Mutex updateMutex;
        int option = 0;

        const int THREAD_CNT[] = {1, 2, 5, 10, 20};
        const int NUM_THREAD_CNT = sizeof THREAD_CNT / sizeof *THREAD_CNT;

        const int OPTION[] = {PUTENV, CACHE_AND_PUTENV};
        const int NUM_OPTION = sizeof OPTION / sizeof *OPTION;

        bsl::vector<bsl::string> hostnames;
        hostnames.push_back("sundev1");
        hostnames.push_back("nylxdev1");
        hostnames.push_back("ibm1");
        hostnames.push_back("hp1");
        hostnames.push_back("nysbldo1");

        bteso_ResolveUtil::setResolveByNameCallback(&resolverCallback);

        for (int ti = 0; ti < NUM_OPTION; ++ti) {
            int OPT = OPTION[ti];
            P(OPT);
            for (int tj = 0; tj < NUM_THREAD_CNT; ++tj) {
                double      totalSystemTime = 0;
                double      totalUserTime   = 0;
                double      totalWallTime   = 0;

                const int THREAD = THREAD_CNT[tj];
                P(THREAD);

                bcep_FixedThreadPool::Job job = bdef_BindUtil::bind(
                                                              &resolve,
                                                              &updateMutex,
                                                              &totalSystemTime,
                                                              &totalUserTime,
                                                              &totalWallTime,
                                                              &hostnames,
                                                              &OPT);

                bcep_FixedThreadPool pool(THREAD, 2000);
                pool.start();

                for (int i = 0; i < THREAD; ++i) {
                    pool.enqueueJob(job);
                }

                pool.drain();

                cout << "Total System Time: " << totalSystemTime << endl;
                cout << "Total User Time: "   << totalUserTime   << endl;
                cout << "Total Wall Time: "   << totalWallTime   << endl;

            }
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
