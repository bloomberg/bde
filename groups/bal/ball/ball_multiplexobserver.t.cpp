// ball_multiplexobserver.t.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_multiplexobserver.h>

#include <ball_context.h>                       // for testing only
#include <ball_defaultobserver.h>               // for testing only
#include <ball_record.h>                        // for testing only
#include <ball_recordattributes.h>              // for testing only
#include <ball_testobserver.h>                  // for testing only
#include <ball_transmission.h>                  // for testing only

#include <bdlma_bufferedsequentialallocator.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a container of Observers that forwards records
// and their corresponding publication contexts to the contained observers.  We
// must ensure that the component behaves correctly as both a container of
// Observers and a re-layer of records and contexts.
//-----------------------------------------------------------------------------
// [ 2] ball::MultiplexObserver(bslma::Allocator *ba = 0);
// [ 2] ~ball::MultiplexObserver();
// [ 4] void publish(const ball::Record& rec, const ball::Context& ctxt);
// [ 5] void publish(const bsl::shared_ptr<const ball::Record>& rec,
//                   const ball::Context&                       ctxt);
// [ 5] void releaseRecords();
// [ 3] int registerObserver(ball::Observer *observer);
// [ 3] int deregisterObserver(ball::Observer *observer);
// [ 3] int numRegisteredObservers() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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
    explicit my_OstreamObserver(ostream& stream) : d_stream(stream) { }
    ~my_OstreamObserver() { }
    void publish(const ball::Record&  record,
                 const ball::Context& context);
};

// my_ostreamobserver.cpp

void my_OstreamObserver::publish(const ball::Record&  record,
                                 const ball::Context& context)
{

    switch (context.transmissionCause()) {
      case ball::Transmission::e_PASSTHROUGH: {
        d_stream << "Single Passthrough Message:" << endl;
      } break;
      case ball::Transmission::e_TRIGGER_ALL: {
        d_stream << "Remotely ";      // no 'break'; concatenated output
      }
      case ball::Transmission::e_TRIGGER: {
        d_stream << "Triggered Publication Sequence: Message ";
        d_stream << context.recordIndex() + 1  // Account for 0-based index.
                 << " of " << context.sequenceLength() << endl;
      } break;
      default: {
        d_stream << "***ERROR*** Unknown Message Cause:" << endl;
      } break;
    }

    d_stream << "\tTimestamp:  " << record.fixedFields().timestamp() << endl;
    d_stream << "\tProcess ID: " << record.fixedFields().processID() << endl;
    d_stream << "\tThread ID:  " << record.fixedFields().threadID()  << endl
             << endl;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::MultiplexObserver Obj;

typedef ball::Context           Ctxt;
typedef ball::Record            Rec;
typedef ball::RecordAttributes  Attr;
typedef ball::TestObserver      TestObs;

const int SEQUENCE_LENGTH = 99;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// The following three helper functions are used to test 'publish' (case 4).

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

static int isNthRecord(const ball::Record& record, int nth)
    // Return 1 if the specified 'record' is the specified 'nth' in the
    // publication sequence, and 0 otherwise.
{
    const ball::RecordAttributes& attr = record.fixedFields();

    if (nth == attr.lineNumber()
     && nth == attr.processID()
     && nth == attr.severity()
     && nth == attr.threadID()) {
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

static int isNthContext(const ball::Context& context, int nth)
    // Return 1 if the specified 'context' is the specified 'nth' in the
    // publication sequence, and 0 otherwise.
{
    if (ball::Transmission::e_TRIGGER == context.transmissionCause()
        &&                     nth == context.recordIndex()
        &&         SEQUENCE_LENGTH == context.sequenceLength()) {
        return 1;                                                     // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------
// skong: REMOVE THIS after it is moved to the ball::TestObserver

class TestAsyncObserver : public ball::Observer {
    // DATA
    bsl::ostream& d_stream;       // target of 'publish' method
    ball::Record  d_record;       // most-recently-published record
    ball::Context d_context;      // most-recently-published context
    int           d_numRecords;   // total number of published records
    int           d_numClears;    // total number of 'clear' called

    // NOT IMPLEMENTED
    TestAsyncObserver(const TestAsyncObserver&);
    TestAsyncObserver& operator=(const TestAsyncObserver&);

  public:
    // CREATORS
    explicit TestAsyncObserver(bsl::ostream&     stream,
                               bslma::Allocator *basicAllocator = 0);

    virtual ~TestAsyncObserver();

    // MANIPULATORS
    virtual void publish(const bsl::shared_ptr<const ball::Record>&  record,
                         const ball::Context&                        context);

    virtual void releaseRecords();

    // ACCESSORS
    int numPublishedRecords() const;

    int numClears() const;

    const ball::Record& lastPublishedRecord() const;

    const ball::Context& lastPublishedContext() const;
};

// CREATORS
inline
TestAsyncObserver::TestAsyncObserver(bsl::ostream&     stream,
                                     bslma::Allocator *basicAllocator)
: d_stream(stream)
, d_record(basicAllocator)
, d_context(basicAllocator)
, d_numRecords(0)
, d_numClears(0)
{
}

TestAsyncObserver::~TestAsyncObserver()
{
}

// MANIPULATORS
void TestAsyncObserver::publish(
                            const bsl::shared_ptr<const ball::Record>& record,
                            const ball::Context&                       context)
{
    d_record  = *record;
    d_context = context;
    ++d_numRecords;
}

void TestAsyncObserver::releaseRecords()
{
    ++d_numClears;
}

// ACCESSORS
inline
int TestAsyncObserver::numPublishedRecords() const
{
    return d_numRecords;
}

inline
int TestAsyncObserver::numClears() const
{
    return d_numClears;
}

inline
const ball::Record& TestAsyncObserver::lastPublishedRecord() const
{
    return d_record;
}

inline
const ball::Context& TestAsyncObserver::lastPublishedContext() const
{
    return d_context;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    //     int veryVerbose = argc > 3; // unused
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    bslma::TestAllocator *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            typedef ball::DefaultObserver my_LogfileObserver;
            typedef ball::DefaultObserver my_EncryptingObserver;

            ball::MultiplexObserver multiplexor;
            ASSERT(0 == multiplexor.numRegisteredObservers());

            ball::DefaultObserver  defaultObserver(&bsl::cout);
            my_LogfileObserver     logfileObserver(&bsl::cout);
            my_EncryptingObserver  encryptingObserver(&bsl::cout);

            ASSERT(0 == multiplexor.registerObserver(&defaultObserver));
            ASSERT(0 == multiplexor.registerObserver(&logfileObserver));
            ASSERT(0 == multiplexor.registerObserver(&encryptingObserver));
            ASSERT(3 == multiplexor.numRegisteredObservers());

            // Do *not* do this.  It significantly increases the level # of
            // this component.
            //
            // ball::LoggerManager::initSingleton(&multiplexor);

            ASSERT(0 == multiplexor.deregisterObserver(&defaultObserver));
            ASSERT(0 == multiplexor.deregisterObserver(&logfileObserver));
            ASSERT(0 == multiplexor.deregisterObserver(&encryptingObserver));
            ASSERT(0 == multiplexor.numRegisteredObservers());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ASYNCHRONOUS PUBLISH AND RELEASE RECORDS
        //
        // Concerns:
        //
        //   The primary concerns are:
        //   a. the async publish method
        //      'publish(const bsl::shared_ptr<const ball::Record>&,
        //               const ball::Context&)'
        //      correctly forwards records and their corresponding publication
        //      contexts to all registered observers that should received them.
        //   b. the 'clear' method correctly calls the 'clear' method of all
        //      registered observers.
        //
        // Plan:
        //   Construct a pair of multiplex observers x and y, and a trio of
        //   'TestAsyncObserver' o1, o2, and o3.  Arrange x, y, o1, o2, and o3
        //   into various configurations with o1, o2, and o3 registered with at
        //   most one of the multiplex observers.  Also register y with x for a
        //   portion of the test.  Publish several distinct records to x and
        //   y and verify that all registered observers correctly receive the
        //   records and corresponding publication contexts.  Invoke 'clear'
        //   method of x and y and verify that all registered observers
        //   correctly have their 'clear' methods called.  The helper functions
        //   'nextRecord', 'isNthRecord', and 'isNthContext' are used to
        //   verify that the records and contexts are properly transmitted.
        //
        // Testing:
        //   void publish(const bsl::shared_ptr<const ball::Record>& rec,
        //                const ball::Context&                       ctxt);
        //   void releaseRecords();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing asynchronous 'publish'" << endl
                                  << "==============================" << endl;

        {
            TestAsyncObserver mO1(bsl::cout);
            const TestAsyncObserver& O1 = mO1;

            TestAsyncObserver mO2(bsl::cout);
            const TestAsyncObserver& O2 = mO2;

            TestAsyncObserver mO3(bsl::cout);
            const TestAsyncObserver& O3 = mO3;

            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            Obj mY;  const Obj& Y = mY;
            ASSERT(0 == Y.numRegisteredObservers());

            bsl::shared_ptr<ball::Record> mR(
                              new (testAllocator) ball::Record(&testAllocator),
                              &testAllocator);
            const bsl::shared_ptr<ball::Record>& R = mR;

            Ctxt mC(ball::Transmission::e_TRIGGER, 0, SEQUENCE_LENGTH);
            const Ctxt& C = mC;

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(0 == O1.numPublishedRecords());

            const int I1 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I1));
            mC.setRecordIndexRaw(I1);        ASSERT(1 == isNthContext(C, I1));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(1 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());

            const int I2 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I2));
            mC.setRecordIndexRaw(I2);        ASSERT(1 == isNthContext(C, I2));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(2 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == O1.numPublishedRecords());

            const int I3 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I3));
            mC.setRecordIndexRaw(I3);        ASSERT(1 == isNthContext(C, I3));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(3 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I3));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I3));

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(0 == O2.numPublishedRecords());

            const int I4 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I4));
            mC.setRecordIndexRaw(I4);        ASSERT(1 == isNthContext(C, I4));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(3 == O1.numPublishedRecords());
            ASSERT(4 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I4));
            ASSERT(1 == O2.numPublishedRecords());
            ASSERT(1 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I4));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(&mY));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I5 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I5));
            mC.setRecordIndexRaw(I5);        ASSERT(1 == isNthContext(C, I5));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(4 == O1.numPublishedRecords());
            ASSERT(5 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I5));
            ASSERT(2 == O2.numPublishedRecords());
            ASSERT(2 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I5));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mY.registerObserver(&mO3));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());
            ASSERT(0 == O3.numPublishedRecords());

            const int I6 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I6));
            mC.setRecordIndexRaw(I6);        ASSERT(1 == isNthContext(C, I6));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(5 == O1.numPublishedRecords());
            ASSERT(6 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I6));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(3 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(1 == O3.numPublishedRecords());
            ASSERT(1 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I6));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O3
            ASSERT(0 == mX.deregisterObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I7 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I7));
            mC.setRecordIndexRaw(I7);        ASSERT(1 == isNthContext(C, I7));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(6 == O1.numPublishedRecords());
            ASSERT(7 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I7));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(4 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(2 == O3.numPublishedRecords());
            ASSERT(2 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I7));

            //     X
            //    / |
            //  O1  Y
            //     / |
            //   O3  O2
            ASSERT(0 == mY.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(2 == Y.numRegisteredObservers());
            ASSERT(3 == O2.numPublishedRecords());

            const int I8 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I8));
            mC.setRecordIndexRaw(I8);        ASSERT(1 == isNthContext(C, I8));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(7 == O1.numPublishedRecords());
            ASSERT(8 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I8));
            ASSERT(4 == O2.numPublishedRecords());
            ASSERT(5 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I8));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(3 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O2
            ASSERT(0 == mY.deregisterObserver(&mO3));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I9 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I9));
            mC.setRecordIndexRaw(I9);        ASSERT(1 == isNthContext(C, I9));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(8 == O1.numPublishedRecords());
            ASSERT(9 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I9));
            ASSERT(5 == O2.numPublishedRecords());
            ASSERT(6 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //   |   |
            //   O1  O2
            ASSERT(0 == mX.deregisterObserver(&mY));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I10 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I10));
            mC.setRecordIndexRaw(I10);       ASSERT(1 == isNthContext(C, I10));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(10 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(5 == O2.numPublishedRecords());
            ASSERT(7 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I11 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I11));
            mC.setRecordIndexRaw(I11);       ASSERT(1 == isNthContext(C, I11));
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(10 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(6 == O2.numPublishedRecords());
            ASSERT(8 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //       |
            //       O2
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I12 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I12));
            mC.setRecordIndexRaw(I12);       ASSERT(1 == isNthContext(C, I12));
            mX.publish(R, C);
            mX.releaseRecords();
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(6 == O2.numPublishedRecords());
            ASSERT(8 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I13 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I13));
            mC.setRecordIndexRaw(I13);       ASSERT(1 == isNthContext(C, I13));
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(7 == O2.numPublishedRecords());
            ASSERT(9 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            ASSERT(0 == mY.deregisterObserver(&mO2));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I14 = nextRecord(*mR); ASSERT(1 == isNthRecord(*R, I14));
            mC.setRecordIndexRaw(I14);       ASSERT(1 == isNthContext(C, I14));
            mX.publish(R, C);
            mX.releaseRecords();
            mY.publish(R, C);
            mY.releaseRecords();
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(11 == O1.numClears());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(7 == O2.numPublishedRecords());
            ASSERT(10 == O2.numClears());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(4 == O3.numClears());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING SYNCHRONOUS (DEPRECATED) PUBLISH
        //
        // Concerns:
        //   The primary concern is that the sync publish method
        //   'publish(const ball::Record&, const ball::Context&)' correctly
        //   forwards records and their corresponding publication contexts
        //   to all registered observers that should received them.
        //
        // Plan:
        //   Construct a pair of multiplex observers x and y, and a trio of
        //   test observers o1, o2, and o3.  Arrange x, y, o1, o2, and o3 into
        //   various configurations with o1, o2, and o3 registered with at most
        //   one of the multiplex observers.  Also register y with x for a
        //   portion of the test.  Publish several distinct records to x and
        //   y and verify that all registered observers correctly receive the
        //   records and corresponding publication contexts.  The helper
        //   functions 'nextRecord', 'isNthRecord', and 'isNthContext' are
        //   used to verify that the records and contexts are properly
        //   transmitted.
        //
        // Testing:
        //   void publish(const ball::Record& rec, const ball::Context& ctxt);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing synchronous 'publish'" << endl
                                  << "=============================" << endl;

        {
            TestObs mO1(bsl::cout);  const TestObs& O1 = mO1;
            TestObs mO2(bsl::cout);  const TestObs& O2 = mO2;
            TestObs mO3(bsl::cout);  const TestObs& O3 = mO3;

            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            Obj mY;  const Obj& Y = mY;
            ASSERT(0 == Y.numRegisteredObservers());


            Rec mR;  const Rec& R = mR;

            Ctxt mC(ball::Transmission::e_TRIGGER, 0, SEQUENCE_LENGTH);
            const Ctxt& C = mC;

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(0 == O1.numPublishedRecords());

            const int I1 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I1));
            mC.setRecordIndexRaw(I1);       ASSERT(1 == isNthContext(C, I1));
            mX.publish(R, C);
            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());

            const int I2 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I2));
            mC.setRecordIndexRaw(I2);       ASSERT(1 == isNthContext(C, I2));
            mX.publish(R, C);
            ASSERT(1 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I1));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I1));

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == O1.numPublishedRecords());

            const int I3 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I3));
            mC.setRecordIndexRaw(I3);       ASSERT(1 == isNthContext(C, I3));
            mX.publish(R, C);
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I3));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I3));

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(0 == O2.numPublishedRecords());

            const int I4 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I4));
            mC.setRecordIndexRaw(I4);       ASSERT(1 == isNthContext(C, I4));
            mX.publish(R, C);
            ASSERT(3 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I4));
            ASSERT(1 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I4));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I4));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(&mY));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I5 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I5));
            mC.setRecordIndexRaw(I5);       ASSERT(1 == isNthContext(C, I5));
            mX.publish(R, C);
            ASSERT(4 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I5));
            ASSERT(2 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I5));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I5));

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mY.registerObserver(&mO3));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());
            ASSERT(0 == O3.numPublishedRecords());

            const int I6 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I6));
            mC.setRecordIndexRaw(I6);       ASSERT(1 == isNthContext(C, I6));
            mX.publish(R, C);
            ASSERT(5 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I6));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(1 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I6));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O3
            ASSERT(0 == mX.deregisterObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I7 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I7));
            mC.setRecordIndexRaw(I7);       ASSERT(1 == isNthContext(C, I7));
            mX.publish(R, C);
            ASSERT(6 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I7));
            ASSERT(3 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I6));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I6));
            ASSERT(2 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I7));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I7));

            //     X
            //    / |
            //  O1  Y
            //     / |
            //   O3  O2
            ASSERT(0 == mY.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(2 == Y.numRegisteredObservers());
            ASSERT(3 == O2.numPublishedRecords());

            const int I8 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I8));
            mC.setRecordIndexRaw(I8);       ASSERT(1 == isNthContext(C, I8));
            mX.publish(R, C);
            ASSERT(7 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I8));
            ASSERT(4 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I8));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O2
            ASSERT(0 == mY.deregisterObserver(&mO3));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I9 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I9));
            mC.setRecordIndexRaw(I9);       ASSERT(1 == isNthContext(C, I9));
            mX.publish(R, C);
            ASSERT(8 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I9));
            ASSERT(5 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //   |   |
            //   O1  O2
            ASSERT(0 == mX.deregisterObserver(&mY));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I10 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I10));
            mC.setRecordIndexRaw(I10);       ASSERT(1 == isNthContext(C, I10));
            mX.publish(R, C);
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(5 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I9));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I9));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I11 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I11));
            mC.setRecordIndexRaw(I11);       ASSERT(1 == isNthContext(C, I11));
            mY.publish(R, C);
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(6 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            //       |
            //       O2
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            const int I12 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I12));
            mC.setRecordIndexRaw(I12);       ASSERT(1 == isNthContext(C, I12));
            mX.publish(R, C);
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(6 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I11));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I11));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            const int I13 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I13));
            mC.setRecordIndexRaw(I13);       ASSERT(1 == isNthContext(C, I13));
            mY.publish(R, C);
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(7 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));

            //   X   Y
            ASSERT(0 == mY.deregisterObserver(&mO2));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            const int I14 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I14));
            mC.setRecordIndexRaw(I14);       ASSERT(1 == isNthContext(C, I14));
            mX.publish(R, C);
            mY.publish(R, C);
            ASSERT(9 == O1.numPublishedRecords());
            ASSERT(1 == isNthRecord(O1.lastPublishedRecord(), I10));
            ASSERT(1 == isNthContext(O1.lastPublishedContext(), I10));
            ASSERT(7 == O2.numPublishedRecords());
            ASSERT(1 == isNthRecord(O2.lastPublishedRecord(), I13));
            ASSERT(1 == isNthContext(O2.lastPublishedContext(), I13));
            ASSERT(3 == O3.numPublishedRecords());
            ASSERT(1 == isNthRecord(O3.lastPublishedRecord(), I8));
            ASSERT(1 == isNthContext(O3.lastPublishedContext(), I8));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OBSERVER REGISTRATION
        //
        // Concerns:
        //   Our concerns are that 'registerObserver' and 'deregisterObserver'
        //   produce the expected side-effects on the container of observers
        //   and that 'numRegisteredObservers' returns the current number of
        //   registered observers.
        //
        // Plan:
        //   Construct a pair of multiplex observers x and y, and a trio of
        //   test observers o1, o2, and o3.  Arrange x, y, o1, o2, and o3 into
        //   various configurations with o1, o2, and o3 registered with at most
        //   one of the multiplex observers.  Also register y with x for a
        //   portion of the test.  Test the return value of the three methods
        //   under test to infer that they fulfill their contractual
        //   obligations.
        //
        // Testing:
        //   int registerObserver(ball::Observer *observer);
        //   int deregisterObserver(ball::Observer *observer);
        //   int numRegisteredObservers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Observer Registration" << endl
                                  << "=============================" << endl;

        if (verbose)
            cout << "Testing invalid registration and deregistration." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            TestObs mO1(bsl::cout);
            TestObs mO2(bsl::cout);

            ASSERT(0 != mX.registerObserver(0));
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 != mX.deregisterObserver(&mO1));
            ASSERT(0 != mX.deregisterObserver(0));
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            ASSERT(0 != mX.registerObserver(0));
            ASSERT(0 != mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            ASSERT(0 != mX.deregisterObserver(&mO2));
            ASSERT(0 != mX.deregisterObserver(0));
            ASSERT(1 == X.numRegisteredObservers());

            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 != mX.deregisterObserver(&mO1));
            ASSERT(0 != mX.deregisterObserver(0));
            ASSERT(0 == X.numRegisteredObservers());
        }

        if (verbose)
            cout << "Testing valid registration and deregistration." << endl;
        {
            //  X
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            //  Y
            Obj mY;  const Obj& Y = mY;
            ASSERT(0 == Y.numRegisteredObservers());

            TestObs mO1(bsl::cout);
            TestObs mO2(bsl::cout);
            TestObs mO3(bsl::cout);

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            //   X
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());

            //   X
            //   |
            //   O1
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            //     X
            //    / |
            //  O1  O2
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            ASSERT(0 == mX.registerObserver(&mY));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());

            //     --X--
            //    /  |  |
            //  O1   O2 Y
            //          |
            //          O3
            ASSERT(0 == mY.registerObserver(&mO3));
            ASSERT(3 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1  Y
            //      |
            //      O3
            ASSERT(0 == mX.deregisterObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1  Y
            //     / |
            //   O3  O2
            ASSERT(0 == mY.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(2 == Y.numRegisteredObservers());

            //     X
            //    / |
            //  O1   Y
            //       |
            //       O2
            ASSERT(0 == mY.deregisterObserver(&mO3));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            //   |   |
            //   O1  O2
            ASSERT(0 == mX.deregisterObserver(&mY));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            //       |
            //       O2
            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(1 == Y.numRegisteredObservers());

            //   X   Y
            ASSERT(0 == mY.deregisterObserver(&mO2));
            ASSERT(0 == X.numRegisteredObservers());
            ASSERT(0 == Y.numRegisteredObservers());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CTOR AND DTOR
        //
        // Concerns:
        //   The basic concern is that the constructor and destructor operate
        //   as expected.  We have the following specific concerns:
        //     1. The constructor:
        //        a. Creates the correct initial state (empty).
        //        b. Has the internal memory management system hooked up
        //           properly so that *all* internally allocated memory draws
        //           from the same user-supplied allocator whenever one is
        //           specified.
        //     2. The destructor properly deallocates all allocated memory to
        //        its corresponding allocator.
        //
        // Plan:
        //   To address concerns 1a & 1b, create an object:
        //     - With and without passing in an allocator to the constructor.
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bdlma::BufferedSequentialAllocator') and never
        //       destroyed.
        //
        //   To address concern 2, allow each object to leave scope, so that
        //   the destructor asserts internal object invariants appropriately.
        //   Use a 'bslma::TestAllocator' to verify memory usage.
        //
        // Testing:
        //   ball::MultiplexObserver(bslma::Allocator *ba = 0);
        //   ~ball::MultiplexObserver();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'ctor' and 'dtor'" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());
        }
        {
            TestObs mO1(bsl::cout);
            TestObs mO2(bsl::cout);

            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;
        const int NUM_BLOCKS_IN_USE = testAllocator.numBlocksInUse();
        const int NUM_BYTES_IN_USE  = testAllocator.numBytesInUse();
        {
            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());
        }
        ASSERT(NUM_BLOCKS_IN_USE == testAllocator.numBlocksInUse());
        ASSERT(NUM_BYTES_IN_USE  == testAllocator.numBytesInUse());

        {
            TestObs mO1(bsl::cout);
            TestObs mO2(bsl::cout);

            Obj mX(&testAllocator);  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());

            // White-box test: 'publish' does not utilize the supplied
            // allocator.

            const int NUM_BLOCKS_TOTAL = testAllocator.numBlocksTotal();
            const int NUM_BYTES_TOTAL  = testAllocator.numBytesTotal();
            Rec  mR;  const Rec&  R = mR;
            Ctxt mC;  const Ctxt& C = mC;
            mX.publish(R, C);
            ASSERT(NUM_BLOCKS_TOTAL == testAllocator.numBlocksTotal());
            ASSERT(NUM_BYTES_TOTAL  == testAllocator.numBytesTotal());
        }
        ASSERT(NUM_BLOCKS_IN_USE == testAllocator.numBlocksInUse());
        ASSERT(NUM_BYTES_IN_USE  == testAllocator.numBytesInUse());

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            TestObs mO1(bsl::cout);
            TestObs mO2(bsl::cout);

            char memory[2048];
            bdlma::BufferedSequentialAllocator a(memory, sizeof memory);
            Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(&a);
            Obj& mX = *doNotDelete;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());

            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic functionality.  In particular, we want
        //   to demonstrate a base-line level of correct operation for all
        //   methods.
        //
        // Plan:
        //   1.  Construct an object x.
        //   2.  Register an observer o1.
        //   3.  Deregister observer o1.
        //   4.  Reregister observer o1.
        //   4a. Publish a record r1.
        //   5.  Register an observer o2.
        //   5a. Publish a record r2.
        //   6.  Deregister observer o1.
        //   6a. Publish a record r3.
        //   7.  Deregister observer o2.
        //   7a. Publish a record r4.
        //   8.  Destroy x.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        {
            // ----------------------------------------------------------------

            if (verbose) cout << "\t1.  Construct an object x." << endl;

            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t2.  Register an observer o1." << endl;

            TestObs mO1(bsl::cout);  const TestObs& O1 = mO1;
            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t3.  Deregister observer o1." << endl;

            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(0 == X.numRegisteredObservers());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t4.  Reregister observer o1." << endl;

            ASSERT(0 == mX.registerObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());
            ASSERT(0 == O1.numPublishedRecords());

            if (verbose) cout << "\t4a. Publish a record r1." << endl;

            Rec  mR1;  const Rec&  R1 = mR1;
            Ctxt mC1;  const Ctxt& C1 = mC1;

            mX.publish(R1, C1);
            ASSERT(1 == O1.numPublishedRecords());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t5.  Register an observer o2." << endl;

            TestObs mO2(bsl::cout);  const TestObs& O2 = mO2;
            ASSERT(0 == mX.registerObserver(&mO2));
            ASSERT(2 == X.numRegisteredObservers());
            ASSERT(0 == O2.numPublishedRecords());

            if (verbose) cout << "\t5a. Publish a record r2." << endl;

            Rec  mR2;  const Rec&  R2 = mR2;
            Ctxt mC2;  const Ctxt& C2 = mC2;

            mX.publish(R2, C2);
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(1 == O2.numPublishedRecords());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t6.  Deregister observer o1." << endl;

            ASSERT(0 == mX.deregisterObserver(&mO1));
            ASSERT(1 == X.numRegisteredObservers());

            if (verbose) cout << "\t6a. Publish a record r3." << endl;

            Rec  mR3;  const Rec&  R3 = mR3;
            Ctxt mC3;  const Ctxt& C3 = mC3;

            mX.publish(R3, C3);
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(2 == O2.numPublishedRecords());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t7.  Deregister observer o2." << endl;

            ASSERT(0 == mX.deregisterObserver(&mO2));
            ASSERT(0 == X.numRegisteredObservers());

            if (verbose) cout << "\t7a. Publish a record r4." << endl;

            Rec  mR4;  const Rec&  R4 = mR4;
            Ctxt mC4;  const Ctxt& C4 = mC4;

            mX.publish(R4, C4);
            ASSERT(2 == O1.numPublishedRecords());
            ASSERT(2 == O2.numPublishedRecords());

            // ----------------------------------------------------------------

            if (verbose) cout << "\t8.  Destroy x." << endl;
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING: Warning about destroyed observer.
        //
        // Concerns:
        //  1 Verify that we log a message to 'stderr' if a registered
        //    observer is destroyed before the multiplex observer is destroyed.
        //
        //  2 Verify that we log a message to 'stderr' if a registered
        //    observer is destroyed before a call to 'releaseRecords'.
        //
        // Plan:
        //  1 Create an observer, and supply it multiplex observer, then
        //    destroy the observer prior to calling 'releaseRecords' and the
        //    destructor of the multiplex observer.  Visually verify that
        //    output is written to stderr.
        //
        //  Note that this is a test of undefined behavior and must be run,
        //  and verified, manually.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Warning about destroyed observer." << endl
                          << "================================" << endl;

        {
            Obj x;
            {
                ball::TestObserver testObserver(cout);
                x.registerObserver(&testObserver);
            }
            x.releaseRecords();
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
