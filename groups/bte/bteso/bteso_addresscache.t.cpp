// bteso_addresscache.t.cpp                                           -*-C++-*-
#include <bteso_addresscache.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>
#include <bsls_stopwatch.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>

#include <bcemt_lockguard.h>
#include <bcep_fixedthreadpool.h>

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

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bteso_AddressCache Obj;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

class CachedResolver {
    static bteso_AddressCache                       *s_addressCachePtr;
    static bteso_ResolveUtil::ResolveByNameCallback  s_originalResolverCb;
  public:
    CachedResolver();
    ~CachedResolver();
    static int resolveByName(bsl::vector<bteso_IPv4Address> *hostAddresses,
                  const char                     *hostName,
                  int                             numAddresses,
                  int                            *errorCode);
};

bteso_AddressCache                       *CachedResolver::s_addressCachePtr;
bteso_ResolveUtil::ResolveByNameCallback  CachedResolver::s_originalResolverCb;

CachedResolver::CachedResolver()
{
    static bteso_AddressCache cache;
    s_addressCachePtr = &cache;
    s_originalResolverCb = bteso_ResolveUtil::setResolveByNameCallback(
                                                               &resolveByName);
}

CachedResolver::~CachedResolver()
{
    s_originalResolverCb = bteso_ResolveUtil::setResolveByNameCallback(
                                                         s_originalResolverCb);
}

int CachedResolver::resolveByName(
                                 bsl::vector<bteso_IPv4Address> *hostAddresses,
                                 const char                     *hostName,
                                 int                             numAddresses,
                                 int                            *errorCode)
{
    return s_addressCachePtr->resolveAddress(hostAddresses,
                                             hostName,
                                             numAddresses,
                                             errorCode);
}

static bteso_AddressCache* s_addressCachePtr;

static
int resolveByName(bsl::vector<bteso_IPv4Address> *hostAddresses,
                  const char                     *hostName,
                  int                             numAddresses,
                  int                            *errorCode)
{
    return s_addressCachePtr->resolveAddress(hostAddresses,
                                             hostName,
                                             numAddresses,
                                             errorCode);
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
         ++it)
    {
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
                putenv("NSORDER=bind,local");
                int rc = bteso_ResolveUtil::getAddressDefault(&address,
                                                              it->c_str());
                BSLS_ASSERT(!rc);
            }
          } break;
          case CACHE_AND_PUTENV: {
            for (int i = 0; i < NUM_CALLS; ++i) {
                putenv("NSORDER=bind,local");
                int rc = bteso_ResolveUtil::getAddress(&address,
                                                       it->c_str());
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

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    bteso_AddressCache addressCache;
    s_addressCachePtr = &addressCache;

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
///Usage
///-----
// In this section, we show the intended usage of this component.
//
///Example 1: Retrieving the IPv4 addresses of a given host
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we create a 'bteso_AddressCache', and use it to retrieve
// the IPv4 addresses of several host machines.
//
// First, we create a 'bteso_AddressCache' object.  By default, it will use
// 'bteso_ResolveUtil::defaultResolveByNameCallback' to retrieve addresses that
// are not in the cache:
//..
    bteso_AddressCache cache;
//..
// Then, we verify the newly constructed cache does not contain the addresses
// of either 'sundev1' and 'ibm1':
//..
    bsl::vector<bteso_IPv4Address> hostAddresses;
    ASSERT(0 != cache.lookupAddress(&hostAddresses, "sundev1", 1));
    ASSERT(0 != cache.lookupAddress(&hostAddresses, "ibm1", 1));
//..
// Next, we call 'resolveAddress' method to retrieve a list of IPv4 address for
// the 'sundev1' machine.  Since this is the first call to 'resolveAddress',
// 'resolverCallback' will be invoked to retrieve the addresses:
//..
    int rc = cache.resolveAddress(&hostAddresses, "sundev1", 1);
    ASSERT(                                  0 == rc);
    ASSERT(                                  1 == hostAddresses.size());
    ASSERT(bteso_IPv4Address("172.17.1.20", 0) == hostAddresses[0]);
//..
//  Finally, we verify that subsquent call to 'lookupAddress' returns the
//  cached value for 'sundev1', but not 'ibm1':
//..
    ASSERT(0 == cache.lookupAddress(&hostAddresses, "sundev1", 1));
    ASSERT(0 != cache.lookupAddress(&hostAddresses, "ibm1", 1));
//..
//
///Example 2: Using this cache with 'bteso_ResolveUtil'
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can use the cache with 'bteso_ResolveUtil' by setting the resolver
// callback.  When using 'bteso_ResolveUtil::setResolverCallback' method, we
// need a free function instead of a member function of a class.
//
// First, we declared a static variable that will point to a cache:
//..
// static bteso_AddressCache* s_addressCachePtr;
//..
// Then, we create a function using the pointer that was just declared:
//..
// static
// int resolveByName(bsl::vector<bteso_IPv4Address> *hostAddresses,
//                   const char                     *hostName,
//                   int                             numAddresses,
//                   int                            *errorCode)
// {
//     return s_addressCachePtr->resolveAddress(hostAddresses,
//                                              hostName,
//                                              numAddresses,
//                                              errorCode);
// }
//..
// Next, we create a cache that will be used by the free function, and assign
// the static pointer to refer to this cache.
//..
//  bteso_AddressCache addressCache;
//  s_addressCachePtr = &addressCache;
//..
// Then, we set the callback for 'bteso_ResolveUtil':
//..
    bteso_ResolveUtil::setResolveByNameCallback(resolveByName);
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'sundev1':
//..
    bteso_IPv4Address ipv4;
    bteso_ResolveUtil::getAddress(&ipv4, "sundev1");
    ASSERT(bteso_IPv4Address("172.17.1.20", 0) == ipv4);
//..
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
        // bteso_AddressCache
        //
        // clear
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // bteso_AddressCache
        //
        // resolveaddress
        // --------------------------------------------------------------------
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // bteso_AddressCache
        //
        // resolveaddress
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // bteso_AddressCache
        //
        // manipulator
        // --------------------------------------------------------------------
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // bteso_addresscache
        //
        // basic accessors
        // --------------------------------------------------------------------
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // bteso_AddressCacheEntry
        //
        // lock
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // bteso_AddressCacheEntry
        //
        // accessor
        // --------------------------------------------------------------------
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // bteso_AddressCacheEntry
        //
        // manipulator
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // bteso_AddressCacheData
        // --------------------------------------------------------------------

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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        bsl::vector<bteso_IPv4Address> addresses;
        int errorCode = 0;

        //int rc = cache.resolveAddress(&addresses, "nylxdev1", 16, &errorCode);
        //bsl::cout << "rc: " << rc << bsl::endl;

        //bsl::cout << "addresses.size(): " << addresses.size() << bsl::endl;
        //for (int i = 0; i < addresses.size(); ++i) {
        //    bsl::cout << "addresses[" << i << "]: " << addresses[i] << bsl::endl;
        //}
        //bsl::cout << "errorCode: " << errorCode << bsl::endl;

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
            bteso_ResolveUtil::setResolveByNameCallback(resolveByName);

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

        bteso_ResolveUtil::setResolveByNameCallback(resolveByName);

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

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
