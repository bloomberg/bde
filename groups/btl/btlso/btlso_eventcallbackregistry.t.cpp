// btlso_eventcallbackregistry.t.cpp

#include <btlso_eventcallbackregistry.h>
#include <btlso_socketimputil.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslmt_threadutil.h>
#include <bsltf_alloctesttype.h>

#include <bdlf_bind.h>
#include <bsl_iostream.h>
#include <bsl_cstring.h>

using namespace BloombergLP;
using namespace btlso;
using namespace bsl;
using namespace bdlf::PlaceHolders;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
// The salient feature that EventCallbackRegistry implements, beyond those
// provided by its underlying unordered_map, is deferred removal of callbacks
// when that removal takes places in the scope of a callback.  The usage
// example will test the correctness of this scenario; in addition, we need to
// construct a test callback that holds complex arguments bound by const
// reference so that we can validate that callbacks are not prematurely
// destroyed (white-box testing).
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] EventCallbackRegistry(bslma::Allocator *ba = 0);
//
// MANIPULATORS
// [ 2] uint32_t registerCallback(const Event&, const EventManager::Callback&);
// [ 2] uint32_t remove(const Event& event);
// [ 2] void     removeAll();
// [ 3] int      removeSocket(const SocketHandle::Handle& socket);
//
// ACCESSORS
// [ 2] bool     contains(const Event& event) const;
// [ 3] uint32_t getRegisteredEventMask(const SocketHandle::Handle&) const;
// [ 4] int      invoke(const Event& event) const;
// [ 3] int      numSockets() const;
// [ 3] int      numCallbacks() const;
// [ 4] void     visitEvents(VISITOR* visitor) const;
// [ 3] void     visitSockets(VISITOR* visitor) const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE

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

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

// ============================================================================
//                  HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// Assign 'true' to the specified 'flag'.
void setFlag(bool *flag) {
    *flag = true;
}

int expectedArgument = 0;

void checkArgument(const bsltf::AllocTestType& arg) {
    ASSERTV(arg.data(), expectedArgument == arg.data());
}

void storeSocket(bsl::vector<SocketHandle::Handle> *sockets,
                 const SocketHandle::Handle&        socket) {
    sockets->push_back(socket);
}

void noOp() {
}

struct CountedEventVisitor {
    // A visitor suitable for visitEvents() that counts how many times it's
    // invoked.
    int d_count;

    CountedEventVisitor() : d_count(0) {}

    void operator()(const Event&) {
        d_count++;
    }
};

class NonzeroScopedInt {
    // Objects of this type hold a pointer to a nonzero int value until they
    // are destroyed, at which point the value is set to 0.
    //
    // To ensure these semantics work properly, objects of this type are
    // only available by shared pointer and cannot be copied.

    int *d_value_p;

    NonzeroScopedInt(int *value) {
        BSLS_ASSERT(0 != value);
        BSLS_ASSERT(0 != *value);

        d_value_p = value;
    }

    // NOT IMPLEMENTED
    NonzeroScopedInt(const NonzeroScopedInt&);
    NonzeroScopedInt& operator=(const NonzeroScopedInt&);

  public:

    static bsl::shared_ptr<NonzeroScopedInt>
    makeScopedInt(int* value) {
        return bsl::shared_ptr<NonzeroScopedInt>(new NonzeroScopedInt(value));
    }

    ~NonzeroScopedInt() {
        *d_value_p = 0;
    }

    int* value() const {
        return d_value_p;
    }
};

enum RemoveType {
    k_REMOVE_EVENT = 1,
    k_REMOVE_EVENT_HANDLE,
    k_REMOVE_ALL_EVENTS,
    k_REPLACE_EVENT
};

void selfRemovingCallback(EventCallbackRegistry             *mX,
                          const Event&                       event,
                          bsl::shared_ptr<NonzeroScopedInt>  action)
    // Based on the value of the specified 'action', remove the specified
    // 'event' from the specified 'mX' registry; then check that 'action' still
    // references a nonzero value (i.e., has not been destroyed).
    //
    // The value in 'action' is interpreted as one of the RemoveType constants:
    // k_REMOVE_EVENT: remove the specified 'event'
    // k_REMOVE_EVENT_HANDLE: remove 'event.handle()'
    // k_REMOVE_ALL_EVENTS: remove all events
    // k_REPLACE_EVENT: replace the specified 'event' (with a no-op callback)
{
    int *valuePtr = action->value();
    // Relinquish ownership. The bound functor should retain a reference until
    // after the end of this function.
    action.reset();

    switch (*valuePtr) {
    case k_REMOVE_EVENT:
        mX->remove(event);
        break;
    case k_REMOVE_EVENT_HANDLE:
        mX->removeSocket(event.handle());
        break;
    case k_REMOVE_ALL_EVENTS:
        mX->removeAll();
        break;
    case k_REPLACE_EVENT: {
        uint32_t mask = mX->registerCallback(event, &noOp);
        ASSERT(0 == mask);
    } break;
    default:
        ASSERT(!"UNREACHABLE");
    }

    // If the bound functor was destroyed, 'value' will be 0
    ASSERT(0 != *valuePtr);
}

int testCallbackDeregistration(EventCallbackRegistry *mX,
                               int                    numSockets,
                               int                    numEvents,
                               RemoveType             action)
    // Register the specified 'numEvents' events for the specified 'numSockets'
    // sockets on the specified 'mX' eventManager; then remove some based on
    // the specified 'action' (see 'selfRemovingCallback' for definition).
    // Return the total number of events expected to remain in 'mX'.
{
    Event event((SocketHandle::Handle)999, EventType::e_ACCEPT);

    int actionAsInt = (int)action;

    // We use this value for all callbacks except the final one registered,
    // which is the one we're going to invoke.  The other callbacks need
    // a valid int address *other than* that of 'actionAsInt', but will not be
    // invoked (if they are, they'll ASSERT on this dummy value).
    int dummyAction = -1;

    for (int i = 1; i <= numSockets; ++i) {
        event.setHandle((SocketHandle::Handle)i);

        for (int j = 0; j < numEvents; ++j) {
            event.setType(0 == j ? EventType::e_READ : EventType::e_WRITE);

            int* actionPtr = i == numSockets && j == numEvents - 1
                ? &actionAsInt
                : &dummyAction;

            mX->registerCallback(
                event,
                bdlf::BindUtil::bind(
                    &selfRemovingCallback,
                    mX,
                    event,
                    NonzeroScopedInt::makeScopedInt(actionPtr)));
        }
    }

    int rc = mX->invoke(event);
    ASSERT(0 == rc);

    // By this point, selfRemovingCallback will have executed and cleared
    // 'action'.
    ASSERT(0 == actionAsInt);

    switch (action) {
    case k_REMOVE_EVENT:
        return (numSockets * numEvents) - 1;
    case k_REMOVE_EVENT_HANDLE:
        return (numSockets - 1) * numEvents;
    case k_REMOVE_ALL_EVENTS:
        return 0;
    case k_REPLACE_EVENT:
        return numSockets * numEvents;
    default:
        ASSERT(!"Unreachable");
        return -1; // keep compiler happy
    }
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
// 'EventCallbackRegistry' is intended to simplify the implementation of socket
// event manager components by handling bookkeeping.  Those components, in
// turn, will provide callback-based interfaces to their callers.  In this
// example we use 'EventCallbackRegistry' to manage a callback for a 'READ'
// event that deregisters itself after consuming a certain number of bytes.
//
// First, we define the callback to be invoked.  Note that details of error
// handling are elided from this example.
//..
void readBytes(const SocketHandle::Handle&  socket,
               int                          bytesToRead,
               char                        *buffer,
               int                         *bytesRead,
               EventCallbackRegistry       *registry) {
    // Read up to 'bytesToRead' bytes from the specified 'socket' into the
    // specified 'buffer', updating the specified 'bytesRead' counter to
    // store the total number of bytes.  Once 'bytesToRead' bytes have
    // been read into 'buffer', deregister the 'READ' callback from the
    // specified 'registry' for 'socket'.

    int maxRead = bytesToRead - *bytesRead;
    int numRead = SocketImpUtil::read(buffer + *bytesRead, socket,
                                      maxRead, 0);
    *bytesRead += numRead;
    if (*bytesRead == bytesToRead) {
        registry->remove(Event(socket, EventType::e_READ));
    }

    // At this point, we can confirm that the registry reports that there
    // is no callback registered, though in fact the callback object
    // remains in scope.

    ASSERT(!registry->contains(Event(socket, EventType::e_READ)));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    ASSERT(0 == btlso::SocketImpUtil::startup());

    switch (test) { case 0:  // Zero is always the leading case.
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //:
        //: 2 Implement functionality elided from the usage example involving
        //:   socket reads and writes. (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        // This section is elided from the usage example. We create a socket
        // pair; the read callback will be registered on one side of it.
        SocketHandle::Handle pair[2];
        {
            int rc = SocketImpUtil::socketPair<IPv4Address>(
                                               pair,
                                               SocketImpUtil::k_SOCKET_STREAM);
            ASSERT(0 == rc);
        }
        SocketHandle::Handle socket = pair[0];

        static const int NUM_BYTES = 1;
//..
// Next, we create an event callback registry and register this callback for
// a socket.  Note that the details of creating 'socket' (an object of type
// SocketHandle::Handle) are elided from this example.
//..
        EventCallbackRegistry registry;
        char data[NUM_BYTES];
        int numRead = 0;

        registry.registerCallback(Event(socket, EventType::e_READ),
                                  bdlf::BindUtil::bind(&readBytes, socket,
                                                       (int)NUM_BYTES, &data[0],
                                                       &numRead, &registry));

        // This section is also elided from the usage example. We write to
        // the other side of the socket pair (and, on some platforms, sleep
        // briefly).
        bsl::memset(data, 'A', NUM_BYTES);
        int len = SocketImpUtil::write(pair[1], data, NUM_BYTES);
        ASSERT(NUM_BYTES == len);
#if defined(BSLS_PLATFORM_OS_HPUX) || defined(BSLS_PLATFORM_OS_DARWIN)
        enum { SLEEP_MS = 4 };
        bslmt::ThreadUtil::microSleep(SLEEP_MS * 1000);
#endif
//..
// Finally, we write data to the socket, the details of which are elided from
// this example; and we invoke the read callback via the registry.  The
// bound functor object will be destroyed only after the callback completes
// (thus ensuring any bound arguments remain valid for the duration of the
// callback).
//..
        int rc = registry.invoke(Event(socket, EventType::e_READ));
        ASSERT(0 == rc);
        ASSERT(NUM_BYTES == numRead);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // INVOKING CALLBACKS
        //
        // Concerns:
        //: 1 Callbacks can deregister themselves using any of the 'remove'
        //:   methods without the callback objects being destroyed.
        //:
        //: 2 Callbacks can re-register themselves using 'registerCallback'
        //:   without the previous callback objects being destroyed.
        //:
        //: 3 Deregistered or replaced events are not visited by 'visitEvents'.
        //
        // Plan:
        //: 1 Using a loop-driven technique, register one or two callbacks for
        //:   one or two sockets.
        //:
        //: 2 For each configuration in P-1, invoke each of the 'remove'
        //:   methods in turn from the event callback, afterwards asserting the
        //:   validity of the bound callback arguments. (C-1)
        //:
        //: 3 Invoke registerCallback() from the event callback to replace the
        //:   running event, afterwards asserting the validity of the bound
        //:   callback arguments. (C-2)
        //:
        //: 4 Visit all remaining callbacks using a visitor that counts the
        //:   number of times it is invoked, and assert that the count
        //:   represents the number of valid callbacks. (C-3)
        //
        // Testing:
        //   int invoke(const Event& event) const;
        //   vod visitEvents(VISITOR* visitor) const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nInvoking Callbacks"
                          << "\n==================" << endl;

        bslma::TestAllocator ta("test", veryVeryVerbose);
        {
            EventCallbackRegistry mX(&ta);

            for (int numSockets = 1; numSockets <= 2; ++numSockets) {
                for (int numEvents = 1; numEvents <= 2; ++numEvents) {
                    for (int action = 1; action <= 4; ++action) {
                        // See 'selfRemovingCallback' for the meaning of
                        // 'action'
                        int numRemaining =
                            testCallbackDeregistration(&mX,
                                                       numSockets,
                                                       numEvents,
                                                       (RemoveType)action);

                        ASSERT(numRemaining == mX.numCallbacks());

                        // Also test that visitEvents() doesn't see any
                        // callbacks we've deregistered
                        CountedEventVisitor counter;
                        mX.visitEvents(&counter);
                        ASSERT(numRemaining == counter.d_count);

                        mX.removeAll();
                    }
                }
            }
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // SOCKETS
        //
        // Concerns:
        //: 1 numSockets() returns the number of sockets with events
        //
        //: 2 removeSocket() deregisters all events for a socket
        //
        //: 3 visitSockets() is invoked once for each socket with events
        //
        //: 4 getRegisteredEventMask() returns a bitmask of registered events
        //    for a given socket
        //
        // Plan:
        //: 1 Test the socket methods for an event manager with no events.
        //:   (C-1..4)
        //:
        //: 2 Using an arbitrary integer to represent a socket, register one
        //:   and then two events for the socket and test the socket methods
        //:   (C-1..4)
        //:
        //: 3 Using two arbitrary integers to represent different sockets,
        //:   test the various socket methods when one or two events are
        //:   registered for the sockets (C-1..4)
        //
        // NOTE: this test relies on the existing enum values of
        // EventType::Type and will need to be changed if the integer values
        // are reassigned.
        //
        // Testing:
        //   int  removeSocket(const SocketHandle::Handle& socket);
        //   int  numSockets() const;
        //   int  numCallbacks() const;
        //   void visitSockets(VISITOR* visitor) const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Sockets"
                          << "\n===============" << endl;

        // test invariants
        BSLS_ASSERT(2 == EventType::e_READ);
        BSLS_ASSERT(3 == EventType::e_WRITE);

        EventType::Type write = EventType::e_WRITE;
        EventType::Type read = EventType::e_READ;
        enum {
            EXPECTED_WRITE_MASK     =  8,
            EXPECTED_READWRITE_MASK = 12
        };

        EventCallbackRegistry mX;

        if (verbose) cout << "Sub-test 1: No registered sockets." << endl;
        {
            ASSERT(0 == mX.numSockets());
            SocketHandle::Handle handle = (SocketHandle::Handle)123;

            ASSERT(0 == mX.getRegisteredEventMask(handle));

            bsl::vector<SocketHandle::Handle> sockets;
            bsl::function<void(const SocketHandle::Handle&)> visitor =
                bdlf::BindUtil::bind(&storeSocket, &sockets, _1);

            mX.visitSockets(&visitor);
            ASSERT(sockets.empty());

            // Invoke removeSocket for a socket with no events registered
            // (should have no effect)
            int numRemoved = mX.removeSocket(handle);
            ASSERT(0 == numRemoved);
        }

        if (verbose) cout << "Sub-test 2: One registered socket." << endl;
        {
            SocketHandle::Handle handle = (SocketHandle::Handle)123;

            Event writeEvent(handle, write);
            Event readEvent(handle, read);

            if (veryVerbose) {
                cout << "\tOne registered event." << endl;
            }
            uint32_t mask = mX.registerCallback(writeEvent, &noOp);
            ASSERT(EXPECTED_WRITE_MASK == mask);
            ASSERT(mask == mX.getRegisteredEventMask(handle));
            ASSERT(1 == mX.numSockets());

            bsl::vector<SocketHandle::Handle> sockets;
            bsl::function<void(const SocketHandle::Handle&)> visitor =
                bdlf::BindUtil::bind(&storeSocket, &sockets, _1);

            mX.visitSockets(&visitor);
            ASSERT(1 == sockets.size());
            ASSERT(handle == sockets[0]);

            if (veryVerbose) {
                cout << "\tTwo registered events." << endl;
            }
            mask = mX.registerCallback(readEvent, &noOp);
            ASSERT(EXPECTED_READWRITE_MASK == mask);
            ASSERT(mask == mX.getRegisteredEventMask(handle));
            ASSERT(1 == mX.numSockets());

            sockets.clear();
            mX.visitSockets(&visitor);
            ASSERT(1 == sockets.size());
            ASSERT(handle == sockets[0]);

            int numRemoved = mX.removeSocket(handle);
            ASSERT(2 == numRemoved);
            ASSERT(0 == mX.numSockets());
            sockets.clear();
            mX.visitSockets(&visitor);
            ASSERT(0 == sockets.size());
            ASSERT(0 == mX.getRegisteredEventMask(handle));

            // Finally, put an event back for socket 123 to prepare for
            // the next test
            mX.registerCallback(writeEvent, &noOp);
        }

        if (verbose) cout << "Sub-test 3: Two registered sockets." << endl;
        {
            SocketHandle::Handle handle = (SocketHandle::Handle)456;

            Event writeEvent(handle, write);
            Event readEvent(handle, read);

            if (veryVerbose) {
                cout << "\tOne registered event." << endl;
            }
            uint32_t mask = mX.registerCallback(writeEvent, &noOp);
            ASSERT(EXPECTED_WRITE_MASK == mask);
            ASSERT(mask == mX.getRegisteredEventMask(handle));
            ASSERT(2 == mX.numSockets());

            bsl::vector<SocketHandle::Handle> sockets;
            bsl::function<void(const SocketHandle::Handle&)> visitor =
                bdlf::BindUtil::bind(&storeSocket, &sockets, _1);

            mX.visitSockets(&visitor);
            ASSERT(2 == sockets.size());

            // Can't rely on the ordering of this vector
            ASSERT(handle == sockets[0] || handle == sockets[1]);

            if (veryVerbose) {
                cout << "\tTwo registered events." << endl;
            }
            mask = mX.registerCallback(readEvent, &noOp);
            ASSERT(EXPECTED_READWRITE_MASK == mask);
            ASSERT(mask == mX.getRegisteredEventMask(handle));
            ASSERT(2 == mX.numSockets());

            sockets.clear();
            mX.visitSockets(&visitor);
            ASSERT(2 == sockets.size());
            ASSERT(handle == sockets[0] || handle == sockets[1]);

            int numRemoved = mX.removeSocket(handle);
            ASSERT(2 == numRemoved);
            ASSERT(1 == mX.numSockets());
            sockets.clear();
            mX.visitSockets(&visitor);
            ASSERT(1 == sockets.size());
            ASSERT(0 == mX.getRegisteredEventMask(handle));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // REGISTER/REMOVE
        //
        // Concerns:
        //: 1 registerCallback() stores the specified callback for (only) the
        //:   specified event, and contains() reports that callback is present.
        //:
        //: 2 After removeCallback(), the callback is no longer executed by
        //:   invoke() or reported by contains().
        //:
        //: 3 After removeAll(), no callback is executed for any event.
        //:
        //: 4 The specified allocator, and not the default allocator, is
        //:   used for all operations.
        //
        // Plan:
        //: 1 This is a simple direct test of the specified methods. Use two
        //:   arbitrary integers to represent "sockets" and register a write
        //:   event for each, asserting that the specific callback is invoked
        //:   as appropriate. (C-1..3)
        //:
        //: 2 Install a test allocator as the default allocator, supply a
        //:   different test allocator to the registry, and assert that the
        //:   supplied allocator is used rather than the default allocator (C4)
        //
        // Testing:
        //   EventCallbackRegistry(bslma::Allocator *ba = 0);
        //   uint32_t registerCallback(const Event&, const EM::Callback&);
        //   uint32_t remove(const Event& event);
        //   void     removeAll();
        //   bool     contains(const Event& event) const;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Register/Remove"
                          << "\n=====================" << endl;

        bslma::TestAllocator da("default", veryVeryVerbose);
        bslma::TestAllocator ta("test", veryVeryVerbose);
        {
            // Create the callbacks prior to setting the default allocator,
            // since there isn't a good way to get an ordinary (non-shared)
            // Bind object without using the default allocator.  The
            // temporaries used below will use the ordinary (non-test) default
            // allocator.
            bsltf::AllocTestType arg1(1, &ta);
            bsltf::AllocTestType arg2(2, &ta);
            EventManager::Callback callback1(
                                       bsl::allocator_arg_t(),
                                       &ta,
                                       bdlf::BindUtil::bind(&checkArgument,
                                                            arg1));
            EventManager::Callback callback2(
                                       bsl::allocator_arg_t(),
                                       &ta,
                                       bdlf::BindUtil::bind(&checkArgument,
                                                            arg2));

            // Nothing after this point should require the use of the default
            // allocator, so install a test allocator to assert that.
            bslma::DefaultAllocatorGuard guard(&da);
            EventCallbackRegistry mX(&ta);

            SocketHandle::Handle handle1 = (SocketHandle::Handle)123;
            SocketHandle::Handle handle2 = (SocketHandle::Handle)456;
            EventType::Type write = EventType::e_WRITE;

            Event event1(handle1, write);
            Event event2(handle2, write);

            mX.registerCallback(event1, callback1);
            mX.registerCallback(event2, callback2);

            expectedArgument = 2;
            int rc = mX.invoke(event2);
            ASSERT(0 == rc);
            ASSERT(mX.contains(event2));

            expectedArgument = 1;
            rc = mX.invoke(event1);
            ASSERT(0 == rc);
            ASSERT(mX.contains(event1));

            mX.remove(event1);
            rc = mX.invoke(event1);
            ASSERT(0 != rc);
            ASSERT(!mX.contains(event1));
            ASSERT(mX.contains(event2));

            mX.removeAll();
            rc = mX.invoke(event2);
            ASSERT(0 != rc);
            ASSERT(!mX.contains(event2));
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 <  ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
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
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBreathing Test"
                          << "\n==============" << endl;

        EventCallbackRegistry mX;

        SocketHandle::Handle handle = (SocketHandle::Handle)123;
        EventType::Type write = EventType::e_WRITE;

        bool flag = false;
        bool added = mX.registerCallback(
                  Event(handle, write), bdlf::BindUtil::bind(&setFlag, &flag));
        ASSERT(added);

        int rc = mX.invoke(Event(handle, write));
        ASSERT(0 == rc);
        ASSERT(flag);
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
// Copyright 2016 Bloomberg Finance L.P.
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
