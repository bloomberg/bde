// btemt_channelpoolchannel.t.cpp      -*-C++-*-

#include <btemt_channelpoolchannel.h>

#include <btemt_asyncchannel.h>
#include <btemt_channelpool.h>
#include <btemt_message.h>
#include <btemt_session.h>

#include <bcema_blobutil.h>
#include <bcema_testallocator.h>
#include <bcemt_thread.h>
#include <bcemt_barrier.h>

#include <bdef_function.h>
#include <bdef_memfn.h>

#include <bdex_bytestreamimputil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bteso_ipv4address.h>
#include <bteso_inetstreamsocketfactory.h>
#include <bteso_streamsocket.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>

#if defined(BDES_PLATFORMUTIL__NO_LONG_HEADER_NAMES)
#include <strstrea.h>
#else
#include <bsl_strstream.h>
#endif
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
// Create concrete implementations of the btemt_Session, and
// btemt_SessionFactory protocols and instantiate them.
//
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver simply verifies that a concrete instance of btemt_Session
// and btemt_SessionFactory can be implement and instantiated.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
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
#define NL() cout << endl;                    // End of line
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//          USAGE example from header(with assert replaced with ASSERT)
//-----------------------------------------------------------------------------
//

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef btemt_ChannelPoolChannel Obj;

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

static bcemt_Mutex coutMutex;

#define MTCOUT   { coutMutex.lock(); cout \
                                           << bcemt_ThreadUtil::selfIdAsInt() \
                                           << ": "
#define MTENDL   endl << bsl::flush ;  coutMutex.unlock(); }
#define MTFLUSH  bsl::flush; } coutMutex.unlock()

#define PT(X) { MTCOUT << #X " = " << (X) << MTENDL; }
#define PT2(X,Y) { MTCOUT << #X " = " << (X) << ", " \
                          << #Y " = " << (Y) << MTENDL; }
#define PT3(X,Y,Z) { MTCOUT << #X " = " << (X) << ", " \
                            << #Y " = " << (Y) << ", " \
                            << #Z " = " << (Z) << MTENDL; }
                                                 // Print identifier and value.

// The following macro facilitates outputting a vector of events.

// template <class TYPE>
// void printVector(const bsl::vector<TYPE>& vec, const char *name)
// {
//     cout << name << " = {";
//     copy(vec.begin(), vec.end(), ostream_iterator<TYPE>(cout, "\n\t"));
//     cout << "\n}" << endl;
// }

// #define PV(X) printVector( (X), #X);

#define MTASSERT(X) { \
  if (!(X)) { MTCOUT; aSsErT(1, #X, __LINE__); cout << MTFLUSH; } }

#define MTLOOP_ASSERT(I,X) { \
  if (!(X)) { MTCOUT; P(I); aSsErT(1, #X, __LINE__); cout << MTFLUSH; }}

#define MTLOOP2_ASSERT(I,J,X) { \
  if (!(X)) { MTCOUT; P_(I); P(J); aSsErT(1, #X, __LINE__); cout << MTFLUSH; }}

namespace CASE1 {

                    // ================
                    // class DataReader
                    // ================

class DataReader {
    // This class implements a data reader whose callback functions are
    // invoked to read the data from a btemt_ChannelPoolChannel object.

    int                 d_msgId;           // message id
    int                 d_msgLength;       // message length
    bsl::string         d_data;            // actual data
    mutable bcemt_Mutex d_mutex;           // mutex for data

  public:
    // CREATORS
    DataReader();
        // Construct a default reader.

    ~DataReader();
        // Destroy this reader.

    // MANIPULATORS
    void blobBasedReadCb(int         state,
                         int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId);
        // Blob based read callback.

    void pbcBasedReadCb(int                   state,
                        int                  *numConsumed,
                        int                  *needed,
                        const btemt_DataMsg&  msg);
        // Pooled Buffer Chain based read callback.

    // ACCESSORS
    const bsl::string& data() const;
        // Return the data stored in this reader.

    int msgId() const;
        // Return the message id of the data in this reader.

    int msgLength() const;
        // Return the message length of data expected by this reader.

    bool done() const;
        // Return 'true' if this reader has read the expected number of
        // data bytes, and 'false' otherwise.
};

                    // ===============
                    // class my_Server
                    // ===============

class my_Server {
    // This class implements a multi-user multi-threaded server.

    // DATA
    typedef bsl::map<int, Obj *>    ChannelMap;           // Channel Map
                                                          // typedef

    btemt_ChannelPoolConfiguration  d_config;             // pool
                                                          // configuration

    bcemt_Mutex                     d_mapMutex;           // map lock

    mutable bcemt_Mutex             d_idxMutex;           // idx lock

    ChannelMap                      d_channelMap;         // channel map

    bsl::vector<int>                d_channelIdxMap;      // channel idx
                                                          // map

    btemt_ChannelPool              *d_channelPool_p;      // channel pool

    int                             d_portNumber;         // port on which
                                                          // this server
                                                          // is listening

    bool                            d_useBlobForDataReads;
                                                          // use blob based
                                                          // reads

    bcema_PooledBufferChainFactory  d_bufferChainFactory; // buffer chain
                                                          // factory

    bcema_PooledBlobBufferFactory   d_blobBufferFactory;  // blob buffer
                                                          // factory

    bcema_PoolAllocator             d_spAllocator;        // smart pointers
                                                          // allocators

    bslma_Allocator                *d_allocator_p;        // memory
                                                          // allocator
                                                          // (held)

    // PRIVATE MANIPULATORS
    void channelStateCb(int   channelId,
                        int   sourceId,
                        int   state,
                        void *userData);
        // Wrapper for channel pool's channel callback state callback.

    void pbcBasedReadCb(int                  *numConsumed,
                        int                  *numNeeded,
                        const btemt_DataMsg&  msg,
                        void                 *userData);
        // Channel pool's pooled buffer chain based read callback.

    void blobBasedReadCb(int        *numNeeded,
                         bcema_Blob *msg,
                         int         channelId,
                         void       *userData);
        // Channel pool's blob based read callback.

    void poolStateCb(int reason, int source, int severity);
        // Indicates the status of the whole pool.

  private:
    // NOT IMPLEMENTED
    my_Server(const my_Server& original);
    my_Server& operator=(const my_Server& rhs);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(my_Server,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    my_Server(const btemt_ChannelPoolConfiguration&  config,
              bool                                   useBlobForDataReads,
              bslma_Allocator                       *basicAllocator = 0);
        // Construct this server configured by the specified 'config' and
        // if 'useBlobForDataReads' is 'true' use 'bcema_Blob' for data
        // reads, and if it is 'false' then use 'bcema_PooledBufferChain'
        // based data reads.  Optionally specify 'basicAllocator' used to
        // allocate memory.

    ~my_Server();
        // Destroy this server.

    // MANIPULATORS
    int portNumber() const;
        // Return the actual port number on which this server is
        // listening.

    Obj *channel(int index);
        // Return the channel corresponding to the specified 'index'.
};

                    // =====================
                    // class CallbackDetails
                    // =====================

struct CallbackDetails {
    // This 'struct' represents a POD type specifying the details of the
    // read callback.

    int  d_numBytes;    // number of bytes to be read
    bool d_blobBased;   // use the blob based read callback
};

                    // ==============
                    // class TestData
                    // ==============

struct TestData {
    // This 'struct' represents the data passed the testing thread.

    int                    d_threadId;        // thread id
    DataReader             d_reader;          // data reader
    my_Server             *d_server_p;        // server to read from
    const CallbackDetails *d_callbacks;       // callbacks
    int                    d_numCbs;          // num callbacks

    void run();
        // Run the test function.
};

                        // ----------------
                        // class DataReader
                        // ----------------

// CREATORS
DataReader::DataReader()
: d_msgId(-1)
, d_msgLength(-1)
{
}

DataReader::~DataReader()
{
}

// MANIPULATORS
void DataReader::blobBasedReadCb(int         state,
                                 int        *numNeeded,
                                 bcema_Blob *msg,
                                 int         channelId)
{
    if (veryVerbose) {
        MTCOUT << "Blob based callback called with "
               << "for Channel Id: " << channelId
               << " with state: " << state << MTENDL;
    }

    if (btemt_AsyncChannel::BTEMT_SUCCESS != state) {
        return;                                                   // RETURN
    }

    if (veryVerbose) {
        MTCOUT << "Blob based callback called with "
               << "for Channel Id: " << channelId
               << " with data of length: " << msg->length()
               << MTENDL;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (-1 == d_msgId) {
        if (msg->length() < sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }

        char *dataPtr = msg->buffer(0).data();
        if (msg->buffer(0).size() < sizeof(int)) {
            char tmp[sizeof(int)];
            const int numBytes = msg->buffer(0).size();
            memcpy(tmp, msg->buffer(0).data(), numBytes);
            ASSERT(msg->buffer(1).size() <= sizeof(int) - numBytes);
            memcpy(tmp + numBytes,
                   msg->buffer(1).data(),
                   sizeof(int) - numBytes);
            dataPtr = tmp;
        }

        bdex_ByteStreamImpUtil::getInt32(&d_msgId, dataPtr);
        ASSERT(0 <= d_msgId);
        bcema_BlobUtil::erase(msg, 0, sizeof(int));
        if (0 == msg->length()) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }
    }

    if (-1 == d_msgLength) {
        if (msg->length() < sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                                   // RETURN
        }

        char *dataPtr = msg->buffer(0).data();
        if (msg->buffer(0).size() < sizeof(int)) {
            char tmp[sizeof(int)];
            const int numBytes = msg->buffer(0).size();
            memcpy(tmp, msg->buffer(0).data(), numBytes);
            ASSERT(msg->buffer(1).size() >= sizeof(int) - numBytes);
            memcpy(tmp + numBytes,
                   msg->buffer(1).data(),
                   sizeof(int) - numBytes);
            dataPtr = tmp;
        }

        bdex_ByteStreamImpUtil::getInt32(&d_msgLength, dataPtr);
        ASSERT(0 <= d_msgLength);
        bcema_BlobUtil::erase(msg, 0, sizeof(int));

        if (0 == msg->length()) {
            *numNeeded = d_msgLength;
            return;                                                   // RETURN
        }
    }

    bsl::string msgData;

    const int numDataBufs = msg->numDataBuffers();
    for (int i = 0; i < numDataBufs - 1; ++i) {
        msgData.append(msg->buffer(i).data(), msg->buffer(i).size());
    }
    msgData.append(msg->buffer(numDataBufs - 1).data(),
                   msg->lastDataBufferLength());

    bcema_BlobUtil::erase(msg, 0, msg->length());
    d_data.append(msgData);
    const int numRemaining = d_msgLength - d_data.size();
    *numNeeded = numRemaining <= 0 ? 0 : numRemaining;
}

void DataReader::pbcBasedReadCb(int                   state,
                                int                  *numConsumed,
                                int                  *numNeeded,
                                const btemt_DataMsg&  msg)
{
    if (veryVerbose) {
        MTCOUT << "PBC based callback called with "
               << " state: " << state << MTENDL;
    }

    if (btemt_AsyncChannel::BTEMT_SUCCESS != state) {
        return;                                                   // RETURN
    }

    if (veryVerbose) {
        MTCOUT << "PBC based callback called with "
               << "for Channel Id: " << msg.channelId()
               << " with data of length: " << msg.data()->length()
               << MTENDL;
    }

    bcema_PooledBufferChain *chain = msg.data();
    ASSERT(chain);

    const int bufSize    = chain->bufferSize();
    const int numBuffers = (chain->length() / bufSize) + 1;
    int       bufOffset  = 0;
    int       bufIdx     = 0;
    *numConsumed         = 0;

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    if (-1 == d_msgId) {
        if (chain->length() < sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                               // RETURN
        }
        bdex_ByteStreamImpUtil::getInt32(&d_msgId, chain->buffer(bufIdx));
        ASSERT(0 <= d_msgId);

        bufOffset += sizeof(int);
        *numConsumed += sizeof(int);
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }

        if (*numConsumed == chain->length()) {
            *numNeeded = sizeof(int);
            return;                                               // RETURN
        }
    }

    if (-1 == d_msgLength) {
        if (chain->length() < *numConsumed + sizeof(int)) {
            *numNeeded = sizeof(int);
            return;                                               // RETURN
        }
        ASSERT(bufSize >= bufOffset + sizeof(int));

        bdex_ByteStreamImpUtil::getInt32(&d_msgLength,
                                        chain->buffer(bufIdx) + bufOffset);
        ASSERT(0 <= d_msgLength);
        bufOffset += sizeof(int);

        *numConsumed += sizeof(int);
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }

        if (*numConsumed == chain->length()) {
            *numNeeded = d_msgLength;
            return;                                               // RETURN
        }
    }

    bsl::string msgData;
    if (bufOffset >= bufSize) {
        ++bufIdx;
        bufOffset = 0;
    }

    int remaining = chain->length() - *numConsumed;
    if (d_msgLength < remaining) {
        remaining = d_msgLength;
    }

    while (remaining) {
        int currSize = remaining > bufSize - bufOffset
                     ? bufSize - bufOffset : remaining;
        msgData.append(chain->buffer(bufIdx) + bufOffset, currSize);
        remaining -= currSize;
        bufOffset += currSize;
        *numConsumed += currSize;
        if (bufOffset >= bufSize) {
            ++bufIdx;
            bufOffset = 0;
        }
    }

    d_data.append(msgData);

    const int numRemaining = d_msgLength - d_data.size();
    *numNeeded = numRemaining <= 0 ? 0 : 1;
}

// ACCESSORS
const bsl::string& DataReader::data() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_data;
}

int DataReader::msgId() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_msgId;
}

int DataReader::msgLength() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_msgLength;
}

bool DataReader::done() const
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);
    return d_data.size() == d_msgLength;
}

                        // ---------------
                        // class my_Server
                        // ---------------

// PRIVATE MANIPULATORS
void my_Server::channelStateCb(int   channelId,
                               int   sourceId,
                               int   state,
                               void *userData)
{
    if (veryVerbose) {
        MTCOUT << "Channel state callback called with"
               << " Channel Id: " << channelId
               << " Source Id: "  << sourceId
               << " State: " << state << MTENDL;
    }

    bcemt_LockGuard<bcemt_Mutex> guard(&d_mapMutex);
    ChannelMap::iterator iter = d_channelMap.find(channelId);

    switch (state) {
      case btemt_ChannelPool::BTEMT_CHANNEL_DOWN: {
        if (iter == d_channelMap.end()) {
            if (veryVerbose) {
                MTCOUT << "Channel Id: " << channelId
                       << " not found" << MTENDL;
            }
            return;                                               // RETURN
        }
      } break;

      case btemt_ChannelPool::BTEMT_CHANNEL_UP: {
        if (iter != d_channelMap.end()) {
            if (veryVerbose) {
                MTCOUT << "Channel Id: " << channelId
                       << " already exists" << MTENDL;
            }
            d_channelPool_p->shutdown(channelId,
                                      btemt_ChannelPool::BTEMT_IMMEDIATE);
            return;                                               // RETURN
        }

        Obj *channel;
        if (d_useBlobForDataReads) {
            channel = new (*d_allocator_p)
                             btemt_ChannelPoolChannel(channelId,
                                                      d_channelPool_p,
                                                      NULL,
                                                      &d_blobBufferFactory,
                                                      &d_spAllocator,
                                                      d_allocator_p,
                                                      d_useBlobForDataReads);
        }
        else {
            channel = new (*d_allocator_p)
                            btemt_ChannelPoolChannel(channelId,
                                                     d_channelPool_p,
                                                     &d_bufferChainFactory,
                                                     NULL,
                                                     &d_spAllocator,
                                                     d_allocator_p,
                                                     d_useBlobForDataReads);
        }

        d_channelMap[channelId] = channel;
        d_idxMutex.lock();
        d_channelIdxMap.push_back(channelId);
        d_idxMutex.unlock();
      } break;
      default: {
      } break;
    }
}

void my_Server::poolStateCb(int reason,
                            int source,
                            int severity)
{
    if (veryVerbose) {
        MTCOUT << "Pool state changed: (" << reason << ", " << source
               << ") " << MTENDL;
    }
}

void my_Server::pbcBasedReadCb(int                  *numConsumed,
                               int                  *numNeeded,
                               const btemt_DataMsg&  msg,
                               void                 *userData)
{
    int channelId = msg.channelId();
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mapMutex);
    ChannelMap::iterator iter = d_channelMap.find(channelId);
    if (iter == d_channelMap.end()) {
        if (veryVerbose) {
            MTCOUT << "Channel Id: " << channelId
                   << " not found" << MTENDL;
        }
        return;                                                   // RETURN
    }
    Obj *channel = iter->second;
    guard.release()->unlock();
    channel->dataCb(numConsumed, numNeeded, msg);
}

void my_Server::blobBasedReadCb(int        *numNeeded,
                                bcema_Blob *msg,
                                int         channelId,
                                void       *userData)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mapMutex);
    ChannelMap::iterator iter = d_channelMap.find(channelId);
    if (iter == d_channelMap.end()) {
        if (veryVerbose) {
            MTCOUT << "Channel Id: " << channelId
                   << " not found" << MTENDL;
        }
        return;                                                   // RETURN
    }

    Obj *channel = iter->second;
    guard.release()->unlock();
    channel->blobBasedDataCb(numNeeded, msg);
}

// CREATORS
my_Server::my_Server(
                const btemt_ChannelPoolConfiguration&  config,
                bool                                   useBlobForDataReads,
                bslma_Allocator                       *basicAllocator)
: d_config(config)
, d_channelMap(basicAllocator)
, d_channelPool_p(0)
, d_portNumber(0)
, d_useBlobForDataReads(useBlobForDataReads)
, d_bufferChainFactory(config.maxIncomingMessageSize(), basicAllocator)
, d_blobBufferFactory(config.maxIncomingMessageSize(), basicAllocator)
, d_spAllocator(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    btemt_ChannelPool::ChannelStateChangeCallback channelStateFunctor(
            bdef_MemFnUtil::memFn(&my_Server::channelStateCb, this)
          , d_allocator_p);

    btemt_ChannelPool::PoolStateChangeCallback poolStateFunctor(
            bdef_MemFnUtil::memFn(&my_Server::poolStateCb, this)
          , d_allocator_p);

    if (d_useBlobForDataReads) {
        btemt_ChannelPool::BlobBasedReadCallback dataFunctor =
            bdef_MemFnUtil::memFn(&my_Server::blobBasedReadCb,
                                  this);

        d_channelPool_p = new (*d_allocator_p)
                                     btemt_ChannelPool(channelStateFunctor,
                                                       dataFunctor,
                                                       poolStateFunctor,
                                                       d_config,
                                                       d_allocator_p);
    }
    else {
        bdef_Function<void (*)(int *, int*, const btemt_DataMsg&, void*)>
            dataFunctor(
                    bdef_MemFnUtil::memFn(&my_Server::pbcBasedReadCb, this)
                  , d_allocator_p);

        d_channelPool_p = new (*d_allocator_p)
                                     btemt_ChannelPool(channelStateFunctor,
                                                       dataFunctor,
                                                       poolStateFunctor,
                                                       d_config,
                                                       d_allocator_p);
    }

    d_channelPool_p->start();

    bteso_IPv4Address endpoint;
    endpoint.setPortNumber(d_portNumber);
    d_channelPool_p->listen(endpoint, 5, 1);
    const bteso_IPv4Address *address = d_channelPool_p->serverAddress(1);
    if (address) {
        d_portNumber = address->portNumber();
    }
}

my_Server::~my_Server()
{
    for (ChannelMap::iterator iter = d_channelMap.begin();
         iter != d_channelMap.end(); ++iter) {
        d_allocator_p->deleteObject(iter->second);
    }
    d_allocator_p->deleteObject(d_channelPool_p);
}

// ACCESSORS
Obj *my_Server::channel(int index)
{
    bcemt_LockGuard<bcemt_Mutex> guard1(&d_idxMutex);
    if (index >= d_channelIdxMap.size()) {
        return 0;                                                 // RETURN
    }
    int channelId = d_channelIdxMap[index];
    guard1.release()->unlock();

    bcemt_LockGuard<bcemt_Mutex> guard2(&d_mapMutex);
    ChannelMap::iterator iter = d_channelMap.find(channelId);
    if (iter != d_channelMap.end()) {
        return iter->second;
    }
    return 0;
}

int my_Server::portNumber() const
{
    return d_portNumber;
}

extern "C" void* threadFunction(void *testData)
{
    ((TestData *) testData)->run();
    return 0;
}

                        // --------------
                        // class TestData
                        // --------------

void TestData::run()
{
    Obj *channel = 0;
    while (!channel) {
        channel = d_server_p->channel(d_threadId);
        bcemt_ThreadUtil::yield();
    }

    btemt_AsyncChannel::BlobBasedReadCallback blobBasedCb =
                        bdef_MemFnUtil::memFn(&DataReader::blobBasedReadCb,
                                              &d_reader);
    btemt_AsyncChannel::ReadCallback pbcBasedCb =
                         bdef_MemFnUtil::memFn(&DataReader::pbcBasedReadCb,
                                               &d_reader);

    for (int i = 0; i < d_numCbs; ++i) {
        const CallbackDetails cbDetails =
                                      ((CallbackDetails *) d_callbacks)[i];
        const int numBytes  = cbDetails.d_numBytes;
        const bool blobBased= cbDetails.d_blobBased;

        if (blobBased) {
            channel->read(numBytes, blobBasedCb);
        }
        else {
            channel->read(numBytes, pbcBasedCb);
        }
    }

    while (!d_reader.done()) {
        bcemt_ThreadUtil::yield();
    }
}

}  // close namespace CASE1

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

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //   The usage example from the header has been incorporated into this
        //   test driver.  All references to 'assert' have been replaced with
        //   'ASSERT'.  Call the test example function and assert that it works
        //   as expected.
        //
        // Plan:
        // Testing:
        // --------------------------------------------------------------------

      }break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        using namespace CASE1;

        const int MAX_NUM_CBS = 100;
        struct {
            const int             d_line;
            const bsl::string     d_expData;
            const int             d_numCbs;
            const CallbackDetails d_cbDetails[MAX_NUM_CBS];
        } DATA[] = {
            {
                L_,
                "Hello World!",
                7,
                {
                    { 4, true },
                    { 4, true },
                    { 1, true },
                    { 2, true },
                    { 3, true },
                    { 4, true },
                    { 5, true },
                },
            },
            {
                L_,
                "Cruel Life",
                7,
                {
                    { 4, false },
                    { 4, false },
                    { 1, false },
                    { 2, false },
                    { 3, false },
                    { 4, false },
                    { 5, false },
                }
            },
            {
                L_,
                "Testing this component requires some code",
                7,
                {
                    { 4, true  },
                    { 4, false },
                    { 1, true  },
                    { 2, false },
                    { 3, true  },
                    { 4, false },
                    { 5, true  },
                }
            },
            {
                L_,
                "Another short message",
                7,
                {
                    { 4, false },
                    { 4, true  },
                    { 1, false },
                    { 2, true  },
                    { 3, false },
                    { 4, true  },
                    { 5, false },
                }
            },
            {
                L_,
                "A long message that should invoke quite a few callbacks and "
                "give us some confidence that this test case works.",
                17,
                {
                    {  4, true },
                    {  4, true },
                    {  1, true },
                    {  2, true },
                    {  3, true },
                    {  4, true },
                    {  5, true },
                    {  6, true },
                    {  7, true },
                    {  8, true },
                    {  9, true },
                    { 10, true },
                    { 11, true },
                    { 12, true },
                    { 13, true },
                    { 14, true },
                    { 15, true },
                }
            },
            {
                L_,
                "A second long message that should invoke quite a few "
                "callbacks and give us some confidence that this works.",
                17,
                {
                    {  4, false },
                    {  4, false },
                    {  1, false },
                    {  2, false },
                    {  3, false },
                    {  4, false },
                    {  5, false },
                    {  6, false },
                    {  7, false },
                    {  8, false },
                    {  9, false },
                    { 10, false },
                    { 11, false },
                    { 12, false },
                    { 13, false },
                    { 14, false },
                    { 15, false },
                }
            },
            {
                L_,
                "To be or not to be that is the question."
                "To be or not to be that is the question."
                "To be or not to be that is the question.",
                17,
                {
                    {  4, true  },
                    {  4, false },
                    {  1, true  },
                    {  2, false },
                    {  3, true  },
                    {  4, false },
                    {  5, true  },
                    {  6, false },
                    {  7, true  },
                    {  8, false },
                    {  9, true  },
                    { 10, false },
                    { 11, true  },
                    { 12, false },
                    { 13, true  },
                    { 14, false },
                    { 15, true  },
                }
            },
            {
                L_,
                "A long message that should invoke quite a few callbacks and "
                "give us some confidence that this test case works as we want",
                17,
                {
                    {  4, false },
                    {  4, true  },
                    {  1, false },
                    {  2, true  },
                    {  3, false },
                    {  4, true  },
                    {  5, false },
                    {  6, true  },
                    {  7, false },
                    {  8, true  },
                    {  9, false },
                    { 10, true  },
                    { 11, false },
                    { 12, true  },
                    { 13, false },
                    { 14, true  },
                    { 15, false },
                }
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        btemt_ChannelPoolConfiguration config;
        config.setMaxThreads(NUM_DATA);           // I/O threads
        config.setReadTimeout(1.0);               // in seconds
        config.setMetricsInterval(10.0);          // seconds
        config.setIncomingMessageSizes(sizeof(int), 10, 1024);

        typedef bteso_InetStreamSocketFactory<bteso_IPv4Address> IPv4Factory;
        typedef bteso_StreamSocket<bteso_IPv4Address>            Socket;

        for (int k = 0; k < 2; ++k) {
            bcema_TestAllocator     ta(veryVeryVerbose);
            my_Server               server(config, (bool) k, &ta);
            const bteso_IPv4Address ADDRESS("127.0.0.1", server.portNumber());

            IPv4Factory              factory(&ta);
            bsl::vector<TestData>    tests(NUM_DATA);
            bcemt_ThreadUtil::Handle handles[NUM_DATA];
            bsl::vector<bsl::string> expData(NUM_DATA);
            bsl::vector<Socket *>    sockets(NUM_DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const bsl::string EXP_DATA = DATA[i].d_expData;
                const int         LEN      = EXP_DATA.size();
                const int         NUM_CBS  = DATA[i].d_numCbs;
                TestData&         test     = tests[i];

                Socket *socket   = factory.allocate();
                test.d_threadId  = i;
                test.d_server_p  = &server;
                test.d_numCbs    = NUM_CBS;
                test.d_callbacks = DATA[i].d_cbDetails;
                bcemt_ThreadUtil::create(&handles[i],
                                         threadFunction,
                                         &test);

                sockets[i] = socket;
                expData[i] = EXP_DATA;
                ASSERT(0 == socket->connect(ADDRESS));
            }

            const int MAX_DATA_SIZE = 1024;
            for (int i = 0; i < NUM_DATA; ++i) {
                Socket             *socket    = sockets[i];
                const bsl::string&  writeData = expData[i];
                const int           DATA_SIZE = writeData.size();

                char data[MAX_DATA_SIZE];
                bsl::memset(data, 0, MAX_DATA_SIZE);
                ASSERT(DATA_SIZE <= MAX_DATA_SIZE);

                int offset = 0;
                bdex_ByteStreamImpUtil::putInt32(data, i);
                offset += sizeof(int);

                bdex_ByteStreamImpUtil::putInt32(data + offset, DATA_SIZE);
                offset += sizeof(int);

                const int TOTAL_SIZE = DATA_SIZE + offset;

                bsl::memcpy(data + offset, writeData.data(), DATA_SIZE);

                offset = 0;
                int incr = 1, remaining = TOTAL_SIZE;
                while (remaining > 0) {
                    ASSERT(incr == socket->write(data + offset, incr));
                    remaining -= incr;
                    offset += incr;
                    if (remaining <= incr) {
                        incr = remaining;
                    }
                    else {
                        ++incr;
                    }
                    bcemt_ThreadUtil::microSleep(100);
                }
            }

            for (int i = 0; i < NUM_DATA; ++i) {
                ASSERT(0 == bcemt_ThreadUtil::join(handles[i]));
            }

            for (int i = 0; i < NUM_DATA; ++i) {
                const DataReader& reader = tests[i].d_reader;
                LOOP_ASSERT(i, reader.data() == expData[reader.msgId()]);
                factory.deallocate(sockets[i], true);
            }
        }
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
