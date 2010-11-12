// bcecs_persistentjournal.t.cpp      -*-C++-*-

#include <bcecs_persistentjournal.h>

#include <bdema_testallocator.h> // **
#include <bcema_testallocator.h>
#include <bcemt_barrier.h>
#include <bcemt_thread.h>

#include <bdes_platformutil.h>
#include <bdes_timeutil.h>


#include <vector>
#include <iostream>
#include <stdio.h>

#ifdef BDES_PLATFORM__OS_UNIX
    #include <unistd.h>
#endif

#ifdef BDES_PLATFORM__OS_WINDOWS
    #undef min
    #undef max
#endif


using namespace BloombergLP;
using namespace std;
//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing 'PersistentJournal' is divided into 2 parts (apart from 
// breathing test and usage example).  
//-----------------------------------------------------------------------------
// CREATORS
// [  ] 
// [  ] 
//
// MANIPULATORS
// [  ] 
// [  ] 
//
// ACCESSORS
// [  ] 
// [  ] 
//-----------------------------------------------------------------------------
// [01] BREATHING TEST
// [02] SINGLE THREADED PERFORMANCE TEST
// [03] TESTING LINK LIST FUNCTIONS
// [04] 
// [05] MULTI-THREADED PERFORMANCE TESTING
// [06] TESTING LINK LIST FUNCTIONALITY
// [07] TESTRING COPY RECORD FROM AND TO BLOB
// [08] TESTING JOURNAL-GROWING LOGIC
// [09] USER_DATA_LENGTH TEST
// [10] TESTING MAX_CONFIRMED_RECORDS_KEPT AND MAX_CONFIRMED_BYTES_KEPT
// [11] TESTING REMAINING ACCESSORS
// [12] TYPICAL USAGE MULTITHREADED BENCHMARK
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

//=============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline
//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------



static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bcecs_PersistentJournal Obj;
typedef Obj::RecordHandle H;
typedef bcecs_PersistentJournalIterator It;
typedef BloombergLP::bdes_PlatformUtil::Int64 T;
char g_buf[1000000];

int rmFile(const char *file) {
#ifdef BDES_PLATFORM__OS_UNIX
    return ::unlink(file);
#else
    return DeleteFile(file)? 0: -1;
#endif
}



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

struct RecordInfo {
    int d_id;
    int d_len;
    char d_char;
    bdet_Datetime d_creationDateTime;
    bdet_Datetime d_confirmationDateTime;
};

struct JournalInfo {
    std::vector<int>         d_freeBlocks;
    std::vector<RecordInfo>  d_confRecInfos;
    std::vector<RecordInfo>  d_unconfRecInfos;
    int                      d_highestBlockUsed;
    int                      d_numPageTables;
};


void print(const JournalInfo& ji) {
    puts("Journal Contents:");
    puts("=================");
    printf("free blocks:\n    ");
    for (int i = 0; i < ji.d_freeBlocks.size(); ++i) {
        printf("%d %s", 
               ji.d_freeBlocks[i], 
               ((i == ji.d_freeBlocks.size()-1)?  "\n":  ""));
    }

    printf("confirmed records:\n");
    for (int i = 0; i < ji.d_confRecInfos.size(); ++i) {
        int id = ji.d_confRecInfos[i].d_id;
        int len = ji.d_confRecInfos[i].d_len;
        char ch = ji.d_confRecInfos[i].d_char;
        bdet_Datetime creationDateTime = 
            ji.d_confRecInfos[i].d_creationDateTime;
        bdet_Datetime confirmationDateTime = 
            ji.d_confRecInfos[i].d_confirmationDateTime;
        
        printf("    id = %d, len = %d, char = %c\n", id, len, ch);
    }

    printf("unconfirmed records:\n");
    for (int i = 0; i < ji.d_unconfRecInfos.size(); ++i) {
        int id = ji.d_unconfRecInfos[i].d_id;
        int len = ji.d_unconfRecInfos[i].d_len;
        char ch = ji.d_unconfRecInfos[i].d_char;
        bdet_Datetime creationDateTime = 
            ji.d_unconfRecInfos[i].d_creationDateTime;
        bdet_Datetime confirmationDateTime = 
            ji.d_unconfRecInfos[i].d_confirmationDateTime;
        
        printf("    id = %d, len = %d, char = %c\n", id, len, ch);
    }
    printf("highestUsedBlock = %d, numPageTables = %d", ji.d_highestBlockUsed,
           ji.d_numPageTables);
}



class SimpleBlobBufferFactory : public bcema_BlobBufferFactory
{
    bdema_Allocator *d_allocator_p;
    std::size_t      d_bufferSize;

    private:
    // not implemented
    SimpleBlobBufferFactory(const SimpleBlobBufferFactory&);
    SimpleBlobBufferFactory& operator=(const SimpleBlobBufferFactory&);

    public:
    // CREATORS
    explicit SimpleBlobBufferFactory(int              bufferSize = 1024,
                                     bdema_Allocator *basicAllocator = 0);
    ~SimpleBlobBufferFactory();

    // MANIPULATORS
    void allocate(bcema_BlobBuffer *buffer);
};

SimpleBlobBufferFactory::SimpleBlobBufferFactory(
                                           int              bufferSize,
                                           bdema_Allocator *basicAllocator)
: d_bufferSize(bufferSize)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
}

SimpleBlobBufferFactory::~SimpleBlobBufferFactory()
{
}

void SimpleBlobBufferFactory::allocate(bcema_BlobBuffer *buffer)
{

    bcema_SharedPtr<char> shptr(
                            (char *) d_allocator_p->allocate(d_bufferSize),
                            d_allocator_p);

    buffer->reset(shptr, d_bufferSize);
}


void fillBuf(char *buf, int SIZE, char CHAR)
{
    for (int i = 0; i < SIZE; ++i) {
        *buf++ = CHAR++;
    }
}
void fillBlob(bcema_Blob* blob, int SIZE, char CHAR)
{
    blob->setLength(SIZE);
    int remainingBytes = SIZE;
    for (int i = 0; remainingBytes; ++i) {
        const bcema_BlobBuffer& blobBuf = blob->buffer(i);
        char*          buf = blobBuf.data();
        int         bufLen = blobBuf.size();


        int cpyLen = std::min(remainingBytes, bufLen);

        fillBuf(buf, cpyLen, CHAR);
        CHAR += cpyLen;

        

        remainingBytes -= cpyLen;
    }
}

bool validateBuf(const char *buf, int SIZE, char CHAR)
{
    for (int i = 0; i < SIZE; ++i) {
        if (*buf++ != CHAR++)
            return false;
    }
    return true;
}

bool validateBlob(const bcema_Blob& blob, int SIZE, char CHAR)
{ // FIX it
    for (int i = 0; i < blob.numBuffers(); ++i) {
        const bcema_BlobBuffer& blobBuf = blob.buffer(i);

        const char*    buf = blobBuf.data();
        int         bufLen = blobBuf.size();
        if (!validateBuf(buf, bufLen, CHAR))
            return false;
        CHAR += bufLen;
    }
    return true;    
}


namespace BloombergLP {
void bcecs_PersistentJournalTest(
    void *vp_info, const bcecs_PersistentJournal *o)
{
    JournalInfo *info = (JournalInfo *) vp_info;
    int i;

    info->d_freeBlocks.clear();
    info->d_confRecInfos.clear();
    info->d_unconfRecInfos.clear();

    o->fillFreeBlocks(&info->d_freeBlocks);



    for (It it = o->firstUnconfirmedRecord(); it; ++it) {
        RecordInfo recInfo;
        recInfo.d_id = *it;
        recInfo.d_len = it.recordLength();
        ASSERT(1 == it.data(&recInfo.d_char, 1));
        validateBlob(it.data(), it.recordLength(), recInfo.d_char);

        ASSERT(sizeof(g_buf) > it.recordLength());
        ASSERT(it.recordLength() == it.data(g_buf, it.recordLength()));
        validateBuf(g_buf, it.recordLength(), recInfo.d_char);


        recInfo.d_creationDateTime = it.creationDateTime();
        recInfo.d_confirmationDateTime = it.confirmationDateTime();
        info->d_unconfRecInfos.push_back(recInfo);
    }


    i = 0;
    for (It it = o->lastUnconfirmedRecord(); it; --it, ++i) {
        ASSERT(info->d_unconfRecInfos[info->d_unconfRecInfos.size()-i-1].d_id 
                                        == *it);
    }
    ASSERT(info->d_unconfRecInfos.size() == i);



    for (It it = o->firstConfirmedRecord(); it; ++it) {
        RecordInfo recInfo;
        recInfo.d_id = *it;
        recInfo.d_len = it.recordLength();
        ASSERT(1 == it.data(&recInfo.d_char, 1));
        validateBlob(it.data(), it.recordLength(), recInfo.d_char);

        ASSERT(sizeof(g_buf) > it.recordLength());
        ASSERT(it.recordLength() == it.data(g_buf, it.recordLength()));
        validateBuf(g_buf, it.recordLength(), recInfo.d_char);


        recInfo.d_creationDateTime = it.creationDateTime();
        recInfo.d_confirmationDateTime = it.confirmationDateTime();

        info->d_confRecInfos.push_back(recInfo);
    }


    i = 0;
    for (It it = o->lastConfirmedRecord(); it; --it, ++i) {
        ASSERT(info->d_confRecInfos[info->d_confRecInfos.size()-i-1].d_id 
                                                                      == *it);
    }
    ASSERT(info->d_confRecInfos.size() == i);

    info->d_highestBlockUsed = o->highestBlockUsed();
    info->d_numPageTables = o->numPageTables();
}
}
static void fillJournalInfo(JournalInfo *info, const bcecs_PersistentJournal
                            *o)
{

    bcecs_PersistentJournalTest((void *) info, o);
}



void menu() {
    puts("1. add STRING");
    puts("2. remove recId");
    puts("3. confirm recId");
    puts("4. print");
}

void printBlob(const bcema_Blob& data)
{
    for (int i = 0; i < data.numBuffers(); ++i) {
        const bcema_BlobBuffer& blobBuf = data.buffer(i);
        const char* buf = blobBuf.data();
        int bufLen = blobBuf.size();

        for (int j = 0; j < bufLen; ++j) {
            putchar(buf[j]);
        }
    }

}
#define STREAM_MEMBER(m) #m ": " << m
#define STREAM_MEMBER_L(inst,m) STREAM_MEMBER(inst.m) << std::endl

enum CmdType {
    ADD = 0, // addRecord
    CNF, // confirmRecord
    REM, // removeRecord
    DUM // dummy
};


const char * CmdTypeToString[] = {
    "ADD",
    "CNF",
    "REM",
    "DUM"
};
std::ostream& operator<< (std::ostream&     stream,
                          const CmdType& rhs)
{
    stream
        << CmdTypeToString[rhs] << std::endl;
    return stream;
}

struct Record {
    int d_len;  // 0 for last record
    int d_char;
    int d_blobBufferSize;
};
std::ostream& operator<< (std::ostream&     stream,
                          const Record& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs, d_len            )
        << STREAM_MEMBER_L(rhs, d_char            )
        << STREAM_MEMBER_L(rhs, d_blobBufferSize            )
        << std::endl;
    return stream;
}


struct Cmd {
    CmdType d_cmdType;
    int d_handle;
    Record d_record;
};
std::ostream& operator<< (std::ostream&     stream,
                          const Cmd& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs, d_cmdType            )
        << STREAM_MEMBER_L(rhs, d_handle            )
        << STREAM_MEMBER_L(rhs, d_record            )
        << std::endl;
    return stream;
}

struct RecordAndHandle {
    Record d_record;
    int d_handle;
};
std::ostream& operator<< (std::ostream&     stream,
                          const RecordAndHandle& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs, d_record            )
        << STREAM_MEMBER_L(rhs, d_handle            )
        << std::endl;
    return stream;
}


struct DATA {
    int d_line;
    Cmd d_cmd;
    RecordAndHandle d_unconfRecInfos[100];
    RecordAndHandle d_confRecInfos[100];
    int d_freeBlocks[100];
    int d_highestBlockUsed;
};



std::ostream& operator<< (std::ostream&     stream,
                          const DATA& rhs)
{
    stream
        << STREAM_MEMBER_L(rhs, d_line            )
        << STREAM_MEMBER_L(rhs, d_cmd            )
        << STREAM_MEMBER_L(rhs, d_highestBlockUsed            )
        << std::endl;

        for (int i = 0; rhs.d_freeBlocks[i] != -1; ++i) {
            stream << "d_freeBlocks[] = " << rhs.d_freeBlocks[i] << std::endl;
        }


        for (int i = 0; rhs.d_unconfRecInfos[i].d_record.d_len; ++i) {
            stream << "d_unconfRecInfos[] = " << rhs.d_unconfRecInfos[i] 
                   << std::endl;
        }


        for (int i = 0; rhs.d_confRecInfos[i].d_record.d_len; ++i) {
            stream << "d_confRecInfos[] = " << rhs.d_confRecInfos[i] 
                   << std::endl;
        }




    return stream;
}

int addRecord(Obj *j, const Record& r) {

    SimpleBlobBufferFactory fact(r.d_blobBufferSize);
    bcema_Blob blob(&fact);
    fillBlob(&blob, r.d_len, r.d_char);

    int handle;
    ASSERT(j->addRecord(&handle, blob) == 0);
    return handle;

}


void validate(const JournalInfo& ji, const DATA& d)
{
    int i;

    ASSERT(d.d_highestBlockUsed == ji.d_highestBlockUsed);
    for (i = 0; i < ji.d_freeBlocks.size(); ++i) {
        ASSERT(ji.d_freeBlocks[i] == d.d_freeBlocks[i]);
    }
    ASSERT(d.d_freeBlocks[i] == -1);


    for (i = 0; i < ji.d_unconfRecInfos.size(); ++i) {
        ASSERT(ji.d_unconfRecInfos[i].d_len == 
               d.d_unconfRecInfos[i].d_record.d_len);

        ASSERT(ji.d_unconfRecInfos[i].d_char == 
               d.d_unconfRecInfos[i].d_record.d_char);

        ASSERT(ji.d_unconfRecInfos[i].d_id == d.d_unconfRecInfos[i].d_handle);
    }
    ASSERT(d.d_unconfRecInfos[i].d_record.d_len == 0);

    for (i = 0; i < ji.d_confRecInfos.size(); ++i) {
        ASSERT(ji.d_confRecInfos[i].d_len == 
               d.d_confRecInfos[i].d_record.d_len);

        ASSERT(ji.d_confRecInfos[i].d_char == 
               d.d_confRecInfos[i].d_record.d_char);

        ASSERT(ji.d_confRecInfos[i].d_id == d.d_confRecInfos[i].d_handle);
    }
    ASSERT(d.d_confRecInfos[i].d_record.d_len == 0);
    

}

void process(const DATA *data, int size, Obj& o)
{
    for (int i = 0; i < size; ++i) {
        const DATA& d = data[i];
        if (veryVerbose) { P(d.d_line); }
        const Cmd& cmd = d.d_cmd;
        switch(cmd.d_cmdType) {
          case ADD: {
              ASSERT(addRecord(&o, cmd.d_record) == cmd.d_handle);
          }; break;

          case CNF: {
              ASSERT(o.confirmRecord(cmd.d_handle) == 0);
          }; break;

          case REM: {
              ASSERT(o.removeRecord(cmd.d_handle) == 0);
          }; break;

          case DUM: {
          }; break;

          default: {
              ASSERT(0);
          }; break;
        };
        JournalInfo ji;
        fillJournalInfo(&ji, &o);
        if (veryVerbose) { print(ji); /* std::cout << d << std::endl; */ }
        validate(ji, d);
    }
}


//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================
//                         CASE 12 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase12
{



enum { 
    DEFAULT_NUM_THREADS    = 3, 
    DEFAULT_NUM_RECORDS_TO_ADD = 10000,

    DEFAULT_NUM_INITIAL_UNCONFIRMED_RECORDS = 1000,
    DEFAULT_NUM_INITIAL_CONFIRMED_RECORDS = 500,

    DEFAULT_BLK_SIZE =   1024, 
    DEFAULT_NUM_BLKS_IN_A_PAGE =   1024, 
    DEFAULT_REC_SIZE =   DEFAULT_BLK_SIZE


};

int NUM_THREADS = DEFAULT_NUM_THREADS;
int NUM_RECORDS_TO_ADD = DEFAULT_NUM_RECORDS_TO_ADD;

int NUM_INITIAL_UNCONFIRMED_RECORDS = DEFAULT_NUM_INITIAL_UNCONFIRMED_RECORDS;
int NUM_INITIAL_CONFIRMED_RECORDS = DEFAULT_NUM_INITIAL_CONFIRMED_RECORDS;



int BLK_SIZE = DEFAULT_BLK_SIZE;
int NUM_BLKS_IN_A_PAGE = DEFAULT_NUM_BLKS_IN_A_PAGE;
int REC_SIZE = DEFAULT_REC_SIZE;






bcema_TestAllocator ta;
Obj o(Obj::IMPLICIT_COMMIT, &ta);

volatile int               unconfAvailCount;
bcemt_Mutex       unconfAvailMutex;
bcemt_Condition   unconfAvailCond;

volatile int               confAvailCount;
bcemt_Mutex       confAvailMutex;
bcemt_Condition   confAvailCond;


void signalCountUp(volatile int *count, 
                   bcemt_Mutex *lock, 
                   bcemt_Condition *cond)
    // increment and signal
{
    lock->lock();
    ++*count;
    cond->signal();
    lock->unlock();
}

void waitCountNonZero(volatile int *count, 
                      bcemt_Mutex *lock, 
                      bcemt_Condition *cond)
    // wait and decrement
{
    lock->lock();
    if (*count == 0)
        ASSERT(0 == cond->wait(lock));
    
    --*count;
    lock->unlock();
}


extern "C" { 
    void *workerThread12(void *arg)
    {

        T t1 = BloombergLP::bdes_TimeUtil::getTimer();




        H h;
        int id = (long)arg;
        switch(id) {
          case 0: {
              for(int i = 0; i < NUM_RECORDS_TO_ADD; ++i){

                  fillBuf(g_buf, REC_SIZE, 'A');
                  ASSERT(o.addRecord(&h, g_buf, REC_SIZE) == 0); 

              }
          }; break;

          case 1: {
              int n = NUM_RECORDS_TO_ADD + NUM_INITIAL_UNCONFIRMED_RECORDS;
              for(int i = 0; i < n; ++i){

                  while(1)
                  {
                      {
                          It it = o.firstUnconfirmedRecord();
                          if (it) {
                              h=*it;
                              break;
                          }
                      }
                      bcemt_ThreadUtil::microSleep(1);
                  }
                  ASSERT(o.confirmRecord(h) == 0); 
              }

          }; break;

          case 2: {
              int n = NUM_RECORDS_TO_ADD + NUM_INITIAL_UNCONFIRMED_RECORDS + NUM_INITIAL_CONFIRMED_RECORDS;
              for(int i = 0; i < n; ++i){
                  while(1)
                  {
                      {
                          It it = o.firstConfirmedRecord();
                          if (it) {
                              h=*it;
                              break;
                          }
                      }
                      bcemt_ThreadUtil::microSleep(1);
                  }


                  ASSERT(o.removeRecord(h) == 0); 
              }
          }; break;
          
          default: {
              ASSERT(0);
          }; break;
          
        }; // switch;
        

        T t2 = BloombergLP::bdes_TimeUtil::getTimer();
        P_(id); P(t2-t1);  
        return NULL;
    } // workerThread
} // extern "C"


/*
extern "C" { 
    void *workerThread12(void *arg)
    {
        H h;
        int id = (long)arg;
        switch(id) {
          case 0: {
              for(int i = 0; i < NUM_RECORDS_TO_ADD; ++i){

                  fillBuf(g_buf, REC_SIZE, 'A');
                  ASSERT(o.addRecord(&h, g_buf, REC_SIZE) == 0); 


                  signalCountUp(&unconfAvailCount, 
                                &unconfAvailMutex, 
                                &unconfAvailCond);
              }
          }; break;

          case 1: {
              for(int i = 0; i < NUM_RECORDS_TO_ADD; ++i){

                  waitCountNonZero(&unconfAvailCount, 
                                   &unconfAvailMutex, 
                                   &unconfAvailCond);
                  
                  {
                      It it = o.firstUnconfirmedRecord();
                      ASSERT(it);
                      h = *it;
                  }
                  ASSERT(o.confirmRecord(h) == 0); 

                  signalCountUp(&confAvailCount, 
                                &confAvailMutex, 
                                &confAvailCond);
                  
              }

          }; break;

          case 2: {
              for(int i = 0; i < NUM_RECORDS_TO_ADD; ++i){
                  waitCountNonZero(&confAvailCount, 
                                   &confAvailMutex, 
                                   &confAvailCond);
                  
                  {
                      It it = o.firstConfirmedRecord();
                      ASSERT(it);
                      h = *it;
                  }
                  ASSERT(o.removeRecord(h) == 0); 
              }
          }; break;
          
          default: {
              ASSERT(0);
          }; break;
          
        }; // switch;
        
        return NULL;
    } // workerThread
} // extern "C"

*/
} // namespace TestCase12
//=============================================================================
//                         CASE 10 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace TestCase10
{
} // namespace TestCase10
//=============================================================================
//                         CASE 9 RELATED ENTITIES
//-----------------------------------------------------------------------------

namespace TestCase9
{
} // namespace TestCase9
//=============================================================================
//                         CASE 8 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase8
{
} // namespace TestCase8
//=============================================================================
//                          CASE 7 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase7
{
} // namespace TestCase7
//=============================================================================
//                         CASE 6 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase6
{
} // namespace TestCase6
//=============================================================================
//                         CASE 5 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase5
{

enum { 
    DEFAULT_NUM_THREADS    = 3, 
    DEFAULT_NUM_ITERATIONS = 10000
};


enum {
    DEFAULT_BLK_SIZE =   1024, 
    DEFAULT_NUM_BLKS_IN_A_PAGE =   1024, 
    DEFAULT_REC_SIZE =   DEFAULT_BLK_SIZE
};

int NUM_THREADS = DEFAULT_NUM_THREADS;
int NUM_ITERATIONS = DEFAULT_NUM_ITERATIONS;
int BLK_SIZE = DEFAULT_BLK_SIZE;
int NUM_BLKS_IN_A_PAGE = DEFAULT_NUM_BLKS_IN_A_PAGE;
int REC_SIZE = DEFAULT_REC_SIZE;




const char *BUF  = "bla";


bcemt_Barrier *barrier;

bcema_TestAllocator ta;
Obj o(Obj::IMPLICIT_COMMIT, &ta);

extern "C" { 
    void *workerThread5(void *arg)
    {
        int id = (long)arg;
        H h;

        barrier->wait();
        for(int i = 0; i < NUM_ITERATIONS; ++i){

            fillBuf(g_buf, REC_SIZE, 'A');
            ASSERT(o.addRecord(&h, g_buf, REC_SIZE) == 0); 
            ASSERT(o.numUnconfirmedRecords() >= 1);
            ASSERT(o.numUnconfirmedRecords() <= 3);

            ASSERT(o.confirmRecord(h) == 0); 
            ASSERT(o.numConfirmedRecords() >= 1);
            ASSERT(o.numConfirmedRecords() <= 3);

            ASSERT(o.removeRecord(h) == 0); 
            ASSERT(o.numUnconfirmedRecords() <= 2);
            ASSERT(o.numConfirmedRecords() <= 2);

        }
        return NULL;
    }
} // extern "C"

} // namespace TestCase5
//=============================================================================
//                         CASE 4 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase4
{
} // namespace TestCase4
//=============================================================================
//                         CASE 3 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase3
{
} // namespace TestCase3
//=============================================================================
//                         CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase2
{
} // namespace TestCase2
//=============================================================================
//                         CASE 1 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace TestCase1
{
} // namespace TestCase1
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:  // Zero is always the leading case.

      case -12: {
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
/*
        if (verbose) cout << endl
                          << "Typeical usage test" << endl
                          << "=====================" << endl;



        enum {
            B = 32,
            N = 3
        };

        enum {
            MAX_CONFIRMED_RECORDS_KEPT = 2,
            MAX_CONFIRMED_BYTES_KEPT  = -1
        };
        char BUF[B];
        bcema_TestAllocator ta(veryVeryVerbose);
        
        {
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f12";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0,
                             MAX_CONFIRMED_RECORDS_KEPT, 
                             MAX_CONFIRMED_BYTES_KEPT, 
                             B, 
                             N) == 0);



        
        H h;
        char c = 'A';
        int i = 0;
        while(1) {
            BUF[0] = c + i%26;
            ASSERT(0 == o.addRecord(&h, (i == 2000)?  0:  BUF, B));
            ASSERT(0 == o.confirmRecord(h));
            ++i;
        }

        }
*/

      } break;


      case -1: {
        // --------------------------------------------------------------------
        // UTILITY TO PRINT HEADER OF A JOURNAL:
        // Usage: cmd -1 <journalFile>
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
        //   TESTING REMAINING ACCESSORS
        // --------------------------------------------------------------------

         
        if (argc != 3) {
            cerr << "Usage: cmd -1 <journalFile>" << endl;
            return -1;
        }


        H h;

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        int i;
        const char *FILE_NAME = argv[2];
        int rc = o.openJournal(FILE_NAME, Obj::READ_ONLY);
        if (rc) {
            cerr << "openJournal failed with rc = " << rc << endl;
            return -2;
        }

        o.printHeader(cout);
        return 0;

      } break;

#ifdef BDES_PLATFORM__OS_UNIX
      case 12: {
        // --------------------------------------------------------------------
        // TYPICAL USAGE MULTITHREADED BENCHMARK
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
        //   TYPICAL USAGE MULTITHREADED BENCHMARK
        // --------------------------------------------------------------------



        using namespace TestCase12;

        if (verbose) cout << endl
                          << "TYPICAL USAGE MULTITHREADED BENCHMARK" << endl
                          << "=====================================" << endl;


        const char *FILE_NAME = "f12";
        H h;

        // if (argc > 4) NUM_THREADS = std::atoi(argv[4]);
        if (argc > 4) NUM_RECORDS_TO_ADD = std::atoi(argv[4]);

        if (argc > 5) NUM_INITIAL_UNCONFIRMED_RECORDS = std::atoi(argv[5]);
        if (argc > 6) NUM_INITIAL_CONFIRMED_RECORDS = std::atoi(argv[6]);


        if (argc > 7) BLK_SIZE = std::atoi(argv[7]);
        if (argc > 8) NUM_BLKS_IN_A_PAGE = std::atoi(argv[8]);
        if (argc > 9) REC_SIZE = std::atoi(argv[9]);


        if (veryVerbose) {
            P(NUM_RECORDS_TO_ADD);
            P(NUM_INITIAL_UNCONFIRMED_RECORDS);
            P(NUM_INITIAL_CONFIRMED_RECORDS);

            P(NUM_THREADS);

            P(BLK_SIZE);
            P(NUM_BLKS_IN_A_PAGE);
            P(REC_SIZE);

            cout << "Usage: cmd <case> 1 1 <NUM_RECORDS_TO_ADD> <NUM_INITIAL_UNCONFIRMED_RECORDS> <NUM_INITIAL_CONFIRMED_RECORDS> <BLK_SIZE> <NUM_BLKS_IN_A_PAGE> <REC_SIZE>" << endl;
        }

        ASSERT(NUM_THREADS == 3);
        if (verbose) {
            std::cout
                << "Create 3 threads.  All threds runs a loop of " 
                << "NUM_RECORDS_TO_ADD.  In the loop, the first thread does " 
                << "'addRecord', second one does 'confirmRecord' and the "
                << "third one does 'removeRecord'.  "
                << std::endl;
        }

        rmFile(FILE_NAME);


        {
        Obj o1(Obj::IMPLICIT_COMMIT, &ta);            
        ASSERT(o1.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             BLK_SIZE, 
                             NUM_BLKS_IN_A_PAGE) == 0);

        std::vector<H> handles;

        int n = NUM_INITIAL_UNCONFIRMED_RECORDS + 
                                          NUM_INITIAL_CONFIRMED_RECORDS;
        for (int i = 0; i < n; ++i) {
            fillBuf(g_buf, REC_SIZE, 'A');
            ASSERT(o1.addRecord(&h, g_buf, REC_SIZE) == 0); 
            handles.push_back(h);

        }

        for (int i = 0; i < NUM_INITIAL_CONFIRMED_RECORDS; ++i) {
            ASSERT(o1.confirmRecord(handles[i]) == 0); 
        }
        

        }

        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             BLK_SIZE, 
                             NUM_BLKS_IN_A_PAGE) == 0);
        
        ASSERT(o.numUnconfirmedRecords() == NUM_INITIAL_UNCONFIRMED_RECORDS);
        ASSERT(o.numConfirmedRecords()   == NUM_INITIAL_CONFIRMED_RECORDS);
        bcemt_ThreadUtil::setConcurrency(NUM_THREADS + 1);

        T t1 = BloombergLP::bdes_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread12);
        T t2 = BloombergLP::bdes_TimeUtil::getTimer();
        

        if (verbose) {
            printf("Time in nanoseconds:" ); P(t2-t1);
        }

        ASSERT(o.numUnconfirmedRecords() == 0);
        ASSERT(o.numConfirmedRecords()   == 0);





      } break;
#endif
      case 11: {
        // --------------------------------------------------------------------
        // TESTING REMAINING ACCESSORS
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
        //   TESTING REMAINING ACCESSORS
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING REMAINING ACCESSORS" << endl
                          << "===========================" << endl;





        Obj::CommitMode commitMode = Obj::EXPLICIT_COMMIT;
        Obj::Mode mode = Obj::READ_WRITE;
        int alignmentSize = 64 * 1024;
        

        H h;

        enum {
            B = 32,
            N = 4
        };

        enum {
            MAX_CONFIRMED_RECORDS_KEPT = 2,
            MAX_CONFIRMED_BYTES_KEPT  = 2 * B
        };
        char BUF1[B];        char BUF2[2*B];
        bcema_TestAllocator ta(veryVeryVerbose);
        
        {
        Obj o(commitMode, &ta);
        const char *FILE_NAME = "f11";  rmFile(FILE_NAME);
        bdet_Datetime t1 = bdetu_SystemTime::nowAsDatetimeGMT();
        bcemt_ThreadUtil::microSleep(2000000);
            
        ASSERT(o.openJournal(FILE_NAME, 
                             mode, 
                             true, 
                             0,
                             MAX_CONFIRMED_RECORDS_KEPT,
                             MAX_CONFIRMED_BYTES_KEPT, 
                             B, 
                             N, 
                             alignmentSize) == 0);


        ASSERT(o.alignmentSize() == alignmentSize);
        ASSERT(o.mode() == mode);
        ASSERT(o.commitMode() == commitMode);


        bcemt_ThreadUtil::microSleep(2000000);
        ASSERT(0 == o.addRecord(&h, BUF1, B));

        bcemt_ThreadUtil::microSleep(2000000);
        ASSERT(0 == o.confirmRecord(h));

        ASSERT(t1 < o.journalCreationDateTime());
        ASSERT(o.journalCreationDateTime() < 
               o.firstConfirmedRecord().creationDateTime());

        ASSERT(o.firstConfirmedRecord().creationDateTime() < 
               o.firstConfirmedRecord().confirmationDateTime());

        if (verbose) {
            P(t1);
            P(o.journalCreationDateTime());
            P(o.firstConfirmedRecord().creationDateTime()); 
            P(o.firstConfirmedRecord().confirmationDateTime());
        };



        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING MAX_CONFIRMED_RECORDS_KEPT AND MAX_CONFIRMED_BYTES_KEPT
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
        //   TESTING MAX_CONFIRMED_RECORDS_KEPT AND MAX_CONFIRMED_BYTES_KEPT
        // --------------------------------------------------------------------
          if (verbose) 
              cout 
                  << endl
                  << "TESTING MAX_CONFIRMED_RECORDS_KEPT AND " 
                  << "MAX_CONFIRMED_BYTES_KEPT" << endl
                  << "=======================================" 
                  << "========================" << endl;

        using namespace TestCase10;


        enum {
            B = 32,
            N = 4
        };

        enum {
            MAX_CONFIRMED_RECORDS_KEPT = 2,
            MAX_CONFIRMED_BYTES_KEPT  = 2 * B
        };
        char BUF1[B];        char BUF2[2*B];
        bcema_TestAllocator ta(veryVeryVerbose);
        
        {
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f10";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0,
                             MAX_CONFIRMED_RECORDS_KEPT, 
                             MAX_CONFIRMED_BYTES_KEPT, 
                             B, 
                             N) == 0);


        H h1, h2, h3;
        ASSERT(0 == o.addRecord(&h1, BUF1, B));
        ASSERT(0 == o.addRecord(&h2, BUF1, B));
        ASSERT(0 == o.addRecord(&h3, BUF2, B*2));


        ASSERT(0 == o.confirmRecord(h1));
        LOOP_ASSERT(o.numConfirmedRecords(), o.numConfirmedRecords()   == 1);
        ASSERT(0 == o.confirmRecord(h2));
        LOOP_ASSERT(o.numConfirmedRecords(), o.numConfirmedRecords()   == 2);
        ASSERT(0 == o.confirmRecord(h3));
        LOOP_ASSERT(o.numConfirmedRecords(), o.numConfirmedRecords()   == 1);

        }

        {

        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f10";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             0, 
                             0, 
                             B, 
                             N) == 0);


        H h;
        ASSERT(0 == o.addRecord(&h, BUF1, B));
        ASSERT(0 == o.confirmRecord(h));
        ASSERT(o.numConfirmedRecords()   == 0);


        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // USER_DATA_LENGTH TEST:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   USER_DATA_LENGTH TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USER_DATA_LENGTH TEST" << endl
                          << "=====================" << endl;

        using namespace TestCase9;

        enum { USER_DATA_SIZE = 10 };


        enum {
            B = 32,
            N = 4
        };

        bcema_TestAllocator ta(veryVeryVerbose);
        
        {
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f9";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, Obj::READ_WRITE, true, USER_DATA_SIZE,
                             -1, -1, B, N) == 0);
        ASSERT(o.userDataLength() == USER_DATA_SIZE);
        char * userData = o.userData();
        *userData = 'X';
        }
        {
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f9";  
        ASSERT(o.openJournal(FILE_NAME, Obj::READ_WRITE, true, USER_DATA_SIZE,
                             -1, -1, B, N) == 0);
        ASSERT(o.userDataLength() == USER_DATA_SIZE);
        char * userData = o.userData();
        ASSERT(*userData == 'X');
        }


        

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING JOURNAL-GROWING LOGIC:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   TESTING JOURNAL-GROWING LOGIC:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING JOURNAL-GROWING LOGIC:" << endl
                          << "==============================" << endl;

        using namespace TestCase8;

        enum {
            B = 32,
            N = 4
        };

        Record R1 = {1*B, 'a', B} ;
        Record R2 = {2*B, 'b', B} ;
        Record R3 = {3*B, 'c', B} ;
        Record R4 = {4*B, 'd', B} ;
        Record R5 = {5*B, 'e', B} ;
        Record R6 = {6*B, 'f', B} ;
        Record R7 = {7*B, 'g', B} ;
        Record R8 = {8*B, 'h', B} ;
        Record R9 = {9*B, 'i', B} ;

        DATA data[] = {
            {
                L_,                                   // line
                { ADD, 0, R2 },                       // cmd
                { {R2, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,-1},                            // expected free list
                1                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 2, R1 },                       // cmd
                { {R2, 0}, {R1, 2}         },         // expected unconf
                {                          },         // expected conf
                { 3,-1},                            // expected free list
                2                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 2 },                       // cmd
                { {R2, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,-1},                            // expected free list
                2                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 2, R2 },                       // cmd
                { {R2, 0}, {R2, 2}          },         // expected unconf
                {                          },         // expected conf
                { -1},                            // expected free list
                3                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 2 },                       // cmd
                { {R2, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,-1},                            // expected free list
                3                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 4, R3 },                       // cmd
                { {R2, 0} , {R3, 4}                 },         // expected unconf
                {                          },         // expected conf
                { 7,2,3,-1},                            // expected free list
                6                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 4},                       // cmd
                { {R2, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 4,5,6,7,2,3,-1},                    // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},                   // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R1},                       // cmd
                { {R1, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 1,4,5,6,7,2,3,-1},                  // expected free list
                6                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},                // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R3},                       // cmd
                { {R3, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 5,6,7,2,3,-1},                    // expected free list
                6                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},                 // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R4},                       // cmd
                { {R4, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 6,7,2,3,-1},                            // expected free list
                6                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},                   // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R5},                       // cmd
                { {R5, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 7,2,3,-1},                            // expected free list
                6                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},              // expected free list
                6                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R7},                       // cmd
                { {R7, 0}                 },         // expected unconf
                {                          },         // expected conf
                { 3,-1},                            // expected free list
                7                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},          // expected free list
                7                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R8},                       // cmd
                { {R8, 0}                 },         // expected unconf
                {                          },         // expected conf
                { -1},                            // expected free list
                7                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 0},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 0,1,4,5,6,7,2,3,-1},              // expected free list
                7                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 8, R9},                       // cmd
                { {R9, 8}                 },         // expected unconf
                {                          },         // expected conf
                { 7,2,3,-1},                            // expected free list
                11                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 8},                       // cmd
                {                  },         // expected unconf
                {                          },         // expected conf
                { 8,9,10,11,0,1,4,5,6,7,2,3,-1},  // expected free list
                11                                     // exp highestBlockUsed
            }



        };

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f8";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             B, 
                             N) == 0);
        process(data, sizeof(data)/sizeof(*data), o);


      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTRING COPY RECORD FROM AND TO BLOB:
        //  
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   TESTRING COPY RECORD FROM AND TO BLOB:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTRING COPY RECORD FROM AND TO BLOB:" << endl
                          << "======================================" << endl;

        using namespace TestCase7;
        enum {
            B = 32,
            N = 10
        };

        Record R1 = {     1, 'a', B/2+1} ;
        Record R2 = {   B-1, 'b', B/2+1} ;
        Record R3 = {     B, 'c', B/2+1} ;
        Record R4 = {   B+1, 'd', B/2+1} ;
        Record R5 = { 2*B-1, 'e', B/2+1} ;
        Record R6 = {   2*B, 'f', B/2+1} ;


        Record R7 = {  10*B, 'g', 1} ;
        Record R8 = {  10*B, 'h', B/2} ;
        Record R9 = {  10*B, 'i', B/2+1} ;
        Record R10= {  10*B, 'j', B} ;
        Record R11= {  10*B, 'k', B+3*B/4} ;

        DATA data[] = {
            {
                L_,                                   // line
                { ADD, 0, R1 },                       // cmd
                { {R1, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 1,2,3,4,5,6,7,8,9,-1},              // expected free list
                0                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                0                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R2 },                       // cmd
                { {R2, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 1,2,3,4,5,6,7,8,9,-1},              // expected free list
                0                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                0                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R3 },                       // cmd
                { {R3, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 1,2,3,4,5,6,7,8,9,-1},              // expected free list
                0                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                0                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R4 },                       // cmd
                { {R4, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,4,5,6,7,8,9,-1},              // expected free list
                1                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                1                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R5 },                       // cmd
                { {R5, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,4,5,6,7,8,9,-1},              // expected free list
                1                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                1                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R6 },                       // cmd
                { {R6, 0}                  },         // expected unconf
                {                          },         // expected conf
                { 2,3,4,5,6,7,8,9,-1},              // expected free list
                1                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                1                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R7 },                       // cmd
                { {R7, 0}                  },         // expected unconf
                {                          },         // expected conf
                { -1},              // expected free list
                9                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                9                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R8 },                       // cmd
                { {R8, 0}                  },         // expected unconf
                {                          },         // expected conf
                { -1},              // expected free list
                9                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                9                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R9 },                       // cmd
                { {R9, 0}                  },         // expected unconf
                {                          },         // expected conf
                { -1},              // expected free list
                9                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                9                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R10 },                       // cmd
                { {R10, 0}                  },         // expected unconf
                {                          },         // expected conf
                { -1},              // expected free list
                9                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                9                                     // exp highestBlockUsed
            },

            {
                L_,                                   // line
                { ADD, 0, R11 },                       // cmd
                { {R11, 0}                  },         // expected unconf
                {                          },         // expected conf
                { -1},              // expected free list
                9                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM,0 },                            // cmd
                {                   },                // expected unconf
                {                           },        // expected conf
                { 0,1,2,3,4,5,6,7,8,9,-1},            // expected free list
                9                                     // exp highestBlockUsed
            },



        };
        

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        const char *FILE_NAME = "f7";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             B, 
                             N) == 0);
        process(data, sizeof(data)/sizeof(*data), o);


      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING LINK LIST FUNCTIONALITY:
        // 
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   TESTING LINK LIST FUNCTIONALITY:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING LINK LIST FUNCTIONALITY:" << endl
                          << "================================" << endl;

        using namespace TestCase6;
        enum {
            B = 1024,
            N = 3
        };

        if (veryVerbose) {
            P(B);
            P(N);
        }

        Record R1 = { B, 'a', B} ;
        Record R2 = { B, 'b', B} ;
        Record R3 = { B, 'c', B} ;
        Record R4 = { B*2, 'd', B} ;

        DATA data[] = {
            {
                L_,                                   // line
                { ADD, 0, R1 },                       // cmd
                { {R1, 0}                  },         // expected unconf
                {                           },         // expected conf
                { 1, 2, -1},                       // expected free list
                0                                     // exp highestBlockUsed
            },
            {
                L_,
                { ADD, 1, R2 },                       // cmd
                { {R1, 0}, {R2, 1}          },         // expected unconf
                {                           },         // expected conf
                { 2, -1},                       // expected free list
                1                                     // exp highestBlockUsed
            },            
            {
                L_,
                { ADD, 2, R3 },                       // cmd
                { {R1, 0}, {R2, 1}, {R3, 2} },         // expected unconf
                {                           },         // expected conf
                { -1},                       // expected free list
                2                                     // exp highestBlockUsed
            },



            {
                L_,
                { CNF, 1  },                           // cmd
                { {R1, 0}, {R3, 2}          },         // expected unconf
                { {R2, 1}                   },         // expected conf
                { -1},                                 // expected free list
                2                                      // exp highestBlockUsed
            },
            {
                L_,
                { CNF, 0  },                           // cmd
                { {R3, 2}                   },         // expected unconf
                { {R2, 1},  {R1, 0}         },         // expected conf
                { -1},                                 // expected free list
                2                                      // exp highestBlockUsed
            },
            {
                L_,
                { CNF, 2  },                           // cmd
                {                           },         // expected unconf
                { {R2, 1}, {R1, 0}, {R3, 2} },         // expected conf
                { -1},                                 // expected free list
                2                                      // exp highestBlockUsed
            },



            {
                L_,
                { REM, 0  },                           // cmd
                {                           },         // expected unconf
                { {R2, 1},          {R3, 2} },         // expected conf
                { 0, -1},                              // expected free list
                2                                      // exp highestBlockUsed
            },
            {
                L_,
                { REM, 2  },                           // cmd
                {                           },         // expected unconf
                { {R2, 1},                  },         // expected conf
                { 2, 0, -1},                           // expected free list
                2                                      // exp highestBlockUsed
            },
            {
                L_,
                { REM, 1  },                           // cmd
                {                           },         // expected unconf
                {                           },         // expected conf
                { 1, 2, 0, -1},                        // expected free list
                2                                      // exp highestBlockUsed
            },




            {
                L_,                                   // line
                { ADD, 1, R4 },                       // cmd
                { {R4, 1}                  },         // expected unconf
                {                           },         // expected conf
                { 0, -1},                       // expected free list
                2                                     // exp highestBlockUsed
            },
            {
                L_,
                { REM, 1  },                           // cmd
                {                           },         // expected unconf
                {                           },         // expected conf
                { 1, 2, 0, -1},                        // expected free list
                2                                      // exp highestBlockUsed
            },



            {
                L_,                                   // line
                { ADD, 1, R1 },                       // cmd
                { {R1, 1}                  },         // expected unconf
                {                           },         // expected conf
                { 2, 0, -1},                       // expected free list
                2                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { ADD, 2, R4 },                       // cmd
                { {R1, 1}, {R4, 2}                  },    // expected unconf
                {                           },         // expected conf
                { -1},                       // expected free list
                2                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 2},                       // cmd
                { {R1, 1}                  },         // expected unconf
                {                           },         // expected conf
                { 2, 0, -1},                       // expected free list
                2                                     // exp highestBlockUsed
            },
            {
                L_,                                   // line
                { REM, 1 },                       // cmd
                {                   },         // expected unconf
                {                           },         // expected conf
                { 1, 2, 0, -1},                       // expected free list
                2                                     // exp highestBlockUsed
            }


        };
        

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);


        const char *FILE_NAME = "f6";  rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             B, 
                             N) == 0);


        process(data, sizeof(data)/sizeof(*data), o);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MULTI-THREADED PERFORMANCE TESTING:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   MULTI-THREADED PERFORMANCE TESTING:
        // --------------------------------------------------------------------


        using namespace TestCase5;

        if (verbose) cout << endl
                          << "MULTI-THREADED PERFORMANCE TESTING:" << endl
                          << "===================================" << endl;
        

        
        H h, h1, h2, h3, h4, h5;
        
        int i;
        const char *FILE_NAME = "f5";




        if (argc > 4) NUM_THREADS = std::atoi(argv[4]);
        if (argc > 5) NUM_ITERATIONS = std::atoi(argv[5]);
        if (argc > 6) BLK_SIZE = std::atoi(argv[6]);
        if (argc > 7) NUM_BLKS_IN_A_PAGE = std::atoi(argv[7]);
        if (argc > 8) REC_SIZE = std::atoi(argv[8]);




        if (veryVerbose) {
            P(BLK_SIZE);
            P(NUM_BLKS_IN_A_PAGE);
            P(REC_SIZE);

            P(NUM_THREADS);
            P(NUM_ITERATIONS);

            cout << "Usage: cmd <case> 1 1 <NUM_THREADS> <NUM_ITERATIONS> <BLK_SIZE> <NUM_BLKS_IN_A_PAGE> <REC_SIZE>" << endl;
        }

        bcemt_Barrier localBarrier(NUM_THREADS);
        barrier = &localBarrier;
        if (verbose) {
            std::cout
                << "Create NUM_THREAD threads, each running a loop "
                << "of NUM_ITERATIONS doing 'addRecord', 'confirmRecord' "
                << "and than 'removeRecord'"
                << std::endl;
        }

        rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, Obj::READ_WRITE, true, 0, -1, -1, BLK_SIZE, NUM_BLKS_IN_A_PAGE) == 0);
        

        bcemt_ThreadUtil::setConcurrency(NUM_THREADS + 1);

        T t1 = BloombergLP::bdes_TimeUtil::getTimer();
        executeInParallel(NUM_THREADS, workerThread5);
        T t2 = BloombergLP::bdes_TimeUtil::getTimer();
        

        if (verbose) {
            printf("Time in nanoseconds:" ); P(t2-t1);
        }

        ASSERT(o.numConfirmedRecords()   == 0);
        ASSERT(o.numUnconfirmedRecords() == 0);
        JournalInfo ji;
        fillJournalInfo(&ji, &o);

      } break;
      case -3: {
        // --------------------------------------------------------------------
        // TESTING XYZ:
        //   Verifying xyz. 
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   void xyz();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING XYZ" << endl
                          << "===========" << endl;

        using namespace TestCase3;

        

        H h;
        int BLK_SIZE = std::atoi(argv[2]);
        int NUM_BLK_IN_PAGE = std::atoi(argv[3]);
        const char* file = argv[4];
        Obj::Mode mode = (std::atoi(argv[5]) == 0)?  Obj::READ_ONLY:  Obj::READ_WRITE;

        /*
        enum {
            BLK_SIZE = 100,
            NUM_BLK_IN_PAGE = 2
        };
        */

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);

        int rc = o.openJournal(file, mode, true, 0, -1, -1, BLK_SIZE, NUM_BLK_IN_PAGE);
        
        ASSERT(rc == 0); P(rc);

        int choice;

        do {
            menu();
            if (!(std::cin >> choice)) break;
            switch(choice) {
              case 1: {
                std::string s;
                float n; 
                std::cin >> s >> n; 
                int size = n * BLK_SIZE;
                ASSERT(size < sizeof(g_buf));
                fillBuf(g_buf, size, (s.c_str())[0]);
                ASSERT(0 == o.addRecord(&h, g_buf, n * BLK_SIZE));

                printf("recID = %d added", h);
                }break;
                
              case 2: {
                int recId; std::cin >> recId;
                ASSERT(0 == o.removeRecord(recId));
                puts("removed");
                }break;
                
              case 3: {
                int recId; std::cin >> recId;
                ASSERT(0 == o.confirmRecord(recId));
                puts("confirmed");

                }break;
                
              case 4:{
                  JournalInfo ji;
                  fillJournalInfo(&ji, &o);
                  print(ji);
                  //o.printHeader(std::cout);
                }break;
                
              default:
                P("BAD CHOICE");
            };
        } while (1);
            

        ASSERT(o.close() == 0);



      } break;

      case 4: {
        // --------------------------------------------------------------------
        // STRESS TEST:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   
        //
        // --------------------------------------------------------------------

/*
        using namespace TestCase4;

        
        H h, h1, h2, h3, h4, h5;
        const char *BUF  = "111";

        const char *BUF1 = "abc";
        const char *BUF2 = "def";
        const char *BUF3 = "ghi";
        const char *BUF4 = "jkl";
        const char *BUF5 = "mno";
        
        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        int i;
        const char *FILE_NAME = "f4";




        enum {
                      BLK_SIZE =   1024 * 1024, 
            NUM_BLKS_IN_A_PAGE =   1 
        };


        if (veryVerbose) {
            P(BLK_SIZE);
            P(NUM_BLKS_IN_A_PAGE);
            P(REC_SIZE);
        }



        {
        if (verbose) cout << endl
                          << "STRESS TEST:" << endl
                          << "==========="<< endl;
        
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
        Obj::READ_WRITE, 
        true, 
        0, 
        -1, 
        -1, 
        BLK_SIZE, 
        NUM_BLKS_IN_A_PAGE) == 0);        
        
        int rc = 0;
        while(!(rc = o.addRecord(&h, "a", 1))) {
            if (veryVerbose) {
                if (h % 100000 == 0)
                    P(h);
            }
        }
        
        if (veryVerbose) { P(rc); P(h); }
        
//         for (int i = h; i >=0; --i) 
//             ASSERT(o.confirmRecord(h) == 0); 
        
//         for (int i = h; i >=0; --i) 
//             ASSERT(o.removeRecord(h) == 0); 


        }
*/

      } break;
      
      case 3: {
        // --------------------------------------------------------------------
        // TESTING LINK LIST FUNCTIONS:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   TESTING LINK LIST FUNCTIONS:
        // --------------------------------------------------------------------
        using namespace TestCase3;

        
        H h, h1, h2, h3, h4, h5;
        const char *BUF  = "111";

        const char *BUF1 = "abc";
        const char *BUF2 = "def";
        const char *BUF3 = "ghi";
        const char *BUF4 = "jkl";
        const char *BUF5 = "mno";

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        int i;
        const char *FILE_NAME = "f3";




        enum {
                      BLK_SIZE =   1024, 
            NUM_BLKS_IN_A_PAGE =   1024, 
                      REC_SIZE =   BLK_SIZE + BLK_SIZE/2
        };


        if (veryVerbose) {
            P(BLK_SIZE);
            P(NUM_BLKS_IN_A_PAGE);
            P(REC_SIZE);
        }



        {
        if (verbose) cout << endl
                          << "TESTING LINK LIST FUNCTIONS:" << endl
                          << "============================"<< endl;


        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        rmFile(FILE_NAME);        
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             BLK_SIZE, 
                             NUM_BLKS_IN_A_PAGE) == 0);        

        ASSERT(o.addRecord(&h1, BUF1, strlen(BUF1)) == 0); 
        ASSERT(h1 == 0);

        ASSERT(o.addRecord(&h2, BUF2, strlen(BUF2)) == 0); 
        ASSERT(h2 == 1);

        ASSERT(o.addRecord(&h3, BUF3, strlen(BUF3)) == 0); 
        ASSERT(h3 == 2);

        ASSERT(o.removeRecord(h2) == 0); 
        ASSERT(o.removeRecord(h1) == 0); 
        ASSERT(o.removeRecord(h3) == 0); 

        ASSERT(o.addRecord(&h1, BUF1, strlen(BUF1)) == 0); 
        ASSERT(h1 == 2);

        ASSERT(o.addRecord(&h2, BUF2, strlen(BUF2)) == 0); 
        ASSERT(h2 == 0);

        ASSERT(o.addRecord(&h3, BUF3, strlen(BUF3)) == 0); 
        ASSERT(h3 == 1);
        }

        {
        if (verbose) cout << endl
                          << "TESTING LINK LIST FUNCTIONS:" << endl
                          << "============================"<< endl;


        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        rmFile(FILE_NAME);
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             BLK_SIZE, 
                             NUM_BLKS_IN_A_PAGE) == 0);        

        ASSERT(o.addRecord(&h1, BUF1, strlen(BUF1)) == 0); 
        ASSERT(h1 == 0);

        ASSERT(o.addRecord(&h2, BUF2, strlen(BUF2)) == 0); 
        ASSERT(h2 == 1);

        ASSERT(o.addRecord(&h3, BUF3, strlen(BUF3)) == 0); 
        ASSERT(h3 == 2);


        ASSERT(o.confirmRecord(h1) == 0); 
        ASSERT(o.confirmRecord(h2) == 0); 
        ASSERT(o.confirmRecord(h3) == 0); 

        ASSERT(o.removeRecord(h2) == 0); 
        ASSERT(o.removeRecord(h1) == 0); 
        ASSERT(o.removeRecord(h3) == 0); 

        ASSERT(o.addRecord(&h1, BUF1, strlen(BUF1)) == 0); 
        ASSERT(h1 == 2);

        ASSERT(o.addRecord(&h2, BUF2, strlen(BUF2)) == 0); 
        ASSERT(h2 == 0);

        ASSERT(o.addRecord(&h3, BUF3, strlen(BUF3)) == 0); 
        ASSERT(h3 == 1);
        }

      }break;
      case 2: {
        // --------------------------------------------------------------------
        // SINGLE THREADED PERFORMANCE TEST:
        //   
        //
        // Concerns:
        //   That 
        //
        // Plan:
        //   Create 
        //
        // Testing:
        //   SINGLE THREADED PERFORMANCE TEST:
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "SINGLE THREADED PERFORMANCE TEST:" << endl
                          << "================="<< endl;

        using namespace TestCase2;


        H h;

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        int i;
        const char *FILE_NAME = "f2";

        rmFile(FILE_NAME);


        enum {
                      DEFAULT_BLK_SIZE =   1024, 
            DEFAULT_NUM_BLKS_IN_A_PAGE =   1024, 
                      DEFAULT_REC_SIZE =   DEFAULT_BLK_SIZE,
                 DEFAULT_NUM_ITERATION =   10000,
    DEFAULT_MAX_CONFIRMED_RECORDS_KEPT = 5
        };




        int NUM_ITERATION = DEFAULT_NUM_ITERATION;
        int MAX_CONFIRMED_RECORDS_KEPT = DEFAULT_MAX_CONFIRMED_RECORDS_KEPT;
        int BLK_SIZE = DEFAULT_BLK_SIZE;
        int NUM_BLKS_IN_A_PAGE = DEFAULT_NUM_BLKS_IN_A_PAGE;
        int REC_SIZE = DEFAULT_REC_SIZE;

        if (argc > 4) NUM_ITERATION = std::atoi(argv[4]);
        if (argc > 5) MAX_CONFIRMED_RECORDS_KEPT = std::atoi(argv[5]);
        if (argc > 6) BLK_SIZE = std::atoi(argv[6]);
        if (argc > 7) NUM_BLKS_IN_A_PAGE = std::atoi(argv[7]);
        if (argc > 8) REC_SIZE = std::atoi(argv[8]);

        if (verbose) {
            P(BLK_SIZE);
            P(NUM_BLKS_IN_A_PAGE);
            P(REC_SIZE);
            P(NUM_ITERATION);
            P(MAX_CONFIRMED_RECORDS_KEPT);

            cout << "Usage: cmd <case> 1 1 <NUM_ITERATION> <MAX_CONFIRMED_RECORDS_KEPT> <BLK_SIZE> <NUM_BLKS_IN_A_PAGE> <REC_SIZE>" << endl;
        }


        
        ASSERT(sizeof(g_buf) > REC_SIZE);


        if (verbose) {
            std::cout 
                << "Add a record and than confirm it.  " 
                << "Repeat this NUM_ITERATION times" 
                << std::endl;
        }
        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             MAX_CONFIRMED_RECORDS_KEPT, 
                             -1, 
                             BLK_SIZE, 
                             NUM_BLKS_IN_A_PAGE) == 0);        



        T t1 = BloombergLP::bdes_TimeUtil::getTimer();
        for (int i = 0; i < NUM_ITERATION; ++i) {            

            // add
            fillBuf(g_buf, REC_SIZE, 'A');
            ASSERT(o.addRecord(&h, g_buf, REC_SIZE) == 0); 
            
            // confirm
            ASSERT(o.confirmRecord(h) == 0);
            
        }
        T t2 = BloombergLP::bdes_TimeUtil::getTimer();
        
        
        if (verbose) {
            printf("Time in nanoseconds:" ); P(t2-t1);
        }

        if(NUM_ITERATION > MAX_CONFIRMED_RECORDS_KEPT) {
            ASSERT(o.numConfirmedRecords()   == MAX_CONFIRMED_RECORDS_KEPT);
        }

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
        //
        // Testing:
        //   1. 
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        using namespace TestCase1;




        
        H h;

        bcema_TestAllocator ta(veryVeryVerbose);
        Obj o(Obj::IMPLICIT_COMMIT, &ta);
        int i;
        const char *FILE_NAME = "f1";
        enum {
            B        =     32,        // block size
            NUM_BLKS =     32,        // num blocks in a page
            P        =    B * NUM_BLKS  // page size
        };

        rmFile(FILE_NAME);

        ASSERT(o.openJournal(FILE_NAME, 
                             Obj::READ_WRITE, 
                             true, 
                             0, 
                             -1, 
                             -1, 
                             B, 
                             NUM_BLKS) == 0);

        std::vector<H> records;

        static const struct {
            int d_line;
            int d_size;
            int d_char;
        } DATA[] = {
            //line no.         size             char
            //-------   ----------------  --------------

            // small records, ~= 1 K
            { L_,                B,              'A' },
            { L_,          B + B/2,              'B' },
            { L_,            2 * B,              'C' },
            { L_,        2*B + B/2,              'd' },
            

            // mediam records, ~= 1 M
            { L_,                P,              'E' },
            { L_,          P + P/2,              'F' },
            { L_,            2 * P,              'G' },
            { L_,        2*P + P/2,              'H' },

            
            // big records, ~=  100 M
            { L_,          100 * P,              'I' },
            { L_,          100 * P,              'I' },
            { L_,          100 * P,              'I' }
            
        };
        
        
        H prevHandle = -1;
        const int NUM_VALUES = sizeof DATA / sizeof *DATA;

        ASSERT(o.numConfirmedRecords()   == 0);
        ASSERT(o.numUnconfirmedRecords() == 0);
        

        //----------------
        // add records
        //----------------
        for (int di = 0; di < NUM_VALUES; ++di) {
            const int  LINE  = DATA[di].d_line;
            const int  SIZE  = DATA[di].d_size;
            const char CHAR  = DATA[di].d_char;
            
            if (veryVerbose) { T_(); P_(LINE); P_(SIZE); P(CHAR);  }
            
            
            int blob = 1; if (argc >= 5) blob = 0;
            

            if (blob) {
                SimpleBlobBufferFactory blobFact((B*2));
                bcema_Blob blob(&blobFact);
                fillBlob(&blob, SIZE, CHAR);
                ASSERT(o.addRecord(&h, blob) == 0); 
            }
            else {
                ASSERT(sizeof(g_buf) > SIZE);
                fillBuf(g_buf, SIZE, CHAR);
                ASSERT(o.addRecord(&h, g_buf, SIZE) == 0); 
            }
            
            records.push_back(h);
            
            ASSERT(h > prevHandle); 
            prevHandle = h;
        }


        ASSERT(o.numConfirmedRecords()   == 0);
        ASSERT(o.numUnconfirmedRecords() == NUM_VALUES);


        //----------------
        // iterate unconfirmed
        //----------------
        i = 0; 
        for (It it = o.firstUnconfirmedRecord(); it; ++it, ++i) {
            // handle check 
            ASSERT(*it == records[i]);

            // size check
            ASSERT(it.recordLength() == DATA[i].d_size);

            // content check thru buf
            ASSERT(it.data(g_buf, it.recordLength()) == DATA[i].d_size);
            ASSERT(validateBuf(g_buf, DATA[i].d_size, DATA[i].d_char));

            // content check thru buf
            ASSERT(validateBlob(it.data(),it.recordLength(), DATA[i].d_char));
        }


        ASSERT(o.numConfirmedRecords()   == 0);
        ASSERT(o.numUnconfirmedRecords() == NUM_VALUES);


        //----------------
        // confirm records
        //----------------
        for (int i = 0; i < records.size(); ++i) {
            ASSERT(o.confirmRecord(records[i]) == 0);
        }


        ASSERT(o.numConfirmedRecords()   == NUM_VALUES);
        ASSERT(o.numUnconfirmedRecords() == 0);


        //----------------
        // iterate confirmed // almost repeat
        //----------------
        i = 0; 
        for (It it = o.firstConfirmedRecord(); it; ++it, ++i) {
            // handle check 
            ASSERT(*it == records[i]);

            // size check
            it.recordLength() == DATA[i].d_size;

            // content check thru buf
            ASSERT(it.data(g_buf, it.recordLength()) == DATA[i].d_size);
            ASSERT(validateBuf(g_buf, DATA[i].d_size, DATA[i].d_char));

            // content check thru buf
            ASSERT(validateBlob(it.data(),it.recordLength(), DATA[i].d_char));
        }


        ASSERT(o.numConfirmedRecords()   == NUM_VALUES);
        ASSERT(o.numUnconfirmedRecords() == 0);


        //----------------
        // remove records
        //----------------
        for (int i = 0; i < records.size(); ++i) {
            ASSERT(o.removeRecord(records[i]) == 0);
        }

        ASSERT(o.numConfirmedRecords()   == 0);
        ASSERT(o.numUnconfirmedRecords() == 0);



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
