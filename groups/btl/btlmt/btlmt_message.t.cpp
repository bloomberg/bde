// btlmt_message.t.cpp                                                -*-C++-*-

#include <btlmt_message.h>

#include <bdlcc_queue.h>
#include <bslma_testallocator.h>                // for testing only
#include <bdlma_concurrentpool.h>
//#include <bdlma_defaultdeleter.h>
#include <bdlmca_blob.h>
#include <bdlmca_blobutil.h>
#include <bdlmca_pooledblobbufferfactory.h>
#include <bdlmca_xxxpooledbufferchain.h>
#include <bdlmt_threadpool.h>
#include <bdlmtt_xxxatomictypes.h>

#include <bdlf_function.h>
#include <bdlf_bind.h>

#include <bdlsb_fixedmemoutput.h>
#include <bdlxxxx_genericbyteoutstream.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>

#include <bsl_c_stdlib.h>     // atoi()

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//#############################################################################
//#*** TBD: This test driver is not sufficient - we must run               ***#
//#*** btlmt_channelpool's test driver to validate this component.  This   ***#
//#*** test driver should be made self-sufficient.                         ***#
//#############################################################################

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//    [TBD - Overview of the test]
//-----------------------------------------------------------------------------
// [XX] [TBD - Method name]
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static
void processMessages(bdlcc::Queue<btlmt::Message> *queue,
                     bdlmtt::AtomicInt *count)
{
    ASSERT(queue);
    btlmt::Message msg = queue->popFront();
    btlmt::DataMsg& dataMsg = msg.dataMsg();
    ASSERT(dataMsg.userDataField1() < dataMsg.userDataField2());
    ++(*count);
}

static
void processBlobMessages(bdlcc::Queue<btlmt::Message> *queue,
                         bdlmtt::AtomicInt *count)
{
    ASSERT(queue);
    btlmt::Message msg = queue->popFront();
    btlmt::BlobMsg& blobMsg = msg.blobMsg();
    ASSERT(blobMsg.userDataField1() < blobMsg.userDataField2());
    ++(*count);
}

                        // --------------------
                        // class DefaultDeleter
                        // --------------------

template <class TYPE>
class DefaultDeleter : public bdlma::Deleter<TYPE> {
    bslma::Allocator *d_allocator_p;  // memory allocator (held, *not* owned)

  public:
    DefaultDeleter(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {}

    virtual void deleteObject(TYPE *instance)
    {
        d_allocator_p->deleteObject(instance);
    }
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
namespace BTEMT_MESSAGE_USAGE_EXAMPLE {

///Usage Example
///-------------
void usageExample()
{
///Usage Example
///-------------
// It is possible to initialize a 'btlmt::Message' object to a value of one of
// the specific-message types defined in this component (e.g., 'btlmt::DataMsg',
// 'btlmt::ChannelMsg').  (See the Usage Example of 'btlmt_channelqueuepool' for
// additional examples.)  First, create a channel message object
// 'channelMessage' initialized with arbitrary data:
//..
    btlmt::ChannelMsg::ChannelState state   =
                                            btlmt::ChannelMsg::BTEMT_CHANNEL_UP;
    int                            chanId  = 123;
    int                            allocId = 456;

    btlmt::ChannelMsg channelMessage(state, chanId, allocId);
//..
// We can now initialize a newly created object 'message' of type
// 'btlmt::Message' with the value of 'channelMessage', and use 'message' to
// discover the attributes of 'channelMessage'.
//..
    btlmt::Message message1 = channelMessage; // calls initializing constructor

    ASSERT(btlmt::ChannelMsg::BTEMT_CHANNEL_UP
               == message1.channelMsg().channelState());
    ASSERT(123 == message1.channelMsg().channelId());
    ASSERT(456 == message1.channelMsg().allocatorId());
//..
//
// Another way is to use the 'btlmt::Message' constructor taking a
// 'btlmt::Message::MessageType' argument to construct a message representing
// the indicated type and then use that message, e.g., via a specific-message
// reference, to manipulate the "contained" specific message.  For example,
// using the previous definitions:
//..
    btlmt::Message message2(btlmt::Message::BTEMT_CHANNEL_STATE);
    btlmt::ChannelMsg &channelMsg = message2.channelMsg();

    channelMsg.setChannelState(state);
    channelMsg.setChannelId(chanId);
    channelMsg.setAllocatorId(allocId);
//..
//
// Note that the channel messages created in either way should have the same
// type and value:
//..
    ASSERT(btlmt::Message::BTEMT_CHANNEL_STATE == message1.type());
    ASSERT(btlmt::Message::BTEMT_CHANNEL_STATE == message2.type());
    ASSERT(message1.channelMsg() == message2.channelMsg());
//..
// The second way presented above is slightly more error-prone in case the
// 'message2' is initialized with the wrong message type.  The user should make
// sure that specific-message references are always used in conjunction with a
// valid 'btlmt::Message' of the same type.
}

} // close namespace BTEMT_MESSAGE_USAGE_EXAMPLE

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING btlmt::MessageUtil::assignData
        // Testing:
        //   btlmt::MessageUtil::assignData(bdlmca::Blob*, const btlmt::DataMsg&,
        //                                 int)
        //   btlmt::MessageUtil::assignData(btlmt::DataMsg*, const bdlmca::Blob&,
        //                                 int,
        //                                 bdlmca::PooledBufferChainFactory*,
        //                                 bslma::Allocator*);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING btlmt::MessageUtil::assignData"
                          << "\n=============" << endl;

        enum {
            NUM_SIZES = 7,
            DATA_SIZE = 81799
        };
        int bufSizes[NUM_SIZES] = {5, 17, 64, 239, 8192, 81799, 131072};

        for(int i = 0; i< NUM_SIZES; ++i) {
            int blobBufSize = bufSizes[i];
            bdlmca::PooledBlobBufferFactory blobFactory(blobBufSize);
            for(int j=0; j < NUM_SIZES; ++j) {
                int chainBufSize = bufSizes[j];
                if (veryVerbose) {
                    bsl::cout << "Testing blob buffer size = " << blobBufSize
                              << ", chain buffer size = " << chainBufSize
                              << bsl::endl;
                }
                bdlmca::PooledBufferChainFactory chainFactory(chainBufSize);

                char origData[DATA_SIZE];
                for(int p=0; p<DATA_SIZE; ++p) {
                    origData[p] = rand() & 0xFF;
                }

                bdlmca::Blob blob(&blobFactory);
                bdlmca::BlobUtil::append(&blob, origData, DATA_SIZE);
                ASSERT(blob.length() == DATA_SIZE);

                btlmt::DataMsg dataMsg;
                btlmt::MessageUtil::assignData(
                    &dataMsg, blob, DATA_SIZE, &chainFactory,
                    bslma::Default::allocator());
                ASSERT(dataMsg.data()->length() == blob.length());
                char dataCopy[DATA_SIZE];
                dataMsg.data()->copyOut(dataCopy, DATA_SIZE, 0);
                ASSERT(0 == memcmp(origData, dataCopy, DATA_SIZE));
                memset(dataCopy, 0, DATA_SIZE);

                blob.removeAll();
                btlmt::MessageUtil::assignData(&blob, dataMsg, DATA_SIZE);
                ASSERT(blob.length() == DATA_SIZE);

                // TBD: don't we have a simple utility method somewhere?
                {
                bdlsb::FixedMemOutput osb(dataCopy, DATA_SIZE);
                bdlxxxx::GenericByteOutStream<bdlsb::FixedMemOutput> bos(&osb);
                bdlmca::BlobUtil::write(bos, blob, 0, DATA_SIZE);
                }
                ASSERT(0 == memcmp(origData, dataCopy, DATA_SIZE));
                memset(dataCopy, 0, DATA_SIZE);
            }
        }
      } break;
      case 8: {
        ;
        // --------------------------------------------------------------------
        // TESTING btlmt::Message ON USER MESSAGES
        // Testing:
        //   btlmt::Message(const btlmt::UserMsg&)
        //   btlmt::UserMsg& userMsg();
        //   const btlmt::UserMsg& userMsg() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        using namespace BTEMT_MESSAGE_USAGE_EXAMPLE;

        usageExample();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING btlmt::Message ON USER MESSAGES
        // Testing:
        //   btlmt::Message(const btlmt::UserMsg&)
        //   btlmt::UserMsg& userMsg();
        //   const btlmt::UserMsg& userMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::Message ON USER MESSAGES" << endl
            << "======================================" << endl;
        struct {
            int   d_srcIsManaged;
            int   d_srcType;
            int   d_srcIntData;
            void *d_srcVoidPtrData;
        } DATA[] =
        {
            { 0, 2, -1,  (void*)0xBEAD },
            { 0, 1, -2,  (void*)0xDEAD },
            { 1, 2, -1,  (void*)0xBEAD },
            { 1, 0, -1,  (void*)0xBEAD },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        enum { BUFFER_LENGTH = 64 };

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVerbose) P(i);
            bslma::TestAllocator localAllocator(veryVeryVerbose);
            bdlmca::PooledBufferChainFactory factory(BUFFER_LENGTH,
                                                   &localAllocator);

            btlmt::UserMsg src;
            src.setType(DATA[i].d_srcType);
            src.setIntData(DATA[i].d_srcIntData);

            if (DATA[i].d_srcIsManaged) {
                bdlmca::PooledBufferChain *chain =
                    factory.allocate(0);
                src.setManagedData(chain, &factory, &localAllocator);
            }
            else {
                src.setVoidPtrData(DATA[i].d_srcVoidPtrData);
            }

            btlmt::Message mX(src);
            const btlmt::Message& X = mX;

            btlmt::Message mY(mX);

            btlmt::UserMsg &srcMX = mX.userMsg();
            const btlmt::UserMsg &srcX = X.userMsg();

            LOOP_ASSERT(i, srcMX.type() == DATA[i].d_srcType);
            LOOP_ASSERT(i, srcMX.intData() == DATA[i].d_srcIntData);
            LOOP_ASSERT(i, srcMX.isManaged() == DATA[i].d_srcIsManaged);
            if (srcMX.isManaged()) {
                LOOP_ASSERT(i, srcMX.managedData() == src.managedData());
            }
            else {
                LOOP_ASSERT(i, srcMX.voidPtrData()==DATA[i].d_srcVoidPtrData);
            }

            LOOP_ASSERT(i, srcX.type() == DATA[i].d_srcType);
            LOOP_ASSERT(i, srcX.intData() == DATA[i].d_srcIntData);
            LOOP_ASSERT(i, srcX.isManaged() == DATA[i].d_srcIsManaged);
            if (!srcX.isManaged()) {
                LOOP_ASSERT(i, srcX.voidPtrData() == DATA[i].d_srcVoidPtrData);

            }
            if (srcX.isManaged()) {
                LOOP_ASSERT(i, srcX.managedData() == src.managedData());
            }
            if (veryVerbose)
                Q("NEXT");
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING btlmt::Message ON BLOB MESSAGES
        // Plan:
        //   We will consider a simple producer-consumer test in which
        //   the main thread will produce items, in form of data messages,
        //   and an auxiliary threads, managed by the pool will consume them.
        // Testing:
        //   btlmt::Message(const btlmt::DataMsg&)
        //   btlmt::DataMsg& dataMsg();
        //   const btlmt::UserMsg& dataMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::Message ON BLOB MESSAGES" << endl
            << "======================================" << endl;

        bslma::TestAllocator testAllocator;
        enum { CHUNK_SIZE = 32 };
        bdlmca::PooledBlobBufferFactory factory(CHUNK_SIZE, &testAllocator);

        enum {
            NUM_MESSAGES     = 100,
            NUM_SUB_MESSAGES = 30
        };

        bcemt_Attribute defaultAttributes;
        bdlmt::ThreadPool pool(defaultAttributes, 1, 2, 100);
        bdlcc::Queue<btlmt::Message> queue;
        pool.start();
        bdlmtt::AtomicInt count = 0;
        bdlf::Function<void (*)()> functor(
                bdlf::BindUtil::bind(&processMessages, &queue, &count));
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            int length = NUM_SUB_MESSAGES * CHUNK_SIZE + 1;
            bsl::shared_ptr<bdlmca::Blob> blob;
            blob.createInplace(&testAllocator, &factory, &testAllocator);
            blob->setLength(length);

            btlmt::BlobMsg msg(blob, i);
            for (int j = 0; j < NUM_SUB_MESSAGES; ++j) {
                msg.setUserDataField1(j);
                msg.setUserDataField2(j + CHUNK_SIZE - 1);
                btlmt::Message subMsg(msg);
                queue.pushBack(subMsg);
                pool.enqueueJob(functor);
            }
        }
        pool.drain();
        LOOP_ASSERT(count, count == NUM_MESSAGES * NUM_SUB_MESSAGES);
        if (verbose) {
            P_(count); P(NUM_MESSAGES * NUM_SUB_MESSAGES);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING btlmt::Message ON DATA MESSAGES
        // Plan:
        //   We will consider a simple producer-consumer test in which
        //   the main thread will produce items, in form of data messages,
        //   and an auxiliary threads, managed by the pool will consume them.
        // Testing:
        //   btlmt::Message(const btlmt::DataMsg&)
        //   btlmt::DataMsg& dataMsg();
        //   const btlmt::UserMsg& dataMsg() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::Message ON DATA MESSAGES" << endl
            << "======================================" << endl;

        bslma::TestAllocator testAllocator;
        enum { CHUNK_SIZE = 32 };
        bdlmca::PooledBufferChainFactory factory(CHUNK_SIZE, &testAllocator);

        enum {
            NUM_MESSAGES     = 100,
            NUM_SUB_MESSAGES = 30
        };

        bcemt_Attribute defaultAttributes;
        bdlmt::ThreadPool pool(defaultAttributes, 1, 2, 100);
        bdlcc::Queue<btlmt::Message> queue;
        pool.start();
        bdlmtt::AtomicInt count = 0;
        bdlf::Function<void (*)()>functor(
                bdlf::BindUtil::bind(&processMessages, &queue, &count));
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            int length = NUM_SUB_MESSAGES * CHUNK_SIZE + 1;
            bdlmca::PooledBufferChain *chain = factory.allocate(length);
            btlmt::DataMsg msg(chain, &factory, i, &testAllocator);

            for (int j = 0; j < NUM_SUB_MESSAGES; ++j) {
                msg.setUserDataField1(j);
                msg.setUserDataField2(j + chain->bufferSize() - 1);
                btlmt::Message subMsg(msg);
                queue.pushBack(subMsg);
                pool.enqueueJob(functor);
            }
        }
        pool.drain();
        LOOP_ASSERT(count, count == NUM_MESSAGES * NUM_SUB_MESSAGES);
        if (verbose) {
            P_(count); P(NUM_MESSAGES * NUM_SUB_MESSAGES);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING btlmt::UserMsg ASSIGNMENT OPERATOR
        //  Verify that 'btlmt::UserMsg''s assignment operator by creating
        //  two objects in different states, assigning one to another and
        //  verifying state.  Also verify that aliasing is handled properly.
        //
        // Testing:
        //  btlmt::UserMsg& operator=(const btlmt::UserMsg& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::UserMsg ASSIGNMENT OPERATOR" << endl
            << "=========================================" << endl;
        struct {
            int   d_srcIsManaged;
            int   d_srcType;
            int   d_srcIntData;
            void *d_srcVoidPtrData;
            int   d_dstIsManaged;
            int   d_dstType;
            int   d_dstIntData;
            void *d_dstVoidPtrData;
        } DATA[] =
        {
            { 0, 1, -1,  (void*)0xBEAD, 0, 2, -3, (void*)0xDEAD   },
            { 0, 1, -1,  (void*)0xBEAD, 1, 2, -3, (void*)0xDEAD   },
            { 1, 1, -1,  (void*)0xBEAD, 0, 2, -3, (void*)0xDEAD   },
            { 1, 1, -1,  (void*)0xBEAD, 1, 2, -3, (void*)0xDEAD   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        enum { BUFFER_LENGTH = 64 };

        for (int i = 0; i < NUM_DATA; ++i) {
            bslma::TestAllocator localAllocator(veryVeryVerbose);
            bdlma::ConcurrentPool myPool(BUFFER_LENGTH, &localAllocator);
            DefaultDeleter<bdlmca::PooledBufferChain>
                deleter(&localAllocator);

            btlmt::UserMsg src;
            src.setType(DATA[i].d_srcType);
            src.setIntData(DATA[i].d_srcIntData);
            if (DATA[i].d_srcIsManaged) {
                bdlmca::PooledBufferChain *chain =
                    new (localAllocator) bdlmca::PooledBufferChain(&myPool);

                src.setManagedData(chain, &deleter, &localAllocator);
            }
            else {
                src.setVoidPtrData(DATA[i].d_srcVoidPtrData);
            }

            btlmt::UserMsg dst;
            dst.setType(DATA[i].d_dstType);
            dst.setIntData(DATA[i].d_dstIntData);
            if (DATA[i].d_dstIsManaged) {
                bdlmca::PooledBufferChain *chain =
                    new (localAllocator) bdlmca::PooledBufferChain(&myPool);

                dst.setManagedData(chain, &deleter, &localAllocator);
            }
            else {
                dst.setVoidPtrData(DATA[i].d_dstVoidPtrData);
            }

            dst = src;
            LOOP_ASSERT(i, dst.type() == DATA[i].d_srcType);
            LOOP_ASSERT(i, dst.intData() == DATA[i].d_srcIntData);
            LOOP_ASSERT(i, dst.isManaged() == DATA[i].d_srcIsManaged);
            if (src.isManaged()) {
                LOOP_ASSERT(i, dst.managedData() == src.managedData());
            }
            else {
                LOOP_ASSERT(i, dst.voidPtrData() == DATA[i].d_srcVoidPtrData);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING btlmt::UserMsg COPY CONSTRUCTOR
        //  Verify that 'btlmt::UserMsg''s copy constructor functions properly.
        //  Particularly, copy construct from both opaque and managed messages.
        //  and verify state.
        //
        // Testing:
        //  btlmt::UserMsg(const btlmt::UserMsg& original);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::UserMsg COPY CONSTRUCTOR" << endl
            << "======================================" << endl;
        {
            btlmt::UserMsg mX;
            btlmt::UserMsg mY(mX);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());
        }
        enum {
            TEST_TYPE = 5,
            TEST_INT_DATA = 6
        };
        void *TEST_VOID_DATA = (void*) 0xAB;

        {
            btlmt::UserMsg mX; mX.setType(TEST_TYPE);
            btlmt::UserMsg mY(mX);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(TEST_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());
        }
        {
            btlmt::UserMsg mX; mX.setIntData(TEST_INT_DATA);
            btlmt::UserMsg mY(mX);

            ASSERT(TEST_INT_DATA == mY.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mY.voidPtrData());

        }
        {
            btlmt::UserMsg mX; mX.setVoidPtrData(TEST_VOID_DATA);
            btlmt::UserMsg mY(mX);

            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(0 == mY.isManaged());
            ASSERT(TEST_VOID_DATA == mY.voidPtrData());
        }
        {
            bslma::TestAllocator   localTestAllocator(veryVeryVerbose);
            enum { NUM_BYTES = 64 }; // some number
            bdlma::ConcurrentPool myPool(NUM_BYTES, &localTestAllocator);
            DefaultDeleter<bdlmca::PooledBufferChain>
                deleter(&localTestAllocator);

            bdlmca::PooledBufferChain *chain =
                    new (localTestAllocator) bdlmca::PooledBufferChain(&myPool);

            btlmt::UserMsg mX; mX.setManagedData(chain, &deleter,
                                                &localTestAllocator);
            btlmt::UserMsg mY(mX);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mY.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mY.type());
            ASSERT(1 == mY.isManaged());
            ASSERT(chain == mY.managedData().get());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING btlmt::UserMsg
        //  Verify that 'btlmt::UserMsg' class functions correctly.
        //
        // Plan:
        //  Exercise methods one at a time; pay extra attention to
        // managed property and managed data members.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "TESTING btlmt::UserMsg: Basic Manipulators" << endl
            << "=========================================" << endl;
        {
            btlmt::UserMsg mX;
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());

        }
        enum {
            TEST_TYPE = 5,
            TEST_INT_DATA = 6
        };
        void *TEST_VOID_DATA = (void*) 0xAB;

        {
            btlmt::UserMsg mX; mX.setType(TEST_TYPE);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(TEST_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());
        }
        {
            btlmt::UserMsg mX; mX.setIntData(TEST_INT_DATA);
            ASSERT(TEST_INT_DATA == mX.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT((void*)btlmt::UserMsg::BTEMT_INVALID_VOID_DATA ==
                   mX.voidPtrData());

        }
        {
            btlmt::UserMsg mX; mX.setVoidPtrData(TEST_VOID_DATA);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(0 == mX.isManaged());
            ASSERT(TEST_VOID_DATA == mX.voidPtrData());
        }
        {
            bslma::TestAllocator   localTestAllocator(veryVeryVerbose);
            enum { NUM_BYTES = 64 }; // some number
            bdlma::ConcurrentPool myPool(NUM_BYTES, &localTestAllocator);
            DefaultDeleter<bdlmca::PooledBufferChain>
                deleter(&localTestAllocator);

            bdlmca::PooledBufferChain *chain =
                    new (localTestAllocator) bdlmca::PooledBufferChain(&myPool);

            btlmt::UserMsg mX; mX.setManagedData(chain, &deleter,
                                                &localTestAllocator);
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_INT_DATA == mX.intData());
            ASSERT(btlmt::UserMsg::BTEMT_INVALID_MESSAGE_TYPE == mX.type());
            ASSERT(1 == mX.isManaged());
            ASSERT(chain == mX.managedData().get());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        // [TBD - Description]
        //
        // Plan:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        {
            btlmt::ChannelMsg mX; const btlmt::ChannelMsg& X = mX;
            if (veryVerbose) P(mX);
            ASSERT(btlmt::ChannelMsg::BTEMT_INVALID_ID == X.channelId());
            ASSERT(btlmt::ChannelMsg::BTEMT_INVALID_ID == X.allocatorId());
            ASSERT(btlmt::ChannelMsg::BTEMT_CHANNEL_DOWN == X.event());
        }
        {
            btlmt::PoolMsg mX; const btlmt::PoolMsg& X = mX;
            if (veryVerbose) P(mX);
            ASSERT(btlmt::PoolMsg::BTEMT_INVALID_ID == X.sourceId());
            ASSERT(btlmt::PoolMsg::BTEMT_ACCEPT_TIMEOUT == X.event());
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
