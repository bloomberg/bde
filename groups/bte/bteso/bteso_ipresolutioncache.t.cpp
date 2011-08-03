// bteso_ipresolutioncache.t.cpp                                      -*-C++-*-
#include <bteso_ipresolutioncache.h>
#include <bteso_ipv4address.h>
#include <bteso_resolveutil.h>
#include <bsls_stopwatch.h>

#include <bdef_bind.h>
#include <bdef_placeholder.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>

#include <bcemt_lockguard.h>
#include <bcep_fixedthreadpool.h>

// #include <bces_atomictypes.h>

#include <bcemt_threadattributes.h>
#include <bcemt_threadutil.h>
#include <bcemt_once.h>

#include <bsl_climits.h>     // 'INT_MIN', 'INT_MAX'
#include <bsls_asserttest.h>
#include <bsls_byteorder.h>

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

typedef bteso_IpResolutionCache                Obj;
typedef bteso_IpResolutionCache_Entry          Entry;
//typedef bteso_IpResolutionCache_Data           Data;
typedef bteso_IPv4Address                 IPv4;
typedef bsl::vector<bteso_IPv4Address>    Vec;
typedef bteso_IpResolutionCache_Entry::DataPtr DataPtr;

/*
struct ThreadInfo {
    Entry          *d_lock;
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
*/

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

                         // ============
                         // TestResolver
                         // ============

class TestResolver {
    static int s_count;
  public:
    static int callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                        const char                     *hostName,
                        int                             numAddresses,
                        int                            *errorCode);
    static int count();
    static bteso_IPv4Address lastIPv4Added();
    static void reset();
};

                         // ------------
                         // TestResolver
                         // ------------

int TestResolver::s_count = 0;

int TestResolver::callback(bsl::vector<bteso_IPv4Address> *hostAddresses,
                           const char                     *hostName,
                           int                             numAddresses,
                           int                            *errorCode)
{
    //cout << "callback" << endl;
    ++s_count;

    hostAddresses->clear();
    hostAddresses->push_back(bteso_IPv4Address(BSLS_BYTEORDER_HTONL(s_count), 0));

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
    s_count = 0;
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
    ASSERT(0 != cache.lookupAddress(&ipAddresses, "www.bloomberg.com", 1));
    ASSERT(0 != cache.lookupAddress(&ipAddresses, "www.businessweek.com", 1));
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
//  Finally, we verify that subsquent call to 'lookupAddress' return 0 to
//  indicate "www.bloomberg.com" is stored in the cache, but not
//  "www.businessweek.com":
//..
    ASSERT(0 == cache.lookupAddress(&ipAddresses, "www.bloomberg.com", 1));
    ASSERT(0 != cache.lookupAddress(&ipAddresses, "www.businessweek.com", 1));
//..
//
}
{
///Example 2: Using Address Cache with 'bteso_ResolveUtil'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate how to configure the 'bteso_ResolverUtil'
// component to use the cache for resolving IP addresses.
//
// First, we create a cache for the IP addresses:
//..
    bteso_IpResolutionCache cache;
//..
// Now, we set the callback for 'bteso_ResolveUtil' by using 'bdef_BindUtil':
//..
    using namespace bdef_PlaceHolders;
    bteso_ResolveUtil::setResolveByNameCallback(
                  bdef_BindUtil::bind(&bteso_IpResolutionCache::resolveAddress,
                                      &cache,
                                      _1,
                                      _2,
                                      _3,
                                      _4));
//..
// Finally, we call the 'bteso_ResolveUtil::getAddress' method to retrieve the
// IPv4 address of 'www.bloomberg.com':
//..
    bteso_IPv4Address ipv4;
    bteso_ResolveUtil::getAddress(&ipv4, "www.bloomberg.com");
    bsl::cout << "IP Address: " << ipv4 << std::endl;
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
        // clear
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache
        //
        // resolveaddress multithread
        // --------------------------------------------------------------------
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
                    LOOP_ASSERT(LINE, 0 == X.lookupAddress(&mV, NAME, 1));
                }
                else {
                    LOOP_ASSERT(LINE, 0 != X.lookupAddress(&mV, NAME, 1));
                }

                LOOP2_ASSERT(LINE, NAME,
                             0 == X.resolveAddress(&mV, NAME, NUM_ADDR, &err));

                if (!CACHED) {
                    ++count;
                }

                LOOP3_ASSERT(LINE, TestResolver::count(), count,
                             TestResolver::count() == count);

                LOOP3_ASSERT(LINE, ADDR, mV[0], ADDR == mV[0]);

                LOOP_ASSERT(LINE, 0 == X.lookupAddress(&mV, NAME, 1));
            }
        }

        TestResolver::reset();

        if (verbose) { cout << "Test expiration time" << endl; }
        {
            bslma_TestAllocator oa("object",  veryVeryVeryVerbose);
            Obj X(&TestResolver::callback, &oa);
            X.setTimeToLiveInSeconds(2);

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
        //   bool timeToLiveInSeconds() const;
        //   int resolverCallback() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        // Attribute Types

        typedef Obj::ResolveByNameCallback T1;     // 'resolverCallback'
        typedef int                        T2;     // 'timeToLiveInSeconds'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = bteso_ResolveUtil::defaultResolveByNameCallback();
                               // 'resolverCallback'
        const T2 D2 = 3600;    // 'timeToLiveInSeconds'

        // -------------------------------------------------------
        // 'A' values
        // -------------------------------------------------------

        const T1 A1   = &TestResolver::callback;
        const T2 A2   = 1;

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

            const T2& timeToLiveInSeconds = X.timeToLiveInSeconds();
            LOOP2_ASSERT(D2, timeToLiveInSeconds, D2 == timeToLiveInSeconds);

            ASSERT(&oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(timeToLiveInSeconds) }
        {
            mX.setTimeToLiveInSeconds(A2);

            bslma_TestAllocatorMonitor oam(oa), dam(da);

            const T2& timeToLiveInSeconds = X.timeToLiveInSeconds();
            LOOP2_ASSERT(A2, timeToLiveInSeconds, A2 == timeToLiveInSeconds);

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
        //   setTimeToLiveInSeconds(bool value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        // Attribute Types

        typedef Obj::ResolveByNameCallback T1;     // 'resolverCallback'
        typedef int                        T2;     // 'timeToLiveInSeconds'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const T1 D1 = bteso_ResolveUtil::defaultResolveByNameCallback();
                               // 'resolverCallback'
        const T2 D2 = 3600;    // 'timeToLiveInSeconds'

        // 'A' values

        const T1 A1   = &TestResolver::callback;
        const T2 A2   = 1;

        // 'B' values

        const T1 B1;            // Default constructed value.
        const T2 B2 = INT_MAX;

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
            LOOP3_ASSERT(CONFIG, D2, X.timeToLiveInSeconds(),
                         D2 == X.timeToLiveInSeconds());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'timeToLiveInSeconds'
            {
                bslma_TestAllocatorMonitor tam(oa);

                mX.setTimeToLiveInSeconds(A2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, A2 == X.timeToLiveInSeconds());

                mX.setTimeToLiveInSeconds(B2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, B2 == X.timeToLiveInSeconds());

                mX.setTimeToLiveInSeconds(D2);
                LOOP_ASSERT(CONFIG, D1 == X.resolverCallback());
                LOOP_ASSERT(CONFIG, D2 == X.timeToLiveInSeconds());

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

            bslma_TestAllocator da("default",   veryVeryVeryVerbose);
            bslma_Default::setDefaultAllocatorRaw(&da);

            Obj obj;

            if (veryVerbose) cout << "\tdescription" << endl;
            {
                ASSERT_SAFE_PASS(obj.setTimeToLiveInSeconds(1));
                ASSERT_SAFE_FAIL(obj.setTimeToLiveInSeconds(0));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCacheEntry
        //
        // lock
        // --------------------------------------------------------------------
        /*
        enum {
            MAX_SLEEP_CYCLES = 1000,
            SLEEP_MS = 100
        };

        {
            Entry mX;  const Entry &X = mX;
            X.lock();

            ThreadInfo args;
            args.d_lock = &mX;

            args.d_retval = 0;
            args.d_retvalSet = 0;
            bcemt_ThreadAttributes attr;
            attr.setDetachedState(
                                bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
            bcemt_ThreadUtil::Handle dum;
            bcemt_ThreadUtil::create(&dum, attr, &MyThread, &args);

            for (int i = 0; 0 == args.d_retvalSet && i < MAX_SLEEP_CYCLES;
                 ++i) {
                bcemt_ThreadUtil::microSleep(1000 * SLEEP_MS);
            }
            ASSERT(args.d_retvalSet);
            ASSERT(0 != args.d_retval); // should fail
            if (verbose) {
                P(args.d_retval);
            }

            X.unlock();

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
        */
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

        /*
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

        const T1  A1   = Entry::DataPtr(new Data(Vec(), bdet_TimeInterval()));

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
        */
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
        //   setTimeToLiveInSeconds(bool value);
        //   setResolverCallback(int value);
        // --------------------------------------------------------------------

        /*
        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const Entry::DataPtr  D1;

        // 'A' values

        const Entry::DataPtr  A1(new Data(vector<bteso_IPv4Address>(),
                                          bdet_TimeInterval(0.0),
                                          &scratch));

        // 'B' values

        Entry::DataPtr b1;
        b1.createInplace(&scratch,
                         vector<bteso_IPv4Address>(),
                         bdet_TimeInterval(0.0),
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
                ASSERT(A1 == X.data());

                mX.setData(B1);
                ASSERT(B1 == X.data());
            }
        }
        */
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // bteso_IpResolutionCache_Data Value CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1, 3..11)
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   2 For each of the three iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma_TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   baetzo_LocalTimeDescriptor(int o, bool f, const SRef& d, *bA = 0);
        // --------------------------------------------------------------------

        /*
        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        static const struct {
            int                d_line;
            char               d_mem;
            const char        *d_ip;
            bsls_Types::Int64  d_time;
        } DATA[] = {
            { __LINE__,  'Y', "0.0.0.0",          LLONG_MIN },
            { __LINE__,  'Y', "256.256.256.256",         -1 },
            { __LINE__,  'Y', "0.0.0.1",                  0 },
            { __LINE__,  'Y', "0.0.1.0",                  1 },
            { __LINE__,  'Y', "0.1.0.0",          LLONG_MAX },
            { __LINE__,  'Y', "1.0.0.0",                  0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int               LINE = DATA[ti].d_line;
                const char              MEM  = DATA[ti].d_mem;
                const bteso_IPv4Address IP   =
                                           bteso_IPv4Address(DATA[ti].d_ip, 0);
                const bdet_TimeInterval TIME =
                                         bdet_TimeInterval(DATA[ti].d_time, 0);
                const vector<bteso_IPv4Address> ADDR(1, IP, &scratch);

                //if (veryVerbose) { T_ P_(OFFSET) P_(FLAG) P(DESC) }

                LOOP2_ASSERT(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma_TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma_Default::setDefaultAllocatorRaw(&da);

                    Data                *objPtr;
                    bslma_TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Data(ADDR, TIME);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Data(ADDR, TIME, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Data(ADDR, TIME, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP2_ASSERT(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Data) == fa.numBytesInUse());

                    Data& mX = *objPtr;  const Data& X = mX;

                    bslma_TestAllocator&  oa = *objAllocatorPtr;
                    bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    // TBD: FIXME
                    LOOP4_ASSERT(LINE, CONFIG, ADDR[0], X.addresses()[0],
                                 ADDR == X.addresses());

                    LOOP4_ASSERT(LINE, CONFIG, TIME, X.expirationTime(),
                                 TIME == X.expirationTime());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        LOOP4_ASSERT(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());

                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
        */

        /*
        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            // Note that any string arguments are now of type 'string', which
            // require their own "scratch" allocator.

            bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_line;
                const char   MEM    = DATA[ti].d_mem;
                const int    OFFSET = DATA[ti].d_utcOffsetInSeconds;
                const bool   FLAG   = DATA[ti].d_dstInEffectFlag;
                const string DESC(DATA[ti].d_description, &scratch);

                if (veryVerbose) { T_ P_(MEM) P_(OFFSET) P_(FLAG) P(DESC) }

                bslma_TestAllocator da("default",  veryVeryVeryVerbose);
                bslma_TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma_Default::setDefaultAllocatorRaw(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Data obj(OFFSET, FLAG, DESC, &sa);
                    LOOP3_ASSERT(LINE, OFFSET, obj.resolverCallback(),
                                 OFFSET == obj.resolverCallback());
                    LOOP3_ASSERT(LINE, FLAG, obj.timeToLiveInSeconds(),
                                 FLAG == obj.timeToLiveInSeconds());
                    LOOP3_ASSERT(LINE, DESC, obj.description(),
                                 DESC == obj.description());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }
        */
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
            Obj cache;
            using namespace bdef_PlaceHolders;
            bteso_ResolveUtil::setResolveByNameCallback(
                       bdef_BindUtil::bind(&bteso_IpResolutionCache::resolveAddress,
                                           &cache,
                                           _1,
                                           _2,
                                           _3,
                                           _4));

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

        Obj cache;
        using namespace bdef_PlaceHolders;
        bteso_ResolveUtil::setResolveByNameCallback(
                       bdef_BindUtil::bind(&bteso_IpResolutionCache::resolveAddress,
                                           &cache,
                                           _1,
                                           _2,
                                           _3,
                                           _4));

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
