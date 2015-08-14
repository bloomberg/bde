// ball_observeradapter.t.cpp                                         -*-C++-*-

#include <ball_observeradapter.h>

#include <ball_record.h>                        // for testing only
#include <ball_context.h>                       // for testing only
#include <ball_transmission.h>                  // for testing only

#include <bdlt_datetimeutil.h>                     // for testing only
#include <bdlt_epochutil.h>                     // for testing only

#include <bslma_testallocator.h>                // for testing only

#include <bsls_protocoltest.h>                  // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a observer adapter class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) that a usage example
// obtains the behavior specified by the protocol from the concrete subclass.
//-----------------------------------------------------------------------------
// [ 1] virtual ~ball::ObserverAdapter();
// [ 1] virtual void publish(const record&, const context&) = 0;
// [ 1] virtual void publish(const sharedptr&, const context&);
// [ 1] virtual void releaseRecords();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

struct ConcreteObserver : public ball::ObserverAdapter {
    using ball::ObserverAdapter::publish;
    void publish(const ball::Record&, const ball::Context&)
    {
        cout << "ConcreteObserver::publish(record&)" << endl;
    }
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_ostreamobserver.h

class my_OstreamObserver : public ball::ObserverAdapter {
    ostream *d_stream;

  public:
    using ball::ObserverAdapter::publish;
    explicit my_OstreamObserver(ostream& stream) : d_stream(&stream) { }
    virtual ~my_OstreamObserver();
    virtual void publish(const ball::Record&  record,
                         const ball::Context& context);
};

// my_ostreamobserver.cpp

my_OstreamObserver::~my_OstreamObserver()
{
}

void my_OstreamObserver::publish(const ball::Record&  record,
                                 const ball::Context& )
{
    const ball::RecordAttributes& fixedFields = record.fixedFields();

    *d_stream << '\n';

    *d_stream << fixedFields.timestamp()               << ' '
              << fixedFields.processID()               << ' '
              << fixedFields.threadID()                << ' '
              << fixedFields.fileName()                << ' '
              << fixedFields.lineNumber()              << ' '
              << fixedFields.category()                << ' '
              << fixedFields.message()                 << ' ';

    const ball::UserFields& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        *d_stream << userFields[i] << ' ';
    }
    *d_stream << '\n' << bsl::flush;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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
            char buf[2048];
            ostrstream out(buf, sizeof buf);

            my_OstreamObserver    myObserver(out);
            ball::ObserverAdapter *adapter = &myObserver;

            bdlt::Datetime         now;
            ball::RecordAttributes fixed;
            ball::UserFields  userFields;

            if (verbose)
                cout << "Publish a sequence of three messages." << endl;
            {
                out.seekp(0);
                const int NUM_MESSAGES = 3;
                for (int n = 0; n < NUM_MESSAGES; ++n) {
                    now = bdlt::EpochUtil::convertFromTimeT(time(0));
                    fixed.setTimestamp(now);
                    fixed.setProcessID(201 + n);
                    fixed.setThreadID(31 + n);

                    bsl::shared_ptr<const ball::Record> handle(
                              new (testAllocator) ball::Record(fixed,
                                                              userFields,
                                                              &testAllocator),
                              &testAllocator);
                    adapter->publish(
                                  handle,
                                  ball::Context(ball::Transmission::BAEL_TRIGGER,
                                               n,
                                               NUM_MESSAGES));
                }
                out << ends;
                if (veryVerbose) cout << buf << endl;
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        // Concerns:
        //   We must ensure that (1) a subclass of the 'ball::ObserverAdapter'
        //   class compiles and links when all virtual functions are defined,
        //   and (2) the functions are in fact virtual.
        //
        // Plan:
        //   Construct an object of a class derived from 'ball::ObserverAdapter'
        //   and bind a 'ball::ObserverAdapter' reference to the object.  Using
        //   the base class reference, invoke the two 'publish' methods,
        //   'releaseRecords' method and destructor.  Verify that the correct
        //   implementations of the methods are called.
        //
        // Testing:
        //   virtual ~ball::ObserverAdapter();
        //   virtual void publish(const record&, const context&) = 0;
        //   virtual void publish(const sharedptr&, const context&);
        //   virtual void releaseRecords();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        ConcreteObserver concreteObserver;
        ball::ObserverAdapter& adapter =
                        dynamic_cast<ball::ObserverAdapter&>(concreteObserver);

        adapter.publish(ball::Record(), ball::Context());

        bsl::shared_ptr<const ball::Record> handle(
                             new (testAllocator) ball::Record(&testAllocator),
                             &testAllocator);
        adapter.publish(handle, ball::Context());
        adapter.releaseRecords();

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
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
