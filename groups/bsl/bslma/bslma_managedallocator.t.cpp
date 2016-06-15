// bslma_managedallocator.t.cpp                                       -*-C++-*-

#ifndef BDE_OPENSOURCE_PUBLICATION // DEPRECATED

#include <bslma_managedallocator.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>     // 'memcpy'

#include <iostream>

using namespace BloombergLP;
using std::endl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goal of this 'bslma::ManagedAllocator' test suite is to verify that a
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
// [1] virtual ~bslma::ManagedAllocator();
// [1] virtual void *allocate(size) = 0;
// [1] virtual void deallocate(address) = 0;
// [1] virtual void release() = 0;
//-----------------------------------------------------------------------------
// [1] PROTOCOL TEST - Ensure derived class works as expected.
// [2] ALLOCATING DERIVED CLASS TEST - Implement concrete subclass that
//       actually does allocate & provide memory (for use by usage example.)
// [3] USAGE TEST - Ensure main usage example compiles and works properly.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                      CONCRETE DERIVED CLASS FOR TESTING
//-----------------------------------------------------------------------------

static int globalTestAllocatorDtorCalled;

class TestAllocator : public bslma::ManagedAllocator {
    // Test class used to verify protocol.

    int  d_fun; // Holds value indicating the most recent method invoked.
    size_type d_arg; // Holds argument from 'allocate'.

  public:
    enum { ALLOCATE = 1, DEALLOCATE = 2, RELEASE = 3 };

    // CREATORS
    TestAllocator() : d_fun(0) { globalTestAllocatorDtorCalled = 0; }
    ~TestAllocator() { globalTestAllocatorDtorCalled = 1; }

    // MANIPULATORS
    void *allocate(size_type s) { d_fun = ALLOCATE; d_arg = s; return this; }
    void deallocate(void *)     { d_fun = DEALLOCATE; }
    void release()              { d_fun = RELEASE;    }

    // ACCESSORS
    size_type arg() const { return d_arg; }
    // Return last argument value for 'allocate'.

    int fun() const { return d_fun; }
    // Return descriptive code for the function called.
};

//=============================================================================
//                    ALLOCATING DERIVED CLASS TEST
//                     (FOR USE BY USAGE EXAMPLE)
//-----------------------------------------------------------------------------

class LinkedListMA : public bslma::ManagedAllocator {

    class Node {
      public:
        Node      *d_next;
        size_type  d_dataSize;
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

    friend std::ostream& operator<<(std::ostream& stream,
                                    const LinkedListMA& allctr);
};

// Private helper function 'changeLists' moves an item from one list to
// another; it is used by the class to move items from the free list to the
// used list or vice-versa.  We expect the implementation never to incorrectly
// call this helper, i.e., if it says to move an item from a particular list,
// then the item is in fact on that list.

void LinkedListMA::changeLists(Node *moveItem, Node **from, Node **to)
{
    // Quick sanity check.
    ASSERT(0 != from);
    ASSERT(0 != *from);

    Node *item    = *from;
    Node *preItem = *from;

    // Find the node holding this item.  Yes, it's, O(n).  Special-case if it
    // is the first element in the list.
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

    // We have removed it from its source.  Now add it to target.
    item->d_next = *to;
    *to = item;
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

// CREATORS
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

// MANIPULATORS
void * LinkedListMA::allocate(size_type size)
{
    // Search the free list -- cheap "first-fit" strategy.
    Node *localNext = d_freeList;
    while (localNext != 0) {
        // Is this block big enough to satisfy the request?
        if (localNext->d_dataSize >= size) {
            changeLists(localNext, &d_freeList, &d_usedList);
            return ((char *)localNext + DATAOFFSET);                  // RETURN
        }
        localNext = localNext->d_next;
    }

    // If we got here, there were no blocks that could satisfy the requirement.
    // Get truly new memory.
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

void LinkedListMA::release()
{
    removeAll(&d_usedList);

    // Another alternative is to thread the usedList onto the 'freeList' by
    // maintaining a 'freeListHead', and then setting
    // 'freeListHead->d_next = usedList', and setting 'usedList = 0'.
}

std::ostream& operator<<(std::ostream& stream, const LinkedListMA& allctr)
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
// handled with some sophistication without requiring programming complexity on
// the part of the application.
//
// This usage example creates a simple implementation for a logger which can
// store messages, output single messages, or output a sequence of messages,
// depending on the severity level specified for a given message.  A message
// passed in with severity RECORD is stored, pending further instructions.  A
// message passed in with severity PASS is both recorded as part of a
// historical archive, and immediately output.  A message passed in with
// severity TRIGGER causes immediate output of all currently-stored messages.
//
// Clients can directly instruct the logger to output everything it holds.
// They can also instruct the logger to delete all messages it currently holds,
// without outputting.
//
// To simplify this example, messages are simple ASCII strings.  To further
// simplify, we assume client maintains ownership of their strings so that we
// are compelled to copy in order to store.

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
    void push(char *ptr);
    void reallocStack(void);
    void reset(void) { d_stackBottom = 0; }

  public:
    enum Severity { RECORD = 0, PASS = 1, TRIGGER = 2 };

    explicit Logger(int verbose) {
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
    memcpy((char *)d_stack, temp, d_stackSize);
    d_stackSize *= 2;
    d_stackMem.deallocate(temp);
}

void Logger::logMsg(enum Severity sev, const char *msg)
{
    // In "Real Life", we would skip the next 3 statements for TRIGGER
    // messages; but we'll not do that here, to exercise allocator more.
    char *newMem = (char *)d_msgMem.allocate(strlen(msg) + 1);
    strcpy(newMem, msg);
    push(newMem);

    if (sev == PASS) {
        if (d_verbose) puts(msg);
    }
    if (sev == TRIGGER) {
       publish(1);
    }
}

void Logger::publish(int remove)
{
    if (d_stackBottom == -1) {
        if (d_verbose) puts("[there are no messages in the logger]");
        return;                                                       // RETURN
    }
    for (int i = d_stackBottom; i >= 0; --i) {
        if (d_verbose) std::cout << d_stack[i] << endl;
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
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;       // suppress unused variable warning
    (void)veryVeryVeryVerbose;   // suppress unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

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

        if (verbose) printf("\nUSAGE EXAMPLE TEST"
                            "\n==================\n");;

        if (verbose) puts(
           "\nTesting the Logger, a client of Linked List Managed Allocator.");

        Logger myLogger(veryVerbose);

        if (veryVerbose) {
            puts("\tDisplaying empty logger.");
            myLogger.publish();
            puts("\n\tInserting a message of RECORD severity."
                 "\n\t(Should not display immediately.)");
        }

        myLogger.logMsg(Logger::RECORD, "String number one, severity RECORD.");

        if (veryVerbose) {
            puts("\n\tInserting a message of PASS severity."
                 "\n\t(Should see its output immediately, by itself.)");
        }

        myLogger.logMsg(Logger::PASS, "String number two, severity PASS.");

        if (veryVerbose) {
            puts("\n\tInserting a message of TRIGGER severity."
                 "\n\t(This will force printing and removal of all msgs.)"
                 "\n\tThis particular insertion will also force a memory "
                     "reallocation"
                 "\n\tand then the 'Trigger' level forces immediate "
                     "deallocation.");
        }
        myLogger.logMsg(Logger::TRIGGER,
                        "String number three, severity TRIGGER.");

        if (veryVerbose) {
            puts("\n\tNow log two new ones, after having deallocated msg "
                     "memory."
                 "\n\tThey are messages of RECORD severity."
                 "\n\t(Should not display immediately.)");
        }

        myLogger.logMsg(Logger::RECORD, "Hello. (RECORD)");
        myLogger.logMsg(Logger::RECORD, "How are you? (RECORD)");

        if (veryVerbose) {
            puts("\n\tInserting a message of PASS severity."
                 "\n\t(Should see its output immediately, by itself.)");
        }
        myLogger.logMsg(Logger::PASS, "Still here? (PASS)");

        if (veryVerbose) {
            puts("\n\tInserting a message of TRIGGER severity."
                 "\n\t(This will force printing and removal of all msgs.)");
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

        if (verbose) printf("\nDERIVED ALLOCATING CLASS TEST\n"
                            "\n=============================\n");

        if (verbose) puts("\nTesting the Linked List Managed Allocator.");

        // Create the allocator and check that it is empty, with
        // proper-looking infrastructure.
        LinkedListMA mommy;
        if (verbose) {
            std::cout << "\nOUTPUTTING AN EMPTY MANAGED ALLOCATOR AS TEXT:"
                      << endl
                      << mommy;
        }

        const int NUMBER_OF_ALLOCATIONS = 15; // magic number, change at will
        char *allocatedAddress[NUMBER_OF_ALLOCATIONS];

        if (verbose) {
            std::cout << "\nUSING ALLOCATOR TO OBTAIN SPACE FOR AND ADD "
                 << NUMBER_OF_ALLOCATIONS << " STRINGS RANDOMLY:";
        }

        // Do NUMBER_OF_ALLOCATIONS allocations, and check that allocator
        // holds everything properly.
        for (int i = 0; i < NUMBER_OF_ALLOCATIONS; ++i) {
            int nextStrIdx;
            char *newMem = (char *)mommy.allocate(
                           strlen(sampleStrings[
                           (nextStrIdx = rand() % NUMBER_OF_STRINGS)]) + 1);
            strcpy(newMem, sampleStrings[nextStrIdx]);
            allocatedAddress[i] = newMem;
            if (verbose) { std::cout << "\nAdding line: " << newMem; }
            if (veryVerbose) {
                std::cout << "\n>>>>>>>>>>>>>>>>>>>\n" << mommy
                          <<   ">>>>>>>>>>>>>>>>>>>\n";
            }
        }
        if (verbose) {
            std::cout << "\n\nOUTPUTTING ALLOCATOR AFTER "
                 << NUMBER_OF_ALLOCATIONS << " ALLOCATIONS:" << endl << mommy;
        }

        // Now deallocate half the items, and recheck infrastructure.
        if (verbose) puts("\nWILL NOW DELETE ROUGHLY HALF THE ITEMS:");
        int upperLimit = NUMBER_OF_ALLOCATIONS;
        for (int i = 0; i < NUMBER_OF_ALLOCATIONS / 2; ++i) {
             upperLimit -= 1;
             int NODEindex = rand() % upperLimit;
             if (verbose) {
                 std::cout << "Deleting block at address "
                      << (void *)((char *)(allocatedAddress[NODEindex]) - 8)
                      << endl;
             }
             mommy.deallocate(allocatedAddress[NODEindex]);
             allocatedAddress[NODEindex] = allocatedAddress[upperLimit];

             if (veryVerbose) {
                 std::cout << "\n>>>>>>>>>>>>>>>>>>>\n" << mommy
                           <<   ">>>>>>>>>>>>>>>>>>>\n";
             }
        }

        if (verbose) {
            std::cout
                 << "\nOUTPUTTING ALLOCATOR AFTER DEALLOCATING ROUGHLY HALF:\n"
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
            puts("WILL NOW ALLOCATE NEW SMALL AND LARGE STRINGS");
        }

        char *newMem;
        newMem = (char *)mommy.allocate(strlen(smallStrings[0]) + 1);
        strcpy(newMem, smallStrings[0]);
        newMem = (char *)mommy.allocate(strlen(smallStrings[1]) + 1);
        strcpy(newMem, smallStrings[1]);
        newMem = (char *)mommy.allocate(strlen(smallStrings[2]) + 1);
        strcpy(newMem, smallStrings[2]);

        newMem = (char *)mommy.allocate(strlen(largeStrings[0]) + 1);
        strcpy(newMem, largeStrings[0]);
        newMem = (char *)mommy.allocate(strlen(largeStrings[1]) + 1);
        strcpy(newMem, largeStrings[1]);

        if (verbose) {
            std::cout
                 << "\nOUTPUTTING ALLOCATOR AFTER INSERTING NEW SMALL & LARGE:"
                 << endl << mommy;
        }

        // Finally, test releasing all memory & checking that an allocate after
        // that release works.
        mommy.release();
        if (verbose) {
            std::cout << "\nAFTER RELEASING ALL MEMORY, ALLOCATOR HAS:\n"
                      << mommy;
        }
      }
      break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //
        // Concerns:
        //   Our primary concern is that a class derived from
        //   'bslma::ManagedAllocator' works as expected.
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
        //   virtual ~bslma::ManagedAllocator();
        //   virtual void *allocate(size) = 0;
        //   virtual void deallocate(address) = 0;
        //   virtual void release() = 0;
        // --------------------------------------------------------------------

        if (verbose) printf("\nPROTOCOL TEST"
                            "\n=============\n");

        if (verbose) puts("\nTesting 'allocate', 'deallocate', 'release'"
                            " and destructor.");
        {
            TestAllocator myA;
            ASSERT(0 == globalTestAllocatorDtorCalled);

            bslma::ManagedAllocator& a = myA;
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
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

#else

int main(int argc, char *argv[])
{
    return -1;
}

#endif  // BDE_OPENSOURCE_PUBLICATION -- DEPRECATED

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
