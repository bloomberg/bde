// bslma_rawdeleterguard.t.cpp                                        -*-C++-*-

#include <bslma_rawdeleterguard.h>

#include <bslma_allocator.h>      // for testing only
#include <bslma_default.h>        // for testing only
#include <bslma_testallocator.h>  // for testing only

#include <cstdlib>     // atoi()
#include <deque>       // usage example
#include <iostream>
#include <string>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a guard object to ensure that, when the guard object goes
// out of scope, it both destroys its managed object and deallocates memory
// used by that object.  We achieve this goal by creating (using
// 'bslma::TestAllocator') a dynamically-allocated, customized "test" object
// that is initialized with the address of a global counter initialized to 0.
// We then guard the object with a 'bslma::RawDeleterGuard'.  As the guard goes
// out of scope, the object is destroyed, and that object's destructor
// increments the counter held by the (test) object.  After the guard is
// destroyed, we verify that the (global) counter of the (managed) object is
// correctly incremented to 1, and that all memory allocated by the test
// allocator has been returned.  We then repeat the above process for a
// 'my_Pool' class, which also has a 'deallocate' method, but is non-virtual
// (i.e., does not inherit its interface from 'bslma::Allocator').
//
//-----------------------------------------------------------------------------
// [3] bslma::RawDeleterGuard<TYPE, ALLOCATOR>(obj, allocator);
// [3] ~bslma::RawDeleterGuard<TYPE, ALLOCATOR>();
//-----------------------------------------------------------------------------
// [1] Breathing Test
// [2] Helper Class: 'my_Class'
// [2] Helper Class: 'my_Pool'
// [3] Concern: the (non-virtual) 'deallocate' method for pools is also invoked
// [4] Usage Example
//=============================================================================

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
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

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                          GLOBALS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

class my_Pool {
    // This class provides a 'deallocate' method, used to exercise the
    // contract promised by the destructor of the 'bslma::RawDeleterGuard'.
    // This object indicates that its 'deallocate' method is called by
    // incrementing the global counter (supplied at construction) that it
    // *holds*.

    // DATA
    int *d_counter_p;  // counter to be incremented when 'deallocate' is called

  public:
    // CREATORS
    my_Pool(int *counter) : d_counter_p(counter) {}
        // Create this object holding the specified (global) counter.

    // MANIPULATORS
    void deallocate(void *) { ++*d_counter_p; }
        // Increment this object's counter.
};

class my_Class {
    // This object indicates that its destructor is called by incrementing the
    // global counter (supplied at construction) that it holds.  Note that the
    // id is used by the usage example's 'my_Queue' helper class to verify
    // which element in the queue is popped.

    // DATA
    int *d_counter_p;  // (global) counter to be incremented at destruction
    int  d_id;         // id used to identify this object (for usage example)

  public:
    // CREATORS
    my_Class(int *counter, int id = 0) : d_counter_p(counter), d_id(id) {}
        // Create this object using the address of the specified 'counter' to
        // be held.  Optionally specify an 'id' used to distinguish this
        // instance from others.  If 'id' is not specified, it will default to
        // 0.

    ~my_Class() { ++*d_counter_p; }
        // Destroy this object and increment this object's (global) counter.

    // ACCESSORS
    int id() const { return d_id; }
        // Return the id of this object.
};

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

// This example shows how one might use a 'bslma::RawDeleterGuard' to guard a
// dynamically-allocated object, deleting that object automatically when the
// guard goes out of scope.
//
// Suppose we have a simple queue class that stores object values using an
// "out-of-place" representation (i.e., an array of dynamically-allocated
// object pointers):
//..
// myqueue.h
// ...

template <class TYPE>
class my_Queue {
    // This class is a container that uses an "out-of-place"
    // representation to manage objects of parameterized 'TYPE'.  Note
    // that this class is implemented with the native version of 'deque',
    // instead of the version provided in 'bslstl_Deque'.  This is so that
    // a circular dependency in the physical hierarchy will not be created.

    // DATA
    deque<TYPE *>        d_objects;      // objects stored in the queue
    bslma::Allocator    *d_allocator_p;  // allocator (held, not owned)

  public:
    // CREATORS
    my_Queue(bslma::Allocator *basicAllocator = 0);
        // Create a 'my_Queue' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~my_Queue();
        // Destroy this 'my_Queue' object and all elements currently stored.

    // MANIPULATORS
    void pushBack(const TYPE& object);
        // Push the value of the specified 'object' of parameterized 'TYPE'
        // onto the back of this queue.

    TYPE popFront();
        // Remove and return (by value) the object of parameterized 'TYPE'
        // that is currently at the front of this queue.

    // ...
};
//..
// Note that the 'popFront' method returns an object by value because (1) there
// may be no reasonable default object to pass in, (2) there may be no
// reasonable copy assignment semantics, or (3) it is simply more syntactically
// convenient (e.g., if, say, the queued objects are themselves pointers):
//..
// CREATORS
template <class TYPE>
inline
my_Queue<TYPE>::my_Queue(bslma::Allocator *basicAllocator)
: d_objects(0)  // !!!WARNING: Modified to simplify assertion for memory usage
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
my_Queue<TYPE>::~my_Queue()
{
    for (size_t i = 0; i < d_objects.size(); ++i) {
        d_allocator_p->deleteObjectRaw(d_objects[i]);
    }
}
//..
// Note that the 'pushBack' method should be implemented with a constructor
// proxy that determines whether 'TYPE' takes an allocator at construction (see
// bslalg_constructorproxy).  However, for the purpose of this example, the
// implementation is simplified by assuming 'TYPE' takes an allocator.
//..
// MANIPULATORS
template <class TYPE>
inline
void my_Queue<TYPE>::pushBack(const TYPE& object)
{
    TYPE *tmp = (TYPE *)new(*d_allocator_p) TYPE(object);
    d_objects.push_back(tmp);
}

template <class TYPE>
inline
TYPE my_Queue<TYPE>::popFront()
{
    TYPE *tmp = d_objects.front();
    d_objects.pop_front();

    //***********************************************************
    //* Note the use of the raw deleter guard on 'tmp' (below). *
    //***********************************************************

    bslma::RawDeleterGuard<TYPE, bslma::Allocator> guard(tmp, d_allocator_p);

    return *tmp;
}
//..
// The 'pushBack' method defined above stores a copy of the provided object.
// The 'popFront' method returns the leading object by value, and the
// 'bslma::RawDeleterGuard' is used to automatically delete the copy the queue
// manages when the guard goes out of scope (i.e., when the function returns).

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example and exercise the creators and manipulators
        //   of 'my_Queue' using a 'bslma::TestAllocator' to verify that memory
        //   is allocated and deallocated properly.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUsage Example Test"
                          << "\n==================" << endl;

        // See usage example section at top of this file.

        enum { INIT_SIZE = 5 };

        bslma::TestAllocator allocator(veryVeryVerbose);
        const bslma::TestAllocator *Z = &allocator;

        int counter = 0;
        my_Queue<my_Class> queue(&allocator);

        ASSERT(0 == Z->numBytesInUse());

        // Initialize the queue.
        for (int i = 0; i < INIT_SIZE; ++i) {
            queue.pushBack(my_Class(&counter, i));
        }

        // Verify that 5 objects were copied.
        ASSERT(INIT_SIZE            == counter);
        // Verify that memory is allocated using the test allocator.
        ASSERT(5 * sizeof(my_Class) == Z->numBytesInUse());

        for (int i = 0; i < INIT_SIZE; ++i) {
            const my_Class& obj = queue.popFront();

            // Verify that the object popped is what we pushed in.
            ASSERT(i == obj.id());

            // Verify that destructor for the copy inside the queue is
            // destroyed.  Need '2 * i' because the copy returned is also
            // destroyed after the 'const' reference goes out of scope.
            ASSERT(INIT_SIZE + (2 * i) + 1 == counter);

            // Verify that memory is deallocated properly.
            ASSERT((4 - i) * sizeof(my_Class) == Z->numBytesInUse());
        }

        ASSERT(3 * INIT_SIZE == counter);
        ASSERT(0             == Z->numBytesInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   1) The guard object properly deletes its managed object
        //   at destruction using the allocator supplied at construction.
        //   2) When an allocator (or pool) not inherited from
        //   'bslma::Allocator' is supplied to the 'bslma::RawDeleterGuard',
        //   the destructor of the managed object and 'deallocate' method of
        //   the allocator (or pool) supplied is still invoked.
        //
        // Plan:
        //   Create a 'my_Class' object, which increments a counter every time
        //   it is destructed, using a 'bslma::TestAllocator' which keeps track
        //   of how many bytes are allocated.  Next create a
        //   'bslma::RawDeleterGuard' object to guard the created 'my_Class'
        //   object.  When the guard object goes out of scope, verify that both
        //   the counter is incremented and memory is deallocated.  Repeat for
        //   'my_Pool', which does not inherit from 'bslma::Allocator'.
        //
        // Testing:
        //   bslma::RawDeleterGuard<TYPE, ALLOCATOR>(obj, allocator);
        //   ~bslma::RawDeleterGuard<TYPE, ALLOCATOR>();
        //   Concern: the (non-virtual) 'deallocate' method for pools is also
        //            invoked
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\nCTOR / DTOR TEST"
                                  << "\n================" << endl;

        if (verbose) cout << "\nTesting with bslma::TestAllocator" << endl;
        {
            const int NUM_TEST = 10;
            int counter        = 0;
            int i;

            bslma::TestAllocator allocator(veryVeryVerbose);
            const bslma::TestAllocator *Z = &allocator;

            for (i = 0; i < NUM_TEST; ++i) {

                if (veryVerbose) {
                    T_; P(counter);
                }

                ASSERT(i == counter);
                ASSERT(0 == Z->numBytesInUse());

                my_Class *pMC = (my_Class *)allocator.allocate(sizeof *pMC);
                new (pMC) my_Class(&counter);

                ASSERT(i           == counter);
                ASSERT(sizeof *pMC == Z->numBytesInUse());

                bslma::RawDeleterGuard<my_Class, bslma::Allocator> mX(
                                                                   pMC,
                                                                   &allocator);

                ASSERT(i           == counter);
                ASSERT(sizeof *pMC == Z->numBytesInUse());
            }
            ASSERT(i == counter);
            ASSERT(0 == Z->numBytesInUse());
        }

        if (verbose) cout << "\nTesting with my_Pool" << endl;
        {
            const int NUM_TEST = 10;
            int counter        = 0;
            int allocCounter   = 0;

            bslma::TestAllocator allocator(veryVeryVerbose);
            const bslma::TestAllocator *Z = &allocator;

            my_Pool myAlloc(&allocCounter);

            int i;
            my_Class *pMC = 0;
            for (i = 0; i < NUM_TEST; ++i) {

                if (veryVerbose) {
                    T_; P_(counter); P(allocCounter);
                }

                if (pMC) {
                    allocator.deallocate(pMC);
                }

                ASSERT(i == counter);
                ASSERT(i == allocCounter);
                ASSERT(0 == Z->numBytesInUse());

                pMC = (my_Class *)allocator.allocate(sizeof *pMC);
                new (pMC) my_Class(&counter);

                ASSERT(i           == counter);
                ASSERT(i           == allocCounter);
                ASSERT(sizeof *pMC == Z->numBytesInUse());

                bslma::RawDeleterGuard<my_Class, my_Pool> mX(pMC, &myAlloc);

                ASSERT(i           == counter);
                ASSERT(i           == allocCounter);
                ASSERT(sizeof *pMC == Z->numBytesInUse());
            }
            allocator.deallocate(pMC);

            ASSERT(i == counter);
            ASSERT(i == allocCounter);
            ASSERT(0 == Z->numBytesInUse());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   1) The helper class 'my_Class' properly increments its counter at
        //      destruction.
        //   2) The 'id' method of 'my_Class' returns the same id provided at
        //      construction of the 'my_Class' object.
        //   3) The helper class 'my_Pool' properly increments its counter when
        //      its 'deallocate' method is called.
        //
        // Plan:
        //   Create 'my_Class' objects and assign to each object a counter
        //   variable initialized to 0, and an id.  Verify that the id returned
        //   by the 'id' method is the expected id.  Also verify that the
        //   counter is incremented after each 'my_Class' object is destroyed.
        //
        //   Next create 'my_Pool' objects and assign to each object a counter
        //   variable initialized to 0.  Invoke the 'deallocate' method and
        //   verify that the counter is incremented.
        //
        // Testing:
        //   Helper Class: 'my_Class'
        //   Helper Class: 'my_Pool'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\nHELPER CLASS TEST"
                                  << "\n=================" << endl;

        if (verbose) cout << "\nTesting 'my_Class'" << endl;

        if (verbose) cout << "\tTesting default ctor, dtor, and id()" << endl;
        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Class mx(&counter, i);
                const my_Class& X = mx;
                ASSERT(i == X.id());
            }
            ASSERT(NUM_TEST == counter);
        }

        if (verbose) cout << "\nTesting 'my_Pool'" << endl;

        if (verbose) cout << "\tTesting default ctor and 'deallocate'"
                          << endl;

        {
            int counter = 0;
            const int NUM_TEST = 5;
            for (int i = 0; i < NUM_TEST; ++i) {
                LOOP_ASSERT(i, counter == i);
                my_Pool mx(&counter);
                mx.deallocate(0);
                LOOP_ASSERT(i, counter == i + 1);
            }
            ASSERT(NUM_TEST == counter);
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) The 'bslma::RawDeleterGuard' can be constructed and destructed
        //      gracefully.
        //   1) The allocator's 'deallocate' method is invoked.
        //   2) The (managed) object's destructor is invoked.
        //
        // Plan:
        //   Allocate an 'std::string' with a 'bslma::TestAllocator' and guard
        //   it with 'bslma::RawDeleterGuard' to show that both the destructor
        //   and 'deallocate' is called (by verifying all memory is returned
        //   to the 'bslma::TestAllocator').
        //
        // Testing:
        //   Breathing Test
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "\nBREATHING TEST"
                                  << "\n==============" << endl;

        bslma::TestAllocator allocator(veryVeryVerbose);
        const bslma::TestAllocator *Z = &allocator;

        ASSERT(0 == Z->numBytesInUse());
        if (verbose) cout << "\tTesting with 'string' object" << endl;
        {
            string *s = (string *)new(allocator)string();
            ASSERT(0 < Z->numBytesInUse());
            *s = "Hello World!";
            bslma::RawDeleterGuard<string, bslma::Allocator> guard(s,
                                                                   &allocator);
        }
        ASSERT(0 == Z->numBytesInUse());

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
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
