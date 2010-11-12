// baecs_journal.t.cpp -*-C++-*-
// The _CRT_RAND_S macro MUST be defined on Windows *PRIOR* to inclusion of
// stdlib.h in order to defined 'rand_s'.
#include <bsls_platform.h>
#ifdef BSLS_PLATFORM__OS_WINDOWS
// Remembering to define _CRT_RAND_S prior to inclusion statement
#define _CRT_RAND_S
#endif

#include <baecs_journal.h>
#include <baecs_mappingmanager.h>
#include <bael_defaultobserver.h>
#include <bael_severity.h>
#include <bael_loggermanager.h>

#include <bcec_fixedqueue.h>
#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>
#include <bcema_sharedptr.h>
#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_lockguard.h>
#include <bcemt_semaphore.h>
#include <bcemt_threadgroup.h>
#include <bcemt_thread.h>
#include <bcep_timereventscheduler.h>

#include <bdef_bind.h>
#include <bdesu_processutil.h>
#include <bdetu_systemtime.h>

#include <bsls_assert.h>
#include <bsls_platformutil.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>

#include <bsl_iostream.h>
#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

#include <bsl_cstdio.h>   // tmpnam
#include <bsl_c_math.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

typedef baecs_Journal Obj;
typedef Obj::RecordHandle H;

//=============================================================================
//                        STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;
static int verbose, veryVerbose, veryVeryVerbose;

static bcemt_Mutex g_assertLock;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bcemt_LockGuard<bcemt_Mutex> guard(&g_assertLock);
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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

#if defined(BSLS_ASSERT) && !defined(BDE_BUILD_TARGET_OPT)
#define BAECS_JOURNAL_PARANOID
#endif

#ifndef L_tmpnam
#define L_tmpnam 128
#endif

#define MAX_TMPFILENAME (L_tmpnam+64)

#if defined(BSLS_PLATFORM__OS_AIX) || defined(BSLS_PLATFORM__OS_HPUX)
#define tmpnam_r tmpnam
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
#define snprintf _snprintf
#define rand_r   rand_s
#define tmpnam_r tmpnam_s
#endif

namespace {

enum {
    MODE_RW = baecs_Journal::BAECS_READWRITE,
    MODE_RW_P = MODE_RW | baecs_Journal::BAECS_PARANOID,
    MODE_RO = baecs_Journal::BAECS_READONLY,
    MODE_RW_SAFE = MODE_RW | baecs_Journal::BAECS_SAFE,
    MODE_RW_AUTOCOMMIT = MODE_RW_SAFE | baecs_Journal::BAECS_AUTO_COMMIT
};

enum {
    NUM_PRIO = baecs_Journal::BAECS_NUM_PRIORITIES
};

void commitJournal(baecs_Journal* journal) {
   if (veryVerbose) {
      cout << "calling commit()..." << endl;
   }
   journal->commit();
}

void stampBuffer(int* buffer, int length, int value) {
   for (int i = 0; i < length; ++i) {
      buffer[i] = value;
   }
}

int verifyBuffer(int* buffer, int length, int value) {
   int rc = 0;
   for (int i = 0; i < length; ++i) {
      rc += (buffer[i] != value);

//  debug
//      if (buffer[i] != value) {
//         cout << "DEBUG: " << buffer[i] << "(" << i << ") != "
//              << value << endl;
//      }
   }
   return rc;
}

int genericRand(int n) {
   int retval = (int)(n * (double)rand() / (((double)RAND_MAX)+1));

   return min(n-1, retval);
}

inline unsigned fastrand() {
    static unsigned v = (unsigned)time(0);
    return v = v*1664525 + 1013904223;
}

inline unsigned fastrand(unsigned n) {
    return fastrand()%n;
}

void getTmpFileName(char* filename, int testCase) {
   char randLetters[11];
   for (int i = 0; i < 10; ++i) {
      randLetters[i] = genericRand(26) + 'A';
   }
   randLetters[10] = 0;

   const char* envPrefix = getenv("BDE_TEST_TEMPFILE_PREFIX");
   if (envPrefix) {
       snprintf(filename, MAX_TMPFILENAME,
                "%s%s%d",
                envPrefix, randLetters, bdesu_ProcessUtil::getProcessId());
   } else {
#ifdef BSLS_PLATFORM__OS_WINDOWS
       const char *tmpdirName = getenv("TMP");
#else
       const char* tmpdirName = getenv("TMPDIR");
#endif
       if (0 == tmpdirName) {
          tmpdirName = "/bb/data/tmp";
       }

       snprintf(filename, MAX_TMPFILENAME,
                "%s/baecs_journal__%02d%s%d",
                tmpdirName,
                testCase,
                randLetters,
                bdesu_ProcessUtil::getProcessId());
   }
   filename[MAX_TMPFILENAME-1] = 0;
}

class CaseNeg3TestPlan {
   struct PlanStep {
      enum {ADD, REMOVE} d_action;
      int                d_bytesOrPosition;
   };
   vector<PlanStep> d_plan;
   int d_numRecords;
   enum {MAX_RECORD = 10000};

public:

   CaseNeg3TestPlan() : d_numRecords(0) {}

   void appendAddOperation(int numBytes) {
      PlanStep step;
      step.d_action = PlanStep::ADD;
      step.d_bytesOrPosition = numBytes;
      ++d_numRecords;
      d_plan.push_back(step);
   }

   void appendRemoveOperation(int position) {

      //TEST INVARIANT
      BSLS_ASSERT(0 < d_numRecords);

      PlanStep step;
      step.d_action = PlanStep::REMOVE;
      step.d_bytesOrPosition = position;
      --d_numRecords;
      d_plan.push_back(step);
   }

   void generate(int numSteps, bool suppressRemovals = false) {
      d_numRecords = 0;

      d_plan.reserve(numSteps);
      for (int i = 0; i < numSteps; ++i) {
         PlanStep step;
         if (suppressRemovals || 0 == d_numRecords || rand() > RAND_MAX/2) {
            step.d_action = PlanStep::ADD;
            step.d_bytesOrPosition = genericRand(MAX_RECORD) + 1;
            ++d_numRecords;
         }
         else {
            // This is a TEST INVARIANT
            BSLS_ASSERT(0 < d_numRecords);

            step.d_action = PlanStep::REMOVE;
            step.d_bytesOrPosition = genericRand(d_numRecords);
            --d_numRecords;
         }
         d_plan.push_back(step);
      }
   }

   int numFinalRecords() { return d_numRecords; }

   void execute(Obj* journal) {
      H h;
      vector<H> handles;
      char record[MAX_RECORD+1];
      for (vector<PlanStep>::const_iterator i = d_plan.begin();
           i != d_plan.end(); ++i) {
         if (PlanStep::ADD == i->d_action) {
            if (veryVeryVerbose) {
               cout << "Adding " << i->d_bytesOrPosition << endl;
            }
            ASSERT(0 == journal->addRecord(&h, (void*)record,
                                           i->d_bytesOrPosition));
            handles.push_back(h);
         }
         else {
            if (veryVeryVerbose) {
               cout << "Removing " << i->d_bytesOrPosition << endl;
            }
            vector<H>::iterator it = handles.begin() + i->d_bytesOrPosition;
            ASSERT(0 == journal->removeRecord(*it));
            handles.erase(it);
         }
      }
   }
};

class CaseNeg3 {

   Obj*             d_journal;
   CaseNeg3TestPlan* d_plan;
   volatile double* d_elapsedSec;

public:

   CaseNeg3(Obj* journal, CaseNeg3TestPlan* plan,
            volatile double* elapsedSec)
      : d_journal(journal), d_plan(plan), d_elapsedSec(elapsedSec) {}

   void operator()() {
      bdet_TimeInterval start, stop;
      bsls_PlatformUtil::Int64 elapsed;
      enum {MICROSECS_PER_SEC     = 1000000};

      bdetu_SystemTime::loadSystemTimeDefault(&start);
      d_plan->execute(d_journal);
      bdetu_SystemTime::loadSystemTimeDefault(&stop);
      elapsed = (stop - start).totalMicroseconds();

      *d_elapsedSec = elapsed / (double)MICROSECS_PER_SEC;
   }
};

struct Case4Data {

   enum { NUM_CONFIRM_ATTEMPTS = 45 };

   bcemt_Barrier  d_bp;
   bcemt_Barrier  d_bs;
   bcemt_Barrier  d_bt;
   Obj           *d_journal;
   vector<H>      d_handles;
   volatile int   d_handlesSize;
   bsl::set<H>    d_confirmed;
   bcemt_Mutex    d_removalLock;
   volatile int   d_confirmerRunning;
   H              d_confirming;

   explicit Case4Data(Obj *j)
      : d_bp(2), d_bs(2), d_bt(2), d_journal(j), d_confirming(0),
        d_handlesSize(0)
   {}

   // seeds for rand_r
   static unsigned int confirmerSeed;
   static unsigned int mainSeed;

   static int confirmerRand(int n);
   static int mainRand(int n);

   void confirmerThread();
   void stopConfirmer() {
      d_confirmerRunning = 0;
   }
};

void randBuffer(char* &buffer, int size, int allocSize=-1) {
   if (-1 == allocSize || allocSize <= size) {
      allocSize = size+1;
   }

   if (0 != allocSize) {
      buffer = (char*)malloc(allocSize);
   }

   for (int i = 0; i < size; ++i) {
      buffer[i] = genericRand(26) + 'A';
   };
   buffer[size] = 0;
}

void randBuffers(char** buffers, int numBuffers, int size) {
   for (int i = 0; i < numBuffers; ++i) {
      randBuffer(buffers[i], size);
   }
}

void freeBuffers(char** buffers, int numBuffers)
{
   for (int i = 0; i < numBuffers; ++i) {
      free(buffers[i]);
   }
}

void case3AllocateBuffers(char *buf1[2], char *buf2[2], char *buf3[2],
                          int minsize)
{
   static const int multipliers[] = {8, 12, 2, 3, 1, 4};
   char** buffers[] = {&buf1[0], &buf1[1],
                       &buf2[0], &buf2[1],
                       &buf3[0], &buf3[1]};

   for (int i = 0; i < 6; ++i) {
      randBuffer(*(buffers[i]), minsize, minsize * multipliers[i]);
   }
}

unsigned int Case4Data::confirmerSeed = 0;
unsigned int Case4Data::mainSeed = 0;

void
Case4Data::confirmerThread() {
   ASSERT(d_journal);
   char myBuffer[100];

   d_confirmerRunning = 1;
   d_bs.wait();
   while (d_confirmerRunning) {
      for (int i = 0; i < NUM_CONFIRM_ATTEMPTS; ++i) {
         bcemt_LockGuard<bcemt_Mutex> guard(&d_removalLock);
         if (d_handlesSize > 1) {
            int size = d_handlesSize - 1;
            d_confirming = d_handles[confirmerRand(size)];
            if (d_confirmed.end() != d_confirmed.find(d_confirming)) {
               ASSERT(d_journal->isConfirmedRecord(d_confirming));
               ASSERT(!d_journal->isUnconfirmedRecord(d_confirming));
               //try one time to get an unconfirmed record
               d_confirming = d_handles[confirmerRand(size)];
               if (d_confirmed.end() != d_confirmed.find(d_confirming)) {
                  continue;
               }
            }
            ASSERT(!d_journal->isConfirmedRecord(d_confirming));
            ASSERT(d_journal->isUnconfirmedRecord(d_confirming));
         }
         else {
            bcemt_ThreadUtil::yield();
            continue;
         }

         d_journal->copyRecordData(myBuffer, 100, d_confirming);
         ASSERT(0 == strcmp(myBuffer, "Here is some data of length 31"));
         d_journal->confirmRecord(d_confirming);
         d_confirmed.insert(d_confirming);
         d_confirming = 0;
      }
      d_bp.wait();
      d_bt.wait();
   }
}

int Case4Data::confirmerRand(int n) {
   if (0 == confirmerSeed) {
      confirmerSeed = time(NULL);
   }

   // bias to higher-order bits which tend to be more random
   int retval = (int)floor(n * ((double)rand_r(&confirmerSeed) /
                                (((double)RAND_MAX) + 1)));

   // test invariant:
   BSLS_ASSERT(retval < n);

   return retval;
}

int Case4Data::mainRand(int n) {
   if (0 == mainSeed) {
      mainSeed = time(NULL);
   }
   // bias to higher-order bits which tend to be more random
   int retval = (int)floor(n * ((double)rand_r(&mainSeed) /
                                (((double)RAND_MAX) + 1)));

   // test invariant:
   BSLS_ASSERT(retval < n);

   return retval;
}

int verifyBlob(bcema_Blob *blob, const char* data) {
   ostringstream oss;
   bcema_BlobUtil::asciiDump(oss, *blob);

   int rc = strcmp(data, oss.str().c_str());

   // debug
   //if (rc != 0) {
   //   cout << "DEBUG: \"" << data << "\" != \""
   //        << oss.str().c_str() << "\"" << endl;
   //}

   return rc;
}

int case5(Obj* mX, const char* data) {
   H handle = 0;
   int prevStatus = testStatus;

   bcema_Blob *b1 = new bcema_Blob, *b2 = new bcema_Blob;

   if (veryVerbose) {
      cout << "Checking getRecordData..." << endl;
   }
   int datalen = strlen(data)+1;
   ASSERT(0 == mX->addRecord(&handle, (void*)data, datalen));
   ASSERT(datalen == mX->getRecordData(b1, handle));
   ASSERT(datalen == mX->getRecordData(b2, handle));

   // verify both blobs, then destroy one
   ASSERT(0 != b1->length());
   ASSERT(0 == verifyBlob(b1, data));
   ASSERT(0 != b2->length());
   ASSERT(0 == verifyBlob(b2, data));

   delete b1;
   b1 = 0;
   ASSERT(0 != b2->length());
   ASSERT(0 == verifyBlob(b2, data));

   {
      bcema_Blob autoBlob; // auto as in "automatic variable"
      // make a third blob; verify it and re-verify the previous one
      ASSERT(datalen == mX->getRecordData(&autoBlob, handle));
      ASSERT(0 != autoBlob.length());
      ASSERT(0 == verifyBlob(&autoBlob, data));
      ASSERT(0 == verifyBlob(b2, data));
   }

   if (veryVerbose) {
      cout << "Checking copyRecordData..." << endl;
   }

   // Verify that copyRecordData works for the same record
   char* buffer = new char[datalen];
   ASSERT(datalen == mX->copyRecordData(buffer, datalen, handle));
   ASSERT(0 == strcmp(data, buffer));

   // Destroy the second blob; remove and reuse the record
   delete b2;

   ASSERT(0 == mX->removeRecord(handle));

   const char* mydata = "mydata";
   int mydatalen = strlen(mydata)+1;
   // The following line is a TEST INVARIANT
   BSLS_ASSERT(mydatalen <= datalen);

   // Test reuse of the handle
   ASSERT(0 == mX->addRecord(&handle, (void*)mydata, mydatalen));
   b1 = new bcema_Blob;
   ASSERT(mydatalen == mX->getRecordData(b1, handle));
   ASSERT(0 != b1->length());
   ASSERT(0 == verifyBlob(b1, mydata));

   delete b1;
   delete[] buffer;
   return testStatus == prevStatus ? 0 : -1;
}

#ifdef BAECS_JOURNAL_PARANOID
unsigned int
myAdler(unsigned char *data, size_t len)
{
    enum {MOD_ADLER = 65521};
    unsigned int a = 1, b = 0;

    while (len)
    {
        size_t tlen = len > 5550 ? 5550 : len;
        len -= tlen;
        do
        {
            a += *data++;
            b += a;
        } while (--tlen);

        a = (a & 0xffff) + (a >> 16) * (65536-MOD_ADLER);
        b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);

    }

    /* It can be shown that a <= 0x1013a here, so a single subtract will do.*/
    if (a >= MOD_ADLER)
            a -= MOD_ADLER;

    /* It can be shown that b can reach 0xffef1 here.*/
    b = (b & 0xffff) + (b >> 16) * (65536-MOD_ADLER);

    if (b >= MOD_ADLER)
            b -= MOD_ADLER;

    return (b << 16) | a;

}
#endif

class BaseJournal
{
public:
    virtual int addRecord(int *handle, char* data, int size) = 0;
    virtual int removeRecord(int handle) = 0;
    virtual int readRecord(char* buf, int size, int record) = 0;
    virtual void setMappingLimit(int megs) = 0;
    virtual void printStats(bsls_PlatformUtil::Int64 totalDataSize) = 0;
    virtual void reopen() = 0;
    virtual ~BaseJournal();
};

BaseJournal::~BaseJournal()
{
}

// TBD bcecs_persistentjournal is obsolete
#if 0
class OldJournalWrapper : public BaseJournal
{
    bcecs_PersistentJournal     d_journal;
    bsl::string                 d_filename;
public:
    OldJournalWrapper(const char* filename);

    virtual int addRecord(int *handle, char* data, int size);
    virtual int removeRecord(int handle);
    virtual int readRecord(char* buf, int size, int record);
    virtual void setMappingLimit(int megs);
    virtual void printStats(bsls_PlatformUtil::Int64 totalDataSize);
    virtual void reopen();
    virtual ~OldJournalWrapper();
};

void OldJournalWrapper::reopen()
{
    d_journal.close();
    int rc = d_journal.openJournal(d_filename.c_str(),
            bcecs_PersistentJournal::READ_WRITE);
    ASSERT(rc == 0);
}

OldJournalWrapper::OldJournalWrapper(const char* filename)
: d_filename(filename)
{
    int rc = d_journal.openJournal(d_filename.c_str(),
            bcecs_PersistentJournal::READ_WRITE);
    ASSERT(rc == 0);
}

int OldJournalWrapper::addRecord(int *handle, char* data, int size)
{
    return d_journal.addRecord(handle, data, size);
}

int OldJournalWrapper::removeRecord(int handle)
{
    return d_journal.removeRecord(handle);
}

int OldJournalWrapper::readRecord(char* buf, int size, int record)
{
    bcecs_PersistentJournalIterator it(&d_journal, record);
    return it.data(buf, size);
}

void OldJournalWrapper::setMappingLimit(int megs)
{
    d_journal.setNumMappedPagesLimit(megs);
}

OldJournalWrapper::~OldJournalWrapper()
{
    d_journal.close();
}

void OldJournalWrapper::printStats(bsls_PlatformUtil::Int64 totalDataSize)
{

}
#endif

class NewJournalWrapper : public BaseJournal {
    baecs_MappingManager                         d_mappingManager;
    baecs_Journal                                d_journal;
    bsl::string                                  d_filename;
    int                                          d_mode;
    enum {
        MODE = baecs_Journal::BAECS_READWRITE | baecs_Journal::BAECS_SAFE
    };
public:
    NewJournalWrapper(const char* filename, int mode = -1);

    virtual int addRecord(int *handle, char* data, int size);
    virtual int removeRecord(int handle);
    virtual int readRecord(char* buf, int size, int record);
    virtual void setMappingLimit(int megs);
    virtual void printStats(bsls_PlatformUtil::Int64 totalDataSize);
    virtual void reopen();
    virtual ~NewJournalWrapper();
};

void NewJournalWrapper::reopen()
{
    d_journal.close();
    int rc = d_journal.open(d_filename.c_str(), d_mode);
    ASSERT(rc == 0);
}

NewJournalWrapper::NewJournalWrapper(
    const char* filename, int mode)
: d_mappingManager(1024*1024*10, 2) // 10Mb by default
, d_journal(&d_mappingManager)
, d_filename(filename)
, d_mode(mode>=0 ? mode : MODE)
{
    bdesu_FileUtil::remove(d_filename.c_str());
    int rc = d_journal.create(d_filename.c_str(), d_mode);
    ASSERT(rc == 0);
}

NewJournalWrapper::~NewJournalWrapper()
{
    d_journal.close();
}

int NewJournalWrapper::addRecord(int *handle, char* data, int size) {
    return d_journal.addRecord((unsigned int*)handle, data, size);
}

int NewJournalWrapper::removeRecord(int handle) {
    return d_journal.removeRecord(handle);
}

int NewJournalWrapper::readRecord(char* buf, int size, int record) {
    return d_journal.copyRecordData(buf, size, record);
}

void NewJournalWrapper::setMappingLimit(int megs)
{
    d_mappingManager.setMappingLimit(megs<<20);
}

void NewJournalWrapper::printStats(bsls_PlatformUtil::Int64 totalDataSize)
{
    printf("mmap calls:%d file size:%dM "
        "data size:%dM overhead:%d%%",
        d_mappingManager.mapCountReset(),
        (int)(d_journal.getFileSize()/1024/1024),
        (int)(totalDataSize/1024/1024),
        (totalDataSize
           ? (int)((d_journal.getFileSize()-totalDataSize)/(totalDataSize/100))
           : 999999
        )
    );
}

void caseNeg10Test(baecs_Journal *mX, int numRecords, int recordSize)
{
    char *buffer =
       (char*)bslma_Default::allocator()->allocate(
                                                 recordSize * mX->blockSize());
    bsl::memset(buffer, 0xAB, recordSize * mX->blockSize());
    bsl::vector<baecs_Journal::RecordHandle> handles;
    for (int i = 0; i < numRecords; ++i)
    {
        baecs_Journal::RecordHandle h;
        ASSERT(0 == mX->addRecord(&h, buffer, recordSize * mX->blockSize()));
        ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != h);
        handles.push_back(h);
    }
    bslma_Default::allocator()->deallocate(buffer);

    bsl::vector<int> bCounts, pCounts;

    for (int i = 0; i < numRecords; ++i)
    {
        int numBlocks, numPages;
        numBlocks = 0;
        numPages = 0;
        ASSERT(0 == mX->recordFragmentation(&numBlocks,
                                            &numPages,
                                            handles[i]));
        if(bCounts.size() < numBlocks)
        {
            bCounts.resize(numBlocks + 1);
        }

        bCounts[numBlocks]++;
        if(pCounts.size() < numPages)
        {
            pCounts.resize(numPages + 1);
        }
        pCounts[numPages]++;

    }

    printf("Num records: %d\n", numRecords);
    for (int i = 1; i < bCounts.size(); ++i)
    {
        printf("\t(%d->%d)\n", i, bCounts[i]);
    }
    for (int i = 1; i < pCounts.size(); ++i)
    {
        printf("\t(%d->%d)\n", i, pCounts[i]);
    }
}

struct JournalBenchmark
{
    bsl::vector<char>   d_buf;
    bces_AtomicInt      d_nAdd;
    bces_AtomicInt      d_nRemove;
    bces_AtomicInt      d_nRead;
    bsls_PlatformUtil::Int64
                        d_totalSize;
    bdet_TimeInterval   d_timer;
    bdet_TimeInterval   d_now;
    int                 d_minSize;
    int                 d_maxSize;

    bces_AtomicInt      d_stopFlag;
    bces_AtomicInt      d_targetSize;

    BaseJournal        *d_journal;

    bcemt_Condition     d_cond;
    bcemt_Mutex         d_lock;

    struct Record
    {
        int handle;
#ifdef BAECS_JOURNAL_PARANOID
        unsigned int checksum;
#endif
        int size;
    };

    bsl::vector<Record> d_records;

    JournalBenchmark(BaseJournal *journal);
    void addRecord();
    void removeRecord();
    void readRecord();
    void run(int argc, char** argv);
    void runImpl(int argc, char** argv, const char* pfx);
    void tickThread();
};

JournalBenchmark::JournalBenchmark(BaseJournal *journal)
: d_buf(0)
, d_nAdd(0)
, d_nRemove(0)
, d_nRead(0)
, d_totalSize(0)
, d_maxSize(1024)
, d_minSize(1)
, d_journal(journal)
, d_stopFlag(0)
, d_targetSize(0)
{
    d_buf.resize(d_maxSize+sizeof(int));
}

void JournalBenchmark::addRecord()
{
    int size = d_minSize;
    if (d_maxSize-d_minSize) {
        size += fastrand()%(d_maxSize-d_minSize);
    }
    Record record;
#ifdef BAECS_JOURNAL_PARANOID
    int n = (size+sizeof(int)-1)/sizeof(int);
    for(int i=0; i<n; i++) ((int*)&d_buf.front())[i] = fastrand();
    record.checksum = myAdler((unsigned char*)&d_buf.front(), size);
#endif
    int rc = d_journal->addRecord(&record.handle,&d_buf.front(), size);
    ASSERT(rc == 0);
    record.size = size;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_totalSize += size;
    d_records.push_back(record);
    d_nAdd++;
}

void JournalBenchmark::removeRecord()
{
    Record record;
    {
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    ASSERT(d_records.size());
    int n = fastrand() % d_records.size();
//    bsl::printf("[-%d@%d sz %d]\n",records[n].first, n, records[n].second);
    record = d_records[n];
    d_records[n] = d_records.back();
    d_records.pop_back();
    d_nRemove++;
    d_totalSize -= record.size;
    }
    int rc = d_journal->removeRecord(record.handle);
    ASSERT(rc == 0);
}

void JournalBenchmark::readRecord()
{
    Record record;
    {
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    ASSERT(d_records.size());
    int n = fastrand() % d_records.size();
    record = d_records[n];
    d_records[n] = d_records.back();
    d_records.pop_back();
    }
    d_journal->readRecord(&d_buf.front(), d_buf.size(), record.handle);
#ifdef BAECS_JOURNAL_PARANOID
    unsigned checksum = myAdler((unsigned char*)&d_buf.front(), record.size);
    ASSERT(checksum == record.checksum);
#endif
    {
    bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
    d_nRead++;
    d_records.push_back(record);
    }
}

void JournalBenchmark::tickThread()
{
    d_timer = bdetu_SystemTime::now();

    bcemt_Mutex mutex;
    mutex.lock();
    while(!d_stopFlag) {
        d_cond.timedWait(&mutex, d_timer+bdet_TimeInterval(2, 0));
        d_now = bdetu_SystemTime::now();
        if ((d_now-d_timer).seconds() < 1) {
            continue;
        }
        double secs = (d_now - d_timer).totalSecondsAsDouble();
        bcemt_LockGuard<bcemt_Mutex> guard(&d_lock);
        double addRate = d_nAdd.swap(0) / secs;
        double removeRate = d_nRemove.swap(0) / secs;
        double readRate = d_nRead.swap(0) / secs;
        printf("%f add/s, %f remove/s, %f read/s records: %d ",
                addRate, removeRate, readRate,
                d_records.size());
        d_journal->printStats(d_totalSize);
        printf("\n");
        d_timer = d_now;
    }
    mutex.unlock();
}

void JournalBenchmark::run(int argc, char** argv) {
    if (!argc) {
        printf(
        "Usage: test -1/-2 command...\n"
        "Commands:\n"
        "g<nnn>: grow/shrink the journal to <nnn> records\n"
        "s<nnn>: spin repeating add record/remove random record for <nnn>"
                                                                   " seconds\n"
        "r<nnn>: spin repeating read random record for <nnn> seconds\n"
        "n<nnn>: set min record size to <nnn> bytes\n"
        "x<nnn>: set max record size to <nnn> bytes\n"
        "m<nnn>: set mapping limit to <nnn> Mbytes\n"
        "t     : begin a group of commands\n"
        "f<nnn>: execute the group starting with t command in <nnn> "
                                                           "parallel threads\n"
        "q     : reopen the journal\n"
        "\n");
        return;
    }
    bcemt_ThreadGroup tg;
    d_stopFlag = 0;
    tg.addThread(bdef_BindUtil::bind(&JournalBenchmark::tickThread, this));
    static const char* pfxs
                          = " > > > > > > > > > > > > > > > > > > > > > > > >";
    runImpl(argc, argv, pfxs+strlen(pfxs));
    d_stopFlag = 1;
    d_cond.signal();
    tg.joinAll();
}

void JournalBenchmark::runImpl(int argc, char** argv, const char* pfx)
{
    int depth = 0;
    int beg = 0;
    for(int arg = 0; arg<argc; arg++) {
        if (argv[arg][0] == 't') {
                if (!(depth++)) beg = arg+1;
                continue;
        }
        if (argv[arg][0] == 'f') {
            BSLS_ASSERT(depth>0);
            if (!(--depth)) {
                int nThreads=atoi(argv[arg]+1);
                printf("%sForking %d threads\n", pfx, nThreads);
                bcemt_ThreadGroup tg;
                for(int i=0; i<nThreads; i++) {
                    tg.addThread(bdef_BindUtil::bind(
                                                    &JournalBenchmark::runImpl,
                                                    this,
                                                    arg-beg,
                                                    argv+beg, pfx-2));
                }
                tg.joinAll();
                printf("%sJoined %d threads\n", pfx, nThreads);
            }
            continue;
        }
        if (depth>0) continue;
        switch(argv[arg][0]) {
            case 'g': {
                int size=atoi(argv[arg]+1);
                printf("%s%s the journal to %d records\n", pfx,
                    (size<d_records.size()?"Shrinking":"Growing"), size);
                int tSize;
                while((tSize = d_targetSize) != size) {
                    if (tSize < size) {
                        if (tSize == d_targetSize.testAndSwap(tSize,
                                                              tSize+1)) {
                            addRecord();
                        }
                    } else if (tSize > size) {
                        if (tSize == d_targetSize.testAndSwap(tSize,
                                                              tSize-1)) {
                            removeRecord();
                        }
                    }
                }
            } break;
            case 's': {
                int sec=atoi(argv[arg]+1);
                bdet_TimeInterval start = bdetu_SystemTime::now();
                printf("%sSpinning with remove random/add new for %d "
                                                                   "seconds\n",
                       pfx,
                       sec);
                while((d_now - start).totalSecondsAsDouble() < sec) {
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                    addRecord();
                    removeRecord();
                }
            } break;
            case 'r': {
                int sec=atoi(argv[arg]+1);
                bdet_TimeInterval start = bdetu_SystemTime::now();
                printf("%sSpinning with read random for %d seconds\n",
                       pfx,
                       sec);
                while((d_now - start).totalSecondsAsDouble() < sec) {
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                    readRecord();
                }
            } break;
            case 'n': {
                d_minSize=atoi(argv[arg]+1);
                if (d_maxSize < d_minSize) d_maxSize=d_minSize;
                printf("Record size: min=%d, max=%d\n", d_minSize, d_maxSize);
            } break;
            case 'x': {
                d_maxSize=atoi(argv[arg]+1);
                if (d_minSize > d_maxSize) d_minSize=d_maxSize;
                if (d_buf.size() < d_maxSize + sizeof(int)) {
                    d_buf.resize(d_maxSize + sizeof(int));
                }
                printf("Record size: min=%d, max=%d\n", d_minSize, d_maxSize);
            } break;
            case 'm': {
                int mappingLimit=atoi(argv[arg]+1);
                d_journal->setMappingLimit(mappingLimit);
                printf("Mapping limit set to %dM\n", mappingLimit);
            } break;
            case 'q': {
                d_journal->reopen();
                printf("Reopened the journal\n");
            } break;
        }
    }
}

struct Case16_Record {
    Obj        *d_journal_p;
    H           d_handle;
    unsigned    d_length;
};

class Case16 {
    enum {
        MAPPING_LIMIT   = 512*1024*1024,
        NUM_JOURNALS    = 32,
        MAX_BUFFER_SIZE = 11972,
        MAX_BUFFERS     = 100,
        QUEUE_SIZE      = 65536,
        NUM_PRODUCED_RECORDS
                        = 1000, // per thread
        NUM_PRODUCERS   = 8,
        NUM_CONSUMERS   = 16
    };
    baecs_MappingManager                d_mappingManager;
    int                                 d_numJournals;
    bsl::vector< bcema_SharedPtr<Obj> > d_journals;
    bsl::vector< bsl::string >          d_journalNames;
    bcec_FixedQueue<Case16_Record>     *d_queue_p;
    bces_AtomicInt                      d_numRecordsAdded;
    bces_AtomicInt                      d_numRecordsRemoved;
    bces_AtomicInt                      d_haltFlag;
    int                                 d_mode;
    bslma_Allocator                    *d_allocator_p;
    bcep_TimerEventScheduler            d_eventScheduler;
    bcep_TimerEventScheduler::Handle    d_commitHandle;

    void addBlob(const bcema_Blob &blob);
    void producerThread();
    void consumerThread();
    void commitAll();
public:
    Case16(int numJournals, bslma_Allocator *allocator = 0);
    ~Case16();

    void close();
       // Close the journals

    void open(int mode);
       // Open the journals in the 'specified' mode.

    void removeFiles();
       // Remove all files

    int run(double commitInterval);
       // Run the test case

};

Case16::Case16(int numJournals, bslma_Allocator *allocator)
: d_mappingManager(MAPPING_LIMIT, 2, allocator)
, d_numJournals(numJournals)
, d_journals(allocator)
, d_queue_p(NULL)
, d_allocator_p(allocator)
{
    ASSERT(0 < d_numJournals);
    for(int i=0; i<d_numJournals; ++i) {
        bcema_SharedPtr<Obj> journal;
        journal.createInplace(allocator, &d_mappingManager, allocator);
        char filename[MAX_TMPFILENAME];
        getTmpFileName(filename, 16);
        d_journalNames.push_back(filename);

        if (verbose) {
            P(filename);
        }
        d_journals.push_back(journal);
    }
    d_eventScheduler.start();
}

Case16::~Case16() {
    removeFiles();
    d_eventScheduler.stop();
}

void Case16::open(int mode)
{
    for (int i = 0; i < d_numJournals; ++i)
    {
        LOOP_ASSERT(i, 0 == d_journals[i]->create(d_journalNames[i].c_str(),
                                                  mode, 0));
    }
}

void Case16::close()
{
    for(int i=0; i<d_numJournals; ++i) {
        int rc = d_journals[i]->validate(veryVerbose);
        d_journals[i]->close();
        ASSERT(!rc);
    }
}

void Case16::removeFiles()
{
    for(int i=0; i<d_numJournals; ++i) {
        bdesu_FileUtil::remove(d_journalNames[i].c_str());
    }
}

void Case16::addBlob(const bcema_Blob &blob) {
    Obj* newJournal = d_journals[fastrand(d_numJournals)].ptr();
    H newHandle;
    bsls_Stopwatch w;
    w.start();
    ASSERT(0 == newJournal->addRecord(&newHandle, blob));
    w.stop();
    if (w.elapsedTime() > 0.4) // 400 ms
    {
        P(w.elapsedTime());
    }
    if (newHandle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
    {
        Case16_Record newRec;
        newRec.d_journal_p = newJournal;
        newRec.d_handle = newHandle;
        newRec.d_length = blob.length();
        d_queue_p->pushBack(newRec);
    }
}

void Case16::producerThread()
{
    for(int i=0; i < NUM_PRODUCED_RECORDS; ++i) {
        while (d_numRecordsAdded - d_numRecordsRemoved > QUEUE_SIZE/2) {
            bcemt_ThreadUtil::yield();
        }
        int numBuffers = fastrand(MAX_BUFFERS);
        int blobLength = 0;
        bcema_Blob blob;
        for(int i=0; i<numBuffers; i++) {
            int bufSize = fastrand(MAX_BUFFER_SIZE-1)+1;
            // not using test allocator here (too slow)
            const bcema_SharedPtr<char> sp = bcema_SharedPtrUtil::
                createInplaceUninitializedBuffer(bufSize);
            bcema_BlobBuffer bb(sp, bufSize);
            blob.appendBuffer(bb);
            blobLength += bufSize;
        }
        blob.setLength(blobLength);
        if (blobLength > 512 * 1024 && verbose) {
            bsl::printf("Adding a 'long' record: %d bytes\n", blobLength);
        }
        if (!d_haltFlag) {
            ++d_numRecordsAdded;
            //        cout << "N" << bsl::flush;
            addBlob(blob);
        }
        else {
            // bsl::cout << "PRODUCER " << bcemt_ThreadUtil::selfIdAsInt()
            //    << " HALTING." << bsl::endl;
            break;
        }
    }
}

void Case16::consumerThread()
{
    while(1) {
        Case16_Record rec = d_queue_p->popFront();
        if (!rec.d_journal_p) {
            return;
        }
        ASSERT(rec.d_handle != Obj::BAECS_INVALID_RECORD_HANDLE);
        // get the data
        bcema_Blob blob;
        if (d_haltFlag) {
            // bsl::cout << "CONSUMER " << bcemt_ThreadUtil::selfIdAsInt()
            //    << " HALTING." << bsl::endl;
            return;
        }

        ASSERT(rec.d_length == rec.d_journal_p->getRecordData(&blob,
                                                              rec.d_handle));

        LOOP_ASSERT(blob.numBuffers(), blob.numBuffers() < 15);

        if (rand()&1) {
            // 1 time out of 10: remove the record
            ASSERT(0 == rec.d_journal_p->removeRecord(rec.d_handle));
            ++d_numRecordsRemoved;
        } else {
            // 9 times out of 10: move the record to another journal
//            cout << "R" << bsl::flush;
            addBlob(blob);
        }
    }
}

void Case16::commitAll()
{
    for(int i=0; i < d_numJournals; i++) {
        if (d_haltFlag) {
            // bsl::cout << "COMMIT THREAD: HALTING" << bsl::endl;
            return;
        }
        int rc = d_journals[i]->commit();
        if (verbose) {
            bsl::cout << "Commited " << d_journalNames[i]
                << ": " << rc << bsl::endl;
            ASSERT(0 == rc);
        }
    }
}

int Case16::run(double commitInterval)
{
    d_numRecordsAdded = 0;
    d_numRecordsRemoved = 0;
    d_haltFlag = 0;

    d_queue_p = new bcec_FixedQueue<Case16_Record>(QUEUE_SIZE);

    if (0 < commitInterval) {
        d_commitHandle =
            d_eventScheduler.startClock(bdet_TimeInterval(commitInterval),
                                    bdef_BindUtil::bind(
                                                    &Case16::commitAll, this));

    }
    if (verbose) {
        P(commitInterval);
    }

    bcemt_ThreadGroup producerThreads, consumerThreads;
    for(int i=0; i<NUM_PRODUCERS; ++i) {
        producerThreads.addThread(bdef_BindUtil::bind(
                &Case16::producerThread, this));
    }
    for(int i=0; i<NUM_CONSUMERS; ++i) {
        consumerThreads.addThread(bdef_BindUtil::bind(
            &Case16::consumerThread, this));
    }
    while(!d_haltFlag
          && d_numRecordsAdded < NUM_PRODUCERS * NUM_PRODUCED_RECORDS)
    {
        cout << "Added: " << d_numRecordsAdded
            << " Removed: " << d_numRecordsRemoved
            << " In queue: " << d_queue_p->length()
            << " Total: " << NUM_PRODUCERS * NUM_PRODUCED_RECORDS
            << bsl::endl;
        bcemt_ThreadUtil::microSleep(0, 1);
#if 0

        for(int i = 0; i < d_numJournals; ++i) {
            int rc = d_journals[i]->validate(true);
            if (rc) {
                d_haltFlag = 1;   // FREEZE!
                bsl::cout << "VALIDATION FAILED FOR "
                          << d_journals[i]->filename()
                          << ": rc = " << rc
                          << ".  ABORTING TEST." << bsl::endl;
                bsl::string renameTo(d_journals[i]->filename());
                renameTo += ".BAD";
                bsl::rename(d_journals[i]->filename(), renameTo.c_str());
                break;
            }
        }
#endif
    }
    producerThreads.joinAll();
    Case16_Record rec;
    rec.d_journal_p = 0;
    for(int i=0; i<NUM_CONSUMERS; ++i) {
        d_queue_p->pushBack(rec);
    }
    consumerThreads.joinAll();
    if (0 < commitInterval)
    {
        d_eventScheduler.cancelClock(d_commitHandle);
    }

    delete d_queue_p;

    return d_haltFlag;
}

} // namespace

int main(int argc, char *argv[]) {
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    if (test > 0)
    {
        // NOTE: This condition is actually important.
        // Test cases 6 and 7 are doing 'popen' on the executable
        // (for the test cases of -6 and -7 respectively) and should not
        // print this message.

        bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    }
    bael_DefaultObserver observer(bsl::cerr);
    bael_LoggerManagerConfiguration configuration;
    if (veryVeryVerbose) {
       configuration.setDefaultThresholdLevelsIfValid(
                                                   bael_Severity::BAEL_TRACE,
                                                   bael_Severity::BAEL_TRACE,
                                                   bael_Severity::BAEL_ERROR,
                                                   bael_Severity::BAEL_ERROR);
    }

    bael_LoggerManagerScopedGuard guard(&observer, configuration);

    srand(time(0));

    switch(test) { case 0:
      case 17: {
          enum {
              MAPPING_LIMIT = (1 << 20) * 100,
              NUM_ITERATIONS = 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          bcema_TestAllocator ta; // must use thread-safe allocator
          {
             baecs_Journal mX(&mappingManager, &ta);
             char filename1[MAX_TMPFILENAME];
             getTmpFileName(filename1, 2);

//             for(int i=0; i<NUM_ITERATIONS; i++)
             if (mX.create(filename1, MODE_RW))
             {
                bsl::printf("Can't open journal %s: %d\n", filename1, errno);
                exit(-1);
             }

             enum {
                NUM_RECORDS = 1000,
                MAX_RECORD_SIZE = 13243
             };

             if (verbose) {
                P(filename1);
                P(NUM_RECORDS);
                P(MAX_RECORD_SIZE);
             }

             for (int i = 0; i < NUM_RECORDS; ++i) {
                char record[MAX_RECORD_SIZE];
                baecs_Journal::RecordHandle handle;
                int rc = mX.addRecord(&handle, record, MAX_RECORD_SIZE - 1);
                ASSERT(0 == rc);
                ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != handle);
             }
             if (verbose) {
                bsl::cout << "-> Wrote " << NUM_RECORDS
                          << " records into a journal."
                          << bsl::endl;
             }

             ASSERT(NUM_RECORDS == mX.numUnconfirmedRecords());
             ASSERT(0 == mX.numConfirmedRecords());

             baecs_Journal::RecordHandle handle = mX.firstUnconfirmedRecord();
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                int size = mX.getRecordLength(handle);
                ASSERT(MAX_RECORD_SIZE - 1 == size);
                handle = mX.nextUnconfirmedRecord(handle);
             }

             handle = mX.firstUnconfirmedRecord();
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                baecs_Journal::RecordHandle next =
                   mX.nextUnconfirmedRecord(handle);
                mX.confirmRecord(handle);
                handle = next;
             }

             ASSERT(0 == mX.numUnconfirmedRecords());
             ASSERT(NUM_RECORDS == mX.numConfirmedRecords());

             mX.close();
             bdesu_FileUtil::remove(filename1);
          }
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 16: {
          // ----------------------
          // Concern: thread safety and journal integrity
          //
          // Plan (stress test): create a set of journals.  Also, create a
          // bunch of producer threads adding records to the journal, then
          // passing their handles via a bcemt_FixedQueue.  Also create a
          // bunch of consumer threads, verifying the data, then either
          // removing the record or re-adding it.  Run the test until a
          // certain number of records is processed.
          // ----------------------

          if (verbose) {
             cout << "THEAD SAFETY STRESS TEST" << endl
                  << "========================" << endl;
          }

          bcema_TestAllocator ta;

          {
              enum { NUM_JOURNALS = 32 };
              Case16 test(NUM_JOURNALS, &ta);
              test.open(MODE_RW);
              test.run(0.0);
          }

          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case -16: {

          if (verbose) {
             cout << "THEAD SAFETY STRESS TEST" << endl
                  << "========================" << endl;
          }

          bcema_TestAllocator ta;

          {
              // TBD: reopen in safe/not-safe modes
              enum { NUM_JOURNALS = 2 };

              Case16 test(NUM_JOURNALS, &ta);

              for (int i = 0; i < 10; ++i)
              {
                  bsl::cout << "=> RUN " << i << bsl::endl;

                  test.open(i==0 ? MODE_RW: MODE_RW_SAFE); // open the journals
                  int rc = test.run(i/2*1.0);              // (seconds) commit
                  // 0 -- fast mode
                  // 1 -- safe mode, no commit
                  // 2 -- safe mode, commits
                  test.close();
                  if (rc)
                  {
                      bsl::cout << "=> RUN " << i << " FAILED."
                          << bsl::endl;
                      break;
                  }
              }
          }

          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 15: {
          // ----------------------
          // Concern: rollback
          //
          // Plan: Create a journal in safe mode.  Add some initial records
          // and commit them.  Then add some more records and roll them back;
          // add some further records and commit them.  Reopen the journal
          // and verify that the first and third groups are present but
          // not the second (which was rolled back).
          //
          // While adding the second group of records, also confirm all the
          // records in the first group to verify that they are still present
          // afterwards with an unconfirmed status.
          // ----------------------

          if (verbose) {
             cout << "ROLLBACK TEST" << endl
                  << "=============" << endl;
          }

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 15);

          if (verbose) {
             P(filename);
          }

          enum {
             MAPPING_LIMIT = (1 << 20) * 100
          };

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;
          baecs_MappingManager mappingManager(MAPPING_LIMIT, 2);

          struct Parameters {
             int d_bytesPerBlock;
             int d_blocksPerPage;
             int d_keptRecordSize;
             int d_numKept;
             int d_rollbackRecordSize;
             int d_numRolled;
          } parameters[] = {
             {DEF_ByPB, 8190, 8, 1, 8, 1},
             {DEF_ByPB, 8190, 16, 300, 16, 50},
             {64, 128, 256, 100, 256, 100},
             {64, 128, 256, 1, 256, 1},
             {64, 128, 256, 1, 256, 10000},
             {64, 128, 8, 10000, 800000, 2},
             {DEF_ByPB, 8190, 800000, 2, 8, 2},
             {DEF_ByPB, 8190, 8, 1, 800000, 2},
             {64, 128, 800000, 2, 32, 25000},
             {DEF_ByPB, 8190, 8, 200000, 800000, 1}
          };

          bcema_TestAllocator ta;
          for (int a = 0; a < sizeof(parameters) / sizeof(Parameters); ++a) {
             const Parameters& p = parameters[a];

             int initTestStatus = testStatus;
             const int keptRecordBytes = p.d_keptRecordSize * sizeof(int);
             const int rollbackRecordBytes =
                p.d_rollbackRecordSize * sizeof(int);

             if (verbose) {
                P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                P(p.d_keptRecordSize);
                P(p.d_numKept);
                P(p.d_rollbackRecordSize);
                P(p.d_numRolled);
                cout << "--------------------" << endl;
             }

             if (veryVerbose) {
                cout << "Creating initial journal file..." << endl;
             }

             Obj mX(&mappingManager, &ta);

             int* kBuffer = (int*)malloc(keptRecordBytes);
             int* rBuffer = (int*)malloc(rollbackRecordBytes);

             baecs_JournalParameters jp;
             jp.blockSize() = p.d_bytesPerBlock;
             jp.blocksPerPage() = p.d_blocksPerPage;
             ASSERT(0 == mX.create(filename, MODE_RW_SAFE, 0, jp));

             H handle;
             for (int i = 0; i < p.d_numKept; ++i) {
                stampBuffer(kBuffer, p.d_keptRecordSize, i);
                ASSERT(0 == mX.addRecord(&handle, (void*)kBuffer,
                                         keptRecordBytes));
             }

             if (veryVerbose) {
                cout << "Committing..." << endl;
             }
             mX.commit();

             if (veryVerbose) {
                cout << "Modifying journal..." << endl;
             }

             ASSERT(p.d_numKept == mX.numUnconfirmedRecords());

             handle = mX.firstUnconfirmedRecord();
             for (int i = 0; handle != Obj::BAECS_INVALID_RECORD_HANDLE; ++i) {
                H next = mX.nextUnconfirmedRecord(handle);
                mX.confirmRecord(handle);
                handle = next;
             }

             for (int i = 0; i < p.d_numRolled; ++i) {
                stampBuffer(rBuffer, p.d_rollbackRecordSize,
                            p.d_numKept + i);
                ASSERT(0 == mX.addRecord(&handle, (void*)rBuffer,
                                         rollbackRecordBytes));
             }

             ASSERT(p.d_numRolled == mX.numUnconfirmedRecords());
             ASSERT(p.d_numKept == mX.numConfirmedRecords());

             if (veryVerbose) {
                cout << "Rolling back and adding more records..." << endl;
             }
             ASSERT(0 == mX.rollback());

             ASSERT(0 == mX.numConfirmedRecords());
             ASSERT(p.d_numKept == mX.numUnconfirmedRecords());
             ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                                                    mX.firstConfirmedRecord());

             for (int i = 0; i < p.d_numKept; ++i) {
                stampBuffer(kBuffer, p.d_keptRecordSize,
                            p.d_numKept + p.d_numRolled + i);
                ASSERT(0 == mX.addRecord(&handle, (void*)kBuffer,
                                         keptRecordBytes));
             }

             ASSERT(2 * p.d_numKept == mX.numUnconfirmedRecords());

             if (veryVerbose) {
                cout << "Closing and reopening journal..." << endl;
             }

             mX.close();

             Obj mY(&mappingManager, &ta);
             ASSERT(0 == mY.open(filename, MODE_RO));

             if (veryVerbose) {
                cout << "Verifying data..." << endl;
             }

             ASSERT(2 * p.d_numKept == mY.numUnconfirmedRecords());
             ASSERT(0 == mY.numConfirmedRecords());
             ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                                                    mY.firstConfirmedRecord());

             if (initTestStatus != testStatus) {
                cout << "Failure, aborting!" << endl;
                exit(testStatus);
             }

             handle = mY.firstUnconfirmedRecord();
             if (veryVerbose) {
                cout << "Verifying first block..." << endl;
             }
             for (int i = 0; i < p.d_numKept; ++i) {
                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE != handle);
                ASSERT(keptRecordBytes == mY.copyRecordData(
                                                (char*)kBuffer,
                                                keptRecordBytes,
                                                handle));
                ASSERT(0 == verifyBuffer(kBuffer, p.d_keptRecordSize, i));
                handle = mY.nextUnconfirmedRecord(handle);
             }
             if (veryVerbose) {
                cout << "Verifying last block..." << endl;
             }
             for (int i = 0; i < p.d_numKept; ++i) {
                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE != handle);
                ASSERT(keptRecordBytes == mY.copyRecordData(
                                                (char*)kBuffer,
                                                keptRecordBytes,
                                                handle));
                ASSERT(0 == verifyBuffer(kBuffer, p.d_keptRecordSize,
                                         p.d_numKept + p.d_numRolled + i));
                handle = mY.nextUnconfirmedRecord(handle);
             }

             if (veryVerbose) {
                cout << "Cleaning up..." << endl;
             }

             mY.close();
             bdesu_FileUtil::remove(filename);

             free(kBuffer);
             free(rBuffer);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;

      case 14: {
          // ----------------------
          //
          // Concern: fast mode coherency
          //
          // Plan: create a journal in fast mode.  Create a test plan
          // consisting of many additions and removals of records at random.
          // Run the test and confirm that the correct number of records
          // are present at the end.
          //
          // Note that this is identical to case 13 but in fast mode.
          // -----------------------

          if (verbose) {
             cout << "FAST MODE COHERENCY TEST" << endl
                  << "========================" << endl;
          }

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 14);

          if (verbose) {
             P(filename);
          }

          enum {
              MAPPING_LIMIT  = (1 << 20) * 100
          };

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          struct Parameters {
             int d_bytesPerBlock;
             int d_blocksPerPage;
             int d_userDataSize;
             int d_numSteps;
          } parameters[] = {
             {DEF_ByPB, DEF_BlPB, 0, 10},
             {DEF_ByPB, DEF_BlPB, 0, 50000},
             {DEF_ByPB, DEF_BlPB, 100000, 10},
             {DEF_ByPB, DEF_BlPB, 1000000, 100000},
             {64, 128, 0, 10},
             {64, 128, 0, 100000},
             {64, 128, 1000000, 10},
             {64, 128, 1000000, 30000},
          };

          bcema_TestAllocator ta;
          for (int a = 0; a < sizeof(parameters) / sizeof(Parameters); ++a) {
             const Parameters& p = parameters[a];

             if (verbose) {
                P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                P(p.d_userDataSize);
                P(p.d_numSteps);
                cout << "--------------------" << endl;
             }

             Obj mX(&mappingManager, &ta);

             baecs_JournalParameters jp;
             jp.blockSize() = p.d_bytesPerBlock;
             jp.blocksPerPage() = p.d_blocksPerPage;
             ASSERT(0 == mX.create(filename, MODE_RW,
                                          p.d_userDataSize, jp));

             CaseNeg3TestPlan plan;
             plan.generate(p.d_numSteps);

             double dummy;
             CaseNeg3 runner(&mX, &plan, &dummy);
             runner();

             LOOP2_ASSERT(plan.numFinalRecords(), mX.numUnconfirmedRecords(),
                          plan.numFinalRecords() ==
                              mX.numUnconfirmedRecords());

             mX.close();
             bdesu_FileUtil::remove(filename);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 13: {
          // ----------------------
          //
          // Concerns:
          // (1) safe mode coherency
          // (2) A particular sequence of safe-mode additions and removals
          //     which caused internal errors during development
          //
          // Plan (1): create a journal in various modes (implicit and auto-
          // commit will be tested.)  Create a test plan consisting of many
          // additions and removals of records at random.  Run the test and
          // confirm that the correct number of records are present at the
          // end.
          //
          // (2): Execute a specific sequence of additions and removals of
          // records known to cause issues previously during development.
          // -----------------------

          if (verbose) {
             cout << "SAFE MODE COHERENCY TEST" << endl
                  << "========================" << endl;
          }

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 13);

          if (verbose) {
             P(filename);
          }

          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          };

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          bcema_TestAllocator ta;
          {
             if (verbose) {
                cout << "First Phase: Fixed Test Sequence" << endl;
             }

             Obj mX(&mappingManager, &ta);

             baecs_JournalParameters jp;
             jp.blockSize() = 64;
             jp.blocksPerPage() = 128;

             ASSERT(0 == mX.create(filename, MODE_RW_AUTOCOMMIT,
                                          0, jp));

             H h[3];
             char record[10000];

             if (veryVerbose) {
                cout << "Adding 6062 bytes..." << endl;
             }
             mX.addRecord(&h[0], (void*) record, 6062);
             if (veryVerbose) {
                cout << "Adding 631 bytes..." << endl;
             }
             mX.addRecord(&h[1], (void*) record, 631);
             if (veryVerbose) {
                cout << "Adding 4517 bytes..." << endl;
             }
             mX.addRecord(&h[2], (void*) record, 4517);
             if (veryVerbose) {
                cout << "Removing record 0..." << endl;
             }
             mX.removeRecord(h[0]);
             if (veryVerbose) {
                cout << "Removing record 2..." << endl;
             }
             mX.removeRecord(h[2]);
             if (veryVerbose) {
                cout << "Removing record 1..." << endl;
             }
             mX.removeRecord(h[1]);
             if (veryVerbose) {
                cout << "Adding 4677 bytes..." << endl;
             }
             mX.addRecord(&h[0], (void*) record, 4677);
             if (veryVerbose) {
                cout << "Adding 5367 bytes..." << endl;
             }
             mX.addRecord(&h[1], (void*) record, 5367);

             LOOP_ASSERT(mX.numUnconfirmedRecords(),
                         2 == mX.numUnconfirmedRecords());

             mX.close();
             bdesu_FileUtil::remove(filename);
          }

          struct Parameters {
             bool d_suppressRemovals;
             int d_bytesPerBlock;
             int d_blocksPerPage;
             int dummy;
             int d_numSteps;
             int d_mode;
          } parameters[] = {
             {1, 64, 128, 0, 10, MODE_RW_AUTOCOMMIT},
             {0, 64, 128, 0, 10, MODE_RW_AUTOCOMMIT},
             {0, DEF_ByPB, DEF_BlPB, 0, 10, MODE_RW_AUTOCOMMIT},
             {1, DEF_ByPB, DEF_BlPB, 0, 1000, MODE_RW_AUTOCOMMIT},
             {0, DEF_ByPB, DEF_BlPB, 0, 2000, MODE_RW_AUTOCOMMIT},
             {1, 64, 128, 0, 1000, MODE_RW_AUTOCOMMIT},
             {0, 64, 128, 0, 2000, MODE_RW_AUTOCOMMIT},
             {0, DEF_ByPB, DEF_BlPB, 0, 10, MODE_RW_SAFE},
             {0, DEF_ByPB, DEF_BlPB, 0, 50000, MODE_RW_SAFE},
             {1, DEF_ByPB, DEF_BlPB, 0, 50000, MODE_RW_SAFE},
             {0, 64, 128, 0, 10, MODE_RW_SAFE},
             {0, 64, 128, 0, 25000, MODE_RW_SAFE},
             {1, 64, 128, 0, 15000, MODE_RW_SAFE},
          };

          if (verbose) {
             cout << "Second Phase: Randomized Testing" << endl;
          }

          for (int a = 0; a < sizeof(parameters) / sizeof(Parameters); ++a) {
             const Parameters& p = parameters[a];

             if (verbose) {
                P(p.d_suppressRemovals);
                P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                P(p.d_numSteps);
                if (MODE_RW_AUTOCOMMIT == p.d_mode) {
                   cout << "p.d_mode = MODE_RW_AUTOCOMMIT" << endl;
                }
                else {
                   cout << "p.d_mode = MODE_RW_SAFE" << endl;
                }
                cout << "--------------------" << endl;
             }

             Obj mX(&mappingManager, &ta);

             baecs_JournalParameters jp;
             jp.blockSize() = p.d_bytesPerBlock;
             jp.blocksPerPage() = p.d_blocksPerPage;
             ASSERT(0 == mX.create(filename, p.d_mode,
                                          0, jp));

             CaseNeg3TestPlan plan;
             plan.generate(p.d_numSteps, p.d_suppressRemovals);

             double dummy;
             CaseNeg3 runner(&mX, &plan, &dummy);
             runner();

             LOOP2_ASSERT(plan.numFinalRecords(), mX.numUnconfirmedRecords(),
                          plan.numFinalRecords() ==
                              mX.numUnconfirmedRecords());

             mX.close();
             bdesu_FileUtil::remove(filename);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;

      case 12: {
#ifndef BSLS_PLATFORM__OS_WINDOWS
          // ----------------------
          // Concern: appending to an existing journal
          //
          // Plan: open a new journal and add a few records to it.
          // Close it and reopen it.  Remove some of the records and
          // confirm the remainder; then add records of various sizes.
          // Close and reopen the journal to verify the changes.
          //
          // ----------------------

          if (verbose) {
             cout << "APPEND TEST" << endl
                  << "===========" << endl;
          }

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 12);

          if (verbose) {
             P(filename);
          }

          enum {
              MAPPING_LIMIT = (1 << 20) * 100,
              NUM_INIT      = 10,
              NUM_APPEND    = 1002
          };
          char *initRecords[NUM_INIT];
          char *appendRecords[NUM_APPEND];

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          struct Parameters {
             int d_bytesPerBlock;
             int d_blocksPerPage;
             int d_userDataSize;
             int d_initRecordSize;
             int d_appendRecordSize;
          } parameters[] = {
             {DEF_ByPB, DEF_BlPB, 0, 100, 512},
             {DEF_ByPB, DEF_BlPB, 0, 512, 17000},
             {DEF_ByPB, DEF_BlPB, 100, 100, 512},
             {DEF_ByPB, DEF_BlPB, 100, 6000, 1},
             {DEF_ByPB, DEF_BlPB, 100000, 512, 17000},
             {64, 128, 0, 100, 512},
             {64, 128, 0, 512, 17000},
             {64, 128, 100, 100, 512},
             {64, 128, 100000, 5, 50},
             {64, 128, 100000, 512, 17000}
          };

          char commandBuffer[512];

          bcema_TestAllocator ta;
          for (int a = 0; a < sizeof(parameters) / sizeof(Parameters); ++a) {
             const Parameters& p = parameters[a];

             if (verbose) {
                P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                P(p.d_userDataSize);
                P(p.d_initRecordSize);
                P(p.d_appendRecordSize);
                cout << "--------------------" << endl;
             }
             randBuffers(initRecords, NUM_INIT, p.d_initRecordSize);
             randBuffers(appendRecords, NUM_APPEND, p.d_appendRecordSize);

             if (veryVerbose) {
                cout << "Creating initial journal file..." << endl;
             }

             Obj mX(&mappingManager, &ta);

             baecs_JournalParameters jp;
             jp.blockSize() = p.d_bytesPerBlock;
             jp.blocksPerPage() = p.d_blocksPerPage;
             ASSERT(0 == mX.create(filename, MODE_RW, p.d_userDataSize, jp));

             H handle;
             for (int i = 0; i < NUM_INIT; ++i) {
                initRecords[i][p.d_initRecordSize-1] = 0;
                ASSERT(0 == mX.addRecord(&handle, (void*)initRecords[i],
                                         p.d_initRecordSize));
             }

             if (veryVerbose) {
                cout << "Closing journal..." << endl;
             }
             mX.close();

             if (veryVerbose) {
                cout << "Opening and modifying journal..." << endl;
             }

             ASSERT(0 == mX.open(filename,
                                 baecs_Journal::BAECS_READWRITE));
             ASSERT(mX.userDataSize() == p.d_userDataSize);

             for (int i = 0; i < p.d_userDataSize; ++i) {
                mX.userData()[i] = char((i % 200) + 1);
             }

             ASSERT(NUM_INIT == mX.numUnconfirmedRecords());
             handle = mX.firstUnconfirmedRecord();
             for (int i = 0; handle != Obj::BAECS_INVALID_RECORD_HANDLE; ++i) {
                H next = mX.nextUnconfirmedRecord(handle);
                if (i < NUM_INIT/2) {
                   mX.removeRecord(handle);
                }
                else {
                   mX.confirmRecord(handle);
                }
                handle = next;
             }
             for (int i = 0; i < NUM_APPEND; ++i) {
                appendRecords[i][p.d_appendRecordSize-1] = 0;
                ASSERT(0 == mX.addRecord(&handle, (void*)appendRecords[i],
                                         p.d_appendRecordSize));
                if (i >= NUM_APPEND/2) {
                   mX.confirmRecord(handle);
                }
             }

             if (veryVerbose) {
                cout << "Closing and reopening journal..." << endl;
             }

             mX.close();

             Obj mY(&mappingManager, &ta);
             ASSERT(0 == mY.open(filename,
                                        baecs_Journal::BAECS_READONLY));
             ASSERT(mY.userDataSize() == p.d_userDataSize);

             if (veryVerbose) {
                cout << "Verifying data..." << endl;
             }

             for (int i = 0; i < p.d_userDataSize; ++i) {
                ASSERT(char((i % 200) + 1) == mY.userData()[i]);
             }

             bcema_Blob *blob = new bcema_Blob;
             LOOP3_ASSERT(NUM_INIT, NUM_APPEND, mY.numConfirmedRecords(),
                          NUM_INIT/2 + NUM_APPEND/2==mY.numConfirmedRecords());

             if (0 != testStatus) {
                cout << "Failure, aborting!" << endl;
                exit(3);
             }

             handle = mY.firstConfirmedRecord();
             for (int i = NUM_INIT/2; i < NUM_INIT; ++i) {
                ASSERT(p.d_initRecordSize == mY.getRecordData(blob, handle));
                ASSERT(0 == verifyBlob(blob, initRecords[i]));
                handle = mY.nextConfirmedRecord(handle);
             }
             for (int i = NUM_APPEND/2; i < NUM_APPEND; ++i) {
                ASSERT(p.d_appendRecordSize ==
                       mY.getRecordData(blob, handle));
                ASSERT(0 == verifyBlob(blob, appendRecords[i]));
                handle = mY.nextConfirmedRecord(handle);
             }

             ASSERT(NUM_APPEND/2 == mY.numUnconfirmedRecords());
             handle = mY.firstUnconfirmedRecord();
             for (int i = 0; i < NUM_APPEND/2; ++i) {
                ASSERT(p.d_appendRecordSize ==
                       mY.getRecordData(blob, handle));
                ASSERT(0 == verifyBlob(blob, appendRecords[i]));
                handle = mY.nextUnconfirmedRecord(handle);
             }

             if (veryVerbose) {
                cout << "Cleaning up..." << endl;
             }

             delete blob;
             mY.close();
             bdesu_FileUtil::remove(filename);

             freeBuffers(initRecords, NUM_INIT);
             freeBuffers(appendRecords, NUM_APPEND);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
#endif
      } break;
      case 11: {
          // ---------------------
          // Concern: openJournal argument checking
          //
          // This case tests the behavior of openJournal for some of its
          // arguments.  Specifically, the method should fail:
          //   (A) if createIfNotFound is 'false' and the file does not exist
          //   (B) if bytesPerBlock is too small
          //
          // Additionally, confirm that a large odd blocksPerPage value is
          // safe when mapped.
          // ---------------------

          if (verbose) {
             cout << "openJournal ARGUMENT TEST" << endl
                  << "========================" << endl;
          }

          enum {
              MAPPING_LIMIT = (1 << 20) * 100  // 100 MB
          };
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);
          Obj mX(&mappingManager);

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 11);

          if (verbose) {
             cout << "Checking createIfNotFound..." << endl;
          }
          ASSERT(0 != mX.open(filename, MODE_RW));
          ASSERT(0 == mX.create(filename, MODE_RW));
          mX.close();
          bdesu_FileUtil::remove(filename);

          if (verbose) {
             cout << "Checking bytesPerBlock safety..." << endl;
          }
          baecs_JournalParameters param;

          param.blockSize() = 8;
          ASSERT(0 != mX.create(filename, MODE_RW, 0, param));

          param.blockSize() = 31;
          ASSERT(0 != mX.create(filename, MODE_RW, 0, param));

          param.blockSize() = 32;
          ASSERT(0 == mX.create(filename, MODE_RW, 0, param));

          mX.close();
          bdesu_FileUtil::remove(filename);

          if (verbose) {
             cout << "Checking blocksPerPage safety..." << endl;
          }

          param.blockSize() = baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          param.blocksPerPage() = 8191;
          ASSERT(0 == mX.create(filename, MODE_RW, 0, param));
          char* large = (char*)malloc(1500000);
          H h;
          ASSERT(0 == mX.addRecord(&h, (void*)large, 1500000));

          mX.close();
          bdesu_FileUtil::remove(filename);
          free(large);
      } break;

      case 10: {
          // ---------------------
          // Concerns: Huge journal, ordering
          //
          // Ensure that a process can create *AND* re-open a journal larger
          // than a 32-bit address space.  Also ensure that, after a journal
          // is reopened, unconfirmed records appear in the same order they
          // were added.
          //
          // Plan: Open a new journal.  Add about 5 gigabytes worth of records
          // in such a way that they can be validated.  Close the journal,
          // re-open it, and validate that all the records are still present.
          // Validation will be order-dependent, thus also ensuring that
          // ordering is correct.
          // ---------------------
          if (verbose) {
             cout << "HUGE JOURNAL TEST" << endl
                  << "=================" << endl;
          }

          enum {RECORD_SIZE=17500,
                RECORD_SIZE_BYTES=RECORD_SIZE*sizeof(int),
                NUM_RECORDS=57500};

          enum {
              MAPPING_LIMIT = (1 << 20) * 100 // 100 MB
          };
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 10);
          if (verbose) {
             P(filename);
             P(RECORD_SIZE_BYTES);
             P(NUM_RECORDS);
          }

          bcema_TestAllocator ta;
          {
             Obj mX(&mappingManager, &ta);

             ASSERT(0 == mX.create(filename,
                                        MODE_RW));

             int record[RECORD_SIZE];
             H handle;
             for (int i = 0; i < NUM_RECORDS; ++i) {
                record[0] = i;
                ASSERT(0 == mX.addRecord(&handle, (void*)record,
                                         RECORD_SIZE_BYTES));
             }

             mX.close();
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
          if (veryVerbose) {
             cout << "Journal created; reopening to validate" << endl;
          }

          {
             Obj mX(&mappingManager, &ta);

             ASSERT(0 == mX.open(filename, MODE_RO));
             ASSERT(NUM_RECORDS == mX.numUnconfirmedRecords());
             int compareNumber = 0;
             bcema_Blob* blob = new bcema_Blob;
             for (H h = mX.firstUnconfirmedRecord();
                  h != Obj::BAECS_INVALID_RECORD_HANDLE;
                  h = mX.nextUnconfirmedRecord(h)) {
                ASSERT(RECORD_SIZE_BYTES == mX.getRecordData(blob, h));
                ASSERT(*((int*)(blob->buffer(0).data())) == compareNumber++);
             }
             ASSERT(NUM_RECORDS == compareNumber);
             delete blob;
          }
          bdesu_FileUtil::remove(filename);
          ASSERT(0 == ta.numBytesInUse());
          break;
      }

      case 9: {
          // ---------------------
          // Concern: performance does not exhibit fragmentation behavior
          //
          // This test will measure the difference in time between adding
          // records and adding the same records after many random removals.
          //
          //
          // ---------------------
          if (verbose) {
             cout << "FRAGMENTATION PERFORMANCE TEST" << endl
                  << "==============================" << endl;
          }

          enum {NUM_ITERATIONS = 25000,
                NUM_LOOPS = 5,
                NUM_LENGTHS = 800,
                MAX_RECORDLEN = 2000};
          enum {MICROSECS_PER_SEC     = 1000000};

          int lengths[NUM_LENGTHS];
          for (int i = 0; i < NUM_LENGTHS; ++i) {
             lengths[i] = genericRand(MAX_RECORDLEN-1) + 1;
          }

          char record[MAX_RECORDLEN];

          enum {
              MAPPING_LIMIT = (1 << 20) * 100 // 100 MB
          };
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          double rate1Avg=0, rate2Avg=0, rate3Avg=0;
          bcema_TestAllocator ta;
          {
             for (int a = 0; a < NUM_LOOPS; ++a) {
                Obj mX(&mappingManager, &ta);

                char filename[MAX_TMPFILENAME];
                getTmpFileName(filename, 9);

                ASSERT(0 == mX.create(filename,
                                      MODE_RW));

                H handle;
                bdet_TimeInterval start, stop;
                bdetu_SystemTime::loadSystemTimeDefault(&start);
                // PHASE 1: Add all records, timed,
                //          and then remove them all in order
                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                   mX.addRecord(&handle, (void*)record,
                                lengths[i % NUM_LENGTHS]);
                }
                bdetu_SystemTime::loadSystemTimeDefault(&stop);
                bsls_PlatformUtil::Int64 elapsed =
                   (stop - start).totalMicroseconds();
                double rate1 = ((double)NUM_ITERATIONS / elapsed) *
                   MICROSECS_PER_SEC;

                while ((handle = mX.firstUnconfirmedRecord()) !=
                       Obj::BAECS_INVALID_RECORD_HANDLE) {
                   mX.removeRecord(handle);
                }

                vector<H> handles;
                handles.reserve(NUM_ITERATIONS);

                //PHASE 2: Add all records, timed, then remove 3/4 of them
                //         in a random order

                bdetu_SystemTime::loadSystemTimeDefault(&start);
                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                   ASSERT(0 == mX.addRecord(&handle, (void*)record,
                                            lengths[i % NUM_LENGTHS]));
                   handles.push_back(handle);
                }
                bdetu_SystemTime::loadSystemTimeDefault(&stop);
                elapsed = (stop - start).totalMicroseconds();
                double rate2 = ((double)NUM_ITERATIONS / elapsed) *
                   MICROSECS_PER_SEC;

                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                   int index = genericRand(handles.size());
                   vector<H>::iterator which = handles.begin() + index;
                   ASSERT(0 == mX.removeRecord(*which));
                   handles.erase(which);
                }

                // PHASE 3: add all records, timed

                bdetu_SystemTime::loadSystemTimeDefault(&start);
                for (int i = 0; i < NUM_ITERATIONS; ++i) {
                   H handle;
                   ASSERT(0 == mX.addRecord(&handle, (void*)record,
                                            lengths[i % NUM_LENGTHS]));
                   handles.push_back(handle);
                }
                bdetu_SystemTime::loadSystemTimeDefault(&stop);
                elapsed = (stop - start).totalMicroseconds();
                double rate3 = ((double)NUM_ITERATIONS / elapsed) *
                   MICROSECS_PER_SEC;

                if (veryVerbose) {
                   cout << "Rates: Loop="
                        << a+1 << " Initialize="
                        << rate1 << " Before=" << rate2
                        << " After=" << rate3 << endl;
                }

                rate1Avg += rate1;
                rate2Avg += rate2;
                rate3Avg += rate3;

                bdesu_FileUtil::remove(filename);
             }
             rate1Avg /= NUM_LOOPS;
             rate2Avg /= NUM_LOOPS;
             rate3Avg /= NUM_LOOPS;
             if (verbose) {
                cout << "Rates Overall: Initialize=" << rate1Avg
                     << " Before=" << rate2Avg << " After=" << rate3Avg
                     << endl;
             }
             LOOP2_ASSERT(rate2Avg, rate1Avg, rate2Avg > rate1Avg);
             LOOP2_ASSERT(rate3Avg, rate2Avg, rate3Avg > 0.6 * rate2Avg);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 8: {
          // ------------------
          // Concern: persistence
          //
          // This case tests the persistence of the journal across multiple
          // uses.  The journal will be opened, records will be added, and
          // user data will be modified.  It will then be closed and a new
          // journal opened on the same file.  A different part of user
          // data will be changed and records will be appended.
          //
          // The journal will be closed and a new one opened.  This will
          // be used to verify the records and user data.
          // -------------------
          if (verbose) {
             cout << "PERSISTENCE TEST" << endl
                  << "================" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);
          enum {RECORD_SIZE=12345};

          const int USER_DATA_SIZE =
                     (baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
                      baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE * 5)/2;

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 8);
          for (int i = 0; i < 2; ++i) {
             bcema_TestAllocator ta;
             {
                Obj mX(&mappingManager, &ta);

                if (0 == i) {
                   if (verbose) {
                      cout << "Opening initial journal" << endl;
                      P(USER_DATA_SIZE);
                   }
                   ASSERT(0 == mX.create(filename,
                                                MODE_RW, USER_DATA_SIZE));
                }
                else {
                   if (verbose) {
                      cout << "Opening for append" << endl;
                   }
                   ASSERT(0 == mX.open(filename, MODE_RW));
                }

                for (int j = 1000*i; j < (1000*i)+1000; ++j) {
                   mX.userData()[j] = (i+1)*123;
                }

                char record[RECORD_SIZE];
                for (int j = 1; j <= 3; ++j) {
                   for (int k = 0; k < RECORD_SIZE; ++k) {
                      record[k] = j + 3*i;
                   }
                   H dum;
                   mX.addRecord(&dum, (void*)record, RECORD_SIZE);
                }

                ASSERT(3*(i+1) == mX.numUnconfirmedRecords());
                if (verbose) {
                   cout << "Closing journal..." << endl;
                }
                mX.close();
             }
             ASSERT(0 < ta.numAllocations());
             ASSERT(0 == ta.numBytesInUse());
          }
          bcema_TestAllocator ta;
          {
             Obj mX(&mappingManager, &ta);

             if (verbose) {
                cout << "Reopening journal for verify..." << endl;
             }

             ASSERT(0 == mX.open(filename, MODE_RO)); // MODE_RO

             ASSERT(mX.userDataSize() == USER_DATA_SIZE);
             ASSERT(6 == mX.numUnconfirmedRecords());
             ASSERT(0 == mX.numConfirmedRecords());

             if (veryVerbose) {
                cout << "Verifying user data area" << endl;
             }
             //First verify the user data
             for (int i = 0; i < 1000; ++i) {
                int value = (unsigned char)mX.userData()[i];
                if (veryVerbose) {
                   LOOP2_ASSERT(i, value, 123 == value);
                }
                else if (testStatus < 100) {
                   ASSERT(123 == value);
                }
             }
             for (int i = 1000; i < 2000; ++i) {
                int value = (unsigned char)mX.userData()[i];
                if (veryVerbose) {
                   LOOP2_ASSERT(i, value, 246 == value);
                }
                else if (testStatus < 100) {
                   ASSERT(246 == value);
                }
             }

             if (veryVerbose) {
                cout << "Verifying records" << endl;
             }
             bool recordFound[6] = {0, 0, 0, 0, 0, 0};
             char record[RECORD_SIZE];
             int recordCount = 0;
             for (H handle = mX.firstUnconfirmedRecord();
                  handle != Obj::BAECS_INVALID_RECORD_HANDLE;
                  handle = mX.nextUnconfirmedRecord(handle)) {
                memset(record, 99, RECORD_SIZE);
                mX.copyRecordData(record, RECORD_SIZE, handle);
                ASSERT(0 < record[0] && 6 >= record[0]);
                for (int i = 0; i < RECORD_SIZE; ++i) {
                   LOOP_ASSERT(i, record[i] == record[0]);
                }
                int index = ((unsigned char)record[0]) - 1;
                if (veryVeryVerbose) {
                   cout << "Found record " << index + 1 << endl;
                }
                recordFound[index] = true;
                ++recordCount;
             }
             LOOP_ASSERT(recordCount, 6 == recordCount);
             for (int i = 0; i < 6; ++i) {
                LOOP_ASSERT(i, recordFound[i]);
             }

             mX.close();
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
          bdesu_FileUtil::remove(filename);
      } break;

#ifndef BSLS_PLATFORM__OS_WINDOWS
      case -7: {
          // ------------------
          // CHILD PROCESS for case 7
          //
          // See case 7.
          // ------------------
          if (argc != 4 ||
              (!(verbose = (9 == bsl::atoi(argv[2]))) &&
               8 != bsl::atoi(argv[2]))) {
             cout << "Case -7 is for INTERNAL USE ONLY.  GO AWAY." << endl;
             exit(9);
          }
          if (verbose) {
             cerr << "CASE 7 Child Process Running" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);
          Obj mX(&mappingManager);
          H h1, dum;

          const char *BUF1 = "abcdefghi";
          const char *BUF2 = "j";
          const char *BUF3 = "klm";

          const int USER_DATA_SIZE = (
              baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
              baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE * 5) / 2;

          mX.create(argv[3], MODE_RW_SAFE, USER_DATA_SIZE);

          int loopStart =
                  baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
                  baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE - 4;
          for (int i = loopStart; i < loopStart + 10; ++i) {
             mX.userData()[i] = '0' + (i - loopStart);
          }

          mX.addRecord(&h1, (void*)BUF1, (int)strlen(BUF1)+1);
          mX.addRecord(&dum, (void*)BUF2, (int)strlen(BUF2)+1);
          mX.addRecord(&dum, (void*)BUF3, (int)strlen(BUF3)+1);
          mX.confirmRecord(h1);
          mX.commit();

          cout << 'A' << endl;
          bcemt_ThreadUtil::sleep(bdet_TimeInterval(4));
          mX.close();
          if (verbose) {
             cerr << "CASE 7 Child Process Done" << endl;
          }

      } break;
      case 7: {
          // ------------------
          // commit() / user data
          //
          // This case tests the commit() method and the basic functionality
          // of the user data feature.  It is similar in design to case 6.
          // A child process is spawned to execute case -7 of this driver.
          // Case -7 creates a journal with a large user area and writes three
          // small records to it, then does a commit().  This process
          // then copies the backing file, opens it, and verifies the changes
          // are present.
          // -------------------
          if (verbose) {
             cout << "commit() / USER DATA TEST" << endl
                  << "=========================" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          char filename[MAX_TMPFILENAME], filename2[MAX_TMPFILENAME];
          getTmpFileName(filename, 7);
          getTmpFileName(filename2, 7);

          char buffer[512];
          snprintf(buffer, 512, "%s -7 %d %s", argv[0],
                   verbose? 9 : 8, filename);
          FILE* childFD = popen(buffer, "r");
          BSLS_ASSERT(0 != childFD);  // test invariant
          fgets(buffer, 3, childFD);
          if (buffer[0] != 'A') {
             cout << "Communication error with child process!!" << endl;
             exit(9);
          }

          snprintf(buffer, 512, "cp -f %s %s", filename, filename2);
          system(buffer);

          bcema_TestAllocator ta;
          {
             Obj mX(&mappingManager, &ta);
             const int USER_DATA_SIZE = (
                 baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
                 baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE * 5) / 2;

             ASSERT(0 == mX.open(filename2, MODE_RO));
             ASSERT(mX.userDataSize() == USER_DATA_SIZE);

             H handle;

             for (int i = 0; i < 2; ++i) {
                bool stringFound[2] = {0,0};

                H handle = mX.firstConfirmedRecord();
                bcema_Blob* blob = new bcema_Blob;
                ASSERT(handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                ASSERT(10 == mX.getRecordData(blob, handle));
                ASSERT(0 == verifyBlob(blob, "abcdefghi"));

                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                       mX.nextConfirmedRecord(handle));

                handle = mX.firstUnconfirmedRecord();
                ASSERT(handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                mX.getRecordData(blob, handle);
                if (0 == verifyBlob(blob, "j")) {
                   stringFound[0] = true;
                }
                if (0 == verifyBlob(blob, "klm")) {
                   stringFound[1] = true;
                }

                handle = mX.nextUnconfirmedRecord(handle);
                ASSERT(handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                mX.getRecordData(blob, handle);
                if (0 == verifyBlob(blob, "j")) {
                   stringFound[0] = true;
                }
                if (0 == verifyBlob(blob, "klm")) {
                   stringFound[1] = true;
                }

                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                       mX.nextUnconfirmedRecord(handle));

                ASSERT(stringFound[0] && stringFound[1]);

                int loopStart =
                 baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
                 baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE -4;
                for (int i = loopStart; i < loopStart + 10; ++i) {
                   ASSERT(mX.userData()[i] == '0' + (i - loopStart));
                }

                delete blob;
                if (childFD) {
                   pclose(childFD);
                   childFD = 0;
                   mX.close();
                   ASSERT(0 == mX.open(filename, MODE_RO));
                }
             }

             mX.close();
             bdesu_FileUtil::remove(filename);
             bdesu_FileUtil::remove(filename2);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case -6: {
          // ------------------
          // CHILD PROCESS for case 6
          //
          // See case 6.
          // ------------------
          if (argc != 4 ||
              (!(verbose = (9 == bsl::atoi(argv[2]))) &&
               8 != bsl::atoi(argv[2]))) {
             cout << "Case -6 is for INTERNAL USE ONLY.  GO AWAY." << endl;
             exit(9);
          }
          if (verbose) {
             cerr << "CASE 6 Child Process Running" << endl;
           }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);
          Obj mX(&mappingManager);
          H h1, dum;

          const char *BUF1 = "abcdefghi";
          const char *BUF2 = "j";
          const char *BUF3 = "klm";

          ASSERT(0 == mX.create(argv[3], MODE_RW_AUTOCOMMIT));
          mX.addRecord(&h1, (void*)BUF1, (int)strlen(BUF1)+1);
          mX.addRecord(&dum, (void*)BUF2, (int)strlen(BUF2)+1);
          mX.addRecord(&dum, (void*)BUF3, (int)strlen(BUF3)+1);
          mX.confirmRecord(h1);
          cout << 'A' << endl;
          bcemt_ThreadUtil::sleep(bdet_TimeInterval(4));
          mX.close();
          if (verbose) {
             cerr << "CASE 6 Child Process Done" << endl;
          }

      } break;
      case 6: {
          // ------------------
          // EXPLICIT_COMMIT / READ_ONLY
          //
          // This case tests both the EXPLICIT_COMMIT and READ_ONLY features.
          // We spawn a child process to execute case -6 of this test driver.
          // That process will write three small records to the journal
          // in EXPLICIT_COMMIT mode.  This process will wait until the child
          // has indicated the records have been written, and then copy the
          // backing file and open it READ_ONLY to validate the three small
          // records exist.  The child process will then close the journal
          // and terminate.  This process will then re-validate the records,
          // using the original journal file.
          //
          // At that point this process will do some READ_ONLY tests -
          // attempting to add, remove, and confirm records.  All attempts
          // should fail.
          //
          // The child process is spawned as "argv[0] -6 X" where X indicates
          // a verbosity flag.
          // -------------------
          if (verbose) {
             cout << "EXPLICIT_COMMIT / READ_ONLY MULTIPROCESS TEST" << endl
                  << "=============================================" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          char filename[MAX_TMPFILENAME], filename2[MAX_TMPFILENAME];
          getTmpFileName(filename, 6);
          getTmpFileName(filename2, 6);

          char buffer[512];
          snprintf(buffer, 512, "%s -6 %d %s", argv[0],
                   verbose? 9 : 8, filename);
          if (verbose) {
              P(buffer);
          }
          FILE* childFD = popen(buffer, "r");
          BSLS_ASSERT(0 != childFD);  // test invariant
          char *s = fgets(buffer, 3, childFD);
          if (!s) {
             cout << "Failed to read data from the child process." << endl;
             exit(9);
          }
          if (buffer[0] != 'A') {
              cout << "Communication error with child process: "
                  << buffer << " vs.  'A'" << endl;
              exit(9);
          }
          snprintf(buffer, 512, "cp -f %s %s", filename, filename2);
          system(buffer);

          bcema_TestAllocator ta;
          {
             Obj mX(&mappingManager, &ta);
             ASSERT(0 == mX.open(filename2, MODE_RO));

             Obj::RecordHandle handle;
             const char* rec = "abcde";
//             ASSERT(0 != mX.addRecord(&handle, (void*)rec,
//                                      (int)strlen(rec)+1));

             for (int i = 0; i < 2; ++i) {
                bool stringFound[2] = {0,0};

                Obj::RecordHandle handle = mX.firstConfirmedRecord();
                bcema_Blob* blob = new bcema_Blob;
                LOOP_ASSERT(i, handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                LOOP_ASSERT(i, 10 == mX.getRecordData(blob, handle));
                LOOP_ASSERT(i, 0 == verifyBlob(blob, "abcdefghi"));

                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                       mX.nextConfirmedRecord(handle));

                handle = mX.firstUnconfirmedRecord();
                ASSERT(handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                mX.getRecordData(blob, handle);
                if (0 == verifyBlob(blob, "j")) {
                   stringFound[0] = true;
                }
                if (0 == verifyBlob(blob, "klm")) {
                   stringFound[1] = true;
                }

                handle = mX.nextUnconfirmedRecord(handle);
                ASSERT(handle != Obj::BAECS_INVALID_RECORD_HANDLE);
                mX.getRecordData(blob, handle);
                if (0 == verifyBlob(blob, "j")) {
                   stringFound[0] = true;
                }
                if (0 == verifyBlob(blob, "klm")) {
                   stringFound[1] = true;
                }

                ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE ==
                       mX.nextUnconfirmedRecord(handle));

                ASSERT(stringFound[0] && stringFound[1]);

                delete blob;
                if (childFD) {
                   pclose(childFD);
                   childFD = 0;
                   mX.close();
                   ASSERT(0 == mX.open(filename,
                                              MODE_RO));
                }
             }

//             ASSERT(0 != mX.addRecord(&handle, (void*)rec,
//                                      (int)strlen(rec)+1));
             handle = mX.firstUnconfirmedRecord();
             ASSERT(Obj::BAECS_INVALID_RECORD_HANDLE != handle);
//             ASSERT(0 != mX.confirmRecord(handle));
//             ASSERT(0 != mX.removeRecord(handle));

             Obj::RecordHandle h2 = mX.firstUnconfirmedRecord();
             ASSERT(handle == h2);
             mX.close();
             bdesu_FileUtil::remove(filename);
             bdesu_FileUtil::remove(filename2);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
#endif
      case 5: {
          // ------------------
          // BLOB DATA TEST
          //
          // Concerns: that getRecordData(), which takes a blob, behaves
          // correctly.  In particular, it must be possible to get two
          // blobs on the same record and use one after the other has been
          // destroyed; it must be possible to copyRecordData() for a record
          // with an outstanding blob; and it must be possible to release and
          // then reuse a record previously returned as a blob (which has since
          // been destroyed).  All of this must be possible with a large record
          // also.
          //
          // Plan: Create a new journal with two records.  For one of the
          // records, pull the data into a blob as specified above.  Then
          // add another record which is much larger and repeat.  Run the
          // first test again after the much-larger-record test.
          // ------------------
          if (verbose) {
             cout << "Blob Data Test" << endl
                  << "==============" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                              NUM_PRIO);

          bcema_TestAllocator ta; // must use thread-safe allocator
          {
             H h1, h2, h3;
             const char *BUF1 = "abcdefghi";
             const char *BUF2 = "stuff: jkl";
             const int BUF3_LEN = (
                 baecs_JournalParameters::DEFAULT_BLOCK_SIZE *
                 baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE * 5) / 2;
             char *BUF3 = new char[BUF3_LEN];

             BUF3[BUF3_LEN-1] = 0;

             // Fill with random characters
             for (int i = 0; i < BUF3_LEN-1; ++i) {
                BUF3[i] = genericRand(26) + 'A';
             }
             char filename[MAX_TMPFILENAME];
             getTmpFileName(filename, 5);

             Obj mX(&mappingManager, &ta);
             ASSERT(0 == mX.create(filename, MODE_RW_AUTOCOMMIT));

             H dum;
             ASSERT(0 == mX.addRecord(&dum, (void*)BUF1, strlen(BUF1)+1));
             if (0 != case5(&mX, BUF2)) {
                cout << "(Error occured in first case5 run)" << endl;
             }
             if (0 != case5(&mX, BUF3)) {
                cout << "(Error occured in second case5 run)" << endl;
             }
             if (0 != case5(&mX, BUF2)) {
                cout << "(Error occured in third case5 run)" << endl;
             }

             delete[] BUF3;
             mX.close();
             bdesu_FileUtil::remove(filename);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;

      case 4: {
          // ------------------
          // MULTITHREADED CONFIRM COUNT TEST
          //
          // Concerns: That the confirmed/unconfirmed counts are maintained
          // correctly with one thread adding and removing records, and
          // another confirming them.  This also serves as a stress test
          // on the journal's thread safety.
          //
          // Plan: The main thread will serve as the adder and remover of
          // records and the checker of count correctness.  A second thread
          // will be spawned that will simply confirm records at random.
          // ------------------
          if (verbose) {
             cout << "Multithreaded Stress / Count Test" << endl
                  << " ================================" << endl;
          }

          enum {
              MAPPING_LIMIT = (1 << 20) * 100,
              NUM_ITERATIONS = 1000,
              NUM_OPERATIONS = 100
          };

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 4);
          if (verbose) {
             P(filename);
          }

          baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);
          bcema_TestAllocator ta; // must use thread-safe allocator
          struct Parameters {
             int d_bufferSize;
             int d_bytesPerBlock;
             int d_blocksPerPage;
          } parameters[] = {
             {32, 32, 256},
             {64, 256, 32},
             {32, 8192, 1},
             {32, DEF_ByPB, DEF_BlPB},
             {1550, 32, 256},
             {21550, 256, 32},
             {1550, DEF_ByPB, DEF_BlPB},
             {8200, 32, 256},
             {8200, 8192, 1},
             {8200, 8192, 8190},
             {8200, DEF_ByPB, DEF_BlPB}
          };

          for (int a = 0; a < sizeof(parameters)/sizeof(Parameters); ++a) {
             const Parameters& p = parameters[a];
             if (verbose) {
                P(p.d_bufferSize);
                P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                cout << "--------------------" << endl;
             }

             Obj mX(&mappingManager, &ta);

             baecs_JournalParameters jp;
             jp.blockSize() = p.d_bytesPerBlock;
             jp.blocksPerPage() = p.d_blocksPerPage;
             ASSERT(0 == mX.create(filename, MODE_RW, 0, jp));

             Case4Data data(&mX);
             bcemt_Attribute detached;
             bcemt_ThreadUtil::Handle dummy;
             detached.setDetachedState(bcemt_Attribute::BCEMT_CREATE_DETACHED);

             ASSERT(0 == bcemt_ThreadUtil::create(&dummy, detached,
                       bdef_BindUtil::bind(&Case4Data::confirmerThread,
                                           &data)));

             char *buffer = new char[p.d_bufferSize];
             strcpy(buffer, "Here is some data of length 31");

             H h;
             data.d_handles.resize(NUM_ITERATIONS*NUM_OPERATIONS + 1);
             // Add some records to get things started
             for (int i = 0; i < NUM_OPERATIONS; ++i) {
                ASSERT(0 == mX.addRecord(&h, (void*)buffer, p.d_bufferSize));
                data.d_handles[data.d_handlesSize++] = h;
             }
             data.d_bs.wait();

             for (int i = 0; i < NUM_ITERATIONS; ++i) {
                for (int j = 0; j < NUM_OPERATIONS; ++j) {
                   int add = Case4Data::mainRand(2);
                   if (add || 0 == data.d_handlesSize) {
                      ASSERT(0 == mX.addRecord(&h, (void*)buffer,
                                               p.d_bufferSize));
                      data.d_handles[data.d_handlesSize++] = h;
                   }
                   else {
                      bcemt_LockGuard<bcemt_Mutex>
                         guard(&data.d_removalLock);
                      int which = Case4Data::mainRand(data.d_handlesSize--);
                      H remove = data.d_handles[which];
                      if (veryVeryVerbose) {
                         cout << "(" << i << "," << j << "): removing "
                              << remove << endl;
                      }
                      for (int k = which; k < data.d_handlesSize; ++k) {
                         data.d_handles[k] = data.d_handles[k+1];
                      }
                      ASSERT(0 == mX.removeRecord(remove));
                      data.d_confirmed.erase(remove);
                   }
                }
                data.d_bp.wait();

                //Now we have the journal to ourselves until we wait on
                //data.d_bt.  Confirm that the count of confirmed and
                //unconfirmed records matches what's found by iteration
                int countedConfirmed = 0, countedUnconfirmed = 0;
                for (H iter = mX.firstUnconfirmedRecord();
                     iter != Obj::BAECS_INVALID_RECORD_HANDLE;
                     iter = mX.nextUnconfirmedRecord(iter)) {
                   ++countedUnconfirmed;
                }
                ASSERT(countedUnconfirmed == mX.numUnconfirmedRecords());

                for (H iter = mX.firstConfirmedRecord();
                     iter != Obj::BAECS_INVALID_RECORD_HANDLE;
                     iter = mX.nextConfirmedRecord(iter)) {
                   ++countedConfirmed;
                }
                ASSERT(countedConfirmed == mX.numConfirmedRecords());

                if (NUM_ITERATIONS-1 == i) {
                   //after this iteration we will stop.  Clear the flag
                   //here so the confirmer thread is guaranteed to see it
                   //before the next iteration
                   data.d_confirmerRunning = 0;
                }
                data.d_bt.wait();
             }
             delete [] buffer;
             mX.close();
             bdesu_FileUtil::remove(filename);
          }

          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 3: {
          // ------------------
          // LINK LIST BEHAVIOR TEST
          // (this is very much a 'white-box' test)
          //
          // Concerns: That the handles of removed records are made available
          // again for new records in the order in which they were removed,
          // and that the new records may be accessed using the
          // recycled handles.
          //
          // Plan: Create a new journal.  Add three records to it.  Upon
          // freeing a single record, ensure that its handle is reassigned
          // to the next record.  Upon freeing all records, ensure that their
          // handles are all reassigned in the expected order.  Then use the
          // recycled handles to access and verify the records.
          // ------------------
          if (verbose) {
             cout << "Link List Behavior Test" << endl
                  << "=======================" << endl;
          }

          char filename[MAX_TMPFILENAME];
          getTmpFileName(filename, 3);
          if (verbose) {
             P(filename);
          }

          static const int DEF_ByPB =
             baecs_JournalParameters::DEFAULT_BLOCK_SIZE;
          static const int DEF_BlPB =
             baecs_JournalParameters::DEFAULT_BLOCKS_PER_PAGE;

          struct Parameters {
              int d_mappingLimit;
              int d_numPriorities;
              int d_bytesPerBlock;
              int d_blocksPerPage;
              int d_smallestRecordSize;
              int d_mult;
          } parameters[] =
                { {(1 << 20) * 100, 2, 32, 256,            1, 2},
                  {(1 << 20) * 100, 2, DEF_ByPB, DEF_BlPB, 1, 1},
                  {(1 << 20) * 100, 2, 2048, 128,          1, 1},
                  {(1 << 16),       2, 32, 256,            1, 2},
                  {(1 << 16),       2, DEF_ByPB, DEF_BlPB, 10, 1},
                  {(1 << 16),       2, 2048, 128,          50, 1},
                  {(1 << 20) * 100, 2, 32, 256,            15, 2},
                  {(1 << 20) * 100, 2, DEF_ByPB, DEF_BlPB, 15, 1},
                  {(1 << 20) * 100, 2, 2048, 128,          100, 1},
                  {(1 << 16),       2, 32, 256,            300, 11},
                  {(1 << 16),       2, DEF_ByPB, DEF_BlPB, 300, 2},
                  {(1 << 16),       2, 2048, 128,          1111, 1}};

          for (int i = 0; i < sizeof(parameters) / sizeof(Parameters); ++i) {
             const Parameters& p = parameters[i];
             if (verbose) {
                P(p.d_mappingLimit); P(p.d_bytesPerBlock);
                P(p.d_blocksPerPage);
                P(p.d_smallestRecordSize);
                cout << "-----------------------" << endl;
             }

             baecs_MappingManager mappingManager(p.d_mappingLimit,
                                                 p.d_numPriorities);

             bcema_TestAllocator ta; // must use thread-safe allocator
             {
                H h1, h2, h3;
                char *buf1[2], *buf2[2], *buf3[2];
                case3AllocateBuffers(buf1, buf2, buf3,
                                      p.d_smallestRecordSize);
                int maxsize = p.d_smallestRecordSize*12 + 1;
                char* compareBuffer = (char*)malloc(maxsize);

                Obj mX(&mappingManager, &ta);

                baecs_JournalParameters jp;
                jp.blockSize() = p.d_bytesPerBlock;
                jp.blocksPerPage() = p.d_blocksPerPage;
                ASSERT(0 == mX.create(filename, MODE_RW, 0, jp));

                ASSERT(0 == mX.addRecord(&h1, (void*)buf1[0],
                                         (int)strlen(buf1[0])));
                LOOP_ASSERT(h1, 0 == h1);
                ASSERT(0 == mX.addRecord(&h2, (void*)buf2[0],
                                         (int)strlen(buf2[0])));
                LOOP2_ASSERT(h2, p.d_mult, p.d_mult * 1 == h2);
                ASSERT(0 == mX.addRecord(&h3, (void*)buf3[0],
                                         (int)strlen(buf3[0])));
                LOOP2_ASSERT(h3, p.d_mult, p.d_mult * 2 == h3);

                ASSERT(0 == mX.removeRecord(h2));

                ASSERT(2 == mX.numUnconfirmedRecords());

                ASSERT(0 == mX.addRecord(&h2, (void*)buf2[1],
                                         (int)(strlen(buf2[1]) + 1)));

                ASSERT(3 == mX.numUnconfirmedRecords());
                LOOP2_ASSERT(h2, p.d_mult, p.d_mult * 1 == h2);

                ASSERT(strlen(buf2[1])+1 == mX.copyRecordData(compareBuffer,
                                                              maxsize, h2));
                int rc = strcmp(compareBuffer, buf2[1]);
                ASSERT(0 == rc);
                if (0 != rc)
                {
                    P(compareBuffer);
                    P(buf2[1]);
                    P(maxsize);
                    P(strlen(buf2[1]) + 1);
                }

                // Now repeat, but removing/replacing *all* records instead
                // of just one
                ASSERT(0 == mX.removeRecord(h2));
                ASSERT(0 == mX.removeRecord(h1));
                ASSERT(0 == mX.removeRecord(h3));

                ASSERT(0 == mX.numUnconfirmedRecords());

                ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE ==
                       mX.firstConfirmedRecord());

                ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE ==
                       mX.firstUnconfirmedRecord());

                ASSERT(0 == mX.addRecord(&h1, (void*)buf1[1],
                                         (int)(strlen(buf1[1])+1)));

                ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE ==
                       mX.firstConfirmedRecord());

                LOOP2_ASSERT(h1, p.d_mult, p.d_mult * 2 == h1);

                LOOP2_ASSERT(mX.firstUnconfirmedRecord(), p.d_mult,
                            p.d_mult * 2 == mX.firstUnconfirmedRecord());

                ASSERT(0 == mX.addRecord(&h2, (void*)buf2[0],
                                         (int)(strlen(buf2[0])+1)));
                LOOP_ASSERT(h2, 0 == h2);
                ASSERT(0 == mX.addRecord(&h3, (void*)buf3[1],
                                         (int)(strlen(buf3[1])+1)));
                LOOP2_ASSERT(h3, p.d_mult, p.d_mult * 1 == h3);

                ASSERT(strlen(buf1[1])+1 == mX.copyRecordData(compareBuffer,
                                                              maxsize, h1));
                ASSERT(0 == strcmp(compareBuffer, buf1[1]));
                ASSERT(strlen(buf2[0])+1 == mX.copyRecordData(compareBuffer,
                                                              maxsize, h2));
                ASSERT(0 == strcmp(compareBuffer, buf2[0]));
                ASSERT(strlen(buf3[1])+1 == mX.copyRecordData(compareBuffer,
                                                              maxsize, h3));
                ASSERT(0 == strcmp(compareBuffer, buf3[1]));

                mX.close();
                bdesu_FileUtil::remove(filename);
                free(buf1[0]); free(buf1[1]);
                free(buf2[0]); free(buf2[1]);
                free(buf3[0]); free(buf3[1]);
                free(compareBuffer);
             }
             ASSERT(0 < ta.numAllocations());
             ASSERT(0 == ta.numBytesInUse());
          }
      } break;
      case 2: {
          // ------------------
          // USAGE EXAMPLE TEST
          // ------------------
          if (verbose) {
             cout << "Usage Example Test" << endl
                  << "==================" << endl;
          }
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(
                  MAPPING_LIMIT,
                  baecs_Journal::BAECS_NUM_PRIORITIES);

          bcema_TestAllocator ta; // must use thread-safe allocator
          {
             baecs_Journal mX(&mappingManager, &ta);
             char filename[MAX_TMPFILENAME];
             getTmpFileName(filename, 2);

             if (mX.create(filename, baecs_Journal::BAECS_READWRITE |
                                     baecs_Journal::BAECS_SAFE))
             {
                bsl::printf("Can't open journal %s: %d\n", filename, errno);
                exit(-1);
             }

             enum {
                NUM_RECORDS = 1000,
                MAX_RECORD_SIZE = 13243
             };

             if (verbose) {
                P(filename);
                P(NUM_RECORDS);
                P(MAX_RECORD_SIZE);
             }

             for (int i = 0; i < NUM_RECORDS; ++i) {
                char record[MAX_RECORD_SIZE];
                baecs_Journal::RecordHandle handle;
                int rc = mX.addRecord(&handle, record, MAX_RECORD_SIZE - 1);
                ASSERT(0 == rc);
                ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != handle);
             }
             if (verbose) {
                bsl::cout << "-> Wrote "
                          << NUM_RECORDS << " records into a journal."
                          << bsl::endl;
             }

             ASSERT(NUM_RECORDS == mX.numUnconfirmedRecords());
             ASSERT(0 == mX.numConfirmedRecords());

             baecs_Journal::RecordHandle handle = mX.firstUnconfirmedRecord();
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                int size = mX.getRecordLength(handle);
                ASSERT(MAX_RECORD_SIZE - 1 == size);
                handle = mX.nextUnconfirmedRecord(handle);
             }

             handle = mX.firstUnconfirmedRecord();
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                baecs_Journal::RecordHandle next =
                   mX.nextUnconfirmedRecord(handle);
                mX.confirmRecord(handle);
                handle = next;
             }

             ASSERT(0 == mX.numUnconfirmedRecords());
             ASSERT(NUM_RECORDS == mX.numConfirmedRecords());

             mX.close();
             bdesu_FileUtil::remove(filename);
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
      case 1: {
         // --------------
         // BREATHING TEST
         // --------------
        if (verbose) {
           cout << "Breathing Test" << endl
                << "==============" << endl;
        }
        enum {
            MAPPING_LIMIT = (1 << 20) * 100 // 100 MB
        }; // 100 MB
        baecs_MappingManager mappingMgr(MAPPING_LIMIT, NUM_PRIO);

        char filename[MAX_TMPFILENAME];
        getTmpFileName(filename, 1);

        bcema_TestAllocator ta(veryVeryVerbose); // must use thread-safe
                                                 // allocator
        {
           baecs_Journal mX(&mappingMgr, &ta);
           ASSERT(0 == mX.create(filename));

           char record[10000];
           baecs_Journal::RecordHandle handle;
           ASSERT(0 == mX.addRecord(&handle, record, 10000));
           ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != handle);
        }
        bdesu_FileUtil::remove(filename);
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case -3: {
        // --------------
        // EXPLICIT/IMPLICIT/timed commit TEST
        //
        // Measure the performance difference between three strategies:
        // a journal with EXPLICIT commit mode, a journal with IMPLICIT
        // commit mode, and a journal with timed commit (IMPLICIT commit
        // with commit() called periodically).  Try a timed commit period
        // of 0.5 and 2.0 seconds, for a total of four tests.
        //
        // Before the test is run, a randomized test plan is generated
        // such that all tests are performing the same actions in the same
        // order.
        // --------------
        if (verbose) {
           cout << "Commit Strategy Test" << endl
                << "====================" << endl;
        }

        enum {NUM_STEPS = 300000};
        enum {
            MAPPING_LIMIT = (1 << 20) * 100
        }; // 100 MB
        baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);

        Obj implicitJournal(&mappingManager);
        Obj explicitJournal(&mappingManager);
        char filename[MAX_TMPFILENAME];
        tmpnam_r(filename);

        volatile double elapsed = 0;
        CaseNeg3TestPlan plan;
        plan.generate(NUM_STEPS);

        // PHASE 1: FAST MODE

        ASSERT(0 == implicitJournal.create(filename, MODE_RW));

        CaseNeg3 phase2(&implicitJournal, &plan, &elapsed);
        phase2();

        cout << "Implicit Commit: elapsed time: " << elapsed << endl;

        implicitJournal.close();
        bdesu_FileUtil::remove(filename);

        // PHASE 2: AUTOCOMMIT MODE

        ASSERT(0 == explicitJournal.create(filename, MODE_RW_AUTOCOMMIT));

        CaseNeg3 phase1(&explicitJournal, &plan, &elapsed);
        phase1();

        cout << "Explicit Commit: elapsed time: " << elapsed << endl;

        explicitJournal.close();
        bdesu_FileUtil::remove(filename);

        bcep_TimerEventScheduler scheduler;
        scheduler.start();

        // PHASE 3: TIMED COMMIT - 0.5 sec

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase3(&implicitJournal, &plan, &elapsed);
        bcemt_ThreadUtil::Handle threadHandle;
        bcep_TimerEventScheduler::Handle clockHandle;
        clockHandle = scheduler.startClock
           (bdet_TimeInterval(0.5),
            bdef_BindUtil::bind(commitJournal, &implicitJournal));
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase3));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Timed Commit 0.5: elapsed time: " << elapsed << endl;

        scheduler.cancelClock(clockHandle, true);
        implicitJournal.close();
        bdesu_FileUtil::remove(filename);

        // PHASE 4: TIMED COMMIT - 2 sec

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase4(&implicitJournal, &plan, &elapsed);
        clockHandle = scheduler.startClock
           (bdet_TimeInterval(2),
            bdef_BindUtil::bind(commitJournal, &implicitJournal));
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase4));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Timed Commit 2.0: elapsed time: " << elapsed << endl;

        scheduler.cancelClock(clockHandle, true);
        implicitJournal.close();
        bdesu_FileUtil::remove(filename);
        // PHASE 5: NO COMMIT

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase5(&implicitJournal, &plan, &elapsed);
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase5));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Safe Mode But No Commit: elapsed time: " << elapsed << endl;

        implicitJournal.close();
        bdesu_FileUtil::remove(filename);
        break;
      }
      case -4: {
        // --------------
        // timed commit
        //
        // Case -3, but with only the timed commit test, with the
        // period taken from the first argument
        // --------------
        if (verbose) {
           cout << "Commit Strategy Test: Timed Only" << endl
                << "================================" << endl;
        }
        else {
           cout << "Case -4 requires an argument." << endl;
           exit(9);
        }

        BSLS_ASSERT(argc >= 3);
        double period = strtod(argv[2], 0);
        if (!(0 < period && period < 10000)) {
           cout << "Period argument " << period << " is incorrect." << endl;
           exit(9);
        }

        enum {NUM_STEPS = 300000};
        enum {
            MAPPING_LIMIT = (1 << 20) * 100
        }; // 100 MB

        baecs_MappingManager mappingManager(MAPPING_LIMIT,
                                            NUM_PRIO);

        Obj implicitJournal(&mappingManager);
        char filename[MAX_TMPFILENAME];
        tmpnam_r(filename);

        volatile double elapsed = 0;
        CaseNeg3TestPlan plan;
        plan.generate(NUM_STEPS);

        bcep_TimerEventScheduler scheduler;
        scheduler.start();

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase3(&implicitJournal, &plan, &elapsed);
        bcemt_ThreadUtil::Handle threadHandle;
        bcep_TimerEventScheduler::Handle clockHandle;
        clockHandle = scheduler.startClock
           (bdet_TimeInterval(period),
            bdef_BindUtil::bind(commitJournal, &implicitJournal));
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase3));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Timed Commit " << period
             << ": elapsed time: " << elapsed << endl;

        scheduler.cancelClock(clockHandle, true);
        implicitJournal.close();
        bdesu_FileUtil::remove(filename);
        break;
      }
      case -10: {
        if (verbose) {
            cout << "MT-threaded addRecord of constant block sizes" << endl
                 << "=============================================" << endl;

        }

        enum {
            MAPPING_LIMIT = (1 << 20) * 100
        }; // 100 MB

        baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);
        Obj mX(&mappingManager);
        char filename[MAX_TMPFILENAME];
        tmpnam_r(filename);
        ASSERT(0 == mX.create(filename, MODE_RW));
        P(filename);

        const int recordSizes[] =  // multiple of block size
        {
            1,
            2,
            3,
            4,
            8,
            16,
            32,
            64,
            128,
            256,
            512,
            1024,
            2048
        };

        enum {
            NUM_THREADS = 5,
            TOTAL_PAGES = 100

        };

        int numRecordSizes = sizeof recordSizes / sizeof *recordSizes;
        for (int i = 0; i < numRecordSizes; ++i)
        {
            int recordSize   = recordSizes[i];
            int blocksPerPage = mX.blocksPerPage();
            int numRecords   =
                TOTAL_PAGES * blocksPerPage / (NUM_THREADS * recordSize);
            P(numRecords);

            bcemt_ThreadGroup th;
            bdef_Function<void(*)(void)> cb;
            cb = bdef_BindUtil::bind(&caseNeg10Test,
                                     &mX,
                                     numRecords,
                                     recordSizes[i]);
            th.addThreads(cb, NUM_THREADS);
            th.joinAll();
        }
        bsl::remove(filename);
      } break;
      case -33:{
        // --------------
        // IMPLICIT/timed commit TEST
        //
        // Identical to case -3 but without the EXPLICIT commit timing.
        // --------------
        if (verbose) {
           cout << "Commit Strategy Test: Implicit/Timed Only" << endl
                << "=========================================" << endl;
        }

        enum { NUM_STEPS = 450000 };
        enum { MAPPING_LIMIT = (1 << 20) * 100 }; // 100 MB
        baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);

        Obj implicitJournal(&mappingManager);
        char filename[MAX_TMPFILENAME];
        tmpnam_r(filename);

        volatile double elapsed = 0;
        CaseNeg3TestPlan plan;
        plan.generate(NUM_STEPS);

        // PHASE 1: IMPLICIT COMMIT

        ASSERT(0 == implicitJournal.create(filename, MODE_RW));

        CaseNeg3 phase2(&implicitJournal, &plan, &elapsed);
        phase2();

        cout << "Implicit Commit: elapsed time: " << elapsed << endl;

        implicitJournal.close();
        bdesu_FileUtil::remove(filename);
        bcep_TimerEventScheduler scheduler;
        scheduler.start();

        // PHASE 2: TIMED COMMIT - 0.5 sec

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase3(&implicitJournal, &plan, &elapsed);
        bcemt_ThreadUtil::Handle threadHandle;
        bcep_TimerEventScheduler::Handle clockHandle;
        clockHandle = scheduler.startClock
           (bdet_TimeInterval(0.5),
            bdef_BindUtil::bind(&Obj::commit, &implicitJournal));
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase3));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Timed Commit 0.5: elapsed time: " << elapsed << endl;

        scheduler.cancelClock(clockHandle, true);
        implicitJournal.close();
        bdesu_FileUtil::remove(filename);

        // PHASE 3: TIMED COMMIT - 2 sec

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase4(&implicitJournal, &plan, &elapsed);
        clockHandle = scheduler.startClock
           (bdet_TimeInterval(2),
            bdef_BindUtil::bind(&Obj::commit, &implicitJournal));
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase4));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Timed Commit 2.0: elapsed time: " << elapsed << endl;

        scheduler.cancelClock(clockHandle, true);
        implicitJournal.close();
        bdesu_FileUtil::remove(filename);

        // PHASE 5: NO COMMIT

        ASSERT(0 == implicitJournal.create(filename, MODE_RW_SAFE));

        CaseNeg3 phase5(&implicitJournal, &plan, &elapsed);
        ASSERT(0 == bcemt_ThreadUtil::create(&threadHandle, phase5));
        bcemt_ThreadUtil::join(threadHandle);

        cout << "Safe Mode But No Commit: elapsed time: " << elapsed << endl;

        implicitJournal.close();
        bdesu_FileUtil::remove(filename);
        break;
      }
// TBD bcecs_persistentjournal is obsolete
#if 0
      case -2: {
            OldJournalWrapper   journal("oldbenchmark.mmap");
            JournalBenchmark    benchmark(&journal);
            benchmark.run(argc-2, argv+2);
      } break;
#endif
      case -1: {
            NewJournalWrapper   journal("newbenchmark.mmap",atoi(argv[2]));
            JournalBenchmark    benchmark(&journal);
            benchmark.run(argc-3, argv+3);
      } break;
      case -20: {
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);
          bcema_TestAllocator ta;
          baecs_Journal mX(&mappingManager, &ta);
          system("cp -f bad.mmap bad.mmap.test");
          int rc = mX.create("bad.mmap.test", MODE_RW);
          ASSERT(0 != rc);
          mX.create("new.mmap.test", MODE_RW);
          ASSERT(0 != rc);
          mX.close();
      } break;
      case -19: {
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);
          bcema_TestAllocator ta;
          baecs_Journal mX(&mappingManager, &ta);

          enum { NUM_RECORDS = 4096 };
          if (argc > 2) {
              ASSERT(0 == mX.create(argv[2], MODE_RW_SAFE));
          }
          else {
              const char *filename = bsl::tmpnam(NULL);
              P(filename);
              ASSERT(0 == mX.create(filename, MODE_RW_SAFE));
          }

          for (int i = 0; i < NUM_RECORDS; ++i) {
              baecs_Journal::RecordHandle handle;
              mX.addRecord(&handle, (void*)"foo", (unsigned)3);
              ASSERT(baecs_Journal::BAECS_INVALID_RECORD_HANDLE != handle);
              mX.commit();
          }
          mX.validate(true);
          mX.close();
      } break;
      case -18: {
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB

          if (argc < 3)
          {
              bsl::cout << "This test case validates a journal." << bsl::endl
                        << "Usage: " << argv[0] << " -18 <journalName>"
                        << bsl::endl;
              exit(-18);
          }

          baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);
          bcema_TestAllocator ta;
          baecs_Journal mX(&mappingManager, &ta);
          mX.open(argv[2], MODE_RO);
          mX.validate(true);
          mX.close();
      } break;
      case -17: {
          enum {
              MAPPING_LIMIT = (1 << 20) * 100
          }; // 100 MB
          baecs_MappingManager mappingManager(MAPPING_LIMIT, NUM_PRIO);

          bcema_TestAllocator ta; // must use thread-safe allocator
          {
             baecs_Journal mX(&mappingManager, &ta);

             if (argc < 3)
             {
                 bsl::cout << "Usage: " << argv[0] << " -17 <journalName>"
                     << bsl::endl;
                 exit(-1);
             }

             int rc = mX.open(argv[2], MODE_RO);
             if (rc) {
                bsl::printf("Can't open journal %s: %d\n", argv[2], rc);
                exit(-1);
             }

             int numUnconfirmed = mX.numUnconfirmedRecords();
             printf("%d unconfirmed records reported, iterating... ",
                    numUnconfirmed);
             baecs_Journal::RecordHandle handle = mX.firstUnconfirmedRecord();
             int count = 0;
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                printf("%d ", handle);
                int size = mX.getRecordLength(handle);
                handle = mX.nextUnconfirmedRecord(handle);
                ++count;
             }
             printf("completed %d records.\n", count);
             ASSERT(numUnconfirmed == count);

             int numConfirmed = mX.numConfirmedRecords();
             printf("%d confirmed records reported, iterating... ",
                    numConfirmed);
             handle = mX.firstConfirmedRecord();
             count = 0;
             while (handle != baecs_Journal::BAECS_INVALID_RECORD_HANDLE)
             {
                printf("%d ", handle);
                int size = mX.getRecordLength(handle);
                handle = mX.nextConfirmedRecord(handle);
                ++count;
             }
             printf("completed %d records.\n", count);
             ASSERT(numConfirmed == count);

             mX.close();
          }
          ASSERT(0 < ta.numAllocations());
          ASSERT(0 == ta.numBytesInUse());
      } break;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
