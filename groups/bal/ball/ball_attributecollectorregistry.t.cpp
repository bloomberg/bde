// ball_attributecollectorregistry.t.cpp                              -*-C++-*-

#include <ball_attributecollectorregistry.h>
#include <ball_attribute.h>
#include <ball_managedattribute.h>

#include <bdlb_variant.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_usesallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_climits.h>     // 'INT_MIN', 'INT_MAX'
#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a registry of the attribute collector
// functors that are used to collect and process a collection of log
// attributes.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ball::AttributeCollectorRegistry();
// [ 2] ball::AttributeCollectorRegistry(const allocator_type& a);
// [ 2] ~ball::AttributeCollectorRegistry();
//
// MANIPULATORS
// [ 2] int addCollector(const Collector&, const bsl::string_view& );
// [ 2] void removeAll();
// [ 2] int removeCollector(const bsl::string_view&);
//
// ACCESSORS
// [ 4] void collect(const Visitor& visitor) const;
// [ 3] allocator_type get_allocator() const;
// [ 3] bool hasCollector(const bsl::string_view& name) const;
// [ 3] int numCollectors() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ *] CONCERN: All memory allocation is from the object's allocator.

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

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                      CONVENIENCE MACROS
// ----------------------------------------------------------------------------

// For use in ASSERTV macro invocations to print allocator.
#define ALLOC_OF(EXPR) (EXPR).get_allocator().mechanism()

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::AttributeCollectorRegistry Obj;
typedef Obj::allocator_type              AllocType; // Test allocator trait.

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT((bsl::uses_allocator<Obj, bsl::allocator<char> >::value));

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace USAGE_EXAMPLE {

void printAttribute(bsl::ostream *stream, const ball::Attribute& attribute)
    // Write the value of the specified 'attribute' to the specified output
    // 'stream' in a human-readable format.
{
    *stream << attribute.name() << "=" << attribute.value() << " ";
}

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Collecting attributes from non-uniform sources
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we will collect a set of application properties and perform
// some manipulation of the collected data.
//
//..
// First, we define a few collector functions that will collect the application
// properties from various parts of application and call the specified visitor
// functor for every collected attribute:
//..
    void userInfo(
                   const bsl::function<void(const ball::Attribute &)>& visitor)
    {
        int         uuid     = 12345;    // getUuid();
        bsl::string userName = "proxy";  // getUserName();

        visitor(ball::Attribute("myLib.uuid", uuid));
        visitor(ball::Attribute("myLib.user", userName));
    }
    void threadInfo(
                   const bsl::function<void(const ball::Attribute &)>& visitor)
    {
        int threadId = 87654;            // getThreadId();

        visitor(ball::Attribute("myLib.threadId", threadId));
    }

    void usageExample1()
    {
//  Then, we register collector functions with the attribute collector
//  registry:
//..
        ball::AttributeCollectorRegistry registry;

        int rc = registry.addCollector(&userInfo, "userInfoCollector");
        ASSERT(0 == rc);
        ASSERT(true == registry.hasCollector("userInfoCollector"));
        rc = registry.addCollector(&threadInfo, "threadInfoCollector");
        ASSERT(0 == rc);
        ASSERT(true == registry.hasCollector("threadInfoCollector"));
        ASSERT(2 == registry.numCollectors());
//..
//  Next, we print every attribute gathered by all registered attribute
//  collectors in the registry:
//..
        bsl::stringstream output1;

        registry.collect(bdlf::BindUtil::bind(&printAttribute,
                                              &output1,
                                              bdlf::PlaceHolders::_1));

        ASSERT("myLib.uuid=12345 myLib.user=proxy myLib.threadId=87654 "
               == output1.str());
//..
//  Finally, we remove one of the collectors and collect attributes again:
//..
        rc = registry.removeCollector("threadInfoCollector");
        ASSERT(0 == rc);
        ASSERT(false == registry.hasCollector("threadInfoCollectory"));
        ASSERT(1 == registry.numCollectors());

        bsl::stringstream output2;

        registry.collect(bdlf::BindUtil::bind(&printAttribute,
                                              &output2,
                                              bdlf::PlaceHolders::_1));

        ASSERT("myLib.uuid=12345 myLib.user=proxy " == output2.str());
    }
//..

}  // close namespace USAGE_EXAMPLE

namespace u {
    // Collection of collectors and visitors used throughout the test.

void testCollector(const bsl::function<void(const ball::Attribute&)>& visitor)
    // Pass a simple attribute to the specified 'visitor'.
{
    visitor(ball::Attribute("test", 1234));
}

void uuidCollector(const bsl::function<void(const ball::Attribute&)>& visitor)
    // Pass a simple attribute to the specified 'visitor'.
{
    visitor(ball::Attribute("uuid", 2345));
}

void spanCollector(const bsl::function<void(const ball::Attribute&)>& visitor)
    // Pass a simple attribute to the specified 'visitor'.
{
    visitor(ball::Attribute("span", "1234-5678-0123-4567"));
}

void noopVisitor(const ball::Attribute& attribute)
    // Do nothing with the specified 'attribute'.
{
    (void)attribute;
}

void assertVisitor(const ball::Attribute& attribute)
    // Must not be called.
{
    (void)attribute;
    ASSERTV(0);
}

void collectingVisitor(bsl::vector<ball::ManagedAttribute>  *vector,
                       const ball::Attribute&                attribute)
    // Add the specified 'attribute' to the specified 'vector'.
{
    vector->push_back(ball::ManagedAttribute(attribute));
}

}  // close namespace u

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 5: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        USAGE_EXAMPLE::usageExample1();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'collect' METHOD
        //
        // Concerns:
        //: 1 All registered collectors are called.
        //:
        //: 2 'collect' method is declared 'const'.
        //:
        //: 3 Collectors called in the order they had been registered.
        //
        // Plan:
        //: 1 Initialize the object with a set of test collectors and invoke
        //:   'collect' method.
        //:
        //: 2 Verify that all collectors had been invoked in the right order.
        //
        // Testing:
        //   void collect(const Visitor& visitor) const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'collect' METHOD"
                          << "\n========================" << endl;

        Obj mX; const Obj& X = mX;

        // Testing that the visitor is not called if there are no collectors.
        X.collect(&u::assertVisitor);

        {
            int rc = mX.addCollector(&u::uuidCollector, "first");
            ASSERTV(0 == rc);

            bsl::vector<ball::ManagedAttribute> vec;
            X.collect(bdlf::BindUtil::bind(&u::collectingVisitor,
                                           &vec,
                                           bdlf::PlaceHolders::_1));

            ASSERTV(1 == vec.size());
            ASSERTV(ball::ManagedAttribute("uuid", 2345) == vec[0]);
        }

        {
            int rc = mX.addCollector(&u::spanCollector, "second");
            ASSERTV(0 == rc);

            bsl::vector<ball::ManagedAttribute> vec;
            X.collect(bdlf::BindUtil::bind(&u::collectingVisitor,
                                           &vec,
                                           bdlf::PlaceHolders::_1));

            ASSERTV(2 == vec.size());
            ASSERTV(ball::ManagedAttribute("uuid", 2345) == vec[0]);
            ASSERTV(ball::ManagedAttribute("span", "1234-5678-0123-4567")
                                                                    == vec[1]);
        }
        {
            int rc = mX.removeCollector("first");
            ASSERTV(0 == rc);

            bsl::vector<ball::ManagedAttribute> vec;
            X.collect(bdlf::BindUtil::bind(&u::collectingVisitor,
                                           &vec,
                                           bdlf::PlaceHolders::_1));

            ASSERTV(1 == vec.size());
            ASSERTV(ball::ManagedAttribute("span", "1234-5678-0123-4567")
                                                                    == vec[0]);
        }
        {
            mX.removeAll();

            bsl::vector<ball::ManagedAttribute> vec;
            X.collect(bdlf::BindUtil::bind(&u::collectingVisitor,
                                           &vec,
                                           bdlf::PlaceHolders::_1));

            ASSERTV(0 == vec.size());
        }
      } break;
      case 3: {
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
        //:   that take an allocator in their constructor) return a 'const'
        //:   reference.
        //
        // Plan:
        //   In case 2 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 3 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-2, returns the expected value.  (C-2)
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
        //   allocator_type get_allocator() const;
        //   bool hasCollector(const bsl::string_view& name) const;
        //   int numCollectors() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBASIC ACCESSORS"
                          << "\n===============" << endl;

        if (veryVerbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bsl::allocator<char> oaa(&oa);

        bslma::DefaultAllocatorGuard dag(&da);

        if (veryVerbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(oaa);  const Obj& X = mX;

        if (veryVerbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const int numCollectors = X.numCollectors();
            ASSERTV(numCollectors, 0 == numCollectors);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());
        }

        if (veryVerbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        {
            int rc;
            rc = mX.addCollector(&u::testCollector, "collector1");
            ASSERTV(0 == rc);
            rc = mX.addCollector(&u::testCollector, "collector2");
            ASSERTV(0 == rc);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            ASSERTV(2     == X.numCollectors());
            ASSERTV(true  == X.hasCollector("collector1"));
            ASSERTV(true  == X.hasCollector("collector2"));
            ASSERTV(false == X.hasCollector("collector3"));

            // This test visitor should not do any allocation for the purposes
            // of this test.
            X.collect(&u::noopVisitor);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        // Testing insertion of the collector with already registered name.
        {
            int rc;
            rc = mX.addCollector(&u::testCollector, "collector1");
            ASSERTV(0 != rc);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            ASSERTV(2     == X.numCollectors());
            ASSERTV(true  == X.hasCollector("collector1"));
            ASSERTV(true  == X.hasCollector("collector2"));
            ASSERTV(false == X.hasCollector("collector3"));

            // This test visitor should not do any allocation for the purposes
            // of this test.
            X.collect(&u::noopVisitor);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }
        {
            int rc;
            rc = mX.removeCollector("collector1");
            ASSERTV(0 == rc);

            // Removing non-existing collector.
            rc = mX.removeCollector("collector1");
            ASSERTV(0 != rc);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            ASSERTV(1     == X.numCollectors());
            ASSERTV(false == X.hasCollector("collector1"));
            ASSERTV(true  == X.hasCollector("collector2"));

            // This test visitor should not do any allocation for the purposes
            // of this test.
            X.collect(&u::noopVisitor);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }
        {
            mX.removeAll();

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            ASSERTV(0     == X.numCollectors());
            ASSERTV(false == X.hasCollector("collector1"));
            ASSERTV(false == X.hasCollector("collector2"));

            // This test visitor should not do any allocation for the purposes
            // of this test.
            X.collect(&u::noopVisitor);

            ASSERTV(ALLOC_OF(X),   oaa == X.get_allocator());

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS & DTOR
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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:10 Any memory allocation is exception neutral.
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
        //: 2 Execute an inner loop that creates an object by
        //:   default-construction, but invokes the default constructor
        //:   differently in each iteration: (a) without passing an allocator,
        //:   (b) passing a default-constructed allocator explicitly (c)
        //:   passing the address of a test allocator distinct from the
        //:   default, and (d) passing in an allocator constructed from the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'get_allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'get_allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   7 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //
        // Testing:
        //   ball::AttributeCollectorRegistry();
        //   ball::AttributeCollectorRegistry(const allocator_type& a);
        //   ~ball::AttributeCollectorRegistry();
        //   int addCollector(const Collector&, const bsl::string_view& );
        //   int removeCollector(const bsl::string_view&);
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDEFAULT CTOR, PRIMARY MANIPULATORS & DTOR"
                          << "\n========================================="
                          << endl;

        if (veryVeryVeryVerbose)
            cout << "\nTesting with various allocator configurations." << endl;

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

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
                objPtr = new (fa) Obj(Obj::allocator_type());
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              case 'd': {
                objAllocatorPtr = &sa;
                Obj::allocator_type alloc(objAllocatorPtr);
                objPtr = new (fa) Obj(alloc);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // Also invoke the object's 'get_allocator' accessor.
            ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.get_allocator());

            // Verify no allocations from the object allocator (list)
            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());

            // Verify no allocations from the non-object allocator.
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Verify no temporary memory is allocated from the object
            // allocator.
            ASSERTV(CONFIG, oa.numBlocksMax(), 0 == oa.numBlocksMax());

            // Invoke basic manipulators.
            int rc;
            rc = mX.addCollector(&u::testCollector, "collector1");
            ASSERTV(0 == rc);
            rc = mX.addCollector(&u::testCollector, "collector2");
            ASSERTV(0 == rc);
            ASSERTV(2 == X.numCollectors());
            ASSERTV(true == X.hasCollector("collector1"));
            ASSERTV(true == X.hasCollector("collector2"));

            // Add collector under already registered name.
            rc = mX.addCollector(&u::testCollector, "collector2");
            ASSERTV(0 != rc);
            ASSERTV(2 == X.numCollectors());
            ASSERTV(true == X.hasCollector("collector1"));
            ASSERTV(true == X.hasCollector("collector2"));

            // Verify no allocations from the non-object allocator.
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Verify memory was allocated from the object allocator.
            ASSERTV(CONFIG, oa.numBlocksMax(), 2 == oa.numBlocksMax());

            rc = mX.removeCollector("collector1");
            ASSERTV(0 == rc);
            ASSERTV(1 == X.numCollectors());
            ASSERTV(false == X.hasCollector("collector1"));
            ASSERTV(true == X.hasCollector("collector2"));

            // Remove non-existing collector.
            rc = mX.removeCollector("collector1");
            ASSERTV(0 != rc);
            ASSERTV(1 == X.numCollectors());
            ASSERTV(false == X.hasCollector("collector1"));
            ASSERTV(true == X.hasCollector("collector2"));

            // Remove all collectors.
            mX.removeAll();
            ASSERTV(0 == X.numCollectors());
            ASSERTV(false == X.hasCollector("collector1"));
            ASSERTV(false == X.hasCollector("collector2"));

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.
            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());
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
        //: 1 Create an object 'w' (default ctor)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        Obj mX;  const Obj& X = mX;

        ASSERTV(0 == X.numCollectors());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
