// bcecs_roundrobin.t.cpp      -*-C++-*-
#include <bcecs_roundrobin.h>

#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>
#include <bslma_testallocator.h>
#include <bsls_alignment.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'bcecs_RoundRobin' is divided into following parts (apart from
// breathing test and usage example).
//   Testing primary manipulator 'add'.  Once we test it, we can easily
//   bring a round robin object into variety of white-box states (in order
//   to test other functions).  It is tested in [2].
//
//   Testing the rest of the manipulators ('remove', 'removeAll', 'next') and
//   accessors ('numObjects') () by using (already tested) 'add' (to bring
//   a round robin object into variety of white-box state).  It is tested
//   in [3], [4] and [5].
//
//   Verify allocation and deallocation of the contained objects.  It is
//   tested in [6].
//
//   Verify construction and destruction of the contained objects.  It is
//   tested in [7].
//
//   Concurrency test i.e., testing that round robin remains consistent
//   upon concurrent access to it.  It is tested in [8], [9] and [10].
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] bcecs_RoundRobin(bslma_Allocator *allocator=0);
// [ 1] ~bcecs_RoundRobin();
//
// MANIPULATORS
// [ 2] int  add(const TYPE& object);
// [ 3] int  remove(const TYPE& object);
// [ 4] void removeAll();
//
// ACCESSORS
// [ 5] int  numObjects() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] TESTING ALLOCATION AND DEALLOCATION
// [ 7] TESTING CONSTRUCTION AND DESTRUCTION
// [ 8] CONCURRENCY TEST FOR 'NEXT'
// [ 9] CONCURRENCY TEST FOR 'ADD', 'REMOVE' AND 'NUMOBJECTS'
// [10] CONCURRENCY TEST FOR 'ADD', 'REMOVEALL' AND 'NUMOBJECTS'
// [11] USAGE EXAMPLE
//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
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
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << #M << ": " <<  \
       M << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
//=============================================================================
//                    THREAD-SAFE OUTPUT AND ASSERT MACROS
//-----------------------------------------------------------------------------
static bcemt_Mutex printMutex;  // mutex to protect output macros
#define PT(X) { printMutex.lock(); P(X); printMutex.unlock(); }
#define PT_(X) { printMutex.lock(); P_(X); printMutex.unlock(); }

static bcemt_Mutex &assertMutex = printMutex; // mutex to protect assert macros

#define ASSERTT(X) {assertMutex.lock(); aSsErT(!(X), #X, __LINE__); \
                                          assertMutex.unlock();}

#define LOOP_ASSERTT(I,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP2_ASSERTT(I,J,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << endl; aSsErT(1, #X, __LINE__); } \
       assertMutex.unlock(); }

#define LOOP3_ASSERTT(I,J,K,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP4_ASSERTT(I,J,K,L,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << endl; aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

#define LOOP5_ASSERTT(I,J,K,L,M,X) { \
   if (!(X)) { assertMutex.lock(); cout << #I << ": " << I << "\t" \
       << #J << ": " << J << "\t" << #K << ": " << K << "\t" << #L \
       << ": " << L << "\t" << #M << ": " << M << endl; \
       aSsErT(1, #X, __LINE__); } assertMutex.unlock(); }

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
// static int veryVeryVerbose;  // not used
void executeInParallel(int numThreads, bcemt_ThreadUtil::ThreadFunction func)
   // Create the specified 'numThreads', each executing the specified 'func'.
   // Number each thread (sequentially from 0 to 'numThreads-1') by passing i
   // to i'th thread.  Finally join all the threads.
{
    bcemt_ThreadUtil::Handle *threads =
                               new bcemt_ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bcemt_ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}
//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================
//                         CASE 11 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase11 {
typedef int ServerID;
typedef int Query;
typedef int QueryResult;

Query getClientRequest()
    // Return a query requested from the user.  Return 0 when no more queries
    // are left.
{
    enum { MAX_QURIES = 1001 };
    static int i = MAX_QURIES;
    return --i;
}

QueryResult executeQuery(ServerID server, Query query)
    // Send the specified 'query' to the specified 'server' and return the
    // query result obtained from the server.
{
    return QueryResult(query);
}

void display(QueryResult queryResult)
    // Display the specified 'queryResult'.
{
    if(veryVerbose) P(queryResult);
}

}  // close namespace TestCase11
//=============================================================================
//                         CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase10 {

enum {
    NUM_THREADS    = 3,
    NUM_ITERATIONS = 100
};

bcemt_Barrier barrier(NUM_THREADS);

bcema_TestAllocator ta;
bcecs_RoundRobin<int> rr(&ta);

extern "C" {
    void *workerThread10(void *arg)
    {
        int id = (bsls_PlatformUtil::IntPtr)arg;

        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){
            ASSERT(rr.add(id) == 1);
            ASSERT(rr.numObjects() >= 0);
            ASSERT(rr.numObjects() <= NUM_THREADS);
            rr.removeAll();
            ASSERT(rr.numObjects() >= 0);
            ASSERT(rr.numObjects() <= NUM_THREADS - 1);
        }
        return NULL;
    }
}  // close extern "C"

}  // close namespace TestCase10
//=============================================================================
//                         CASE 9 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace TestCase9 {
enum {
    NUM_THREADS    = 3,
    NUM_ITERATIONS = 100
};

bcemt_Barrier barrier(NUM_THREADS);

bcema_TestAllocator ta;
bcecs_RoundRobin<int> rr(&ta);

extern "C" {
    void *workerThread9(void *arg)
    {
        int id = (bsls_PlatformUtil::IntPtr)arg;

        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){
            ASSERT(rr.add(id) == 1);
            ASSERT(rr.numObjects() >= 1);
            ASSERT(rr.numObjects() <= NUM_THREADS);
            ASSERT(rr.remove(id) == 1);
            ASSERT(rr.numObjects() >= 0);
            ASSERT(rr.numObjects() <= NUM_THREADS - 1);
        }
        return NULL;
    }
}  // close extern "C"

}  // close namespace TestCase9
//=============================================================================
//                         CASE 8 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase8 {
enum {
    NUM_THREADS    = 3,
    NUM_ITERATIONS = 100
};

int arr[NUM_THREADS * NUM_ITERATIONS];
int n = 0;

bcemt_Barrier barrier(NUM_THREADS);
bcemt_Mutex mutex;

bcema_TestAllocator ta;
bcecs_RoundRobin<int> rr(&ta);

extern "C" {
    void *workerThread8(void *arg)
    {
        int v;

        barrier.wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){
            mutex.lock();
            ASSERT(rr.next(&v) == 1);
            arr[n++] = v;
            mutex.unlock();
        }
        return NULL;
    }
}  // close extern "C"

}  // close namespace TestCase8
//=============================================================================
//                         CASE 7 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase7 {
class A {
    // This class has a static variable 'objCount', that holds the number of
    // objects created for this class.

  int d_value;
  public:
    static int objCount;
    // CONSTRUCTORS
    A(int value)
    {
        objCount++;
        d_value = value;
    }

    A(const A& a)
    {
        objCount++;
        d_value = a.d_value;
    }

    ~A()
    {
        objCount--;
    }

    // ACCESSORS
    int value() const
    {
        return d_value;
    }
};
bool operator==(const A& lhs, const A& rhs)
{
    return lhs.value() == rhs.value();
}

bool operator!=(const A& lhs, const A& rhs)
{
    return lhs.value() != rhs.value();
}

int A::objCount = 0;

}  // close namespace TestCase7
//=============================================================================
//                          CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase6 {
}  // close namespace TestCase6
//=============================================================================
//                         CASE 5 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase5 {
}  // close namespace TestCase5
//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase4 {
}  // close namespace TestCase4
//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase3 {
}  // close namespace TestCase3
//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase2 {
}  // close namespace TestCase2
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase1 {
}  // close namespace TestCase1

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        using namespace TestCase11;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<ServerID> roundRobin(&ta);

        ServerID server1(1);
        ServerID server2(2);
        ServerID server3(3);
        ServerID server4(4);
        ServerID server5(5);

        roundRobin.add(server1);
        roundRobin.add(server2);
        roundRobin.add(server3);
        roundRobin.add(server4);
        roundRobin.add(server5);

        Query query;
        while((query=getClientRequest()) != 0) {
            ServerID server;
            roundRobin.next(&server);
            QueryResult result = executeQuery(server, query);
            display(result);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'ADD', 'REMOVEALL' AND 'NUMOBJECTS':
        //   Verify the concurrent access through 'add' and 'removeAll'
        //
        // Concerns:
        //   That concurrent access to round robin (by 'add' and 'removeAll')
        //   do not introduce inconsistencies.
        //
        // Plan:
        //   Create a round robin.  Create 'NUM_THREADS' threads, each of
        //   which runs a loop.  Each thread, in every iteration, adds an
        //   object, verify the state using 'numObjects', invoke 'removeAll'
        //   and verify the state using 'numObjects'.  Finally join all the
        //   threads and verify the state.
        //
        // Testing:
        //   int   add(const TYPE& object);
        //   void  removeAll();
        //   int   numObjects() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
         << "CONCURRENCY TEST FOR 'ADD', 'REMOVEALL' AND 'NUMOBJECTS'" << endl
         << "========================================================" << endl;

        using namespace TestCase10;

        executeInParallel(NUM_THREADS, workerThread10);

        ASSERT(rr.numObjects() == 0);
        int v;
        ASSERT(rr.next(&v) == 0);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'ADD', 'REMOVE' AND 'NUMOBJECTS':
        //   Verify the concurrent access through 'add' and 'remove'
        //
        // Concerns:
        //   That concurrent access to round robin (by 'add' and 'remove')
        //   do not introduce inconsistencies.
        //
        // Plan:
        //   Create a round robin.  Create 'NUM_THREADS' threads, each of
        //   which runs a loop.  Each thread, in every iteration, adds an
        //   object, verify the state using 'numObjects', removes the same
        //   object using 'remove' and verify the state using
        //   'numObjects'.  Finally join all the threads and verify the
        //   state.
        //
        // Testing:
        //   int   add(const TYPE& object);
        //   int   remove(const TYPE& object);
        //   int   numObjects() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
         << "CONCURRENCY TEST FOR 'ADD', 'REMOVE' AND 'NUMOBJECTS'" << endl
         << "========================================================" << endl;

        using namespace TestCase9;

        executeInParallel(NUM_THREADS, workerThread9);

        ASSERT(rr.numObjects() == 0);
        int v;
        ASSERT(rr.next(&v) == 0);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'NEXT':
        //   Verify the concurrent invocations of 'next'
        //
        // Concerns:
        //   That 'next' works correctly when multiple threads invoke it.
        //
        // Plan:
        //   Create a round robin and add 5 objects to it.  Now create
        //   'NUM_THREADS' threads, each of which runs a loop.  Each
        //   thread, in every iteration, invokes 'next' and (atomically)
        //   put the result into a global array 'arr'.  Finally join all
        //   the threads and verify that 'arr' contains objects in correct
        //   sequence.
        //
        // Testing:
        //   int   next(TYPE *result);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONCURRENCY TEST FOR 'NEXT'" << endl
                          << "===========================" << endl;

        using namespace TestCase8;

        rr.add(1); rr.add(5); rr.add(4); rr.add(3); rr.add(2);
        executeInParallel(NUM_THREADS, workerThread8);
        ASSERT(n == NUM_ITERATIONS * NUM_THREADS);
        for (int i=0; i<n; i+=5) {
            LOOP2_ASSERT(i, arr[i+0], arr[i+0] == 1);
            LOOP2_ASSERT(i, arr[i+1], arr[i+1] == 2);
            LOOP2_ASSERT(i, arr[i+2], arr[i+2] == 3);
            LOOP2_ASSERT(i, arr[i+3], arr[i+3] == 4);
            LOOP2_ASSERT(i, arr[i+4], arr[i+4] == 5);
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTION AND DESTRUCTION:
        //   Verify the object construction and destruction
        //
        // Concerns:
        //   That the objects are properly constructed (by 'add') and
        //   destructed (by 'remove', 'removeAll' and on round robin
        //   destruction).
        //
        // Plan:
        //   Create a round robin, add three objects and verify the number
        //   of created objects.
        //
        //   Remove all 3 objects using 'remove' and verify the number of
        //   created objects.
        //
        //   Add 3 objects, Remove all 3 objects using 'removeAll' and
        //   verify the number of created objects.
        //
        //   Add 3 objects, let the round robin be destroyed and verify
        //   the number of created objects.
        //
        // Testing:
        //   int   add(const TYPE& object);
        //   int   remove(const TYPE& object);
        //   void  removeAll();
        //   ~bcecs_RoundRobin();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING CONSTRUCTION AND DESTRUCTION" << endl
                          << "====================================" << endl;

        using namespace TestCase7;

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<A> rr(&ta);

        A a1(1), a2(2), a3(3);
        ASSERT(A::objCount == 3);

        rr.add(a1); rr.add(a2); rr.add(a3);
        ASSERT(A::objCount == 6);

        rr.remove(a1); rr.remove(a2); rr.remove(a3);
        ASSERT(A::objCount == 3);

        rr.add(a1); rr.add(a2); rr.add(a3);
        ASSERT(A::objCount == 6);

        rr.removeAll();
        ASSERT(A::objCount == 3);

        rr.add(a1); rr.add(a2); rr.add(a3);
        }
        ASSERT(A::objCount == 0);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATION AND DEALLOCATION:
        //   Verify the object allocation and deallocation
        //
        // Concerns:
        //   That the objects are properly allocated (by 'add') and
        //   deallocated (by 'remove' and 'removeAll').
        //
        // Plan:
        //   Create a round robin passing it an instance of
        //   'bdema_testAllocator'.
        //
        //   Add three objects (this will cause the underlying
        //   'bdema_Pool' to request 2 blocks from 'bdema_testAllocator') and
        //   Verify that the number of blocks allocated by
        //   'bdema_testAllocator' is 2.
        //
        //   Invoke 'removeAll', add 3 objects and verify that no more
        //   blocks were requested from 'bdema_testAllocator'.
        //
        //   Remove all 3 objects using 'remove', add 3 objects and verify
        //   that no more blocks were requested from 'bdema_testAllocator'.
        //
        //   Note that this test case depends upon the implementation
        //   detail of 'bdema_Pool' and 'bdema_testAllocator'.
        //
        // Testing:
        //   int   add(const TYPE& object);
        //   int   remove(const TYPE& object);
        //   void  removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ALLOCATION AND DEALLOCATION" << endl
                          << "===================================" << endl;

        using namespace TestCase6;

        bslma_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        rr.add(1); rr.add(2); rr.add(3);

        // note that we are relying on the implementation of 'bdema_Pool'
        LOOP_ASSERT(ta.numAllocations(), ta.numAllocations() == 2);

        rr.removeAll();
        rr.add(1); rr.add(2); rr.add(3);
        LOOP_ASSERT(ta.numAllocations(), ta.numAllocations() == 2);

        rr.remove(1); rr.remove(2); rr.remove(3);
        rr.add(1); rr.add(2); rr.add(3);
        LOOP_ASSERT(ta.numAllocations(), ta.numAllocations() == 2);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'NEXT' AND 'NUMOBJECTS':
        //   Verify 'next' and 'numobjects'.
        //
        // Concerns:
        //   That 'numObjects' and 'next' works correctly in variety of
        //   white-box states.
        //
        // Plan:
        //   Create a round robin and verify the initial state.  Add four
        //   objects and verify 'numObjects' and 'next' after each addition.
        //
        // Testing:
        //   int   next(TYPE *result);
        //   int   numObjects() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'NEXT' AND 'NUMOBJECTS'" << endl
                          << "===============================" << endl;

        using namespace TestCase5;

        int v;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.numObjects() == 0);
        ASSERT(rr.next(&v) == 0);

        ASSERT(rr.add(1) == 1);
        ASSERT(rr.numObjects() == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.add(2) == 1);
        ASSERT(rr.numObjects() == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.add(3) == 1);
        ASSERT(rr.numObjects() == 3);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.add(4) == 1);
        ASSERT(rr.numObjects() == 4);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVEALL':
        //   Verify 'removeAll'.
        //
        // Concerns:
        //   That 'removeAll' works correctly in variety of
        //   white-box states.
        //
        // Plan:
        //   Create a round robin, invoke 'removeAll' and verify that it
        //   fails.
        //
        //   Create a round robin, add one object, invoke 'removeAll' and
        //   verify the state.
        //
        //   Create a round robin, add two object, invoke 'removeAll' and
        //   verify the state.
        //
        //   Create a round robin, add three object, invoke 'removeAll' and
        //   verify the state.
        //
        // Testing:
        //   void  removeAll();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVEALL'" << endl
                          << "===================" << endl;

        using namespace TestCase4;

        int v;

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);

        rr.removeAll();

        ASSERT(rr.numObjects() == 0);

        ASSERT(rr.next(&v) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);

        ASSERT(rr.add(1) == 1);
        rr.removeAll();

        ASSERT(rr.numObjects() == 0);
        ASSERT(rr.next(&v) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);

        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        rr.removeAll();

        ASSERT(rr.numObjects() == 0);
        ASSERT(rr.next(&v) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);

        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.add(3) == 1);
        rr.removeAll();

        ASSERT(rr.numObjects() == 0);
        ASSERT(rr.next(&v) == 0);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE':
        //   Verify 'remove'.
        //
        // Concerns:
        //   That 'remove' works correctly in variety of
        //   white-box states.
        //
        // Plan:
        //   Create a round robin, invoke remove and verify that it fails.
        //
        //   Create a round robin, add one object, remove the added object
        //   and verify the state.
        //
        //   Create a round robin, add two objects, remove the first added
        //   object and verify the state.
        //
        //   Create a round robin, add two objects, remove the second
        //   added object and verify the state.
        //
        //   Create a round robin, add three objects, remove the first
        //   added object and verify the state.
        //
        //   Create a round robin, add three objects, remove the second
        //   added object and verify the state.
        //
        //   Create a round robin, add three objects, remove the third
        //   added object and verify the state.
        //
        // Testing:
        //   int   remove(const TYPE& object);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVE'" << endl
                          << "================" << endl;

        using namespace TestCase3;

        int v;

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.remove(-1) == 0);
        ASSERT(rr.remove(0) == 0);
        ASSERT(rr.remove(1) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.remove(1) == 1);

        ASSERT(rr.numObjects() == 0);

        ASSERT(rr.next(&v) == 0);

        ASSERT(rr.remove(1) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.remove(1) == 1);

        ASSERT(rr.numObjects() == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.remove(1) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.remove(2) == 1);

        ASSERT(rr.numObjects() == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.remove(2) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.add(3) == 1);
        ASSERT(rr.remove(1) == 1);

        ASSERT(rr.numObjects() == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.remove(1) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.add(3) == 1);
        ASSERT(rr.remove(2) == 1);

        ASSERT(rr.numObjects() == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);

        ASSERT(rr.remove(2) == 0);
        }

        {
        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.add(3) == 1);
        ASSERT(rr.remove(3) == 1);

        ASSERT(rr.numObjects() == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.remove(3) == 0);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'ADD':
        //   Verify 'add'.
        //
        // Concerns:
        //   That 'add' works correctly in variety of white-box states.
        //
        // Plan:
        //   Create a round robin and verify the initial state.  Add four
        //   objects and verify state after each addition.
        //
        // Testing:
        //   int add(const TYPE& object);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'ADD'" << endl
                          << "=============" << endl;

        using namespace TestCase2;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        int v;
        enum { PATTERN = 0xaabbccdd };

        ASSERT(rr.numObjects() == 0);

        v = PATTERN;
        ASSERT(rr.next(&v) == 0);
        ASSERT((unsigned)v == PATTERN);

        ASSERT(rr.add(1) == 1);
        ASSERT(rr.numObjects() == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);

        ASSERT(rr.add(1) == 0);

        ASSERT(rr.add(2) == 1);
        ASSERT(rr.numObjects() == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.add(1) == 0);
        ASSERT(rr.add(2) == 0);

        ASSERT(rr.add(3) == 1);
        ASSERT(rr.numObjects() == 3);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.add(1) == 0);
        ASSERT(rr.add(2) == 0);
        ASSERT(rr.add(3) == 0);

        ASSERT(rr.add(4) == 1);
        ASSERT(rr.numObjects() == 4);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.add(1) == 0);
        ASSERT(rr.add(2) == 0);
        ASSERT(rr.add(3) == 0);
        ASSERT(rr.add(4) == 0);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create a round robin, add five objects and verify the state
        //   using 'numObjects' and 'next'.  Remove one object and verify
        //   the state.  Invoke 'removeAll' and verify the state.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace TestCase1;

        bcema_TestAllocator ta(veryVeryVerbose);
        bcecs_RoundRobin<int> rr(&ta);
        int v;

        ASSERT(rr.add(1) == 1);
        ASSERT(rr.add(2) == 1);
        ASSERT(rr.add(3) == 1);
        ASSERT(rr.add(4) == 1);
        ASSERT(rr.add(5) == 1);

        ASSERT(rr.numObjects() == 5);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 5);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 5);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 4);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        ASSERT(rr.remove(4) == 1);
        ASSERT(rr.numObjects() == 4);

        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 5);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 1);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 5);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 3);
        ASSERT(rr.next(&v) == 1);
        LOOP_ASSERT(v, v == 2);

        rr.removeAll();
        ASSERT(rr.numObjects() == 0);
        ASSERT(rr.next(&v) == 0);

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
