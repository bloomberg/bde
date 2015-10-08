// bsls_spinlock.t.cpp                                                -*-C++-*-
#include <bsls_spinlock.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdlib.h>  // atoi
#include <stdio.h>   // printf

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
typedef HANDLE    ThreadId;

// no rand_r on Windows, so put a simple definition here
static int rand_r(unsigned *seed) {
    unsigned int next = *seed;
    int result;

    next *= 1103515245; next += 12345;
    result = (unsigned int) (next / 65536) % 2048;

    next *= 1103515245; next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    *seed = next;
    return result;
}

#else
typedef pthread_t ThreadId;
#endif

typedef void *(*ThreadFunction)(void *arg);

static
ThreadId createThread(ThreadFunction func, void *arg)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return CreateThread(0, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, 0);
#else
    ThreadId id;
    pthread_create(&id, 0, func, arg);
    return id;
#endif
}

static
void joinThread(ThreadId id)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    WaitForSingleObject(id, INFINITE);
    CloseHandle(id);
#else
    pthread_join(id, 0);
#endif
}

static
void sleepSeconds(int seconds)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

using namespace BloombergLP;
using namespace std;

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
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

           // ==============================================
           //                  USAGE EXAMPLE
           // ----------------------------------------------

// Suppose that we want to determine the maximum number of threads executing
// a block of code concurrently. Note that such a use case naturally calls
// for a statically initialized lock and the critical region involves
// a few integer operations; spinlock may be suitable.
//
// First, we define a type to manage the count within a scope:

class MaxConcurrencyCounter {
     // This type manages a count and high-water-mark within a scope.
     // It decrements the count in its destructor upon leaving the scope.

     // DATA
     int           *d_count_p;
     bsls::SpinLock *d_lock_p;

   public:
     // CREATORS
    MaxConcurrencyCounter(int *count, int *max, bsls::SpinLock *lock);
         // Acquire the specified 'lock' and increment the specified 'count'.
         // If the resulting value is larger than the specified 'max',
         // load it into 'max'. Release 'lock' and create a scoped guard to
         // decrement 'count' on destruction.

     ~MaxConcurrencyCounter();
         // Acquire the lock specified at construction, decrement the count
         // variable, and release the lock.
};

MaxConcurrencyCounter::MaxConcurrencyCounter(int            *count,
                                             int            *max,
                                             bsls::SpinLock *lock)
: d_count_p(count)
, d_lock_p(lock) {
    bsls::SpinLockGuard guard(lock);
    int result = ++(*count);
    if (result > *max) {
        *max = result;
    }
}

MaxConcurrencyCounter::~MaxConcurrencyCounter() {
    bsls::SpinLockGuard guard(d_lock_p);
    --(*d_count_p);
}

static int            usageExampleThreadCount = 0;
static int            usageExampleMaxThreads = 0;
static bsls::SpinLock usageExampleThreadLock = BSLS_SPINLOCK_UNLOCKED;

extern "C" void *usageExampleFn(void *arg) {
    // Next, by creating a 'MaxConcurrencyCounter' object, each thread
    // entering the block of code uses the 'SpinLock' to synchronize
    // manipulation of the static count variables:

    MaxConcurrencyCounter counter(&usageExampleThreadCount,
                                  &usageExampleMaxThreads,
                                  &usageExampleThreadLock);

    sleepSeconds(1);

    // Finally, closing the block synchronizes on the 'SpinLock' again
    // to decrement the thread count. Any intervening code can run in
    // parallel.
    return 0;
}

// Suppose that we have a large array of objects to be manipulated concurrently
// by multiple threads, but the size of the array itself does not change.
// (This might be because it represents an inherently fixed number of objects
// or because changes to the array size are infrequent and controlled by some
// other synchronization mechanism like a "reader-writer" lock). Thus one
// thread can manipulate a particular object in the array concurrently with a
// different thread manipulating another. If the manipulations are short and
// contention is likely to be low, SpinLock might be suitable due to its small
// size.
//
// In particular, imagine we want a threadsafe "multi-queue". In this case,
// we would have an array of queues, each with a SpinLock member for
// fine-grained locking.  First, we define the type to be held in the array.
template<typename TYPE>
class LightweightThreadsafeQueue {
  // This type implements a threadsafe queue with a small memory
  // footprint and low initialization costs. It is designed for
  // low-contention use only.

  // TYPES
  struct Node {
       TYPE  d_item;
       Node *d_next_p;

       Node(const TYPE& item) : d_item(item), d_next_p(0) {}
   };

  // DATA
  Node           *d_front_p; // Front of queue, or 0 if empty
  Node           *d_back_p; // Back of queue, or 0 if empty
  bsls::SpinLock  d_lock;

 public:
  // CREATORS
  LightweightThreadsafeQueue();
    // Create an empty queue.

  ~LightweightThreadsafeQueue();
    // Destroy this object.

  // MANIPULATORS
  int dequeue(TYPE* value);
     // Remove the element at the front of the queue and load it into the
     // specified 'value'. Return '0' on success, or a nonzero value if
     // the queue is empty.

  void enqueue(const TYPE& value);
     // Add the specified 'value' to the back of the queue.
};

// Next, we implement the creators. Note that a different idiom is used
// to initialize member variables of 'SpinLock' type than is used for static
// variables:
template<typename TYPE>
LightweightThreadsafeQueue<TYPE>::LightweightThreadsafeQueue()
: d_front_p(0)
, d_back_p(0)
, d_lock(bsls::SpinLock::s_unlocked)
{}

template<typename TYPE>
LightweightThreadsafeQueue<TYPE>::~LightweightThreadsafeQueue() {
   for (Node *node = d_front_p; 0 != node; ) {
       Node *next = node->d_next_p;
       delete node;
       node = next;
   }
}

// Then we implement the manipulator functions using 'SpinLockGuard' to ensure
// thread safety.
template<typename TYPE>
int LightweightThreadsafeQueue<TYPE>::dequeue(TYPE* value) {
   Node *front;
   {
      bsls::SpinLockGuard guard(&d_lock);
      front = d_front_p;
      if (0 == front) {
        return 1;
      }

      *value = front->d_item;

      if (d_back_p == front) {
         d_front_p = d_back_p = 0;
      } else {
         d_front_p = front->d_next_p;
      }
   }
   delete front;
   return 0;
}

template<typename TYPE>
void LightweightThreadsafeQueue<TYPE>::enqueue(const TYPE& value) {
   Node *node = new Node(value);
   bsls::SpinLockGuard guard(&d_lock);
   if (0 == d_front_p && 0 == d_back_p) {
      d_front_p = d_back_p = node;
   } else {
      d_back_p->d_next_p = node;
      d_back_p = node;
   }
}

//  To illustrate fine-grained locking with this queue, we create a thread
//  function that will manipulate queues out of a large array at random.
//  Since each element in the array is locked independently, these threads
//  will rarely contend for the same queue and can run largely in parallel.

const int NUM_QUEUES = 10000;
const int NUM_ITERATIONS = 20000;

struct QueueElement {
   int d_threadId;
   int d_value;
};

struct ThreadParam {
   LightweightThreadsafeQueue<QueueElement> *d_queues_p;
   int                                       d_threadId;
};

void *addToRandomQueues(void *paramAddr) {
   ThreadParam *param = (ThreadParam*)paramAddr;
   LightweightThreadsafeQueue<QueueElement> *queues = param->d_queues_p;
   int threadId = param->d_threadId;
   unsigned seed = threadId;
   for (int i = 0; i < NUM_ITERATIONS; ++i) {
      int queueIndex = rand_r(&seed) % NUM_QUEUES;
      LightweightThreadsafeQueue<QueueElement> *queue = queues + queueIndex;
      QueueElement value = { threadId, i };
      queue->enqueue(value);
   }
   return 0;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int             test = argc > 1 ? atoi(argv[1]) : 0;
    const bool         verbose = argc > 2;
    const bool     veryVerbose = argc > 3;
    const bool veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
    case 1: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concern:
        //: 1 The usage examples provided in the component header file compile,
        //:   link, and run as shown.
        //
        // Plan:
        //: 1 Place the block of code from usage example 1 in a function
        //:   to be executed by N threads. In the parallelizable region, sleep
        //:   for a second. This should allow all N threads to be in that region
        //:   concurrently. Validate that the "maxThreads" count is N after the
        //:   threads are joined.
        //:
        //: 2 Execute usage example 2, validating that all elements from all
        //:   threads are present in the multiqueue after joining the worker
        //:   threads.

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        {
            if (veryVerbose) printf("Example 1...\n");
            enum { NUM_THREADS = 10 };

            ThreadId threads[NUM_THREADS];

            for (int i = 0; i < NUM_THREADS; ++i) {
                threads[i] = createThread(&usageExampleFn, 0);
            }
            for (int i = 0; i < NUM_THREADS; ++i) {
                joinThread(threads[i]);
            }

            ASSERTV(usageExampleThreadCount,
                    0 == usageExampleThreadCount);
            ASSERTV(usageExampleMaxThreads,
                    NUM_THREADS == usageExampleMaxThreads);
        }

        {
            if (veryVerbose) printf("Example 2...\n");

// Finally, we create the "multi-queue" and several of these threads to
// manipulate it.  We assume the existence of a createThread() function that
// starts a new thread of execution with a parameter, and we omit details of
// "joining" these threads.
            enum { NUM_THREADS = 3};
            LightweightThreadsafeQueue<QueueElement> multiQueue[NUM_QUEUES];
            ThreadParam threadParams[NUM_THREADS];
            ThreadId threadIds[NUM_THREADS];
            for (int i = 0; i < NUM_THREADS; ++i) {
                threadParams[i].d_queues_p = multiQueue;
                threadParams[i].d_threadId = i + 1;
                threadIds[i] =
                    createThread(addToRandomQueues, threadParams + i);
            }

            // Join the threads, then count the number of values pushed by
            // each thread into all queues. This should be exactly
            // NUM_ITERATIONS for each.
            int elementCount[NUM_THREADS];
            for (int i = 0; i < NUM_THREADS; ++i) {
                joinThread(threadIds[i]);
                elementCount[i] = 0;
            }

            for (int i = 0; i < NUM_QUEUES; ++i) {
                QueueElement element;
                while (0 == multiQueue[i].dequeue(&element)) {
                    ++elementCount[element.d_threadId - 1];
                }
            }

            for (int i = 0; i < NUM_THREADS; ++i) {
                ASSERTV(i, elementCount[i],
                        NUM_ITERATIONS == elementCount[i]);
            }
        }
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
