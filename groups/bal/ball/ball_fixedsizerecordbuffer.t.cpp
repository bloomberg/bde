// ball_fixedsizerecordbuffer.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_fixedsizerecordbuffer.h>

#include <ball_severity.h>

#include <bslmt_barrier.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>

#include <bslma_default.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_ctime.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'ball::FixedSizeRecordBuffer' is divided into following parts (apart
// from breathing test and usage example).
//   Testing in presence of single thread.  This is further divided into
//   following subparts.
//     Testing primary manipulator 'pushBack'.  This is tested in [2].  In this
//     test case, we do not consider the effect of 'maxTotalSize' parameter
//     (provided at buffer creation); that is tested in a later test case.
//
//     Testing manipulators 'popBack', 'popFront', 'pushFront', 'removeAll'.
//     They are tested in [3], [12], [13] and [4] respectively.
//
//     Testing accessors 'back' 'front' and 'length'.  They are tested in
//     [5], [11] and [6] respectively.
//
//     Testing 'pushBack' considering the effect of 'maxTotalSize' parameter.
//     This is tested in [7].
//
//   Testing in presence of multiple thread.  This is further divided into
//   following subparts.
//     Testing concurrent access to the buffer by manipulators 'pushBack',
//     'back' and 'removeAll'.  This is tested in [8].
//
//     Testing that a thread gets exclusive access to the buffer between the
//     calls to 'beginSequence' and 'endSequence'.  This is tested in [9]
//     and [10].
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] ball::FixedSizeRecordBuffer();
// [ 1] ~ball::FixedSizeRecordBuffer();
//
// MANIPULATORS
// [10] void beginSequence();
// [10] void endSequence();
// [ 3] void popBack();
// [12] void popFront();
// [ 2] int  pushBack(const bsl::shared_ptr<ball::Record>& handle);
// [13] int  pushFront(const bsl::shared_ptr<ball::Record>& handle);
// [ 4] void removeAll();
//
// ACCESSORS
// [ 6] int length() const;
// [ 5] const bsl::shared_ptr<ball::Record>& back() const;
// [11] const bsl::shared_ptr<ball::Record>& front() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] TESTING 'PUSHBACK' CONSIDERING THE EFFECT OF 'MAX_TOTAL_SIZE' PARAMETER
// [ 8] CONCURRENCY TEST FOR 'PUSHBACK', 'POPBACK', 'REMOVE_ALL' AND 'LENGTH'
// [ 9] CONCURRENCY TEST-1 FOR 'BEGIN_SEQUENCE' AND 'END_SEQUENCE'
// [14] STRESS TEST
// [15] USAGE EXAMPLE
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

static bslmt::Mutex printMutex;  // mutex to protect output macros

#define PT(X) { printMutex.lock(); P(X); printMutex.unlock(); }
#define PT_(X) { printMutex.lock(); P_(X); printMutex.unlock(); }

static bslmt::Mutex &assertMutex = printMutex;  // mutex to protect assert
                                                // macros

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
typedef bsl::shared_ptr<ball::Record> Handle;
//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
void executeInParallel(int numThreads, bslmt::ThreadUtil::ThreadFunction func)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Number each thread (sequentially from 0 to 'numThreads-1') by passing
    // 'i' to i'th thread.  Finally join all the threads.
{
    bslmt::ThreadUtil::Handle *threads =
                               new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], func, (void*)i);
    }
    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

void executeInParallel(int                                numThreads,
                       bslmt::ThreadUtil::ThreadFunction  func,
                       Handle                            *handles)
    // Create the specified 'numThreads', each executing the specified 'func'.
    // Pass 'handle[i]' to i'th created thread where 'i' ranges from 0 to
    // 'numThreads'.  Finally join all the threads.
{
    bslmt::ThreadUtil::Handle *threads =
                               new bslmt::ThreadUtil::Handle[numThreads];
    ASSERT(threads);

    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::create(&threads[i], func, (void *)(&handles[i]));
    }
    for (int i = 0; i < numThreads; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    delete [] threads;
}

ball::Record *buildRecord(bdlt::Datetime    time,
                          int               pid,
                          int               tid,
                          const char       *file,
                          int               line,
                          const char       *category,
                          int               severity,
                          const char       *msg,
                          bslma::Allocator *alloc)
{
    ball::Record *r = new (*alloc) ball::Record(alloc);

    r->fixedFields().setTimestamp(time);
    r->fixedFields().setProcessID(pid);
    r->fixedFields().setThreadID(tid);
    r->fixedFields().setFileName(file);
    r->fixedFields().setCategory(category);
    r->fixedFields().setMessage(msg);
    r->fixedFields().setLineNumber(line);
    r->fixedFields().setSeverity(severity);

    return r;
}
//=============================================================================
//                         CASE 15 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace TestCase15
{
enum {
    K              = 1024,
    MAX_TOTAL_SIZE = 32 * K,
    NUM_ITERATIONS = 1000,
    NUM_THREADS    = 4
};

bslma::Allocator *basicAllocator = bslma::Default::defaultAllocator();
ball::FixedSizeRecordBuffer recordBuffer(MAX_TOTAL_SIZE, basicAllocator);

extern "C" {
void *workerThread15(void *arg)
{
    int id = (int)(bsls::Types::IntPtr)arg; // thread id
    for(int i = 0; i < NUM_ITERATIONS; ++i){
        ball::Record *record =
                       new (*basicAllocator) ball::Record(basicAllocator);

        // build a message
        enum { MAX = 100 };
        char msg[MAX];
        sprintf(msg, "message no. %d from thread no. %d", i, id);

        record->fixedFields().setMessage(msg);
        Handle handle(record, basicAllocator, basicAllocator);
        recordBuffer.pushBack(handle);
    }

    // print record message in LIFO order
    recordBuffer.beginSequence();
    while (recordBuffer.length()) {
        const ball::Record &rec = *recordBuffer.back();
        if (veryVerbose) cout << "\t" << rec.fixedFields().message() << endl;
        recordBuffer.popBack();
    }
    recordBuffer.endSequence();

    return NULL;
}
} // extern "C"

}  // close namespace TestCase15
//-----------------------------------------------------------------------------
namespace TestCase14
{
}  // close namespace TestCase14
//-----------------------------------------------------------------------------
namespace TestCase13
{
}  // close namespace TestCase13
//-----------------------------------------------------------------------------
namespace TestCase12
{
}  // close namespace TestCase12
//-----------------------------------------------------------------------------
namespace TestCase11
{
}  // close namespace TestCase11
//=============================================================================
//                         CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase10
{
enum {
    K               = 1024,
    MAX_TOTAL_SIZE  = 1000 * K,
    SLEEP_TIME      = 1,
    NUM_ITERATIONS1 = 10,
    NUM_ITERATIONS2 = 1000
};

bslmt::Barrier barrier(2);

bslma::Allocator *alloc = bslma::Default::defaultAllocator();
ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

extern "C" {
void *pushBackPopRemoveAllThread(void *arg)
{
    static int call = 0;
    barrier.wait();
    for (int i=0; i < NUM_ITERATIONS2; ++i) {
        Handle& h = *((Handle *)arg);
        rb.pushBack(h); rb.pushBack(h); rb.pushBack(h); rb.pushBack(h);
        rb.pushBack(h);
        if (++call % 2 == 0)
            rb.removeAll();
        else {
            rb.popBack(); rb.popBack(); rb.popBack(); rb.popBack();
            rb.popBack();
        }
    }
    return NULL;
}

void *beginSequenceEndSequenceThread(void *arg)
{
    barrier.wait();
    for (int i=0; i < NUM_ITERATIONS1; ++i) {
        rb.beginSequence();
        int len = rb.length();
        bslmt::ThreadUtil::microSleep(SLEEP_TIME);
        int len1 = rb.length();
        LOOP2_ASSERT(len, len1, len == len1);
        rb.endSequence();
    }
    return NULL;
}

} // extern "C"

}  // close namespace TestCase10
//=============================================================================
//                          CASE 9 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase9
{
enum {
    K              = 1024,
    MAX_TOTAL_SIZE = 32 * K,
    NUM_THREADS    = 4,
    SLEEP_TIME     = 1
};

enum {
    VALID,
    INVALID
};
int state = VALID;

bslma::Allocator *alloc = bslma::Default::defaultAllocator();
ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

extern "C" {
    void *workerThread9(void *arg)
    {
        rb.beginSequence();
        ASSERT(state == VALID);
        state = INVALID;
        bslmt::ThreadUtil::microSleep(SLEEP_TIME);
        state = VALID;
        rb.endSequence();
        return NULL;
    }
} // extern "C"

}  // close namespace TestCase9
//=============================================================================
//                         CASE 8 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace TestCase8
{

enum {
    K              = 1024,
    MAX_TOTAL_SIZE = 32 * K,
    NUM_THREADS    = 3,              // Don't change it!
    NUM_ITERATIONS = 1000
};

bslma::Allocator *alloc = bslma::Default::defaultAllocator();
ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

extern "C" {
void *pushBackPopThread(void *arg)
{
    for (int i=0; i < NUM_ITERATIONS; ++i) {
        Handle& h = *((Handle *)arg);
        rb.pushBack(h);
        int len = rb.length();
        LOOP_ASSERT(len, len >= 1);
        LOOP_ASSERT(len, len <= NUM_THREADS);
        rb.popBack();
        len = rb.length();
        LOOP_ASSERT(len, len >= 0);
        LOOP_ASSERT(len, len <= NUM_THREADS-1);
    }
    return NULL;
}

void *pushBackRemoveAllThread(void *arg)
{
    for (int i=0; i < NUM_ITERATIONS; ++i) {
        Handle& h = *((Handle *)arg);
        rb.pushBack(h);
        int len = rb.length();
        LOOP_ASSERT(len, len >= 0);
        LOOP_ASSERT(len, len <= NUM_THREADS);
        rb.removeAll();
        len = rb.length();
        LOOP_ASSERT(len, len >= 0);
        LOOP_ASSERT(len, len <= NUM_THREADS-1);
    }
    return NULL;
}

} // extern "C"

}  // close namespace TestCase8
//=============================================================================
//                         CASE 7 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase7
{
}  // close namespace TestCase7

//=============================================================================
//                         CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase6
{
}  // close namespace TestCase6
//=============================================================================
//                         CASE 5 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase5
{
}  // close namespace TestCase5
//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase4
{
}  // close namespace TestCase4
//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase3
{
}  // close namespace TestCase3
//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase2
{
}  // close namespace TestCase2
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase1
{
}  // close namespace TestCase1
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    // veryVeryVerbose = argc > 4;  // not used

    switch (test) { case 0:  // Zero is always the leading case.

      case 15: {
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
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        using namespace TestCase15;

        executeInParallel(NUM_THREADS, workerThread15);

      } break;

      case 14: {
        // --------------------------------------------------------------------
        // STRESS TEST:
        //
        // Concerns:
        //   That record buffer is in good state after pushing large number of
        //   records.
        //
        // Plan:
        //   Create a record buffer, 'pushFront' a large number of records
        //   and verify the state (specifically after pushing all the
        //   records, the buffer should not be empty).  Repeat above with
        //   'pushBack'.
        //
        // Testing:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "STRESS TEST" << endl
                          << "===========" << endl;

        using namespace TestCase14;

        enum {
            K              = 1024,     // kilo byte
            MAX_TOTAL_SIZE = 32 * K,
            NUM_ITERATIONS = 10000     // number of records pushed
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R =  buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY",
                                      ball::Severity::e_WARN,
                                      "MESSAGE",
                                      alloc);
        Handle H(R, alloc, alloc);

        // stress test for 'pushFront'
        // --------------------------
        if (verbose) cout << endl
                          << "STRESS TEST FOR 'pushFront'" << endl
                          << "===========================" << endl;

        for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
            rb.pushFront(H);
            if (verbose) {
                P_(iter); P(rb.length());
            }
        }

        if (verbose) {
            P(rb.length());
        }

        LOOP_ASSERT(rb.length(), rb.length() > 0);

        // stress test for 'pushBack'
        // --------------------------
        if (verbose) cout << endl
                          << "STRESS TEST FOR 'pushBack'" << endl
                          << "==========================" << endl;

        ball::FixedSizeRecordBuffer rb1(MAX_TOTAL_SIZE, alloc);

        for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
            rb1.pushBack(H);
            if (verbose) {
                P_(iter); P(rb1.length());
            }
        }

        if (verbose) {
            P(rb1.length());
        }

        LOOP_ASSERT(rb1.length(), rb1.length() > 0);

      } break;

      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'pushFront':
        //   Verify 'pushFront' without considering the effect of
        //   'maxTotalSize'.
        //
        // Concerns:
        //   That 'pushFront' works correctly in variety of white-box states.
        //   In this test case.
        //
        // Plan:
        //   Create a record buffer.
        //
        //   Invoke 'pushFront' on the empty buffer and verify the state.
        //
        //   Invoke 'pushFront' on the buffer while it has 1 record in it and
        //   verify the state.
        //
        //   Invoke 'pushFront' on the buffer while it has 2 record in it and
        //   verify the state.
        //
        // Testing:
        //   int pushFront(const bsl::shared_ptr<ball::Record>& handle);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'pushFront'" << endl
                          << "===================" << endl;

        using namespace TestCase13;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        if (verbose)  cout << "\tpushing front of an empty buffer" << endl;
        ASSERT(0 == rb.pushFront(H1));
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R1);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R1==>\n\t\t" << *R1 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushFront(H2));
        if (verbose)  cout << "\tpushing front of an buffer with 1 record"
                           << endl;
        ASSERT(0 == rb.pushFront(H3));
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R3);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R3==>\n\t\t" << *R3 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R2);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R2==>\n\t\t" << *R2 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushFront(H4));
        ASSERT(0 == rb.pushFront(H5));
        if (verbose)  cout << "\tpushing front of an buffer with 2 record"
                           << endl;
        ASSERT(0 == rb.pushFront(H6));
        LOOP_ASSERT(rb.length(), rb.length() == 3);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R6);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R6==>\n\t\t" << *R6 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R5);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R5==>\n\t\t" << *R5 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R4);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R4==>\n\t\t" << *R4 << endl;

        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'popFront':
        //   Verify 'popFront'.
        //
        // Concerns:
        //   That 'popFront' works correctly in variety of white-box states.
        //
        // Plan:
        //   Ensure that 'popFront' works correctly when the record buffer
        //   has 1, 2 and 3 records.  This is achieved by following.
        //
        //   Create a record buffer, push back 1 record, invoke 'popFront' and
        //   verify the state.
        //
        //   Remove all the records from the record buffer, push back 2
        //   records, invoke 'popFront' and verify the state.
        //
        //   Remove all the records from the record buffer, push back 3
        //   records, invoke 'popFront' and verify the state.
        //
        // Testing:
        //   void popFront();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'popFront'" << endl
                          << "==================" << endl;

        using namespace TestCase12;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        ASSERT(0 == rb.pushBack(H1));

        if (verbose)
            cout << "\tinvoking 'popFront' on a buffer with one record"
                 << endl;
        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        rb.removeAll();
        ASSERT(0 == rb.pushBack(H2));
        ASSERT(0 == rb.pushBack(H3));

        if (verbose)
            cout << "\tinvoking 'popFront' on a buffer with two records"
                 << endl;
        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R3);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R3==>\n\t\t" << *R3 << endl;

        rb.removeAll();
        ASSERT(0 == rb.pushBack(H4));
        ASSERT(0 == rb.pushBack(H5));
        ASSERT(0 == rb.pushBack(H6));

        if (verbose)
            cout << "\tinvoking 'popFront' on a buffer with three records"
                 << endl;
        rb.popFront();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.front() == *R5);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R5==>\n\t\t" << *R5 << endl;
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'FRONT':
        //   Verify 'front'.
        //
        // Concerns:
        //   That 'front' works correctly in variety of white-box states.
        //
        // Plan:
        //   Ensure that 'front' works correctly when the record buffer has 1,
        //   2 and 3 records.  This is achieved by pushing back 3 records and
        //   invoking 'front' after each push back.
        //
        // Testing:
        //   const ball::Record& front() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'FRONT'" << endl
                          << "==============" << endl;

        using namespace TestCase11;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);

        ASSERT(0 == rb.pushBack(H1));
        if (verbose)
            cout << "\tinvoking 'front' on a buffer with one record"
                 << endl;
        ASSERT(*rb.front() == *R1);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R1==>\n\t\t" << *R1 << endl;

        ASSERT(0 == rb.pushBack(H2));
        if (verbose)
            cout << "\tinvoking 'front' on a buffer with two record" << endl;
        ASSERT(*rb.front() == *R1);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R1==>\n\t\t" << *R1 << endl;

        ASSERT(0 == rb.pushBack(H3));
        if (verbose)
            cout << "\tinvoking 'front' on a buffer with three record"
                 << endl;
        ASSERT(*rb.front() == *R1);
        if (veryVerbose) cout << "\t*rb.front()==>\n\t\t" << *rb.front()
                              << "\n\n"
                              <<  "\t*R1==>\n\t\t" << *R1 << endl;

      } break;

      case 10: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-2 FOR 'BEGIN_SEQUENCE' AND 'END_SEQUENCE':
        //   Verify 'beginSequence' and 'endSequence'.
        //
        // Concerns:
        //   That a thread gets exclusive access to the buffer
        //   between the calls to 'beginSequence' and 'endSequence'.
        //
        // Plan:
        //   Create 2 threads.  The first thread runs a loop; in each
        //   iteration, it first pushes back 5 records and then removes all of
        //   them (by invoking 'removeAll' in even numbered iterations and
        //   by invoking 'popBack' five times in odd ones).  The second thread
        //   runs a loop; in each iteration, it invokes 'beginSequence',
        //   saves record buffer length in a local variable, sleeps for a
        //   while (thus allowing first thread to run), verify that record
        //   buffer length is unchanged and then invoke 'endSequence'.
        //
        // Testing:
        //   void beginSequence();
        //   void endSequence();
        // --------------------------------------------------------------------
        if (verbose) cout
          << endl
          << "CONCURRENCY TEST-2 FOR 'BEGIN_SEQUENCE' AND 'END_SEQUENCE'"
          << endl
          << "=========================================================="
          << endl;

        using namespace TestCase10;

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY",
                                      ball::Severity::e_WARN,
                                      "MESSAGE",
                                      alloc);

        Handle H(R, alloc, alloc);

        bslmt::ThreadUtil::Handle t1, t2;
        bslmt::ThreadUtil::create(&t1,
                                 beginSequenceEndSequenceThread,
                                 NULL);
        bslmt::ThreadUtil::create(&t2,
                                 pushBackPopRemoveAllThread,
                                 (void *)(&H));

        bslmt::ThreadUtil::join(t1);
        bslmt::ThreadUtil::join(t2);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST-1 FOR 'BEGIN_SEQUENCE' AND 'END_SEQUENCE':
        //   Verify 'beginSequence' and 'endSequence'.
        //
        // Concerns:
        //   That a thread gets exclusive access to the buffer
        //   between the calls to 'beginSequence' and 'endSequence'.
        //
        // Plan:
        //   Initialize a global variable 'state' with 'VALID'.  Create
        //   'NUM_THREADS' (sequentially numbered from 0 to NUM_THREADS)
        //   threads.  Each thread invokes 'beginSequence' and then verifies
        //   that the invariant 'state == VALID' is true, makes the
        //   invariant false by setting the 'state' to 'INVALID', sleeps for
        //   a while (to allow other threads to run), sets the 'state' to
        //   'VALID', invoke 'endSequence' and returns.  Finally, join all
        //   the threads and verity that the invariant 'state == VALID' is
        //   still true.
        //
        // Testing:
        //   void beginSequence();
        //   void endSequence();
        // --------------------------------------------------------------------
        if (verbose) cout
          << endl
          << "CONCURRENCY TEST-1 FOR 'BEGIN_SEQUENCE' AND 'END_SEQUENCE'"
          << endl
          << "=========================================================="
          << endl;

        using namespace TestCase9;

        executeInParallel(NUM_THREADS, workerThread9);
        ASSERT(state == VALID);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST FOR 'PUSHBACK', 'POPBACK', 'REMOVE_ALL', 'LENGTH':
        //   Verify the concurrent access by 'pushBack', 'popBack', 'removeAll'
        //   and 'length'.
        //
        // Concerns:
        //   That concurrent manipulations of record buffer (by 'pushBack',
        //   'popBack' and 'removeAll') do not introduce inconsistencies.
        //
        // Plan:
        //   Create a record buffer.  Create 'NUM_THREADS' threads, each
        //   of which runs a loop.  Each thread, in every iteration, push
        //   back a record, verify the state using 'length', removes the record
        //   using 'popBack' and verify the state using 'length'.  Finally join
        //   all the threads and verify the state.
        //
        //   Repeat above, but this time invoke 'removeAll' instead of 1
        //   'popBack' to remove the records.
        //
        // Testing:
        //   void popBack();
        //   int pushBack(const bsl::shared_ptr<ball::Record>& handle);
        //   void removeAll();
        //   int length() const;
        // --------------------------------------------------------------------
        if (verbose) cout
         << endl
         << "CONCURRENCY TEST FOR 'PUSHBACK', 'POPBACK', 'REMOVE_ALL','LENGTH'"
         << endl
         << "================================================================="
         << endl;

        using namespace TestCase8;

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);

        Handle handles[] = {H1, H2, H3};

        executeInParallel(NUM_THREADS, pushBackPopThread, handles);
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        executeInParallel(NUM_THREADS, pushBackRemoveAllThread, handles);
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

      } break;

      case 7: {
        // TBD
        // --------------------------------------------------------------------
        // TESTING 'PUSHBACK' CONSIDERING THE EFFECT OF 'MAX_TOTAL_SIZE':
        //   Verify 'pushBack' considering the effect of 'maxTotalSize'.
        //
        // Concerns:
        //   That 'pushBack' works correctly in variety of white-box states.
        //   Specifically we consider the effect of 'maxTotalSize'.
        //
        // Plan:
        //
        // Testing:
        //   int pushBack(const bsl::shared_ptr<ball::Record>& handle);
        // --------------------------------------------------------------------
        if (verbose) cout
       << endl
       << "TESTING 'PUSHBACK' CONSIDERING THE EFFECT OF 'MAX_TOTAL_SIZE'"
       << endl
       << "============================================================="
       << endl;

        using namespace TestCase7;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'LENGTH':
        //   Verify 'length'.
        //
        // Concerns:
        //   That 'length' works correctly in variety of white-box states.
        //
        // Plan:
        //   Ensure that 'length' works correctly when the record buffer has
        //   0, 1, 2 and 3 records.  This is achieved by pushBacking 3
        //   records and invoking 'length' after each push back.
        //
        // Testing:
        //   int length() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'LENGTH'" << endl
                          << "================" << endl;

        using namespace TestCase6;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);

        if (verbose)
            cout << "\tinvoking 'length' on an empty buffer" << endl;
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H1));
        if (verbose)
            cout << "\tinvoking 'length' on a buffer with one record" << endl;
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H2));
        if (verbose)
            cout << "\tinvoking 'length' on a buffer with two record" << endl;
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H3));
        if (verbose)
            cout << "\tinvoking 'length' on a buffer with three record"
                 << endl;
        LOOP_ASSERT(rb.length(), rb.length() == 3);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'BACK':
        //   Verify 'back'.
        //
        // Concerns:
        //   That 'back' works correctly in variety of white-box states.
        //
        // Plan:
        //   Ensure that 'back' works correctly when the record buffer has 1,
        //   2 and 3 records.  This is achieved by pushing back 3 records and
        //   invoking 'back' after each push back.
        //
        // Testing:
        //   const bsl::shared_ptr<const ball::Record>& back() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'BACK'" << endl
                          << "==============" << endl;

        using namespace TestCase5;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);

        ASSERT(0 == rb.pushBack(H1));
        if (verbose)
            cout << "\tinvoking 'back' on a buffer with one record"
                 << endl;
        ASSERT(*rb.back() == *R1);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R1==>\n\t\t" << *R1 << endl;

        ASSERT(0 == rb.pushBack(H2));
        if (verbose)
            cout << "\tinvoking 'back' on a buffer with two record" << endl;
        ASSERT(*rb.back() == *R2);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R2==>\n\t\t" << *R2 << endl;

        ASSERT(0 == rb.pushBack(H3));
        if (verbose)
            cout << "\tinvoking 'back' on a buffer with three record"
                 << endl;
        ASSERT(*rb.back() == *R3);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R3==>\n\t\t" << *R3 << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE_ALL':
        //   Verify removeAll'.
        //
        // Concerns:
        //   That 'removeAll' works correctly in variety of white-box
        //   states.
        //
        // Plan:
        //   Ensure that 'removeAll' works correctly when the record buffer
        //   has 0, 1, 2 and 3 records.  This is achieved by following.
        //
        //   Create a record buffer, invoke 'removeAll' and verify the state.
        //
        //   Push back 1 records, invoke 'removeAll' and verify the state.
        //
        //   Push back 2 records, invoke 'removeAll' and verify the state.
        //
        //   Push back 3 records, invoke 'removeAll' and verify the state.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVE_ALL'" << endl
                          << "====================" << endl;

        using namespace TestCase4;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        if (verbose)
            cout << "\tinvoking 'removeAll' on a an empty buffer" << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H1));
        if (verbose)
            cout << "\tinvoking 'removeAll' on a buffer with one record"
                 << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H2));
        ASSERT(0 == rb.pushBack(H3));
        if (verbose)
            cout << "\tinvoking 'removeAll' on a buffer with two record"
                 << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H4));
        ASSERT(0 == rb.pushBack(H5));
        ASSERT(0 == rb.pushBack(H6));
        if (verbose)
            cout << "\tinvoking 'removeAll' on a buffer with three record"
                 << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'POPBACK':
        //   Verify 'popBack'.
        //
        // Concerns:
        //   That 'popBack' works correctly in variety of white-box states.
        //
        // Plan:
        //   Ensure that 'popBack' works correctly when the record buffer
        //   has 1, 2 and 3 records.  This is achieved by following.
        //
        //   Create a record buffer, push back 1 record, invoke 'popBack' and
        //   verify the state.
        //
        //   Remove all the records from the record buffer, push back 2
        //   records, invoke 'popBack' and verify the state.
        //
        //   Remove all the records from the record buffer, push back 3
        //   records, invoke 'popBack' and verify the state.
        //
        // Testing:
        //   void popBack();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'POPBACK'" << endl
                          << "=================" << endl;

        using namespace TestCase3;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        ASSERT(0 == rb.pushBack(H1));

        if (verbose)
            cout << "\tinvoking 'popBack' on a buffer with one record"
                 << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        rb.removeAll();
        ASSERT(0 == rb.pushBack(H2));
        ASSERT(0 == rb.pushBack(H3));

        if (verbose)
            cout << "\tinvoking 'popBack' on a buffer with two records"
                 << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R2);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R2==>\n\t\t" << *R2 << endl;

        rb.removeAll();
        ASSERT(0 == rb.pushBack(H4));
        ASSERT(0 == rb.pushBack(H5));
        ASSERT(0 == rb.pushBack(H6));

        if (verbose)
            cout << "\tinvoking 'popBack' on a buffer with three records"
                 << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R5);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R5==>\n\t\t" << *R5 << endl;
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'PUSHBACK':
        //   Verify 'pushBack' without considering the effect of
        //   'maxTotalSize'.
        //
        // Concerns:
        //   That 'pushBack' works correctly in variety of white-box states.
        //   In this testcase, we do not consider the
        //   effect of 'maxTotalSize' parameter i.e., we always push back the
        //   record successfully.
        //
        // Plan:
        //   Create a record buffer.
        //
        //   Invoke push back on the empty buffer and verify the state.
        //
        //   Invoke push back on the buffer while it has 1 record in it and
        //   verify the state.
        //
        //   Invoke push back on the buffer while it has 2 record in it and
        //   verify the state.
        //
        // Testing:
        //   int pushBack(const bsl::shared_ptr<ball::Record>& handle);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING PUSHBACK" << endl
                          << "============" << endl;

        using namespace TestCase2;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        if (verbose)  cout << "\tpushing back in an empty buffer" << endl;
        ASSERT(0 == rb.pushBack(H1));
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R1);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     <<  "\t*R1==>\n\t\t" << *R1 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H2));
        if (verbose)  cout << "\tpushing back in an buffer with 1 record"
                           << endl;
        ASSERT(0 == rb.pushBack(H3));
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R3);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     <<  "\t*R3==>\n\t\t" << *R3 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R2);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R2==>\n\t\t" << *R2 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        ASSERT(0 == rb.pushBack(H4));
        ASSERT(0 == rb.pushBack(H5));
        if (verbose)  cout << "\tpushing back in an buffer with 2 record"
                           << endl;
        ASSERT(0 == rb.pushBack(H6));
        LOOP_ASSERT(rb.length(), rb.length() == 3);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R6);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R6==>\n\t\t" << *R6 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R5);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R5==>\n\t\t" << *R5 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R4);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R4==>\n\t\t" << *R4 << endl;

        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

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
        //   Create a record buffer, push back 3 records, verify the state,
        //   invoke 'popBack' two times verifying the state after each
        //   invocation, invoke 'removeAll' and verify the state.
        //
        //   Repeat above, but this time all the code is executed between
        //   the invocation to 'beginSequence' and 'endSequence'.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace TestCase1;

        enum {
                K              = 1024,
                MAX_TOTAL_SIZE = 32 * K
        };

        bslma::Allocator *alloc = bslma::Default::defaultAllocator();
        ball::FixedSizeRecordBuffer rb(MAX_TOTAL_SIZE, alloc);

        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        int pid = 1, tid = 2;
        ball::Record *R1 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-1",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-1",
                                      alloc);

        ball::Record *R2 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-2",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-2",
                                      alloc);

        ball::Record *R3 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-3",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-3",
                                      alloc);

        ball::Record *R4 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-4",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-4",
                                      alloc);

        ball::Record *R5 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-5",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-5",
                                      alloc);

        ball::Record *R6 = buildRecord(now,
                                      pid,
                                      tid,
                                      __FILE__,
                                      __LINE__,
                                      "CATEGORY-6",
                                      ball::Severity::e_WARN,
                                      "MESSAGE-6",
                                      alloc);

        Handle H1(R1, alloc, alloc);
        Handle H2(R2, alloc, alloc);
        Handle H3(R3, alloc, alloc);
        Handle H4(R4, alloc, alloc);
        Handle H5(R5, alloc, alloc);
        Handle H6(R6, alloc, alloc);

        if (verbose) cout << "\trb.pushBack(H1)" << endl;
        ASSERT(0 == rb.pushBack(H1));

        if (verbose) cout << "\trb.pushBack(H2)" << endl;
        ASSERT(0 == rb.pushBack(H2));

        if (verbose) cout << "\trb.pushBack(H3)" << endl;
        ASSERT(0 == rb.pushBack(H3));
        LOOP_ASSERT(rb.length(), rb.length() == 3);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R3);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R3==>\n\t\t" << *R3 << endl;

        if (verbose) cout << "\trb.popBack()" << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R2);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R2==>\n\t\t" << *R2 << endl;

        if (verbose) cout << "\trb.popBack()" << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R1);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     << "\t*R1==>\n\t\t" << *R1 << endl;

        if (verbose) cout << "\trb.removeAll()" << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;

        if (verbose) cout << "\n" << endl;

        if (verbose) cout << "\trb.startSequence()" << endl;
        rb.beginSequence();
        if (verbose) cout << "\trb.pushBack(H4)" << endl;
        ASSERT(0 == rb.pushBack(H4));
        if (verbose) cout << "\trb.pushBack(H5)" << endl;
        ASSERT(0 == rb.pushBack(H5));
        if (verbose) cout << "\trb.pushBack(H6)" << endl;

        ASSERT(0 == rb.pushBack(H6));
        LOOP_ASSERT(rb.length(), rb.length() == 3);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R6);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     <<  "\t*R6==>\n\t\t" << *R6 << endl;

        if (verbose) cout << "\trb.popBack()" << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 2);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R5);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     <<  "\t*R5==>\n\t\t" << *R5 << endl;

        if (verbose) cout << "\trb.popBack()" << endl;
        rb.popBack();
        LOOP_ASSERT(rb.length(), rb.length() == 1);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        ASSERT(*rb.back() == *R4);
        if (veryVerbose)
                cout << "\t*rb.back()==>\n\t\t" << *rb.back() << "\n\n"
                     <<  "\t*R4==>\n\t\t" << *R4 << endl;

        if (verbose) cout << "\trb.removeAll()" << endl;
        rb.removeAll();
        LOOP_ASSERT(rb.length(), rb.length() == 0);
        if (veryVerbose) cout << "\trb.length() = " << rb.length() << endl;
        if (verbose) cout << "\trb.endSequence()" << endl;
        rb.endSequence();
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
