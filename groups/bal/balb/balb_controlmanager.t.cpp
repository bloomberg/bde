// balb_controlmanager.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balb_controlmanager.h>

#include <bslim_testutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

namespace {

void onEcho(const bsl::string& prefix, bsl::istream& stream)
{
    bsl::string word;
    bsl::cout << "onEcho: \"" << prefix << ' ';
    while (stream.good()) {
        stream >> word;
        bsl::cout << word << ' ';
    }
    bsl::cout << '\"' << bsl::endl;
}

void noop(const string&, istream&)
{}

struct Incrementer
{
    int* d_value_p;

    Incrementer (int* value_p) : d_value_p(value_p) {
        *d_value_p = 0;
    }

    void operator()(const bsl::string&, bsl::istream&) {
        ++*d_value_p;
    }
};

class Dispatcher
{
    typedef vector<pair<string, int> > FunctionVector;

    balb::ControlManager* d_manager_p; //held
    FunctionVector  d_functions;
    int             d_iterations;
    bslmt::Mutex     d_mutex;
    bslmt::Barrier   d_barrier;

  public:
    Dispatcher(int iterations, balb::ControlManager* manager_p)
    : d_manager_p(manager_p), d_iterations(iterations), d_barrier(2)
    {}

    void addFunction(const string& function)
    {
        bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
        d_functions.push_back(bsl::make_pair(function, 0));
    }

    int count(int i)
    {
        return d_functions[i].second;
    }

    bslmt::Barrier* barrier()
    {
        return &d_barrier;
    }

    void run()
    {
        d_barrier.wait();
        for (int i = 1; i < d_iterations; ++i) {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
            for (FunctionVector::iterator j = d_functions.begin();
                 j != d_functions.end();
                 ++j)
            {
                d_manager_p->dispatchMessage(j->first);
                ++j->second;
            }
        }
        d_barrier.wait();
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
            for (FunctionVector::iterator j = d_functions.begin();
                 j != d_functions.end();
                 ++j)
            {
                d_manager_p->dispatchMessage(j->first);
                ++j->second;
            }
        }
        d_barrier.wait();
    }
};

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   Usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Usage Example" << endl
                 << "=============" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            balb::ControlManager manager(&ta);
            manager.registerHandler("ECHO",
                                    "<text>",
                                    "Print specified text to terminal",
                                    &onEcho);

            manager.registerHandler(
                             "HELP",
                             "",
                             "Print documentation",
                             bdlf::BindUtil::bind(
                                 &balb::ControlManager::printUsageHelper,
                                 &manager,
                                 &bsl::cout,
                                 bsl::string("The following commands are "
                                             "accepted by the test driver:")));

            manager.dispatchMessage("ECHO repeat this text");
            manager.dispatchMessage("echo matching is case-insensitive");
            manager.dispatchMessage("HELP");
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'registerUsageHandler' METHOD
        //
        // Concerns:
        //: 1 There is no default "HELP" message handler.
        //: 2 The "HELP" message handler lists "HELP".
        //: 3 The "HELP" message handler can be reinstalled without effect.
        //: 4 The "HELP" message handler lists all registered messages.
        //: 5 The "HELP" message prefix is case insensitive and any additional
        //:   arguments are ignored.
        //
        // Plan:
        //: 1 A series of ad hoc tests using a default constructed
        //:   'ControlManager' object.
        //:
        //: 2 Check response before the HELP message handler is installed
        //:   ('dispatchMesage' fails so no default handler), after
        //:   installation, and after a second invocation of
        //:   'registerUsageHandler'.
        //:
        //: 3 Incrementally register additional message handlers.  Confirm that
        //:   the HELP message handler s shows all currently installed
        //:   handlers in alphabetical order.
        //:
        //: 4 The HELP message handler shows the expected result irrespective
        //:   of the case of the prefix and irrespective of additional text
        //:   after the prefix.
        //
        // Testing:
        //   registerUsageHandler(bsl::ostream& stream);
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "TESTING 'registerUsageHandler' METHOD" << endl
                << "=====================================" << endl;
        }

        int                  rc;
        balb::ControlManager manager;
        bsl::ostringstream   oss;
        bsl::string          usageMessage;

        // no HELP handler installed
        rc = manager.dispatchMessage("HELP");
        ASSERTV(rc, 0 != rc);

        // Install HELP handler
        rc = manager.registerUsageHandler(oss);
        ASSERTV(rc, 0 == rc);

        // HELP is the one and only handler
        rc = manager.dispatchMessage("HELP");
        ASSERTV(rc, 0 == rc);

        const char* EXPECTED1 =
                      "This process responds to the following messages: " "\n"
                      "    HELP "                                         "\n"
                      "        Display this message"                      "\n";
        usageMessage = oss.str();
        ASSERTV(EXPECTED1,   usageMessage,
                EXPECTED1 == usageMessage);
        oss.str(""); oss.clear();

        // HELP is case in-sensitive
        rc = manager.dispatchMessage("hElP");
        ASSERTV(rc, 0 == rc);
        usageMessage = oss.str();
        ASSERTV(EXPECTED1,   usageMessage,
                EXPECTED1 == usageMessage);
        oss.str(""); oss.clear();

        // HELP handler can be re-registered and still show the same behavior.
        rc = manager.registerUsageHandler(oss);
        ASSERTV(rc, 0 < rc);
        rc = manager.dispatchMessage("HeLp");
        ASSERTV(rc, 0 == rc);
        usageMessage = oss.str();
        ASSERTV(EXPECTED1,   usageMessage,
                EXPECTED1 == usageMessage);
        oss.str(""); oss.clear();

        // HELP will show list two messages once a second is registered.
        rc = manager.registerHandler("ECHO",
                                     "<text>",
                                     "Print specified text to terminal",
                                     &onEcho);
        ASSERTV(rc, 0 == rc);
        rc = manager.dispatchMessage("help");
        ASSERTV(rc, 0 == rc);

        const char* EXPECTED2 =
                      "This process responds to the following messages: " "\n"
                      "    ECHO <text>"                                   "\n"
                      "        Print specified text to terminal"          "\n"
                      "    HELP "                                         "\n"
                      "        Display this message"                      "\n";
        usageMessage = oss.str();
        ASSERTV(EXPECTED2,   usageMessage,
                EXPECTED2 == usageMessage);
        oss.str(""); oss.clear();

        // HELP will show list three messages once a third is registered.
        rc = manager.registerHandler("NOOP",
                                     "",
                                     "Do nothing (no operation)",
                                     &noop);
        ASSERTV(rc, 0 == rc);
        rc = manager.dispatchMessage(
                               "Help me Obi-Wan Kenobi. You're my only hope.");
        ASSERTV(rc, 0 == rc);

        const char* EXPECTED3 =
                      "This process responds to the following messages: " "\n"
                      "    ECHO <text>"                                   "\n"
                      "        Print specified text to terminal"          "\n"
                      "    HELP "                                         "\n"
                      "        Display this message"                      "\n"
                      "    NOOP "                                         "\n"
                      "        Do nothing (no operation)"                 "\n";
        usageMessage = oss.str();
        ASSERTV(EXPECTED3,   usageMessage,
                EXPECTED3 == usageMessage);
        oss.str(""); oss.clear();

      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Dispatching and Thread Safety
        //
        // Concerns:
        //   The 'dispatchMessage' function may be run in high contention with
        //   another thread that is registering message handlers.
        //
        // Plan:
        //   Invoke the 'dispatchMessage' function repeatedly in a background
        //   thread while registering handlers in the main thread.
        //
        // Testing:
        //   Thread safety
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing Thread Safety" << endl
                 << "=====================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            using namespace bdlf::PlaceHolders;

            enum {NUM_ITERATIONS = 30000, NUM_FUNCTIONS=60};
            balb::ControlManager manager(&ta);
            Dispatcher dispatcher(NUM_ITERATIONS, &manager);

            int counts[NUM_FUNCTIONS+1];
            manager.registerHandler("FOOBAR", "", "", Incrementer(&counts[0]));
            dispatcher.addFunction("FOOBAR zippy");

            bslmt::ThreadAttributes detached;
            bslmt::ThreadUtil::Handle dummy;
            detached.setDetachedState(
                                   bslmt::ThreadAttributes::e_CREATE_DETACHED);
            ASSERT(0 == bslmt::ThreadUtil::create(&dummy,
                                                  detached,
                                                  bdlf::BindUtil::bind(
                                                              &Dispatcher::run,
                                                              &dispatcher)));
            dispatcher.barrier()->wait();

            string currentTaskName;
            vector<string> functionsToAdd;

            for (int i = 0; i < NUM_FUNCTIONS; ++i) {
                if (0 == i % (NUM_FUNCTIONS / 12)) {
                    for (vector<string>::iterator j  = functionsToAdd.begin();
                                                  j != functionsToAdd.end();
                                                ++j)
                    {
                        dispatcher.addFunction(*j);
                    }
                    functionsToAdd.clear();
                }

                ostringstream os;
                os << "FAKE" << i;
                manager.registerHandler(os.str(), "", "",
                                        Incrementer(&counts[i+1]));
                os << " args go here";
                functionsToAdd.push_back(os.str());
           }
           for (vector<string>::iterator j  = functionsToAdd.begin();
                                         j != functionsToAdd.end();
                                       ++j)
           {
               dispatcher.addFunction(*j);
           }
           dispatcher.barrier()->wait();

           if (veryVerbose) {
              cout << "Done adding functions..." << endl;
           }

           dispatcher.barrier()->wait();
           for (int i = 0; i < NUM_FUNCTIONS+1; ++i) {
               LOOP_ASSERT(i, 0 < counts[i]);
               LOOP3_ASSERT(i, dispatcher.count(i), counts[i],
                            dispatcher.count(i) == counts[i]);
               if (veryVeryVeryVerbose) {
                   cout << i << ": " << counts[i] << endl;
               }
           }
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Task Creation and Handler Registration
        //
        // Concerns:
        //   - Category names must be unique.
        //   - A category must first be created before a handler function can
        //     be registered.
        //   - The manager safely returns non-zero values when attempting to
        //     dispatch messages which do not specify a known message or
        //     sub-task.
        //   - Categories may be deleted.
        //   - Handlers may be deregistered.
        //   - The manager safely returns non-zero values when attempting to
        //     dispatch a message for a prefix with no registered handler.
        //
        // Testing:
        //   createCategory, registerHandler, deregisterHandler,
        //   dispatchMessage
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "Testing Task Creation and Handler Registration" << endl
                << "==============================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
           using namespace bdlf::PlaceHolders;

           balb::ControlManager manager(&ta);

           ASSERT(0 == manager.registerHandler("1", "", "", &noop));
           ASSERT(0 != manager.registerHandler("1", "", "", &noop));

           ASSERT(0 == manager.registerHandler("2", "", "", &noop));
           ASSERT(0 != manager.registerHandler("2", "", "", &noop));

           ASSERT(0 != manager.dispatchMessage("BOGUS MESSAGE"));

           ASSERT(0 == manager.dispatchMessage("1 foo bar baz"));
           ASSERT(0 == manager.dispatchMessage("2 foo bar baz"));
           ASSERT(0 != manager.dispatchMessage("3 foo bar baz"));

           ASSERT(0 == manager.deregisterHandler("1"));
           ASSERT(0 == manager.deregisterHandler("2"));
           ASSERT(0 != manager.deregisterHandler("3"));

           ASSERT(0 != manager.dispatchMessage("1 foo bar baz"));
           ASSERT(0 != manager.dispatchMessage("2 foo bar baz"));
           ASSERT(0 != manager.dispatchMessage("B 1 foo bar baz"));
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Exercise the basic functionality of the 'balb::ControlManager'
        //   class.  We want to exercise each class accessor and manipulator.
        //
        // Plan:
        //   Invoke the primary accessors and manipulators of the tested class.
        //
        // Testing:
        //   Exercise basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "Breathing Test" << endl
                << "==============" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
          using namespace bdlf::PlaceHolders;

          balb::ControlManager manager(&ta);

          ASSERT(0 == manager.registerHandler("MyPrefix1", "", "", &noop));
          ASSERT(0 == manager.registerHandler("MyPrefix2", "", "", &noop));

          ASSERT(0 == manager.dispatchMessage("MyPrefix1"));
          ASSERT(0 == manager.dispatchMessage("MyPrefix2"));

          ASSERT(0 != manager.dispatchMessage("MyCategory MyCategoryPrefix"));

          int         counter = 0;
          Incrementer functor(&counter);

          ASSERT(0 == manager.registerHandler("MyPrefix3", "", "", functor));
          ASSERT(0 == manager.registerHandler("MyPrefix4", "", "", functor));

          ASSERT(0 == counter);
          ASSERT(0 == manager.dispatchMessage("MyPrefix3", bsl::cin));
          ASSERT(1 == counter);
          ASSERT(0 == manager.dispatchMessage("MyPrefix4", bsl::cin));
          ASSERT(2 == counter);
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      default: {
           cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
           testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
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
