// bdlm_metricsregistry.t.cpp                                         -*-C++-*-
#include <bdlm_metricsregistry.h>

#include <bdlm_metricdescriptor.h>

#include <bdlf_bind.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_functional.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#endif

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 6] static MetricsRegistry& defaultInstance();
//
// CREATORS
// [ 2] MetricsRegistry(bslma::Allocator *basicAllocator = 0);
// [ 2] ~MetricsRegistry();
//
// MANIPULATORS
// [ 2] void registerCollectionCallback(*result, descriptor, callback);
// [ 2] void removeMetricsAdapter(MetricsAdapter *adapter);
// [ 2] void setMetricsAdapter(MetricsAdapter *adapter);
//
// ACCESSORS
// [ 3] bslma::Allocator *allocator() const;
// [ 3] int numRegisteredCollectionCallbacks() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 7] DRQS 174793420
// [ 5] CONCERN: THE DESTRUCTOR UNREGISTERS ALL FROM THE ADAPTER
// [ 4] CONCERN: REGISTRATION HANDLE CAN OUTLIVE THE REGISTRY

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

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlm::MetricsRegistry                   Obj;
typedef bdlm::MetricsRegistryRegistrationHandle ObjHandle;

const char longName[] = "abcdefghijklmnopqrstuvwxyz01234567890123456789012345";
BSLMF_ASSERT(sizeof(longName) > sizeof(bsl::string));

// ============================================================================
//                   GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int s_testMetricCount = 0;

void testMetric(BloombergLP::bdlm::Metric *value)
{
    (void)value;
    ++s_testMetricCount;
}

// ============================================================================
//                       GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                           // ===================
                           // class LargeCallback
                           // ===================

/// This class implements the callback type `MetricsAdapter::Callback` and
/// is very large so as to trigger allocation.
class LargeCallback {

    // DATA
    char d_large[1024];

    public:
    // MANIPULATORS

    /// Do nothing.
    void operator()(BloombergLP::bdlm::Metric*);
};

                           // ===================
                           // class LargeCallback
                           // ===================

void LargeCallback::operator()(BloombergLP::bdlm::Metric*)
{
}

                         // ===============
                         // class Callback1
                         // ===============
struct Callback1 {
    void operator()(BloombergLP::bdlm::Metric*) const {}
};
                         // ===============
                         // class Callback2
                         // ===============
struct Callback2 {
    void operator()(BloombergLP::bdlm::Metric*) const {}
};

                         // ========================
                         // class TestMetricsAdapter
                         // ========================

/// This class implements a pure abstract interface for clients and
/// suppliers of metrics adapters.  The implemtation does not register
/// callbacks with any monitoring system, but does track registrations to
/// enable testing of thread-enabled objects metric registration.
class TestMetricsAdapter : public bdlm::MetricsAdapter {

    // PRIVATE TYPES
    typedef bsl::pair<bdlm::MetricDescriptor, Callback> MetricPair;

    typedef bsl::map<int, MetricPair> Map;

    // DATA
    Map d_descriptors;
    int d_next;

  public:
    // CREATORS

    /// Create a `TestMetricsAdapter`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    TestMetricsAdapter(bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~TestMetricsAdapter() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Save the specified `metricsDescriptor` and `callback` pair.
    /// Return a callback handle that will be verified in
    /// `removeCollectionCallback`.
    CallbackHandle registerCollectionCallback(
                 const bdlm::MetricDescriptor& metricDescriptor,
                 const Callback&               callback) BSLS_KEYWORD_OVERRIDE;

    int removeCollectionCallback(const CallbackHandle& handle)
                                                         BSLS_KEYWORD_OVERRIDE;
        // Do nothing with the specified `handle`.  Assert the supplied
        // `handle` matches what was provided by `registerCollectionCallback`.
        // Return 0.

    // ACCESSORS

    /// Return the number of registered metrics.
    int size() const;

    /// Return `true` if this adapter contains a pair of the specified
    /// `descriptor` and `Callback`, and `false` otherwise.
    template <class Callback>
    bool contains(const bdlm::MetricDescriptor& descriptor) const;
};

                         // ------------------------
                         // class TestMetricsAdapter
                         // ------------------------

// CREATORS
TestMetricsAdapter::TestMetricsAdapter(bslma::Allocator *basicAllocator)
: d_descriptors(basicAllocator)
, d_next(1000)
{
}

TestMetricsAdapter::~TestMetricsAdapter()
{
}

// MANIPULATORS
bdlm::MetricsAdapter::CallbackHandle
                                TestMetricsAdapter::registerCollectionCallback(
                                const bdlm::MetricDescriptor& metricDescriptor,
                                const Callback&               callback)
{
    d_descriptors[d_next] = bsl::make_pair(metricDescriptor, callback);
    return d_next++;
}

int TestMetricsAdapter::removeCollectionCallback(const CallbackHandle& handle)
{
    d_descriptors.erase(handle);
    return 0;
}

// ACCESSORS
int TestMetricsAdapter::size() const
{
    return static_cast<int>(d_descriptors.size());
}

template <class t_CALLBACK>
bool TestMetricsAdapter::contains(const bdlm::MetricDescriptor& descriptor)
                                                                          const
{
    for (Map::const_iterator it  = d_descriptors.begin();
                             it != d_descriptors.end();
                           ++it) {
        if (it->second.first == descriptor
        &&  it->second.second.target<t_CALLBACK>()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bdlm::MetricsRegistry`
///- - - - - - - - - - - - - - - - - - - -
// This example demonstrates the initialization and usage of the
// `bdlm::MetricsRegistry` object, allowing for registering metric callback
// functions with the `bdlm` monitoring system.
//
// First, we declare a class that provides some metric for the `bdlm`
// monitoring system:
// ```
    class LowLevelFacility {
        // PRIVATE DATA
        bdlm::MetricsRegistryRegistrationHandle d_metricHandle;
      public:
        // CREATORS
        explicit LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry =
                                     bdlm::MetricsRegistry::defaultInstance());

        // ACCESSORS
        int someMetric() const
        {
            return 0; // just a stub
        }
    };
// ```
// Next, we provide a metric function to be used during callback registration:
// ```
    void metricCallback(bdlm::Metric *value, const LowLevelFacility *object)
    {
        *value = bdlm::Metric::Gauge(object->someMetric());
    }
// ```
// Here is the constructor definition that registers the collection callback:
// ```
    LowLevelFacility::LowLevelFacility(bdlm::MetricsRegistry& metricsRegistry)
    {
        // Construct a `bdlm::MetricsDescriptor` object to be used when
        // registering the callback function:
        bdlm::MetricDescriptor descriptor("bdlm",
                                          "example",
                                          1,
                                          "bdlmmetricsregistry",
                                          "bmr",
                                          "identifier");

        // Register the collection callback:
        metricsRegistry.registerCollectionCallback(
                                   &d_metricHandle,
                                   descriptor,
                                   bdlf::BindUtil::bind(&metricCallback,
                                                        bdlf::PlaceHolders::_1,
                                                        this));
        ASSERT(d_metricHandle.isRegistered());
    }
// ```
// Notice that the compiler-supplied destructor is sufficient because the
// `d_metricHandle` will deregister the metric on destruction.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: `BSLS_REVIEW` failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // Access the default instance before assign the global allocator
    Obj::defaultInstance();

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Allocator *savedGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    bool expectGlobalAllocation = false;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
    bool expectDefaultAllocation = false;

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, replace
        //    leading comment characters with spaces, replace `assert` with
        //    `ASSERT`, and insert `if (veryVerbose)` before all output
        //    operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Now, we construct a `bdlm::MetricsRegistry` object with a test allocator:
// ```
    bslma::TestAllocator  ta;
    bdlm::MetricsRegistry registry(&ta);
    ASSERT(registry.numRegisteredCollectionCallbacks() == 0);
// ```
// Then, we create the object and pass the constructed `bdlm::MetricsRegistry`
// object there:
// ```
    {
        LowLevelFacility facility(registry);
        ASSERT(registry.numRegisteredCollectionCallbacks() == 1);
// ```
// If we don't provide a `bdlm::MetricsRegistry` object explicitly, the default
// global instance will be used.
//
// Finally, the callback is removed the monitoring system by the destructor of
// `facility` object:
// ```
    } // `facility.d_metricHandle.unregister()` is called here
    ASSERT(registry.numRegisteredCollectionCallbacks() == 0);
// ```
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // DRQS 174793420
        //
        // Concerns:
        // 1. All allocations are from the specified allocator.
        //
        // Plan:
        // 1. Provide an allocator to an object, then exercise it while
        //    monitoring the default and global allocators.
        //
        // Testing:
        //   DRQS 174793420
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DRQS 174793420" << endl
                          << "==============" << endl;

        bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&oa);

        bdlm::MetricDescriptor md(longName,
                                  longName,
                                  1,
                                  longName,
                                  longName,
                                  longName,
                                  &oa);

        typedef bsl::function<void(BloombergLP::bdlm::Metric*)> Callback;
        Callback fn(bsl::allocator_arg, &oa, LargeCallback());
        BloombergLP::bdlm::MetricsRegistryRegistrationHandle handle;
        mX.registerCollectionCallback(&handle, md, fn);

        ASSERTV(da.numBytesInUse(),
                da.numBlocksInUse(),
                0 == da.numBlocksInUse());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD `defaultInstance`
        //
        // Concerns:
        // 1. The `defaultInstance` class method returns a reference to default
        //    instance of `MetricsRegistry`.
        //
        // 2. Subsequent calls return the same instance.
        //
        // 3. The instance uses global allocator.
        //
        // Plan:
        // 1. Call the class method and save the returned reference.  (C-1)
        //
        // 2. Call the class method again.  Verify that the same reference has
        //    been returned.  (C-2)
        //
        // 3. Save the allocator set before `setGlobalAllocator()` call.
        //    Verify that `defaultInstance().allocator()` returns the same
        //    allocator.  (C-3)
        //
        // Testing:
        //   static MetricsRegistry& defaultInstance();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCLASS METHOD `defaultInstance`"
                             "\n==============================" << endl;

        Obj& mX = Obj::defaultInstance();

        ASSERT(&Obj::defaultInstance() == &mX);

        ASSERT(Obj::defaultInstance().allocator() == savedGlobalAllocator);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: THE DESTRUCTOR UNREGISTERS ALL FROM THE ADAPTER
        //
        // Concerns:
        // 1. The destructor unregisters all the registered metric callbacks
        //    from the associated adapter.
        //
        // Plan:
        // 1. Create a metric adapter object.
        //
        // 2. In a nested scope create a metric registry object, associate the
        //    adapter with it, and register 2 callbacks.  Verify that the
        //    adapter contains the registered callbacks.
        //
        // 3. Close the nested scope causing the metric registry end-of-life.
        //
        // 4. Verify that the adapter is empty.
        //
        // Testing:
        //   CONCERN: THE DESTRUCTOR UNREGISTERS ALL FROM THE ADAPTER
        // --------------------------------------------------------------------

        if (verbose) cout <<
          "\nCONCERN: THE DESTRUCTOR UNREGISTERS ALL FROM THE ADAPTER"
          "\n========================================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        TestMetricsAdapter adapter(&oa);
        ASSERTV(adapter.size(), adapter.size() == 0);

        bdlm::MetricDescriptor descriptor1("mns", "metric1", 1,
                                           "otn1", "ota1", "oi1"),
                               descriptor2("mns", "metric2", 2,
                                           "otn2", "ota2", "oi2");
        ObjHandle handle1, handle2;
        {
            Obj mX(&oa);
            mX.setMetricsAdapter(&adapter);
            mX.registerCollectionCallback(&handle1,
                                          descriptor1,
                                          Callback1());
            mX.registerCollectionCallback(&handle2,
                                          descriptor2,
                                          Callback2());
            ASSERT(handle1.isRegistered());
            ASSERT(handle2.isRegistered());
            ASSERTV(mX.numRegisteredCollectionCallbacks(),
                    mX.numRegisteredCollectionCallbacks() == 2);
            ASSERTV(adapter.size(), adapter.size() == 2);
        }
        ASSERT(!handle1.isRegistered());
        ASSERT(!handle2.isRegistered());

        ASSERTV(adapter.size(), adapter.size() == 0);

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // No memory allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: REGISTRATION HANDLE CAN OUTLIVE THE REGISTRY
        //
        // Concerns:
        // 1. When metric registry object ends its lifetime, all registered
        //    callbacks it owns are unregistered.  All handles associated with
        //    the registered callbacks become "unregistered".
        //
        // Plan:
        // 1. Create two `bslma::TestAllocator` objects, and install one as
        //    the current default allocator.
        //
        // 2. Create a `MetricDescriptor` object and a registraiton handle.
        //
        // 3. Open a nested scope.  Use the default constructor, using the
        //    other test allocator from P-1, to create a registry object.
        //
        // 4. Register a new callback.  Verify it was successful.
        //
        // 5. Close the nested scope causing the metric registry end-of-life.
        //    Verify that the `isRegistered` accessor of the handle returns
        //    false.
        //
        // 6. Verify that no memory allocated from the default allocator.
        //
        // Testing:
        //   CONCERN: REGISTRATION HANDLE CAN OUTLIVE THE REGISTRY
        // --------------------------------------------------------------------

        if (verbose) cout <<
             "\nCONCERN: REGISTRATION HANDLE CAN OUTLIVE THE REGISTRY"
             "\n=====================================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        bdlm::MetricDescriptor descriptor("mns", "mn", 1,
                                          "otn", "ota", "oi");
        ObjHandle handle;
        {
            Obj mX(&oa);
            mX.registerCollectionCallback(&handle,
                                          descriptor,
                                          &testMetric);
            ASSERT(handle.isRegistered());
            ASSERTV(mX.numRegisteredCollectionCallbacks(),
                    mX.numRegisteredCollectionCallbacks() == 1);
        }
        ASSERT(!handle.isRegistered());

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // No memory allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        // 1. Each accessor returns the value of the corresponding attribute
        //    of the object.
        //
        // 2. Each accessor method is declared `const`.
        //
        // 3. No accessor allocates any memory.
        //
        // Plan:
        // 1. Create two `bslma::TestAllocator` objects, and install one as
        //    the current default allocator.
        //
        // 2. Use the default constructor, using the other test allocator
        //    from P-1, to create an object.
        //
        // 3. Verify that each basic accessor, invoked on a `const` reference
        //    to the object created in P-2, returns the expected value.  (C-2)
        //
        // 4. Verify that the `allocator` accessor returns the allocator
        //    provided during construction.  (C-1)
        //
        // 5. Verify that the `numRegisteredCollectionCallbacks` accessor
        //    returns 0.  (C-1)
        //
        // 6. Register a new callback.  Save the returned handle.  Verify that
        //    `numRegisteredCollectionCallbacks` accessor returns 1.
        //
        // 7. Register one more callback.  Save the returned handle.  Verify
        //    that `numRegisteredCollectionCallbacks` accessor returns 2.
        //
        // 8. Call `unregister()` with one of the saved handles.  Verify that
        //    `numRegisteredCollectionCallbacks` accessor returns 1.
        //
        // 9. Invoke the destructor of the second handle.  Verify that
        //    `numRegisteredCollectionCallbacks` accessor returns 0 again.
        //
        // 7. Monitor the memory allocated from both the default and object
        //    allocators before and after calling the accessor; verify that
        //    there is no change in total memory allocation.  (C-3)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   int numRegisteredCollectionCallbacks() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBASIC ACCESSORS"
                             "\n===============" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&oa);  const Obj& X = mX;

        ASSERTV(X.allocator(), X.allocator() == &oa);

        ASSERTV(X.numRegisteredCollectionCallbacks(),
                X.numRegisteredCollectionCallbacks() == 0);

        {
            bdlm::MetricDescriptor descriptor("mns", "mn", 1,
                                              "otn", "ota", "oi");

            // Add one callback
            ObjHandle handle1;
            mX.registerCollectionCallback(&handle1,
                                          descriptor,
                                          &testMetric);
            ASSERT(handle1.isRegistered());
            ASSERTV(X.numRegisteredCollectionCallbacks(),
                    X.numRegisteredCollectionCallbacks() == 1);

            // Add one more callback
            ObjHandle handle2;
            mX.registerCollectionCallback(&handle2,
                                          descriptor,
                                          &testMetric);
            ASSERT(handle2.isRegistered());
            ASSERTV(X.numRegisteredCollectionCallbacks(),
                    X.numRegisteredCollectionCallbacks() == 2);

            // Unregister the callback
            int rc = handle2.unregister();
            ASSERTV(rc, rc == 0);
            ASSERTV(X.numRegisteredCollectionCallbacks(),
                    X.numRegisteredCollectionCallbacks() == 1);

        }  // Destructor of `handle1` is invoked here
        ASSERTV(X.numRegisteredCollectionCallbacks(),
                X.numRegisteredCollectionCallbacks() == 0);

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // No memory allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        // 1. An object can be created using the default constructor (with or
        //    without a supplied allocator).
        //
        // 2. If an allocator is NOT supplied to the default constructor, the
        //    default allocator in effect at the time of construction becomes
        //    the object allocator for the resulting object.
        //
        // 3. If an allocator IS supplied to the default constructor, that
        //    allocator becomes the object allocator for the resulting object.
        //
        // 4. Supplying a default-constructed allocator has the same effect as
        //    not supplying an allocator.
        //
        // 5. Any memory allocation is from the object allocator.
        //
        // 6. There is no temporary allocation from any allocator.
        //
        // 7. Every object releases any allocated memory at destruction.
        //
        // 8. A successful call of the `registerCollectionCallback` manipulator
        //    increments the `numRegisteredCollectionCallbacks` accessor value.
        //
        // 9. A registered callback is registered until the handle object,
        //    returned by the `registerCollectionCallback` call, is alive.  End
        //    of its life (the destructor call) decrements  the
        //    `numRegisteredCollectionCallbacks` accessor value.
        //
        // 10. The `setMetricsAdapter` call registers all the collection
        //    callbacks with the specified adapter.
        //
        // 11. The `setMetricsAdapter` call unregisters all the collection
        //    callbacks from the previous adapter, if any.
        //
        // 12. The `removeMetricsAdapter` call disassociates the current adapter
        //    if its address is passed to the call.
        //
        // 13. The `removeMetricsAdapter` call unregisters all the collection
        //    callbacks from the current adapter if its address is passed to
        //    the call.
        //
        // 14. The same callback can be registered more than once.
        //
        // 15. Unregistering callbacks from the registry unregisters them from
        //    the adapter.
        //
        // 16. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Execute an inner loop that creates an object by
        //    default-construction, but invokes the default constructor
        //    differently in each iteration: (a) without passing an allocator,
        //    (b) passing a default-constructed allocator explicitly, and (c)
        //    passing the address of a test allocator distinct from the
        //    default.  For each of these iterations:  (C-1..14)
        //
        //   1. Create three `bslma::TestAllocator` objects, and install one as
        //      as the current default allocator.
        //
        //   2. Use the default constructor to dynamically create an object
        //      `X`, with its object allocator configured appropriately; use a
        //      distinct test allocator for the object's footprint.
        //
        //   3. Use the `allocator` accessor to ensure that its object
        //      allocator is properly installed.  (C-2..4)
        //
        //   4. Create 2 `MetricsAdapter` instances: `adapter1` & `adapter2`.
        //      Verify that no callbacks are registered.
        //
        //   5. Register one callback using the `registerCollectionCallback`
        //      manipulator.  Save the returned handle. Verify that the
        //      `numRegisteredCollectionCallbacks` accessor returns 1.  Verify
        //      that `adapter1` & `adapter2` still have no registered
        //      callbacks.  (C-8..9)
        //
        //   6. Call `setMetricsAdapter(&adapter1)`.  Verify that `adapter1`
        //      has 1 registered callback, and `adapter2` has no registered
        //      callbacks.  (C-10)
        //
        //   7. Call `setMetricsAdapter(&adapter2)`.  Verify that `adapter2`
        //      has 1 registered callback, and `adapter1` has no registered
        //      callbacks.  (C-10..11)
        //
        //   8. Call `removeMetricsAdapter(&adapter2)`.  Verify that `adapter1`
        //      & `adapter2` have no registered callbacks.  (C-12..13)
        //
        //   9. Destroy the handle from P-5.  Verify that the
        //      `numRegisteredCollectionCallbacks` accessor returns 0.  (C-9)
        //
        //  10. Register the same callback 2 times.  Verify that the registry
        //      contains 2 registered callbacks.  Clear the registry.
        //
        //  11. Associate an adapter with the registry.  Register 2 callbacks.
        //      Verify that the adapter contains these callbacks.  Unregister
        //      the callbacks one by one from the registry.  After each step
        //      verify that the callback was unregistered from the adapter.
        //
        //  12. Verify that no temporary memory is allocated from the object
        //      allocator.  (C-6)
        //
        //  13. Verify that all object memory is released when the object is
        //      destroyed.  (C-7)
        //
        // 2. Verify that, in appropriate build modes, defensive checks are
        //    triggered for invalid attribute values, but not triggered for
        //    adjacent valid ones (using the `BSLS_ASSERTTEST_*` macros).
        //    (C-16)
        //
        // Testing:
        //   MetricsRegistry(bslma::Allocator *basicAllocator = 0);
        //   ~MetricsRegistry();
        //   void registerCollectionCallback(*result, descriptor, callback);
        //   void removeMetricsAdapter(MetricsAdapter *adapter);
        //   void setMetricsAdapter(MetricsAdapter *adapter);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                        "\n==========================================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(0);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(0 == "Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // ------------------------------
            // Verify the object's allocator.
            // ------------------------------

            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                    X.numRegisteredCollectionCallbacks() == 0);

            // ------------------------
            // Verify the manipulators.
            // ------------------------

            {
                bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
                bdlm::MetricDescriptor descriptor1("mns", "metric1", 1,
                                                   "otn1", "ota1", "oi1"),
                                       descriptor2("mns", "metric2", 2,
                                                   "otn2", "ota2", "oi2");
                TestMetricsAdapter adapter1(&ta),
                                   adapter2(&ta);
                ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);
                ASSERTV(CONFIG, adapter2.size(), adapter2.size() == 0);
                {
                    // Register 2 metric callbacks
                    ObjHandle handle1;
                    mX.registerCollectionCallback(&handle1,
                                                  descriptor1,
                                                  Callback1());
                    ASSERTV(CONFIG, handle1.isRegistered());

                    ObjHandle handle2;
                    mX.registerCollectionCallback(&handle2,
                                                  descriptor2,
                                                  Callback2());
                    ASSERTV(CONFIG, handle2.isRegistered());

                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 2);
                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);
                    ASSERTV(CONFIG, adapter2.size(), adapter2.size() == 0);

                    // Now set the metric adapter
                    mX.setMetricsAdapter(&adapter1);

                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 2);
                    ASSERTV(CONFIG, adapter1.contains<Callback1>(descriptor1));
                    ASSERTV(CONFIG, adapter1.contains<Callback2>(descriptor2));

                    ASSERTV(CONFIG, adapter2.size(), adapter2.size() == 0);

                    // Replace the metric adapter
                    mX.setMetricsAdapter(&adapter2);

                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);

                    ASSERTV(CONFIG, adapter2.size(), adapter2.size() == 2);
                    ASSERTV(CONFIG, adapter2.contains<Callback1>(descriptor1));
                    ASSERTV(CONFIG, adapter2.contains<Callback2>(descriptor2));

                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 2);

                    // Remove the metric adapter
                    mX.removeMetricsAdapter(&adapter2);

                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);
                    ASSERTV(CONFIG, adapter2.size(), adapter2.size() == 0);
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 2);
                }
                ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                        X.numRegisteredCollectionCallbacks() == 0);

                // Register the same callback twice
                {
                    ObjHandle handle1, handle2;
                    mX.registerCollectionCallback(&handle1,
                                                  descriptor1,
                                                  Callback1());
                    ASSERTV(CONFIG, handle1.isRegistered());
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 1);

                    mX.registerCollectionCallback(&handle2,
                                                  descriptor1,
                                                  Callback1());
                    ASSERTV(CONFIG, handle2.isRegistered());
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 2);
                }
                ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                        X.numRegisteredCollectionCallbacks() == 0);

                // Unregistering callbacks from the registry unregisters them
                // from the adapter
                {
                    mX.setMetricsAdapter(&adapter1);
                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);

                    // Register 2 metric callbacks
                    ObjHandle handle1, handle2;
                    mX.registerCollectionCallback(&handle1,
                                                  descriptor1,
                                                  Callback1());
                    mX.registerCollectionCallback(&handle2,
                                                  descriptor2,
                                                  Callback2());
                    ASSERTV(CONFIG, handle1.isRegistered());
                    ASSERTV(CONFIG, handle2.isRegistered());
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 2);

                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 2);
                    ASSERTV(CONFIG, adapter1.contains<Callback1>(descriptor1));
                    ASSERTV(CONFIG, adapter1.contains<Callback2>(descriptor2));

                    // Unregister one of the callbacks
                    handle1.unregister();
                    ASSERTV(CONFIG, !handle1.isRegistered());
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 1);

                    // Verify it was unregistered from the adapter
                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 1);
                    ASSERTV(CONFIG,!adapter1.contains<Callback1>(descriptor1));
                    ASSERTV(CONFIG, adapter1.contains<Callback2>(descriptor2));

                    // Unregister the second
                    handle2.unregister();
                    ASSERTV(CONFIG, !handle2.isRegistered());
                    ASSERTV(CONFIG, X.numRegisteredCollectionCallbacks(),
                            X.numRegisteredCollectionCallbacks() == 0);

                    ASSERTV(CONFIG, adapter1.size(), adapter1.size() == 0);
                }
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            ASSERTV(CONFIG, oa.numBlocksMax(), 3 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;
            bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

            Obj obj(&ta);
            TestMetricsAdapter adapter(&ta);

            ASSERT_FAIL(obj.setMetricsAdapter(       0));
            ASSERT_PASS(obj.setMetricsAdapter(&adapter));

            ASSERT_FAIL(obj.removeMetricsAdapter(       0));
            ASSERT_PASS(obj.removeMetricsAdapter(&adapter));
        }
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
        // 1. Exercise the class.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);

        bdlm::MetricDescriptor descriptor("mns", "mn", 1, "otn", "ota", "oi");
        TestMetricsAdapter adapter(&ta);

        Obj mX(&ta);

        ASSERT(0 == adapter.size());

        ObjHandle handle;
        mX.registerCollectionCallback(&handle, descriptor, &testMetric);
        typedef void (*CallbackPtr)(BloombergLP::bdlm::Metric*);
        {
            // Verify that `CallbackPtr` is `decltype(&testMetric)`
            CallbackPtr ptr = &testMetric;
            (void) ptr;
        }

        ASSERT(0 == adapter.size());

        mX.setMetricsAdapter(&adapter);

        ASSERT(adapter.size() == 1 &&
               adapter.contains<CallbackPtr>(descriptor));

        mX.removeMetricsAdapter(&adapter);

        ASSERT(0 == adapter.size());

        mX.setMetricsAdapter(&adapter);

        ASSERT(adapter.size() == 1 &&
               adapter.contains<CallbackPtr>(descriptor));

        handle.unregister();

        ASSERT(0 == adapter.size());
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    if (!expectGlobalAllocation) {
        ASSERTV(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());
    }

    // CONCERN: In no case does memory come from the default allocator.

    if (!expectDefaultAllocation) {
        ASSERTV(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
