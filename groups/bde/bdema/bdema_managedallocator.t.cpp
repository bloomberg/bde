// bdema_managedallocator.t.cpp      -*-C++-*-

#include <bdema_managedallocator.h>

#include <bsl_cstdlib.h>     // 'atoi()'
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goal of this 'bdema_ManagedAllocator' test suite is to verify that a
// concrete derived class compiles and links, and to provide a meaningful
// usage example illustrating the power of the added facility a managed
// allocator provides over an ordinary allocator.
//
// To aid testing, the concrete derived class 'TestAllocator' is implemented.
// This allocator records the associated constant for its most recently invoked
// method.  For the 'allocate' method, it also records the argument that the
// method was invoked with.  The allocator's destructor sets the global
// variable 'globalTestAllocatorDtorCalled' to indicate that the destructor has
// been called.  Note that this global variable is automatically reset to 0 in
// the allocator's constructor.
//
// To achieve the above goal, create a 'TestAllocator' and invoke its
// 'allocate', 'deallocate' and 'release' methods.  Verify that the
// corresponding constant for each method is recorded after each method's
// invocation.
//-----------------------------------------------------------------------------
// [1] virtual ~bdema_ManagedAllocator();
// [1] virtual void *allocate(size) = 0;
// [1] virtual void deallocate(address) = 0;
// [1] virtual void release() = 0;
//-----------------------------------------------------------------------------
// [1] PROTOCOL TEST - Ensure derived class works as expected.
// [2] ALLOCATING DERIVED CLASS TEST - Implement concrete subclass that
//       actually does allocate & provide memory (for use by usage example.)
// [3] USAGE TEST - Ensure main usage example compiles and works properly.
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                      CONCRETE DERIVED CLASS FOR TESTING
//-----------------------------------------------------------------------------

static int globalTestAllocatorDtorCalled;

class TestAllocator : public bdema_ManagedAllocator {
    // Test class used to verify protocol.

    int  d_fun; // Holds value indicating the most recent method invoked.
    int  d_arg; // Holds argument from 'allocate'.

  public:
    enum { ALLOCATE = 1, DEALLOCATE = 2, RELEASE = 3 };

    TestAllocator() : d_fun(0) { globalTestAllocatorDtorCalled = 0; }
    ~TestAllocator() { globalTestAllocatorDtorCalled = 1; }

    void *allocate(size_type s) { d_fun = ALLOCATE; d_arg = s; return this; }
    void deallocate(void *)     { d_fun = DEALLOCATE; }
    void release()              { d_fun = RELEASE;    }

    int fun() const { return d_fun; }
    // Return descriptive code for the function called.

    int arg() const { return d_arg; }
    // Return last argument value for 'allocate'.
};

//=============================================================================
//                    ALLOCATING DERIVED CLASS TEST
//                     (FOR USE BY USAGE EXAMPLE)
//-----------------------------------------------------------------------------

class LinkedListMA : public bdema_ManagedAllocator {

    class Node {
    public:
        Node *d_next;
        int   d_dataSize;
        // Data implicitly here.
    };

    enum { DATAOFFSET = sizeof(Node) };

    Node *d_freeList;
    Node *d_usedList;

    void changeLists(Node *moveItem, Node **from, Node **to);
    void removeAll(Node **list);

public:

    // CREATORS
    LinkedListMA();
    ~LinkedListMA();

    // MANIPULATORS
    void *allocate(size_type size);
    void deallocate(void *address);
    void release(void);

    friend ostream& operator<<(ostream& stream, const LinkedListMA& allctr);
};

// Private helper function 'changeLists' moves an item from one list to
// another; it is used by the class to move items from the free list to
// the used list or vice-versa.
// We expect the implementation never to incorrectly call this helper,
// i.e., if it says to move an item from a particular list, then the item
// is in fact on that list.

void LinkedListMA::changeLists(Node *moveItem, Node **from, Node **to) {

    // Quick sanity check.
    ASSERT(0 != from);
    ASSERT(0 != *from);

    Node *item    = *from;
    Node *preItem = *from;

    // Find the node holding this item.  Yes, O(n).
    // Special-case if its first element is in the list.
    if (*from == moveItem) {
        *from = (*from)->d_next;
    }
    else {
        while (item != 0) {
            preItem = item;
            item = item->d_next;
            if (item == moveItem) {
               break;
            }
        }

        // Quick sanity check.
        ASSERT(item != 0);

        // Remove this item from source list.
        preItem->d_next = item->d_next;
    }

    // We've removed it from its source.  Now add it to target.
    item->d_next = *to;
    *to = item;
}

LinkedListMA::LinkedListMA()
{
    d_freeList = d_usedList = 0;
}

LinkedListMA::~LinkedListMA()
{
    removeAll(&d_freeList);
    removeAll(&d_usedList);

    // For safety.
    d_freeList = d_usedList = 0;
}

void * LinkedListMA::allocate(size_type size)
{
    // Search the free list -- cheap "first-fit" strategy.
    Node *localNext = d_freeList;
    while(localNext != 0) {
        // Is this block big enough to satisfy the request?
        if(localNext->d_dataSize >= size) {
            changeLists(localNext, &d_freeList, &d_usedList);
            return((char *)localNext + DATAOFFSET);
        }
        localNext = localNext->d_next;
    }

    // If we got here, there were no blocks that could satisfy
    // the requirement.  Get truly new memory.
    char *newMem = new char[sizeof(Node) + size];
    ((Node *)newMem)->d_dataSize = size;
    ((Node *)newMem)->d_next = d_usedList;
    d_usedList = (Node *)newMem;

    return newMem + DATAOFFSET;
}

void LinkedListMA::deallocate(void *address)
{
    changeLists((Node *)((char *)address - DATAOFFSET),
                &d_usedList, &d_freeList);
}

void LinkedListMA::removeAll(Node **list)
{
    Node *forward = *list;
    Node *item    = *list;

    while ((item = forward) != 0) {
        forward = item->d_next;
        delete [] item;
    }
    *list = 0;
}

void LinkedListMA::release(void)
{
    removeAll(&d_usedList);

    // Another alternative is to thread the usedList onto the freeList
    // by maintaining a freeListHead, and then setting
    // freeListHead->d_next = usedList, and setting usedList = 0.
}

ostream& operator<<(ostream& stream, const LinkedListMA& allctr)
{
    stream << "The allocator holds allocated blocks:" << endl;
    LinkedListMA::Node *list = allctr.d_usedList;
    if (0 == list) {
        stream << "(none).";
    }
    else {
        while (list != 0) {
            stream << "Block Address: " << list             << endl;
            stream << "Block Size: "    << list->d_dataSize << endl;
            stream << "Block Contents: " << (char *)((char *)list +
                                            LinkedListMA::DATAOFFSET) << endl;
            list = list->d_next;
        }
    }
    stream << endl;

    stream << "The allocator also owns free blocks:" << endl;
    list = allctr.d_freeList;
    if (0 == list) {
        stream << "(none).";
    }
    else {
        while (list != 0) {
            stream << "Block Address: " << list             << endl;
            stream << "Block Size: "    << list->d_dataSize << endl;
            stream << "Which holds the useless: " << (char *)((char *)list +
                                          LinkedListMA::DATAOFFSET) << endl;
            list = list->d_next;
        }
    }
    stream << endl;

    return stream;
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// A logging subsystem allows applications to generate messages which are
// handled with some sophistication without requiring programming complexity
// on the part of the application.
//
// This usage example creates a simple implementation for a logger which
// can store messages, output single messages, or output a sequence of
// messages, depending on the severity level specified for a given message.
// A message passed in with severity RECORD is stored, pending further
// instructions.  A message passed in with severity PASS is both recorded as
// part of a historical archive, and immediately output.  A message passed
// in with severity TRIGGER causes immediate output of all currently-stored
// messages.
//
// Clients can directly instruct the logger to output everything it holds.
// They can also instruct the logger to delete all messages it currently
// holds, without outputting.
//
// To simplify this example, messages are simple ASCII strings.  To further
// simplify, we assume client maintains ownership of their strings so that
// we are compelled to copy in order to store.

class Logger {

    enum { InitialStackSize = 2 };

    LinkedListMA d_msgMem;    // Memory source for the actual strings.
    LinkedListMA d_stackMem;  // Memory source for pointers to strings -- we
                              //  need a separate one because we'll want to
                              //  remove strings all at once.
    char **d_stack;
    int d_stackBottom;
    int d_stackSize;
    int d_verbose;

    // Stack functions.  In "Real Life", it would be separate component.
    void reallocStack(void);
    void push(char *ptr);
    void reset(void) { d_stackBottom = 0; }

public:
    enum Severity { RECORD = 0, PASS = 1, TRIGGER = 2 };

    Logger(int verbose) {
               d_verbose = verbose; d_stackBottom = -1;
               d_stack = (char **)d_stackMem.allocate(
                                            InitialStackSize * sizeof(char *));
               d_stackSize = InitialStackSize * sizeof(char *); }
    ~Logger() { d_stackMem.release(); d_msgMem.release();
                // extra precaution
                d_stack = 0; d_stackBottom = -1; d_stackSize = 0; }
    void logMsg(enum Severity sev, const char *msg);
    void publish(int remove = 0);
    void removeAll(void);
};

void Logger::push(char *ptr)
{
    static int psize = sizeof(char *);
    if (d_stackSize/psize - 1 == d_stackBottom) {
        reallocStack();
    }
    d_stack[++d_stackBottom] = ptr;
}

void Logger::reallocStack(void)
{
    char *temp = (char *)d_stack;
    d_stack = (char **)d_stackMem.allocate(d_stackSize * 2);
    std::memcpy((char *)d_stack, temp, d_stackSize);
    d_stackSize *= 2;
    d_stackMem.deallocate(temp);
}

void Logger::logMsg(enum Severity sev, const char *msg)
{
    // In "Real Life", we would skip the next 3 statements for TRIGGER
    // messages; but we'll not do that here, to exercise allocator more.
    char *newMem = (char *)d_msgMem.allocate(std::strlen(msg) + 1);
    std::strcpy(newMem, msg);
    push(newMem);

    if (sev == PASS) {
        if (d_verbose) cout << msg << endl;
    }
    if (sev == TRIGGER) {
       publish(1);
    }
}

void Logger::publish(int remove)
{
    if (d_stackBottom == -1) {
        if (d_verbose) cout << "[there are no messages in the logger]" << endl;
        return;
    }
    for(int i = d_stackBottom; i >= 0; --i) {
        if (d_verbose) cout << d_stack[i] << endl;
    }
    if (remove) {
        removeAll();
    }
}

void Logger::removeAll(void)
{
    d_msgMem.release();
    d_stackBottom = -1;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATING DERIVED CLASS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        if (verbose) cout <<
            "\nTesting the Logger, a client of Linked List Managed Allocator."
            << endl;

        Logger myLogger(veryVerbose);

        if (veryVerbose) {
            cout << "\tDisplaying empty logger." << endl;
            myLogger.publish();
            cout << "\n\tInserting a message of RECORD severity." << endl;
            cout << "\t(Should not display immediately.)"           << endl;
        }

        myLogger.logMsg(Logger::RECORD, "String number one, severity RECORD.");

        if (veryVerbose) {
            cout << "\n\tInserting a message of PASS severity." << endl;
            cout << "\t(Should see its output immediately, by itself.)"
                 << endl;
        }

        myLogger.logMsg(Logger::PASS, "String number two, severity PASS.");

        if (veryVerbose) {
            cout << "\n\tInserting a message of TRIGGER severity.\n"
                    "\t(This will force printing and removal of all msgs.)\n"
                    "\tThis particular insertion will also force a memory "
                    "reallocation\n\tand then the 'Trigger' level forces "
                    "immediate deallocation.\n";
        }
        myLogger.logMsg(Logger::TRIGGER,
                        "String number three, severity TRIGGER.");

        if (veryVerbose) {
            cout << "\n\tNow log two new ones, after having deallocated msg "
                    "memory.\n\tThey are messages of RECORD severity.\n"
                    "\t(Should not display immediately.)\n";
        }

        myLogger.logMsg(Logger::RECORD, "Hello. (RECORD)");
        myLogger.logMsg(Logger::RECORD, "How are you? (RECORD)");

        if (veryVerbose) {
            cout << "\n\tInserting a message of PASS severity." << endl;
            cout << "\t(Should see its output immediately, by itself.)"
                 << endl;
        }
        myLogger.logMsg(Logger::PASS, "Still here? (PASS)");

        if (veryVerbose) {
            cout << "\n\tInserting a message of TRIGGER severity.\n"
                    "\t(This will force printing and removal of all msgs.)\n";
        }
        myLogger.logMsg(Logger::TRIGGER,
                        "Despite these boring messages? (TRIGGER)");
      }
      break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ...
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        const char *sampleStrings[] = {
            "Hello", "How are you?", "Es geht mir gut",
            "Porque tenemos que trabajar?", "Donde esta el gatito?",
            "You get a shiver in the dark/it's raining in the "
            "park/but meantime"
        };

        const int NUMBER_OF_STRINGS =
                                  sizeof sampleStrings / sizeof *sampleStrings;

        if (verbose) cout << endl << "DERIVED ALLOCATING CLASS TEST" << endl
                                  << "=============================" << endl;

        if (verbose) cout << "\nTesting the Linked List Managed Allocator.\n";

        // Create the allocator and check that it is empty, with
        // proper-looking infrastructure.
        LinkedListMA mommy;
        if (verbose) {
            cout << "\nOUTPUTTING AN EMPTY MANAGED ALLOCATOR AS TEXT:" << endl
                 << mommy;
        }

        const int NUMBER_OF_ALLOCATIONS = 15; // magic number, change at will
        char *allocatedAddress[NUMBER_OF_ALLOCATIONS];

        if (verbose) {
            cout << "\nUSING ALLOCATOR TO OBTAIN SPACE FOR AND ADD "
                 << NUMBER_OF_ALLOCATIONS << " STRINGS RANDOMLY:";
        }

        // Do NUMBER_OF_ALLOCATIONS allocations, and check that allocator
        // holds everything properly.
        for (int i = 0; i < NUMBER_OF_ALLOCATIONS; ++i) {
            int nextStrIdx;
            char *newMem = (char *)mommy.allocate(
                           std::strlen(sampleStrings[
                           (nextStrIdx = rand() % NUMBER_OF_STRINGS)]) + 1);
            std::strcpy(newMem, sampleStrings[nextStrIdx]);
            allocatedAddress[i] = newMem;
            if (verbose) { cout << "\nAdding line: " << newMem; }
            if (veryVerbose) {
                cout << "\n>>>>>>>>>>>>>>>>>>>\n" << mommy
                     <<   ">>>>>>>>>>>>>>>>>>>\n";
            }
        }
        if (verbose) {
            cout << "\n\nOUTPUTTING ALLOCATOR AFTER "
                 << NUMBER_OF_ALLOCATIONS << " ALLOCATIONS:" << endl << mommy;
        }

        // Now deallocate half the items, and recheck infrastructure.
        if (verbose) cout << "\nWILL NOW DELETE ROUGHLY HALF THE ITEMS:\n";
        int upperLimit = NUMBER_OF_ALLOCATIONS;
        for (int i = 0; i < NUMBER_OF_ALLOCATIONS / 2; ++i) {
             upperLimit -= 1;
             int NODEindex = rand() % upperLimit;
             if (verbose) {
                 cout << "Deleting block at address "
                      << (void *)((char *)(allocatedAddress[NODEindex]) - 8)
                      << endl;
             }
             mommy.deallocate(allocatedAddress[NODEindex]);
             allocatedAddress[NODEindex] = allocatedAddress[upperLimit];

             if (veryVerbose) {
                cout << "\n>>>>>>>>>>>>>>>>>>>\n" << mommy
                     <<   ">>>>>>>>>>>>>>>>>>>\n";
             }
        }

        if (verbose) {
            cout << "\nOUTPUTTING ALLOCATOR AFTER DEALLOCATING ROUGHLY HALF:\n"
                 << mommy;
        }

        // Now allocate again, using sizes small enough to reclaim
        // already-present blocks, and sizes large enough to need
        // creation of new blocks.
        const char *smallStrings[] = {
            "Rhiannon", "rings like a bell", "through the night"
        };

        const char *largeStrings[] = {
            "Now here you go again, you say you want your freedom/Well who "
            "am I to keep you down?/It's only right that you should play "
            "the way you feel it/But listen carefully to the sound",
            "She's always dancing down the street/With her suede blue "
            "eyes/And every new boy that she meets/Doesn't know the real "
            "surprise"
        };

        if (verbose) {
            cout << "WILL NOW ALLOCATE NEW SMALL AND LARGE STRINGS\n";
        }

        char *newMem;
        newMem = (char *)mommy.allocate(strlen(smallStrings[0]) + 1);
        std::strcpy(newMem, smallStrings[0]);
        newMem = (char *)mommy.allocate(strlen(smallStrings[1]) + 1);
        std::strcpy(newMem, smallStrings[1]);
        newMem = (char *)mommy.allocate(strlen(smallStrings[2]) + 1);
        std::strcpy(newMem, smallStrings[2]);

        newMem = (char *)mommy.allocate(strlen(largeStrings[0]) + 1);
        std::strcpy(newMem, largeStrings[0]);
        newMem = (char *)mommy.allocate(strlen(largeStrings[1]) + 1);
        std::strcpy(newMem, largeStrings[1]);

        if (verbose) {
            cout << "\nOUTPUTTING ALLOCATOR AFTER INSERTING NEW SMALL & LARGE:"
                 << endl << mommy;
        }

        // Finally, test releasing all memory & checking that an
        // allocate after that release works.
        mommy.release();
        if (verbose) {
            cout << "\nAFTER RELEASING ALL MEMORY, ALLOCATOR HAS:\n" << mommy;
        }
      }
      break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //
        // Concerns:
        //   Our primary concern is that a class derived from
        //   'bdema_ManagedAllocator' works as expected.
        //
        // Plan:
        //   Create a 'TestAllocator' and invoke its 'allocate', 'deallocate'
        //   and 'release' methods.  Verify that the corresponding constant
        //   associated with each method is recorded after each method
        //   invocation.  Also verify that after the allocator leaves its
        //   scope, the global variable 'globalTestAllocatorDtorCalled' is set
        //   to non-zero.
        //
        // Testing:
        //   virtual ~bdema_ManagedAllocator();
        //   virtual void *allocate(size) = 0;
        //   virtual void deallocate(address) = 0;
        //   virtual void release() = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting 'allocate', 'deallocate', 'release'"
                          << " and destructor." << endl;
        {
            TestAllocator myA;
            ASSERT(0 == globalTestAllocatorDtorCalled);

            bdema_ManagedAllocator& a = myA;
            ASSERT(0 == myA.fun());

            ASSERT(&myA == a.allocate(100));
            ASSERT(TestAllocator::ALLOCATE   == myA.fun());

            a.deallocate(&myA);
            ASSERT(TestAllocator::DEALLOCATE == myA.fun());

            a.release();
            ASSERT(TestAllocator::RELEASE    == myA.fun());
        }
        ASSERT(1 == globalTestAllocatorDtorCalled);

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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
