// bcemt_rwmutex.t.cpp    -*-C++-*-

#include <bcemt_rwmutex.h>

#include <bcemt_barrier.h>
#include <bcemt_readerwriterlock.h>
#include <bcemt_semaphore.h>
#include <bcemt_thread.h>
#include <bcemt_threadgroup.h>
#include <bces_platform.h>
#include <bdef_bind.h>
#include <bdetu_systemtime.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_numeric.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error "__FILE__"(%d): %s"
               "    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bcemt_RWMutex Obj;

//=============================================================================
//                 TEST PLAN: Positive test cases
//
// This is a breathing test only.  bcemt_RWMutex is a trivial wrapper which
// passes all calls directly to an underlying implementation.  This test
// driver will just verify that the calls are reaching some kind of RW mutex
// by having two reader threads, blocked from finishing by a barrier,
// access a shared read lock.  While they have it, the main thread will try
// to get a write lock, and should fail.
//=============================================================================

//=============================================================================
//                 TEST PLAN: Negative test cases
//
// The negative test cases are for manual benchmarking of the POSIX RW mutex
// vs. the bcemt_readerwriterlock component.  Case -1 is a speed benchmark of
// the POSIX RW mutex.  If POSIX is not available on the platform this driver
// was built for, the test will fail.  Case -2 is a speed benchmark of the
// bcemt_readerwriterlock component.  Case -3 checks the 'bias' of the POSIX
// RW mutex (a concern on IBM) and case -4 checks it for the
// bcemt_readerwriterlock component.
//
// For case -1 and -2, the speed benchmarks, there are two tests done: a
// contention-free test, intended to measure the time overhead involved
// simply in making calls to the lock; and a contention test, intended to
// measure the performance of the lock in switching between multiple
// contending threads.  For the second test, you can optionally set the level
// of contention on the command line.  The syntax is:
//    driver.tsk -1 [NUM_READERS] [NUM_WRITERS]
//
// The default values are 5 and 1.
//=============================================================================

template <typename LOCK>
struct WriteThread
{
   LOCK*            d_lock;
   bcemt_Semaphore *d_startSema;
   bcemt_Barrier   *d_releaseBarrier;

   WriteThread(LOCK *lock = 0,
               bcemt_Semaphore *startSema = 0,
               bcemt_Barrier   *releaseBarrier = 0)
      : d_lock(lock),
        d_startSema(startSema),
        d_releaseBarrier(releaseBarrier)
   {}

   void operator() () {
      d_startSema->post();
      d_lock->lockWrite();
      d_releaseBarrier->wait();
      d_lock->unlock();
   }
};

template <typename LOCK>
struct ReadThread
{
   LOCK            *d_lock;
   bcemt_Barrier   *d_holdBarrier;
   bcemt_Barrier   *d_releaseBarrier;
   bcemt_Semaphore *d_doneSema;

   ReadThread(LOCK *lock = 0,
              bcemt_Barrier   *holdBarrier = 0,
              bcemt_Barrier   *releaseBarrier = 0,
              bcemt_Semaphore *doneSema = 0)
      : d_lock(lock),
        d_holdBarrier(holdBarrier),
        d_releaseBarrier(releaseBarrier),
        d_doneSema(doneSema)
   {}

   void operator() () {
      d_lock->lockRead();
      d_holdBarrier->wait();
      d_releaseBarrier->wait();
      d_lock->unlock();
      if (d_doneSema) {
         d_doneSema->post();
      }
   }
};

template <typename LOCK>
struct ReadWaitThread
{
   LOCK*            d_lock;
   bcemt_Semaphore *d_startSema;
   int*             d_haveLock;

   ReadWaitThread(LOCK            *lock = 0,
                  bcemt_Semaphore *startSema = 0,
                  int             *haveLock = 0)
      : d_lock(lock),
        d_startSema(startSema),
        d_haveLock(haveLock)
   {}

   void operator() () {
      d_startSema->post();
      d_lock->lockRead();
      *d_haveLock = 1;
      d_lock->unlock();
   }
};

template <typename LOCK>
struct PingPongWriter
{
   LOCK*            d_locks;
   bcemt_Semaphore *d_readySema;
   bcemt_Barrier   *d_barrier;
   bces_AtomicInt  *d_stop;
   double          *d_score;

   PingPongWriter(LOCK *locks = 0, bces_AtomicInt *stop = 0, double *score=0,
                  bcemt_Semaphore *readySema=0,
                  bcemt_Barrier *barrier=0)
      : d_locks(locks),
        d_readySema(readySema),
        d_stop(stop),
        d_score(score),
        d_barrier(barrier)
   {}

   void operator() () {
      enum {MICROSECS_PER_SEC     = 1000000};
      static const double SCORE_SCALE = 0.011;
      int numCycles = 0;
      d_locks[0].lockWrite();
      d_locks[2].lockWrite();
      bdet_TimeInterval start, stop;
      if (d_readySema) {
         d_readySema->post();
      }
      if (d_barrier) {
         d_barrier->wait();
      }
      bdetu_SystemTime::loadSystemTimeDefault(&start);
      while (0 == *d_stop) {
         d_locks[0].unlock();
         d_locks[1].lockWrite();
         d_locks[2].unlock();
         d_locks[3].lockWrite();
         d_locks[1].unlock();
         d_locks[0].lockWrite();
         d_locks[3].unlock();
         d_locks[2].lockWrite();
         ++numCycles;
      }
      bdetu_SystemTime::loadSystemTimeDefault(&stop);

      bsls_PlatformUtil::Int64 elapsed = (stop - start).totalMicroseconds();
      *d_score = ((double)numCycles / elapsed) *
         MICROSECS_PER_SEC * SCORE_SCALE;

      d_locks[0].unlock();
      d_locks[2].unlock();
   }
};

template <typename LOCK>
struct PingPongReader
{
   LOCK            *d_locks;
   bcemt_Semaphore *d_readySema;
   bcemt_Barrier   *d_barrier;
   bces_AtomicInt  *d_stop;
   double          *d_score;

   PingPongReader(LOCK *locks = 0, bces_AtomicInt *stop = 0, double *score=0,
                  bcemt_Semaphore *readySema=0,
                  bcemt_Barrier *barrier=0)
      : d_locks(locks),
        d_readySema(readySema),
        d_stop(stop),
        d_score(score),
        d_barrier(barrier)
   {}

   void operator() () {
      enum {MICROSECS_PER_SEC     = 1000000};
      static const double SCORE_SCALE = 0.011;
      int numCycles = 0;
      d_locks[1].lockRead();
      d_locks[3].lockRead();
      bdet_TimeInterval start, stop;
      if (d_readySema) {
         d_readySema->post();
      }
      if (d_barrier) {
         d_barrier->wait();
      }
      bdetu_SystemTime::loadSystemTimeDefault(&start);
      while (0 == *d_stop) {
         d_locks[0].lockRead();
         d_locks[1].unlock();
         d_locks[2].lockRead();
         d_locks[3].unlock();
         d_locks[1].lockRead();
         d_locks[0].unlock();
         d_locks[3].lockRead();
         d_locks[2].unlock();
         ++numCycles;
      }
      bdetu_SystemTime::loadSystemTimeDefault(&stop);

      bsls_PlatformUtil::Int64 elapsed = (stop - start).totalMicroseconds();
      *d_score = ((double)numCycles / elapsed) *
         MICROSECS_PER_SEC * SCORE_SCALE;

      d_locks[1].unlock();
      d_locks[3].unlock();
   }
};

template <typename LOCK>
struct ContentionWriter
{
   LOCK*            d_locks;
   bcemt_Barrier   *d_barrier;
   bces_AtomicInt  *d_stop;
   double          *d_score;

   ContentionWriter(LOCK *locks = 0, bces_AtomicInt *stop = 0,
                    double *score=0, bcemt_Barrier *barrier=0)
      : d_locks(locks),
        d_stop(stop),
        d_score(score),
        d_barrier(barrier)
   {}

   void operator() () {
      enum {MICROSECS_PER_SEC     = 1000000};
      static const double SCORE_SCALE = 2;
      int numCycles = 0;
      d_locks[0].lockWrite();
      bdet_TimeInterval start, stop;
      if (d_barrier) {
         d_barrier->wait();
      }
      bdetu_SystemTime::loadSystemTimeDefault(&start);
      while (0 == *d_stop) {
         d_locks[0].unlock();
         d_locks[1].lockWrite();
         d_locks[0].lockWrite();
         d_locks[1].unlock();
         ++numCycles;
      }
      bdetu_SystemTime::loadSystemTimeDefault(&stop);

      bsls_PlatformUtil::Int64 elapsed = (stop - start).totalMicroseconds();
      *d_score = ((double)numCycles / elapsed) *
         MICROSECS_PER_SEC * SCORE_SCALE;

      d_locks[0].unlock();
   }
};

template <typename LOCK>
struct ContentionReader
{
   LOCK            *d_locks;
   bcemt_Barrier   *d_barrier;
   bces_AtomicInt  *d_stop;
   double          *d_score;

   ContentionReader(LOCK *locks = 0, bces_AtomicInt *stop = 0,
                    double *score=0,
                    bcemt_Barrier *barrier=0)
      : d_locks(locks),
        d_stop(stop),
        d_score(score),
        d_barrier(barrier)
   {}

   void operator() () {
      enum {MICROSECS_PER_SEC     = 1000000};
      static const double SCORE_SCALE = 1;
      int numCycles = 0;
      d_locks[1].lockRead();
      bdet_TimeInterval start, stop;
      if (d_barrier) {
         d_barrier->wait();
      }
      bdetu_SystemTime::loadSystemTimeDefault(&start);
      while (0 == *d_stop) {
         d_locks[0].lockRead();
         d_locks[0].unlock();
         d_locks[0].lockRead();
         d_locks[0].unlock();
         d_locks[0].lockRead();
         d_locks[0].unlock();
         d_locks[1].unlock();
         d_locks[1].lockRead();
         ++numCycles;
      }
      bdetu_SystemTime::loadSystemTimeDefault(&stop);

      bsls_PlatformUtil::Int64 elapsed = (stop - start).totalMicroseconds();
      *d_score = ((double)numCycles / elapsed) *
         MICROSECS_PER_SEC * SCORE_SCALE;

      d_locks[1].unlock();
   }
};

template <typename LOCK>
int benchmarkSpeed (LOCK* lock, const char* lockName,
                    int numWriters, int numReaders)
{
   int rc;

   // This 4-lock "ping-pong" benchmark approach is from
   // de Supinski and May, "Benchmarking PTHREADS Performance", 1999
   LOCK locks[4];

   // First, we lock and unlock a vector of mutexes in the main thread
   // for 3 seconds.  This is the "individual overhead" score.
   double score, overallScore = 0;
   {
      enum {NUM_MUTEXES=750, MICROSECS_PER_SEC = 1000000};
      static const double SCORE_SCALE = 1.33;
      bdet_TimeInterval start, stop;
      LOCK lockArray[NUM_MUTEXES];
      LOCK *lockEnd = lockArray + NUM_MUTEXES;
      bdetu_SystemTime::loadSystemTimeDefault(&start);
      int numCycles = 0;
      while (true) {
         for_each(lockArray, lockEnd,
                  mem_fun_ref(&LOCK::lockWrite));
         for_each(lockArray, lockEnd,
                  mem_fun_ref(&LOCK::unlock));
         ++numCycles;
         if (0 == (++numCycles % 4)) {
            bdetu_SystemTime::loadSystemTimeDefault(&stop);
            if (3.0 <= (stop - start).totalSecondsAsDouble()) {
               break;
            }
         }
      }

      bsls_PlatformUtil::Int64 elapsed = (stop - start).totalMicroseconds();
      score = ((double)numCycles / elapsed) * MICROSECS_PER_SEC * SCORE_SCALE;

      cout << "Lock \"" << lockName << "\": individual-overhead score="
           << score << endl;
      overallScore += score / 6.0;
   }

   // Now we run the writer by itself for 3 seconds.  This score
   // is the "no-contention overhead."
   bces_AtomicInt stop = 0;
   bcemt_Semaphore startSema;
   PingPongWriter<LOCK> writerThread(locks, &stop, &score, &startSema);
   bcemt_ThreadUtil::Handle hWriter, hReader;
   if (0 != bcemt_ThreadUtil::create(&hWriter, writerThread)) {
      cout << "ERROR: Could not create a thread!! Failing test" << endl;
      return -4;
   }
   startSema.wait();
   bcemt_ThreadUtil::sleep(bdet_TimeInterval(3));
   stop = 1;
   bcemt_ThreadUtil::join(hWriter);
   cout << "Lock \"" << lockName << "\": no-contention-overhead score="
        << score << endl;
   overallScore += score / 6.0;

   // Now we run the writer with 1 reader to get the "lo-contention" score.
   bcemt_Barrier startBarrier(3);
   stop = 0;
   writerThread.d_readySema = 0;
   writerThread.d_barrier = &startBarrier;
   double readerScore;
   PingPongReader<LOCK> readerThread(locks, &stop, &readerScore, 0,
                                     &startBarrier);
   if (0 != bcemt_ThreadUtil::create(&hWriter, writerThread) ||
       0 != bcemt_ThreadUtil::create(&hReader, readerThread)) {
      cout << "ERROR: Could not create a thread!! Failing test" << endl;
      return -4;
   }
   startBarrier.wait();
   bcemt_ThreadUtil::sleep(bdet_TimeInterval(3));
   stop = 1;
   bcemt_ThreadUtil::join(hWriter);
   bcemt_ThreadUtil::join(hReader);

   score = (readerScore + score) * 100.0;
   cout << "Lock \"" << lockName << "\": lo-contention score="
        << score << endl;
   overallScore += score / 6.0;

   // For the "hi-contention" test, we want more readers contending with
   // a writer over a smaller number of mutexes
   LOCK conLocks[2];

   bcemt_Barrier startBarrier2(numReaders+2);
   stop = 0;
   bcemt_ThreadGroup allThreads;

   if (0 != allThreads.addThread(ContentionWriter<LOCK>(conLocks, &stop,
                                                        &score,
                                                        &startBarrier2))) {
      cout << "ERROR: Could not create a thread!! Failing test" << endl;
      return -4;
   }

   vector<double> readerScores(numReaders, 0.0);
   for (int i = 0; i < numReaders; ++i) {
      ContentionReader<LOCK> reader(conLocks, &stop,
                                    &readerScores[i],
                                    &startBarrier2);
      if (0 != allThreads.addThread(reader)) {
         cout << "ERROR: Could not create a thread!! Failing test" << endl;
         return -4;
      }
   }
   startBarrier2.wait();
   bcemt_ThreadUtil::sleep(bdet_TimeInterval(3));
   stop = 1;
   allThreads.joinAll();

   score = (accumulate(readerScores.begin(),
                       readerScores.end(), 0) / (numReaders * 2.0)
            + score/2.0);
   cout << "Lock \"" << lockName << "\": hi-contention score="
        << score << endl;
   overallScore += score / 2.0;

   cout << "Lock \"" << lockName << "\": overall score="
        << overallScore << endl;

   return 0;
}

template <typename LOCK>
int benchmarkWriterBias(bool* isWriterBias, LOCK* lock)
{
   bcemt_ThreadAttributes detached;
   detached.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
   bcemt_ThreadUtil::Handle h;

   bcemt_Barrier   readHold(2), readRelease(2), writeRelease(2);
   bcemt_Semaphore writeStart;

   if (0 != bcemt_ThreadUtil::create
       (&h, detached, ReadThread<LOCK>(lock, &readHold, &readRelease))) {
      return -4;
   }
   readHold.wait();
   // Now there is a read thread holding the lock

   if (0 != bcemt_ThreadUtil::create
       (&h, detached, WriteThread<LOCK>(lock, &writeStart, &writeRelease))) {
      return -4;
   }
   writeStart.wait();
   bcemt_ThreadUtil::yield();
   // Now we can be pretty confident that the write thread is blocked waiting
   // on the mutex.  For our manual benchmarking this is sufficient.

   if (0 == (*isWriterBias = lock->tryLockRead())) {
      lock->unlock();
   }
   readRelease.wait();
   writeRelease.wait();

   return 0;
}

template <typename LOCK>
int benchmarkBiasFairness(bool* isFair, LOCK* lock)
{
   bcemt_ThreadAttributes detached;
   detached.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);
   bcemt_ThreadUtil::Handle h;

   bcemt_Barrier   readHold(2), readRelease1(2), writeRelease1(2),
      writeRelease2(2);
   bcemt_Semaphore readStart, writeStart, readDone;

   if (0 != bcemt_ThreadUtil::create
       (&h, detached, ReadThread<LOCK>(lock, &readHold,
                                 &readRelease1, &readDone))) {
      return -4;
   }
   readHold.wait();
   // Now there is a read thread holding the lock

   if (0 != bcemt_ThreadUtil::create
       (&h, detached, WriteThread<LOCK>(lock, &writeStart, &writeRelease1))) {
      return -4;
   }
   writeStart.wait();
   bcemt_ThreadUtil::yield();

   // There is one thread blocked waiting for the write lock.  Create
   // a thread that will block waiting for the read qlock.
   int rwThreadHasLock = 0;
   ReadWaitThread<LOCK> readWaitThread(lock, &readStart,
                                       &rwThreadHasLock);
   if (0 != bcemt_ThreadUtil::create(&h, detached, readWaitThread)) {
      return -4;
   }
   readStart.wait();
   bcemt_ThreadUtil::yield();

   // Release the first read lock and allow the first write lock to
   // get the lock
   readRelease1.wait();
   readDone.wait();
   bcemt_ThreadUtil::yield();
   bcemt_ThreadUtil::yield();

   // Create a second thread to wait on the write lock
   if (0 != bcemt_ThreadUtil::create
       (&h, detached, WriteThread<LOCK>(lock, &writeStart, &writeRelease2))) {
      return -4;
   }
   writeStart.wait();
   bcemt_ThreadUtil::yield();

   // Let the first writer release the lock
   writeRelease1.wait();
   bcemt_ThreadUtil::sleep(bdet_TimeInterval(1));

   // Now, is the second read thread unblocked?
   *isFair = rwThreadHasLock;

   writeRelease2.wait();

   return 0;
}

template <typename LOCK>
int benchmarkBias (LOCK* lock, const char* lockName)
{
   // The "bias" of the RW mutex is the behavior it exhibits when the
   // read lock is held, a thread is blocked waiting for the write lock,
   // and a second thread attempts to get a shared read lock.  Implementations
   // that bias against writer starvation will prevent the second thread
   // from acquiring the lock.
   //
   // If the lock biases towards writers, there is a second possible fairness
   // behavior to test.  In the scenario above, if there are *two* threads
   // blocked waiting for the write lock, the lock has a decision to make
   // after the first thread acquires and releases the lock - whether or
   // not to release the read thread that was blocked, or release the second
   // write thread.  If it releases the read thread, it is biased to writers
   // with fairness for readers.

   bool writerBias = false;

   int rc = benchmarkWriterBias(&writerBias, lock);
   if (0 != rc) {
      cout << "Error benchmarking writer bias: " << rc << endl;
      return rc;
   }

   if (writerBias) {
      cout << "Lock \"" << lockName << "\" is biased to writers...  ";

      bool fair = false;
      rc = benchmarkBiasFairness(&fair, lock);
      if (0 != rc) {
         cout << "Error benchmarking writer fairness: " << rc << endl;
         return rc;
      }

      cout << (fair ? "with " : "without ")
           << "fairness provision for readers" << endl;
   }
   else {
      cout << "Lock \"" << lockName << "\" is biased to readers." << endl;
   }

   return 0;
}

template <typename LOCK>
int benchmarkRecursion (LOCK* lock, const char* lockName)
{
   // We are specifically testing read-lock recursion here.  First try
   // naive support - can we call lockRead twice?

   lock->lockRead();
   if (0 == lock->tryLockRead()) {
      lock->unlock();
      cout << "Lock \"" << lockName
           << "\" supports naive read-lock recursion.  " << endl;
   }
   else {
      cout << "Lock \"" << lockName
           << "\" does not support naive read-lock recursion." << endl;
   }
   lock->unlock();
   if (0 == lock->tryLockWrite()) {
      lock->unlock();
   }
   else {
      cout << "...OOPS!  Lock \"" << lockName
           << "\" does not clean up after naive read-lock recursion." << endl;
   }

   // The main thread will get the read lock and allow a writer to wait.
   // Then it will attempt to get the read lock again.  If it succeeds,
   // read-lock recursion is supported.

   bcemt_ThreadUtil::Handle handle;
   bcemt_Semaphore writeStart;
   bcemt_Barrier writeRelease(2);

   bcemt_ThreadAttributes detached;
   detached.setDetachedState(bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED);

   lock->lockRead();
   if (0 != bcemt_ThreadUtil::create
       (&handle, detached,
        WriteThread<LOCK>(lock, &writeStart, &writeRelease))) {
      return -4;
   }
   writeStart.wait();
   bcemt_ThreadUtil::yield();

   // Now we can be pretty confident that the write thread is blocked waiting
   // on the mutex.  For our manual benchmarking this is sufficient.

   if (0 == lock->tryLockRead()) {
      lock->unlock();
      cout << "Lock \"" << lockName
           << "\" supports read-lock recursion.  " << endl;
   }
   else {
      cout << "Lock \"" << lockName
           << "\" does not support read-lock recursion." << endl;
   }

   lock->unlock();
   writeRelease.wait();

   return 0;
}

void readerThread (bcemt_Barrier* start, bcemt_Barrier* end, Obj* mutex)
{
   ASSERT(0 == mutex->tryLockRead());
   start->wait();
   end->wait();
   mutex->unlock();
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    int readers = 5, writers = 1; // for negative cases
    if (argc > 2) {
       readers = atoi(argv[2]);
    }
    if (argc > 3) {
       cout << "WARNING: setting # of writers not supported yet" << endl;
       //writers = atoi(argv[3]);
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case -1: {
#ifdef BCES_PLATFORM__POSIX_THREADS
         cout << "Running POSIX speed test" << endl;

#ifdef BSLS_PLATFORM__OS_AIX
         bcemt_RWMutexImpl<bces_Platform::PosixThreads> lock;
#else
         bcemt_RWMutex lock;
#endif

         ASSERT(0 == benchmarkSpeed(&lock, "PTHREAD", writers, readers));
#else
         cout << "POSIX speed test cannot be run on this platform!" << endl;
#endif
      } break;
      case -2: {
         cout << "Running WIN32 speed test" << endl;

         bcemt_ReaderWriterLock lock;
         ASSERT(0 == benchmarkSpeed(&lock, "PURE BCE", writers, readers));
      } break;
      case -3: {
#ifdef BCES_PLATFORM__POSIX_THREADS
         cout << "Running POSIX bias test" << endl;

#ifdef BSLS_PLATFORM__OS_AIX
         bcemt_RWMutexImpl<bces_Platform::PosixThreads> lock;
#else
         bcemt_RWMutex lock;
#endif
         ASSERT(0 == benchmarkBias(&lock, "PTHREAD"));
#else
         cout << "POSIX bias test cannot be run on this platform!" << endl;
#endif
      } break;
      case -4: {
         cout << "Running WIN32 bias test" << endl;

         bcemt_ReaderWriterLock lock;
         ASSERT(0 == benchmarkBias(&lock, "PURE BCE"));
      } break;
      case -5: {
         cout << "Running POSIX recursion test" << endl;
#ifdef BSLS_PLATFORM__OS_AIX
         bcemt_RWMutexImpl<bces_Platform::PosixThreads> lock;
#else
         bcemt_RWMutex lock;
#endif
         ASSERT(0 == benchmarkRecursion(&lock, "PTHREAD"));
      } break;
      case -6: {
         cout << "Running WIN32 recursion test" << endl;

         bcemt_ReaderWriterLock lock;
         ASSERT(0 == benchmarkRecursion(&lock, "PURE BCE"));
      } break;

      case 1: {
        if (verbose) cout << "Testing: breathing test" << endl
                          << "=========================="
                          << endl;
        Obj mutex;
        bcemt_Barrier startBarrier(3), endBarrier(3);

        bcemt_ThreadUtil::Handle t1, t2;

        if (0 != bcemt_ThreadUtil::create
            (&t1, bdef_BindUtil::bind(&readerThread,
                                      &startBarrier, &endBarrier,
                                      &mutex)) ||
            0 != bcemt_ThreadUtil::create
            (&t2, bdef_BindUtil::bind(&readerThread,
                                      &startBarrier, &endBarrier,
                                      &mutex))) {
           ASSERT(!"Could not create threads!! Bad state! Failing test.");
        }
        else {
           startBarrier.wait();
           ASSERT(0 != mutex.tryLockWrite());
           endBarrier.wait();

           ASSERT(0 == bcemt_ThreadUtil::join(t1));
           ASSERT(0 == bcemt_ThreadUtil::join(t2));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
