// balb_controlmanager.t.cpp                                          -*-C++-*-
#include <balb_controlmanager.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bslim_testutil.h>

#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>  // `bsl::ostringstream`
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
// The component under test provides a single class, a mechanism, that
// maintains a set of set of handler functors indexed by a string (key) value.
// Note that in practice, the key value corresponds of the first first
// whitespace separated field of a message (the "prefix") received from another
// process (see `balb_pipetaskmanager`).
//
// When one of `dispatchMessage` overloads is called, there is a lookup based
// on the first field (the "prefix") and, if found, the user-registered handler
// functor is invoked with the message passed as an argument.
//
// There is provision for the user to also register a "default" handler to be
// invoked for messages whose prefix has not been previously registered.
//
// The `printUsage` method generates a formatted help message based on
// information provided when handlers are registered.
//
// All manipulator and accessor methods must be thread safe.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ControlManager(bslma::Allocator *basicAllocator = 0);
// [ 2] ~ControlManager();
//
// MANIPULATORS
// [ 2] int registerHandler(prefix, args, desc, handler);
// [ 4] int registerUsageHandler(bsl::ostream& stream);
// [ 5] int registerDefaultHandler(const ControlHandler& hander);
// [ 2] int deregisterHandler(const bsl::string_view& prefix);
// [ 4] int deregisterUsageHandler();
// [ 5] int deregisterDefaultHandler();
//
// ACCESSOR
// [ 2] int dispatchMessage(const bsl::string_view& message) const;
// [ 2] int dispatchMessage(const bsl::string& , bsl::istream& ) const;
// [ 5] bool hasDefaultHandler() const;
// [ 5] void printUsage(bsl::ostream& , const bsl::string_view& ) const;
// [ 6] void printUsageHelper(bsl::ostream *, const bsl::string_view& ) const;
//
// [ 2] bslma::Allocator *allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] CONCERN: THREAD SAFETY
// [ 4] CONCERN: `printUsage` MENTIONS DEFAULT HANDLER IF SET
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
//            GLOBAL TYPES, CONSTANTS, AND VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

static int             verbose = 0;
static int         veryVerbose = 0;
static int     veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef balb::ControlManager Obj;

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
    int *d_value_p;

    Incrementer (int* value_p) : d_value_p(value_p) {
        *d_value_p = 0;
    }

    void operator()(const bsl::string&, bsl::istream&) {
        ++*d_value_p;
    }
};

struct DisplayArguments
{
    bsl::string *d_prefix;
    bsl::string *d_message;

    DisplayArguments(bsl::string *prefix, bsl::string *message) {
        d_prefix  = prefix;
        d_message = message;
    }

    void operator()(const bsl::string& prefix, bsl::istream& message) {
        *d_prefix = prefix;
        char c;
        while (message.get(c)) {
            d_message->append(1, c);
        }
    }
};

class Dispatcher
{
    typedef vector<pair<string, int> > FunctionVector;

    balb::ControlManager *d_manager_p; //held
    FunctionVector        d_functions;
    int                   d_iterations;
    bslmt::Mutex          d_mutex;
    bslmt::Barrier        d_barrier;

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
    int            test = (argc > 1) ? atoi(argv[1]) : 1;
                verbose = (argc > 2);
            veryVerbose = (argc > 3);
        veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        // 1. Usage example provided in the component header file must
        //    compile, link, and run on all platforms as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING USAGE EXAMPLE" << endl
                 << "=====================" << endl;
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
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT HANDLER
        //
        // Concerns:
        // 1. A newly constructed `ControlManager` has no default handler.
        //
        // 2. The `registerDefaultHandler` method can install a handler for
        //    unregistered messages and `deregisterDefaultHandler` can
        //    uninstall the current default handler.  The
        //    `registerDefaultHandler` and `deregisterDefaultHandler` return
        //    the expected values.
        //
        // 3. The `hasDefaultHandler` accessor indicates whether or not a
        //    default hander is currently installed.
        //
        // 4. The `printUsage` method mentions the default handler only if set.
        //
        // 5. All messages having unregistered prefixes are handled by the
        //    default handler, if any, and otherwise ignored.
        //
        // 6. The presence a default handler does not interfere with the
        //    dispatch of messages with registered prefixes.
        //
        // 7. The default handler is called with the correct arguments.
        //
        // Plan:
        // 1. Create a `ControlManager` apply a series of operations:
        //     1. Register a regular message handler.
        //     2. Install a default message handler.
        //     3. Uninstall the default message handler.
        //     4. Re-install the default message handler.
        //     5. Overwrite the default message handler with another handler.
        //
        // 2. At each stage of P-1 confirm the expected state.
        //     1. The return values of `registerDefaultHandler`.
        //     2. The return values of `deregisterDefaultHandler`.
        //     3. The return values of `hasDefaultHander`.
        //     4. The output of `printUsage` has (does not have) the expected
        //        mention of the default handler.
        //     5. Registered and unregistered messages are handled as expected.
        //        Note that both overloads of `dispatchMessage` are tested
        //        since each separately tests for the availability of a default
        //        handler.
        //     6. The test messages include various capitalizations; some have
        //        arguments, others not.
        //
        // Testing:
        //   int registerDefaultHandler(const ControlHandler& handler);
        //   int deregisterDefaultHandler();
        //   bool hasDefaultHandler() const;
        //   CONCERN: `printUsage` MENTIONS DEFAULT HANDLER IF SET
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "TESTING DEFAULT HANDLER" << endl
                << "=======================" << endl;
        }

        if (veryVerbose) cout << "Initialize variables for later use." << endl;

        bsl::ostringstream oss;
        int                rc;

        bdlsb::FixedMemInStreamBuf insbNo(0,       0);
        bdlsb::FixedMemInStreamBuf insbUc(" A1",   3);
        bdlsb::FixedMemInStreamBuf insbLc(" a1",   3);
        bdlsb::FixedMemInStreamBuf insbTo(" 1 3 ", 5);
        bdlsb::FixedMemInStreamBuf insbHi(" HI",   3);
        bsl::istream               argsNo(&insbNo);
        bsl::istream               argsUc(&insbUc);
        bsl::istream               argsLc(&insbLc);
        bsl::istream               argsTo(&insbTo);
        bsl::istream               argsHi(&insbHi);

        if (veryVerbose) cout << "Construct a `ControlManager`" << endl;

        Obj mX;  const Obj& X = mX;
        ASSERT(false == X.hasDefaultHandler());                         // TEST

        X.printUsage(oss,  "preamble");
        ASSERTV(oss.str(), "preamble\n" == oss.str());  oss.str("");

        if (veryVerbose) cout << "Register `P1` messages" << endl;

        int p1Count = -1;
        rc = mX.registerHandler("P1",
                                "A1",
                                "D1",
                                Incrementer(&p1Count));
        ASSERTV(rc,      0 == rc);
        ASSERTV(p1Count, 0 == p1Count);
        rc = mX.deregisterDefaultHandler();                             // TEST
        ASSERTV(rc, 0 == rc);
        ASSERT (false == X.hasDefaultHandler());                        // TEST

        X.printUsage(oss,  "preamble");
        ASSERTV(oss.str(), "preamble\n"
                            "    P1 A1\n"
                            "        D1\n" == oss.str());  oss.str("");

        if (veryVerbose) cout << "Set a default handler" << endl;

        int         miscCount = -2;
        Incrementer miscMsgs(&miscCount);
        rc = mX.registerDefaultHandler(miscMsgs);                       // TEST
        ASSERTV(rc,        0 == rc);
        ASSERTV(miscCount, 0 == miscCount);
        ASSERT (true  == X.hasDefaultHandler());                        // TEST

        X.printUsage(oss,  "preamble");
        ASSERTV(oss.str(),
                "preamble\n"
                "    P1 A1\n"
                "        D1\n"
                "    Has (default) handler for unregistered messages.\n"
                == oss.str());  oss.str("");

        if (veryVerbose) cout << "Unregistered messages are handled" << endl;

        // Check handling via single-argument `dispatchMessage`.
        X.dispatchMessage("P1 A1");         ASSERTV(  p1Count, 1 == p1Count  );

        X.dispatchMessage("Tom");           ASSERTV(miscCount, 1 == miscCount);
        X.dispatchMessage("Dick");          ASSERTV(miscCount, 2 == miscCount);
        X.dispatchMessage("Harry");         ASSERTV(miscCount, 3 == miscCount);

        X.dispatchMessage("p1 a1");         ASSERTV( p1Count,  2 == p1Count  );

        // Check handling via double-argument `dispatchMessage`.
        X.dispatchMessage("P1", argsUc);    ASSERTV( p1Count,  3 == p1Count  );

        X.dispatchMessage("Tom",   argsNo); ASSERTV(miscCount, 4 == miscCount);
        X.dispatchMessage("Dick",  argsNo); ASSERTV(miscCount, 5 == miscCount);
        X.dispatchMessage("Harry", argsNo); ASSERTV(miscCount, 6 == miscCount);

        X.dispatchMessage("p1", argsLc);    ASSERTV( p1Count,  4 == p1Count  );

        if (veryVerbose) cout << "Unset the default handler" << endl;

        rc = mX.deregisterDefaultHandler();                             // TEST
        ASSERTV(rc, 0 == rc);
        ASSERT (false == X.hasDefaultHandler());                        // TEST
        X.printUsage(oss,  "preamble");
        ASSERTV(oss.str(), "preamble\n"
                            "    P1 A1\n"
                            "        D1\n" == oss.str());  oss.str("");

        if (veryVerbose) cout << "Unregistered messages now no-ops" << endl;

        X.dispatchMessage("Tom");           ASSERTV(miscCount, 6 == miscCount);
        X.dispatchMessage("Dick");          ASSERTV(miscCount, 6 == miscCount);
        X.dispatchMessage("Harry");         ASSERTV(miscCount, 6 == miscCount);
        X.dispatchMessage("Tom",   argsNo); ASSERTV(miscCount, 6 == miscCount);
        X.dispatchMessage("Dick",  argsNo); ASSERTV(miscCount, 6 == miscCount);
        X.dispatchMessage("Harry", argsNo); ASSERTV(miscCount, 6 == miscCount);

        if (veryVerbose) cout << "Reinstall previous default handler" << endl;
        rc = mX.registerDefaultHandler(miscMsgs);                       // TEST
        ASSERTV(rc, 0 == rc);
        ASSERT (true  == X.hasDefaultHandler());                        // TEST

        if (veryVerbose) cout << "Unregistered messages handled again" << endl;

        X.dispatchMessage("tom");           ASSERTV(miscCount, 7 == miscCount);
        X.dispatchMessage("dick");          ASSERTV(miscCount, 8 == miscCount);
        X.dispatchMessage("harry");         ASSERTV(miscCount, 9 == miscCount);
        X.dispatchMessage("Tom",   argsNo); ASSERTV(miscCount,10 == miscCount);
        X.dispatchMessage("Dick",  argsNo); ASSERTV(miscCount,11 == miscCount);
        X.dispatchMessage("Harry", argsNo); ASSERTV(miscCount,12 == miscCount);

        if (veryVerbose) cout << "Overwrite existing default handler" << endl;

        int         miscCount2 = -2;
        Incrementer miscMsgs2(&miscCount2);
        rc = mX.registerDefaultHandler(miscMsgs2);                      // TEST

        ASSERTV(rc,         1 == rc);  // not 0
        ASSERTV(miscCount2, 0 == miscCount2);
        ASSERT (true  == X.hasDefaultHandler());                        // TEST

        X.printUsage(oss,  "preamble");
        ASSERTV(oss.str(),
                "preamble\n"
                "    P1 A1\n"
                "        D1\n"
                "    Has (default) handler for unregistered messages.\n"
                == oss.str());  oss.str("");

        if (veryVerbose) cout <<
             "Unregistered messages handled by latest default handler" << endl;
                                          ASSERTV(miscCount ,12 == miscCount );

        X.dispatchMessage("TOM 1 3"     );ASSERTV(miscCount2, 1 == miscCount2);
        X.dispatchMessage("TOM",  argsTo);ASSERTV(miscCount2, 2 == miscCount2);
        X.dispatchMessage("DICK hi"     );ASSERTV(miscCount2, 3 == miscCount2);
        X.dispatchMessage("DICK", argsHi);ASSERTV(miscCount2, 4 == miscCount2);
        X.dispatchMessage("HARRY"       );ASSERTV(miscCount2, 5 == miscCount2);
        X.dispatchMessage("HARRY",argsNo);ASSERTV(miscCount2, 6 == miscCount2);

                                          ASSERTV(miscCount ,12 == miscCount );

        if (veryVerbose) cout << "Default handler gets expected arguments"
                              << endl;
        bsl::string prefix;
        bsl::string message;
        DisplayArguments displayArguments(&prefix, &message);
        rc = mX.registerDefaultHandler(displayArguments);
        ASSERTV(rc, 1 == rc);
        ASSERT (true  == X.hasDefaultHandler());

        X.dispatchMessage("TOM 1 3 ");
            ASSERTV(prefix,  "TOM"   == prefix );  prefix .clear();
            ASSERTV(message, " 1 3 " == message);  message.clear();

        X.dispatchMessage("DICK hi" );
            ASSERTV(prefix,  "DICK"  == prefix );  prefix .clear();
            ASSERTV(message, " hi"   == message);  message.clear();

        X.dispatchMessage("HARRY"   );
            ASSERTV(prefix,  "HARRY" == prefix );  prefix .clear();
            ASSERTV(message, ""      == message);  message.clear();

        X.dispatchMessage("TOM",  argsTo);
            ASSERTV(prefix,  "TOM"   == prefix );  prefix .clear();
            ASSERTV(message, " 1 3 " == message);  message.clear();

        X.dispatchMessage("DICK", argsHi);
            ASSERTV(prefix,  "DICK"  == prefix );  prefix .clear();
            ASSERTV(message, " HI"   == message);  message.clear();

        X.dispatchMessage("HARRY",argsNo);
            ASSERTV(prefix,  "HARRY" == prefix );  prefix .clear();
            ASSERTV(message, ""      == message);  message.clear();
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `registerUsageHandler` METHOD
        //
        // Concerns:
        // 1. There is no default "HELP" message handler.
        // 2. The "HELP" message handler lists "HELP".
        // 3. The "HELP" message handler can be reinstalled without effect.
        // 4. The "HELP" message handler lists all registered messages.
        // 5. The "HELP" message prefix is case insensitive and any additional
        //    arguments are ignored.
        // 6. The "HELP" message handler can be deregistered by the
        //    `deregisterUsageHandlerMethod`.
        //
        // Plan:
        // 1. A series of ad hoc tests using a default constructed
        //    `ControlManager` object.
        //
        // 2. Check response before the HELP message handler is installed
        //    (`dispatchMesage` fails so no default handler), after
        //    installation, and after a second invocation of
        //    `registerUsageHandler`.
        //
        // 3. Incrementally register additional message handlers.  Confirm that
        //    the HELP message handler s shows all currently installed
        //    handlers in alphabetical order.
        //
        // 4. The HELP message handler shows the expected result irrespective
        //    of the case of the prefix and irrespective of additional text
        //    after the prefix.
        //
        // 5. Deregister the HELP message handler by calling the
        //    `deregisterUsageHandler` method.  The return value should be 0
        //    (successful removal) and the `ControlManger` object
        //    should not respond to a "HELP" message.  Deregister again and
        //    test for a non-zero value (failure) since there is no such
        //    handler to be removed (having been removed by the previous call).
        //
        // Testing:
        //   int registerUsageHandler(bsl::ostream& stream);
        //   int deregisterUsageHandler();
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "TESTING `registerUsageHandler` METHOD" << endl
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

        rc = manager.deregisterUsageHandler();
        ASSERTV(rc, 0 == rc);

        rc = manager.dispatchMessage(
                               "Help me Obi-Wan Kenobi. You're my only hope.");
        ASSERTV(rc,         0 != rc);
        ASSERTV(oss.str(), "" == oss.str());

        rc = manager.deregisterUsageHandler();
        ASSERTV(rc, 0 != rc);
      }  break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONCERN: DISPATCHING AND THREAD SAFETY
        //
        // Concerns:
        // 1 The `dispatchMessage` function may be run in high contention with
        //   another thread that is registering message handlers.
        //
        // Plan:
        // 1 Invoke the `dispatchMessage` function repeatedly in a background
        //   thread while registering handlers in the main thread.
        //
        // Testing:
        //   CONCERN: THREAD SAFETY
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "TESTING CONCERN: DISPATCHING AND THREAD SAFETY" << endl
                 << "==============================================" << endl;
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
        // 1. Prefix names must be unique.
        // 2. The manager safely returns non-zero values when attempting to
        //    dispatch messages which do not specify a known message or
        //    sub-task.
        // 3. Handlers may be deregistered.
        // 4. The manager safely returns non-zero values when attempting to
        //    dispatch a message for a prefix with no registered handler.
        // 5. The manager uses the intended allocator.
        //
        // Testing:
        //   int dispatchMessage(const bsl::string_view& message) const;
        //   int dispatchMessage(const bsl::string& , bsl::istream& ) const;
        //   int deregisterHandler(const bsl::string_view& prefix);
        //   int registerHandler(prefix, args, desc, handler);
        // --------------------------------------------------------------------

        if (verbose) {
           cout
          << "TESTING CONCERN: TASK CREATION AND HANDLER REGISTRATION" << endl
          << "=======================================================" << endl;
        }

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);

            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

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
                objPtr = new (fa) Obj(&sa);
              } break;
              default: {
                BSLS_ASSERT_OPT(0 == "Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            ASSERTV(CONFIG, &oa == X.allocator());

            using namespace bdlf::PlaceHolders;

            ASSERTV(CONFIG, 0 == mX.registerHandler("1", "", "", &noop));
            ASSERTV(CONFIG, 0 != mX.registerHandler("1", "", "", &noop));

            ASSERTV(CONFIG, 0 == mX.registerHandler("2", "", "", &noop));
            ASSERTV(CONFIG, 0 != mX.registerHandler("2", "", "", &noop));

            ASSERTV(CONFIG, 0 != mX.dispatchMessage("BOGUS MESSAGE"));

            ASSERTV(CONFIG, 0 == mX.dispatchMessage("1 foo bar baz"));
            ASSERTV(CONFIG, 0 == mX.dispatchMessage("2 foo bar baz"));
            ASSERTV(CONFIG, 0 != mX.dispatchMessage("3 foo bar baz"));

            bdlsb::FixedMemInStreamBuf  insFooBarBaz(" foo bar baz",  12);
            bsl::istream               argsFooBarBaz(&insFooBarBaz);

            ASSERTV(CONFIG, 0 == mX.dispatchMessage("1", argsFooBarBaz));
            ASSERTV(CONFIG, 0 == mX.dispatchMessage("2", argsFooBarBaz));
            ASSERTV(CONFIG, 0 != mX.dispatchMessage("3", argsFooBarBaz));

            ASSERTV(CONFIG, 0 == mX.deregisterHandler("1"));
            ASSERTV(CONFIG, 0 == mX.deregisterHandler("2"));
            ASSERTV(CONFIG, 0 != mX.deregisterHandler("3"));

            ASSERTV(CONFIG, 0 != mX.dispatchMessage("1 foo bar baz"));
            ASSERTV(CONFIG, 0 != mX.dispatchMessage("2 foo bar baz"));
            ASSERTV(CONFIG, 0 != mX.dispatchMessage("B 1 foo bar baz"));

            fa.deleteObject(objPtr);

            ASSERTV(CONFIG,  fa.numBlocksInUse(), 0 ==  fa.numBlocksInUse());
            ASSERTV(CONFIG,  oa.numBlocksInUse(), 0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
        }
      }  break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Invoke the primary accessors and manipulators of the tested
        //    class.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
           cout << "BREATHING TEST" << endl
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
