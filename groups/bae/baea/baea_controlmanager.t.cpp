// baea_controlmanager.t.cpp   -*-C++-*-

#include <baea_controlmanager.h>
#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_thread.h>
#include <bdef_bind.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100)  ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); }}

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\t" << #L << ": " << L << "\t" \
                    << #M << ": " << M << "\n";                           \
               aSsErT(1, #X, __LINE__); }}

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number.
#define T_()  cout << '\t' << flush;          // Print tab w/o newline.

//=============================================================================
//              GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
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

    baea_ControlManager* d_manager_p; //held
    FunctionVector  d_functions;
    int             d_iterations;
    bcemt_Mutex     d_mutex;
    bcemt_Barrier   d_barrier;

  public:
    Dispatcher(int iterations, baea_ControlManager* manager_p)
    : d_manager_p(manager_p), d_iterations(iterations), d_barrier(2)
    {}

    void addFunction(const string& function)
    {
        bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
        d_functions.push_back(bsl::make_pair(function, 0));
    }

    int count(int i)
    {
        return d_functions[i].second;
    }

    bcemt_Barrier* barrier()
    {
        return &d_barrier;
    }

    void run()
    {
        d_barrier.wait();
        for (int i = 1; i < d_iterations; ++i) {
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
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
            bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
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

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? atoi(argv[1]) : 1;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            baea_ControlManager manager(&ta);
            manager.registerHandler("ECHO", "<text>",
                                    "Print specified text to terminal",
                                    &onEcho);

            manager.registerHandler("HELP", "",
                                    "Print documentation",
                                    bdef_BindUtil::bind(
               &baea_ControlManager::printUsageHelper, &manager, &bsl::cout,
               bsl::string(
                 "The following commands are accepted by the test driver:")));

            manager.dispatchMessage("ECHO repeat this text");
            manager.dispatchMessage("echo matching is case-insensitive");
            manager.dispatchMessage("HELP");
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: Dispatching and Thread Safety
        //
        // Concerns:
        //   The 'dispatchMessage' function may be run in high contention with
        //   another thread which is registering M-trap handlers.
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
            using namespace bdef_PlaceHolders;

            enum {NUM_ITERATIONS = 30000, NUM_FUNCTIONS=60};
            baea_ControlManager manager(&ta);
            Dispatcher dispatcher(NUM_ITERATIONS, &manager);

            int counts[NUM_FUNCTIONS+1];
            manager.registerHandler("FOOBAR", "", "",
                                    Incrementer(&counts[0]));
            dispatcher.addFunction("FOOBAR zippy");

            bcemt_Attribute detached;
            bcemt_ThreadUtil::Handle dummy;
            detached.setDetachedState(bcemt_Attribute::BCEMT_CREATE_DETACHED);
            ASSERT(0 == bcemt_ThreadUtil::create(&dummy,
                                                 detached,
                                                 bdef_BindUtil::bind(
                                                     &Dispatcher::run,
                                                     &dispatcher)));
            dispatcher.barrier()->wait();

            int currentTask = 1;
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
           using namespace bdef_PlaceHolders;

           baea_ControlManager manager(&ta);

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
        //   Exercise the basic functionality of the 'baea_ControlManager'
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

        bslma_TestAllocator ta(veryVeryVeryVerbose);
        {
          using namespace bdef_PlaceHolders;

          baea_ControlManager manager(&ta);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
