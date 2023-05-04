// ball_broadcastobserver.t.cpp                                       -*-C++-*-
#include <ball_broadcastobserver.h>

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_testobserver.h>
#include <ball_transmission.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_annotation.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace ball;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a broadcast observer that forwards log
// records and their corresponding publication contexts to a set of registered
// observers.  We must ensure that the component behaves correctly as both a
// container of observers and publication broadcast agent.
//-----------------------------------------------------------------------------
// [ 2] BroadcastObserver(bslma::Allocator *ba = 0);
// [ 2] ~BroadcastObserver();
// [ 3] void deregisterAllObservers();
// [ 3] int deregisterObserver(const bsl::string_view& name);
// [ 4] shared_ptr<Observer> findObserver(const bsl::string_view& name);
// [ 4] int findObserver(shared_ptr<OBSERVER> *, const bsl::string_view&);
// [ 5] void publish(const Record& record, const Context& context);
// [ 6] void publish(const shared_ptr<const Record>& r, const Context& c);
// [ 3] int registerObserver(const shared_ptr<Observer>&, name);
// [ 6] void releaseRecords();
// [ 8] void visitObservers(OBSERVER_VISITOR& visitor);
// [ 4] shared_ptr<const Observer> findObserver(name) const;
// [ 4] int findObserver(shared_ptr<const OBSERVER> *, name) const;
// [ 4] int numRegisteredObservers() const;
// [ 8] void visitObservers(const OBSERVER_VISITOR& visitor) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
// [ 7] CONCERN: REGISTERED OBSERVERS LIFETIME

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_ostreamobserver.h

class my_OstreamObserver : public ball::Observer {
    ostream& d_stream;

  public:
    explicit my_OstreamObserver(ostream& stream) : d_stream(stream)
    {
    }

    ~my_OstreamObserver()
    {
    }

    void publish(const bsl::shared_ptr<const ball::Record>& record,
                 const ball::Context&                       context);
};

// my_ostreamobserver.cpp

void my_OstreamObserver::publish(
                            const bsl::shared_ptr<const ball::Record>& record,
                            const ball::Context&                       context)
{

    switch (context.transmissionCause()) {
      case ball::Transmission::e_PASSTHROUGH: {
        d_stream << "Single Passthrough Message:" << endl;
      } break;
      case ball::Transmission::e_TRIGGER_ALL: {
        d_stream << "Remotely ";      // no 'break'; concatenated output
      } BSLS_ANNOTATION_FALLTHROUGH;
      case ball::Transmission::e_TRIGGER: {
        d_stream << "Triggered Publication Sequence: Message ";
        d_stream << context.recordIndex() + 1  // Account for 0-based index.
                 << " of " << context.sequenceLength() << endl;
      } break;
      default: {
        d_stream << "***ERROR*** Unknown Message Cause:" << endl;
      } break;
    }

    d_stream << "\tTimestamp:  " << record->fixedFields().timestamp() << endl;
    d_stream << "\tProcess ID: " << record->fixedFields().processID() << endl;
    d_stream << "\tThread ID:  " << record->fixedFields().threadID()  << endl
             << endl;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::BroadcastObserver Obj;

const int SEQUENCE_LENGTH = 99;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace TEST_CASE_8 {

static int observerCounter = 0;

static void observerCounterFunc(const bsl::shared_ptr<Observer>&,
                                const bsl::string_view&)
    // Increment invocation counter.
{
    ++observerCounter;
}

struct ObserverCounterFunctor {

    // ACCESSORS
    void operator()(const bsl::shared_ptr<Observer>&,
                    const bsl::string_view&) const
        // Increment invocation counter.
    {
        ++observerCounter;
    }
};

}  // close namespace TEST_CASE_8

namespace {
    // The following helper functions are used to test 'publish' method.

static int nextRecord(ball::Record& record)
    // Modify the specified 'record' to have a value distinct from any other
    // record transmitted so far in the publication sequence and return the
    // 0-based sequence number of 'record'.
{
    static int sequenceNumber = 0;

    ball::RecordAttributes attr;
    attr.setLineNumber(sequenceNumber);
    attr.setProcessID(sequenceNumber);
    attr.setSeverity(sequenceNumber);
    attr.setThreadID(sequenceNumber);
    record.setFixedFields(attr);

    return sequenceNumber++;
}

static bool isNthRecord(const ball::Record& record, int nth)
    // Return 'true' if the specified 'record' is the specified 'nth' in the
    // publication sequence, and 'false' otherwise.
{
    const ball::RecordAttributes& attr = record.fixedFields();

    if (nth    == attr.lineNumber()
        && nth == attr.processID()
        && nth == attr.severity()
        && nth == static_cast<int>(attr.threadID())) {
        return true;                                                  // RETURN
    }

    return false;
}

static bool isNthContext(const ball::Context& context, int nth)
    // Return 'true' if the specified 'context' is the specified 'nth' in the
    // publication sequence, and 'false' otherwise.
{
    if (ball::Transmission::e_TRIGGER == context.transmissionCause()
        &&                        nth == context.recordIndex()
        &&            SEQUENCE_LENGTH == context.sequenceLength()) {
        return true;                                                  // RETURN
    }

    return false;
}

}  // close unnamed namespace

// ============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Delayed Observer Configuration
///- - - - - - - - - - - - - - - - - - - - -
// In this example, we will show how 'ball::BroadcastObserver' can be used to
// implement delayed observer configuration.
//
// First, we define an elided custom observer that conforms to the
// 'ball::Observer' protocol and supports a 'configure' method:
//..
    class ConfigurableObserver : public ball::Observer {
        // DATA
        bool d_configureFlag;   // configuration completion flag

      public:
        // CREATORS
        ConfigurableObserver()
        : d_configureFlag(false)
        {
        }

        // MANIPULATORS
        void configure()
            // Configure this observer.
        {
            d_configureFlag = true;
        }

        void publish(const bsl::shared_ptr<ball::Record>& record,
                     const ball::Context&                 context)
            // Publish the specified 'record' with the specified 'context'.
        {
            (void) record;
            (void) context;

            // Do not publish any records until configuration has been done.
            if (!d_configureFlag) {
                return;                                               // RETURN
            }
            // Publish the record.
            // ...
        }

        // ACCESSORS
        bool isConfigured() const
        {
            return d_configureFlag;
        }
    };

}  // close namespace UsageExample

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: In no case is memory allocated from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        using namespace UsageExample;

        bsl::shared_ptr<ConfigurableObserver>
                                     myObserverPtr(new ConfigurableObserver());

        ball::BroadcastObserver broadcastObserver;

        int rc = broadcastObserver.registerObserver(myObserverPtr, "observer");

        ASSERT(0 == rc);

        bsl::shared_ptr<ConfigurableObserver> tmpObserverPtr;

        rc = broadcastObserver.findObserver(&tmpObserverPtr, "observer");

        ASSERT(0 == rc);
        ASSERT(myObserverPtr == tmpObserverPtr);
        ASSERT(false == tmpObserverPtr->isConfigured());

        tmpObserverPtr->configure();

        ASSERT(true == tmpObserverPtr->isConfigured());

        bsl::shared_ptr<Observer> oPtr =
                                    broadcastObserver.findObserver("observer");

        ASSERT(oPtr.get());

        bsl::shared_ptr<ConfigurableObserver> anotherObserverPtr;

        bslstl::SharedPtrUtil::dynamicCast(&anotherObserverPtr, oPtr);

        ASSERT(myObserverPtr == anotherObserverPtr);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'visitObservers' METHOD
        //
        // Concerns:
        //: 1 All registered observers are returned by 'registeredObservers'.
        //
        // Plan:
        //: 1 Create a broadcast observer, and register a number of observers.
        //:
        //: 2 Verify that all registered observers are returned by
        //:   'registeredObservers' accessor.  (C-1)
        //
        // Testing:
        //   void visitObservers(OBSERVER_VISITOR& visitor);
        //   void visitObservers(const OBSERVER_VISITOR& visitor) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'visitObservers' METHOD"
                          << "\n===============================" << endl;

        using namespace TEST_CASE_8;

        Obj        mX;
        const Obj& X = mX;

        bsl::shared_ptr<Observer> O1(new TestObserver(&cout));

        // Testing no registered observers case.
        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 0 == observerCounter);

            observerCounter = 0;

            X.visitObservers(ObserverCounterFunctor());

            ASSERTV(observerCounter, 0 == observerCounter);
        }

        ASSERTV(0 == mX.registerObserver(O1, "O1"));

        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 1 == observerCounter);

            observerCounter = 0;

            X.visitObservers(ObserverCounterFunctor());

            ASSERTV(observerCounter, 1 == observerCounter);
        }

        bsl::shared_ptr<Observer> O2(new TestObserver(&cout));
        ASSERTV(0 == mX.registerObserver(O2, "O2"));

        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 2 == observerCounter);
        }

        ASSERTV(0 == mX.deregisterObserver("O1"));

        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 1 == observerCounter);
        }

        ASSERTV(0 == mX.registerObserver(O2, "O2a"));

        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 2 == observerCounter);
        }

        mX.deregisterAllObservers();

        {
            observerCounter = 0;

            mX.visitObservers(observerCounterFunc);

            ASSERTV(observerCounter, 0 == observerCounter);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING REGISTERED OBSERVERS LIFETIME
        //
        // Concerns:
        //: 1 Verify that registered observers are destroyed only when the last
        //:   shared pointer to them is destroyed.
        //
        // Plan:
        //: 1 Create an observer, and register it with a broadcast observer.
        //:
        //: 2 Leave the scope where the observer was created.
        //:
        //: 3 Verify that the registered observer is not destroyed.
        //:
        //: 4 Deregister observer and verify that it was destroyed.  (C-1)
        //
        // Testing:
        //   CONCERN: REGISTERED OBSERVERS LIFETIME
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING REGISTERED OBSERVERS LIFETIME"
                          << "\n=====================================" << endl;

        // Registered observer lifetime is shorter.
        {
            Obj           mX;
            TestObserver *mXPtr = 0 ;
            {
                bsl::shared_ptr<Observer> testObserver(
                                                      new TestObserver(&cout));

                ASSERTV(1 == testObserver.use_count());
                mXPtr = dynamic_cast<TestObserver *>(testObserver.get());

                ASSERTV(0 == mX.registerObserver(testObserver, "observer"));
                ASSERTV(2 == testObserver.use_count());
            }
            bsl::shared_ptr<Observer> o = mX.findObserver("observer");
            ASSERTV(2 == o.use_count());
            ASSERTV(mXPtr, o.get(), mXPtr == o.get());

            mX.deregisterAllObservers();
            ASSERTV(1 == o.use_count());
        }

        // Broadcast observer lifetime is shorter.
        {
            bsl::shared_ptr<Observer> testObserver(new TestObserver(&cout));
            ASSERTV(1 == testObserver.use_count());
            {
                Obj           mX;

                ASSERTV(0 == mX.registerObserver(testObserver, "observer"));
                ASSERTV(2 == testObserver.use_count());
            }
            ASSERTV(1 == testObserver.use_count());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASYNCHRONOUS 'publish' AND 'releaseRecords' METHODS
        //
        // Concerns:
        //: 1 The async 'publish' method correctly forwards records and their
        //:   corresponding publication contexts to all registered observers.
        //:
        //: 2 The 'releaseRecords' method is correctly delegated to all
        //:   registered observers.
        //
        // Plan:
        //: 1 Publish several distinct records into differently configured
        //:   broadcast observers.  Verify that all registered observers
        //:   correctly receive published records and their corresponding
        //:   publication contexts.  (C-1)
        //:
        //: 2 Invoke 'releaseRecords' method on the broadcast observers and
        //:   verify that the method was delegated to all registered observers.
        //
        // Testing:
        //   void publish(const shared_ptr<const Record>& r, const Context& c);
        //   void releaseRecords();
        // --------------------------------------------------------------------

        if (verbose) cout
             << "\nTESTING ASYNCHRONOUS 'publish' AND 'releaseRecords' METHODS"
             << "\n==========================================================="
             << endl;

        {
            bslma::TestAllocator ra("record", veryVeryVeryVerbose);

            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            bsl::shared_ptr<Observer> mYPtr(new Obj());

            Obj& mY = *(dynamic_cast<Obj*>(mYPtr.get())); const Obj& Y = mY;

            ASSERT(0 == Y.numRegisteredObservers());

            bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));
            const TestObserver& O1 =
                                 *(dynamic_cast<TestObserver *>(mO1Ptr.get()));

            bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));
            const TestObserver& O2 =
                                 *(dynamic_cast<TestObserver *>(mO2Ptr.get()));

            bsl::shared_ptr<Observer> mO3Ptr(new TestObserver(&bsl::cout));
            const TestObserver& O3 =
                                 *(dynamic_cast<TestObserver *>(mO3Ptr.get()));

            bsl::shared_ptr<Record> mR(new (ra) Record(&ra), &ra);
            const bsl::shared_ptr<Record>& R = mR;

            Context mC(Transmission::e_TRIGGER, 0, SEQUENCE_LENGTH);
            const Context& C = mC;

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(0 == O1.numPublishedRecords());

            const int I1 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I1));
            mC.setRecordIndexRaw(I1);        ASSERT(1 == isNthContext(C, I1));
            mX.publish(R, C);
            mX.releaseRecords();

            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(1 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == X.numRegisteredObservers());

            const int I2 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I2));
            mC.setRecordIndexRaw(I2);        ASSERT(1 == isNthContext(C, I2));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(2 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == O1.numPublishedRecords());

            const int I3 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I3));
            mC.setRecordIndexRaw(I3);        ASSERT(1 == isNthContext(C, I3));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(3 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I3));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I3));

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(0 == O2.numPublishedRecords());

            const int I4 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I4));
            mC.setRecordIndexRaw(I4);        ASSERT(1 == isNthContext(C, I4));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(3 == O1.numPublishedRecords());
            ASSERT(4 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I4));
            ASSERT(1 == O2.numPublishedRecords());
            ASSERT(1 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I4));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(mYPtr, "Y"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I5 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I5));
            mC.setRecordIndexRaw(I5);        ASSERT(1 == isNthContext(C, I5));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(4 == O1.numPublishedRecords());
            ASSERT(5 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I5));
            ASSERT(2 == O2.numPublishedRecords());
            ASSERT(2 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I5));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mY.registerObserver(mO3Ptr, "O3"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());
            ASSERT(0 == O3.numPublishedRecords());

            const int I6 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I6));
            mC.setRecordIndexRaw(I6);        ASSERT(1 == isNthContext(C, I6));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(5 == O1.numPublishedRecords());
            ASSERT(6 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I6));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(3 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(1 == O3.numPublishedRecords());
            ASSERT(1 == O3.numReleases());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I6));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O3
            ASSERT(0 == mX.deregisterObserver("O2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I7 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I7));
            mC.setRecordIndexRaw(I7);        ASSERT(1 == isNthContext(C, I7));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(6 == O1.numPublishedRecords());
            ASSERT(7 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I7));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(4 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(2 == O3.numPublishedRecords());
            ASSERT(2 == O3.numReleases());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I7));

            //     X
            //    / |
            //  O1  Y
            //     / |
            //   O3  O2
            ASSERT(0 == mY.registerObserver(mO2Ptr, "O2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(2 == Y.numRegisteredObservers());
            ASSERT(3 == O2.numPublishedRecords());

            const int I8 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I8));
            mC.setRecordIndexRaw(I8);        ASSERT(1 == isNthContext(C, I8));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(7 == O1.numPublishedRecords());
            ASSERT(8 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I8));
            ASSERT(4 == O2.numPublishedRecords());
            ASSERT(5 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I8));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(3 == O3.numReleases());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O2
            ASSERT(0 == mY.deregisterObserver("O3"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I9 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I9));
            mC.setRecordIndexRaw(I9);        ASSERT(1 == isNthContext(C, I9));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(8 == O1.numPublishedRecords());
            ASSERT(9 == O1.numReleases());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I9));
            ASSERT(5 == O2.numPublishedRecords());
            ASSERT(6 == O2.numReleases());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numReleases());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //   |   |
            //   O1  O2
            ASSERT(0 == mX.deregisterObserver("Y"));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I10 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I10));
            mC.setRecordIndexRaw(I10);       ASSERT(1 == isNthContext(C, I10));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT( 9 == O1.numPublishedRecords());
            ASSERT(10 == O1.numReleases());
            ASSERT( 1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT( 1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT( 5 == O2.numPublishedRecords());
            ASSERT( 7 == O2.numReleases());
            ASSERT( 1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT( 1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT( 3 == O3.numPublishedRecords());
            ASSERT( 4 == O3.numReleases());
            ASSERT( 1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT( 1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I11 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I11));
            mC.setRecordIndexRaw(I11);       ASSERT(1 == isNthContext(C, I11));
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT( 9 == O1.numPublishedRecords());
            ASSERT(10 == O1.numReleases());
            ASSERT( 1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT( 1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT( 6 == O2.numPublishedRecords());
            ASSERT( 8 == O2.numReleases());
            ASSERT( 1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT( 1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT( 3 == O3.numPublishedRecords());
            ASSERT( 4 == O3.numReleases());
            ASSERT( 1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT( 1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //       |
            //       O2
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I12 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I12));
            mC.setRecordIndexRaw(I12);       ASSERT(1 == isNthContext(C, I12));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT( 9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numReleases());
            ASSERT( 1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT( 1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT( 6 == O2.numPublishedRecords());
            ASSERT( 8 == O2.numReleases());
            ASSERT( 1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT( 1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT( 3 == O3.numPublishedRecords());
            ASSERT( 4 == O3.numReleases());
            ASSERT( 1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT( 1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I13 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I13));
            mC.setRecordIndexRaw(I13);       ASSERT(1 == isNthContext(C, I13));
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT( 9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numReleases());
            ASSERT( 1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT( 1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT( 7 == O2.numPublishedRecords());
            ASSERT( 9 == O2.numReleases());
            ASSERT( 1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT( 1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT( 3 == O3.numPublishedRecords());
            ASSERT( 4 == O3.numReleases());
            ASSERT( 1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT( 1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            ASSERT(0 == mY.deregisterObserver("O2"));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I14 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I14));
            mC.setRecordIndexRaw(I14);       ASSERT(1 == isNthContext(C, I14));
            mX.publish(R, C);
            mX.releaseRecords();
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT( 9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numReleases());
            ASSERT( 1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT( 1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT( 7 == O2.numPublishedRecords());
            ASSERT(10 == O2.numReleases());
            ASSERT( 1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT( 1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT( 3 == O3.numPublishedRecords());
            ASSERT( 4 == O3.numReleases());
            ASSERT( 1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT( 1 == isNthContext(O3.lastPublishedContext(), I8));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING SYNCHRONOUS 'publish' METHOD
        //
        // Concerns:
        //: 1 The sync 'publish' method correctly forwards records and their
        //:   corresponding publication contexts to all registered observers.
        //
        // Plan:
        //: 1 Publish several distinct records into differently configured
        //:   broadcast observers. Verify that all registered observers
        //:   correctly receive published records and their corresponding
        //:   publication contexts.  (C-1)
        //
        // Testing:
        //   void publish(const Record& record, const Context& context);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SYNCHRONOUS 'publish' METHOD" << endl
                          << "====================================" << endl;

        if (verbose) cout << endl << "DEPRECATED" << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //
        // Concerns:
        //: 1 'numRegisteredObservers' returns expected number of registered
        //:   observers.
        //:
        //: 2 'findObserver' returns a pointer to the registered observer.
        //
        // Plan:
        //: 1 Construct a pair of broadcast observers 'X' and 'Y'.
        //:
        //: 2 Construct a set of test observers O1, O2, and O3.
        //:
        //: 3 Test various combination of registered observers.
        //:
        //: 4 Verify that basic accessors return expected values.
        //
        // Testing:
        //   shared_ptr<Observer> findObserver(const bsl::string_view& name);
        //   int findObserver(shared_ptr<OBSERVER> *, const bsl::string_view&);
        //   shared_ptr<const Observer> findObserver(name) const;
        //   int findObserver(shared_ptr<const OBSERVER> *, name) const;
        //   int numRegisteredObservers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ACCESSORS" << endl
                                  << "===============" << endl;

        //  X
        Obj mX;  const Obj& X = mX;
        ASSERT(0 == X.numRegisteredObservers());

        //  Y
        bsl::shared_ptr<Observer> mYPtr(new Obj());

        Obj& mY = *(dynamic_cast<Obj*>(mYPtr.get())); const Obj& Y = mY;

        ASSERT(0 == Y.numRegisteredObservers());

        bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));
        bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));
        bsl::shared_ptr<Observer> mO3Ptr(new TestObserver(&bsl::cout));

        //   X
        //   |
        //   O1
        ASSERT(0      == mX.registerObserver(mO1Ptr, "O1"));
        ASSERT(1      == X.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 == mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == mO1Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == mO1Ptr);
        }

        //   X
        ASSERT(0      == mX.deregisterObserver("O1"));
        ASSERT(0      == X.numRegisteredObservers());
        ASSERT(0      == mX.findObserver("O1"));
        ASSERT(0      == X.findObserver("O1"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 != mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == 0);
            ASSERT(0 != X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == 0);
        }

        //   X
        //   |
        //   O1
        ASSERT(0      == mX.registerObserver(mO1Ptr, "O1"));
        ASSERT(1      == X.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));

        //     X
        //    / |
        //  O1  O2
        ASSERT(0      == mX.registerObserver(mO2Ptr, "O2"));
        ASSERT(2      == X.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(mO2Ptr == mX.findObserver("O2"));
        ASSERT(mO2Ptr == X.findObserver("O2"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 == mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == mO1Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == mO1Ptr);

            ASSERT(0 == mX.findObserver(&observerPtr, "O2"));
            ASSERT(observerPtr == mO2Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O2"));
            ASSERT(observerPtrConst == mO2Ptr);
        }

        //     --X--
        //    /  |  |
        //  O1   O2 Y
        ASSERT(0      == mX.registerObserver(mYPtr, "Y"));
        ASSERT(3      == X.numRegisteredObservers());
        ASSERT(0      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(mO2Ptr == mX.findObserver("O2"));
        ASSERT(mO2Ptr == X.findObserver("O2"));
        ASSERT(mYPtr  == mX.findObserver("Y"));
        ASSERT(mYPtr  == X.findObserver("Y"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 == mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == mO1Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == mO1Ptr);

            ASSERT(0 == mX.findObserver(&observerPtr, "O2"));
            ASSERT(observerPtr == mO2Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O2"));
            ASSERT(observerPtrConst == mO2Ptr);

            // Observer Y is not TestObserver (wrong type)
            ASSERT(0 != mX.findObserver(&observerPtr, "Y"));
            ASSERT(observerPtr == 0);
            ASSERT(0 != X.findObserver(&observerPtrConst, "Y"));
            ASSERT(observerPtrConst == 0);

            bsl::shared_ptr<Obj> observerYPtr;
            bsl::shared_ptr<const Obj> observerYPtrConst;

            ASSERT(0 == mX.findObserver(&observerYPtr, "Y"));
            ASSERT(observerYPtr == mYPtr);
            ASSERT(0 == X.findObserver(&observerYPtrConst, "Y"));
            ASSERT(observerYPtrConst == mYPtr);

        }

        //     --X--
        //    /  |  |
        //  O1   O2 Y
        //          |
        //          O3
        ASSERT(0      == mY.registerObserver(mO3Ptr, "O3"));
        ASSERT(3      == X.numRegisteredObservers());
        ASSERT(1      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(mO2Ptr == mX.findObserver("O2"));
        ASSERT(mO2Ptr == X.findObserver("O2"));
        ASSERT(mYPtr  == mX.findObserver("Y"));
        ASSERT(mYPtr  == X.findObserver("Y"));
        ASSERT(0      == mX.findObserver("O3"));
        ASSERT(0      == X.findObserver("O3"));
        ASSERT(mO3Ptr == mY.findObserver("O3"));
        ASSERT(mO3Ptr == Y.findObserver("O3"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 == mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == mO1Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == mO1Ptr);

            ASSERT(0 == mX.findObserver(&observerPtr, "O2"));
            ASSERT(observerPtr == mO2Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O2"));
            ASSERT(observerPtrConst == mO2Ptr);

            // Second-level observers are not looked up.
            ASSERT(0 != mX.findObserver(&observerPtr, "O3"));
            ASSERT(observerPtr == 0);
            ASSERT(0 != X.findObserver(&observerPtrConst, "O3"));
            ASSERT(observerPtrConst == 0);

            ASSERT(0 == mY.findObserver(&observerPtr, "O3"));
            ASSERT(observerPtr == mO3Ptr);
            ASSERT(0 == Y.findObserver(&observerPtrConst, "O3"));
            ASSERT(observerPtrConst == mO3Ptr);
        }

        //     X
        //    / |
        //  O1  Y
        //      |
        //      O3
        ASSERT(0      == mX.deregisterObserver("O2"));
        ASSERT(2      == X.numRegisteredObservers());
        ASSERT(1      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(0      == mX.findObserver("O2"));
        ASSERT(0      == X.findObserver("O2"));
        ASSERT(mYPtr  == mX.findObserver("Y"));
        ASSERT(mYPtr  == X.findObserver("Y"));
        ASSERT(0      == mX.findObserver("O3"));
        ASSERT(0      == X.findObserver("O3"));
        ASSERT(mO3Ptr == mY.findObserver("O3"));
        ASSERT(mO3Ptr == Y.findObserver("O3"));

        //     X
        //    / |
        //  O1  Y
        //     / |
        //   O3  O2
        ASSERT(0      == mY.registerObserver(mO2Ptr, "O2"));
        ASSERT(2      == X.numRegisteredObservers());
        ASSERT(2      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(0      == mX.findObserver("O2"));
        ASSERT(0      == X.findObserver("O2"));
        ASSERT(mYPtr  == mX.findObserver("Y"));
        ASSERT(mYPtr  == X.findObserver("Y"));
        ASSERT(0      == mX.findObserver("O3"));
        ASSERT(0      == X.findObserver("O3"));
        ASSERT(mO2Ptr == mY.findObserver("O2"));
        ASSERT(mO2Ptr == Y.findObserver("O2"));
        ASSERT(mO3Ptr == mY.findObserver("O3"));
        ASSERT(mO3Ptr == Y.findObserver("O3"));

        //     X
        //    / |
        //  O1   Y
        //       |
        //       O2
        ASSERT(0      == mY.deregisterObserver("O3"));
        ASSERT(2      == X.numRegisteredObservers());
        ASSERT(1      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(0      == mX.findObserver("O2"));
        ASSERT(0      == X.findObserver("O2"));
        ASSERT(mYPtr  == mX.findObserver("Y"));
        ASSERT(mYPtr  == X.findObserver("Y"));
        ASSERT(mO2Ptr == mY.findObserver("O2"));
        ASSERT(mO2Ptr == Y.findObserver("O2"));
        ASSERT(0      == mY.findObserver("O3"));
        ASSERT(0      == Y.findObserver("O3"));

        //   X   Y
        //   |   |
        //   O1  O2
        ASSERT(0      == mX.deregisterObserver("Y"));
        ASSERT(1      == X.numRegisteredObservers());
        ASSERT(1      == Y.numRegisteredObservers());
        ASSERT(mO1Ptr == mX.findObserver("O1"));
        ASSERT(mO1Ptr == X.findObserver("O1"));
        ASSERT(0      == mX.findObserver("O2"));
        ASSERT(0      == X.findObserver("O2"));
        ASSERT(0      == mX.findObserver("Y"));
        ASSERT(0      == X.findObserver("Y"));
        ASSERT(mO2Ptr == mY.findObserver("O2"));
        ASSERT(mO2Ptr == Y.findObserver("O2"));

        {
            bsl::shared_ptr<ball::TestObserver> observerPtr;
            bsl::shared_ptr<const ball::TestObserver> observerPtrConst;

            ASSERT(0 == mX.findObserver(&observerPtr, "O1"));
            ASSERT(observerPtr == mO1Ptr);
            ASSERT(0 == X.findObserver(&observerPtrConst, "O1"));
            ASSERT(observerPtrConst == mO1Ptr);

            ASSERT(0 == mY.findObserver(&observerPtr, "O2"));
            ASSERT(observerPtr == mO2Ptr);
            ASSERT(0 == Y.findObserver(&observerPtrConst, "O2"));
            ASSERT(observerPtrConst == mO2Ptr);
        }

        //   X   Y
        //       |
        //       O2
        ASSERT(0      == mX.deregisterObserver("O1"));
        ASSERT(0      == X.numRegisteredObservers());
        ASSERT(1      == Y.numRegisteredObservers());
        ASSERT(0      == mX.findObserver("O1"));
        ASSERT(0      == X.findObserver("O1"));
        ASSERT(mO2Ptr == mY.findObserver("O2"));
        ASSERT(mO2Ptr == Y.findObserver("O2"));

        //   X   Y
        ASSERT(0      == mY.deregisterObserver("O2"));
        ASSERT(0      == X.numRegisteredObservers());
        ASSERT(0      == Y.numRegisteredObservers());
        ASSERT(0      == mY.findObserver("O2"));
        ASSERT(0      == Y.findObserver("O2"));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 'registerObserver' and 'deregisterObserver' produce the expected
        //:   side-effects on the object.
        //
        // Plan:
        //: 1 Construct a pair of broadcast observers 'X' and 'Y'.
        //:
        //: 2 Construct a set of test observers O1, O2, and O3.
        //:
        //: 3 Test various combination of registered observers.
        //:
        //: 4 Test the return value of the methods under test to infer that
        //:   they fulfill their contractual obligations.
        //
        // Testing:
        //   void deregisterAllObservers();
        //   int deregisterObserver(const bsl::string_view& name);
        //   int registerObserver(const shared_ptr<Observer>&, name);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY MANIPULATORS" << endl
                                  << "====================" << endl;

        if (veryVerbose)
            cout << "\tTesting valid registration/deregistration." << endl;
        {
            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            bsl::shared_ptr<Observer> mYPtr(new Obj());

            Obj& mY = *(dynamic_cast<Obj*>(mYPtr.get())); const Obj& Y = mY;

            ASSERT(0 == Y.numRegisteredObservers());

            bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));
            bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));
            bsl::shared_ptr<Observer> mO3Ptr(new TestObserver(&bsl::cout));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //   X
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == X.numRegisteredObservers());

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O2"));
            ASSERT(2 == X.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(mYPtr, "Y"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mY.registerObserver(mO3Ptr, "O3"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O3
            ASSERT(0 == mX.deregisterObserver("O2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1  Y
            //     / |
            //   O3  O2
            ASSERT(0 == mY.registerObserver(mO2Ptr, "O2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(2 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1   Y
            //       |
            //       O2
            ASSERT(0 == mY.deregisterObserver("O3"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            //   |   |
            //   O1  O2
            ASSERT(0 == mX.deregisterObserver("Y"));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            //       |
            //       O2
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            ASSERT(0 == mY.deregisterObserver("O2"));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());
        }

        {
            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            bsl::shared_ptr<Observer> mYPtr(new Obj());

            Obj& mY = *(dynamic_cast<Obj*>(mYPtr.get())); const Obj& Y = mY;

            ASSERT(0 == Y.numRegisteredObservers());

            bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));
            bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));
            bsl::shared_ptr<Observer> mO3Ptr(new TestObserver(&bsl::cout));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //   X
            mX.deregisterAllObservers();
            ASSERT(0 == X.numRegisteredObservers());

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O2"));
            ASSERT(2 == X.numRegisteredObservers());

            //   X
            mX.deregisterAllObservers();
            ASSERT(0 == X.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O2"));
            ASSERT(0 == mX.registerObserver(mYPtr, "OY"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            //   X
            mX.deregisterAllObservers();
            ASSERT(0 == X.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O2"));
            ASSERT(0 == mX.registerObserver(mYPtr, "OY"));
            ASSERT(0 == mY.registerObserver(mO3Ptr, "O3"));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            //       |
            //       O2
            mX.deregisterAllObservers();
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            mY.deregisterAllObservers();
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());
        }

        if (veryVerbose)
            cout << "\tTesting invalid registration/deregistration." << endl;
        {
            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));
            bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //   X
            //   |
            //   O1
            ASSERT(0 != mX.registerObserver(mO1Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //     X
            //    / |
            //   O1 OO1
            ASSERT(0 == mX.registerObserver(mO1Ptr, "OO1"));
            ASSERT(2 == X.numRegisteredObservers());

            //     X
            //    / |
            //  O1  OO1
            ASSERT(0 != mX.registerObserver(mO2Ptr, "O1"));
            ASSERT(2 == X.numRegisteredObservers());

            //     X
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == mX.deregisterObserver("OO1"));
            ASSERT(0 == X.numRegisteredObservers());

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(mO2Ptr, "O1"));
            ASSERT(1 == X.numRegisteredObservers());

            //   X
            ASSERT(0 == mX.deregisterObserver("O1"));
            ASSERT(0 == X.numRegisteredObservers());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
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
        //
        // Plan:
        //: 1 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-1); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory is
        //:     allocated by the default constructor.  (C-9)
        //:
        //:   4 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   5 Register an observer and verify that the memory is allocated
        //:     from the object allocator.  (C-2..7)
        //:
        //:   6 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //
        // Testing:
        //   BroadcastObserver(bslma::Allocator *ba = 0);
        //   ~BroadcastObserver();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING CREATORS" << endl
                                  << "================" << endl;

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
                  if (veryVerbose) {
                      cout << "\tTesting default constructor." << endl;
                  }
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with null allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  if (veryVerbose) {
                      cout << "\tTesting constructor with an allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              }
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG, oa.numBlocksTotal(),  0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            if (veryVerbose) {
                cout << "\t\tTesting 'registerObserver' (bootstrap)." << endl;
            }

            {
                // Registration is done in local scope to leave only one
                // reference to the registered observer.
                bsl::shared_ptr<Observer>
                                 mO1Ptr(new (fa) TestObserver(&bsl::cout, &fa),
                                        &fa);

                ASSERTV(CONFIG, 0 == mX.registerObserver(mO1Ptr, "observer1"));
                ASSERTV(CONFIG, 1 == X.numRegisteredObservers());
            }

            ASSERTV(CONFIG, oa.numBlocksInUse(), 0 < oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.
            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
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
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        {
            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t1.  Construct an object mX." << endl;

            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t2.  Register an observer o1." << endl;

            bsl::shared_ptr<Observer> mO1Ptr(new TestObserver(&bsl::cout));

            TestObserver *mO1 = dynamic_cast<TestObserver *>(mO1Ptr.get());

            const TestObserver& O1 = *mO1;

            ASSERT(0 == mX.registerObserver(mO1Ptr, "observer"));
            ASSERT(1 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t3.  Deregister observer o1." << endl;

            ASSERT(0 == mX.deregisterObserver("observer"));
            ASSERT(0 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t4.  Reregister observer o1." << endl;

            ASSERT(0 == mX.registerObserver(mO1Ptr, "observer1"));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(0 == O1.numPublishedRecords());

            if (veryVerbose) cout << "\t4a. Publish a record r1." << endl;

            bsl::shared_ptr<Record> mR1(new (oa) Record(&oa), &oa);
            const bsl::shared_ptr<Record>& R1 = mR1;

            Context mC1;  const Context& C1 = mC1;

            mX.publish(R1, C1);
            mX.releaseRecords();

            ASSERT(1 == O1.numPublishedRecords());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t5.  Register an observer o2." << endl;

            bsl::shared_ptr<Observer> mO2Ptr(new TestObserver(&bsl::cout));

            TestObserver *mO2 = dynamic_cast<TestObserver *>(mO2Ptr.get());

            const TestObserver& O2 = *mO2;

            ASSERT(0 == mX.registerObserver(mO2Ptr, "observer2"));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(0 == O2.numPublishedRecords());

            if (veryVerbose) cout << "\t5a. Publish a record r2." << endl;

            bsl::shared_ptr<Record> mR2(new (oa) Record(&oa), &oa);
            const bsl::shared_ptr<Record>& R2 = mR2;

            Context mC2;  const Context& C2 = mC2;

            mX.publish(R2, C2);
            mX.releaseRecords();
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(1 == O2.numPublishedRecords());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t6.  Deregister observer o1." << endl;

            ASSERT(0 == mX.deregisterObserver("observer1"));
            ASSERT(1 == X.numRegisteredObservers());

            if (veryVerbose) cout << "\t6a. Publish a record r3." << endl;

            bsl::shared_ptr<Record> mR3(new (oa) Record(&oa), &oa);
            const bsl::shared_ptr<Record>& R3 = mR3;

            Context mC3;  const Context& C3 = mC3;

            mX.publish(R3, C3);
            mX.releaseRecords();
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(2 == O2.numPublishedRecords());

            // ----------------------------------------------------------------

            if (veryVerbose) cout << "\t7.  Deregister observer o2." << endl;

            ASSERT(0 == mX.deregisterObserver("observer2"));
            ASSERT(0 == X.numRegisteredObservers());

            if (veryVerbose) cout << "\t7a. Publish a record r4." << endl;

            bsl::shared_ptr<Record> mR4(new (oa) Record(&oa), &oa);
            const bsl::shared_ptr<Record>& R4 = mR4;

            Context mC4;  const Context& C4 = mC4;

            mX.publish(R4, C4);
            mX.releaseRecords();
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(2 == O2.numPublishedRecords());
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
