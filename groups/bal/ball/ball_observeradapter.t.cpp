// ball_observeradapter.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_observeradapter.h>

#include <ball_context.h>                       // for testing only
#include <ball_record.h>                        // for testing only
#include <ball_recordattributes.h>              // for testing only
#include <ball_transmission.h>                  // for testing only
#include <ball_userfields.h>                    // for testing only
#include <ball_userfieldvalue.h>                // for testing only

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_string.h>
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_sstream.h>

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

namespace USAGE_EXAMPLE_1 {


///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Concrete Observer Derived From 'ball::ObserverAdapter'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following code fragments illustrate the essentials of defining and using
// a concrete observer inherited from 'ball::ObserverAdapter'.
//
// First define a concrete observer 'MyOstreamObserver' derived from
// 'ball::ObserverAdapter' that declares a single publish method accepting a
// const-reference to a 'ball::Record' object:
//..
    class MyOstreamObserver : public ball::ObserverAdapter {
      bsl::ostream  *d_stream;
//
    public:
      explicit MyOstreamObserver(bsl::ostream *stream) : d_stream(stream) { }
      virtual ~MyOstreamObserver();
      virtual void publish(const ball::Record&  record,
                           const ball::Context& context);
    };
//..
// Then, we implement the public methods of 'MyOstreamObserver', including the
// 'publish' method.  This implementation of 'publish' simply prints out the
// content of the record it receives to the stream supplied at construction.
//..
    MyOstreamObserver::~MyOstreamObserver()
    {
    }
//
    void MyOstreamObserver::publish(const ball::Record&  record,
                                    const ball::Context& context)
    {
        const ball::RecordAttributes& fixedFields = record.fixedFields();
//
        *d_stream << fixedFields.timestamp()               << ' '
                  << fixedFields.processID()               << ' '
                  << fixedFields.threadID()                << ' '
                  << fixedFields.fileName()                << ' '
                  << fixedFields.lineNumber()              << ' '
                  << fixedFields.category()                << ' '
                  << fixedFields.message()                 << ' ';
//
        const ball::UserFields& userFields = record.userFields();
        const int numUserFields = userFields.length();
        for (int i = 0; i < numUserFields; ++i) {
            *d_stream << userFields[i] << ' ';
        }
//
        *d_stream << '\n' << bsl::flush;
    }
//..
// Now, we defined a function 'main' in which we create a 'MyOstreamObserver'
// object and assign the address of this object to a 'ball::ObserverAdapter'
// pointer:
//..
    int main(bool verbose)
    {
        bsl::ostringstream     out;
        MyOstreamObserver      myObserver(&out);
        ball::ObserverAdapter *adapter = &myObserver;
//..
// Finally, publish three messages by calling 'publish' method accepting a
// shared-pointer, provided by 'ball::ObserverAdapter', that in turn will call
// the 'publish' method defined in 'MyOstreamObserver':
//..
        bdlt::Datetime         now;
        ball::RecordAttributes fixedFields;
        ball::UserFields       userFields;
//
        const int NUM_MESSAGES = 3;
        for (int n = 0; n < NUM_MESSAGES; ++n) {
            fixedFields.setTimestamp(bdlt::CurrentTime::utc());
//
            bsl::shared_ptr<const ball::Record> handle;
            handle.createInplace(bslma::Default::allocator(),
                                 fixedFields,
                                 userFields);
            adapter->publish(handle,
                             ball::Context(ball::Transmission::e_TRIGGER,
                                           n,
                                           NUM_MESSAGES));
        }
        if (verbose) {
            bsl::cout << out.str() << bsl::endl;
        }
        return 0;
    }
//..
// The above code fragments print to 'stdout' like this:
//..
//  Publish a sequence of three messages.
//  22FEB2012_00:12:12.000 201 31  0
//  22FEB2012_00:12:12.000 202 32  0
//  22FEB2012_00:12:12.000 203 33  0
//..

}  // close namespace USAGE_EXAMPLE_1
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

        USAGE_EXAMPLE_1::main(verbose);

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
        //   Construct an object of a class derived from
        //   'ball::ObserverAdapter' and bind a 'ball::ObserverAdapter'
        //   reference to the object.  Using the base class reference, invoke
        //   the two 'publish' methods, 'releaseRecords' method and destructor.
        //   Verify that the correct implementations of the methods are called.
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
